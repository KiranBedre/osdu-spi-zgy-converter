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

#include "Parameters.h"
#include "../SegyIndex/FileLayoutSpecification.h"
#include "../SegyIndex/Geometry3D.h"

#include <utility>

namespace segysdk::segyindex::tests
{

class Expected
{

private:
    Geometry3D geometry_;
    FileLayoutSpecification fileLayout_;
    Parameters parameters_;

public:
    Expected() {}

public:
    Geometry3D getGeometry() const { return geometry_; }
    void setGeometry(Geometry3D value) { geometry_ = value; }

    FileLayoutSpecification getFileLayout() const { return fileLayout_; }
    void setFileLayout(FileLayoutSpecification value) { fileLayout_ = value; }

    Parameters getParameter() const { return parameters_; }
    void setParameters(Parameters value) { parameters_ = std::move(value); }
};

} // namespace segysdk::segyindex::tests
