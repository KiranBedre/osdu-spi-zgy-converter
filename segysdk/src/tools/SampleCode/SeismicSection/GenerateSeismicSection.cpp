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

#include "ColourTable.h"

#include "../tools/SampleCode/BMPGenerator/BMPImageSection.h"
#include "../tools/SampleCode/Common/SessionUtilities.h"

#include "../SegyIndex/SegyIndex.h"
#include "../SegyIndex/SeismicCubeExtents.h"

#include "../Utilities/EnvironmentVariableStringConsts.h"
#include "../Utilities/StringUtils.h"
#include "../Utilities/InputParser.h"

#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <map>

constexpr int ColourTableDisplayHeight = 60;
constexpr int ColourTableHeight = 20;
constexpr int ColourTablePaddingHeight = (ColourTableDisplayHeight - ColourTableHeight) / 2;

template<typename T>
std::enable_if_t<std::is_arithmetic_v<T>, T>
getNumbFromInputCfg(
    const segysdk::utilities::InputParser& input, const char* input_name, T var)
{
    const std::string& parameter = input.getCmdOption(input_name);
    if (!parameter.empty()) {
        if (std::is_same_v<T, int>) {
            return std::stoi(parameter);
        }
        else {
            return std::stod(parameter);
        }
    }
    return var;
}

struct GenerationConfig
{
    int inlineNo_{std::numeric_limits<int>::min()};
    int crosslineNo_{std::numeric_limits<int>::min()};
    double minAmplitude_{std::numeric_limits<double>::max()};
    double maxAmplitude_{std::numeric_limits<double>::lowest()};
    bool isInlineSection_{};
    bool useGrayScale_{};
    bool forceOverwrite_{false};
    std::string outputFolder_;
    size_t maxThreads_{4U};

    explicit GenerationConfig(const segysdk::utilities::InputParser& input)
    {
        createFromInput(input);
    }

    void createFromInput(const segysdk::utilities::InputParser& input)
    {
        const std::string& folder = input.getCmdOption("-outputFolder");
        if (!folder.empty()) {
            outputFolder_ = (segysdk::utilities::StringUtils::keyEndsWith(folder, "\\")) ? folder : folder + "\\";
        }
        else {
            outputFolder_ = "D:\\Temp\\";
        }

        // Specify inline to display
        inlineNo_ = getNumbFromInputCfg(input, "-Inline", inlineNo_);
        isInlineSection_ = inlineNo_ != std::numeric_limits<int>::min();

        // Specify crossline to display
        crosslineNo_ = getNumbFromInputCfg(input, "-Crossline", crosslineNo_);

        // minAmplitude and maxAmplitude might be used in cases where there are amplitude spikes in the seismic data to make the image look less washed out.
        minAmplitude_ = getNumbFromInputCfg(input, "-MinAmplitude", minAmplitude_);
        maxAmplitude_ = getNumbFromInputCfg(input, "-MaxAmplitude", maxAmplitude_);

        constexpr int gray_scale = 0;
        useGrayScale_ = getNumbFromInputCfg(input, "-grayScale", gray_scale) != gray_scale;

        maxThreads_ = getNumbFromInputCfg(input, "-MaxThreads", maxThreads_);

        constexpr int overwrite = 0;
        forceOverwrite_ = getNumbFromInputCfg(input, "-ForceOverwrite", overwrite) != overwrite;
    }
};

void
GenerateCellColours(SampleCode::BMPGenerator::BMPImageSection& image, SampleCode::BMPGenerator::Colour& colour, const size_t x, const size_t y, const size_t x_width, const size_t y_height)
{

    for (size_t i = 0; i < y_height; i++) {
        for (size_t j = 0; j < x_width; j++) {
            image.SetColour(colour, x + j, y + i);
        }
    }
}

