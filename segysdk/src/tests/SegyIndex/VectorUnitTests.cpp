/*
Copyright 2021 Schlumberger

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

#include <cmath>

#include "../SegyIndex/Vector.h"

#include <gtest/gtest.h>

namespace segysdk::segyindex::tests
{

class VectorTest : public testing::Test
{

protected:
#ifndef M_PI
    static constexpr double M_PI = 3.14159265358979323846;   // pi;
    static constexpr double M_PI_2 = 1.57079632679489661923; // pi / 2;
#endif

    GridCell testCell1_{101, 10101, {2000, 23000}};

    GridCell testCell2_{101, 10125, {3000, 25000}};

    GridCell testCell3_{125, 10101, {3000, 25000}};

    const HorizontalExtent inlineExtent{101, 125, 1};
    const HorizontalExtent crosslineExtent{10101, 10125, 1};
};

TEST_F(VectorTest, InlineVector_Test1)
{

    Vector sut = Vector::GenerateIVector(testCell1_, testCell2_);

    const double expected_dx = (testCell2_.location.X - testCell1_.location.X) / crosslineExtent.gaps();
    const double expected_dy = (testCell2_.location.Y - testCell1_.location.Y) / crosslineExtent.gaps();

    double expected_radians = M_PI_2 - std::atan2(expected_dy, expected_dx);
    if (expected_radians < 0)
        expected_radians += 2 * M_PI;
    const double expected_angle = expected_radians * 180 / M_PI;
    const double expected_distance = sqrt(expected_dx * expected_dx + expected_dy * expected_dy) * crosslineExtent.gaps();

    EXPECT_DOUBLE_EQ(sut.getRadians(), expected_radians);

    EXPECT_DOUBLE_EQ(sut.getDistance(), expected_distance);
    EXPECT_DOUBLE_EQ(sut.getDegrees(), expected_angle);
    EXPECT_DOUBLE_EQ(sut.getDX(), expected_dx);
    EXPECT_DOUBLE_EQ(sut.getDY(), expected_dy);
    EXPECT_EQ(sut.getTraceGaps(), inlineExtent.gaps());
}

TEST_F(VectorTest, InlineVector_Test2)
{

    const double expected_angle = 60.0;
    const double unit_distance = 25.0;

    // input degrees are here grid north!
    const double expected_radians = expected_angle * M_PI / 180;
    const double expected_dx = unit_distance * std::sin(expected_radians);
    const double expected_dy = unit_distance * std::cos(expected_radians);
    const double expected_distance = sqrt(expected_dx * expected_dx + expected_dy * expected_dy) * crosslineExtent.gaps();

    const GridCell expectedCell{testCell1_.i, crosslineExtent.last(), {testCell1_.location.X + expected_dx * crosslineExtent.gaps(), testCell1_.location.Y + expected_dy * crosslineExtent.gaps()}};
    Vector sut = Vector::GenerateIVector(testCell1_, expected_angle, unit_distance, crosslineExtent.gaps());
    const Vector expected = Vector::GenerateIVector(testCell1_, expectedCell);

    EXPECT_DOUBLE_EQ(sut.getRadians(), expected.getRadians());
    EXPECT_DOUBLE_EQ(sut.getDistance(), expected_distance);
    EXPECT_DOUBLE_EQ(sut.getDistance(), expected.getDistance());
    EXPECT_DOUBLE_EQ(sut.getDX(), expected.getDX());
    EXPECT_DOUBLE_EQ(sut.getDY(), expected.getDY());
    EXPECT_EQ(sut.getTraceGaps(), inlineExtent.gaps());
}

TEST_F(VectorTest, CrosslineVector_Test1)
{

    Vector sut = Vector::GenerateJVector(testCell1_, testCell3_);

    const double expected_dx = (testCell3_.location.X - testCell1_.location.X) / inlineExtent.gaps();
    const double expected_dy = (testCell3_.location.Y - testCell1_.location.Y) / inlineExtent.gaps();

    double expected_radians = M_PI_2 - std::atan2(expected_dy, expected_dx);
    if (expected_radians < 0)
        expected_radians += 2 * M_PI;
    const double expected_distance = sqrt(expected_dx * expected_dx + expected_dy * expected_dy) * inlineExtent.gaps();

    EXPECT_DOUBLE_EQ(sut.getRadians(), expected_radians);
    EXPECT_DOUBLE_EQ(sut.getDistance(), expected_distance);
    EXPECT_DOUBLE_EQ(sut.getDX(), expected_dx);
    EXPECT_DOUBLE_EQ(sut.getDY(), expected_dy);
    EXPECT_EQ(sut.getTraceGaps(), inlineExtent.gaps());
}

TEST_F(VectorTest, CrosslineVector_Test2)
{

    const double expected_angle = 60.0;
    const double unit_distance = 25.0;

    // input degrees are here grid north!
    const double expected_radians = expected_angle * M_PI / 180;
    const double expected_dx = unit_distance * std::sin(expected_radians);
    const double expected_dy = unit_distance * std::cos(expected_radians);
    const double expected_distance = sqrt(expected_dx * expected_dx + expected_dy * expected_dy) * inlineExtent.gaps();

    const GridCell expectedCell{inlineExtent.last(), testCell1_.j, {testCell1_.location.X + expected_dx * crosslineExtent.gaps(), testCell1_.location.Y + expected_dy * crosslineExtent.gaps()}};
    Vector sut = Vector::GenerateJVector(testCell1_, expected_angle, unit_distance, inlineExtent.gaps());
    const Vector expected = Vector::GenerateJVector(testCell1_, expectedCell);

    EXPECT_DOUBLE_EQ(sut.getRadians(), expected.getRadians());
    EXPECT_DOUBLE_EQ(sut.getDistance(), expected_distance);
    EXPECT_DOUBLE_EQ(sut.getDistance(), expected.getDistance());
    EXPECT_DOUBLE_EQ(sut.getDX(), expected.getDX());
    EXPECT_DOUBLE_EQ(sut.getDY(), expected.getDY());
    EXPECT_EQ(sut.getTraceGaps(), inlineExtent.gaps());
}
} // namespace segysdk::segyindex::tests
