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

#include "SegyServiceSessionFactory.h"

#include "../Logger/Logger.h"
#include "../segysdk/SegyServiceSessionImpl.h"

namespace segysdk::api
{
struct SegyServiceSessionFactory::Impl
{
    explicit Impl(std::string seismicTraceData)
      : seismicTraceData_(std::move(seismicTraceData))
    {
    }

    std::unique_ptr<SegyServiceSession>
    createSegyServiceSession() const
    {
        return std::make_unique<SegyServiceSessionImpl>(seismicTraceData_);
    }

private:
    std::string seismicTraceData_;
};

SegyServiceSessionFactory::SegyServiceSessionFactory(std::string seismicTraceData)
  : impl_(std::make_shared<Impl>(seismicTraceData))
{
}

std::unique_ptr<SegyServiceSession>
SegyServiceSessionFactory::createSegyServiceSession() const
{
    return impl_->createSegyServiceSession();
}
} // namespace segysdk::api
