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

#pragma once

#include "../SegyIndex/IndexField.h"

#include "../SegyIndex/FileLayoutSpecification.h"
#include "../SegyIndex/Geometry3D.h"

#include <string>
#include <vector>

namespace segysdk::segyindex::tests
{

class SegyTestVolumeDefinition
{
private:
    std::string datasetName_;
    std::string segyFile_;
    std::string configFile_;
    int segyVersion_;
    size_t initialHeaderOffset_;
    int indexedTraceCount_;
    size_t traceSize_;
    bool hasConstantCoordScalar_;
    bool hasConstantTraceLength_;
    bool hasConstantTWF_;
    Geometry3D geometry3D_;
    FileLayoutSpecification fileLayout_;
    std::vector<IndexField> indexLocations_;

public:
    SegyTestVolumeDefinition(std::string datasetName, std::string segyFile, std::string configFile, int segyVersion, size_t initialHeaderOffset,
                             int indexedTraceCount, size_t traceSize,
                             bool hasConstantCoordScalar, bool hasConstantTraceLength, bool hasConstantTWF,
                             Geometry3D geometry3D, FileLayoutSpecification fileLayout);

    std::string getDatasetName() const { return datasetName_; }
    std::string getSegyFile() const { return segyFile_; }
    std::string getConfigFile() const { return configFile_; }
    int getSegyVersion() const { return segyVersion_; }
    size_t getInitialHeaderOffset() const { return initialHeaderOffset_; }
    int getIndexedTraceCount() const { return indexedTraceCount_; }
    size_t getTraceSize() const { return traceSize_; }
    bool hasConstantCoordinateScalar() const { return hasConstantCoordScalar_; }
    bool hasConstantTraceLength() const { return hasConstantTraceLength_; }
    bool hasConstantTraceWeightingFactor() const { return hasConstantTWF_; }
    Geometry3D getGeometryInfo() const { return geometry3D_; }
    FileLayoutSpecification getFileLayoutInfo() const { return fileLayout_; }
    void addIndexTestLocations(IndexField indexLocation) { indexLocations_.push_back(indexLocation); }

    std::vector<IndexField>& getIndexTestLocations() { return indexLocations_; }
};

} // namespace segysdk::segyindex::tests
