# Copyright 2021 Schlumberger
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from ctypes import byref, c_char_p, c_int, c_void_p
import numpy as np
from typing import List
from .segybulkreader import SegyBulkReader
from .segyapierror import run_with_error_handling
from .segydll import dll
from .encoding import Encoding


class SegySession:
    """!
    @brief The main entry point to all SEG-Y operations.

    - To keep the amount of duplication under control, this API documentation is an abridged version of the C++ one.
            For the description of JSON response schemas and other details, refer to the C++ SEG-Y SDK.
    - In the Python wrapper, the creation and configuration options are limited, compared to the C++ API.
    """
    _raw_pointer: c_void_p

    def __init__(self, raw_pointer: c_void_p):
        self._raw_pointer = raw_pointer

    def get_revision(self) -> int:
        """!
        @brief Gets SEG-Y version of current SEG-Y file.

        Supported return values are:
        - 0 : SEG-Y file mostly corresponding to the SEG-Y revision standard 0 of 1975
        - 1 : SEG-Y file corresponding to the SEG-Y revision standard 1.0 of 2002
        - 2 : SEG-Y file corresponding to the SEG-Y revision standard 2.0 of 2017 Although this is a valid SEG-Y
                                revision level this version of SEG-Y is not yet properly supported by the SEG-Y SDK.
        """
        return run_with_error_handling(lambda error: dll.segy_get_revision(self._raw_pointer, error))

    def is_3d(self) -> bool:
        """!
        @brief Does the SEG-Y dataset contain 3D data?

        @returns True if both trace header fields Inline and Crossline have been specified by the user in the input
        trace header definition.
        """
        return run_with_error_handling(lambda error: dll.segy_is_3d(self._raw_pointer, error))

    def get_trace_header_field_count(self) -> int:
        """!
        @brief Gets trace header fields count.

        @returns The number of literals (fields) defined for the SEG-Y trace header.
        """
        return run_with_error_handling(lambda error: dll.segy_get_trace_header_field_count(self._raw_pointer, error))

    def get_ascii_headers(self) -> str:
        """!
        @brief Gets textual headers in ASCII encoding.

        @returns The contents of the SEG-Y textual header as a single 3200 byte string.
        It is the responsibility of the requester to split the string into standard 80 character lines.
        """
        return Encoding.to_python_encoding(
            run_with_error_handling(lambda error: dll.segy_get_ascii_headers(self._raw_pointer, error)))

    def set_vector_header_mapping(self, vector_header_mapping: str, force_remapping: bool) -> bool: 
        """!
        @brief Set the trace header mapping for the SEGY dataset based on the OSDU Vector Header Mapping.

        @returns True if it was possible to remap the SegySession and false if something went wrong (i.e.: index already created)

        @param vector_header_mapping     Header mapping based on https://community.opengroup.org/osdu/data/data-definitions/-/blob/master/E-R/abstract/AbstractVectorHeaderMapping.1.0.0.md.
        @param force_remapping           For the trace header remapping, which will drop existed index.
        
        """
        return run_with_error_handling(
            lambda error: dll.segy_set_vector_header_mapping(
            self._raw_pointer,
            Encoding.to_library_encoding(vector_header_mapping),
            force_remapping,
            error))

    def get_selected_scaled_trace_header_values(
            self, start_trace: int,
            traces_to_dump: int,
            header_ids: List[str]) -> np.ndarray:
        """!
        @brief A performance-optimized way to get the values of only those trace header fields which are needed for the
        caller, for several traces in one batch.

        @returns A 2-D array of trace header values only, as they are seen by the internal methods.
        This means scaling factors and/or user defined overrides are applied.
        Only the trace header literals specified in the header_ids list are extracted.

        @param start_trace     Zero-offset trace number to start extraction of header values.
        @param traces_to_dump  Number of traces to extract values from.
        @param header_ids      List holding the name of the trace header literals to be extracted.
        """
        header_ids_count = len(header_ids)
        utf8_header_ids = [s.encode('utf-8') for s in header_ids]

        c_header_ids = (c_char_p * header_ids_count)()
        for i in range(0, header_ids_count):
            c_header_ids[i] = utf8_header_ids[i]

        buf = np.ndarray(shape=(traces_to_dump, header_ids_count), dtype=np.float64)
        allocated_bytes = buf.size * buf.itemsize
        returned_traces = c_int()

        run_with_error_handling(
            lambda error: dll.segy_get_selected_scaled_trace_header_values(
                self._raw_pointer, start_trace, traces_to_dump, header_ids_count, c_header_ids,
                c_void_p(buf.ctypes.data), allocated_bytes, byref(returned_traces), error))

        buf.resize((returned_traces.value, header_ids_count))

        return buf

    def get_ascii_headers_as_json(self) -> str:
        """!
        @brief Gets textual headers as JSON.

        @returns The data extracted by the get_ascii_headers method encapsulated in a JSON structure.
        """
        return Encoding.to_python_encoding(
            run_with_error_handling(lambda error: dll.segy_get_ascii_headers_as_json(self._raw_pointer, error)))

    def get_extended_ascii_headers_as_json(self) -> str:
        """!
        @brief Gets extended textual headers as JSON.

        @returns The data extracted by the GetExtendedAsciiHeaders method encapsulated in a JSON structure.
        """
        return Encoding.to_python_encoding(run_with_error_handling(
            lambda error: dll.segy_get_extended_ascii_headers_as_json(self._raw_pointer, error)))

    def get_binary_header_as_json(self) -> str:
        """!
        @brief Gets binary headers as JSON.

        @returns The data extracted by the GetBinaryHeaders method encapsulated in a JSON structure.
        This is a struct comprising a string with the literal (field) name and its corresponding binary header value.
        """
        return Encoding.to_python_encoding(
            run_with_error_handling(lambda error: dll.segy_get_binary_header_as_json(self._raw_pointer, error)))

    def get_raw_trace_headers_as_json(self, start_trace: int, traces_to_dump: int) -> str:
        """!
        @brief Gets raw trace headers as JSON.

        @returns The data extracted by the GetRawTraceHeaders method encapsulated in a JSON structure.

        @param start_trace    Zero-offset trace number to start extraction of header values.
        @param traces_to_dump Number of traces to extract values from.
        """
        return Encoding.to_python_encoding(
            run_with_error_handling(lambda error: dll.segy_get_raw_trace_headers_as_json(
                self._raw_pointer, start_trace, traces_to_dump, error)))
    
    def get_traces_as_json(self, start_trace: int, traces_to_dump: int) -> str:
        """!
        @brief Gets trace header and samples as JSON.

        @returns The data extracted by the GetTraces method encapsulated in a JSON structure.

        @param start_trace    Zero-offset trace number to start extraction of header values.
        @param traces_to_dump Number of traces to extract values from.
        """
        return Encoding.to_python_encoding(
            run_with_error_handling(lambda error: dll.segy_get_traces_as_json(
                self._raw_pointer, start_trace, traces_to_dump, error)))    

    def get_scaled_trace_headers_as_json(self, start_trace: int, traces_to_dump: int) -> str:
        """!
        @brief Gets scaled trace headers as JSON.

        @returns The data extracted by the GetScaledTraceHeaders method encapsulated in a JSON structure.

        @param start_trace    Zero-offset trace number to start extraction of header values.
        @param traces_to_dump Number of traces to extract values from.
        """
        return Encoding.to_python_encoding(
            run_with_error_handling(
                lambda error: dll.segy_get_scaled_trace_headers_as_json(
                    self._raw_pointer, start_trace, traces_to_dump, error)))

    def get_survey_bin_grid(self) -> str:
        """!
        @brief Gets calculated Survey Bin Grid as JSON.

        @returns The data extracted by the segy_get_survey_bin_grid method encapsulated in a JSON structure.
        """
        return Encoding.to_python_encoding(
            run_with_error_handling(lambda error: dll.segy_get_survey_bin_grid(self._raw_pointer, error)))

    def generate_index_file(self, force_generate: bool, force_overwrite: bool) -> None:
        """!
        @brief Create or recreate an index file for the dataset.

        @param force_generate  Force generation of the full index even if the dataset is regular and would not need it.
        @param force_overwrite Regenerate the index if it already exists. Otherwise, an exception will be thrown.
        """
        run_with_error_handling(
            lambda error: dll.segy_generate_index_file(self._raw_pointer, force_generate, force_overwrite, error))

    def close(self) -> None:
        """!
        @brief Frees up the temporary storage allocated by this instance.
        """
        return run_with_error_handling(lambda error: dll.segy_close_segy_session(self._raw_pointer, error))

    def create_bulk_reader(self) -> SegyBulkReader:
        """!
        @brief Create bulk data reader for the SEG-Y dataset.

        @returns a pointer to the bulk data (trace data) reader for the current context.
        """
        return SegyBulkReader(
            run_with_error_handling(lambda error: c_void_p(dll.segy_create_bulk_reader(self._raw_pointer, error))))
