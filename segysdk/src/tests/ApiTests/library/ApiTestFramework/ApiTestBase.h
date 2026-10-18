/*
Copyright 2023 Schlumberger

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once

#include "../common/TypeChecks.h"
#include "ApiEnvironment.h"
#include "ApiTestConfig.h"
#include "ApiTestException.h"
#include "ApiTestUtilities.h"
#include "CleanupFiles.h"

#include <Utilities/JsonUtilities.h>

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

namespace segysdk::api::tests
{
using namespace segysdk::utilities;

class ApiTestBase : public ::testing::TestWithParam<ApiTestConfig>
{
public:
    template<typename T>
    T getParameter(const std::string& name) const
    {
        return GetParam().getParameter<T>(name);
    }

    template<typename T>
    std::optional<T> getOptionalParameter(const std::string& name)
    {
        return GetParam().getOptionalParameter<T>(name);
    }

    std::filesystem::path getTestdataPathParameter(const std::string& name);
    std::filesystem::path getCurrentPathParameter(const std::string& name);

protected:
    virtual void SetUp() override;
    virtual void TearDown() override;

    void addToCleanup(std::filesystem::path filePath);

    template<typename TestClass, typename ReturnType>
    void check(ReturnType (TestClass::*func)(), TestClass* obj)
    {
        static_assert(std::is_member_function_pointer_v<std::decay_t<ReturnType (TestClass::*)()>>, "Expecting member function");

        const auto& params = GetParam();

        if (params.getType() == ResultType::R_EXCEPTION) {
            checkException(func, obj);
            return;
        }

        if constexpr (std::is_same_v<ReturnType, void>) {
            switch (params.getType()) {
            case ResultType::R_SUCCESS:
                EXPECT_NO_THROW(std::invoke(func, obj));
                break;
            case ResultType::R_FILE:
                EXPECT_NO_THROW(std::invoke(func, obj));
                if (ApiEnvironment::instance().dumpBaselines()) {
                    copyExpectedFileAsBaselineDump(params.getBaselineFile());
                }
                EXPECT_TRUE(compareFiles(params.getBaselineFile(), params.getExpectedFile()));
                break;
            case ResultType::R_JSON_FILE:
                {
                    EXPECT_NO_THROW(std::invoke(func, obj));
                    checkJson(params.getExpectedFile());
                    break;
                }
            default:
                FAIL() << "Test does not return expected result";
            }
        }
        else {
            // TODO: default constructor and assignment operator is used.
            auto result = ReturnType{};
            EXPECT_NO_THROW(result = std::invoke(func, obj));
            if constexpr (std::is_pointer_v<ReturnType> || is_unique_ptr<ReturnType>::value || is_shared_ptr<ReturnType>::value) {
                EXPECT_TRUE(result != ReturnType(nullptr));
            }
            else if constexpr (std::is_enum_v<ReturnType>) {
                checkValue(GetParam().getValue(), static_cast<std::underlying_type_t<ReturnType>>(result));
            }
            else if constexpr (std::is_same_v<ReturnType, std::string> || std::is_same_v<ReturnType, char*>) {
                if (params.getType() == ResultType::R_TEXT) {
                    auto baselinePath = params.getBaselineTextFilePath();
                    if (ApiEnvironment::instance().dumpBaselines()) {
                        dumpTextFile(addDumpSuffix(baselinePath), result);
                    }

                    std::string expectedText = readTextFile(baselinePath);

                    EXPECT_TRUE(result == expectedText); // Using EXPECT_TRUE here to avoid huge output
                }
                else if (params.getType() == ResultType::R_JSON) {
                    Json::Value json = JsonUtilities::getJsonFromString(result);

                    checkJson(json);
                }
                else {
                    checkValue(GetParam().getValue(), result);
                }
            }
            else if constexpr (is_std_vector<ReturnType>::value) {
                auto baselinePath = params.getBaselineFile();
                // dump vector to the file and compare with expected
                // This way no need to deserialize from a file into vector
                auto tmpDump = addDumpSuffix(baselinePath);
                dumpFile(tmpDump, reinterpret_cast<char*>(result.data()), sizeof(typename ReturnType::value_type) * result.size());
                if (not ApiEnvironment::instance().dumpBaselines()) {
                    addToCleanup(tmpDump);
                }

                EXPECT_TRUE(compareFiles(params.getBaselineFile(), tmpDump));
            }
            else if constexpr (std::is_same_v<ReturnType, Json::Value>) {
                checkJson(result);
            }
            else {
                checkValue(GetParam().getValue(), result);
            }
        }
    }

private:
    void copyExpectedFileAsBaselineDump(const std::filesystem::path& baselinePath);

    void checkJson(const Json::Value& result);
    void checkJson(const std::filesystem::path& jsonFilePath);

    template<typename TestClass, typename ReturnType>
    void checkException(ReturnType (TestClass::*func)(), TestClass* obj)
    {
        // GTest could only check type of the exception or whether the function throws
        // Check whether the exception message is the expected one
        try {
            std::invoke(func, obj);
            FAIL() << "Expected exception: " << GetParam().getExceptionMsg();
        }
        catch (const std::exception& ex) {
            std::string expected = GetParam().getExceptionMsg();
            expected = updateDatasetPathInString(expected, getTestdataPathParameter("dataset").parent_path());
            EXPECT_EQ(std::string(ex.what()), expected);
        }
    }

    template<typename ReturnType>
    void checkValue(const Json::Value& jsonValue, const ReturnType& result)
    {
        // JsonCpp reads integer values as signed unless larger than std::numeric_limits<int64>.
        // When comparing Json::Value with unsigned integer, a temporary Json::Value object is created with unsigned integer type.
        // As resulting two Json::Value-s contain a different type, comparison fails.
        // Thus, when API returns an unsigned integer value, a special handling is required
        // TODO: Explicit cast to Int64/UInt64 types to avoid type resolution issues on Ubuntu 20.04 (Focal). Review later
        if constexpr (std::is_integral_v<ReturnType> && std::is_unsigned_v<ReturnType> && !std::is_same_v<ReturnType, bool>) {
            if (result < static_cast<ReturnType>(std::numeric_limits<Json::Value::Int64>::max())) {
                EXPECT_EQ(jsonValue, static_cast<Json::Value::Int64>(result));
                return;
            }

            EXPECT_EQ(jsonValue, static_cast<Json::Value::UInt64>(result));
        }
        else {
            EXPECT_EQ(jsonValue, result);
        }
    }

private:
    CleanupFiles cleanup_;
};
} // namespace segysdk::api::tests