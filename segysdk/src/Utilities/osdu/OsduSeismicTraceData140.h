/*
Copyright 2023 Schlumberger

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

#include "../Utilities/osdu/OsduSeismicTraceData100.h"

#include "../SegyIndex/SegyIndex.h"
#include "../Utilities/BinGrid.h"
#include "OsduFileCollectionSegy100.h"
#include "OsduObject.h"

#include "../segysdk_details/DLL_EXPORT.h"

#include <memory>

namespace segysdk::utilities::osdu
{

class SEGYSDK_DLL_PUBLIC OsduSeismicTraceData140 : public OsduSeismicTraceData100
{
protected:
    static constexpr std::string_view const version_ = "1.4.0";

public:
    explicit OsduSeismicTraceData140(const Json::Value& root);

    virtual void checkAndUpdateMissingFields(const std::shared_ptr<segysdk::segyindex::SegyIndex>& index,
                                             const std::shared_ptr<segysdk::utilities::BinGrid>& grid);

protected:
    void updateMetaWithUOM(const TraceDomainUOM& uom, std::string property);

private:
    double getSEGYValue(double value);
    void checkAndUpdateSpatialArea(const std::shared_ptr<segysdk::segyindex::SegyIndex>& index,
                                   const std::shared_ptr<segysdk::utilities::BinGrid>& grid);
    void updateTraceDataFields(const std::shared_ptr<segysdk::segyindex::SegyIndex>& index);
    void updateVersionToCurrent();
};

} // namespace segysdk::utilities::osdu
