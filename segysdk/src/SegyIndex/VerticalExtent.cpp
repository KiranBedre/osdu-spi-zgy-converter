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

#include "../SegyIndex/VerticalExtent.h"
#include "../segysdk_details/SegyException.h"
#include <cmath>

namespace segysdk::segyindex
{
struct VerticalExtent::Impl
{
    Impl(double first, double last, double step, segyio::VerticalDomain domain, segyio::VerticalUOM zUoM)
      : first_(first), last_(last), step_(step), domain_(domain), zUoM_(zUoM) {}

    double first() const { return first_; }
    double last() const { return last_; }
    double step() const { return step_; }
    segyio::VerticalDomain domain() const { return domain_; }
    segyio::VerticalUOM zUoM() const { return zUoM_; }

private:
    double first_;
    double last_;
    double step_;
    segyio::VerticalDomain domain_;
    segyio::VerticalUOM zUoM_;
};

VerticalExtent::VerticalExtent(double first, double last, double step)
  : VerticalExtent(first, last, step, segyio::VerticalDomain::Time, segyio::VerticalUOM::seconds) {}

VerticalExtent::VerticalExtent(double first, double last, double step, segyio::VerticalDomain domain, segyio::VerticalUOM zUoM)
  : impl_(std::make_shared<Impl>(first, last, step, domain, zUoM)) {}

bool
VerticalExtent::operator==(const VerticalExtent& other) const
{
    return first() == other.first() && count() == other.count() && step() == other.step() && domain() == other.domain() && zUoM() == other.zUoM();
}

bool
VerticalExtent::operator!=(const VerticalExtent& other) const
{
    return !(*this == other);
}

bool
VerticalExtent::almostEqual(const VerticalExtent& other, const double tolerance) const
{
    return std::abs(first() - other.first()) < tolerance && count() == other.count() && std::abs(step() - other.step()) < tolerance && domain() == other.domain() && zUoM() == other.zUoM();
}

double
VerticalExtent::first() const
{
    return impl_->first();
}

double
VerticalExtent::last() const
{
    return impl_->last();
}

double
VerticalExtent::step() const
{
    return impl_->step();
}

segyio::VerticalDomain
VerticalExtent::domain() const
{
    return impl_->domain();
}

segyio::VerticalUOM
VerticalExtent::zUoM() const
{
    return impl_->zUoM();
}

int
VerticalExtent::count() const
{
    if (last() - first() < step()) {
        return 1;
    }

    //
    // Note:
    //
    // Here is the range [ A, B ] special treatment.
    //
    // Number of samples is estimated by the following formula:
    //    N = ( B - A ) / h
    // where
    //    B - last value in the range [ A, B ]
    //    A - first value in the range [ A, B ]
    //    h - step between the samples
    //
    // E.g. for [ 0.00, 35.04 ] range with the step of 0.04 gives the number of samples of 876 pcs.
    //    ( 35.04 - 0.00 ) / 0.04 = 876
    //

    // Workaround to avoid silly numerical precision issues:
    // The below values have been taken as ms integer values and converted to seconds
    // In this process, numerical inaccuracy has been introduced, so we have to get the proper integer values back
    // We assume the error is tiny, but enough to land on the wrong integer if it is very close to the proper
    // value, e.g. increment = 3.99999 instead of 4.0 or bottom = 4003.9999999
    // Note this is NOT a problem if the error makes the value slightly larger than the expected integer as the static_cast
    // to int will fix that.

    auto roundUp = [](double value) {
        constexpr double tolerance = 0.00001;
        const double multipliedValue = value * 1000;
        if (static_cast<int>(multipliedValue) < static_cast<int>(multipliedValue + tolerance)) {
            return multipliedValue + tolerance;
        }
        return multipliedValue;
    };

    const double top = roundUp(first());
    const double bottom = roundUp(last());
    const double increment = roundUp(step());

    if (std::abs(increment) > 0) {
        // We have got top, bottom and increment represented as milli-second values at this stage.
        // However, beware the increment:
        // For high-frequency data, this might still be a value < 1 if the sample rate is < 1 milli-second.
        // E.g. for site survey data, sample rate can be 48 (or even 32!) micro-seconds, and will still be 0.032 at this stage.

        const int result = std::abs(increment) < 1.0
                               ? static_cast<int>((bottom - top) / increment)
                               : (static_cast<int>(bottom) - static_cast<int>(top)) / static_cast<int>(increment);

        return result;
    }

    throw segysdk::SegyInvalidArgumentException("VerticalExtent: Sample Increment cannot be 0");
}

} // namespace segysdk::segyindex