bool
isValidSectionNumber(const std::shared_ptr<segysdk::segyindex::SegyIndex>& index, bool isInlineSection, int line_no)
{
    segysdk::segyindex::SeismicCubeExtents const seismicCubeExtents = index->getGeometry3DExtents();

    if (isInlineSection) {
        if (line_no < seismicCubeExtents.getJVectorExtent().first() || line_no > seismicCubeExtents.getJVectorExtent().last())
            return false;
    }
    else {
        if (line_no < seismicCubeExtents.getIVectorExtent().first() || line_no > seismicCubeExtents.getIVectorExtent().last())
            return false;
    }

    const int steps = isInlineSection
                          ? line_no - seismicCubeExtents.getJVectorExtent().first()
                          : line_no - seismicCubeExtents.getIVectorExtent().first();

    return isInlineSection
               ? steps % seismicCubeExtents.getJVectorExtent().step() == 0
               : steps % seismicCubeExtents.getIVectorExtent().step() == 0;
}

int
getFirstTraceOfLine(const std::shared_ptr<segysdk::segyindex::SegyIndex>& index, bool isInlineSection, int section_no)
{
    segysdk::segyindex::SeismicCubeExtents const seismicCubeExtents = index->getGeometry3DExtents();

    int const firstTrace = isInlineSection
                               ? seismicCubeExtents.getIVectorExtent().first()
                               : seismicCubeExtents.getJVectorExtent().first();
    int const lastTrace = isInlineSection
                              ? seismicCubeExtents.getIVectorExtent().last()
                              : seismicCubeExtents.getJVectorExtent().last();
    int const step = isInlineSection
                         ? seismicCubeExtents.getIVectorExtent().step()
                         : seismicCubeExtents.getJVectorExtent().step();
    int const lineNo = isInlineSection
                           ? section_no
                           : firstTrace;
    int trace = firstTrace;

    while (trace <= lastTrace
           && index->getIndexAt(lineNo, trace).getOffset() == 0) {
        trace += step;
    }

    return trace;
}

int
getLastTraceOfLine(const std::shared_ptr<segysdk::segyindex::SegyIndex>& index, bool isInlineSection, int section_no)
{
    segysdk::segyindex::SeismicCubeExtents const seismicCubeExtents = index->getGeometry3DExtents();

    int const firstTrace = isInlineSection
                               ? seismicCubeExtents.getIVectorExtent().first()
                               : seismicCubeExtents.getJVectorExtent().first();
    int const lastTrace = isInlineSection
                              ? seismicCubeExtents.getIVectorExtent().last()
                              : seismicCubeExtents.getJVectorExtent().last();
    int const step = isInlineSection
                         ? seismicCubeExtents.getIVectorExtent().step()
                         : seismicCubeExtents.getJVectorExtent().step();
    int const lineNo = isInlineSection
                           ? section_no
                           : firstTrace;
    int trace = lastTrace;

    while (trace >= firstTrace
           && index->getIndexAt(lineNo, trace).getOffset() == 0) {
        trace -= step;
    }

    return trace;
}

double
GetMaximumAmplitudeValue(const std::unique_ptr<float[]>& buffer, const size_t width, const size_t height)
{
    float max = -std::numeric_limits<float>::max();
    for (size_t trace_ix = 0; trace_ix < width; trace_ix++) {
        for (size_t sample_ix = 0; sample_ix < height; sample_ix++) {
            const auto value = buffer[trace_ix * height + sample_ix];
            if (value > max) {
                max = value;
            }
        }
    }
    return static_cast<double>(max);
}

double
GetMinimumAmplitudeValue(const std::unique_ptr<float[]>& buffer, const size_t width, const size_t height)
{
    float min = std::numeric_limits<float>::max();
    for (size_t trace_ix = 0; trace_ix < width; trace_ix++) {
        for (size_t sample_ix = 0; sample_ix < height; sample_ix++) {
            const auto value = buffer[trace_ix * height + sample_ix];
            if (value < min) {
                min = value;
            }
        }
    }
    return static_cast<double>(min);
}

