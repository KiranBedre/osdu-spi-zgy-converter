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

#include "Vector.h"

#include <algorithm>
#include <memory>
#include <numeric>
#include <vector>

namespace segysdk::segyindex
{

class StatsGenerator
{

private:
    double tolerance_ = 0.000001;
    std::vector<std::pair<int, Vector>> statsData_;

    double meanRadian_{0};
    double medianRadian_{0};
    double stddevRadians_{0};
    double totalRadians_{0};
    double meanUnitDistance_{0};
    double medianUnitDistance_{0};
    double stddevUnitDistance_{0};
    double totalUnitDistance_{0};

    std::vector<double> radiansVector_;
    std::vector<double> unitDistanceVector_;

private:
    void prepareVectorData();
    void calculateTotals();
    void calculateMeans();
    void calculateMedians();
    void calculateStandardDeviations();

public:
    explicit StatsGenerator(std::vector<std::pair<int, Vector>> statsData);

    double getMeanRadian() const { return meanRadian_; }
    double getMedianRadian() const { return medianRadian_; }
    double getStdDevRadians() const { return stddevRadians_; }
    double getTotalRadians() const { return totalRadians_; }

    double getMeanUnitDistance() const { return meanUnitDistance_; }
    double getMedianUnitDistance() const { return medianUnitDistance_; }
    double getStdDevUnitDistance() const { return stddevUnitDistance_; }
    double getTotalUnitDitance() const { return totalUnitDistance_; }

    Vector getVector() const;
};

} // namespace segysdk::segyindex
