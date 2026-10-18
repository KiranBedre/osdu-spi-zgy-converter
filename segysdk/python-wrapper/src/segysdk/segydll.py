# Copyright 2022 Schlumberger
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

import os
import sys
import distro
from ctypes import c_char_p, c_void_p, CDLL
from .segyexception import SegyException

_segy_library_name = 'segysdk'
_linux_dependencies_by_distro = {
    'ubuntu/20.04': [
        # MUST be ordered in reverse dependency order
        'ubuntu/20.04/libsdapi.so.3',
    ],
    'ubuntu/22.04': [
        # MUST be ordered in reverse dependency order
        'ubuntu/22.04/libsdapi.so.3',
    ],
    'ubuntu/24.04': [
        # MUST be ordered in reverse dependency order
        'ubuntu/24.04/libsdapi.so.3',
    ],
}

_linux_modules_by_distro = {
    'ubuntu/20.04': [
        'ubuntu/20.04/libsegysdk.logger.gsm.so',
    ],
    'ubuntu/22.04': [
        'ubuntu/22.04/libsegysdk.logger.gsm.so',
    ],
    'ubuntu/24.04': [
        'ubuntu/24.04/libsegysdk.logger.gsm.so',
    ],
}


def find_library(lib):
    dllpath = os.path.abspath(os.path.join(os.path.dirname(__file__), "dll", lib))
    print(f"Found: {dllpath}")
    if not os.path.isfile(dllpath): raise FileNotFoundError(f"{lib} not found at expected location {dllpath}")
    return dllpath


def _dependencies_win():
    return [
        # MUST be ordered in reverse dependency order for Python<3.8
        # SHOULD be ordered in reverse dependency order for Python>=3.8
        'iconv-2.dll',
        'lzma.dll',
        'zlib1.dll',
        'libxml2.dll',
        'libcurl.dll',
        'azure-core.dll',
        'azure-storage-common.dll',
        'azure-storage-blobs.dll',
        'sdapi_3.dll',
        'cpprest_2_10.dll',
        'jsoncpp.dll'
  ]

def _dependencies_linux():
    return _linux_dependencies_by_distro[_linux_distro_id].copy()


def platform_dependencies():
    if sys.platform == 'win32':
        return _dependencies_win()
    else:
        return _dependencies_linux()

def _modules_win():
    return [
        'segysdk.logger.gsm.dll',
    ]

def _modules_linux():
    return _linux_modules_by_distro[_linux_distro_id].copy()

def platform_modules():
    if sys.platform == 'win32':
        return _modules_win()
    else:
        return _modules_linux()


def platform_main_library():
    if sys.platform == 'win32':
        return f'{_segy_library_name}.dll'
    else:
        ubuntu_version = distro.version()
        if ubuntu_version == '24.04':
            return f'ubuntu/24.04/lib{_segy_library_name}.so'
        elif ubuntu_version == '22.04':
            return f'ubuntu/22.04/lib{_segy_library_name}.so'
        else:
            return f'ubuntu/20.04/lib{_segy_library_name}.so'


def _configure_ctypes():
    # if a C method returns a pointer, the return type must be set to c_void_p
    # otherwise the upper 32 bits of the address are lost on Windows
    dll.segy_create_segy_session.restype = c_void_p
    dll.segy_create_session.restype = c_void_p
    dll.segy_create_contextual_session.restype = c_void_p
    dll.segy_create_bulk_reader.restype = c_void_p

    # if a C method returns a char*, the return type must be set to c_char_p
    # otherwise ctypes will try to convert the address to 32-bit int and cause overflow
    dll.segy_get_ascii_headers.restype = c_char_p
    dll.segy_get_ascii_headers_as_json.restype = c_char_p
    dll.segy_get_extended_ascii_headers_as_json.restype = c_char_p
    dll.segy_get_binary_header_as_json.restype = c_char_p
    dll.segy_get_raw_trace_headers_as_json.restype = c_char_p
    dll.segy_get_scaled_trace_headers_as_json.restype = c_char_p
    dll.segy_get_traces_as_json.restype = c_char_p
    dll.segy_get_survey_bin_grid.restype = c_char_p


def _find_linux_distro_id() -> str:
    return f'{distro.id()}/{distro.major_version()}.{distro.minor_version()}'


def _validate_linux_distro() -> None:
    if sys.platform != 'win32' and not _linux_distro_id in _linux_dependencies_by_distro:
        raise SegyException(f'Unsupported Linux distro. Detected {distro.linux_distribution()}')


_linux_distro_id:str = _find_linux_distro_id()
_validate_linux_distro()

for dep in platform_dependencies(): CDLL(find_library(dep))

library_path = find_library(platform_main_library())
dll = CDLL(library_path)

if dll is None:
    raise SegyException(f'Unable to find library {_segy_library_name}')

for module in platform_modules(): CDLL(find_library(module))

_configure_ctypes()