void
PrepareAmplitudeRange(const std::unique_ptr<float[]>& buffer, double& minAmplitude, double& maxAmplitude, const size_t width, const size_t height)
{
    if (std::numeric_limits<double>::max() - std::abs(minAmplitude) < 0.1) {
        minAmplitude = GetMinimumAmplitudeValue(buffer, width, height);
    }

    if (std::numeric_limits<double>::max() - std::abs(maxAmplitude) < 0.1) {
        maxAmplitude = GetMaximumAmplitudeValue(buffer, width, height);
    }

    // make amplitudes zero centric if one is positive and one is negative
    if (minAmplitude < 0 && maxAmplitude > 0) {
        const float pos_min = std::abs(static_cast<float>(minAmplitude));
        if (pos_min > maxAmplitude) {
            minAmplitude = -maxAmplitude;
        }
        else if (maxAmplitude > pos_min) {
            maxAmplitude = pos_min;
        }
    }

    std::cout << "Using colour table range: Min amplitude: " << std::to_string(minAmplitude) << " .. Max amplitude: " << std::to_string(maxAmplitude) << std::endl;
}

std::map<unsigned char, SampleCode::BMPGenerator::Colour>
GetSpectrumSeedColours()
{
    std::map<unsigned char, SampleCode::BMPGenerator::Colour> seeds;

    const SampleCode::BMPGenerator::Colour cyan{0, 255, 255};
    seeds[0] = cyan;
    const SampleCode::BMPGenerator::Colour blue{0, 0, 255};
    seeds[75] = blue;
    const SampleCode::BMPGenerator::Colour black{0, 0, 0};
    seeds[76] = black;
    const SampleCode::BMPGenerator::Colour white{250, 250, 250};
    seeds[128] = white;
    const SampleCode::BMPGenerator::Colour brown{165, 42, 42};
    seeds[181] = brown;
    const SampleCode::BMPGenerator::Colour red{255, 0, 0};
    seeds[182] = red;

    const SampleCode::BMPGenerator::Colour yellow{255, 255, 0};
    seeds[255] = yellow;

    return seeds;
}

void
ColourPlotMargin(const std::unique_ptr<SampleCode::BMPGenerator::BMPImageSection>& image, const size_t from_trace, const size_t to_trace, const size_t height, const SampleCode::BMPGenerator::Colour margin_colour)
{
    for (size_t trace_ix = from_trace; trace_ix < to_trace; trace_ix++) {
        for (size_t sample_ix = 0; sample_ix < height; sample_ix++) {
            //Image builds up from bottom up but the traces are organized from top down
            const size_t image_sample_ix = height - sample_ix - 1;
            image->SetColour(margin_colour, trace_ix, image_sample_ix);
        }
    }
}

void
ColourSeismicSection(const std::unique_ptr<SampleCode::BMPGenerator::BMPImageSection>& image, SampleCode::ColourTable::ColourTable& colour_table,
                     const std::unique_ptr<float[]>& buffer, const std::vector<bool>& liveTraces, const size_t width, const size_t height,
                     const double minAmplitude, const double maxAmplitude)
{
    // width should always be smaller than or equal to image->Width()
    const size_t section_margin = width >= image->Width() ? 0 : (image->Width() - width) / 2;

    constexpr float const margin_color_value = 0.0F;
    SampleCode::BMPGenerator::Colour const margin_colour = colour_table.GetSampleColour(margin_color_value,
                                                                                        static_cast<float>(minAmplitude),
                                                                                        static_cast<float>(maxAmplitude));

    // set left margin traces (if any) to zeroes
    ColourPlotMargin(image, 0, section_margin, height, margin_colour);

    for (size_t trace_ix = 0; trace_ix < width; trace_ix++) {
        for (size_t sample_ix = 0; sample_ix < height; sample_ix++) {
            const float value = buffer[trace_ix * height + sample_ix];
            SampleCode::BMPGenerator::Colour const sample_colour = liveTraces[trace_ix]
                                                                       ? colour_table.GetSampleColour(value,
                                                                                                      static_cast<float>(minAmplitude),
                                                                                                      static_cast<float>(maxAmplitude))
                                                                       : margin_colour;

            //Image builds up from bottom up but the traces are organized from top down
            const size_t image_sample_ix = height - sample_ix - 1;
            image->SetColour(sample_colour, section_margin + trace_ix, image_sample_ix);
        }
    }

    // Make sure border is not black if seismic section has fewer traces than there are number of colours in colour table ...
    ColourPlotMargin(image, section_margin + width, image->Width(), height, margin_colour);
}

