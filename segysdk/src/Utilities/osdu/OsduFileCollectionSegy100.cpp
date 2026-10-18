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

#include "OsduFileCollectionSegy100.h"

#include "GeneratedOutputFileName.h"
#include "OsduFieldNameLiterals.h"

namespace segysdk::utilities::osdu
{

OsduFileCollectionSegy100::OsduFileCollectionSegy100(Json::Value root)
  : OsduObject(std::move(root))
{
}

std::vector<std::string>
OsduFileCollectionSegy100::segyFileNames() const
{
    // TODO review logic
    // TODO relative paths
    std::vector<std::string> ret;

    if (!root_.isMember(OsduFieldNameLiterals::Data) || 
        !root_[OsduFieldNameLiterals::Data].isMember(OsduFieldNameLiterals::DatasetProperties) || 
        !root_[OsduFieldNameLiterals::Data][OsduFieldNameLiterals::DatasetProperties].isMember(OsduFieldNameLiterals::FileSourceInfos)) {
        return ret;
    }
   
    const auto& fileSourceInfos_ = root_[OsduFieldNameLiterals::Data][OsduFieldNameLiterals::DatasetProperties][OsduFieldNameLiterals::FileSourceInfos];
    
    for (const auto& file : fileSourceInfos_) {
        ret.push_back(file[OsduFieldNameLiterals::FileSource].asString());
    }

    return ret;
}

std::string
OsduFileCollectionSegy100::desiredZgyFileName(const std::string& customSuffix) const
{
    return GeneratedOutputFileName(segyFileNames()[0], customSuffix).fileName;
}

int
OsduFileCollectionSegy100::segyRevision() const
{
    const char* const revisionField = "SEGYRevision";
    const int assumeRevision = 1;
    if (not root_.isMember(revisionField)) {
        return assumeRevision;
    }

    // Format is "rev [0|1|2]"
    std::string revision = root_[revisionField].asString();
    std::map<std::string, int> revisions = {
        {"rev 0", 0},
        {"rev 1", 1},
        {"rev 2", 2}
    };
    if (revisions.find(revision) == revisions.end()) {
        // Assume this is 1 as basic revision
        return assumeRevision;
    }
    return revisions[revision];
}


} // namespace segysdk::utilities::osdu
