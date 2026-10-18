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

#include "../segysdk_public/api.h"

using namespace segysdk::api;
using namespace segysdk::bulkaccess;

extern "C" {
typedef struct
{
    bool is_error;
    const char* message;
} segy_api_error;

SEGYSDK_DLL_PUBLIC void segy_configure_remote_access(const char* sdms_url, const char* sdms_app_key, const char* token);

SEGYSDK_DLL_PUBLIC void segy_add_http_header(const char* header_name, const char* header_value);

[[deprecated("Use segy_create_session.")]] SEGYSDK_DLL_PUBLIC SegySession* segy_create_segy_session(const char* segy_filename, const char* loading_parameter_json, segy_api_error* error);

SEGYSDK_DLL_PUBLIC SegySession* segy_create_session(const char* segyFileName, const char* loadingParameterJson, const char* token, segy_api_error* error);

SEGYSDK_DLL_PUBLIC SegySession* segy_create_contextual_session(const char* segy_file_name, const char* loading_parameter_json, const char* token, const char* sdms_url, const char* sdms_api_key, const char* headers, size_t page_size, size_t max_pages, segy_api_error* error);

SEGYSDK_DLL_PUBLIC SegySession* segy_create_osdu_session(const char* fileCollectionSegy, const char* token, segy_api_error* error);

[[deprecated("Use segy_create_session_from_json.")]] SEGYSDK_DLL_PUBLIC SegySession* segy_create_segy_session_from_osdu_file_collection(const char* fileCollectionOSDU, segy_api_error* error);

SEGYSDK_DLL_PUBLIC SegySession* segy_create_session_from_json(const char* fileCollectionOSDU, const char* token, segy_api_error* error);

SEGYSDK_DLL_PUBLIC char segy_load_index(SegySession* session, segy_api_error* error);

SEGYSDK_DLL_PUBLIC int segy_get_revision(SegySession* session, segy_api_error* error);

SEGYSDK_DLL_PUBLIC char segy_session_is_valid(SegySession* session, segy_api_error* error);

SEGYSDK_DLL_PUBLIC char segy_is_3d(SegySession* session, segy_api_error* error);

SEGYSDK_DLL_PUBLIC char segy_has_index(SegySession* session, segy_api_error* error);
SEGYSDK_DLL_PUBLIC char segy_has_missing_traces_index(SegySession* session, segy_api_error* error);

SEGYSDK_DLL_PUBLIC void segy_get_3D_file_extents(SegySession* session, long* first_inline, int* inline_step, long* inline_count, long* first_crossline, int* crossline_step, long* crossline_count, double* first_sample_depth, double* sample_interval, int* sample_count, segy_api_error* error);
SEGYSDK_DLL_PUBLIC void segy_get_2D_line_extents(SegySession* session, long* first_cmp, int* cmp_step, long* cmp_count, double* first_sample_depth, double* sample_interval, int* sample_count, segy_api_error* error);

SEGYSDK_DLL_PUBLIC bool segy_set_vector_header_mapping(SegySession* session, const char* vectorHeaderMapping, bool forceRemapping, segy_api_error* error);

SEGYSDK_DLL_PUBLIC int segy_get_trace_header_field_count(SegySession* session, segy_api_error* error);

SEGYSDK_DLL_PUBLIC const char* segy_get_ascii_headers(SegySession* session, segy_api_error* error);

SEGYSDK_DLL_PUBLIC const char* segy_get_ascii_headers_by_line(SegySession* session, segy_api_error* error);

SEGYSDK_DLL_PUBLIC void segy_get_selected_scaled_trace_header_values(SegySession* session, long start_trace, int traces_to_dump, int header_ids_count, const char** header_ids, double* buffer, size_t buffer_allocated_bytes, int* returned_traces, segy_api_error* error);

SEGYSDK_DLL_PUBLIC const char* segy_get_ascii_headers_as_json(SegySession* session, segy_api_error* error);

SEGYSDK_DLL_PUBLIC const char* segy_get_ascii_headers_by_line_as_json(SegySession* session, segy_api_error* error);

SEGYSDK_DLL_PUBLIC const char* segy_get_extended_ascii_headers_as_json(SegySession* session, segy_api_error* error);

SEGYSDK_DLL_PUBLIC const char* segy_get_binary_header_as_json(SegySession* session, segy_api_error* error);

SEGYSDK_DLL_PUBLIC const char* segy_get_raw_trace_headers_as_json(SegySession* session, long start_trace, int traces_to_dump, segy_api_error* error);

/**
 * @brief Gets trace samples from the requested trace range and vertical range.
 * @param session       pointer to instantiated SegySession object.
 * @param start_trace       Zero-offset trace number to start extraction of header values.
 * @param requested_traces  Number of traces to extract values from.
 * @param sample_offset     Offset number of samples counted from the top of the section
 * @param requested_samples Requested number of samples per trace to be returned. 0 means return all samples
 * @param buffer_allocated_bytes  Size of preallocated buffer in bytes (multiply buffer size by sizeof(float))
 * @param returned_traces_count   Actual number of traces returned
 * @param returned_samples_count  Actual number of samples per trace returned
 * @param buffer            pre-allocated buffer std::vector<std::vector<float>> holding the extracted samples per trace
 * @param error                   Gives operation status error.is_error (true or false) and a message indicating type of failure if is_error is True
 **/
SEGYSDK_DLL_PUBLIC void segy_get_trace_samples(SegySession* session, long start_trace, int requested_traces, int sample_offset, int requested_samples, size_t buffer_allocated_bytes, int* returned_traces_count, int* returned_samples_count, float* buffer, segy_api_error* error);

/**
 * @brief Gets key trace headers, intended for operational use rather than for header mapping purposes. Only 7 key headers (For 2D: SP, CMP, X and Y, sampleCount, sampleRate and firstSampleDepth or for 3D: Inline, Crossline, X and Y, sampleCount, sampleRate and firstSampleDepth) will be returned
 * @param session        pointer to instantiated SegySession object.
 * @param start_trace    Zero-offset trace number to start extraction of header values.
 * @param traces_to_dump Number of traces to extract values from.
 * @param buffer_allocated_bytes  Size of preallocated buffer in bytes (multiply buffer size by sizeof(double)) 
 * @param returned_headers_count  Actual number of headers returned
 * @param returned_traces_count   Actual number of traces returned
 * @param headerIds      reference vector of string holding the extracted trace header keys
 * @param buffer         pre-allocated buffer std::vector<std::vector<double>> holding the extracted headers per trace (minimum size is number of extracted headers (7) x tracesToDump)
 * @param error                   Gives operation status error.is_error (true or false) and a message indicating type of failure if is_error is True
 **/
SEGYSDK_DLL_PUBLIC void segy_get_trace_header_values(SegySession* session, long start_trace, int traces_to_dump, size_t buffer_allocated_bytes, int* returned_headers_count, int* returned_traces, char** header_ids, double* buffer, segy_api_error* error);

SEGYSDK_DLL_PUBLIC const char* segy_get_traces_as_json(SegySession* session, long start_trace, int traces_to_dump, segy_api_error* error);

SEGYSDK_DLL_PUBLIC const char* segy_get_scaled_trace_headers_as_json(SegySession* session, long start_trace, int traces_to_dump, segy_api_error* error);

SEGYSDK_DLL_PUBLIC const char* segy_get_survey_bin_grid(SegySession* session, segy_api_error* error);

SEGYSDK_DLL_PUBLIC void segy_accept_and_register_settings(SegySession* session, segy_api_error* error);

SEGYSDK_DLL_PUBLIC void segy_load_from_saved_settings(SegySession* session, segy_api_error* error);

SEGYSDK_DLL_PUBLIC void segy_generate_index_file(SegySession* session, bool force_generate, bool force_overwrite, segy_api_error* error);

SEGYSDK_DLL_PUBLIC void segy_close_segy_session(SegySession* session, segy_api_error* error);

SEGYSDK_DLL_PUBLIC SegyBulkReader* segy_create_bulk_reader(SegySession* segy_session, segy_api_error* error);

SEGYSDK_DLL_PUBLIC void segy_bulk_read(SegyBulkReader* reader, int inline_index, int crossline_index, int sample_index, int inline_count, int crossline_count, int sample_count, float* buffer, size_t buffer_allocated_bytes, segy_api_error* error);

SEGYSDK_DLL_PUBLIC void segy_close_bulk_reader(SegyBulkReader* reader, segy_api_error* error);

SEGYSDK_DLL_PUBLIC const char* segy_read_text_file(SegySession* session, char* textFilePath, segy_api_error* error);

SEGYSDK_DLL_PUBLIC const char* segy_get_osdu_meta_data(SegySession* session, char* path_or_id, segy_api_error* error);

SEGYSDK_DLL_PUBLIC void segy_set_is_2D(SegySession* segy_session, bool is2D, segy_api_error* error);

SEGYSDK_DLL_PUBLIC Segy2DReader* segy_create_2d_reader(SegySession* segy_session, segy_api_error* error);

SEGYSDK_DLL_PUBLIC void segy_2d_read_cmp(Segy2DReader* reader, int cmp_start, int cmp_end, size_t sample_start, size_t sample_end, float* buffer, size_t buffer_allocated_bytes, segy_api_error* error);

SEGYSDK_DLL_PUBLIC void segy_2d_read_traces(Segy2DReader* reader, size_t trace_start, size_t trace_end, size_t sample_start, size_t sample_end, float* buffer, size_t buffer_allocated_bytes, segy_api_error* error);

SEGYSDK_DLL_PUBLIC void segy_close_2d_reader(Segy2DReader* reader, segy_api_error* error);


SEGYSDK_DLL_PUBLIC void segy_dummy_reference_keeping_function(SegySession* session, segy_api_error* error);
}
