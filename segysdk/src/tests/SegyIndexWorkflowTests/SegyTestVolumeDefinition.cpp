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

#include "SegyTestVolumeDefinition.h"

#include <utility>

namespace segysdk::segyindex::tests
{

SegyTestVolumeDefinition::SegyTestVolumeDefinition(std::string datasetName, std::string segyFile, std::string configFile, int segyVersion, size_t initialHeaderOffset, int indexedTraceCount, size_t traceSize,
                                                   bool hasConstantCoordScalar, bool hasConstantTraceLength, bool hasConstantTWF,
                                                   Geometry3D geometry3D, FileLayoutSpecification fileLayout)
  : datasetName_(std::move(datasetName)),
    segyFile_(std::move(segyFile)),
    configFile_(std::move(configFile)),
    segyVersion_(segyVersion),
    initialHeaderOffset_(initialHeaderOffset),
    indexedTraceCount_(indexedTraceCount),
    traceSize_(traceSize),
    hasConstantCoordScalar_(hasConstantCoordScalar),
    hasConstantTraceLength_(hasConstantTraceLength),
    hasConstantTWF_(hasConstantTWF),
    geometry3D_(geometry3D),
    fileLayout_(fileLayout)
{
}

} // namespace segysdk::segyindex::tests
