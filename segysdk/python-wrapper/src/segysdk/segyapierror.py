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

from ctypes import c_bool, c_char_p, pointer, Structure
from .encoding import Encoding
from .segyexception import SegyException


class SegyApiError(Structure):
    """!
    @brief A substitute for C++ exception mechanism for interfacing with the C API.

    This data structure can store an error message and an error flag,
    and can throw an exception with the stored message.
    """

    _fields_ = [("is_error", c_bool),
                ("message", c_char_p)]

    def raise_on_error(self):
        """!
        @brief In case of error, throw a SegyException with the stored message, do nothing otherwise.
        """
        if self.is_error:
            raise SegyException(Encoding.to_python_encoding(self.message))


def run_with_error_handling(call):
    error = SegyApiError()
    result = call(pointer(error))
    error.raise_on_error()
    return result
