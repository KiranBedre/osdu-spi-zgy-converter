/*
Copyright 2021 Schlumberger

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

#include "../segysdk_details/DLL_EXPORT.h"

#include "BinaryHeaderField.h"
#include "SegyOverrideField.h"
#include "TraceHeaderField.h"

#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace segysdk::segyformat::external
{

class SEGYSDK_DLL_PUBLIC SegyContext
{

private:
    std::vector<std::string> segyFileNames_;

    std::vector<std::shared_ptr<SegyHeaderField>> traceHeaderDefinitions_;
    std::vector<std::shared_ptr<SegyHeaderField>> binaryHeaderDefinitions_;

    std::vector<std::shared_ptr<SegyOverrideField>> binaryHeaderOverrides_;
    std::vector<std::shared_ptr<SegyOverrideField>> traceHeaderOverrides_;

    /* If not specified otherwise in the constructor, set segyVersion by default to rev 1 as this is the richest definition and most likely to be correct*/
    int segyVersion_ = 1;

    // Parameter to be used when saving files in SD
    std::optional<std::string> legalTag_ = std::nullopt;

public:
    /// <summary>
    /// Returns default set of Segy rev 1 Binary header parameters. All offset values are 1-based.
    /// </summary>
    /// <returns>* definitions</returns>
    static std::vector<std::shared_ptr<SegyHeaderField>> getSegyStandardBinaryHeaderDefinitions();

    /// <summary>
    /// Returns default set of Segy Trace header parameters for the requested SEG-Y version. All offset values are 1-based.
    /// </summary>
    /// <returns>* definitions</returns>
    static std::vector<std::shared_ptr<SegyHeaderField>> getDefaultTraceHdrLocations(const int segyVersion);

    /// <summary>
    /// Returns default set of Segy rev 1 trace header parameters. All offset values are 1-based.
    /// </summary>
    /// <returns>* definitions</returns>
    static std::vector<std::shared_ptr<SegyHeaderField>> getSegyRev1TraceHdrLocations();

    /// <summary>
    /// Returns default set of Segy rev 0 trace header parameters. All offset values are 1-based.
    /// </summary>
    /// <returns>* definitions</returns>
    static std::vector<std::shared_ptr<SegyHeaderField>> getSegyRev0Locations();

    static bool CheckForOverlaps(std::vector<std::shared_ptr<SegyHeaderField>> headers);

    static bool CheckForOverlaps(const std::vector<std::shared_ptr<SegyHeaderField>>& headers, const int offset, const unsigned short length);

public:
    SegyContext();
    SegyContext(const std::vector<std::string>& segyFileNames, int segyVersion);

    std::vector<std::string> getSegyFileNames() const { return segyFileNames_; }

    void setSegyFileNames(std::vector<std::string> const& segyFileNames);

    std::vector<std::string> const& getSegyFileNames();

    std::vector<std::shared_ptr<SegyHeaderField>> getTraceHeaderDefinitions() const;
    void setTraceHeaderDefinitions(std::vector<std::shared_ptr<SegyHeaderField>>& traceHeaders);

    std::vector<std::shared_ptr<SegyHeaderField>> getBinaryHeaderDefinitions() const;
    void setBinaryHeaderDefinitions(std::vector<std::shared_ptr<SegyHeaderField>>& binaryHeaders);

    std::vector<std::shared_ptr<SegyOverrideField>> getTraceHeaderOverrideDefinitions() const;
    void setTraceHeaderOverrideDefinitions(std::vector<std::shared_ptr<SegyOverrideField>>& traceHeaderOverridess);

    std::vector<std::shared_ptr<SegyOverrideField>> getBinaryHeaderOverrideDefinitions() const;
    void setBinaryHeaderOverrideDefinitions(std::vector<std::shared_ptr<SegyOverrideField>>& binaryHeaderOverridess);

    void setSegyVersion(int version) { segyVersion_ = version; }

    void ValidateInputParameters(std::vector<std::string> const& fileName, std::vector<std::shared_ptr<SegyHeaderField>> binaryHeaders, std::vector<std::shared_ptr<SegyHeaderField>> traceHeaders) const;

    static std::string getFilePath(const std::string& fileName);

    void setHeaderValues(std::vector<SegyHeaderField> const& binaryHeaders, std::vector<SegyHeaderField> const&
                                                                                traceHeaders);

    // This value must be set, ideally at point of creation, as introducing any of hte SegyIO/FileIO readers will create a cyclic redundancy
    int getSegyVersion() const { return segyVersion_; }

    void setLegalTag(const std::string& legalTag) { legalTag_ = legalTag; }
    const std::optional<std::string>& getLegalTag() const { return legalTag_; }

    /// <summary>
    /// Returns default set of Segy rev 1 trace header parameters. All offset values are 1-based.
    /// </summary>
    /// <returns>* definitions</returns>
    std::vector<std::shared_ptr<SegyHeaderField>> getDefaultTraceHdrLocations() const;
};

} // namespace segysdk::segyformat::external