void
PostColourTable(const std::unique_ptr<SampleCode::BMPGenerator::BMPImageSection>& image, SampleCode::ColourTable::ColourTable& colour_table)
{
    // Post color above the seismic image ...
    // Set background colour
    const auto zeroCrossingColour = colour_table.GetColourAt(128);
    for (size_t cb_height = 0; cb_height < ColourTableDisplayHeight; cb_height++) {
        for (size_t ix = 0; ix < image->Width(); ix++) {
            image->SetColour(zeroCrossingColour, ix, image->Height() - ColourTableDisplayHeight + cb_height);
        }
    }

    const size_t offset = (image->Width() - SampleCode::ColourTable::ColourTable::MaxColourEntry()) / 2;
    const size_t max_colour_entry = SampleCode::ColourTable::ColourTable::MaxColourEntry();

    // Post colour table centered at the top of the seismic display.
    for (size_t cb_height = 0; cb_height < ColourTableHeight; cb_height++) {
        // Careful so ix doesn't get too large so it overflows ...
        for (size_t ix = 0; ix < max_colour_entry; ix++) {
            auto colour = colour_table.GetColourAt(ix);
            const size_t xpos = offset + static_cast<int>(ix);
            const size_t ypos = image->Height() - ColourTableDisplayHeight + ColourTablePaddingHeight + cb_height;
            image->SetColour(colour, xpos, ypos);
        }
        auto colour = colour_table.GetColourAt(255);
        const size_t xpos = offset + 255;
        const size_t ypos = image->Height() - ColourTableDisplayHeight + ColourTablePaddingHeight + cb_height;
        image->SetColour(colour, xpos, ypos);
    }
}

void
PopulateImageData(std::unique_ptr<SampleCode::BMPGenerator::BMPImageSection>& image, const std::unique_ptr<float[]>& buffer, std::vector<bool>& liveTraces, const size_t section_width, const size_t height, double minAmplitude, double maxAmplitude, const bool use_gray_scale)
{
    // Provide initial seed colours for generating the colour table
    const std::map<unsigned char, SampleCode::BMPGenerator::Colour> seed_colours = GetSpectrumSeedColours();

    // Set up the colour selected table to use
    auto colour_table = use_gray_scale
                            ? SampleCode::ColourTable::ColourTable()
                            : SampleCode::ColourTable::ColourTable(seed_colours);

    // Regular seismic samples will normally have a value range distributed as a bell curve around the value 0.
    // This is reflected in the design of the colour tables where there is usually a distinct value for the zero crossing.
    // To prime the logic for matching a trace value to a colour table value we either need to know exactly what our minimum and maximum
    // sample values are, or use a user defined value for minimum and maximum. This will allow boosting low values for sections where
    // there are certain samples having spike-values, much larger in magnitude than the other samples.
    // Minimum and maximum amplitude values will be harmonized so maximum amplitude = abs(minimum amplitude)

    PrepareAmplitudeRange(buffer, minAmplitude, maxAmplitude, section_width, height);

    // Plot the seismic trace values in the graphic canvas. Each trace sample value will be translated to the proper colour table entry.
    // Any missing traces will be posted having all zero values
    ColourSeismicSection(image, colour_table, buffer, liveTraces, section_width, height, minAmplitude, maxAmplitude);

    // Post the colour table used as decoration to allow user to understand what the sample values represent
    PostColourTable(image, colour_table);
}

