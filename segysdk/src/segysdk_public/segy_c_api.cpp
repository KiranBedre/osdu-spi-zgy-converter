/*
Copyright 2021-2025 Schlumberger

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

#include "../LibraryLoader/DynamicLibraryLoader.h"
#include "../NavigationExtractorPlugin/InputParams.h"
#include "../NavigationExtractorPlugin/NavigationExtractorPlugin.h"
#include "../StatisticsGeneratorPlugin/StatisticsGeneratorPlugin.h"
#include "../StatisticsGeneratorPlugin/StatisticsInputParams.h"
#include "../Utilities/Authorization.h"
#include "../Utilities/CrsConverterSvcClient.h"
#include "../Utilities/JsonUtilities.h"
#include "../Utilities/PluginUtilities.h"
#include "../segysdk_details/SegyException.h"
#include "../segysdk_public/internal_api.h"
#include "../SegyFormat/External/SegyBinaryHeaderStringConsts.h"
#include "../SegyIO/Primitives.h"

#include "segy_c_api.h"
#include <cpprest/asyncrt_utils.h>
#include <cstring>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <tuple>

namespace
{
std::map<void*, std::list<std::string*>> session_managed_resource;

std::string sd_url{};
std::string sd_api_key{};
std::mutex session_resource_mutex_;
} // namespace

static const char*
keepStringCopy(void* session, const std::string& str)
{
    std::unique_lock<std::mutex> lock(session_resource_mutex_);
    const auto copied = new std::string(str);
    if (session_managed_resource.find(session) != session_managed_resource.end()) {
        session_managed_resource[session].push_back(copied);
    }
    else {
        std::list<std::string*> resourceList;
        resourceList.push_back(copied);
        session_managed_resource.try_emplace(session, resourceList);
    }

    return copied->c_str();
}

void
segy_configure_remote_access(const char* sdms_url, const char* sdms_app_key, const char* token)
{
    sd_url = sdms_url;
    sd_api_key = sdms_app_key;
    segysdk::utilities::TokenSourceFactory::configureForStaticToken(token);
}

void
segy_add_http_header(const char* header_name, const char* header_value)
{
    segysdk::utilities::Settings::instance().addHttpHeader(header_name, header_value);
}

static void
clear_error(segy_api_error* error)
{
    if (error == nullptr)
        return;
    error->is_error = false;
}

static void
set_error(segy_api_error* error, void* session, const std::exception& ex)
{
    if (error == nullptr)
        return;
    error->is_error = true;
    error->message = keepStringCopy(session, ex.what());
}

SegySession*
segy_create_contextual_session(const char* segy_file_name, const char* loading_parameter_json, const char* token, const char* sdms_url, const char* sdms_api_key, const char* headers, size_t page_size, size_t max_pages, segy_api_error* error)
{
    clear_error(error);
    try {
        std::map<std::string, std::string> http_headers;
        const auto headers_json = segysdk::utilities::JsonUtilities::getJsonFromString(headers);
        for (std::string key : headers_json.getMemberNames()){
            http_headers.emplace(key, headers_json[key].asString());
        }

        std::vector<std::string> segyFileList;
        segyFileList.emplace_back(segy_file_name);
        const SegySessionFactory factory(segyFileList, loading_parameter_json);

        auto tokenSource = segysdk::utilities::TokenSourceFactory::getTokenSource(token != nullptr ? std::string(token) : "");

        auto session = factory.CreateSegySession(
            segysdk::SDCredentials(sdms_url, sdms_api_key, std::move(tokenSource), http_headers),
            segysdk::CacheSizeOption(page_size, max_pages, 1, page_size)
        );

        return session.release();
    }
    catch (const std::exception& ex) {
        // will leak the error message in this case because no session pointer to attach it to
        set_error(error, nullptr, ex);
        return nullptr;
    }
}

SegySession*
segy_create_session(const char* segyFileName, const char* loadingParameterJson, const char* token, segy_api_error* error)
{
    return segy_create_contextual_session(segyFileName, loadingParameterJson, token, sd_url.c_str(), sd_api_key.c_str(), "{}", 1048576, 1024, error);
}


SegySession*
segy_create_osdu_session(const char* fileCollectionSegy, const char* token, segy_api_error* error)
{
    clear_error(error);
    try {
        const SegySessionFactory factory(fileCollectionSegy);

        auto tokenSource = segysdk::utilities::TokenSourceFactory::getTokenSource(token != nullptr ? std::string(token) : "");

        auto session = factory.CreateSegySession(
            segysdk::SDCredentials{sd_url, sd_api_key, std::move(tokenSource)},
            segysdk::CacheSizeOption(4 * 1048576, 1024, 1, 1024U * 1024U) // TODO defaults are hardcoded for now
        );

        return session.release();
    }
    catch (const std::exception& ex) {
        // will leak the error message in this case because no session pointer to attach it to
        set_error(error, nullptr, ex);
        return nullptr;
    }
}


SegySession*
segy_create_segy_session(const char* segyFileName, const char* loadingParameterJson, segy_api_error* error)
{
    return segy_create_session(segyFileName, loadingParameterJson, "", error);
}

SegySession*
segy_create_session_from_json(const char* fileCollectionOSDU, const char* token, segy_api_error* error)
{
    clear_error(error);

    try {
        if (fileCollectionOSDU == segysdk::utilities::JsonUtilities::EmptyJson || !segysdk::utilities::JsonUtilities::isValidJson(fileCollectionOSDU)) {
            throw segysdk::SegyInvalidArgumentException("segy_create_segy_session_from_osdu_file_collection: FileCollection must be valid");
        }

        auto tokenSource = segysdk::utilities::TokenSourceFactory::getTokenSource(token != nullptr ? std::string(token) : "");

        const auto json = segysdk::utilities::JsonUtilities::getJsonFromString(fileCollectionOSDU);
        const auto fc100 = segysdk::utilities::osdu::OsduFileCollectionSegy100(json);
        const std::vector<std::string> files = fc100.segyFileNames();

        const SegySessionFactory factory(fileCollectionOSDU);

        return factory.CreateSegySession(
                          segysdk::SDCredentials{sd_url, sd_api_key, std::move(tokenSource)},
                          segysdk::CacheSizeOption(4U * 1048576, 1024, 1, 1024U * 1024U) // TODO defaults are hardcoded for now
                          )
            .release();
    }
    catch (const std::exception& ex) {
        // will leak the error message in this case because no session pointer to attach it to
        set_error(error, nullptr, ex);
        return nullptr;
    }
}

SegySession*
segy_create_segy_session_from_osdu_file_collection(const char* fileCollectionOSDU, segy_api_error* error)
{
    return segy_create_session_from_json(fileCollectionOSDU, "", error);
}

template<typename TResult>
static TResult
run_with_error_handling(void* session, std::function<TResult()> function, segy_api_error* error)
{
    clear_error(error);
    try {
        return function();
    }
    catch (const std::exception& ex) {
        set_error(error, session, ex);
        return 0;
    }
}

template<>
const char*
run_with_error_handling(void* session, std::function<const char*()> function, segy_api_error* error)
{
    clear_error(error);
    try {
        return keepStringCopy(session, function());
    }
    catch (const std::exception& ex) {
        set_error(error, session, ex);
        return nullptr;
    }
}

template<>
void
run_with_error_handling(void* session, std::function<void()> function, segy_api_error* error)
{
    clear_error(error);
    try {
        function();
    }
    catch (const std::exception& ex) {
        set_error(error, session, ex);
    }
}

char
segy_load_index(SegySession* session, segy_api_error* error)
{
    return run_with_error_handling<char>(
        session,
        std::function<char()>([&session]() {
            const auto index = session->getIndex();
            return index->hasIndexFile();
        }),
        error);
}

int
segy_get_revision(SegySession* session, segy_api_error* error)
{
    return run_with_error_handling<int>(
        session,
        std::function([&session]() { return session->getSegyVersion(); }),
        error);
}
char
segy_session_is_valid(SegySession* session, segy_api_error* error)
{
    return run_with_error_handling<char>(
        session,
        std::function<char()>([&session]() {
            const auto index = session->getIndex();
            return session->validateSession(); }),
        error);
}

char
segy_is_3d(SegySession* session, segy_api_error* error)
{
    return run_with_error_handling<char>(
        session,
        std::function<char()>([&session]() {
            const auto index = session->getIndex();
            return index->isSegy3D(); }),
        error);
}

char
segy_has_index(SegySession* session, segy_api_error* error)
{
    return run_with_error_handling<char>(
        session,
        std::function<char()>([&session]() {
            return session->getIndex()->hasIndexFile();
        }),
        error);
}

char
segy_has_missing_traces_index(SegySession* session, segy_api_error* error)
{
    return run_with_error_handling<char>(
        session,
        std::function<char()>([&session]() { return session->getIndex()->hasMissingTraceIndex(); }),
        error);
}


void segy_get_3D_file_extents(SegySession* session, long* first_inline, int* inline_step, long* inline_count, long* first_crossline, int* crossline_step, long* crossline_count, double* first_sample_depth, double* sample_interval, int* sample_count, segy_api_error* error)
{
    return run_with_error_handling<void>(
        session,
        std::function([&session, &first_inline, &inline_step, &inline_count, &first_crossline, &crossline_step, &crossline_count, &first_sample_depth, &sample_interval, &sample_count ]() {
            const auto index = session->getIndex();
            if (index->hasIndexFile()) {
                *first_inline = index->getFileLayoutExtents().getIVectorExtent().first();
                *inline_step = index->getFileLayoutExtents().getIVectorExtent().step();
                *inline_count = index->getFileLayoutExtents().getIVectorExtent().count();
                *first_crossline = index->getFileLayoutExtents().getJVectorExtent().first();
                *crossline_step = index->getFileLayoutExtents().getJVectorExtent().step();
                *crossline_count = index->getFileLayoutExtents().getJVectorExtent().count();
                *first_sample_depth = index->getFileLayoutExtents().getSampleExtent().first();
                *sample_interval = index->getFileLayoutExtents().getSampleExtent().step();
                *sample_count = index->getFileLayoutExtents().getSampleExtent().count();
            }
            else {
                *first_inline = 0;
                *inline_step = 1;
                *inline_count = 0;
                *first_crossline = 0;
                *crossline_step = 1;
                *crossline_count = 0;
                *first_sample_depth = 0;
                *sample_interval = 1;
                *sample_count = 0;
            }

        }),
        error);
}

void
segy_get_2D_line_extents(SegySession* session, long* first_cmp, int* cmp_step, long* cmp_count, double* first_sample_depth, double* sample_interval, int* sample_count, segy_api_error* error)
{
    return run_with_error_handling<void>(
        session,
        std::function([&session, &first_cmp, &cmp_step, &cmp_count, &first_sample_depth, &sample_interval, &sample_count]() {
            const auto index = session->getIndex();
            if (index->hasIndexFile()) {
                *first_cmp = index->getLineExtents().getCMPExtent().first();
                *cmp_step = index->getLineExtents().getCMPExtent().step();
                *cmp_count = index->getLineExtents().getCMPExtent().count();
                *first_sample_depth = index->getLineExtents().getSampleExtent().first();
                *sample_interval = index->getLineExtents().getSampleExtent().step();
                *sample_count = index->getLineExtents().getSampleExtent().count();
            }
            else {
                *first_cmp = 0;
                *cmp_step = 1;
                *cmp_count = 0;
                *first_sample_depth = 0;
                *sample_interval = 1;
                *sample_count = 0;
            }
        }),
        error);
}

bool
segy_set_vector_header_mapping(SegySession* session, const char* vectorHeaderMapping, bool forceRemapping, segy_api_error* error)
{
    return run_with_error_handling<int>(
        session,
        std::function<bool()>([&session, &vectorHeaderMapping, &forceRemapping]() { return session->setVectorHeaderMapping(vectorHeaderMapping, forceRemapping); }),
        error);
}

int
segy_get_trace_header_field_count(SegySession* session, segy_api_error* error)
{
    return run_with_error_handling<int>(
        session,
        std::function<int()>([&session]() { return session->getTraceHeaderFieldCount(); }),
        error);
}

const char*
segy_get_ascii_headers(SegySession* session, segy_api_error* error)
{
    return run_with_error_handling<const char*>(
        session,
        std::function([&session]() { return keepStringCopy(session, session->getAsciiHeaders()); }),
        error);
}

const char*
segy_get_ascii_headers_by_line(SegySession* session, segy_api_error* error)
{
    return run_with_error_handling<const char*>(
        session,
        std::function([&session]() { return keepStringCopy(session, session->getAsciiHeaders()); }),
        error);
}

void
segy_get_selected_scaled_trace_header_values(SegySession* session, long start_trace, int traces_to_dump, int header_ids_count, const char** header_ids, double* buffer, size_t buffer_allocated_bytes, int* returned_traces, segy_api_error* error)
{
    return run_with_error_handling<void>(
        session,
        std::function([&session, &start_trace, &traces_to_dump, &header_ids_count, &header_ids, &buffer, &buffer_allocated_bytes, &returned_traces]() {
            *returned_traces = 0;
            constexpr auto item_size = sizeof(double);
            if (buffer_allocated_bytes < item_size * traces_to_dump * header_ids_count)
                throw std::runtime_error("Not enough buffer for output.");

            std::vector<std::string> headers;
            for (auto i = 0; i < header_ids_count; ++i) {
                headers.emplace_back(header_ids[i]);
            }
            const auto result = session->getSelectedScaledTraceHeaderValues(start_trace, traces_to_dump, headers);
            double* targetptr = buffer;
            for (const auto& tmp : result) {
                memcpy(targetptr, tmp.data(), header_ids_count * item_size);
                targetptr += header_ids_count;
            }
            *returned_traces = static_cast<int>(result.size());
        }),
        error);
}

const char*
segy_get_ascii_headers_as_json(SegySession* session, segy_api_error* error)
{
    return run_with_error_handling<const char*>(
        session,
        std::function([&session]() { return keepStringCopy(session, session->getAsciiHeadersAsJson()); }),
        error);
}

const char*
segy_get_ascii_headers_by_line_as_json(SegySession* session, segy_api_error* error)
{
    return run_with_error_handling<const char*>(
        session,
        std::function([&session]() { return keepStringCopy(session, session->getAsciiHeadersByLineAsJson()); }),
        error);
}

const char*
segy_get_extended_ascii_headers_as_json(SegySession* session, segy_api_error* error)
{
    return run_with_error_handling<const char*>(
        session,
        std::function([&session]() { return keepStringCopy(session, session->getExtendedAsciiHeadersAsJson()); }),
        error);
}

const char*
segy_get_extended_ascii_headers_by_line_as_json(SegySession* session, segy_api_error* error)
{
    return run_with_error_handling<const char*>(
        session,
        std::function([&session]() { return keepStringCopy(session, session->getExtendedAsciiHeadersByLineAsJson()); }),
        error);
}

const char*
segy_get_binary_header_as_json(SegySession* session, segy_api_error* error)
{
    return run_with_error_handling<const char*>(
        session,
        std::function([&session]() { return keepStringCopy(session, session->getBinaryHeadersAsJson()); }),
        error);
}

const char*
segy_get_raw_trace_headers_as_json(SegySession* session, long start_trace, int traces_to_dump, segy_api_error* error)
{
    return run_with_error_handling<const char*>(
        session,
        std::function([&session, start_trace, traces_to_dump]() {
            return keepStringCopy(session, session->getRawTraceHeadersAsJson(start_trace, traces_to_dump));
        }),
        error);
}

size_t
getMaxIndexedTraceCount(std::shared_ptr<segysdk::segyindex::SegyIndex> index)
{
    try {

        if (index->hasIndexFile()) {
            size_t maxTraces = index->getIndexedTraceCount();
            if (maxTraces == 0) {
                if (index->isSegy3D()) {
                    const auto inlineCount = index->getGeometry3D().JVectorExtent.count();
                    const auto xlineCount = index->getGeometry3D().IVectorExtent.count();
                    maxTraces = inlineCount * xlineCount;
                }
                else {
                    maxTraces = index->getLineExtents().getCMPExtent().count();
                }
            }
            return maxTraces;
        }
    }
    catch (const std::exception& e) {
        std::string msg = "getMaxIndexedTraceCount: Index Exception caught: " + std::string(e.what());
        SEGYLOG_REPORT_ERROR(msg);
    }
    return 0;
}

static size_t
getSampleSizeFromFormat(int sampleFormat)
{
    switch (sampleFormat) {
    case 1:
    case 2:
    case 4:
    case 5:
        return 4;
    case 3:
        return 2;
    case 8:
        return 1;
    default:
        return 4;
    }
}

static size_t
get_max_trace_count_from_index(SegySession* session, const std::shared_ptr<segysdk::segyindex::SegyIndex> index)
{
    size_t traceCount = 0;
    if (index->hasIndexFile()) {
        traceCount = index->getIndexedTraceCount();
        if (index->getIndexedTraceCount() > 0) {
            return traceCount;
        }

        // in case we have an old index where indexedTraceCount has not been populated
        const auto fileId = session->getSelectedSegyFileId();
        const auto offset = index->getFirstTraceOffset(fileId);
        const auto traceSize = index->getTraceSize();
        const auto fileSize = session->getSegyFileSize();
        traceCount = static_cast<double>(fileSize - offset) / traceSize;

        return traceCount;
    }

    return 0;
}

size_t
get_samplecount_from_binary_hdr(SegySession* session)
{
    const auto binaryHeaders = session->getBinaryHeaders();
    size_t samplesPerTrace = 0;
    for (const auto& hdr : binaryHeaders) {
        if (hdr.id == segysdk::segyformat::external::SegyBinaryHeaderStringConsts::SamplesPerTrace) {
            samplesPerTrace = hdr.value;
            break;
        }
    }
    return samplesPerTrace;
}

static size_t
get_max_trace_count(SegySession* session, const std::shared_ptr<segysdk::segyindex::SegyIndex> index)
{
    if (index->hasIndexFile()) {
        return get_max_trace_count_from_index(session, index);
    }

    // We would like this to work even if the index has not been generated.
    int samplesPerTrace = -1;

    int extHdrCount = -1;
    const auto binaryHeaders = session->getBinaryHeaders();


    int sampleFormat = -1;
    int segyRevision = 0;
    for (const auto& hdr : binaryHeaders) {
        if (hdr.id == segysdk::segyformat::external::SegyBinaryHeaderStringConsts::SamplesPerTrace) {
            samplesPerTrace = hdr.value;
        }
        if (hdr.id == segysdk::segyformat::external::SegyBinaryHeaderStringConsts::DataSampleFormatCode) {
            sampleFormat = static_cast<int>(hdr.value);
        }
        if (hdr.id == segysdk::segyformat::external::SegyBinaryHeaderStringConsts::ExtendedHeadersCount) {
            extHdrCount = static_cast<int>(hdr.value);
            if (extHdrCount < 0) {
                extHdrCount = session->getExtendedAsciiHeaders().size();
            }
        }
        if (hdr.id == segysdk::segyformat::external::SegyBinaryHeaderStringConsts::SEGYFormatRevisionNumber) {
            segyRevision = static_cast<int>(hdr.value);
        }
    }
    const auto sampleSize = getSampleSizeFromFormat(sampleFormat);
    const auto fileSize = session->getSegyFileSize();

    const auto extendedHeaderCount = segyRevision > 0 and extHdrCount > 0 ? extHdrCount : 0;
    size_t fileOffset = segysdk::segyio::Primitives::getCombinedHeaderSize(extendedHeaderCount);

    const auto traceSize = segysdk::segyio::Primitives::getTraceSize(sampleSize, samplesPerTrace);
    double result = (fileSize - fileOffset) * 1.0 / traceSize;

    return static_cast<size_t>(result);
}

void
segy_get_trace_samples(SegySession* session, long start_trace, int requested_traces, int sample_offset, int requested_samples, size_t buffer_allocated_bytes, int* returned_traces_count, int* returned_samples_count, float* buffer, segy_api_error* error)
{
    try {

        return run_with_error_handling<void>(
            session,
            std::function([&session, start_trace, requested_traces, sample_offset, requested_samples, &buffer, buffer_allocated_bytes, &returned_samples_count, &returned_traces_count]() {
                // sanity check for requested traces and samples
                auto index = session->getIndex();
                size_t traceCount = static_cast<size_t>(requested_traces);
                size_t startTrace = static_cast<size_t>(start_trace);

                size_t sampleCount = static_cast<size_t>(requested_samples);
                size_t sampleOffset = static_cast<size_t>(sample_offset);

                if (index->hasIndexFile()) {
                    auto maxTraces = getMaxIndexedTraceCount(index);
                    if (startTrace > maxTraces) {
                        startTrace = maxTraces - 2;
                    }
                    if (traceCount > maxTraces) {
                        traceCount = maxTraces;
                    }
                    if (startTrace + traceCount > maxTraces) {
                        traceCount = maxTraces - startTrace;
                    }

                    if (index->isSegy3D()) {
                        size_t sample_extent_count = static_cast<size_t>(index->getGeometry3D().sampleExtent.count());
                        if (sampleCount == 0 or sampleOffset + sampleCount > sample_extent_count) {
                            if (sampleOffset > sample_extent_count) {
                                sampleOffset = sample_extent_count - 2;
                            }
                            sampleCount = sampleOffset > 0
                                              ? sample_extent_count - sampleOffset
                                              : sample_extent_count;
                        }
                    }
                    else {
                        size_t sample_extent_count = static_cast<size_t>(index->getLineExtents().getSampleExtent().count());
                        if (sampleCount == 0 or sampleOffset + sampleCount > sample_extent_count) {
                            if (sampleOffset > sample_extent_count) {
                                sampleOffset = sample_extent_count - 2;
                            }
                            sampleCount = sampleOffset > 0
                                              ? sample_extent_count - sampleOffset
                                              : sample_extent_count;
                        }
                    }
                }
                else {
                    auto maxTraces = get_max_trace_count(session, index);
                    if (startTrace > maxTraces) {
                        startTrace = maxTraces - 2;
                    }
                    if (traceCount > maxTraces) {
                        traceCount = maxTraces;
                    }
                    if (startTrace + traceCount > maxTraces) {
                        traceCount = maxTraces - startTrace;
                    }

                    if (index->isSegy3D()) {
                        size_t sample_extent_count = get_samplecount_from_binary_hdr(session);
                        if (sampleCount == 0 or sampleOffset + sampleCount > sample_extent_count) {
                            if (sampleOffset > sample_extent_count) {
                                sampleOffset = sample_extent_count - 2;
                            }
                            sampleCount = sampleOffset > 0
                                              ? sample_extent_count - sampleOffset
                                              : sample_extent_count;
                        }
                    }
                    else {
                        size_t sample_extent_count = get_samplecount_from_binary_hdr(session);
                        if (sampleCount == 0 or sampleOffset + sampleCount > sample_extent_count) {
                            if (sampleOffset > sample_extent_count) {
                                sampleOffset = sample_extent_count - 2;
                            }
                            sampleCount = sampleOffset > 0
                                              ? sample_extent_count - sampleOffset
                                              : sample_extent_count;
                        }
                    }
                }
                constexpr auto item_size = sizeof(float);
                const auto required_bytes = item_size * traceCount * sampleCount;
                if (buffer_allocated_bytes < required_bytes) {
                    throw std::runtime_error("segy_get_trace_samples: Not enough buffer allocated for output. (" + std::to_string(buffer_allocated_bytes) + ") is less than required bytes (" + std::to_string(required_bytes) + ")");
                }
                std::vector<std::vector<float>> resultVector(traceCount);

                bool success = session->getTraceSamples(resultVector, startTrace, traceCount, sampleOffset, sampleCount);
                if (success && !resultVector.empty()) {
                    size_t traceLength = resultVector.at(0).size();
                    for (const auto& trace : resultVector) {
                        if (trace.size() > traceLength) {
                            traceLength = trace.size();
                        }
                    }

                    float* targetptr = buffer;
                    for (const auto& tmp : resultVector) {
                        memcpy(targetptr, tmp.data(), traceLength * item_size);
                        targetptr += traceLength;
                    }
                    *returned_traces_count = static_cast<int>(resultVector.size());
                    *returned_samples_count = static_cast<int>(traceLength);
                }
            }),
            error);
    }
    catch (std::exception& e) {
        std::cerr << "c++ segy_get_trace_samples: Exception caught: \n"
                  << e.what() << std::endl;
        throw;
    }
}

void
segy_get_trace_header_values(SegySession* session, long start_trace, int traces_to_dump, size_t buffer_allocated_bytes, int* returned_headers_count, int* returned_traces, char** header_ids, double* buffer, segy_api_error* error)
{

    return run_with_error_handling<void>(
        session,
        std::function([&session, start_trace, traces_to_dump, &header_ids, &buffer, buffer_allocated_bytes, &returned_headers_count, &returned_traces, &error]() {
            *returned_headers_count = 0;
            const size_t header_count = 8;
            //sanity check for requested traces
            const auto index = session->getIndex();

            const int maxTraces = get_max_trace_count(session, index);
            const int my_traces_to_dump = maxTraces > 0 and traces_to_dump > maxTraces ? maxTraces : traces_to_dump;

            constexpr auto item_size = sizeof(double);

            const auto required_bytes = item_size * my_traces_to_dump * header_count;
            if (buffer_allocated_bytes < required_bytes)
                throw std::runtime_error("segy_get_trace_header_values: Not enough buffer allocated for output. (" + std::to_string(buffer_allocated_bytes) + ") is less than required bytes (" + std::to_string(required_bytes) + ")");
            std::vector<std::vector<double>> headerValues;
            std::vector<std::string> headerNames;

            const auto success = session->getTraceHeaderValues(headerNames, headerValues, start_trace, my_traces_to_dump);
            if (success) {
                *returned_headers_count = headerNames.size();
                size_t ix = 0;
                for (auto& hdr : headerNames) {
                    char* pc = new char[hdr.size() + 1];
                    std::strcpy(pc, hdr.c_str());
                    header_ids[ix++] = pc;
                }

                *returned_traces = static_cast<int>(headerValues.size());
                double* targetptr = buffer;
                for (const auto& tmp : headerValues) {
                    memcpy(targetptr, tmp.data(), *returned_headers_count * item_size);
                    targetptr += *returned_headers_count;
                }
            }
            else {
                error->is_error = true;
                std::string msg = "getTraceHeaderValues failed to extract trace header value";
                error->message = msg.c_str();
                *returned_traces = -1;
            }
        }),
        error);
}

const char*
segy_get_traces_as_json(SegySession* session, long start_trace, int traces_to_dump, segy_api_error* error)
{
    return run_with_error_handling<const char*>(
        session,
        std::function([&session, start_trace, traces_to_dump]() {
            return keepStringCopy(session, session->getTracesAsJson(start_trace, traces_to_dump));
        }),
        error);
}

const char*
segy_get_scaled_trace_headers_as_json(SegySession* session, long start_trace, int traces_to_dump, segy_api_error* error)
{
    return run_with_error_handling<const char*>(
        session,
        std::function([&session, &start_trace, &traces_to_dump]() {
            return keepStringCopy(session, session->getScaledTraceHeadersAsJson(start_trace, traces_to_dump, session->isSegy3D()));
        }),
        error);
}

const char*
segy_get_survey_bin_grid(SegySession* session, segy_api_error* error)
{
    return run_with_error_handling<const char*>(
        session,
        std::function([&session]() {
            return keepStringCopy(session, session->getSurveyBinGrid());
        }),
        error);
}

void
segy_accept_and_register_settings(SegySession* session, segy_api_error* error)
{
    run_with_error_handling<void>(
        session,
        std::function([&session]() { session->acceptAndRegisterSettings(); }),
        error);
}

void
segy_load_from_saved_settings(SegySession* session, segy_api_error* error)
{
    run_with_error_handling<void>(
        session,
        std::function([&session]() { session->loadStoredSettings(); }),
        error);
}

void
segy_generate_index_file(SegySession* session, bool force_generate, bool force_overwrite, segy_api_error* error)
{
    run_with_error_handling<void>(
        session,
        std::function([&session, &force_generate, &force_overwrite]() { session->generateIndexFile(force_generate, force_overwrite); }),
        error);
}

void
segy_close_segy_session(SegySession* session, segy_api_error* error)
{
    std::unique_lock<std::mutex> lock(session_resource_mutex_);
    run_with_error_handling<void>(
        session,
        std::function([&session]() {
            if (session_managed_resource.find(session) != session_managed_resource.end()) {
                const auto resourceList = session_managed_resource[session];
                for (const std::string* resource : resourceList) {
                    delete resource;
                }

                session_managed_resource.erase(session);
            }

            delete session;
        }),
        error);
}

template<typename T>
static size_t
extent_count(T extent)
{
    return static_cast<size_t>((extent.last - extent.first) / extent.step + 1);
}

/* TODO - duplicated, temporary code until bulk reader is able to read the index - END TODO */

