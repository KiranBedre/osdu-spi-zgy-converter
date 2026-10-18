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

#include "../FileIO/FileReference.h"

#include <utility>

namespace segysdk::fileio
{

struct FileReference::Impl
{
    Impl(std::string fileName, FileReaderOptions readerOptions)
      : fileName_(std::move(fileName)), readerOptions_(std::move(readerOptions)) 
    {}

    Impl(const Impl& other)
     : fileName_(other.fileName_)
     , readerOptions_(other.readerOptions_)
    {}

    Impl(Impl&& other)
    : fileName_(std::move(other.fileName_))
    , readerOptions_(std::move(other.readerOptions_))
   {}

    const std::string
    fileName() const
    {
        return fileName_;
    }

    FileReaderOptions
    readerOptions() const
    {
        return readerOptions_;
    }

    /**
   * @brief Create a FileReference with same parameters but different path.
   *
   * @warning The caller must ensure that the parameters are also valid for the new path.
   */

private:
    const std::string fileName_;
    const FileReaderOptions readerOptions_;
};

std::string
segysdk::fileio::FileReference::fileName() const
{
    return impl_->fileName();
}

FileReaderOptions
FileReference::readerOptions() const
{
    return impl_->readerOptions();
}

FileReference::FileReference(std::string fileName, FileReaderOptions readerOptions)
  : impl_(std::make_shared<Impl>(fileName, readerOptions))
{
}


} // namespace segysdk::fileio