bool
CanWriteOutputFile(const std::string& file_name, bool forceRemove = false)
{
    try {
        if (std::filesystem::exists(file_name)) {
            if (!forceRemove) {
                std::cout << "Output file " << file_name << " already exists.Delete it? [Y/n] ";
                char test;

                std::cin >> test;

                if (test != 'Y' && test != 'y' && test != '\n') {
                    std::cout << "Generation of graphics file cancelled by user." << std::endl;
                    return false;
                }
            }
            std::filesystem::remove(file_name);

            return !std::filesystem::exists(file_name);
        }

        return true;
    }
    catch (std::filesystem::filesystem_error& fse) {
        std::cout << "Failed to remove file " << file_name << std::endl;
        std::cout << "Error: " << fse.what() << std::endl;
    }
    return false;
}

std::vector<bool>
GetLiveTraceLayout(const std::unique_ptr<segysdk::api::SegySession>& segySession, const std::shared_ptr<segysdk::segyindex::SegyIndex>& index, const GenerationConfig& conf)
{
    segysdk::segyindex::SeismicCubeExtents const seismicCubeExtents = index->getFileLayoutExtents();

    size_t const section_width = static_cast<size_t>(conf.isInlineSection_
                                                         ? seismicCubeExtents.getIVectorExtent().count()
                                                         : seismicCubeExtents.getJVectorExtent().count());
    std::vector<bool> result(section_width);

    auto const fileId = segySession->getSelectedSegyFileId();
    auto const headerOffset = index->getFirstTraceOffset(fileId);

    if (conf.isInlineSection_) {
        size_t ix = 0;
        auto const inlineNo = conf.inlineNo_;

        auto isInlineNoValid = [&seismicCubeExtents](int trace) -> bool {
            return trace >= std::min(seismicCubeExtents.getIVectorExtent().first(), seismicCubeExtents.getIVectorExtent().last())
                   and trace <= std::max(seismicCubeExtents.getIVectorExtent().first(), seismicCubeExtents.getIVectorExtent().last());
        };

        for (auto trace = seismicCubeExtents.getIVectorExtent().first(); isInlineNoValid(trace);
             trace += seismicCubeExtents.getIVectorExtent().step()) {
            auto const idx = index->getIndexAt(inlineNo, trace);

            bool const isLive = idx.getOffset() >= headerOffset;
            result[ix++] = isLive;
        }
    }
    else {
        size_t ix = 0;
        auto const crosslineNo = conf.crosslineNo_;

        auto isCrosslineNoValid = [&seismicCubeExtents](int trace) -> bool {
            return trace >= std::min(seismicCubeExtents.getJVectorExtent().first(), seismicCubeExtents.getJVectorExtent().last())
                   and trace <= std::max(seismicCubeExtents.getJVectorExtent().first(), seismicCubeExtents.getJVectorExtent().last());
        };

        for (auto trace = seismicCubeExtents.getJVectorExtent().first(); isCrosslineNoValid(trace);
             trace += seismicCubeExtents.getJVectorExtent().step()) {
            auto const idx = index->getIndexAt(trace, crosslineNo);

            bool const isLive = idx.getOffset() >= headerOffset;
            result[ix++] = isLive;
        }
    }
    return result;
}

