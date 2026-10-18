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

#include "../Logger/Logger.h"

#include "../BasicFileAuthPlugin/AuthPluginBasicFileImpl.h"
#include "../BasicFileAuthPlugin/BasicFileEnvironment.h"

#include "../BasicFileAuthPlugin/BasicFilePluginException.h"
#include "../FileIO/FileIOException.h"
#include "../FileIO/GenericFileManager.h"
#include "../Utilities/StringUtils.h"

#include <chrono>
#include <exception>
#include <filesystem>

namespace segysdk::auth
{

AuthPluginBasicFileImpl::AuthPluginBasicFileImpl(std::string const& TokenValue)
  : AuthPluginInterface(0U),
    tokenValue_(TokenValue)
{
    SEGYLOG_DEBUG("AuthPluginBasicFileImpl c-tor");

    tokenFile_ = BasicFileEnvironment::instance().getSegySDKTokenFile();
    if (tokenFile_.empty()) {
        std::string error = "authPluginBasicFile; environment not set: Variable not initialized: [" + BasicFileVariableStringConsts::SEGYSDK_BASIC_FILE_TOKEN_PATH + "]";
        SEGYLOG_REPORT_ERROR(error);
        throw BasicFileAuthPluginException(error);
    }

    if (fileio::GenericFileManager::isSDDataset(tokenFile_)) {
        const std::string msg = "authPluginBasicFile; TokenFile must be a local file!";
        SEGYLOG_REPORT_ERROR(msg);
        throw fileio::FileIOException(msg);
    }

    if (!std::filesystem::exists(tokenFile_)) {
        SEGYLOG_REPORT_ERROR("authPluginBasicFile; Variable: [" + BasicFileVariableStringConsts::SEGYSDK_BASIC_FILE_TOKEN_PATH + "] is not pointing to a valid file.");
        throw BasicFileAuthPluginException("BasicFileAuthPlugin environment not valid.");
    }
}

AuthPluginBasicFileImpl::~AuthPluginBasicFileImpl()
{
    /* EMPTY */
}

int64_t
AuthPluginBasicFileImpl::GetAuthProviderName(std::string& AuthProviderName)
{
    AuthProviderName = "BasicFileAuth";
    return 0;
}

int64_t
AuthPluginBasicFileImpl::GetToken(std::string& Token)
{
    try {
        if (tokenFile_.empty()) {
            throw BasicFileAuthPluginException("[" + BasicFileVariableStringConsts::SEGYSDK_BASIC_FILE_TOKEN_PATH + "] cannot be empty.");
        }

        const fileio::FileReference fr(tokenFile_, fileio::FileReaderOptions::localOnly());
        auto [success, tokenValue] = fileio::GenericFileManager::readTextFile(fileio::FileManagerParams{fr});
        if (success && !tokenValue.empty()) {
            // replace any CR or LF characters
            utilities::StringUtils::ReplaceAll(tokenValue, "\n", "");
            utilities::StringUtils::ReplaceAll(tokenValue, "\r", "");
            // Trim of leading or trailing spaces
            utilities::StringUtils::Trim(tokenValue);
        }
        Token = skipBearerPrefix(tokenValue);
        return static_cast<int64_t>(ErrorCode::Success);
    }
    catch (BasicFileAuthPluginException& e) {
        const std::string what(e.what());
        SEGYLOG_REPORT_ERROR("BasicFileAuthPluginException exception:\n" + what);

        return -static_cast<int64_t>(ErrorCode::UnableToReadTokenFile);
    }
    catch (std::exception& e) {
        const std::string what(e.what());
        SEGYLOG_REPORT_ERROR("Exception:\n" + what);

        return -static_cast<int64_t>(ErrorCode::UnrecoverableError);
    }
    catch (...) {
        SEGYLOG_REPORT_ERROR("Unexpected exception:\n");

        return -static_cast<int64_t>(ErrorCode::UnrecoverableError);
    }
}

int64_t
AuthPluginBasicFileImpl::IsTokenValid()
{
    return static_cast<int64_t>(ErrorCode::Success);
}

int64_t
AuthPluginBasicFileImpl::GetTimeToExpiry()
{
    return static_cast<int64_t>(BasicFileEnvironment::instance().getAuthenticationSafetyThreshold());
}

int64_t
AuthPluginBasicFileImpl::CanRefreshToken()
{
    return -static_cast<int64_t>(ErrorCode::BasicFileTokenFileIsGiven);
}

std::string
AuthPluginBasicFileImpl::GetErrorDescription(int64_t ErrorCode)
{
    auto const positiveErrorCode = static_cast<size_t>(-ErrorCode);

    if (positiveErrorCode < static_cast<int64_t>(ErrorCode::ERROR_CODES_COUNT)) {
        return {errorMessages_[positiveErrorCode]};
    }

    return {};
}

std::string
AuthPluginBasicFileImpl::skipBearerPrefix(const std::string& auth)
{
    if (utilities::StringUtils::keyBeginsWith(auth, bearer_prefix_)) {
        std::string result = auth;
        utilities::StringUtils::ReplaceAll(result, bearer_prefix_, "");
        return result;
    }

    return auth;
}

} // namespace segysdk::auth
