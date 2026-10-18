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

#include "SeismicCubeExtents.h"

namespace segysdk::segyindex
{

struct SeismicCubeExtents::Impl
{
    Impl(HorizontalExtent&& iVectorExtent, HorizontalExtent&& jVectorExtent, VerticalExtent&& sampleExtent)
      : iVectorExtent_(std::move(iVectorExtent)),
        jVectorExtent_(std::move(jVectorExtent)),
        sampleExtent_(std::move(sampleExtent)) {}

    HorizontalExtent getIVectorExtent() const { return iVectorExtent_; }
    HorizontalExtent getJVectorExtent() const { return jVectorExtent_; }
    VerticalExtent getSampleExtent() const { return sampleExtent_; }

    HorizontalExtent iVectorExtent_;
    HorizontalExtent jVectorExtent_;
    VerticalExtent sampleExtent_;
};

SeismicCubeExtents::SeismicCubeExtents(HorizontalExtent iVectorExtent, HorizontalExtent jVectorExtent, VerticalExtent sampleExtent)
  : impl_(std::make_shared<Impl>(std::move(iVectorExtent), std::move(jVectorExtent), std::move(sampleExtent))) {}

HorizontalExtent
SeismicCubeExtents::getIVectorExtent() const
{
    return impl_->getIVectorExtent();
}

HorizontalExtent
SeismicCubeExtents::getJVectorExtent() const
{
    return impl_->getJVectorExtent();
}

VerticalExtent
SeismicCubeExtents::getSampleExtent() const
{
    return impl_->getSampleExtent();
}

} // namespace segysdk::segyindex
