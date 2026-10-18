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

class Encoding:
    """!
    @brief Static methods for encoding conversion between the non-Unicode C API of the SEG-Y SDK and Python.
    """
    @staticmethod
    def to_library_encoding(str: str) -> bytes:
        """!
        @brief Convert a Python string value to the encoding of the C API.

        @param str Python string.
        """
        return str.encode('utf-8')


    @staticmethod
    def to_python_encoding(str: bytes) -> str:
        """!
        @brief Convert a non-Unicode C string value to Python string encoding.

        @param str C string as a byte array.
        """
        return str.decode('utf-8', 'strict')

    # TODO Uses utf-8 for C string encoding. It usually works, but that may not be true in all cases.
    #  (e.g., if metadata, header, or an error message contains characters from a local charset)
