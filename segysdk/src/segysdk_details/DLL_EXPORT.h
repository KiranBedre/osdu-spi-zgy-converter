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

#if defined _WIN32 || defined __CYGWIN__
    #ifdef SEGYSDK_NO_DLL_PUBLIC
        #define SEGYSDK_DLL_PUBLIC
    #else
        #ifdef SEGYSDK_BUILDING_DLL
            #ifdef __GNUC__
                #define SEGYSDK_DLL_PUBLIC __attribute__((dllexport))
            #else
                #define SEGYSDK_DLL_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
            #endif
        #else
            #ifdef __GNUC__
                #define SEGYSDK_DLL_PUBLIC __attribute__((dllimport))
            #else
                #define SEGYSDK_DLL_PUBLIC __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
            #endif
        #endif
    #endif

    #define SEGYSDK_DLL_LOCAL
#else
    #if __GNUC__ >= 4
        #ifdef SEGYSDK_NO_DLL_PUBLIC
            #define SEGYSDK_DLL_PUBLIC
        #else
            #define SEGYSDK_DLL_PUBLIC __attribute__((visibility("default")))
        #endif

        #define SEGYSDK_DLL_LOCAL __attribute__((visibility("hidden")))
    #else
        #define SEGYSDK_DLL_PUBLIC
        #define SEGYSDK_DLL_LOCAL
    #endif
#endif