std::string
GenerateBMPImage(const std::unique_ptr<segysdk::api::SegySession>& segySession, const std::shared_ptr<segysdk::segyindex::SegyIndex>& index, const GenerationConfig& conf)
{
    segysdk::segyindex::SeismicCubeExtents const seismicCubeExtents = index->getFileLayoutExtents();

    // Height is number of samples in trace
    size_t const height = seismicCubeExtents.getSampleExtent().count();

    int const section_no = conf.isInlineSection_ ? conf.inlineNo_ : conf.crosslineNo_;
    // Max width is number of traces defined in survey grid
    size_t const section_width = static_cast<size_t>(conf.isInlineSection_
                                                         ? seismicCubeExtents.getIVectorExtent().count()
                                                         : seismicCubeExtents.getJVectorExtent().count());

    // Check if max_width is at least as wide as the colour bar
    const size_t colour_bar_width = SampleCode::ColourTable::ColourTable::MaxColourEntry();
    const size_t image_width = section_width < colour_bar_width ? colour_bar_width + 1 : section_width;

    auto image = std::make_unique<SampleCode::BMPGenerator::BMPImageSection>(image_width, height + ColourTableDisplayHeight);

    std::unique_ptr<float[]> section_buffer;

    const auto start = std::chrono::high_resolution_clock::now();
    // Set up the bulk data reader. This is the tool to use for accessing seismic trace data.
    // It will use the SegyIndex to enable it to move directly to the requested trace without having to
    // read through the SEG-Y file from the very beginning to find it.
    const size_t readersCount = conf.maxThreads_;
    std::cout << "Using " << readersCount << " separate readers." << std::endl;

    const std::unique_ptr<segysdk::bulkaccess::SegyBulkReader> bulk_reader = segySession->createSectionBulkReader(readersCount);

    if (conf.isInlineSection_) {
        size_t const inline_index = static_cast<size_t>((section_no - seismicCubeExtents.getJVectorExtent().first()) / seismicCubeExtents.getJVectorExtent().step());
        size_t const buffer_size = section_width * static_cast<size_t>(seismicCubeExtents.getSampleExtent().count());

        // Set up read buffer for seismic samples
        section_buffer = std::make_unique<float[]>(buffer_size);

        // Instruct the bulk_reader to fetch a single inline worth of trace data and put it into the pre-allocated section_buffer.
        // The read method uses the following arguments read(read_direction, first_inline_ix, first_crossline_ix, first_sample_ix, trace_count, last_sample_count, buffer);
        //  * read_direction is direction to read the contents - below we want to read data for all traces in the inline
        //  * first_inline_ix is zero-offset value of inline to extract, so first inline in file has value 0, next inline 1 and so on
        //  * first_crossline_ix is zero-offset value of crossline (or trace) to extract, so smallest crossline value in file has value 0, next crossline step is 1 and so on
        //  * first_sample_ix is zero-offset value for first sample to return, normally this will be 0 if all samples are requested, but can be any value < SampleExtent.count()
        //  * trace_count is number of traces to read - below we want to read data for all traces in the inline
        //  * sample_count is number of trace samples to read, starting from first_sample_ix. We want every sample in the trace.
        bulk_reader->read(segysdk::bulkaccess::ReadDirection::Inline,
                          inline_index,
                          /*xlineIndex*/ 0,
                          /*sampleIndex*/ 0,
                          section_width,
                          static_cast<size_t>(seismicCubeExtents.getSampleExtent().count()),
                          section_buffer.get());
    }
    else {
        size_t const crossline_index = static_cast<size_t>((section_no - seismicCubeExtents.getIVectorExtent().first()) / seismicCubeExtents.getIVectorExtent().step());
        size_t const buffer_size = section_width * static_cast<size_t>(seismicCubeExtents.getSampleExtent().count());

        // Set up read buffer for seismic samples
        section_buffer = std::make_unique<float[]>(buffer_size);

        // Instruct the bulk_reader to fetch a single crossline worth of trace data and put it into the pre-allocated section_buffer.
        // The read method uses the following arguments read(read_direction, first_inline_ix, first_crossline_ix, first_sample_ix, trace_count, last_sample_count, buffer);
        //  * read_direction is direction to read the contents - below we want to read data for the requested crossline
        //  * first_inline_ix is zero-offset value of inline to extract, so first inline in file has value 0, next inline 1 and so on
        //  * first_crossline_ix is zero-offset value of crossline (or trace) to extract, so smallest crossline value in file has value 0, next crossline step is 1 and so on
        //  * first_sample_ix is zero-offset value for first sample to return, normally this will be 0 if all samples are requested, but can be any value < SampleExtent.count()
        //  * trace_count is number of traces to read - below we want to read data for the requested crossline
        //  * sample_count is number of trace samples to read, starting from first_sample_ix. We want every sample in the trace.
        bulk_reader->read(segysdk::bulkaccess::ReadDirection::Crossline,
                          /*inlineIndex*/ 0,
                          crossline_index,
                          /*sampleIndex*/ 0,
                          section_width,
                          static_cast<size_t>(seismicCubeExtents.getSampleExtent().count()),
                          section_buffer.get());
    }

    auto const end = std::chrono::high_resolution_clock::now();
    // Calculating total time taken by the program.
    std::chrono::duration<double, std::milli> const ms_double = end - start;

    std::cout << "SEG-Y Read Execution time: " << std::endl;
    std::cout << ms_double.count() << "ms\n";

    // Get a map indicating if a trace is live (exists in the SEG-Y file) or not
    std::vector<bool> liveTraces = GetLiveTraceLayout(segySession, index, conf);

    // Process the extracted trace samples and generate the graphic output
    PopulateImageData(image, section_buffer, liveTraces, section_width, height, conf.minAmplitude_, conf.maxAmplitude_, conf.useGrayScale_);

    const std::string section_type = conf.isInlineSection_ ? "-Inline-" : "-Crossline-";

    const std::string file_name = conf.useGrayScale_
                                      ? "SeismicSectionImage" + section_type + std::to_string(section_no) + "-GrayScale.BMP"
                                      : "SeismicSectionImage" + section_type + std::to_string(section_no) + ".BMP";

    std::filesystem::path out_file(conf.outputFolder_);
    out_file /= file_name;

    if (CanWriteOutputFile(out_file.string(), conf.forceOverwrite_)) {
        image->Export(out_file);
    }
    return out_file.string();
}

