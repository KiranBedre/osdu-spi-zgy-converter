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

#include "../segysdk_details/SegyException.h"

#include "../Logger/Logger.h"

#include <iostream>
#include <string>

namespace segysdk
{

std::string
SegyException::makestr(std::error_code errcode, std::string msg)
{
    if (!msg.empty()) {
        msg.append(": ");
    }

    msg.append(errcode.message());
    return msg;
}

std::string
SegyException::makestr(const int errval, const std::error_category& errcat, std::string msg)
{
    if (!msg.empty()) {
        msg.append(": ");
    }

    msg.append(errcat.message(errval));
    return msg;
}

SegyException::SegyException(const char* msg)
  : parent_(msg)
{
  SEGYLOG_REPORT_ERROR(what());
}

SegyException::SegyException(const std::string& msg)
  : parent_(msg)
{
  SEGYLOG_REPORT_ERROR(what());
}

SegyException::SegyException(const std::error_code& errcode, const std::string& msg)
  : SegyException(makestr(errcode, msg))
{
  SEGYLOG_REPORT_ERROR(what());
}

SegyException::SegyException(const std::error_code& errcode, const char* msg)
  : SegyException(makestr(errcode, msg))
{
  SEGYLOG_REPORT_ERROR(what());
}

SegyException::SegyException(int errval, const std::error_category& errcat, const std::string& msg)
  : SegyException(std::error_code(errval, errcat), msg)
{
  SEGYLOG_REPORT_ERROR(what());
}

SegyException::SegyException(int errval, const std::error_category& errcat, const char* msg)
  : SegyException(std::error_code(errval, errcat), msg)
{
  SEGYLOG_REPORT_ERROR(what());
}

SegyException::~SegyException()
{
}

} // namespace segysdk
