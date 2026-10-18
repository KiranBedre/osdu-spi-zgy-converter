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

#include "SegyDefinitionProvider.h"

#include <map>
#include <vector>

namespace segysdk::segyio
{

class SEGYSDK_DLL_PUBLIC HeaderDefinitionProvider : public SegyDefinitionProvider
{
private:
    typedef std::map<std::string, std::shared_ptr<HeaderDefinition>> HeaderCache;

private:
    HeaderCache headerMap_;
    std::vector<std::string> headerNames_;

public:
    HeaderDefinitionProvider(const std::vector<std::shared_ptr<HeaderDefinition>>& headers, bool raw = false);
    virtual ~HeaderDefinitionProvider();

    virtual std::shared_ptr<HeaderDefinition> getTraceHeaderDefinition(const std::string& headerId) override;

    const std::vector<std::string>& getHeaderNames() const;
};

} // namespace segysdk::segyio
