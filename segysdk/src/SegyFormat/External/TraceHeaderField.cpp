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

#include "TraceHeaderField.h"

#include <utility>

namespace segysdk::segyformat::external
{

TraceHeaderField::TraceHeaderField()
{}

TraceHeaderField::TraceHeaderField(const std::string& key, const std::string& originalKey, const DataFormat& format, const int length, const int offset)
  : SegyHeaderField(key, originalKey, format, static_cast<unsigned short>(length), offset)
{}

TraceHeaderField::TraceHeaderField(const std::string& key, const std::string& originalKey, const DataFormat& format, const int length, const int offset, std::string uom)
  : SegyHeaderField(key, originalKey, format, static_cast<unsigned short>(length), offset), 
    uom_(std::move(uom))
{
    if (uom_ == "")
        uom_ = UoMStringConsts::Unitless;
}

TraceHeaderField::TraceHeaderField(const std::string& key, const std::string& originalKey, const DataFormat& format, const int length, const int offset, ScalarIndicator scalarIndicator, std::string scalarField, std::string uom)
  : SegyHeaderField(key, originalKey, format, static_cast<unsigned short>(length), offset),
    scalarIndicator_(scalarIndicator), scalarAttribute_(std::move(scalarField)), uom_(std::move(uom))
{
    if (uom_ == "")
        uom_ = UoMStringConsts::Unitless;
}

TraceHeaderField::TraceHeaderField(const std::string& key, const std::string& originalKey, const DataFormat& format, const int length, const int offset, ScalarIndicator scalarIndicator, std::string scalarField, const int scalarOverrideValue, std::string uom)
  : SegyHeaderField(key, originalKey, format, static_cast<unsigned short>(length), offset),
    scalarIndicator_(scalarIndicator), scalarAttribute_(std::move(scalarField)), scalarOverrideValue_(scalarOverrideValue),
    uom_(std::move(uom))
{
    if (uom_ == "")
        uom_ = UoMStringConsts::Unitless;
}

} // namespace segysdk::segyformat::external