SegyBulkReader*
segy_create_bulk_reader(SegySession* segy_session, segy_api_error* error)
{
    return run_with_error_handling<std::unique_ptr<SegyBulkReader>>(
               nullptr, [&segy_session]() {
                   return segy_session->createBulkReader();
               },
               error)
        .release();
}

void
segy_bulk_read(SegyBulkReader* reader, int inline_index, int crossline_index, int sample_index, int inline_count, int crossline_count, int sample_count, float* buffer, size_t buffer_allocated_bytes, segy_api_error* error)
{
    run_with_error_handling<void>(
        reader,
        std::function([&buffer_allocated_bytes, &inline_count, &crossline_count, &sample_count, &inline_index, &crossline_index, &buffer, &sample_index, &reader]() {
            constexpr size_t item_size = sizeof(float);
            if (buffer_allocated_bytes < item_size * inline_count * crossline_count * sample_count) {
                throw std::runtime_error("Not enough buffer for output.");
            }
            reader->read(static_cast<size_t>(inline_index), static_cast<size_t>(crossline_index), static_cast<size_t>(sample_index),
                         static_cast<size_t>(inline_count), static_cast<size_t>(crossline_count), static_cast<size_t>(sample_count), buffer);
        }),
        error);
}

void
segy_close_bulk_reader(SegyBulkReader* reader, segy_api_error* error)
{
    run_with_error_handling<void>(reader, std::function([&reader]() { delete reader; }), error);
}

