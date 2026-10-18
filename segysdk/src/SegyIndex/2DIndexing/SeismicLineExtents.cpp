
/*
Copyright 2024 Schlumberger

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

#include "SeismicLineExtents.h"

namespace segysdk::segyindex::index_2d
{

struct SeismicLineExtents::Impl
{
    Impl(HorizontalExtent&& cmpExtent, VerticalExtent&& sampleExtent)
      : cmpExtent_(std::move(cmpExtent)),
        sampleExtent_(std::move(sampleExtent)) {}

    HorizontalExtent getCMPExtent() const { return cmpExtent_; }
    VerticalExtent getSampleExtent() const { return sampleExtent_; }

    HorizontalExtent cmpExtent_;
    VerticalExtent sampleExtent_;
};

SeismicLineExtents::SeismicLineExtents(HorizontalExtent cmpExtent, VerticalExtent sampleExtent)
  : impl_(std::make_shared<Impl>(std::move(cmpExtent), std::move(sampleExtent))) {}

HorizontalExtent
SeismicLineExtents::getCMPExtent() const
{
    return impl_->getCMPExtent();
}

VerticalExtent
SeismicLineExtents::getSampleExtent() const
{
    return impl_->getSampleExtent();
}

} // namespace segysdk::segyindex::index_2d
