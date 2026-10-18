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

from input_file import empty_config_json, segy_rev0_sample_file, segy_rev0_sample_file_indexed

sys.path.append(os.path.join(os.path.dirname(__file__), "../src/"))
import segysdk


class CreateSegySessionTests(unittest.TestCase):

    def __init__(self, method_name: str = ...):
        super().__init__(method_name)

    if os.environ.get('SD_SVC_TOKEN', False)==False:
        os.environ["SD_SVC_TOKEN"] = 'Dummy token string'
    if os.environ.get('SD_SVC_URL', False)==False:
        os.environ["SD_SVC_URL"] = 'https://dummy.com'
    if os.environ.get('SD_SVC_API_KEY', False)==False:
        os.environ["SD_SVC_API_KEY"] = 'Dummy API string'

    def test_create_bulk_reader_without_index_fails(self):
        session = segysdk.create_session(segy_rev0_sample_file, empty_config_json)
        sut = lambda: session.create_bulk_reader()
        self.assertRaisesRegex(segysdk.SegyException, 'Unable to load index file as no saved index file exists.', sut)

    def test_create_bulk_reader_indexed_input_can_be_opened(self):
        session = segysdk.create_session(segy_rev0_sample_file_indexed, empty_config_json)
        self.assertIsNotNone(session.create_bulk_reader())

    def test_bulk_reader_can_read_single_value(self):
        sut = segysdk.create_session(segy_rev0_sample_file_indexed, empty_config_json).create_bulk_reader()
        result = sut.read(1, 10, 20, 1, 2, 3)
        self.assertSequenceEqual(result.shape, (1, 2, 3))
        self.assertEqual(result.item(0, 0, 0), 148.19100952148438)
