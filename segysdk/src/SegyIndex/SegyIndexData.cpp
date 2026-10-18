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

#include "SegyIndexData.h"
#include "SegyIndexConfig.h"

#include "../SegyIO/Primitives.h"

namespace segysdk::segyindex
{
namespace index_data
{

void
SegyIndexData::setInitialDefaults()
{
    indexVersionMajor_ = index_config::IndexMajorVersionCurrent_;
    indexVersionMinor_ = index_config::IndexMinorVersionCurrent_;

    offsetFirtstSEGYTrace_ = segysdk::segyio::Primitives::getTextualHeaderSize()
                             + segysdk::segyio::Primitives::getBinaryHeaderSize();
    sampleSize_ = segysdk::segyio::Primitives::getSampleSize(segysdk::segyio::SampleFormat::IeeeFloat4);
    traceHeaderSize_ = segysdk::segyio::Primitives::getTraceHeaderSize(); // OBS for SEG-Y rev 2

    hasConstantCoordinateScalar_ = false;
    hasConstantTraceLength_ = false;
    hasConstantTraceWeightingFactor_ = false;

    constantCoordinateScalar_ = 1;
    constantTraceLength_ = 0;
    constantTraceWeightingFactor_ = 1;

    amplitudeMinimum_ = std::numeric_limits<float>::max();
    amplitudeMaximum_ = -std::numeric_limits<float>::max();

    segyFilesIndexed_.clear();
    multiFileMap_.clear();

    indexedTraceCount_ = 0U;
    traceIndexMap_.clearRanges();
    missingTraceIndexMap_.clearRanges();

    traceIndexStorage_.clearStorage();
    indexing2dInfo_.clear();
}

void
SegyIndexData::setEmptyGeometry3D()
{
    geometry3D_.JVectorExtent = {0, 0, 1};

    geometry3D_.IVectorExtent = {0, 0, 1};

    geometry3D_.sampleExtent = {0, 0, 1};

    geometry3D_.dataGrid.gridCell_11.i = 0;
    geometry3D_.dataGrid.gridCell_11.j = 0;
    geometry3D_.dataGrid.gridCell_11.location = {0, 0};

    geometry3D_.dataGrid.gridCell_1N.i = 0;
    geometry3D_.dataGrid.gridCell_1N.j = 0;
    geometry3D_.dataGrid.gridCell_1N.location = {0, 0};

    geometry3D_.dataGrid.gridCell_N1.i = 0;
    geometry3D_.dataGrid.gridCell_N1.j = 0;
    geometry3D_.dataGrid.gridCell_N1.location = {0, 0};

    geometry3D_.dataGrid.gridCell_NN.i = 0;
    geometry3D_.dataGrid.gridCell_NN.j = 0;
    geometry3D_.dataGrid.gridCell_NN.location = {0, 0};

    geometry3D_.dataGrid.JVectorUnitDistance = 0;
    geometry3D_.dataGrid.IVectorUnitDistance = 0;
    geometry3D_.dataGrid.IVectorRotationAngle = 0;
    geometry3D_.dataGrid.JVectorRotationAngle = 0;

    geometry3D_.isInlineOrdered = true;
    geometry3D_.isRegular = false;
}

void
SegyIndexData::setEmptyFileLayout()
{
    fileLayout_.JExtent = {0, 0, 1};

    fileLayout_.IExtent = {0, 0, 1};

    fileLayout_.sampleExtent = {0, 0, 1};

    fileLayout_.isInlineOrdered = true;
    fileLayout_.isRegular = false;
}

void
SegyIndexData::addSegyFileNames(FileList const& segyFileNames)
{
    for (const std::string& fileName : segyFileNames) {
        multi_file::MultiFileStruct mfs(fileName);
        segyFilesIndexed_.push_back(std::move(mfs));
    }
}

bool
SegyIndexData::isValid() const
{
    if (is2D()) {
        return indexing2dInfo_.isValid();
    }
    bool const isGeometry3DValid = geometry3D_.JVectorExtent.first() not_eq geometry3D_.JVectorExtent.last()
                                   and geometry3D_.JVectorExtent.step() not_eq 0
                                   and geometry3D_.IVectorExtent.first() not_eq geometry3D_.IVectorExtent.last()
                                   and geometry3D_.IVectorExtent.step() not_eq 0
                                   and geometry3D_.sampleExtent.first() not_eq geometry3D_.sampleExtent.last()
                                   and geometry3D_.sampleExtent.step() not_eq 0;

    bool const isFileLayoutValid = fileLayout_.JExtent.first() not_eq fileLayout_.JExtent.last()
                                   and fileLayout_.JExtent.step() not_eq 0
                                   and fileLayout_.IExtent.first() not_eq fileLayout_.IExtent.last()
                                   and fileLayout_.IExtent.step() not_eq 0
                                   and fileLayout_.sampleExtent.first() not_eq fileLayout_.sampleExtent.last()
                                   and fileLayout_.sampleExtent.step() not_eq 0;

    bool const isFileRegular = geometry3D_.isRegular and fileLayout_.isRegular;

    return isGeometry3DValid and isFileLayoutValid
           and (isFileRegular or !traceIndexMap_.isEmpty());
}

bool
SegyIndexData::isRegular() const
{
    if (is2D()) {
        return indexing2dInfo_.isRegular();
    }
    bool const isGeometry3dDefined = geometry3D_.IsDefined();
    bool const isFileLayoutDefined = fileLayout_.IsDefined();

    return (isGeometry3dDefined
                ? geometry3D_.isRegular
                : true)
           and (isFileLayoutDefined
                    ? fileLayout_.isRegular
                    : true);
}

bool
SegyIndexData::is2D() const
{
    return is2D_;
}

void
SegyIndexData::set2DFlag(bool is2D)
{
    is2D_ = is2D;
}

} // namespace index_data
} // namespace segysdk::segyindex
