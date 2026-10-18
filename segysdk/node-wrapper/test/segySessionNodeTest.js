/*
Copyright 2022 Schlumberger

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

const assert = require('assert');
const segyApi = require('bindings')('segyApi');
const testData = require('./testData');
const testsDataPath = "/work/testdata";
const tinyFile = `${testsDataPath}/tiny.sgy`;
const tinyWithAscii = `${testsDataPath}/tiny_with_ascii_header.sgy`

describe('SegySessionNode', () => {
    it("should throw exception if sd file doesn't exist", function () {
        const expectedError = /TypeError: DiskFileReader::open: Error opening file - no_such_file.sgy: No such file or directory/;
        assert.throws(() => new segyApi.SegySessionNode({sdPath: "no_such_file.sgy"}), expectedError);
    });

    it("should show ascii headers", function () {
        const segySession = new segyApi.SegySessionNode({sdPath: tinyFile});
        const headers = segySession.getAsciiHeaders();

        assert.equal(headers, testData.textualHeaders);
    });

    it("should show ascii headers as json", function () {
        const segySession = new segyApi.SegySessionNode({sdPath: tinyFile});
        const headers = segySession.getAsciiHeadersAsJson();

        assert.equal(JSON.parse(headers)['Textualheader'], testData.textualHeaders);
    });

    it("should show ascii headers in ascii encoding", function () {
        const segySession = new segyApi.SegySessionNode({sdPath: tinyWithAscii});
        const headers = segySession.getAsciiHeaders();

        assert.equal(headers, testData.textualHeaders);
    });

    it("should show binary header as json", function () {
        const segySession = new segyApi.SegySessionNode({sdPath: tinyFile});
        const header = segySession.getBinaryHeaderAsJson();

        const jsonHeader = JSON.parse(header);
        assert.notEqual(jsonHeader.metadata, undefined);
        assert.equal(jsonHeader.BinaryHeaders.length, 28)
    });
});
