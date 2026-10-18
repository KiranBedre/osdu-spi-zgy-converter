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

#include "SegyHeaderField.h"

#include <utility>

namespace segysdk::segyformat::external
{

SegyHeaderField::SegyHeaderField()
{
}

SegyHeaderField::SegyHeaderField(const std::string& key, const DataFormat& format, const unsigned short length, const int offset)
  : key_(key), original_key_(key), format_(format), length_(length), offset_(offset)
{
}

SegyHeaderField::SegyHeaderField(const std::string& key, std::string originalKey, const DataFormat& format, const unsigned short length, const int offset)
  : key_(key), original_key_(std::move(originalKey)), format_(format), length_(length), offset_(offset)
{
    if (original_key_ == "")
        original_key_ = key;
}

SegyHeaderField::~SegyHeaderField()
{
}

void
SegyHeaderField::updateDefinitions(const SegyHeaderField& hdr)
{
    key_ = hdr.key_;
    original_key_ = hdr.original_key_;
    format_ = hdr.format_;
    offset_ = hdr.offset_;
    length_ = hdr.length_;
}

} // namespace segysdk::segyformat::external
