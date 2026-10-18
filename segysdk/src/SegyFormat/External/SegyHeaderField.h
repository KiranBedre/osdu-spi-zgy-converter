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

#include <memory>
#include <string>

namespace segysdk::segyformat::external
{

enum class SEGYSDK_DLL_PUBLIC DataFormat
{
    Integer = 0,
    UnsignedInt,
    FloatIBM,
    FloatIEEE
};

class SEGYSDK_DLL_PUBLIC SegyHeaderField
{
private:
    std::string key_;
    std::string original_key_;
    DataFormat format_;
    unsigned short length_;
    int offset_;

public:
    SegyHeaderField();
    SegyHeaderField(const std::string& key, const DataFormat& format, const unsigned short length, const int offset);
    SegyHeaderField(const std::string& key, std::string originalKey, const DataFormat& format, const unsigned short length, const int offset);
    virtual ~SegyHeaderField();

public:
    virtual bool IsBinaryHeader() const { return false; }
    virtual bool IsTraceHeader() const { return false; }

    std::string getKey() const { return key_; }
    std::string getOriginalKey() const { return original_key_; }
    DataFormat getNumericFormat() const { return format_; }
    int getOffset() const { return offset_; }
    unsigned short getLength() const { return length_; }

    void updateDefinitions(const SegyHeaderField& hdr);

    // overloaded < operator
    bool operator<(const SegyHeaderField& h) const
    {
        return (offset_ < h.offset_);
    }
    bool operator<(const std::shared_ptr<SegyHeaderField>& h) const
    {
        return (offset_ < h->offset_);
    }
};

} // namespace segysdk::segyformat::external
