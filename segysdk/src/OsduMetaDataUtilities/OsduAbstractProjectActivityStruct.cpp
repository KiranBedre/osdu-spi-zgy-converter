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

#include "../OsduMetaDataUtilities/OsduAbstractProjectActivityStruct.h"

namespace segysdk::metadata::osdu
{
OsduAbstractProjectActivityStruct::OsduAbstractProjectActivityStruct(std::string osduPartition)
  : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

bool
OsduAbstractProjectActivityStruct::isValid() const
{
    return isValidIdString(templateId, IdStringCheckType::Reference) and isValidIdString(parentProjectId, IdStringCheckType::Reference)
           and arrayStructValid(parameters) and arrayStructValid(states) and structValid(lastState);
}

std::vector<std::string>
OsduAbstractProjectActivityStruct::performAttributeValidityScan() const
{
    std::vector<std::string> results;
    if (!isValid()) {
        results.emplace_back("OsduAbstractProjectActivityStruct::isValid() failed.");
    }

    performIdStringValidityScan(results, templateId, "OsduAbstractProjectActivityStruct:templateId: ", IdStringCheckType::Reference);
    performIdStringValidityScan(results, parentProjectId, "OsduAbstractProjectActivityStruct:parentProjectId: ", IdStringCheckType::Reference);
    performArrayStructValidityScan(results, parameters);
    performArrayStructValidityScan(results, states);
    performStructValidityScan(results, lastState);

    return results;
}

void
OsduAbstractProjectActivityStruct::propagatePartition(const std::string& partition)
{
    if (parameters.has_value()) {
        for (auto& parameter : parameters.value()) {
            parameter.propagatePartition(partition);
        }
    }
    if (states.has_value()) {
        for (auto& state : states.value()) {
            state.propagatePartition(partition);
        }
    }
    if (lastState.has_value()) {
        lastState.value().propagatePartition(partition);
    }
}
} // namespace segysdk::metadata::osdu
