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

#include <string>

namespace segysdk::segyio
{
/**
  * @file HeaderOverride.h
  *
  * @details HeaderOverride is the class holding the override definitions for binary and trace header fields.
  *
  */

// TODO: has "implementation"

/**
  * @class HeaderOverride
  *
  * @brief Class to hold the override definition for a specific header parameter.
  *
  **/
class SEGYSDK_DLL_PUBLIC HeaderOverride
{
private:
    std::string headerId_;
    float value_;

public:
    
    HeaderOverride & operator=(const HeaderOverride&) = delete;
        

    /**
  * @brief standard constructor - creates a new instance of the HeaderOverride class.
  *
  * @param headerId  string with the name of the header attribute to be overridden.
  * @param value     float value with the requested constant override value to be used for the specified header literal.
  * 
  **/
    HeaderOverride(std::string headerId, float value);

    /**
  * @brief standard destructor.
  **/
    virtual ~HeaderOverride();

    /**
  * @brief Gets the constant override value specified for the current HeaderOverride instance.
  *
  * @returns float overrride value for the current HeaderOverride instance.
  *
  **/
    float getValue() const;

    /**
  * @brief Sets the constant override value to be used for the current HeaderOverride instance.
  *
  * @param value   float overrride value for the current HeaderOverride instance.
  *
  **/
    void setValue(float value);

    /**
  * @brief Gets the headerid value specified for the current HeaderOverride instance.
  *
  * @returns string  holding the header literal for the current HeaderOverride instance.
  *
  **/
    std::string getHeaderId() const;
};

} // namespace segysdk::segyio
