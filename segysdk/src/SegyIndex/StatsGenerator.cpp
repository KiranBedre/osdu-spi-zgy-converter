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

#include "StatsGenerator.h"

#include "../SegyIO/SegyIOException.h"
#include <cmath>
#include <utility>

namespace segysdk::segyindex
{

StatsGenerator::StatsGenerator(std::vector<std::pair<int, Vector>> statsData)
  : statsData_(std::move(statsData))
{
    if (statsData_.size() <= 0)
        return;

    prepareVectorData();

    calculateTotals();
    calculateMeans();
    calculateMedians();
    calculateStandardDeviations();
}

void
StatsGenerator::prepareVectorData()
{
    for (const auto& [id, data] : statsData_) {
        radiansVector_.push_back(data.getRadians());
        unitDistanceVector_.push_back(data.getUnitDistance());
    }
}

void
StatsGenerator::calculateTotals()
{

    totalRadians_ = std::accumulate(radiansVector_.begin(), radiansVector_.end(), 0.0);
    totalUnitDistance_ = std::accumulate(unitDistanceVector_.begin(), unitDistanceVector_.end(), 0.0);
}

void
StatsGenerator::calculateMeans()
{
    meanRadian_ = totalRadians_ / statsData_.size();
    meanUnitDistance_ = totalUnitDistance_ / statsData_.size();
}

void
StatsGenerator::calculateMedians()
{

    std::sort(radiansVector_.begin(), radiansVector_.end());
    std::sort(unitDistanceVector_.begin(), unitDistanceVector_.end());

    const auto ix = static_cast<int>(std::round(unitDistanceVector_.size() / 2.0));

    medianRadian_ = radiansVector_[ix];
    medianUnitDistance_ = unitDistanceVector_[ix];
}

void
StatsGenerator::calculateStandardDeviations()
{
    std::vector<double> diff(radiansVector_.size());
    double mean = meanRadian_;
    std::transform(radiansVector_.begin(), radiansVector_.end(), diff.begin(), [mean](double x) { return x - mean; });
    double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
    stddevRadians_ = std::sqrt(sq_sum / radiansVector_.size());

    mean = meanUnitDistance_;
    std::transform(unitDistanceVector_.begin(), unitDistanceVector_.end(), diff.begin(), [mean](double x) { return x - mean; });
    sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);

    stddevUnitDistance_ = std::sqrt(sq_sum / unitDistanceVector_.size());
}

Vector
StatsGenerator::getVector() const
{
    for (const auto& [id, data] : statsData_) {
        if (std::abs(data.getRadians() - medianRadian_) < tolerance_) {
            return Vector(data);
        }
    }

    throw segyio::SegyRuntimeException("StatsGenerator::getVector : Unable to locate usable vector");
}

} // namespace segysdk::segyindex
