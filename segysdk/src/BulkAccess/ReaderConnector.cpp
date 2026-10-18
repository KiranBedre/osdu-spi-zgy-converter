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

#include "../BulkAccess/ReaderConnector.h"

namespace segysdk::bulkaccess
{

std::size_t
ReaderConnector::getSampleSize() const
{
    return doGetSampleSize();
}

std::size_t
ReaderConnector::getSizeI() const
{
    return doGetSizeI();
}

std::size_t
ReaderConnector::getSizeJ() const
{
    return doGetSizeJ();
}

std::size_t
ReaderConnector::getSizeK() const
{
    return doGetSizeK();
}

bool
ReaderConnector::findTrace(const std::size_t i, const std::size_t j)
{
    return doFindTrace(i, j);
}

std::size_t
ReaderConnector::getTraceOffset() const
{
    return doGetTraceOffset();
}

ByteBuffer
ReaderConnector::readSamples(const fileio::FileReadParams& params) const
{
    return doReadSamples(params);
}

Samples
ReaderConnector::convertBytesToSamples(const ByteBuffer& bytes) const
{
    return doConvertBytesToSamples(bytes);
}

} // namespace segysdk::bulkaccess
