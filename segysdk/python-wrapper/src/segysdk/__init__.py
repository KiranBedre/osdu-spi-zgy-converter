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

__all__ = [
    'segy_configure_remote_access',
    'create_session',
    'SegyException',
    'SegySession',
    'SegyBulkReader']

from ctypes import c_void_p, c_size_t, pointer
from .encoding import Encoding
from .segyapierror import SegyApiError
from .segyexception import SegyException
from .segysession import SegySession
from .segybulkreader import SegyBulkReader
from .segydll import dll
import json


def segy_configure_remote_access(sdms_url: str, sdms_app_key: str, token: str, http_headers: dict = {}) -> None:
    """
    
    void segy_configure_remote_access(const char* sdms_url, const char* sdms_app_key, const char* token);
    """

    dll.segy_configure_remote_access(Encoding.to_library_encoding(sdms_url),
                                     Encoding.to_library_encoding(sdms_app_key),
                                     Encoding.to_library_encoding(token))

    for k,v in http_headers.items():
        dll.segy_add_http_header(Encoding.to_library_encoding(k), Encoding.to_library_encoding(v))


def create_session(segy_file_name: str, segy_config_json_content: str, token: str = "") -> SegySession:
    """
    SegySession* segy_create_session(const char* segy_filename, const char* loading_parameter_json, const char* token);
    """
    error = SegyApiError()
    native_session = c_void_p(dll.segy_create_session(
        Encoding.to_library_encoding(segy_file_name),
        Encoding.to_library_encoding(segy_config_json_content),
        Encoding.to_library_encoding(token),
        pointer(error)))
    error.raise_on_error()
    return SegySession(native_session)


def create_contextual_session(segy_file_name: str, loading_config: str, token: str, sdms_url: str, sdms_api_key: str = "none", headers: dict = {}, page_size: int = 1048576, max_pages: int = 1024) -> SegySession:
    """
    SegySession* segy_create_contextual_session(const char* segy_file_name, const char* loading_parameter_json, const char* token, const char* sdms_url, const char* sdms_api_key, const char* headers, size_t page_size, size_t max_pages, segy_api_error* error);
    """
    error = SegyApiError()
    native_session = c_void_p(dll.segy_create_contextual_session(
        Encoding.to_library_encoding(segy_file_name),
        Encoding.to_library_encoding(loading_config),
        Encoding.to_library_encoding(token),
        Encoding.to_library_encoding(sdms_url),
        Encoding.to_library_encoding(sdms_api_key),
        Encoding.to_library_encoding(json.dumps(headers)),
        c_size_t(page_size),
        c_size_t(max_pages),
        pointer(error)))
    error.raise_on_error()
    return SegySession(native_session)