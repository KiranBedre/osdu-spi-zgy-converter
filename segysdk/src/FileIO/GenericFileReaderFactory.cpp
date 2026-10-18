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

#include "../FileIO/GenericFileReaderFactory.h"

#include "../FileIO/GenericFileReaderImpl.h"
#include "../Utilities/TokenSourceFactory.h"

#include <memory>
#include <optional>
#include <utility>

namespace segysdk::api
{

struct GenericFileReaderFactory::Impl
{
    explicit Impl(std::string fileName)
      : fileName_(std::move(fileName)),
        sdms_url_{},
        api_key_{},
        tokenSource_{utilities::TokenSourceFactory::getTokenSource()}
    {
    }

    Impl(std::string fileName,
         std::string sdms_url,
         std::string api_key,
         std::unique_ptr<segysdk::utilities::auth::TokenSource> tokenSource)
      : fileName_(std::move(fileName)),
        sdms_url_(std::move(sdms_url)),
        api_key_(std::move(api_key)),
        tokenSource_(std::move(tokenSource))
    {
    }

    std::shared_ptr<segysdk::fileio::GenericFileReader>
    CreateGenericFileReader()
    {
        if (sdms_url_.empty() && api_key_.empty()) {
            // Don't check for tokenSource_ being empty, it is quite likely it won't be ...
            return std::make_shared<segysdk::fileio::GenericFileReaderImpl>(fileName_);
        }

        return std::make_shared<segysdk::fileio::GenericFileReaderImpl>(fileName_, sdms_url_, api_key_, utilities::TokenSourceFactory::getTokenSource());
    }

private:
    std::string fileName_;
    std::string sdms_url_;
    std::string api_key_;
    std::unique_ptr<segysdk::utilities::auth::TokenSource> tokenSource_;
};

GenericFileReaderFactory::GenericFileReaderFactory(const std::string& fileName)
  : impl_(std::make_shared<Impl>(fileName)) {}

GenericFileReaderFactory::GenericFileReaderFactory(const std::string& fileName,
                                                   const std::string& sdms_url,
                                                   const std::string& api_key,
                                                   std::unique_ptr<segysdk::utilities::auth::TokenSource> tokenSource)
  : impl_(std::make_shared<Impl>(fileName,
                                 sdms_url,
                                 api_key,
                                 std::move(tokenSource))) {}

std::shared_ptr<segysdk::fileio::GenericFileReader>
GenericFileReaderFactory::CreateGenericFileReader() const
{
    return impl_->CreateGenericFileReader();
}

} // namespace segysdk::api
