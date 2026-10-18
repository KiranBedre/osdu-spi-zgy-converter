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

#include "ByteConversion.h"

namespace segysdk::segyio::valueconversion
{

std::unique_ptr<ByteConverter>
ByteConversion::createConverter(SampleFormat valueType, bool aligned, bool swapBytes)
{
    if (aligned && swapBytes)
        return createConverter<AlignedSwapConversion>(valueType);

    else if (aligned)
        return createConverter<AlignedConversion>(valueType);

    else if (swapBytes)
        return createConverter<UnalignedSwapConversion>(valueType);

    else
        return createConverter<UnalignedConversion>(valueType);
}

} // namespace segysdk::segyio::valueconversion
