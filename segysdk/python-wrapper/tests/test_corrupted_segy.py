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

import os
import sys
import unittest

from input_file import empty_config_json, input_file

sys.path.append(os.path.join(os.path.dirname(__file__), "../src/"))
import segysdk

empty_file = 'empty_file'
not_a_segy_file = 'not_a_segy_file.txt'
not_a_segy_file_large = 'not_a_segy_file_large.txt'
truncated_textual_header = 'truncated_textual_header.sgy'
truncated_binary_header = 'truncated_binary_header.sgy'
truncated_first_trace_header = 'truncated_first_trace_header.sgy'
truncated_first_trace_data = 'truncated_first_trace_data.sgy'
truncated_second_trace_header = 'truncated_second_trace_header.sgy'


def open_segy(file):
    return segysdk.create_session(input_file(file), empty_config_json)


class CorruptedSegyFileTests(unittest.TestCase):
    if os.environ.get('SD_SVC_TOKEN', False)==False:
        os.environ["SD_SVC_TOKEN"] = 'Dummy token string'
    if os.environ.get('SD_SVC_URL', False)==False:
        os.environ["SD_SVC_URL"] = 'https://dummy.com'
    if os.environ.get('SD_SVC_API_KEY', False)==False:
        os.environ["SD_SVC_API_KEY"] = 'Dummy API string'
    
    # TODO review - describes current behavior, but is the current behavior correct?
    def test_open_corrupted_files_longer_than_segy_version_header(self):
        corrupted_files = [truncated_first_trace_header,
                           truncated_second_trace_header]
        for file in corrupted_files:
            try:
                session = open_segy(input_file(file))
                self.assertIsNotNone(session, f'{file}')
            except segysdk.SegyException as ex:
                self.fail(f'Exception while opening {file}: {ex}')

    # TODO review - describes current behavior, but is the current behavior correct?
    def test_open_corrupted_files_exception_shorter_than_segy_version_header(self):
        corrupted_files = [empty_file,
                           not_a_segy_file,
                           truncated_textual_header,
                           truncated_binary_header]
        for file in corrupted_files:
            self.assertRaisesRegex(segysdk.SegyException,
                                   'Failed to read 2 bytes at offset 3500. Read 0',
                                   open_segy, file)

    # TODO review - describes current behavior, but is the current behavior correct?
    def test_open_corrupted_files_exception(self):
        corrupted_files = [not_a_segy_file_large]
        for file in corrupted_files:
            self.assertRaisesRegex(segysdk.SegyException,
                                   'Unable to determine byte order, unrecognized sample format\\.'
                                   ' Bytes \\(decimal\\): \\[98, 117\\]',
                                   open_segy, file)

    # TODO review - describes current behavior, but is the current behavior correct?
    def test_get_selected_scaled_trace_header_values_truncated_first_trace_data(self):
        session = open_segy(input_file(truncated_first_trace_data))
        result = session.get_selected_scaled_trace_header_values(0, 1, ['Inline'])
        self.assertListEqual(list(result), [[3990.0]])