const char*
segy_read_text_file(SegySession* session, char* textFilePath, segy_api_error* error)
{
    return run_with_error_handling<const char*>(
        session,
        std::function([&session, textFilePath]() {return keepStringCopy(session, session->readTextFile(std::string(textFilePath))); }),
        error);
}

const char*
segy_get_osdu_meta_data(SegySession* session, char* path_or_id, segy_api_error* error)
{
    return run_with_error_handling<const char*>(
        session,
        std::function([&session, path_or_id]() { return keepStringCopy(session, session->getOsduMetadata(std::string(path_or_id))); }),
        error);
}

void
segy_set_is_2D(SegySession* segy_session, bool is2D, segy_api_error* error)
{
    return run_with_error_handling<void>(segy_session, std::function([&segy_session, is2D]() { segy_session->setSegy2DFlag(is2D); }), error);
}


SEGYSDK_DLL_PUBLIC Segy2DReader*
segy_create_2d_reader(SegySession* segy_session, segy_api_error* error)
{
    return run_with_error_handling<std::unique_ptr<Segy2DReader>>(
               nullptr, [&segy_session]() {
                   return segy_session->create2DReader();
               },
               error)
        .release();
}

SEGYSDK_DLL_PUBLIC void
segy_2d_read_cmp(Segy2DReader* reader, int cmp_start, int cmp_end, size_t sample_start, size_t sample_end, float* buffer, size_t buffer_allocated_bytes, segy_api_error* error)
{
    run_with_error_handling<void>(
        reader,
        std::function([&buffer_allocated_bytes, &cmp_start, &cmp_end, &sample_start, &sample_end, &buffer, &reader]() {
            constexpr size_t item_size = sizeof(float);
            size_t requested_samples = (std::abs(cmp_end - cmp_start) + 1) * (sample_end - sample_start + 1);
            if (buffer_allocated_bytes < item_size * requested_samples) {
                throw std::runtime_error("Not enough buffer for output.");
            }
            reader->readByCmp(cmp_start, cmp_end, static_cast<size_t>(sample_start), static_cast<size_t>(sample_end), buffer);
        }),
        error);
}

