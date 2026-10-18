/*
Copyright 2021-2024 Schlumberger

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

#include "tools/SegyToZgy/library/crs/CoordinateReferenceSystemResolver.hpp"

#include "Logger/Logger.h"
#include "Utilities/JsonUtilities.h"

PersistableReferenceCrs
getPersistableReferenceCrsFromCoordinateReferenceSystem(const std::string& coordinateReferenceSystemId)
{
    const auto crsRecord = segysdk::utilities::JsonUtilities::getJsonObjectFromPathOrOsduId(coordinateReferenceSystemId);

    if (crsRecord["data"].isMember("PersistableReference")) {
        return PersistableReferenceCrs(crsRecord["data"]["PersistableReference"].asString());
    }

    throw std::invalid_argument("CoordinateReferenceSystem record does not contain persistableReferenceCrs: " + coordinateReferenceSystemId);
}

ConversionUnit
getBinGridConversionUnit(const segysdk::utilities::BinGrid& binGrid)
{
    if (!binGrid.coordinateReferenceSystemId.empty()) {
        try {
            return getPersistableReferenceCrsFromCoordinateReferenceSystem(binGrid.coordinateReferenceSystemId).getConversionUnit();
        }
        catch (const std::exception& e) {
            SEGYLOG_DEBUG(std::string("Falling back to SeismicBinGrid persistableReferenceCrs after CRS lookup failed: ") + e.what());
        }
    }

    return PersistableReferenceCrs(binGrid.persistableReferenceCrs).getConversionUnit();
}