std::string
generateSectionImage(const std::unique_ptr<segysdk::api::SegySession>& segySession, const GenerationConfig& conf)
{
    if (segySession == nullptr) {
        std::cout << "Segy session has not been properly initialized. Exiting ... " << std::endl;
        return "";
    }

    // Get accessor for the SegyIndex.
    // // The index serves 3 main purposes:
    // 1) It holds the "Extents" of the SEG-Y volume, i.e. first and last inline, crossline and depth range,and their increments
    // 2) It has captured information about certain trace header fields, and if these are constant the bulk reader (trace data reader)
    //    does not need to even read the trace headers for these fields
    // 3) It has an offset map to every trace (inline/crossline intersection) in the entire file. For fully populated files this will be calculated,
    // but for files where some or many traces are missing it allows the reader to skip directly to the requested trace. This is mainly for
    // internal usage, and is not be expected to be heavily used by client applications.
    const std::shared_ptr<segysdk::segyindex::SegyIndex> index = SampleCode::Common::SessionUtilities::GetSegyIndex(segySession);

    if (index == nullptr || !index->hasIndexFile()) {
        std::cout << "Segy file does not have an index. Please generate it before running this program ... " << std::endl;
        return "";
    }

    const int section_no = conf.isInlineSection_ ? conf.inlineNo_ : conf.crosslineNo_;

    if (!isValidSectionNumber(index, conf.isInlineSection_, section_no)) {
        std::string const section_type = conf.isInlineSection_ ? " inline " : " crossline ";

        segysdk::segyindex::SeismicCubeExtents const seismicCubeExtents = index->getFileLayoutExtents();

        int const section_first = conf.isInlineSection_
                                      ? seismicCubeExtents.getJVectorExtent().first()
                                      : seismicCubeExtents.getIVectorExtent().first();
        int const section_last = conf.isInlineSection_
                                     ? seismicCubeExtents.getJVectorExtent().last()
                                     : seismicCubeExtents.getIVectorExtent().last();
        int const section_step = conf.isInlineSection_
                                     ? seismicCubeExtents.getJVectorExtent().step()
                                     : seismicCubeExtents.getIVectorExtent().step();

        std::cout << "Invalid requested section number [" << std::to_string(section_no) << "]. Valid " << section_type << " range: From: " << section_first << " to " << section_last << " Step: " << section_step << std::endl;
        return "";
    }

    auto bmp_file_name = GenerateBMPImage(segySession, index, conf);

    return bmp_file_name;
}

