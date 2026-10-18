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

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include "Geometry3D.h"
#include "Vector.h"

namespace segysdk::segyindex
{

class SEGYSDK_DLL_PUBLIC MinMaxRadians
{
private:
    long double minRadian_;
    long double maxRadian_;

private:
    static long double cleanRadiansMin(long double radian);
    static long double cleanRadiansMax(long double radian);

public:
    MinMaxRadians(long double min, long double max);

public:
    double min() const { return static_cast<double>(minRadian_); }
    double max() const { return static_cast<double>(maxRadian_); }
};

class SEGYSDK_DLL_PUBLIC GridCalculator
{
private:
    Vector iVector_;
    Vector jVector_;
    HorizontalExtent iExtent_;
    HorizontalExtent jExtent_;
    Geometry3D geometry_;

public:
    static constexpr long double PI = 3.1415926535897932384626433832795L;
    static constexpr long double Two_PI = 2 * PI;
    static constexpr long double PI_half = PI / 2.0;
    static constexpr long double Three_PI_half = PI * 3.0 / 2.0;

private:
    static long double cleanRadians(long double radian);
    long double getGridTraceIncrement(long double increment, long double axisRadians, long double radians) const;
    bool isDirectionPositive(long double axisRadians, long double deltaRadians) const;
    static bool isDeltaPositiveDirection(long double axisRadians, long double offsetRadians);

public:
    explicit GridCalculator(const Geometry3D& geometry);
    GridCalculator(const Vector& iVector, const Vector& jVector);

public:
    Coordinate GetXYLocation(int i, int j) const;
    GridCell GetLocation(Coordinate coordinate) const;
};

} // namespace segysdk::segyindex
