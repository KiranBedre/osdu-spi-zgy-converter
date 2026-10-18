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

#include "SegyInputParameters.h"

#include "SegyInputGeometry.h"

#include "../Utilities/Environment.h"

using namespace segysdk;

std::shared_ptr<segysdk::segyindex::SegyIndex>
SegyInputParameters::loadOrGenerateIndex() const
{
    return loadOrGenerateIndex(segySession_);
}

std::shared_ptr<segyindex::SegyIndex>
SegyInputParameters::loadOrGenerateIndex(api::SegySession& segySession) const
{
    auto index = segySession.getIndex();

    if (index == nullptr || !index->isValid() || !index->hasIndexFile(true)) {
        if (segysdk::utilities::Environment::instance().shouldGenerateIndex()) {
            segySession_.generateIndexFile();
        }
        else {
            throw std::invalid_argument("Missing index file for " + segySession.getSegyFileName());
        }
    }

    return segySession.getIndex();
}

SegyInputParameters::SegyInputParameters(segysdk::api::SegySession& segySession)
  : segySession_(segySession),
    geometry_(SegyInputGeometry(loadOrGenerateIndex(segySession_)->getGeometry3DExtents())),
    sampleFormat_(segySession_.getSampleFormat())
{
}
