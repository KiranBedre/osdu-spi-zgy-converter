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

#include "Params.h"

namespace indexer
{

Params::Params(const std::string& fileCollectionId, const std::string& seismicTraceDataId)
  : fileCollectionId_{fileCollectionId}, seismicTraceData_{seismicTraceDataId}
{
}

std::string
Params::toString() const
{
    return std::string{}
        .append("  file collection path: '")
        .append(fileCollectionPath_)
        .append("'\n  file collection id: '")
        .append(fileCollectionId_)
        .append("'\n  seismic trace data: '")
        .append(seismicTraceData_)
        .append("'\n  work product id: '")
        .append(workProductId_)
        .append("'\n  seismic 2D interpretation set: '")
        .append(seismic2DInterpretationSet_)
        .append("'\n  force overwrite: '")
        .append(std::to_string(forceOverwrite_))
        .append("'\n  is SEG-Y 2D: '")
        .append(std::to_string(isSegy2D))
        .append("'");
}

} // namespace indexer
