# Copyright 2021-2023 Schlumberger
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

import os, sys

sys.path.append(os.path.join(os.path.dirname(__file__), "../src/"))
import segysdk

if len(sys.argv) != 2:
    print(f"Usage: {sys.argv[0]} <test_file.sgy>")

if os.environ.get('SD_SVC_TOKEN', False)==False:
    os.environ["SD_SVC_TOKEN"] = 'Dummy token string'
if os.environ.get('SD_SVC_URL', False)==False:
    os.environ["SD_SVC_URL"] = 'https://dummy.com'
if os.environ.get('SD_SVC_API_KEY', False)==False:
    os.environ["SD_SVC_API_KEY"] = 'Dummy API string'

session = segysdk.create_session(sys.argv[1], '{}')

print("(Memory address) ", session._raw_pointer)
print("Revision ", session.get_revision())
print("Is 3D? ", session.is_3d())
print("Trace header fields: ", session.get_trace_header_field_count())
print("ASCII headers:")
print(session.get_ascii_headers())
print("==== END ASCII headers ====")
print("ASCII headers JSON:")
print(session.get_ascii_headers_as_json())
print("==== END ASCII headers JSON ====")
print("Extended ASCII headers JSON:")
print(session.get_extended_ascii_headers_as_json())
print("==== END Extended ASCII headers JSON ====")
print("Binary headers JSON:")
print(session.get_binary_header_as_json())
print("==== END Binary headers JSON ====")
print("Raw trace headers JSON (traces 0 to 2):")
print(session.get_raw_trace_headers_as_json(0, 3))
print("==== END Raw trace headers JSON ====")
print("Scaled trace headers JSON (traces 0 to 2):")
print(session.get_scaled_trace_headers_as_json(0, 3))
print("==== END Scaled trace headers JSON ====")

"""
TODO how to use the bulk reader (draft):

bulk_reader = segysdk.create_bulk_reader(sys.argv[1], TODO)
bytes_per_sample = TODO
buffer_size = ni * nj * nk * bytes_per_sample
buffer = bytearray(buffer_size)
bulk_reader.read(from_i, from_j, from_k, ni, nj, nk, buffer)
"""
