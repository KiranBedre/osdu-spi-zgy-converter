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

import json
import numpy as np
import os
import sys
import unittest

from input_file import empty_config_json, segy_rev0_sample_file, segy_rev0_sample_file_ascii

sys.path.append(os.path.join(os.path.dirname(__file__), "../src/"))
import segysdk

default_mapping = '''{ 
    "data": {
        "VectorHeaderMapping": [
            {
                "KeyName": "default-qa-sis-internal-hq:reference-data--HeaderKeyName:Inline:",
                "WordFormat": "default-qa-sis-internal-hq:reference-data--WordFormatType:INT:",
                "WordWidth": 4,
                "Position": 189,
                "UoM": "default-qa-sis-internal-hq:reference-data--UnitOfMeasure:Unitless:",
                "ScalarIndicator": "NOSCALE",
                "ScalarOverride": 0
            },
            {
                "KeyName": "default-qa-sis-internal-hq:reference-data--HeaderKeyName:Crossline:",
                "WordFormat": "default-qa-sis-internal-hq:reference-data--WordFormatType:INT:",
                "WordWidth": 4,
                "Position": 193,
                "UoM": "default-qa-sis-internal-hq:reference-data--UnitOfMeasure:Unitless:",
                "ScalarIndicator": "NOSCALE",
                "ScalarOverride": 0
            }
        ]
    }
}'''                            

inverted_mapping = '''{ 
    "data": {
        "VectorHeaderMapping": [
            {
                "KeyName": "default-qa-sis-internal-hq:reference-data--HeaderKeyName:Inline:",
                "WordFormat": "default-qa-sis-internal-hq:reference-data--WordFormatType:INT:",
                "WordWidth": 4,
                "Position": 193,
                
                "UoM": "default-qa-sis-internal-hq:reference-data--UnitOfMeasure:Unitless:",
                "ScalarIndicator": "NOSCALE",
                "ScalarOverride": 0
            },
            {
                "KeyName": "default-qa-sis-internal-hq:reference-data--HeaderKeyName:Crossline:",
                "WordFormat": "default-qa-sis-internal-hq:reference-data--WordFormatType:INT:",
                "WordWidth": 4,
                "Position": 189,
                "UoM": "default-qa-sis-internal-hq:reference-data--UnitOfMeasure:Unitless:",
                "ScalarIndicator": "NOSCALE",
                "ScalarOverride": 0
            }
        ]
    }
}'''  

