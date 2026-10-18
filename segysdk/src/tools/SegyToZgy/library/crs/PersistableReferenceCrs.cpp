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

#include "PersistableReferenceCrs.h"

#include "../tools/SegyToZgy/library/ConversionUnit.h"
#include "../tools/SegyToZgy/library/JsonLoader.h"
#include "../tools/SegyToZgy/library/crs/WktCoordinateSystem.h"

#include <json/json.h>

static constexpr char LATE_BOUND_CRS[] = "lateBoundCRS";
static constexpr char EARLY_BOUND_CRS[] = "earlyBoundCRS";
static constexpr char WKT[] = "wkt";
static constexpr char COMPOUND_CRS[] = "compoundCRS";
static constexpr char HORIZONTAL_LATE_BOUND_CRS[] = "horzLateBoundCRS";
static constexpr char HORIZONTAL_EARLY_BOUND_CRS[] = "horzEarlyBoundCRS";

PersistableReferenceCrs::PersistableReferenceCrs(const std::string& crsString)
{
    const auto json = JsonLoader::fromString(crsString);
    auto members = json.getMemberNames();

    if (json.isNull() || (!json.isMember(LATE_BOUND_CRS) && !json.isMember(WKT))) {
        throw std::invalid_argument("Unable to locate late-bound CRS");
    }

    if (json.isMember(WKT)) {
        wkt_ = json[WKT].asString();
    }
    else if (json.isMember(LATE_BOUND_CRS)) {
        Json::Value lbCrs = json[LATE_BOUND_CRS];

        /*
         * The lateBoundCRS type you need to get hold of.
         *     - if you get a lateBoundCRS, you can go to the wkt directly.
         *     - for the earlyBoundCRS you need to go to the lateBoundCRS and then to the wkt.
         *     - for compoundCRS you find either a horzLateBoundCRS, which has a wkt or a horzEarlyBoundCRS, which has a lateBoundCRS with a wkt.
         */

        if (json.isMember(EARLY_BOUND_CRS)) {
            const Json::Value& ebCrs = json[EARLY_BOUND_CRS];
            lbCrs = ebCrs[LATE_BOUND_CRS];
        }
        if (!lbCrs) {
            if (json.isMember(COMPOUND_CRS)) {

                const Json::Value& compoundCrs = json[COMPOUND_CRS];
                lbCrs = compoundCrs[HORIZONTAL_LATE_BOUND_CRS];
            }
            if (!lbCrs) {
                if (json.isMember(HORIZONTAL_EARLY_BOUND_CRS)) {
                    const Json::Value& ebCrs = json[HORIZONTAL_EARLY_BOUND_CRS];
                    lbCrs = ebCrs[LATE_BOUND_CRS];
                }
            }
        }
        if (!lbCrs) {
            throw std::invalid_argument("Unable to locate late-bound CRS");
        }

        const auto& wktNode = lbCrs[WKT];
        wkt_ = wktNode.asString();
    }
}

std::string
PersistableReferenceCrs::getWkt() const
{
    return wkt_;
}

ConversionUnit
PersistableReferenceCrs::getConversionUnit() const
{
    return WktCoordinateSystem{wkt_}.getConversionUnit();
}
