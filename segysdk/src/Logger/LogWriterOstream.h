/*
Copyright 2022 Schlumberger

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

#include "../Logger/LogWriterBase.h"

#include <string>
#include <ostream>
#include <memory>

namespace segysdk::logger
{

class SEGYSDK_DLL_PUBLIC LogWriterOstream : public LogWriterBase
{
public:
    LogWriterOstream(std::shared_ptr<std::ostream> outStream);
    virtual ~LogWriterOstream() override;

    LogWriterOstream(const LogWriterOstream&) = delete;
    LogWriterOstream& operator=(const LogWriterOstream&) = delete;

    virtual void write(const LogWriterBase::PoolMessages& messages) override;

private:
    std::shared_ptr<std::ostream> outStream_;
};

} // namespace segysdk::logger
