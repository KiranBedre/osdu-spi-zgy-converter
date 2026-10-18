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

#include "../SegyIndex/FileLayoutSpecification.h"

#include <cmath>

namespace segysdk::segyindex
{

FileLayoutSpecification::FileLayoutSpecification()
{
    /* EMPTY */
}

FileLayoutSpecification::FileLayoutSpecification(FileLayoutSpecification const& other)
{
    *this = other;
}

FileLayoutSpecification&
FileLayoutSpecification::operator=(FileLayoutSpecification const& other)
{
    JExtent = other.JExtent;
    IExtent = other.IExtent;
    sampleExtent = other.sampleExtent;
    isRegular = other.isRegular;
    isInlineOrdered = other.isInlineOrdered;

    return *this;
}

FileLayoutSpecification::FileLayoutSpecification(FileLayoutSpecification&& other)
{
    *this = std::move(other);
}

FileLayoutSpecification&
FileLayoutSpecification::operator=(FileLayoutSpecification&& other)
{
    std::swap(JExtent, other.JExtent);
    std::swap(IExtent, other.IExtent);
    std::swap(sampleExtent, other.sampleExtent);
    std::swap(isRegular, other.isRegular);
    std::swap(isInlineOrdered, other.isInlineOrdered);

    return *this;
}

bool
FileLayoutSpecification::operator==(const FileLayoutSpecification& other) const
{
    return IExtent == other.IExtent
           and JExtent == other.JExtent
           and sampleExtent == other.sampleExtent
           and isInlineOrdered == other.isInlineOrdered
           and isRegular == other.isRegular;
}

bool
FileLayoutSpecification::operator!=(const FileLayoutSpecification& other) const
{
    return not(*this == other);
}

bool
FileLayoutSpecification::almostEqual(const FileLayoutSpecification& other, double angleTolerance) const
{
    return IExtent == other.IExtent
           and JExtent == other.JExtent
           and sampleExtent.almostEqual(other.sampleExtent, angleTolerance)
           and isInlineOrdered == other.isInlineOrdered
           and isRegular == other.isRegular;
}

bool
FileLayoutSpecification::IsDefined() const
{
    return JExtent.last() - JExtent.first() != 0
           and IExtent.last() - IExtent.first() != 0
           and std::fabs(sampleExtent.last() - sampleExtent.first()) > tolerance_;
}

} // namespace segysdk::segyindex
