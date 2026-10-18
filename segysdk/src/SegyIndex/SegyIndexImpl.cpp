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

#include "SegyIndexImpl.h"

#include "../SegyIndex/SegyIndexMultiFileStruct.h"
#include "../segysdk_details/SegyException.h"
#include "../SegyIndex/Utilities/TraceRangeFactory.h"

#include <algorithm>
#include <memory>
#include <cassert>

namespace segysdk::segyindex
{

SegyIndexImpl::SegyIndexImpl(
    std::vector<std::shared_ptr<segyio::SegyFile>> const& segyFilesVector,
    FileList const& segyFileNames,
    const fileio::FileReaderOptions& fileReaderOptions,
    std::optional<std::string> const& indexBaseFileNameOverride)
  : SegyIndexImpl(Params{segyio::StorageAccessParams{}
                             .withSDCredentials(fileReaderOptions.sdCredentials)
                             .withCacheSizeOption(fileReaderOptions.cacheSize)}
                      .withSegyFiles(segyFilesVector)
                      .withSegyFileNames(segyFileNames)
                      .withIndexBaseFileNameOverride(indexBaseFileNameOverride)
                      .withLegalTag(fileReaderOptions.sdLegalTag))
{
}

SegyIndexImpl::SegyIndexImpl(const Params& params)
  : indexData_{std::make_shared<index_data::SegyIndexData>()},
    indexGenerator_{indexData_},
    indexStorage_{fileio::FileReaderOptions{params.sdCredentials_, params.cacheSizeOption_, params.legalTag_}, indexData_},
    indexTraceInfo_{indexData_}
{
    if (params.segyFileNames_.empty()) {
        throw SegyInvalidArgumentException("SEG-Y index requires at least one input file.");
    }

    ResetIndexData();

    indexData_->addSegyFileNames(params.segyFileNames_);
    indexData_->segyFilesVector_ = params.segyFiles_;
    indexData_->indexBaseFileName_ = params.indexBaseFileNameOverride_.value_or(params.segyFileNames_[0]);
    hasIndexFile_ = indexData_->indexBaseFileName_.empty() ? false : indexStorage_.hasIndexFile(params);
}

SegyIndexImpl::~SegyIndexImpl()
{
    /* EMPTY */
}

bool
SegyIndexImpl::hasIndexFile(bool storageFileCheck) const
{
    if (storageFileCheck) {
        hasIndexFile_ = indexStorage_.hasIndexFile();
    }
    return hasIndexFile_;
}

void
SegyIndexImpl::Load()
{
    Load(request::Params{});
}

void
SegyIndexImpl::Load(const request::Params& params)
{
    if (not isLoaded_) {
        indexStorage_.Load(params);
        isLoaded_ = true;
    }
}

bool
SegyIndexImpl::Save()
{
    bool saved = indexStorage_.Save();
    if (saved) {
        hasIndexFile_ = isLoaded_ = true;
    }
    return saved;
}

void
SegyIndexImpl::SaveAsJson(const std::string& outputFileName) const
{
    indexStorage_.SaveAsJson(outputFileName);
    hasIndexFile_ = true;
    isLoaded_ = true;
}

void
SegyIndexImpl::deleteIndex()
{
    hasIndexFile_ = false;
    isLoaded_ = false;
    indexStorage_.deleteIndex();
}

bool
SegyIndexImpl::GenerateIndex(bool forceGenerateIndex)
{
    if (indexGenerator_.GenerateIndex(forceGenerateIndex)) {
        bool saved = indexStorage_.Save();
        if (saved) {
            hasIndexFile_ = isLoaded_ = true;
        }
        return saved;
    }

    return false;
}

Geometry3D
SegyIndexImpl::getGeometry3D() const
{
    return indexTraceInfo_.getGeometry3D();
}

FileLayoutSpecification
SegyIndexImpl::getFileLayout() const
{
    return indexTraceInfo_.getFileLayout();
}

SeismicCubeExtents
SegyIndexImpl::getGeometry3DExtents() const
{
    return indexTraceInfo_.getGeometry3DExtents();
}

SeismicCubeExtents
SegyIndexImpl::getFileLayoutExtents() const
{
    return indexTraceInfo_.getFileLayoutExtents();
}

index_2d::SeismicLineExtents
SegyIndexImpl::getLineExtents() const
{
    return indexTraceInfo_.getLineExtents();
}

bool
SegyIndexImpl::isRegular() const
{
    return indexTraceInfo_.isRegular();
}

bool
SegyIndexImpl::isSegy3D() const
{
    return not indexData_->is2D();
}

void
SegyIndexImpl::setSegy2DFlag(bool isSegy2D)
{
    indexData_->set2DFlag(isSegy2D);
}

bool
SegyIndexImpl::isValid() const
{
    return indexData_->isValid();
}

IndexField
SegyIndexImpl::getIndexAt(int inlineNo, int crosslineNo) const
{
    return indexTraceInfo_.getIndexAt(inlineNo, crosslineNo);
}

IndexField
SegyIndexImpl::getLiveIndexAt(int inlineNo, int crosslineNo) const
{
    return indexTraceInfo_.getLiveIndexAt(inlineNo, crosslineNo);
}

size_t
SegyIndexImpl::getOffsetForTraceNo(size_t traceNo) const
{
    return indexTraceInfo_.getOffsetForTraceNo(traceNo);
}

size_t
SegyIndexImpl::getOffsetForCMP(int CMP) const
{
    return indexTraceInfo_.getOffsetForCMP(CMP);
}

size_t
SegyIndexImpl::getTraceNoForCMP(int CMP, bool findNextClosest) const
{
    return indexTraceInfo_.getTraceNoForCMP(CMP, findNextClosest);
}

size_t
SegyIndexImpl::getSampleSize() const
{
    return indexTraceInfo_.getSampleSize();
}

size_t
SegyIndexImpl::getTraceSize() const
{
    return indexTraceInfo_.getTraceSize();
}

size_t
SegyIndexImpl::getFirstTraceOffset(uint16_t fileId) const
{
    return indexTraceInfo_.getFirstTraceOffset(fileId);
}

bool
SegyIndexImpl::isInlineOrdered() const
{
    return indexTraceInfo_.isInlineOrdered();
}

bool
SegyIndexImpl::hasConstantCoordinateScalar() const
{
    return indexTraceInfo_.hasConstantCoordinateScalar();
}

bool
SegyIndexImpl::hasConstantTraceLength() const
{
    return indexTraceInfo_.hasConstantTraceLength();
}

bool
SegyIndexImpl::hasConstantTraceWeightingFactor() const
{
    return indexTraceInfo_.hasConstantTraceWeightingFactor();
}

int
SegyIndexImpl::getConstantCoordinateScalar() const
{
    return indexTraceInfo_.getConstantCoordinateScalar();
}

size_t
SegyIndexImpl::getConstantTraceLength() const
{
    return indexTraceInfo_.getConstantTraceLength();
}

unsigned short
SegyIndexImpl::getConstantTraceWeightingFactor() const
{
    return indexTraceInfo_.getConstantTraceWeightingFactor();
}

size_t
SegyIndexImpl::getIndexedTraceCount() const
{
    return indexTraceInfo_.getIndexedTraceCount();
}

bool
SegyIndexImpl::isSeismicTimeIndexed() const
{
    return indexTraceInfo_.getGeometry3DExtents().getSampleExtent().domain() == segyio::VerticalDomain::Time;
}

float
SegyIndexImpl::getMinimumAmplitudeValue() const
{
    return indexTraceInfo_.getMinimumAmplitude();
}

float
SegyIndexImpl::getMaximumAmplitudeValue() const
{
    return indexTraceInfo_.getMaximumAmplitude();
}

std::string
SegyIndexImpl::getIndexBaseFileName() const
{
    return indexData_->indexBaseFileName_;
}

void
SegyIndexImpl::setGeometry3D(Geometry3D const& geometry)
{
    indexStorage_.setGeometry3D(geometry);
}

multi_file::FileNameStatus
SegyIndexImpl::validateIndexedFiles(FileList const& inputFileList, bool logDebugOutput) const
{
    return indexStorage_.validateIndexedFiles(inputFileList, logDebugOutput);
}

void
SegyIndexImpl::reconnectSEGYFilesAfterMove(const std::vector<std::string>& inputFileList)
{
    return indexStorage_.reconnectSEGYFilesAfterMove(inputFileList);
}

bool
SegyIndexImpl::ProcessIndexBuildingData(std::vector<std::shared_ptr<IndexBuilderCollatedData>> const& indexCollatedDataVector,
                                        segyio::VerticalDomain domain,
                                        segyio::VerticalUOM zUoM,
                                        size_t sampleSize,
                                        bool forceGenerateIndex)
{
    return indexGenerator_.ProcessIndexBuildingData(indexCollatedDataVector,
                                                    domain,
                                                    zUoM,
                                                    sampleSize,
                                                    forceGenerateIndex);
}

Geometry3D
SegyIndexImpl::getGeometry3DStructFromJson(Json::Value const& json)
{
    return index_storage::SegyIndexStorage::GetGeometry3DFromJson(json);
}

Json::Value
SegyIndexImpl::getGeometry3DAsJson(Geometry3D const& g)
{
    return index_storage::SegyIndexStorage::getGeometry3DAsJson(g);
}

FileLayoutSpecification
SegyIndexImpl::getFileLayoutStructFromJson(const Json::Value& json)
{
    return index_storage::SegyIndexStorage::getFileLayoutStructFromJson(json);
}

Json::Value
SegyIndexImpl::getFileLayoutAsJson(const FileLayoutSpecification& f)
{
    return index_storage::SegyIndexStorage::getFileLayoutAsJson(f);
}

bool
SegyIndexImpl::hasMissingTraceIndex() const
{     
    return indexStorage_.hasMissingTraceIndex();
}

struct traceSorter
{
public:
    traceSorter(bool isInlineOrdered, bool inlineIncreasing, bool crosslineIncreasing)
      : isInlineOrdered_(isInlineOrdered), inlineIncreasing_(inlineIncreasing), crosslineIncreasing_(crosslineIncreasing)
    {
    }

