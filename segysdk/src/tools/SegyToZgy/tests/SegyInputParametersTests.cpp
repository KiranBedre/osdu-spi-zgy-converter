/*
Copyright 2021-2022 Schlumberger

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

#include "../Utilities/BrickSize.h"
#include "../Utilities/Environment.h"
#include "../tests/UtilityLibrary/AutoCleanupTestFile.h"
#include "../tools/SegyToZgy/library/SegyInputParameters.h"
#include "../segysdk/SegySessionFactory.h"

#include <gtest/gtest.h>

#include <cmath>
#include <cstring>

using namespace segysdk;

TEST(SegyInputParametersTests, test_exception_if_no_index)
{
    AutoCleanupTestFile temp_file("tiny.sgy");
    auto segy_file_name = temp_file.get_path();
    auto segy_config_json_string = temp_file.get_segy_config_json_string();

    auto segy_session = api::SegySessionFactory({segy_file_name}, segy_config_json_string).CreateSegySession();

    auto should_generate_index = segysdk::utilities::Environment::instance().shouldGenerateIndex();

    if (!should_generate_index) {
        EXPECT_THROW(SegyInputParameters sut(*segy_session), std::invalid_argument);
    }
    else {
        EXPECT_NO_THROW(SegyInputParameters sut(*segy_session));
    }
}

TEST(SegyInputParametersTests, test_known_values_for_sample_file)
{
    const AutoCleanupTestFile temp_file("tiny.sgy");
    temp_file.create_index();
    const auto segy_session = api::SegySessionFactory({temp_file.get_path()}, temp_file.get_segy_config_json_string()).CreateSegySession();
    const SegyInputParameters sut(*segy_session);

    EXPECT_EQ(sut.getInlineStart(), 3990);
    EXPECT_EQ(sut.getInlineIncrement(), 256);
    EXPECT_EQ(sut.getInlineCount(), 2);
    EXPECT_EQ(sut.getCrosslineStart(), 3850);
    EXPECT_EQ(sut.getCrosslineIncrement(), 256);
    EXPECT_EQ(sut.getCrosslineCount(), 20);
    EXPECT_EQ(sut.getVerticalStart(), 0);
    EXPECT_EQ(sut.getVerticalIncrement(), 32);
    EXPECT_EQ(sut.getVerticalCount(), 256);

    // TODO: not implemented in SegyIndex yet  EXPECT_EQ(sut.get_data_range_high(), 3267.6353);
    // TODO: not implemented in SegyIndex yet  EXPECT_EQ(sut.get_data_range_low(), -2146.5356);
    EXPECT_EQ(sut.getSampleFormat(), segysdk::segyio::SampleFormat::IeeeFloat4);
    EXPECT_EQ(sut.getVerticalUnitDimension(), OpenZGY::UnitDimension::time);
    EXPECT_EQ(sut.getVerticalUnitName(), "ms");
    EXPECT_EQ(sut.getVerticalUnitValue(), .001);
}

TEST(SegyInputParametersTests, test_known_values_for_sample_file_with_vertical_offset)
{
    const AutoCleanupTestFile temp_file("tiny_starting_at_4160ms.sgy");
    temp_file.create_index();
    const auto segy_session = api::SegySessionFactory({temp_file.get_path()}, temp_file.get_segy_config_json_string()).CreateSegySession();
    const SegyInputParameters sut(*segy_session);

    EXPECT_EQ(sut.getInlineStart(), 3990);
    EXPECT_EQ(sut.getInlineIncrement(), 256);
    EXPECT_EQ(sut.getInlineCount(), 2);
    EXPECT_EQ(sut.getCrosslineStart(), 3850);
    EXPECT_EQ(sut.getCrosslineIncrement(), 256);
    EXPECT_EQ(sut.getCrosslineCount(), 20);
    EXPECT_EQ(sut.getVerticalStart(), 4160);
    EXPECT_EQ(sut.getVerticalIncrement(), 32);
    EXPECT_EQ(sut.getVerticalCount(), 126);

    // EXPECT_EQ(sut.get_data_range_high(), 3267.6353 /* not implemented in SegyIndex yet */);
    // EXPECT_EQ(sut.get_data_range_low(), -2146.5356 /* not implemented in SegyIndex yet */);
    EXPECT_EQ(sut.getSampleFormat(), segysdk::segyio::SampleFormat::IeeeFloat4);
    EXPECT_EQ(sut.getVerticalUnitDimension(), OpenZGY::UnitDimension::time);
    EXPECT_EQ(sut.getVerticalUnitName(), "ms");
    EXPECT_EQ(sut.getVerticalUnitValue(), .001);
}

static segyindex::SeismicCubeExtents
create_geometry3d(segyio::VerticalDomain verticalDomain, segyio::VerticalUOM verticalUoM)
{
    segysdk::segyindex::Geometry3D g;
    g.JVectorExtent = {1, 2, 1};
    g.IVectorExtent = {1, 2, 1};
    g.sampleExtent = {1, 2, 1, verticalDomain, verticalUoM};

    return segyindex::SeismicCubeExtents(g.IVectorExtent, g.JVectorExtent, g.sampleExtent);
}

