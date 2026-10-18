/*
Copyright 2022 Schlumberger

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

#include "../SegyIO/SegyFile.h"
#include "../SegyIndex/FileLayoutSpecification.h"
#include "../SegyIndex/Geometry3D.h"
#include "../SegyIndex/SegyIndexMultiFileStruct.h"
#include "../SegyIndex/Utilities/TraceIndexMap.h"
#include "../SegyIndex/Utilities/TraceIndexStorage.h"
#include "../SegyIndex/SeismicCubeExtents.h"
#include "../SegyIndex/2DIndexing/IndexingTraceData2DInfo.h"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace segysdk::segyindex
{
namespace index_data
{
using TraceIndexStorage = trace_range_map::TraceIndexStorage;
using TraceIndexMap = trace_range_map::TraceIndexMap;
using IndexingTraceData2DInfo = index_2d::IndexingTraceData2DInfo;
using MultiFileMap = multi_file::MultiFileMap;
using FileList = std::vector<std::string>;
using MultiFileStructVector = std::vector<multi_file::MultiFileStruct>;
using SegyFilePtr = std::shared_ptr<segyio::SegyFile>;
using SegyFileVector = std::vector<SegyFilePtr>;

struct SEGYSDK_DLL_PUBLIC SegyIndexData final
{
    Geometry3D geometry3D_{};
    FileLayoutSpecification fileLayout_{};
    TraceIndexStorage traceIndexStorage_{};
    TraceIndexStorage missingTraceIndexStorage_{};
 
    TraceIndexMap traceIndexMap_{};
    TraceIndexMap missingTraceIndexMap_{};
    IndexingTraceData2DInfo indexing2dInfo_{}; 
    std::string indexBaseFileName_{};
    MultiFileStructVector segyFilesIndexed_{};
    MultiFileMap multiFileMap_{};
    SegyFileVector segyFilesVector_{};
    size_t sampleSize_{};
    size_t offsetFirtstSEGYTrace_{};
    size_t traceHeaderSize_{};
    size_t traceLength_{};
    size_t indexedTraceCount_{};
    size_t indexedLiveTraceCount_{};

    float amplitudeMinimum_{std::numeric_limits<float>::max()};
    float amplitudeMaximum_{-std::numeric_limits<float>::max()};

    int indexVersionMajor_{};
    int indexVersionMinor_{};
    bool hasConstantCoordinateScalar_{};
    bool hasConstantTraceLength_{};
    bool hasConstantTraceWeightingFactor_{};
    bool is2D_{false};
    int constantCoordinateScalar_{};
    size_t constantTraceLength_{};
    unsigned short constantTraceWeightingFactor_{};

    void setInitialDefaults();
    void setEmptyGeometry3D();
    void setEmptyFileLayout();
    void addSegyFileNames(FileList const& segyFileNames);
    void set2DFlag(bool is2D);
    bool isValid() const;
    bool isRegular() const;
    bool is2D() const;
};

using SegyIndexDataPtr = std::shared_ptr<SegyIndexData>;

} // namespace index_data
} // namespace segysdk::segyindex
