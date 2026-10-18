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

#include "SafeTime.h"

#include <SegySdkProject.h>

#include <stdexcept>

namespace
{

#ifdef HAVE_MSVC_GMTIME_S

void
safe_gmtime_impl(std::time_t& timer, std::tm& buf)
{
    const errno_t errorCode = gmtime_s(&buf, &timer);
    if (errorCode != 0) {
        throw std::invalid_argument("Invalid arguments for safe_gmtime.");
    }
}

#elif defined(HAVE_GMTIME_R)

void
safe_gmtime_impl(std::time_t& timer, std::tm& buf)
{
    const auto result = gmtime_r(&timer, &buf);
    if (result == nullptr) {
        throw std::invalid_argument("Invalid arguments for safe_gmtime.");
    }
}

#else
#error Neither gmtime_r nor gmtime_s are available on this platform.
#endif


} // namespace

namespace segysdk::utilities
{

void
safe_gmtime(std::time_t& timer, std::tm& buf)
{
    safe_gmtime_impl(timer, buf);
}

} // namespace segysdk::utilities