TEST(SegyInputParametersTests, test_domain_depth_unit_meter)
{
    const auto g = create_geometry3d(segyio::VerticalDomain::Depth, segyio::VerticalUOM::meter);

    const auto sut = SegyInputGeometry(g);
    EXPECT_EQ(sut.getVerticalUnitDimension(), OpenZGY::UnitDimension::length);
    EXPECT_EQ(sut.getVerticalUnitName(), "m");
    EXPECT_EQ(sut.getVerticalUnitValue(), 1.0);
}

TEST(SegyInputParametersTests, test_domain_depth_unit_foot)
{
    const auto g = create_geometry3d(segyio::VerticalDomain::Depth, segyio::VerticalUOM::feet);

    const auto sut = SegyInputGeometry(g);
    EXPECT_EQ(sut.getVerticalUnitDimension(), OpenZGY::UnitDimension::length);
    EXPECT_EQ(sut.getVerticalUnitName(), "ft");
    EXPECT_EQ(sut.getVerticalUnitValue(), 0.3048);
}

TEST(SegyInputParametersTests, test_domain_time_unit_second)
{
    const auto g = create_geometry3d(segyio::VerticalDomain::Time, segyio::VerticalUOM::seconds);

    const auto sut = SegyInputGeometry(g);
    EXPECT_EQ(sut.getVerticalUnitDimension(), OpenZGY::UnitDimension::time);
    EXPECT_EQ(sut.getVerticalUnitName(), "ms");
    EXPECT_EQ(sut.getVerticalUnitValue(), .001);
}

TEST(SegyInputParametersTests, test_error_invalid_vertical_domain)
{
    constexpr auto invalidDomain = static_cast<segyio::VerticalDomain>(-3);
    constexpr segyio::VerticalUOM anyUnit = segyio::VerticalUOM::meter;
    const auto g = create_geometry3d(invalidDomain, anyUnit);

    bool correct_exception_thrown = false;
    try {
        auto sut = SegyInputGeometry(g);
    }
    catch (const std::invalid_argument& ex) {
        if (std::strcmp("Unknown vertical domain value", ex.what()) == 0) {
            correct_exception_thrown = true;
        }
    }
    EXPECT_TRUE(correct_exception_thrown);
}

TEST(SegyInputParametersTests, test_error_invalid_vertical_unit_depth)
{
    const auto g = create_geometry3d(segyio::VerticalDomain::Depth, segyio::VerticalUOM::seconds);

    bool correct_exception_thrown = false;
    try {
        auto sut = SegyInputGeometry(g);
    }
    catch (const std::invalid_argument& ex) {
        if (std::strcmp("Invalid vertical unit for depth domain", ex.what()) == 0) {
            correct_exception_thrown = true;
        }
    }
    EXPECT_TRUE(correct_exception_thrown);
}

TEST(SegyInputParametersTests, test_error_invalid_vertical_unit_time)
{
    const auto g = create_geometry3d(segyio::VerticalDomain::Time, segyio::VerticalUOM::meter);

    bool correct_exception_thrown = false;
    try {
        auto sut = SegyInputGeometry(g);
    }
    catch (const std::invalid_argument& ex) {
        if (std::strcmp("Invalid vertical unit for time domain", ex.what()) == 0) {
            correct_exception_thrown = true;
        }
    }
    EXPECT_TRUE(correct_exception_thrown);
}

TEST(SegyInputParametersTests, test_brick_size)
{
    const utilities::BrickSize sut64("");
    EXPECT_EQ(sut64.sizeI(), 64);
    EXPECT_EQ(sut64.sizeJ(), 64);
    EXPECT_EQ(sut64.sizeK(), 64);

    const utilities::BrickSize sut32("32");
    EXPECT_EQ(sut32.sizeI(), 32);
    EXPECT_EQ(sut32.sizeJ(), 32);
    EXPECT_EQ(sut32.sizeK(), 32);

    const utilities::BrickSize sut12x34x56("12x34X56");
    EXPECT_EQ(sut12x34x56.sizeI(), 12);
    EXPECT_EQ(sut12x34x56.sizeJ(), 34);
    EXPECT_EQ(sut12x34x56.sizeK(), 56);

    EXPECT_EQ(utilities::BrickSize("1234567x1234567x1234567").sizeI(), 1234567);
    EXPECT_THROW(utilities::BrickSize("12345678x1234567x1234567"), std::invalid_argument);
    EXPECT_THROW(utilities::BrickSize("1234567x12345678x1234567"), std::invalid_argument);
    EXPECT_THROW(utilities::BrickSize("1234567x1234567x12345678"), std::invalid_argument);

    EXPECT_THROW(utilities::BrickSize("x34x56"), std::invalid_argument);
    EXPECT_THROW(utilities::BrickSize("12x34x"), std::invalid_argument);
    EXPECT_THROW(utilities::BrickSize("12x34x56x78"), std::invalid_argument);
    EXPECT_THROW(utilities::BrickSize("12xx34x56"), std::invalid_argument);
    EXPECT_THROW(utilities::BrickSize("12*34*56"), std::invalid_argument);
}
