/*
Copyright 2022 Schlumberger

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

#include "IndexingReturnStatus.h"

#include <type_traits>


namespace SegyIndexing::Utilities
{

template<typename ENUM>
constexpr std::underlying_type_t<ENUM>
getEnumValue(ENUM value)
{
    return static_cast<std::underlying_type_t<ENUM>>(value);
}

} // namespace SegyIndexing::Utilities

int
SegyIndexer::Utilities::SegyIndexGeneratorStatus::GetCodeFromEnum(const IndexingReturnStatus enumValue)
{
    return SegyIndexing::Utilities::getEnumValue<IndexingReturnStatus>(enumValue);
}

SegyIndexer::Utilities::IndexingReturnStatus
SegyIndexer::Utilities::SegyIndexGeneratorStatus::GetEnumFromCode(int status)
{
    switch (status) {

    case SegyIndexing::Utilities::getEnumValue(IndexingReturnStatus::Success):
        return IndexingReturnStatus::Success;
    case SegyIndexing::Utilities::getEnumValue(IndexingReturnStatus::HelpInfoDisplayed):
        return IndexingReturnStatus::HelpInfoDisplayed;

    case SegyIndexing::Utilities::getEnumValue(IndexingReturnStatus::NoValidSession):
        return IndexingReturnStatus::NoValidSession;
    case SegyIndexing::Utilities::getEnumValue(IndexingReturnStatus::InvalidToken):
        return IndexingReturnStatus::InvalidToken;

    case SegyIndexing::Utilities::getEnumValue(IndexingReturnStatus::TokenCannotBeRetrieved):
        return IndexingReturnStatus::TokenCannotBeRetrieved;
    case SegyIndexing::Utilities::getEnumValue(IndexingReturnStatus::SAuth2EnvironmentNotSet):
        return IndexingReturnStatus::SAuth2EnvironmentNotSet;
    case SegyIndexing::Utilities::getEnumValue(IndexingReturnStatus::CouldNotGetIndex):
        return IndexingReturnStatus::CouldNotGetIndex;
    case SegyIndexing::Utilities::getEnumValue(IndexingReturnStatus::NavigationExtractionFailed):
        return IndexingReturnStatus::NavigationExtractionFailed;
    case SegyIndexing::Utilities::getEnumValue(IndexingReturnStatus::InvalidArgument):
        return IndexingReturnStatus::InvalidArgument;
    case SegyIndexing::Utilities::getEnumValue(IndexingReturnStatus::SEGYError):
        return IndexingReturnStatus::SEGYError;
    case SegyIndexing::Utilities::getEnumValue(IndexingReturnStatus::RuntimeError):
        return IndexingReturnStatus::RuntimeError;
    case SegyIndexing::Utilities::getEnumValue(IndexingReturnStatus::OperationFailed):
        return IndexingReturnStatus::OperationFailed;

    default: 
        return IndexingReturnStatus::UnspecifiedError;
    }
}
