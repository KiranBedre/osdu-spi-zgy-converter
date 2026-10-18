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

#include "tools/SaveThumbnails/DriveZgy.h"

#include "Utilities/TokenSourceFactory.h"
#include "tools/Keywords/Keywords.h"

#include <openzgy/api.h>
#include <openzgy/iocontext.h>

#include <iostream>

namespace tool::thumbnail::zgy
{

using Voxel = OpenZGY::IZgyMeta::size3i_t;

class Reader
{
public:
    explicit Reader(const Params& params);
    Voxel getSizes() const;
    segysdk::Samples readSamples(const Voxel& start, const Voxel& size) const;

private:
    std::shared_ptr<OpenZGY::IZgyReader> reader_;
    std::unique_ptr<OpenZGY::IZgyReader, std::function<void(OpenZGY::IZgyReader*)>> closer_;
};

BitmapSamplesCollection
Drive::doReadSamples(const Params& params) const
{
    const auto reader = Reader{params};
    const auto& [numInlines, numCrosslines, numVerticals] = reader.getSizes();
    return {
        {
            reader.readSamples(Voxel{numInlines / 2, 0, 0}, Voxel{1, numCrosslines, numVerticals}),
            numVerticals,
            keywords::inLine,
        },
        {
            reader.readSamples(Voxel{0, numCrosslines / 2, 0}, Voxel{numInlines, 1, numVerticals}),
            numVerticals,
            keywords::crossLine,
        },
        {
            reader.readSamples(Voxel{0, 0, numVerticals / 2}, Voxel{numInlines, numCrosslines, 1}),
            numCrosslines,
            keywords::zSlice,
        },
    };
}

static auto
createReader(const Params& params)
{
    auto context = OpenZGY::SeismicStoreIOContext{};
    context.sdapikey(keywords::unused);
    context.sdtoken(segysdk::utilities::TokenSourceFactory::getTokenSource()->token());
    context.sdurl(params.sdUrl_);
    return OpenZGY::IZgyReader::open(params.fileSource_, &context);
}

Reader::Reader(const Params& params)
  : reader_{createReader(params)},
    closer_{
        reader_.get(),
        [](OpenZGY::IZgyReader* reader) {
            try {
                reader->close();
            }
            catch (const OpenZGY::Errors::ZgyError& error) {
                std::cerr << "failed to close reader: " << error.what() << '\n';
            }
        }}
{
}

Voxel
Reader::getSizes() const
{
    return reader_->size();
}

segysdk::Samples
Reader::readSamples(const Voxel& start, const Voxel& size) const
{
    auto result = segysdk::Samples(size[0] * size[1] * size[2]);
    reader_->read(start, size, result.data());
    return result;
}

} // namespace tool::thumbnail::zgy