SEGYSDK_DLL_PUBLIC void
segy_2d_read_traces(Segy2DReader* reader, size_t trace_start, size_t trace_end, size_t sample_start, size_t sample_end, float* buffer, size_t buffer_allocated_bytes, segy_api_error* error)
{
    run_with_error_handling<void>(
        reader,
        std::function([&buffer_allocated_bytes, &trace_start, &trace_end, &sample_start, &sample_end, &buffer, &reader]() {
            constexpr size_t item_size = sizeof(float);
            size_t requested_samples = (trace_end - trace_start + 1) * (sample_end - sample_start + 1);
            if (buffer_allocated_bytes < item_size * requested_samples) {
                throw std::runtime_error("Not enough buffer for output.");
            }
            reader->readByTraceNo(static_cast<size_t>(trace_start), static_cast<size_t>(trace_end),
                                  static_cast<size_t>(sample_start), static_cast<size_t>(sample_end), buffer);
        }),
        error);
}

SEGYSDK_DLL_PUBLIC void
segy_close_2d_reader(Segy2DReader* reader, segy_api_error* error)
{
    run_with_error_handling<void>(reader, std::function([&reader]() { delete reader; }), error);
}

void
segy_dummy_reference_keeping_function(SegySession* session, segy_api_error* error)
{
    run_with_error_handling<void>(
        session, [&session]() -> void {
            {
                [[maybe_unused]] segysdk::utilities::auth::Jwt jwt{"boo"};
            }

            {
                int argc = 1;
                char const* argv[] = {"boo"};
                segysdk::utilities::InputParser inputParser(argc, argv);
            }

            {
                [[maybe_unused]] auto const check = segysdk::utilities::isSDDataset("boo");
            }

            {
                [[maybe_unused]] segysdk::utilities::CrsConverterSvcClient boo(segysdk::utilities::Authorization::storageSvcTokenProvider());
            }

            {
                char buff[10] = {};
                segysdk::segyio::AsciiToEbcdic::bytesToEbcdic("a", buff, 1U);
            }

            {
                segysdk::utilities::BinGrid boo;
                [[maybe_unused]] auto const coord = boo.getCoordinatesIJ(0.0, 0.0);
            }

            {
                auto const boo1 = segysdk::utilities::JsonBinGrid::loadBinGridFromFile("boo1");
                [[maybe_unused]] auto const coord1 = boo1.getCoordinatesIJ(0.0, 0.0);
                auto const boo2 = segysdk::utilities::JsonBinGrid::loadBinGridFromContent("boo2");
                [[maybe_unused]] auto const coord2 = boo2.getCoordinatesIJ(0.0, 0.0);
                [[maybe_unused]] auto const boo3 = segysdk::utilities::JsonBinGrid::parseTransformationMethodCode({});
            }

            {
                segysdk::segyindex::trace_range_map::TraceIndexStorage boo;
            }

            {
                [[maybe_unused]] segysdk::utilities::BrickSize boo("");
            }

            {
                [[maybe_unused]] segysdk::utilities::osdu::OsduFieldNameLiterals boo;
            }
            {
                [[maybe_unused]] const auto pluginPath = segysdk::utilities::PluginUtilities::getPluginPath("boo");
            }
            {
                [[maybe_unused]] segysdk::utilities::osdu::OsduFileCollectionSegy100 boo({});
            }

            {
                [[maybe_unused]] segysdk::utilities::osdu::OsduObject boo({});
            }

            {
                [[maybe_unused]] auto const boo = segysdk::utilities::GuidGenerator::newGuid();
            }

            {
                const std::string boo1 = segysdk::utilities::osdu::WellKnownTypes::SeismicTraceData;
                const std::string boo2 = segysdk::utilities::osdu::WellKnownTypes::FileCollectionSEGY;
                const std::string boo3 = segysdk::utilities::osdu::WellKnownTypes::FileCollectionSlbOpenZGY;
            }

            {
                segysdk::utilities::osdu::OsduSeismicTraceData140 boo({});
                [[maybe_unused]] auto sut = segysdk::utilities::osdu::OsduSeismicTraceDataFactory::getOsduSeismicTraceData({});
            }

            {
                [[maybe_unused]] segysdk::utilities::osdu::OsduWorkProduct100 boo({});
            }

            {
                [[maybe_unused]] segysdk::utilities::EnsureWrapper boo({});
            }

            {
                segysdk::concurrent::ThreadPool boo(2U);
            }

            {
                segysdk::concurrent::details::SemaphoreCounter boo(0U, 100U);
            }

            {
                std::vector<std::shared_ptr<segysdk::fileio::FileReader>> files;
                std::shared_ptr<segysdk::segyio::DerivedHeader> derivedHeader;
                segysdk::segyio::HeaderDefinitionProvider hdp({}, /*raw*/ false);
                segysdk::segyio::TraceSequentialScanner boo(std::move(files),
                                                            derivedHeader,
                                                            hdp,
                                                            hdp.getHeaderNames());
            }

            {
                segysdk::segyformat::external::LoadContextFromDelfiJson boo({});
            }

            {
                segysdk::segyformat::external::LoadContextFromOSDUJson boo({});
            }

            {
                segysdk::segyformat::external::SegyContext boo;
            }

            {
                segysdk::api::GenericFileReaderFactory boo({});
            }

            {
                std::vector<std::shared_ptr<segysdk::fileio::FileReader>> files;
                segysdk::fileio::SequentialFileReader boo(segysdk::fileio::PipelineFileReaderParams{}.withFileReaders(files));
            }

            {
                std::string fileCollectionId = "fileCollectionId";
                std::string seismicTraceDataId = "seismicTraceDataId";
                std::string seismic2DInterpSetId = "seismic2DInterpSetId";
                segysdk::navigation_extractor::InputParams inputParams(fileCollectionId, seismicTraceDataId, seismic2DInterpSetId, true, true);
                if (inputParams.IsValid()) {
                    const std::string pluginPath = "D:/temp/dummyPlugin";
                    auto dynamicLoader = std::make_unique<segysdk::library_loader::DynamicLibraryLoader>(pluginPath);
                    [[maybe_unused]] segysdk::navigation_extractor::NavigationExtractorPlugin navPlugin(std::move(dynamicLoader));
                }
            }
            {
                segysdk::api::SegyIndexFactory boo;

                std::vector<std::shared_ptr<segysdk::segyio::SegyFile>> files;
                std::shared_ptr<segysdk::segyindex::SegyIndex> boo1 = boo.CreateSegyIndex(files,
                                                                                          /*segyFileNames*/ {},
                                                                                          /*fileReaderOptions*/ {},
                                                                                          /*indexBaseFileNameOverride*/ std::nullopt);
                boo1->Load();
            }
            {
                const std::string testFile = "sd://opendes/sntc/seismic/test.sgy";
                const std::string legalTag = "legalTag";
                if (!segysdk::utilities::JsonUtilities::isLocalFile(testFile)) {
                    [[maybe_unused]] const auto readerOptions = segysdk::utilities::JsonUtilities::getFileReaderOptions(testFile, legalTag);
                }
            }
            {
                const std::string testFile = "D:\\Temp\\test.sgy";
                const auto legalTag = std::nullopt;
                if (!segysdk::utilities::JsonUtilities::isLocalFile(testFile)) {
                    [[maybe_unused]] const auto readerOptions = segysdk::utilities::JsonUtilities::getFileReaderOptions(testFile, legalTag);
                }
            }
            {
                std::unique_ptr<segysdk::utilities::auth::TokenSource> token_source = segysdk::utilities::auth::TokenSource::fromToken("");
                segysdk::SDCredentials boo(/*url*/ "",
                                           /*apiKey*/ "",
                                           std::move(token_source));
                segysdk::fileio::FileReference boo2
                    = segysdk::fileio::utilities::FileReferenceUtilities::getGenericFileReference(/*fileName*/ {},
                                                                                                  /*credentials*/ boo,
                                                                                                  /*isLocal*/ true,
                                                                                                  /*cacheSize*/ 6U,
                                                                                                  /*pageCount*/ 7U);
            }

            {
                std::time_t time{1615743865};
                std::tm tm{};

                segysdk::utilities::safe_gmtime(time, tm);
            }

            {
                segysdk::logger::LoggerSettings settings{};
                auto noPath = std::filesystem::path();
                segysdk::library_loader::PluginConfig plugin_config(noPath);
                const auto guid = segysdk::utilities::GuidGenerator::newGuid();
                settings.setCorrelationId(guid);
                settings.setFileCollectionId(guid);
                settings.setLogWriterPluginPath(plugin_config);
                segysdk::logger::initializeLogger(settings);
                SEGYLOG(segysdk::logger::LogLevel::Info, "Linkage issues");
                SEGYLOG_GSM_INFO("Another issue reported");
            }

            {
                [[maybe_unused]] segysdk::utilities::ZgyOutputParams params{};
            }

            {
                [[maybe_unused]] auto const boo = segysdk::utilities::Options::parse(/*argc*/ 0,
                                                                                     /*argv*/ nullptr,
                                                                                     segysdk::utilities::OptionsFileType::Uninitialized);
            }

            {
                [[maybe_unused]] auto const boo1 = segysdk::utilities::auth::TokenSource::fromToken("Boo1");
                [[maybe_unused]] auto const boo2 = segysdk::utilities::auth::TokenSource::fromAuthenticationPlugin("Boo2", 0U);
            }

            {
                [[maybe_unused]] auto const boo1 = segysdk::auth::AuthPluginFactory::instance().CreatePluginInteface("Boo1", 0U);
                [[maybe_unused]] auto const boo2 = segysdk::auth::AuthPluginFactory::instance().CreatePluginInteface("", 0U);
            }

            {
                segysdk::api::SegyIndexFactory boo;
                std::vector<std::shared_ptr<segysdk::segyio::SegyFile>> files;
                std::shared_ptr<segysdk::segyindex::SegyIndex> boo1 = boo.CreateSegyIndex(files,
                                                                                          /*segyFileNames*/ {},
                                                                                          /*fileReaderOptions*/ {},
                                                                                          /*indexBaseFileNameOverride*/ std::nullopt);
                auto loader = std::make_unique<segysdk::library_loader::DynamicLibraryLoader>("");
                segysdk::compute_polygon::ComputePolygonPlugin p(std::move(loader));
                bool isMultiple = false;
                p.computeForSegy(boo1, nullptr, isMultiple);
            }

            {
                using segySession = segysdk::utilities::SessionUtilities;
                const std::string osduFile = "none";
                [[maybe_unused]] auto const isCloud = segySession::TestIfCloudSession(osduFile);
                auto const segySession2 = segySession::GetSegySessionFromFileCollectionJson(osduFile);
                [[maybe_unused]] auto index = segySession::GetSegyIndex(segySession2);
            }

            {
                segysdk::logger::ProgressReporting report(std::chrono::seconds(1), "check");
                report.report("some message", 1.0);
            }

            {
                using segySession = segysdk::utilities::SessionUtilities;
                const std::string osduFile = "none";
                [[maybe_unused]] auto const isCloud = segySession::TestIfCloudSession(osduFile);
                auto const segySession2 = segySession::GetSegySessionFromFileCollectionJson(osduFile);
                [[maybe_unused]] auto index = segySession::GetSegyIndex(segySession2);
            }

            {
                const std::string fileCollectionSegyId = "fileCollectionSegyId";
                const std::string seismicTraceDataId = "seismicTraceDataId";
                const std::string workProductId = "workProductId";
                const bool forceOverwrite = true;
                const bool setVerbose = true;

                segysdk::statistics_generator::StatisticsInputParams inputParams(fileCollectionSegyId, seismicTraceDataId, workProductId, forceOverwrite, setVerbose);
                [[maybe_unused]] auto isOk2 = inputParams.IsValid();

                if (inputParams.IsValid()) {
                    const std::string pluginPath = "D:/temp/dummyPlugin";
                    auto dynamicLoader = std::make_unique<segysdk::library_loader::DynamicLibraryLoader>(pluginPath);
                    [[maybe_unused]] segysdk::statistics_generator::StatisticsGeneratorPlugin statsPlugin(std::move(dynamicLoader));
                }
            }

            const auto partition{"namespace"};
            const std::string json_struct = "{}";

            {
                using namespace segysdk::metadata::osdu;
                {
                    [[maybe_unused]] segysdk::metadata::osdu::OsduMetadataHelper helper1;
                    [[maybe_unused]] segysdk::metadata::osdu::OsduMetadataHelper helper2(partition);
                }
                auto myJson = segysdk::utilities::JsonUtilities::getJsonFromString(json_struct);
                {
                    const std::string myOsduPartition{"myOsduPartition"};
                    segysdk::metadata::osdu::OsduMetadataHelper helper(partition);

                    const std::string fileCollectionSegy = "dataset--FileCollection.SEGY";
                    segysdk::metadata::osdu::OsduFileCollectionSegyStruct fc_segy_Struct(myOsduPartition);
                    [[maybe_unused]] bool isValidSegy = fc_segy_Struct.isValid();

                    fc_segy_Struct.commonMetadataStruct.kind = segysdk::metadata::osdu::OsduMetadataHelper::osduWksReferenceLookupString(fileCollectionSegy, "1.0.0");
                    segysdk::metadata::osdu::OsduFileCollectionSegy fc_segy(fc_segy_Struct);
                    isValidSegy = fc_segy.isValidInput();

                    const std::string fileHistogram = "dataset--File.SeismicHistogram";
                    segysdk::metadata::osdu::OsduFileHistogramStruct histogramStruct(myOsduPartition);
                    histogramStruct.commonMetadataStruct.kind = segysdk::metadata::osdu::OsduMetadataHelper::osduWksReferenceLookupString(fileHistogram, "1.0.0");

                    const std::string seismicTraceData_string = "work-product-component--SeismicTraceData";
                    segysdk::metadata::osdu::OsduSeismicTraceDataStruct seismicTraceDataStruct(myOsduPartition);
                    [[maybe_unused]] bool isValidSeismicTraceData = seismicTraceDataStruct.isValid();

                    segysdk::metadata::osdu::OsduSeismicTraceData seismicTraceData(seismicTraceDataStruct);
                    auto ptr = seismicTraceData.getMetaDataStruct();

                    const std::string seismicTraceNavigation_string = "content--SeismicTraceNavigation";
                    segysdk::metadata::osdu::OsduSeismicTraceNavigationStruct seismicTraceNavigationStruct(myOsduPartition);
                    [[maybe_unused]] bool isValidSeismicTraceNavigation = seismicTraceNavigationStruct.isValid();

                    segysdk::metadata::osdu::OsduSeismicTraceNavigation seismicTraceNavigation(seismicTraceNavigationStruct);
                    auto ptrSeismicTraceNavigation = seismicTraceNavigation.getMetaDataStruct();

                    const std::string seismicLineGeomerty_string = "work-product-component--SeismicLineGeometry";
                    segysdk::metadata::osdu::OsduSeismicLineGeometryStruct seismicLineGeometryStruct(myOsduPartition);
                    [[maybe_unused]] bool isValidSeismicLineGeometry = seismicLineGeometryStruct.isValid();

                    segysdk::metadata::osdu::OsduSeismicLineGeometry seismicLineGeometry(myOsduPartition);
                    auto ptrSeismicLineGeometry = seismicLineGeometry.getMetaDataStruct();

                    const std::string fileSeismicLineGeomerty_string = "dataset--File.SeismicLineGeometry";
                    segysdk::metadata::osdu::OsduFileSeismicLineGeometryStruct fileSeismicLineGeometryStruct(myOsduPartition);
                    [[maybe_unused]] bool isValidFileSeismicLineGeometry = fileSeismicLineGeometryStruct.isValid();

                    segysdk::metadata::osdu::OsduFileSeismicLineGeometry fileSeismicLineGeometry(myOsduPartition);
                    auto ptrFileSeismicLineGeometry = fileSeismicLineGeometry.getMetaDataStruct();

                    const std::string seismic2DInterpretationSet_string = "master-data--Seismic2DInterpretationSet";
                    segysdk::metadata::osdu::OsduSeismic2DInterpretationSetStruct seismic2DInterpretationSetStruct(myOsduPartition);
                    [[maybe_unused]] bool isValidSeismic2DInterpretationSet = seismic2DInterpretationSetStruct.isValid();

                    segysdk::metadata::osdu::OsduSeismic2DInterpretationSet seismic2DInterpretationSet(myOsduPartition);
                    auto ptrSeismic2DInterpretationSet = seismic2DInterpretationSet.getMetaDataStruct();

                    const std::string workProduct_string = "work-product--WorkProduct";
                    segysdk::metadata::osdu::OsduWorkProductStruct workProductStruct(myOsduPartition);
                    [[maybe_unused]] bool isValidWorkProduct = workProductStruct.isValid();

                    segysdk::metadata::osdu::OsduWorkProduct workProduct(myOsduPartition);
                    auto ptrWorkProduct = workProduct.getMetaDataStruct();

                    const std::string seismicBinGrid_string = "work-product-component--SeismicBinGrid";
                    segysdk::metadata::osdu::OsduSeismicBinGridStruct seismicBinGridStruct(myOsduPartition);
                    [[maybe_unused]] bool isValidSeismicBinGrid = seismicBinGridStruct.isValid();

                    segysdk::metadata::osdu::OsduSeismicBinGrid seismicBinGrid(myOsduPartition);
                    auto ptrSeismicBinGrid = seismicBinGrid.getMetaDataStruct();

                    {
                        segysdk::metadata::osdu::OsduGeoJsonPointPropertiesElementStruct pointPropElementStruct(myOsduPartition);
                        segysdk::metadata::osdu::OsduGeoJsonPointPropertiesElement pointPropElement(pointPropElementStruct);
                        [[maybe_unused]] bool isValid = pointPropElement.isValidInput();
                    }
                    {
                        segysdk::metadata::osdu::OsduGeoJsonPointPropertiesElementStruct pointPropElementStruct(myOsduPartition);
                        segysdk::metadata::osdu::OsduGeoJsonPointPropertiesElement pointPropElement(pointPropElementStruct);
                        [[maybe_unused]] bool isValid = pointPropElement.isValidInput();
                    }
                    {
                        const std::string refUom = "reference-data--UnitOfMeasure";

                        segysdk::metadata::osdu::OsduGeoJsonPointPropertiesElementStruct element(myOsduPartition);
                        segysdk::metadata::osdu::OsduContentSeismicHistogramStruct histo_content_struct(myOsduPartition);
                        histo_content_struct.seismicRecordID = helper.osduReferenceLookupString(seismicTraceData_string, "ST0202R08-time-volume");
                        histo_content_struct.amplitudeUnitOfMeasureID = helper.osduReferenceLookupString(refUom, "Euc");
                        [[maybe_unused]] bool isValid = histo_content_struct.isValid();

                        segysdk::metadata::osdu::OsduContentSeismicHistogram histo_content(histo_content_struct);
                        isValid = histo_content.isValidInput();
                    }
                    const auto common_metadata_struct = std::make_shared<segysdk::metadata::osdu::OsduCommonMetadataStruct>(myOsduPartition);
                    segysdk::metadata::osdu::OsduAclStruct acl(myOsduPartition);
                    acl.owners = {"someone@company.com"};
                    acl.viewers = {"someone@company.com"};
                    acl.setPartition("myOsduPartition");
                    if (acl.isValid()) {
                        common_metadata_struct->aclStruct = acl;
                    }
                    segysdk::metadata::osdu::OsduLegalStruct legal(myOsduPartition);
                    legal.otherRelevantDataCountries = {"US"};
                    legal.legalTags = {"Example legaltags"};
                    legal.status = "compliant";
                    if (common_metadata_struct->isValid()) {
                        common_metadata_struct->legalStruct = legal;
                    }
                    if (common_metadata_struct->isValid()) {
                        histogramStruct.commonMetadataStruct = *common_metadata_struct;
                    }

                    segysdk::metadata::osdu::OsduAbstractCommonResourcesStruct common_resources_struct(myOsduPartition);
                    common_resources_struct.existenceKind = "actual";
                    common_resources_struct.source = "Example Data Source";
                    common_resources_struct.resourceHomeRegionID = "US";
                    common_resources_struct.resourceHostRegionIDs = {"US"};

                    segysdk::metadata::osdu::OsduAbstractCommonResources common_resources(common_resources_struct);
                    bool isOk = common_resources.isValidInput();
                    if (isOk) {
                        const auto meta1 = common_resources.getMetaDataStruct();
                    }
                    segysdk::metadata::osdu::OsduAbstractDatasetStruct data_set_struct(myOsduPartition);
                    segysdk::metadata::osdu::OsduAbstractDataset dataset(data_set_struct);
                    isOk = dataset.isValidInput();
                    if (isOk) {
                        const auto meta2 = dataset.getMetaDataStruct();
                    }

                    segysdk::metadata::osdu::OsduAbstractFileSourceInfoStruct file_source_info_struct(myOsduPartition);
                    file_source_info_struct.fileSize = "1023";
                    file_source_info_struct.fileSource = "sd://somedomain/osdu/file.txt";
                    segysdk::metadata::osdu::OsduAbstractFileSourceInfo fsi(file_source_info_struct);
                    isOk = isOk && fsi.isValidInput();
                    if (isOk) {
                        const auto meta3 = fsi.getMetaDataStruct();
                    }

                    segysdk::metadata::osdu::OsduFileHistogram file_histogram(histogramStruct);
                    isOk = file_histogram.isValidInput();
                    if (isOk) {
                        const auto meta5 = file_histogram.getMetaDataStruct();
                    }
                    const auto qc_result = histogramStruct.performAttributeValidityScan();
                    const bool qc_failed = qc_result.empty();

                    const auto reference_ok = histogramStruct.isValidOSDUReferenceIdString(histogramStruct.commonMetadataStruct.kind);
                    [[maybe_unused]] const auto my_qc = !qc_failed && reference_ok;
                }
                {
                    segysdk::metadata::osdu::OsduGenericFileData fileData(myJson, partition);
                    [[maybe_unused]] const auto my_json = fileData.getAsJson();
                    [[maybe_unused]] const auto my_struct = std::reinterpret_pointer_cast<segysdk::metadata::osdu::OsduGenericFileDataStruct>(fileData.getMetaDataStruct());
                }
                {
                    segysdk::metadata::osdu::OsduFileHistogram histogram(myJson, partition);
                    [[maybe_unused]] const auto my_json = histogram.getAsJson();
                    [[maybe_unused]] const auto my_struct = std::dynamic_pointer_cast<segysdk::metadata::osdu::OsduFileHistogramStruct>(histogram.getMetaDataStruct());
                }
                {
                    segysdk::metadata::osdu::OsduAcl acl(myJson, partition);
                    [[maybe_unused]] const auto my_json = acl.getAsJson();
                    [[maybe_unused]] const auto my_members = acl.getJsonMemberNames();
                    [[maybe_unused]] const auto my_ok = acl.isValidInput();
                    [[maybe_unused]] const auto my_struct = acl.getMetaDataStruct();
                }
                {
                    segysdk::metadata::osdu::OsduAcl legal(myJson, partition);
                    [[maybe_unused]] const auto my_json = legal.getAsJson();
                    [[maybe_unused]] const auto my_members = legal.getJsonMemberNames();
                    [[maybe_unused]] const auto my_ok = legal.isValidInput();
                    [[maybe_unused]] const auto my_struct = legal.getMetaDataStruct();
                }
                {
                    segysdk::metadata::osdu::OsduAncestry ancestry(myJson, partition);
                    [[maybe_unused]] const auto my_json = ancestry.getAsJson();
                    [[maybe_unused]] const auto my_members = ancestry.getJsonMemberNames();
                    [[maybe_unused]] const auto my_ok = ancestry.isValidInput();
                    [[maybe_unused]] const auto my_struct = ancestry.getMetaDataStruct();
                }

                {
                    segysdk::metadata::osdu::OsduAbstractFileSourceInfo file_info(myJson, partition);
                    [[maybe_unused]] const auto my_json = file_info.getAsJson();
                    [[maybe_unused]] const auto my_members = file_info.getJsonMemberNames();
                    [[maybe_unused]] const auto my_ok = file_info.isValidInput();
                    [[maybe_unused]] const auto my_struct = file_info.getMetaDataStruct();
                }
                {
                    segysdk::metadata::osdu::OsduMetaArray meta_array(myJson, partition);
                    [[maybe_unused]] const auto my_json = meta_array.getAsJson();
                    [[maybe_unused]] const auto my_members = meta_array.getJsonMemberNames();
                    [[maybe_unused]] const auto my_ok = meta_array.isValidInput();
                    [[maybe_unused]] const auto my_struct = meta_array.getMetaDataStruct();
                }

                {
                    segysdk::metadata::osdu::OsduVectorHeaderMappingElement vhmElement(myJson, partition);
                    [[maybe_unused]] const auto my_json = vhmElement.getAsJson();
                    [[maybe_unused]] const auto my_members = vhmElement.getJsonMemberNames();
                    [[maybe_unused]] const auto my_ok = vhmElement.isValidInput();
                    [[maybe_unused]] const auto my_struct = vhmElement.getMetaDataStruct();
                    [[maybe_unused]] const auto my_struct_ok = my_struct->isValid();
                }
                {
                    segysdk::metadata::osdu::OsduVectorHeaderMapping vhm(myJson, partition);
                    [[maybe_unused]] const auto my_json = vhm.getAsJson();
                    [[maybe_unused]] const auto my_members = vhm.getJsonMemberNames();
                    [[maybe_unused]] const auto my_ok = vhm.isValidInput();
                    [[maybe_unused]] const auto my_struct = vhm.getMetaDataStruct();
                    [[maybe_unused]] const auto my_struct_ok = my_struct->isValid();
                }

                {
                    segysdk::metadata::osdu::OsduOverridesBinaryHeader vhmElement(myJson, partition);
                    [[maybe_unused]] const auto my_json = vhmElement.getAsJson();
                    [[maybe_unused]] const auto my_members = vhmElement.getJsonMemberNames();
                    [[maybe_unused]] const auto my_ok = vhmElement.isValidInput();
                    [[maybe_unused]] const auto my_struct = vhmElement.getMetaDataStruct();
                    [[maybe_unused]] const auto my_struct_ok = my_struct->isValid();
                }
                {
                    segysdk::metadata::osdu::OsduOverridesTraceHeader vhm(myJson, partition);
                    [[maybe_unused]] const auto my_json = vhm.getAsJson();
                    [[maybe_unused]] const auto my_members = vhm.getJsonMemberNames();
                    [[maybe_unused]] const auto my_ok = vhm.isValidInput();
                    [[maybe_unused]] const auto my_struct = vhm.getMetaDataStruct();
                    [[maybe_unused]] const auto my_struct_ok = my_struct->isValid();
                }
                {
                    OsduMetadataHelper helper(partition);

                    OsduFileCollectionOpenZgyStruct zgyFile(partition);
                    zgyFile.commonMetadataStruct.kind = helper.osduWksReferenceLookupString(OsduFileCollectionOpenZgyStruct::getClassName(), OsduFileCollectionOpenZgyStruct::getClassVersion());
                    OsduFileCollectionOpenZgyDataStruct dataStruct(partition);
                    OsduAbstractSeismicPrecisionStruct precisionStruct(partition);
                    precisionStruct.setPrecision(segysdk::segyio::SampleFormat::IBMFloat4);
                    dataStruct.precisionStruct = precisionStruct;
                    OsduAbstractCompressionInfoStruct compressionInfoStruct(partition);
                    segysdk::utilities::ZgyOutputParams zgyOutputParams(OpenZGY::SampleDataType::float32, true, 70, true, 50);
                    constexpr size_t fileSize = 20235345;
                    compressionInfoStruct.setCompressionInfo(zgyOutputParams, fileSize);
                    dataStruct.compressionInfoStruct = compressionInfoStruct;
                    zgyFile.dataStruct = dataStruct;

                    OsduFileCollectionOpenZgy zgy(zgyFile);
                    [[maybe_unused]] const auto json = zgy.getAsFormattedString();
                }
                {
                    auto factory = segysdk::api::SegyServiceSessionFactory("dummy_path");
                    auto serviceSesssion = factory.createSegyServiceSession();
                    serviceSesssion->getTraceNavigation();
                }
                {
                    std::vector<std::shared_ptr<segysdk::segyio::SegyFile>> files;
                    segysdk::api::SegyIndexFactory boo;
                    std::shared_ptr<segysdk::segyindex::SegyIndex> boo1 = boo.CreateSegyIndex(files,
                                                                                              /*segyFileNames*/ {},
                                                                                              /*fileReaderOptions*/ {},
                                                                                              /*indexBaseFileNameOverride*/ std::nullopt);
                    auto connector = std::make_shared<segysdk::bulkaccess::live::ReaderConnector>(
                        segysdk::bulkaccess::live::ReaderConnectorParams{}.withSegyFile(files[0]).withSegyIndex(boo1));
                    auto reader = async::ReaderImpl{std::move(connector)};
                    reader.read(async::ReaderParams{}
                                    .withInline({1, 10})
                                    .withXline({1, 10})
                                    .withSamples({0, 10}));
                }
            }
        },
        error);
}
