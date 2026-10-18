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

#include "tools/SaveThumbnails/PngCreator.h"

#include <png.h>

#include <iostream>

namespace tool::thumbnail
{

ByteBuffer
createPng(const Bitmap& bitmap)
{
    auto result = ByteBuffer{};
    auto* png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png) {
        std::cerr << "could not allocate write structure" << std::endl;
        return result;
    }
    auto* info = png_create_info_struct(png);
    if (!info) {
        std::cerr << "could not allocate info structure" << std::endl;
        png_destroy_write_struct(&png, nullptr);
        return result;
    }
    if (setjmp(png_jmpbuf(png))) {
        std::cerr << "error during png creation" << std::endl;
        png_destroy_write_struct(&png, &info);
        return result;
    }
    png_set_IHDR(png,
                 info,
                 static_cast<png_uint_32>(bitmap.width_),
                 static_cast<png_uint_32>(bitmap.height_),
                 8,
                 PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
    auto rowPointers = std::vector<const ByteBuffer::value_type*>{};
    rowPointers.reserve(bitmap.height_);
    for (std::size_t y = 0; y < bitmap.height_; ++y) {
        rowPointers.push_back(bitmap.getRowPointer(y));
    }
    png_set_rows(png, info, const_cast<png_bytepp>(rowPointers.data()));
    png_set_write_fn(
        png,
        &result,
        [](png_structp png_ptr, png_bytep data, std::size_t length) { segysdk::utilities::appendContainer(
                                                                          *(static_cast<ByteBuffer*>(png_get_io_ptr(png_ptr))),
                                                                          data,
                                                                          data + length); },
        [](png_structp) {
            // no output_flush_fn for memory writer
        });
    png_write_png(png, info, 0, nullptr);
    png_destroy_write_struct(&png, &info);
    return result;
}

} // namespace tool::thumbnail
