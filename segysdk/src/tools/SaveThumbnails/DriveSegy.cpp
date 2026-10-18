/*
Copyright 2025 Schlumberger

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

#include "tools/SaveThumbnails/DriveSegy.h"

#include "Utilities/JsonUtilities.h"
#include "Utilities/TokenSourceFactory.h"
#include "segysdk/SegySessionFactory.h"
#include "tools/Keywords/Keywords.h"
#include "tools/SegySession/CreateSession.h"

namespace tool::thumbnail::segy
{

Drive::Drive(const Params& params)
  : segySession_{createSessionWithIndex(params)}
{
}

BitmapSamplesCollection
Drive::doReadSamples(const Params& params) const
{
    auto requestParams = segysdk::request::Params{}.withCorrelationId(params.correlationId_);
    const auto& layout = segySession_->getFileLayout(requestParams);
    using namespace segysdk::bulkaccess::async;
    auto readerParams = ReaderParams{requestParams}
                            .withInline(ExtentSpan{0, layout.sizeI})
                            .withXline((layout.sizeJ > 1) ? ExtentSpan{layout.sizeJ / 2, 1} : ExtentSpan{0, 1})
                            .withSamples(ExtentSpan{0, layout.sizeK});
    auto reader = segySession_->createAsyncReader(segysdk::api::ReaderCreateParams{requestParams});
    auto result = BitmapSamplesCollection{{reader->read(readerParams), layout.sizeK, keywords::inLine}};
    if (layout.sizeJ > 1) {
        readerParams
            .withInline(ExtentSpan(layout.sizeI / 2, 1))
            .withXline(ExtentSpan{0, layout.sizeJ});
        result.emplace_back(reader->read(readerParams), layout.sizeK, keywords::crossLine);
    }
    return result;
}

} // namespace tool::thumbnail::segy
