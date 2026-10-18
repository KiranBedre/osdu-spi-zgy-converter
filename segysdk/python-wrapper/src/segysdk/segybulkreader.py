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

from ctypes import c_void_p
import numpy as np
from .segyapierror import run_with_error_handling
from .segydll import dll


# TODO incomplete
class SegyBulkReader:
    """!
    @brief Seismic data reader.

    Reads and converts seismic bulk data. Use SegySession::create_bulk_reader() to create an instance.
    """
    _raw_pointer: c_void_p

    def __init__(self, raw_pointer: c_void_p):
        self._raw_pointer = raw_pointer

    def read(self,
             inline_index: int,
             crossline_index: int,
             sample_index: int,
             inline_count: int,
             crossline_count: int,
             sample_count: int) -> np.ndarray:
        """!
        @brief Read seismic data bounded by a 3-D rectangular box.

        Data is converted to machine-native single precision floating point.

        @param inline_index    Inline (I) start index of data to read. 0-based.
        @param crossline_index Crossline (J) start index of data to read. 0-based.
        @param sample_index    Vertical (K) start index of data to read. 0-based.
        @param inline_count    Inline (I) size of data to read.
        @param crossline_count Crossline (J) size of data to read.
        @param sample_count    Vertical (K) size of data to read.

        @returns A NumPy 3-D array of single precision floating-point numbers.
        """
        buf = np.ndarray(shape=(inline_count, crossline_count, sample_count), dtype=np.float32)

        run_with_error_handling(
            lambda error: dll.segy_bulk_read(self._raw_pointer, inline_index, crossline_index, sample_index,
                                             inline_count, crossline_count, sample_count, c_void_p(buf.ctypes.data),
                                             buf.size * buf.itemsize, error))

        return buf

    def close(self) -> None:
        """!
        @brief Close the reader and release its resources.
        """
        run_with_error_handling(lambda error: dll.segy_close_bulk_reader(self._raw_pointer, error))
