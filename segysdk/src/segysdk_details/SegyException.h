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

/** @file
 * @brief SegyException.h contains the base class of all SEG-Y SDK exceptions.
 * @details SegyException.h is necessary to catch SEG-Y SDK exceptions as it declares the SegyException, the base class of all SEG-Y SDK exceptions.
 *
 */

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include <stdexcept>
#include <system_error>

namespace segysdk
{
/**
* @class SegyException
*
* @brief Used as base abstract class of all SEG-Y SDK exceptions.
*
* @details
*  Abstract class to be base of all SEG-Y SDK exceptions. Inherits from std::runtime_error.
*
**/

class SEGYSDK_DLL_PUBLIC SegyException : public std::runtime_error
{
    using parent_ = std::runtime_error;

    static std::string makestr(std::error_code errcode, std::string msg);
    static std::string makestr(int errval, const std::error_category& errcat, std::string msg);

public:
    /**
   * @brief Constructor - Standard constructor for the SegyException class.
   *
   * @param  msg  char*  old c-style character holding error message for exception
   *
   **/
    explicit SegyException(const char* msg);

    /**
    * @brief Constructor - Standard constructor for the SegyException class.
    *
    * @param  msg  string&  std::string const holding error message for exception
    *
    **/
    explicit SegyException(const std::string& msg);

    /**
    * @brief Constructor - Standard constructor for the SegyException class.
    *
    * @param  errcode  std::error_code&  specific system error code to use
    * @param  msg  string&  std::string const holding error message for exception
    *
    **/
    SegyException(const std::error_code& errcode, const std::string& msg);

    /**
    * @brief Constructor - Standard constructor for the SegyException class.
    *
    * @param  errcode  std::error_code&  specific system error code to use
    * @param  msg  char*  old c-style character holding error message for exception
    *
    **/
    SegyException(const std::error_code& errcode, const char* msg);

    /**
    * @brief Constructor - Standard constructor for the SegyException class.
    *
    * @param  errval  int  error value
    * @param  errcat  std::error_category&  error category
    * @param  msg  char*  old c-style character holding error message for exception
    *
    **/
    SegyException(int errval, const std::error_category& errcat, const std::string& msg);

    /**
    * @brief Constructor - Standard constructor for the SegyException class.
    *
    * @param  errval  int  error value
    * @param  errcat  std::error_category&  error category
    * @param  msg  string&  std::string const holding error message for exception
    *
    **/
    SegyException(int errval, const std::error_category& errcat, const char* msg);

protected:
    /**
   * @brief destructor - Standard destructor for the SegyException class.
   **/
    ~SegyException() override;
};

/**
  * @class SegyInvalidArgumentException
  *
  * @brief Indicates argument errors detected inside the SEG-Y SDK.
  *
  **/
class SEGYSDK_DLL_PUBLIC SegyInvalidArgumentException : public SegyException
{
    using parent_ = SegyException;

public:
    using parent_::parent_;
    using parent_::operator=;
};

/**
  * @class SegyNotImplementedException
  *
  * @brief Indicates not implemented call-able invocation detected inside the SEG-Y SDK.
  *
  **/
class SEGYSDK_DLL_PUBLIC SegyNotImplementedException : public SegyException
{
    using parent_ = SegyException;

public:
    using parent_::parent_;
    using parent_::operator=;
};

} // namespace segysdk
