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

#include "../SegyIndex/HorizontalExtent.h"

namespace segysdk::segyindex
{

struct HorizontalExtent::Impl
{
    Impl(int first, int last, int step)
      : first_(first), last_(last), step_(step) {}

    int first() const { return first_; }
    int last() const { return last_; }
    int step() const { return step_; }

private:
    int first_;
    int last_;
    int step_;
};

HorizontalExtent::HorizontalExtent(int first, int last, int step)
  : impl_(std::make_shared<Impl>(first, last, step)) {}

bool
HorizontalExtent::operator==(const HorizontalExtent& other) const
{
    return first() == other.first() && last() == other.last() && step() == other.step() && count() == other.count();
}

bool
HorizontalExtent::operator!=(const HorizontalExtent& other) const
{
    return !(*this == other);
}

int
HorizontalExtent::first() const
{
    return impl_->first();
}

int
HorizontalExtent::last() const
{
    return impl_->last();
}

int
HorizontalExtent::step() const
{
    return impl_->step();
}

int
HorizontalExtent::count() const
{
    return (last() - first()) / step() + 1;
}

int
HorizontalExtent::gaps() const
{
    return (last() - first()) / step();
}

} // namespace segysdk::segyindex