    inline bool operator()(const segysdk::bulkaccess::TraceHeaderResult& t1, const segysdk::bulkaccess::TraceHeaderResult& t2)
    {

        if (isInlineOrdered_) {
            if (inlineIncreasing_ and crosslineIncreasing_) {
                return t1.getInline() < t2.getInline() or (t1.getInline() == t2.getInline() and t1.getCrossline() < t2.getCrossline());
            }
            else if (inlineIncreasing_ and not crosslineIncreasing_) {
                return t1.getInline() < t2.getInline() or (t1.getInline() == t2.getInline() and t2.getCrossline() < t1.getCrossline());
            }
            if (not inlineIncreasing_ and crosslineIncreasing_) {
                return t2.getInline() < t1.getInline() or (t1.getInline() == t2.getInline() and t1.getCrossline() < t2.getCrossline());
            }
            else if (not inlineIncreasing_ and not crosslineIncreasing_) {
                return t2.getInline() < t1.getInline() or (t1.getInline() == t2.getInline() and t2.getCrossline() < t1.getCrossline());
            }
        }
        else {
            if (crosslineIncreasing_ and inlineIncreasing_) {
                return t1.getCrossline() < t2.getCrossline() or (t1.getCrossline() == t2.getCrossline() and t1.getInline() < t2.getInline());
            }
            else if (crosslineIncreasing_ and not inlineIncreasing_) {
                return t1.getCrossline() < t2.getCrossline() or (t1.getCrossline() == t2.getCrossline() and t2.getInline() < t1.getInline());
            }
            else if (not crosslineIncreasing_ and inlineIncreasing_) {
                return t2.getCrossline() < t1.getCrossline() or (t1.getCrossline() == t2.getCrossline() and t1.getInline() < t2.getInline());
            }
            else if (not crosslineIncreasing_ and not inlineIncreasing_) {
                return t2.getCrossline() < t1.getCrossline() or (t1.getCrossline() == t2.getCrossline() and t2.getInline() < t1.getInline());
            }
        }
        // Shouldn't be here!
        assert(false);
        return false;
    }

private:
    bool isInlineOrdered_;
    bool inlineIncreasing_;
    bool crosslineIncreasing_;
};

bool
SegyIndexImpl::storeEmptyDeadMissingTraces(std::vector<segysdk::bulkaccess::TraceHeaderResult> missingTraces, const std::vector<segysdk::bulkaccess::TraceHeaderResult>& emptyTraces, const std::vector<segysdk::bulkaccess::TraceHeaderResult>& deadTraces)
{
    if (!deadTraces.empty()) {
        std::copy(deadTraces.begin(), deadTraces.end(), std::back_inserter(missingTraces));
    }
    if (!emptyTraces.empty()) {
        std::copy(emptyTraces.begin(), emptyTraces.end(), std::back_inserter(missingTraces));
    }

    const bool increasingInlines = indexData_->fileLayout_.JExtent.first() < indexData_->fileLayout_.JExtent.last();
    const bool increasingCrosslines = indexData_->fileLayout_.IExtent.first() < indexData_->fileLayout_.IExtent.last();
    
    const traceSorter sorter(getFileLayout().isInlineOrdered, increasingInlines, increasingCrosslines);    
    std::sort(std::begin(missingTraces), std::end(missingTraces), sorter);

    return indexStorage_.saveBinaryMissingIndexData3D(indexTraceInfo_, missingTraces);
}

void 
SegyIndexImpl::loadMissingTraceIndex(const request::Params& params)
{
    indexStorage_.loadBinaryMissingIndexData3D(params);

    isLiveTraceIndexIndexLoaded_ = indexStorage_.isLiveTracesIndexLoaded();

}

void
SegyIndexImpl::ResetIndexData()
{
    indexGenerator_.ResetIndexData();
    indexStorage_.ResetIndexData();
    isLoaded_ = false;
}

} // namespace segysdk::segyindex
