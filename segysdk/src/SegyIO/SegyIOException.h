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

#include "../segysdk_details/SegyException.h"

using segysdk::SegyException;

namespace segysdk::segyio
{
/**
* @class SegyIOException
*
* @brief Used as base class of all SEG-Y SDK exceptions of IO origin.
*
* @details
  Class to be base of all SEG-Y SDK IO exceptions. Inherits from SegyException.
*
**/

class SEGYSDK_DLL_PUBLIC SegyIOException : public SegyException
{
    using parent_ = SegyException;

public:
    using parent_::parent_;
    using parent_::operator=;

protected:
    ~SegyIOException() {}
};

class SEGYSDK_DLL_PUBLIC SegyFormatException : public SegyException
{
    using parent_ = SegyException;

public:
    using parent_::parent_;
    using parent_::operator=;
};

class SEGYSDK_DLL_PUBLIC SegyMemoryException : public SegyException
{
    using parent_ = SegyException;

public:
    using parent_::parent_;
    using parent_::operator=;
};

class SEGYSDK_DLL_PUBLIC SegyRuntimeException : public SegyException
{
    using parent_ = SegyException;

public:
    using parent_::parent_;
    using parent_::operator=;
};

class SEGYSDK_DLL_PUBLIC SegyFileIOException : public SegyException
{
    using parent_ = SegyException;

public:
    using parent_::parent_;
    using parent_::operator=;
};

class SEGYSDK_DLL_PUBLIC SegySDException final : public SegyFileIOException
{
    using parent_ = SegyFileIOException;

public:
    using parent_::parent_;
    using parent_::operator=;

protected:
    ~SegySDException() override
    {}
};

class SEGYSDK_DLL_PUBLIC SegySDIOException final : public SegyFileIOException
{
    using parent_ = SegyFileIOException;

public:
    using parent_::parent_;
    using parent_::operator=;
};

class SEGYSDK_DLL_PUBLIC SegySDAuthException : public SegyFileIOException
{
    using parent_ = SegyFileIOException;

public:
    using parent_::parent_;
    using parent_::operator=;
};

} // namespace segysdk::segyio
