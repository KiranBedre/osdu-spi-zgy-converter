#Copyright 2024 Schlumberger
#
#Licensed under the Apache License, Version 2.0 (the "License");
#you may not use this file except in compliance with the License.
#You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
#Unless required by applicable law or agreed to in writing, software
#distributed under the License is distributed on an "AS IS" BASIS,
#WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#See the License for the specific language governing permissions and
#limitations under the License.

import argparse
import json

parser = argparse.ArgumentParser(description='Generate JSON file with Collated Index Data Vector that may be used in index UTs.')

parser.add_argument('out',
                    metavar='out',
                    type=str,
                    help='the output file to write the JSON to')
parser.add_argument('--two_dimensional', '-2d',
                    action='store_true',
                    help='generate 2D data')

args = parser.parse_args()

TRACE_HEADER_SIZE = 240
START_OFFSET = 3600
SAMPLE_SIZE = 4
startData = {
    "Offset": START_OFFSET,
    "FileNo": 0,
    "InlineNo": 101,
    "CrosslineNo": 10106,
    "X": 55020.0,
    "Y": 650135.0,
    "TraceNo": 1,
    "CMP": 101,
    "TimeFirstSample": 0.0,
    "TraceLength": 101,
    "SampleRate": 0.004,
    "CoordinateScalar": -100,
    "TraceWeightingFactor": 1
}
START_TRACE = startData["TraceNo"]


def generate2DCMP(data, cmpProgression={}):
    data["InlineNo"] = 0
    data["CrosslineNo"] = 0
    nextOffset = START_OFFSET + TRACE_HEADER_SIZE + data["TraceLength"] * SAMPLE_SIZE
    for step, cmpRange in cmpProgression.items():
        for cmp in range(cmpRange[0], cmpRange[1], step):
            data["CMP"] = cmp

            yield data

            data["Offset"] = nextOffset
            data["TraceNo"] += 1
            nextOffset += TRACE_HEADER_SIZE + data["TraceLength"] * SAMPLE_SIZE


def generate2D(data, totalTraces=501, cmpStep=1, cmpSkip=[], tracesOfDifSize={}):
    data["InlineNo"] = 0
    data["CrosslineNo"] = 0
    nextOffset = START_OFFSET
    for traceNo in range(data["TraceNo"], totalTraces):
        while data["CMP"] in cmpSkip:
            data["CMP"] += cmpStep

        data["TraceNo"] = traceNo
        if data["TraceNo"] in tracesOfDifSize:
            data["TraceLength"] = tracesOfDifSize[data["TraceNo"]]
        data["Offset"] = nextOffset

        yield data

        data["CMP"] += cmpStep
        nextOffset += TRACE_HEADER_SIZE + data["TraceLength"] * SAMPLE_SIZE


def generate3D(data):
    #@TODO: Not implemented, please fill free to update
    yield data


data = startData
if args.two_dimensional:
    #data = [dict(data) for data in generate2D(data)]
    #cmpSkip = [257, 259, 261, 263, 265, 267, 359, 361, 363, 365]
    #data = [dict(data) for data in generate2D(data, totalTraces=200, cmpStep=2, cmpSkip=cmpSkip)]
    #cmpProgression = {2: [23, 99],
    #                  -3: [233, 209],
    #                  5: [320, 400]}
    #data = [dict(data) for data in generate2DCMP(data, cmpProgression=cmpProgression)]
    data["CMP"] = 404
    cmpSkip = [400, 396, 392, 388, 384, 300, 296, 292, 288, 284]
    tracesOfDifSize = {10: 60, 20: 80, 21: 82, 22: 80, 35: 101}
    data = [dict(data) for data in generate2D(data, totalTraces=201, cmpStep=-4, cmpSkip=cmpSkip, tracesOfDifSize=tracesOfDifSize)]
else:
    data = [dict(data) for data in generate3D(data)]

with open(args.out, "w") as write_file:
    data = {"IndexData": data,
            "Expected": []}
    json.dump(data, write_file, indent=2)