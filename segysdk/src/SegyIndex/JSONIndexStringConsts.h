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

#pragma once

#include <string>

namespace segysdk::segyindex
{
class JSONIndexStringConsts
{

private:
public:
    inline static const std::string metaTag = "Metadata";
    inline static const std::string GeneratedDate = "GeneratedDate";

    inline static const std::string IndexCount = "IndexCount";
    inline static const std::string IndexAsciiHeaderSize = "IndexAsciiHeaderSize";

    inline static const std::string segyInfoTag = "SEGYInfo";
    inline static const std::string SampleSize = "SampleSize";

    inline static const std::string ConstantCoordinateScalar = "ConstantCoordinateScalar";
    inline static const std::string ConstantCoordinateScalarValue = "ConstantCoordinateScalarValue";

    inline static const std::string ConstantTraceLength = "ConstantTraceLength";
    inline static const std::string ConstantTraceLengthValue = "ConstantTraceLengthValue";

    inline static const std::string ConstantTraceWeighting = "ConstantTraceWeighting";
    inline static const std::string ConstantTraceWeightingValue = "ConstantTraceWeightingValue";

    inline static const std::string AmplitudeMax = "AmplitudeMax";
    inline static const std::string AmplitudeMin = "AmplitudeMin";

    inline static const std::string SegyDimension = "SEGYDim";

    inline static const std::string OffsetFirtstSEGYTrace = "OffsetFirtstSEGYTrace";
    inline static const std::string TraceHeaderSize = "TraceHeaderSize";
    inline static const std::string SegyFile = "SegyFile";
    inline static const std::string SegyFileList = "SegyFileList";
    inline static const std::string SegyFileMap = "SegyFileMap";

    inline static const std::string versionTag = "Version";
    inline static const std::string Major = "Major";
    inline static const std::string Minor = "Minor";

    inline static const std::string Geometry3DTag = "Geometry3D";
    inline static const std::string FileLayoutSpecificationTag = "FileLayoutSpecification";
    inline static const std::string LineExtentTag = "LineExtent";
    inline static const std::string first = "First";
    inline static const std::string last = "Last";
    inline static const std::string step = "Step";
    inline static const std::string domain = "Domain";
    inline static const std::string zUoM = "zUoM";

    inline static const std::string CMPExtent = "CMPExtent";
    inline static const std::string JVectorExtent = "InlineExtent";
    inline static const std::string IVectorExtent = "CrosslinelineExtent";
    inline static const std::string dataGrid = "DataGrid";
    inline static const std::string sampleExtent = "SampleExtent";

    inline static const std::string corner11 = "Corner11";
    inline static const std::string corner1N = "Corner1N";
    inline static const std::string cornerN1 = "CornerN1";
    inline static const std::string cornerNN = "CornerNN";
    inline static const std::string inlineNo = "Inline";
    inline static const std::string crosslineNo = "Crossline";
    inline static const std::string coordX = "X";
    inline static const std::string coordY = "Y";

    inline static const std::string IVectorRotationAngle = "IVectorRotationAngle";
    inline static const std::string JVectorUnitDistance = "JVectorUnitDistance";
    inline static const std::string JVectorRotationAngle = "JVectorRotationAngle";
    inline static const std::string IVectorUnitDistance = "IVectorUnitDistance";

    inline static const std::string CrosslineRotationAngle = "CrosslineRotationAngle";
    inline static const std::string CrosslineUnitDistance = "CrosslineUnitDistance";
    inline static const std::string InlineRotationAngle = "InlineRotationAngle";
    inline static const std::string InlineUnitDistance = "InlineUnitDistance";

    inline static const std::string IsInlineOrdered = "IsInlineOrdered";
    inline static const std::string IsRegular = "IsRegular";

    inline static const std::string SegyIndexDataTag = "SegyIndexData";
    inline static const std::string Inline = "Inline";
    inline static const std::string Crossline = "Crossline";
    inline static const std::string Offset = "Offset";
    inline static const std::string FileNo = "FileNo";
    inline static const std::string FileOffset = "FileOffset";

    inline static const std::string Primary = "Primary";
    inline static const std::string PrimaryMin = "PrimaryMin";
    inline static const std::string PrimaryMax = "PrimaryMax";

    inline static const std::string meter = "meter";
    inline static const std::string feet = "feet";
    inline static const std::string seconds = "seconds";

    inline static const std::string dimension2D = "2D";
    inline static const std::string dimension3D = "3D";
};

} // namespace segysdk::segyindex
