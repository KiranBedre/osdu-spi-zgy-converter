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

#include "HeaderDefinitionProvider.h"

#include "HeaderDefinition.h"

namespace segysdk::segyio
{

HeaderDefinitionProvider::HeaderDefinitionProvider(const std::vector<std::shared_ptr<HeaderDefinition>>& headers, bool raw)
{
    headerNames_.reserve(headers.size());

    for (const auto& header : headers) {
        if (raw) { // Make copies without scaling or overrides
            const auto rawHeader = std::make_shared<HeaderDefinition>(header->getHeaderId(), header->getDisplayName(), header->getOffset(), header->getByteCount());
            rawHeader->setHeaderDataType(header->getHeaderDataType());
            headerMap_[header->getHeaderId()] = rawHeader;
        }
        else {
            headerMap_[header->getHeaderId()] = header;
        }

        headerNames_.push_back(header->getHeaderId());
    }
}

HeaderDefinitionProvider::~HeaderDefinitionProvider()
{}

std::shared_ptr<HeaderDefinition>
HeaderDefinitionProvider::getTraceHeaderDefinition(const std::string& headerId)
{
    const auto entry = headerMap_.find(headerId);
    if (entry != headerMap_.end())
        return entry->second;

    return nullptr;
}

const std::vector<std::string>&
HeaderDefinitionProvider::getHeaderNames() const
{
    return headerNames_;
}

} // namespace segysdk::segyio
