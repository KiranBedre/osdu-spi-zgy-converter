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

#include "HeaderDefinition.h"

#pragma warning(push)
#pragma warning(disable : 4995) // 'function': name was marked as #pragma deprecated

#include <algorithm>
#include <string>
#include <utility>

#include "HeaderOverride.h"
#pragma warning(pop)


namespace segysdk::segyio
{

HeaderDefinition::HeaderDefinition(std::string id, std::string displayName, uint64_t offset, unsigned short byteCount)
  : id_(std::move(id)),
    displayName_(std::move(displayName)),
    offset_(offset),
    byteCount_(byteCount),
    headerDataType_(HeaderDataType::UnsignedInteger)
{
}

HeaderDefinition::HeaderDefinition(const HeaderDefinition& other)
  : id_(other.id_),
    displayName_(other.displayName_),
    offset_(other.offset_),
    scalarUsageStrategy_(other.scalarUsageStrategy_),
    attribute_uom_(other.attribute_uom_),
    extraUnitScalar_(other.extraUnitScalar_),
    scalarOverride_(other.scalarOverride_),
    scalarHeaderId_(other.scalarHeaderId_),
    // Erik ToDo:
    // unitDescriptor_(other.unitDescriptor_),
    isWellKnown_(other.isWellKnown_),
    byteCount_(other.byteCount_),
    headerDataType_(other.headerDataType_)
{
    if (other.cachedOverride_ != nullptr) {
        this->cachedOverride_ = std::make_unique<HeaderOverride>(*other.cachedOverride_);
    }
    else {
        this->cachedOverride_ = nullptr;
    }
}

HeaderDefinition&
HeaderDefinition::operator=(HeaderDefinition other)
{
    std::swap(id_, other.id_);
    std::swap(displayName_, other.displayName_);
    std::swap(offset_, other.offset_);
    std::swap(byteCount_, other.byteCount_);
    std::swap(scalarHeaderId_, other.scalarHeaderId_);
    std::swap(headerDataType_, other.headerDataType_);
    std::swap(isWellKnown_, other.isWellKnown_);
    std::swap(scalarOverride_, other.scalarOverride_);
    // Erik ToDo:
    // unitDescriptor_(other.unitDescriptor_),
    std::swap(extraUnitScalar_, other.extraUnitScalar_);
    std::swap(scalarUsageStrategy_, other.scalarUsageStrategy_);
    std::swap(attribute_uom_, other.attribute_uom_);

    if (other.cachedOverride_ != nullptr) {
        this->cachedOverride_ = std::make_unique<HeaderOverride>(*other.cachedOverride_);
    }
    else {
        this->cachedOverride_ = nullptr;
    }
    return *this;
}

HeaderDefinition::~HeaderDefinition()
{}

void
HeaderDefinition::setOffset(uint64_t newOffset)
{
    this->offset_ = newOffset;
}

uint64_t
HeaderDefinition::getOffset() const
{
    return this->offset_;
}

uint16_t
HeaderDefinition::getByteCount() const
{
    return this->byteCount_;
}

std::string
HeaderDefinition::getHeaderId() const
{
    return this->id_;
}

void
HeaderDefinition::setHeaderId(const std::string& headerId)
{
    this->id_ = headerId;
}

std::string
HeaderDefinition::getExternalHeaderId() const
{
    return this->externalId_;
}

std::string
HeaderDefinition::getDisplayName() const
{
    return this->displayName_;
}

void
HeaderDefinition::setDisplayName(const std::string& displayName)
{
    this->displayName_ = displayName;
}

std::string
HeaderDefinition::getScalarHeaderId() const
{
    return this->scalarHeaderId_;
}

HeaderDataType
HeaderDefinition::getHeaderDataType() const
{
    return this->headerDataType_;
}

bool
HeaderDefinition::getIsWellKnown() const
{
    return this->isWellKnown_;
}

int
HeaderDefinition::getScalarOverride() const
{
    return this->scalarOverride_;
}

ScalarUsageStrategy
HeaderDefinition::getScalarUsageStrategy() const
{
    return this->scalarUsageStrategy_;
}

void
HeaderDefinition::setCachedHeaderOverride(const HeaderOverride& headerOverride)
{
    this->cachedOverride_.reset(new HeaderOverride(headerOverride));
}

void
HeaderDefinition::setExtraUnitScalar(float value)
{
    this->extraUnitScalar_ = value;
}

void
HeaderDefinition::setScalarHeaderId(const std::string& scalarHeaderId)
{
    this->scalarHeaderId_ = scalarHeaderId;
    if (scalarHeaderId.empty()) {
        this->scalarUsageStrategy_ = (this->scalarOverride_ != 0) ? ScalarUsageStrategy::Override : ScalarUsageStrategy::NoScaling;
    }
    else {
        this->scalarUsageStrategy_ = (this->scalarOverride_ != 0) ? ScalarUsageStrategy::Override : ScalarUsageStrategy::Default;
    }
}

void
HeaderDefinition::setExternalHeaderId(const std::string& externalHeaderId)
{
    this->externalId_ = externalHeaderId;
}

void
HeaderDefinition::setScalarOverride(int value)
{
    this->scalarOverride_ = value;
    if (value != 0) {
        this->scalarUsageStrategy_ = ScalarUsageStrategy::Override;
    }
}

void
HeaderDefinition::setScalarUsageStrategy(const ScalarUsageStrategy scalarUsageStrategy)
{
    this->scalarUsageStrategy_ = scalarUsageStrategy;
}

void
HeaderDefinition::setUoM(std::string unitSymbol)
{
    this->attribute_uom_ = std::move(unitSymbol);
}

} // namespace segysdk::segyio
