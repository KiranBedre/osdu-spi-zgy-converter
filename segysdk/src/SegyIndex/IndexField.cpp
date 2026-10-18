/*
Copyright 2021-2022 Schlumberger

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

#include "../SegyIndex/IndexField.h"

namespace segysdk::segyindex {
IndexField::IndexField(size_t segyFileNo, int inlineNo, int crosslineNo, size_t offset) noexcept :
segyFileNumber_(segyFileNo), inlineNo_(inlineNo), crosslineNo_(crosslineNo), offset_(offset)
{
    /*EMPTY*/
}

uint16_t
IndexField::getSegyFileNumber() const noexcept
{
    return static_cast<uint16_t>(segyFileNumber_);
}

int
IndexField::getInline() const noexcept
{
    return inlineNo_;
}

int
IndexField::getCrossline() const noexcept
{
    return crosslineNo_;
}

size_t
IndexField::getOffset() const noexcept
{
    return offset_;
}

bool
IndexField::operator==(IndexField const& other) const
{
    return getSegyFileNumber() == other.getSegyFileNumber()
           && getInline() == other.getInline()
           && getCrossline() == other.getCrossline()
           && getOffset() == other.getOffset();
}

bool
IndexField::operator!=(IndexField const& other) const
{
    return not operator==(other);
}

}