void
print_usage(const char* exec_name, int argc)
{
    std::cout << exec_name << " called with " << std::to_string(argc - 1) << " parameters." << std::endl;

    std::cout << "Usage: " << std::endl;
    std::cout << "\tFor OSDU Data Platform: " << std::endl;
    std::cout << "\t\t" << exec_name << " -osdu <FileCollection.SEGY.json>  [ -Inline <inlineNo> OR -Crossline <crosslineNo>] [ -MinAmplitude <minimum amplitude>  -MaxAmplitude <maximum amplitude> -grayScale <0|1> -outputFolder <Path/To/Output/> -MaxThreads <read threads> -ForceOverwrite <0|1>]" << std::endl;

    std::cout << "To access data in cloud SDMS the following environment variables must be defined:" << std::endl;
    std::cout << segysdk::utilities::EnvironmentVariableStringConsts::SD_SVC_URL << std::endl;
    std::cout << segysdk::utilities::EnvironmentVariableStringConsts::SD_SVC_API_KEY << std::endl;
    std::cout << segysdk::utilities::EnvironmentVariableStringConsts::SD_SVC_TOKEN << std::endl;
}

int
main(int argc, char** argv)
{
    const segysdk::utilities::InputParser input(argc, argv);

    // Check if user has provided valid runtime parameters. If not, provide usage information
    if (input.cmdOptionExists("-h") || input.cmdOptionExists("-help") || argc < 2) {
        print_usage(argv[0], argc);
        return -1;
    }

    std::string osduFileCollection;
    const std::string& osduFileName = input.getCmdOption("-osdu");
    if (osduFileName.empty()) {
        std::cout << "Input FileCollection.SEGY parameter must be a valid file name!" << std::endl;
        return -1;
    }

    osduFileCollection = osduFileName;

    GenerationConfig conf(input);
    if (conf.useGrayScale_) {
        std::cout << "-grayScale option detected. Gray scale colour table will be used.";
    }

    if (conf.inlineNo_ == std::numeric_limits<int>::min() && conf.crosslineNo_ == std::numeric_limits<int>::min()) {
        std::cout << "generateSectionImage: Either valid inline or valid crossline number must be specified!" << std::endl;
        return -5;
    }

    // Initiate connection to the SegySDK API.
    // Create a SegySession - This is the main programmer interface into the API.
    // From the instantiated SEG-Y session the API client can start generation of the SegyIndex,
    // or get the SegyIndex accessor if it has already been generated,
    // instantiate a new bulk data reader or
    // extract seismic metadata, e.g. textual headers (both standard or extended headers), binary headers and trace headers.
    const auto session = SampleCode::Common::SessionUtilities::GetSegySessionFromFileCollectionJson(osduFileCollection);
    if (session == nullptr) {
        std::cout << "generateSectionImage: Unable to create a valid SEG-Y session, exiting!" << std::endl;
        return -10;
    }

    try {
        bool doLoop = true;
        while (doLoop) {

            auto image = generateSectionImage(session, conf);

            if (!image.empty()) {
                const auto result = std::system(image.c_str());

                if (result == 0) {
                    std::cout << "GenerateSectionImage completed OK" << std::endl;
                }
                else {
                    std::cout << "GenerateSectionImage completed with return status  = " << std::to_string(result) << std::endl;
                }
            }
            else {
                std::cout << "GenerateSectionImage did not generate an output BMP file for selected section." << std::endl;
            }

            std::cout << std::endl
                      << "Enter new line number or 'q' to quit : ";

            std::string response;
            std::cin >> response;

            doLoop = response != "q" && response != "Q";

            if (doLoop) {
                if (conf.inlineNo_ > std::numeric_limits<int>::min()) {
                    conf.inlineNo_ = stoi(response);

                    std::cout << "Generating section for inline " << std::to_string(conf.inlineNo_) << " ... " << std::endl;
                }
                else {
                    conf.crosslineNo_ = stoi(response);
                    std::cout << "Generating section for crossline " << std::to_string(conf.crosslineNo_) << " ... " << std::endl;
                }
            }
        }
    }
    catch (std::exception& ex) {
        std::cout << "Error generating seismic image! Error: " << ex.what() << std::endl;
        return -100;
    }
    return 0;
}
