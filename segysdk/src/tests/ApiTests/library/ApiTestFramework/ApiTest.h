/*
Copyright 2023 Schlumberger

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

#include "ApiTestParamGenerator.h"
#include <gtest/gtest.h>

#define TEST_API(classname, configFile)          \
    TEST_P(classname, classname)                 \
    {                                            \
        check<classname>(&classname::run, this); \
    }                                            \
                                                 \
    INSTANTIATE_TEST_SUITE_P(                    \
        classname##Suite,                        \
        classname,                               \
        ::testing::ValuesIn(ApiTestParamGenerator::get(#classname, ApiTestParamGenerator::getConfigFilePath(configFile))));