class CreateSegySessionTests(unittest.TestCase):

    def __init__(self, method_name: str = ...):
        super().__init__(method_name)

    if os.environ.get('SD_SVC_TOKEN', False)==False:
        os.environ["SD_SVC_TOKEN"] = 'Dummy token string'
    if os.environ.get('SD_SVC_URL', False)==False:
        os.environ["SD_SVC_URL"] = 'https://dummy.com'
    if os.environ.get('SD_SVC_API_KEY', False)==False:
        os.environ["SD_SVC_API_KEY"] = 'Dummy API string'

    def test_open_file_not_found_does_not_crash(self):
        sut = lambda: segysdk.create_session("no_such_file.sgy", empty_config_json)
        # TODO review - message is probably system dependent
        self.assertRaisesRegex(segysdk.SegyException,
                               "DiskFileReader::open: Error opening file - no_such_file.sgy: [Nn]o such file or directory",
                               sut)

    def test_can_open_local_segy(self):
        session = segysdk.create_session(segy_rev0_sample_file, empty_config_json)
        self.assertIsNotNone(session)
        session.close()

    def test_can_open_local_segy_with_token(self):
        session = segysdk.create_session(segy_rev0_sample_file, empty_config_json, os.environ["SD_SVC_TOKEN"])
        self.assertIsNotNone(session)
        session.close()

    def test_rev0_segy_revision_is_0(self):
        session = segysdk.create_session(segy_rev0_sample_file, empty_config_json)
        self.assertFalse(session.get_revision())

    def test_rev0_segy_is_3d(self):
        session = segysdk.create_session(segy_rev0_sample_file, empty_config_json)
        self.assertTrue(session.is_3d())

    # TODO review - why is it a single long string and not a list split by lines?
    expected_ascii_header = \
        'C 1 SEGY OUTPUT FROM Petrel 2020.3 Sunday, November 15 2020 17:16:13            ' \
        'C 2 Name: G3D202004-01_Lns3990_4474[32ms]_1 Type: 3D seismic                    ' \
        'C 3                                                                             ' \
        'C 4 First inline: 3990   Last inline: 4246                                      ' \
        'C 5 First xline:  3850   Last xline:  8714                                      ' \
        'C 6 CRS: Undefined                                                              ' \
        'C 7 X min: 3427481.08 max: 3637505.40 delta: 210024.31                          ' \
        'C 8 Y min: 242280.35 max: 275890.40 delta: 33610.05                             ' \
        'C 9 Time min: -8176.00 max: 16.00 delta: 8192.00                                ' \
        'C10 Lat min: - max: - delta: -                                                  ' \
        'C11 Long min: - max: - delta: -                                                 ' \
        'C12 Trace min: -8160.00 max: 0.00 delta: 8160.00                                ' \
        'C13 Seismic (template) min: -2146.54 max: 3267.64 delta: 5414.17                ' \
        'C14 Amplitude (data) min: -2146.54 max: 3267.64 delta: 5414.17                  ' \
        'C15 Trace sample format: IEEE floating point                                    ' \
        'C16 Coordinate scale factor: 100.00000                                          ' \
        'C17                                                                             ' \
        'C18 Binary header locations:                                                    ' \
        'C19 Sample interval             : bytes 17-18                                   ' \
        'C20 Number of samples per trace : bytes 21-22                                   ' \
        'C21 Trace date format           : bytes 25-26                                   ' \
        'C22                                                                             ' \
        'C23 Trace header locations:                                                     ' \
        'C24 Inline number               : bytes 5-8                                     ' \
        'C25 Xline number                : bytes 21-24                                   ' \
        'C26 Coordinate scale factor     : bytes 71-72                                   ' \
        'C27 X coordinate                : bytes 73-76                                   ' \
        'C28 Y coordinate                : bytes 77-80                                   ' \
        'C29 Trace start time/depth      : bytes 109-110                                 ' \
        'C30 Number of samples per trace : bytes 115-116                                 ' \
        'C31 Sample interval             : bytes 117-118                                 ' \
        'C32                                                                             ' \
        'C33                                                                             ' \
        'C34                                                                             ' \
        'C35                                                                             ' \
        'C36                                                                             ' \
        'C37                                                                             ' \
        'C38                                                                             ' \
        'C39                                                                             ' \
        'C40 END EBCDIC                                                                  '

    def test_textual_headers_ebcdic_encoding(self):
        session = segysdk.create_session(segy_rev0_sample_file, empty_config_json)
        self.assertEqual(session.get_ascii_headers(), self.expected_ascii_header)
        session.close()

    def test_textual_headers_json(self):
        session = segysdk.create_session(segy_rev0_sample_file, empty_config_json)
        result = json.loads(session.get_ascii_headers_as_json())
        self.assertEqual(result['Textualheader'], self.expected_ascii_header)
        session.close()

    def test_extended_textual_headers_json(self):
        session = segysdk.create_session(segy_rev0_sample_file, empty_config_json)
        result = session.get_extended_ascii_headers_as_json()
        self.assertEqual(result, '{}')
        # TODO: Need SEGY rev 1 test data file with extended textual headers:
        # result = json.loads(session.get_extended_ascii_headers_as_json())
        #  self.assertTrue('metadata' in result)
        # TODO self.assertTrue('ExtendedTextualHeaders' in result)
        session.close()

    def test_textual_headers_ascii_encoding(self):
        session = segysdk.create_session(segy_rev0_sample_file_ascii, empty_config_json)
        self.assertEqual(session.get_ascii_headers(), self.expected_ascii_header)
        session.close()

    def test_get_trace_header_field_count(self):
        session = segysdk.create_session(segy_rev0_sample_file_ascii, empty_config_json)
        self.assertEqual(session.get_trace_header_field_count(), 73)
        session.close()

    def test_get_raw_trace_headers_as_json_for_first_trace(self):
        self.maxDiff = None
        session = segysdk.create_session(segy_rev0_sample_file, empty_config_json)
        result = json.loads(session.get_raw_trace_headers_as_json(0, 1), )

        # TODO review asserts
        self.assertEqual(result['Metadata']['TraceCount'], 1)

        headers = result['Metadata']['ColumnHeaders']
        self.assertEqual(len(headers), 73)

        trace_data = result['TraceData']
        self.assertEqual(len(trace_data), 1)
        self.assertEqual(len(trace_data[0]), 2)
        self.assertEqual((trace_data[0]['TraceNo']), 0)
        self.assertEqual(len(trace_data[0]['Traces']), 73)  # TODO Traces is not the best name
        session.close()

    def test_get_binary_header_as_json(self):
        self.maxDiff = None
        session = segysdk.create_session(segy_rev0_sample_file, empty_config_json)
        result = json.loads(session.get_binary_header_as_json())
        print(result)

        # TODO review asserts
        # TODO 'metadata' key is not consistent with 'Metadata' returned from other functions
        self.assertIsNotNone(result['metadata'])

        headers = result['BinaryHeaders']
        self.assertEqual(len(headers), 28)
        session.close()

    def test_get_raw_trace_headers_as_json_more_traces_requested(self):
        self.maxDiff = None
        session = segysdk.create_session(segy_rev0_sample_file, empty_config_json)
        result = json.loads(session.get_raw_trace_headers_as_json(0, 123))
        trace_data = result['TraceData']
        self.assertIsNotNone(trace_data)
        self.assertEqual(len(trace_data), 40)
        [self.assertEqual(len(trace_data[i]['Traces']), 73) for i in range(0, 40)]  # TODO Traces is not the best name
        self.assertEqual(result['Metadata']['TraceCount'], 40)
        session.close()

    def test_get_raw_trace_headers_as_json_beyond_eof(self):
        self.maxDiff = None
        session = segysdk.create_session(segy_rev0_sample_file, empty_config_json)
        result = json.loads(session.get_raw_trace_headers_as_json(9000, 1))
        self.assertIsNone(result['TraceData'])  # TODO probably should return empty array
        self.assertEqual(result['Metadata']['TraceCount'], 0)
        session.close()

    def test_get_selected_scaled_trace_header_values(self):
        session = segysdk.create_session(segy_rev0_sample_file, empty_config_json)
        result = session.get_selected_scaled_trace_header_values(0, 2, [
            'Inline',
            'Crossline',
            'TraceLength',
            'ShotToRecordingLagTime',
            'SampleRate'
        ])
        self.assertTrue(np.array_equiv(result, [[3990.0, 3850.0, 256.0, 0.0, 32000.0],
                                                [3990.0, 4106.0, 256.0, 0.0, 32000.0]]))
        session.close()

    def test_get_selected_scaled_trace_header_values_unknown_field_will_be_set_to_0(self):
        session = segysdk.create_session(segy_rev0_sample_file, empty_config_json)
        result = session.get_selected_scaled_trace_header_values(0, 1, ['Unknown field will be 0.0', 'Inline'])
        self.assertTrue(np.array_equiv(result, [[0.0, 3990.0]]))
        session.close()

    def test_get_selected_scaled_trace_header_values_no_results_beyond_end_of_file(self):
        session = segysdk.create_session(segy_rev0_sample_file, empty_config_json)
        result = session.get_selected_scaled_trace_header_values(9000, 1, ['Inline'])
        self.assertTrue(np.array_equiv(result, []))
        session.close()

    def test_set_vector_header_mapping(self):
        session = segysdk.create_session(segy_rev0_sample_file, empty_config_json)
        session.set_vector_header_mapping(vector_header_mapping=default_mapping, force_remapping=True)
        result = session.get_selected_scaled_trace_header_values(0, 2, ['Inline','Crossline'])
        self.assertTrue(np.array_equiv(result, [[3990.0, 3850.0], [3990.0, 4106.0]]), f'Expected [[3990.0, 3850.0], [3990.0, 4106.0]] but got {result}.')
        session.set_vector_header_mapping(vector_header_mapping=inverted_mapping, force_remapping=True)
        inverted_result = session.get_selected_scaled_trace_header_values(0, 2, ['Inline','Crossline'])
        self.assertTrue(np.array_equiv(inverted_result, [[3850.0, 3990.0], [4106.0, 3990.0]]), f'Expected [[3850.0, 3990.0], [4106.0, 3990.0]] but got {inverted_result}.')
        session.close()

    """
    won't test:
        void segy_configure_remote_access(const char* sdms_url, const char* sdms_app_key, const char* token);
        void segy_accept_and_register_settings(SegySession* session, segy_api_error* error);
        void segy_load_from_saved_settings(SegySession* session, segy_api_error* error);
        void segy_close_segy_session(SegySession* session, segy_api_error* error);

    TODOs:
        const char* segy_get_scaled_trace_headers_as_json(SegySession* session, long start_trace, int traces_to_dump, segy_api_error* error);
        SegyBulkReader* segy_create_bulk_reader(const char* segy_filename, int inline_count, int crossline_count, int vertical_count, segy_api_error* error);
        void segy_bulk_read(SegyBulkReader* reader, int inline_index, int crossline_index, int sample_index, int inline_count, int crossline_count, int sample_count, float* buffer, segy_api_error* error);
        void segy_close_bulk_reader(SegyBulkReader* reader, segy_api_error* error);
    """
    