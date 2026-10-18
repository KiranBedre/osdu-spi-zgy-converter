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

#pragma once

namespace SegyIndexer::Utilities
{

/**
 * @enum IndexingReturnStatus
 *
 * @brief enum class, return status codes from the SegyIndex generation process.
 *
 **/
enum class IndexingReturnStatus
{

    Success = 0,                 /**< App exit after successful index generation */
    RuntimeError = 1,            /**< App exit because runtime error occurred during the generator process */
    InvalidArgument = 2,         /**< App exit because an invalid argument was passed to generator process */
    OperationFailed = 3,         /**< App exit with operation failure */
    UnspecifiedError = 4,        /**< App exit with unspecified failure */

    NoValidSession = 10,         /**< App exit because it was unable to create a valid SEG-Y Session context */
    InvalidToken = 11,           /**< App exit because provided token is of unexpected format */
    TokenCannotBeRetrieved = 12, /**< App exit because token could not be retrieved */
    SAuth2EnvironmentNotSet = 13,/**< App exit because required SAuth2 environment variables are not defined */
    EnvironmentNotSet = 14,      /**< App exit because required environment variables are not defined */
    CouldNotGetIndex = 19,       /**< App exit because it was unable to fetch an existing index */

    SEGYError = 50,              /**< App exit because an error occurred during the SEG-Y handling process */
    NavigationExtractionFailed = 100, /**< App exit because an failure happened during the navigation extraction process */
    HelpInfoDisplayed = 999,     /**< App exit after displaying usage information */

};


class SegyIndexGeneratorStatus {
public:
    static int GetCodeFromEnum(const IndexingReturnStatus enumValue);
    static IndexingReturnStatus GetEnumFromCode(int status);


};
} // namespace SegyIndexer::Utilities
