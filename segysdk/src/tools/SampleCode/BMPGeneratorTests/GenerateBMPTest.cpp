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

#include "../tools/SampleCode/BMPGenerator/BMPImageSection.h"
#include "../tools/SampleCode/Common/EnvironmentVariables.h"
#include "../tools/SampleCode/Common/FileCollectionUtilities.h"

#include "../Utilities/InputParser.h"
#include "../Utilities/StringUtils.h"

#include <iostream>

void
GenerateCellColours(SampleCode::BMPGenerator::BMPImageSection& image, SampleCode::BMPGenerator::Colour& colour, const size_t x, const size_t y, const size_t x_width, const size_t y_height)
{

    for (size_t i = 0; i < y_height; i++) {
        for (size_t j = 0; j < x_width; j++) {
            image.SetColour(colour, x + j, y + i);
        }
    }
}

void
generateTestImage(const size_t width, const size_t height, const std::string& output_folder)
{

    SampleCode::BMPGenerator::BMPImageSection image(width, height);

    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
            auto r = static_cast<unsigned char>((static_cast<double>(x) / static_cast<double>(width)) * 255);
            auto g = static_cast<unsigned char>(255 - (static_cast<double>(x) / static_cast<double>(width)) * 255);
            auto b = static_cast<unsigned char>((static_cast<double>(y) / static_cast<double>(height)) * 255);
            image.SetColour(SampleCode::BMPGenerator::Colour(r, g, b), x, y);
        }
    }

    std::filesystem::path out_file(output_folder);
    out_file /= "SimpleTestImage.BMP";

    if (std::filesystem::exists(out_file.string())) {
        std::filesystem::remove(out_file.string());
    }

    image.Export(out_file);

    if (!out_file.empty() && std::filesystem::exists(out_file)) {

        // Check if file is more than simply header data ...
        if (std::filesystem::file_size(out_file) > 54) {
            std::cout << "Display resulting image ... " << std::endl;
            if (std::system(out_file.generic_string().c_str()) == -1) {
                std::cout << "std::system(): child process error" << std::endl;
            }
            std::cout << "done." << std::endl;
        }
    }
}

int
main(int argc, char** argv)
{
    const segysdk::utilities::InputParser input(argc, argv);
    std::string bmp_folder;

    const std::string& outputFolder = input.getCmdOption("-outputFolder");
    if (!outputFolder.empty()) {
        bmp_folder = (segysdk::utilities::StringUtils::keyEndsWith(outputFolder, "\\")) ? outputFolder : outputFolder + "\\";
    }
    else {
        bmp_folder = "D:\\Temp\\";
    }

    generateTestImage(640, 480, bmp_folder);
}
