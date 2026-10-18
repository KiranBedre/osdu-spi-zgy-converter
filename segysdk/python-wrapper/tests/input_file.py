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

import os, shutil, sys

sys.path.append(os.path.join(os.path.dirname(__file__), "../src/"))
import segysdk

empty_config_json = ""


def input_file(file: str) -> str:
    return os.path.join(os.path.dirname(__file__), "..", "..", "testdata", file)


def create_indexed_sample(src: str, dst: str) -> None:
    if os.path.exists(dst):
        os.remove(dst)
    shutil.copy(src, dst)

    if os.environ.get('SD_SVC_TOKEN', False)==False:
        os.environ["SD_SVC_TOKEN"] = 'Dummy token string'
    if os.environ.get('SD_SVC_URL', False)==False:
        os.environ["SD_SVC_URL"] = 'https://dummy.com'
    if os.environ.get('SD_SVC_API_KEY', False)==False:
        os.environ["SD_SVC_API_KEY"] = 'Dummy API string'

    session = segysdk.create_session(dst, "{}")
    session.generate_index_file(True, True)
    session.close()


segy_rev0_sample_file = input_file('tiny.sgy')
segy_rev0_sample_file_ascii = input_file('tiny_with_ascii_header.sgy')
segy_rev0_sample_file_indexed = input_file('_tmp_tiny_indexed.sgy')

create_indexed_sample(segy_rev0_sample_file, segy_rev0_sample_file_indexed)
