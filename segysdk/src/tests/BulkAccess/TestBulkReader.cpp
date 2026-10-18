/*
Copyright 2021-2022 Schlumberger

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

#include "../BulkAccess/BulkReader.h"
#include "../BulkAccess/MultiBulkReader.h"
#include "../BulkAccess/SegyBulkReader.h"
#include "../BulkAccess/SegyReaderFactory.h"
#include "../BulkAccess/SimpleBulkIndex.h"
#include "../FileIO/FileReaderOptions.h"
#include "../FileIO/FileReference.h"
#include "../FileIO/GenericFileManager.h"
#include "../SegyIO/Primitives.h"
#include "../SegyIO/SegyGleaner.h"
#include "../Utilities/Environment.h"
#include "../Utilities/TokenSourceFactory.h"
#include "../Utilities/auth/TokenSource.h"

#include "../tests/UtilityLibrary/AutoCleanupTestFile.h"

#pragma warning(push)
#pragma warning(disable : 4251)
#include <SDAPI/SDGenericDataset.h>
#include <SDAPI/SDManager.h>
#include <SDAPI/SDUtils.h>
#pragma warning(pop)

#include <gtest/gtest.h>
#include <json/json.h>

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

// Developer note:
// The Remote tests have been disabled until the auth token is available in the build pipeline.
// To enable the tests, simply remove the "DISABLED_" string from the test name. e.g.
// TEST_F(BulkReaderUnitTests, DISABLED_testRemoteWriter) => TEST_F(BulkReaderUnitTests, testRemoteWriter)

namespace segysdk::bulkaccess::tests
{

using namespace fileio;

using bulkaccess::BulkIndex;
using bulkaccess::BulkReader;
using bulkaccess::MultiBulkReader;
using bulkaccess::ReadDirection;
using bulkaccess::SegyBulkReader;
using bulkaccess::SegyReaderFactory;
using bulkaccess::SimpleBulkIndex;
using segyio::SampleFormat;
using segyio::SegyGleaner;
using segysdk::segyio::Primitives;

class BulkReaderUnitTests : public testing::Test
{
    std::unique_ptr<segysdk::utilities::auth::TokenSource> tokenSource_{nullptr};

protected:
    static std::string authCallback(const void* data)
    {
        auto const* tokenSource = reinterpret_cast<segysdk::utilities::auth::TokenSource const*>(data);
        return tokenSource->token();
    }

    FileReference fr(const std::string& fileName)
    {
        if (fileName.substr(0, 5) == "sd://") {
            const SDCredentials sdCredentials{
                std::string(),
                std::string(),
                utilities::TokenSourceFactory::getTokenSource()};

            const FileReaderOptions readerOptions{sdCredentials, CacheSizeOption{2 * 1024 * 1024, 32, 1, 1024U * 1024U}};

            return FileReference{fileName, readerOptions};
        }

        return FileReference{fileName, FileReaderOptions::localOnly()};
    }

    std::string getFileName(const std::string& fileName) const
    {
        if (std::filesystem::exists(fileName))
            return fileName;

        auto const filePath = utilities::Environment::instance().SegyLocalTestDataPath();
        if (filePath.empty()) {
            throw std::runtime_error("File " + fileName + " not found or not accessible. Please specify its location by the SEGY_LOCALTESTDATA_PATH environment variable to run this test.");
        }

        std::filesystem::path path(filePath);
        path.append("BulkReader");
        path.append(fileName);
        if (std::filesystem::exists(path))
            return path.string();

        throw std::runtime_error("File " + path.string() + " not found or not accessible.");
    }

    std::string getLocalFloatFileName() const
    {
        return getFileName("Testdata32.sgy");
    }

    std::string getLocalInt16FileName() const
    {
        return getFileName("Testdata16.sgy");
    }

    std::string getLocalInt8FileName() const
    {
        return getFileName("Testdata8.sgy");
    }

    SegyGleaner::Results getFloatFileDescription() const
    {
        const SegyGleaner::Results::Extent iline(840, 1, 110);
        const SegyGleaner::Results::Extent xline(450, 1, 111);
        const SegyGleaner::Results::Extent<double> sample(0, 4000, 501);
        return SegyGleaner::Results(iline, xline, sample, SampleFormat::IeeeFloat4);
    }

    SegyGleaner::Results getInt16FileDescription() const
    {
        const SegyGleaner::Results::Extent iline(0, 1, 91);
        const SegyGleaner::Results::Extent xline(0, 1, 106);
        const SegyGleaner::Results::Extent<double> sample(0, 4000, 501);
        return SegyGleaner::Results(iline, xline, sample, SampleFormat::INT2);
    }

    SegyGleaner::Results getInt8FileDescription() const
    {
        const SegyGleaner::Results::Extent iline(0, 1, 85);
        const SegyGleaner::Results::Extent xline(0, 1, 91);
        const SegyGleaner::Results::Extent<double> sample(0, 4000, 126);
        return SegyGleaner::Results(iline, xline, sample, SampleFormat::INT1);
    }

    SimpleBulkIndex getBulkIndex(const SegyGleaner::Results& source) const
    {
        return SimpleBulkIndex(source.getInlineExtent().getCount(), source.getCrosslineExtent().getCount(), source.getSampleExtent().getCount(), source.getSampleFormat());
    }

    std::unique_ptr<segysdk::utilities::auth::TokenSource>&
    getAuthenticationToken()
    {
        if (tokenSource_) {
            return tokenSource_;
        }

        auto tokenSource = utilities::TokenSourceFactory::getTokenSource();

        if (!tokenSource->token().empty()) {
            tokenSource_ = std::move(tokenSource);
        }
        else {
            const auto env_vars = utilities::Environment::instance();
            auto const fileName = env_vars.sdTokenFile();
            if (fileName.empty()) {
                throw std::runtime_error("The authentication token has not been defined. Please set it in the SD_SVC_TOKEN environment variable or set SD_SVC_TOKEN_FILE to designate a file containing it to run this test.");
            }

            std::ifstream file(fileName, std::ios::in);
            if (file.bad()) {
                throw std::runtime_error(std::string("Failure getting authentication token from ") + fileName);
            }

            std::string ftoken;
            file >> ftoken;

            tokenSource_ = utilities::auth::TokenSource::fromToken(ftoken);
        }

        return tokenSource_;
    }

    void deleteDataSet(const std::string& sddataset)
    {
        seismicdrive::SDManager sdmanager(std::string(), std::string(), true);
        sdmanager.setAuthProviderCallback(
            BulkReaderUnitTests::authCallback,
            getAuthenticationToken().get());

        seismicdrive::SDUtils utils(&sdmanager);
        if (utils.datasetExist(sddataset)) {
            utils.deleteDataset(sddataset);
        }
    }

    void testVolumeCommon(SegyBulkReader* reader, const BulkIndex& index) const
    {
        ASSERT_FALSE(reader == nullptr);

        bool const isMultiBulkReader = !!dynamic_cast<MultiBulkReader*>(reader);

        const size_t ni = static_cast<size_t>(index.getSizeI());
        const size_t nj = static_cast<size_t>(index.getSizeJ());
        const size_t nk = static_cast<size_t>(index.getSizeK());
        EXPECT_GT(ni, 30);
        EXPECT_GT(nj, 30);
        EXPECT_GT(nk, 120);

        struct Block
        {
            size_t i0;
            size_t j0;
            size_t k0;
            size_t ni;
            size_t nj;
            size_t nk;
        };

        std::array<Block, 10> const blocks = {
            // First trace
            Block{0, 0, 0, 1, 1, nk},
            // Another trace
            Block{ni / 2, nj / 3, 0, 1, 1, nk},
            // Last trace
            Block{ni - 1, nj - 1, 0, 1, 1, nk},
            // Inline
            Block{ni / 3 * 2, 0, 0, 1, nj, nk},
            // Crossline
            Block{0, nj / 4 * 3, 0, ni, 1, nk},
            // Slice
            Block{0, 0, nk / 3, ni, nj, 1},
            // Partial inline
            Block{ni / 3, 9, 33, 1, nj - 12, nk - 70},
            // Partial crossline
            Block{5, nj / 4, 19, ni - 13, 1, nk - 75},
            // Partial slice
            Block{22, 1, nk / 3, ni - 27, nj - 9, 1},
            // Block
            Block{ni / 2, nj / 3, nk / 4, ni / 3, nj / 2, nk / 3}};

        std::array<size_t, 10> multiBulkReaderOffsets{}; // for PushPull

        if (isMultiBulkReader) {
            reader->clearReadOffsetHints();
            size_t offsetIdx{};
            for (auto const& block : blocks) {
                multiBulkReaderOffsets[offsetIdx++] = reader->setReadOffsetHint(block.i0, block.j0, block.k0).value();
            }
        }

        for (auto const& block : blocks) {
            if (isMultiBulkReader) {
                reader->dropCachedData();
                size_t const offsetIdx = static_cast<size_t>(&block - &blocks[0]);
                reader->LoadDataToCache(multiBulkReaderOffsets[offsetIdx], 0U); // PushPull requires cache loading done in explicit way
            }

            testBlock(*reader, block.i0, block.j0, block.k0, block.ni, block.nj, block.nk);
        }
    }

    void testVolumeBricks(SegyBulkReader* reader, const BulkIndex& index, size_t brick_size_inline, size_t brick_size_crossline) const
    {
        ASSERT_FALSE(reader == nullptr);

        size_t const ni = static_cast<size_t>(index.getSizeI());
        size_t const nj = static_cast<size_t>(index.getSizeJ());
        size_t const nk = static_cast<size_t>(index.getSizeK());
        EXPECT_GT(ni, 30U);
        EXPECT_GT(nj, 30U);
        EXPECT_GT(nk, 120U);

        size_t const brick_size_vertical = (brick_size_inline + brick_size_crossline) / 2;

        auto getRemains = [](size_t x, size_t x_step, size_t x_max) -> size_t {
            return (x_max > (x + x_step)) ? x_step : (x_max - x);
        };

        reader->clearReadOffsetHints();

        for (size_t inline_offset = 0; inline_offset < ni; inline_offset += brick_size_inline) {
            for (size_t crossline_offset = 0; crossline_offset < nj; crossline_offset += brick_size_crossline) {
                for (size_t sample_offset = 0; sample_offset < nk; sample_offset += brick_size_vertical) {

                    reader->dropCachedData();
                    auto const offsetHint = reader->setReadOffsetHint(inline_offset, crossline_offset, sample_offset);
                    reader->LoadDataToCache(offsetHint.value(), 0U); // PushPull requires cache loading done in explicit way

                    auto const read_size_inline = getRemains(inline_offset, brick_size_inline, ni);
                    auto const read_size_crossline = getRemains(crossline_offset, brick_size_crossline, nj);
                    auto const read_size_vertical = getRemains(sample_offset, brick_size_vertical, nk);

                    testBlock(*reader,
                              inline_offset, crossline_offset, sample_offset,
                              read_size_inline, read_size_crossline, read_size_vertical);
                }
            }
        }
    }

    void testVolumeSections(SegyBulkReader* reader, const BulkIndex& index) const
    {
        ASSERT_FALSE(reader == nullptr);

        size_t const ni = static_cast<size_t>(index.getSizeI());
        size_t const nj = static_cast<size_t>(index.getSizeJ());
        size_t const nk = static_cast<size_t>(index.getSizeK());
        EXPECT_GT(ni, 30U);
        EXPECT_GT(nj, 30U);
        EXPECT_GT(nk, 120U);

        size_t const slice_size_vertical = nk / 3;

        auto getRemains = [](size_t x, size_t x_step, size_t x_max) -> size_t {
            return (x_max > (x + x_step)) ? x_step : (x_max - x);
        };

        //
        // check inline section reading
        //
        for (size_t inline_offset = 0; inline_offset < ni; ++inline_offset) {

            for (size_t sample_offset = 0; sample_offset < nk; sample_offset += slice_size_vertical) {

                auto const read_size_vertical = getRemains(sample_offset, slice_size_vertical, nk);

                testBlockSection(*reader, ReadDirection::Inline,
                                 inline_offset, 0U, sample_offset,
                                 nj, read_size_vertical);
            }
        }

        //
        // check crossline section reading
        //
        for (size_t crossline_offset = 0; crossline_offset < ni; ++crossline_offset) {

            for (size_t sample_offset = 0; sample_offset < nk; sample_offset += slice_size_vertical) {

                auto const read_size_vertical = getRemains(sample_offset, slice_size_vertical, nk);

                testBlockSection(*reader, ReadDirection::Crossline,
                                 0U, crossline_offset, sample_offset,
                                 ni, read_size_vertical);
            }
        }
    }

    void testBlock(SegyBulkReader& reader, size_t i0, size_t j0, size_t k0, size_t ni, size_t nj, size_t nk) const
    {
        std::vector<float> samples(static_cast<size_t>(ni) * nj * nk);

        reader.read(i0, j0, k0, ni, nj, nk, samples.data());

        verifyBlockContents(samples, i0, j0, k0, ni, nj, nk);
    }

    void testBlockSection(SegyBulkReader& reader, ReadDirection readDirection, size_t i0, size_t j0, size_t k0, size_t xCount, size_t nk) const
    {
        std::vector<float> samples(xCount * nk);

        reader.read(readDirection, i0, j0, k0, xCount, nk, samples.data());

        switch (readDirection) {
        case ReadDirection::Inline:
            {
                verifyBlockContents(samples, i0, j0, k0, 1U, xCount, nk);
            }
            break;
        case ReadDirection::Crossline:
            {
                verifyBlockContents(samples, i0, j0, k0, xCount, 1U, nk);
            }
            break;
        }
    }

    void verifyBlockContents(std::vector<float> const& samples, size_t i0, size_t j0, size_t k0, size_t ni, size_t nj, size_t nk) const
    {
        size_t ix = 0;
        for (size_t i = 0; i < ni; ++i) {
            for (size_t j = 0; j < nj; ++j) {
                for (size_t k = 0; k < nk; ++k) {
                    float sample = samples.at(ix++);
                    switch (k0 + k) {
                    case 50: // Trace number
                        break;
                    case 51:
                        EXPECT_EQ(static_cast<float>(i0 + i), sample);
                        break;
                    case 52:
                        EXPECT_EQ(static_cast<float>(j0 + j), sample);
                        break;
                    default:
                        EXPECT_EQ(static_cast<float>(k0 + k), sample);
                        break;
                    }
                }
            }
        }
    }

    void testRemoteVolume(const std::string& fileName, const std::string& datasetName, const BulkIndex& index)
    {
        const std::string param_ext = ".params.json";
        const std::string index_ext = ".idx";
        const std::string bin_ext = ".idx.bin";

        std::cout << std::endl
                  << "         uploading test data ...";
        upload(fileName, datasetName);
        upload(fileName + param_ext, datasetName + param_ext);
        upload(fileName + index_ext, datasetName + index_ext);

        std::cout << std::endl
                  << "         running test ...";

        std::cout << std::endl
                  << "         (1-3) BulkReader ...";
        testRegularVolumeWithBulkReader(fr(datasetName), index);

        std::cout << std::endl
                  << "         (2-3) MultiBulkReader ...";
        testRegularVolumeWithMultiBulkReader(fr(datasetName), index);

        std::cout << std::endl
                  << "         (3-3) SectionBulkReader ...";
        testRegularVolumeWithSectionBulkReader(fr(fileName), index);

        std::cout << std::endl
                  << "         testing complete, cleaning up ...";

        deleteDataSet(datasetName);
        deleteDataSet(datasetName + param_ext);
        deleteDataSet(datasetName + index_ext);
    }

    void testSimpleVolumeWithBulkReader(const FileReference& fileReference, const BulkIndex& index)
    {
        auto fileReader = GenericFileManager::createReader(fileio::FileManagerParams{fileReference});
        auto indexReader = GenericFileManager::createReader(fileio::FileManagerParams{fileReference});
        auto bulkIndex = std::make_shared<SimpleBulkIndex>(index.getSizeI(), index.getSizeJ(), index.getSizeK(), index.getSampleFormat());
        const auto bulkReader = std::make_unique<BulkReader>(fileReader, indexReader, bulkIndex);

        testVolumeCommon(bulkReader.get(), index);
    }

    void testSimpleVolumeWithMultiBulkReader(const FileReference& fileReference, const BulkIndex& index)
    {
        auto bulkIndex = std::make_shared<SimpleBulkIndex>(index.getSizeI(), index.getSizeJ(), index.getSizeK(), index.getSampleFormat());

        // the brick is supposed consisting of a single inline to make the MultiBulkReader behaving like the BulkReader
        size_t const brick_size_inline = static_cast<size_t>(index.getSizeI() * index.getSizeJ());
        size_t const brick_size_crossline = 1U;

        std::vector<decltype(GenericFileManager::createReader(fileio::FileManagerParams{}))> readers(brick_size_crossline, nullptr);
        std::generate(
            readers.begin(), readers.end(), [&fileReference]() -> auto {
                return GenericFileManager::createReader(fileio::FileManagerParams{fileReference});
            });

        const std::unique_ptr<SegyBulkReader> bulkReader = std::make_unique<MultiBulkReader>(std::move(readers), brick_size_inline, bulkIndex);

        size_t const defaultBufferSize = index.getSizeI() * index.getSizeJ() * (Primitives::getTraceHeaderSize() + index.getSizeK() * Primitives::getSampleSize(index.getSampleFormat()));
        bulkReader->setBufferSize(defaultBufferSize);

        testVolumeCommon(bulkReader.get(), index);
    }

    void testRegularVolumeWithBulkReader(const FileReference& fileReference, const BulkIndex& index)
    {
        const std::unique_ptr simpleReader(SegyReaderFactory::createSimpleReader(fileReference));
        testVolumeCommon(simpleReader.get(), index);

        const std::unique_ptr indexReader(SegyReaderFactory::createReader(/*segyFilesVector*/ {}, fileReference));
        testVolumeCommon(indexReader.get(), index);
    }

    void testRegularVolumeWithMultiBulkReader(const FileReference& fileReference, const BulkIndex& index)
    {
        {
            // the brick is supposed consisting of a single inline to make the MultiBulkReader behaving like the BulkReader
            size_t const brick_size_inline = static_cast<size_t>(index.getSizeI() * index.getSizeJ());
            size_t const brick_size_crossline = 1U;
            size_t const defaultBufferSize = index.getSizeI() * index.getSizeJ() * (Primitives::getTraceHeaderSize() + index.getSizeK() * Primitives::getSampleSize(index.getSampleFormat()));

            const std::unique_ptr simpleReader(SegyReaderFactory::createSimpleMultiBulkReader(fileReference, brick_size_inline, brick_size_crossline));
            simpleReader->setBufferSize(defaultBufferSize);
            testVolumeCommon(simpleReader.get(), index);
        }

        {
            // the brick is supposed consisting of a single inline to make the MultiBulkReader behaving like the BulkReader
            size_t const brick_size_inline = static_cast<size_t>(index.getSizeI() * index.getSizeJ());
            size_t const brick_size_crossline = 1U;
            size_t const defaultBufferSize = index.getSizeI() * index.getSizeJ() * (Primitives::getTraceHeaderSize() + index.getSizeK() * Primitives::getSampleSize(index.getSampleFormat()));

            const std::unique_ptr indexReader(SegyReaderFactory::createMultiBulkReader(/*segyFilesVector*/ {}, fileReference, brick_size_inline, brick_size_crossline));
            indexReader->setBufferSize(defaultBufferSize);
            testVolumeCommon(indexReader.get(), index);
        }

        {
            size_t const brick_size_inline = 16U;
            size_t const brick_size_crossline = 16U;
            size_t const defaultBufferSize = brick_size_inline * brick_size_crossline * (Primitives::getTraceHeaderSize() + index.getSizeK() * Primitives::getSampleSize(index.getSampleFormat()));

            const std::unique_ptr multiReader(SegyReaderFactory::createMultiBulkReader(/*segyFilesVector*/ {}, fileReference, brick_size_inline, brick_size_crossline));
            multiReader->setBufferSize(defaultBufferSize);

            testVolumeBricks(multiReader.get(), index, brick_size_inline, brick_size_crossline);
        }
    }

    void testRegularVolumeWithSectionBulkReader(const FileReference& fileReference, const BulkIndex& index)
    {
        size_t const number_of_readers = 16U;

        const std::unique_ptr sectionReader(SegyReaderFactory::createSectionBulkReader(/*segyFilesVector*/ {}, fileReference, number_of_readers));

        testVolumeSections(sectionReader.get(), index);
    }

    void testGleanerResults(const FileReference& fileReference, const SegyGleaner::Results& expected) const
    {
        SegyGleaner gleaner(fileReference);
        const bool success = gleaner.glean();
        EXPECT_TRUE(success);
        const auto results = gleaner.getResults();
        EXPECT_EQ(expected.getInlineExtent().getFirst(), results.getInlineExtent().getFirst());
        EXPECT_EQ(expected.getInlineExtent().getIncrement(), results.getInlineExtent().getIncrement());
        EXPECT_EQ(expected.getInlineExtent().getCount(), results.getInlineExtent().getCount());
        EXPECT_EQ(expected.getCrosslineExtent().getFirst(), results.getCrosslineExtent().getFirst());
        EXPECT_EQ(expected.getCrosslineExtent().getIncrement(), results.getCrosslineExtent().getIncrement());
        EXPECT_EQ(expected.getCrosslineExtent().getCount(), results.getCrosslineExtent().getCount());
        EXPECT_EQ(expected.getSampleExtent().getFirst(), results.getSampleExtent().getFirst());
        EXPECT_EQ(expected.getSampleExtent().getIncrement(), results.getSampleExtent().getIncrement());
        EXPECT_EQ(expected.getSampleExtent().getCount(), results.getSampleExtent().getCount());
        EXPECT_EQ(expected.getSampleFormat(), results.getSampleFormat());
    }

    void copy(const FileReference& sourceFile, const FileReference& targetFile) const
    {
        const auto source = GenericFileManager::createReader(fileio::FileManagerParams{sourceFile});
        const auto target = GenericFileManager::createWriter(targetFile, WriteMode::Truncate);
        size_t size = source->getFileSize();
        const size_t bufferSize = size / 13 + 1;
        size_t offset = 0;

        while (size > 0) {
            const size_t segmentSize = std::min(size, bufferSize);
            const auto& buffer = source->read(
                fileio::FileReadParams{}
                    .withOffset(offset)
                    .withSize(segmentSize));
            target->write(buffer.data(), buffer.size());

            offset += segmentSize;
            size -= segmentSize;
        }

        EXPECT_EQ(source->getFileSize(), target->getFileSize());

        source->close();
        target->close();
    }

    void upload(const std::string& localfile, const std::string& sddataset)
    {
        seismicdrive::SDManager sdmanager(std::string(), std::string(), 0);
        sdmanager.setAuthProviderCallback(
            BulkReaderUnitTests::authCallback,
            getAuthenticationToken().get());

        std::fstream file(localfile, std::ios::in | std::ios::binary | std::ios::ate);
        const std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        seismicdrive::SDGenericDataset dataset(&sdmanager, sddataset);
        dataset.open(seismicdrive::SDDatasetDisposition::OVERWRITE);

        // Split the dataset to exercise block handling
        const size_t blockSize = (size / 2 + 2047) & ~1023;
        std::vector<char> buffer(blockSize);

        size_t remainingSize = size;
        int block = 0;
        while (remainingSize > 0) {
            const size_t thisSize = std::min(blockSize, remainingSize);
            file.read(buffer.data(), thisSize);
            dataset.writeBlock(block, buffer.data(), thisSize);

            remainingSize -= thisSize;
            ++block;
        }

        dataset.flush();
        dataset.close();
    }

    void compareIndex(const BulkIndex& expected, const BulkIndex& index) const
    {
        EXPECT_EQ(expected.getSizeI(), index.getSizeI());
        EXPECT_EQ(expected.getSizeJ(), index.getSizeJ());
        EXPECT_EQ(expected.getSizeK(), index.getSizeK());
        EXPECT_EQ(expected.getSampleFormat(), index.getSampleFormat());
    }
};

TEST_F(BulkReaderUnitTests, testLocalWriter)
{
    const std::string fileName = getLocalFloatFileName();
    const SegyGleaner::Results description = getFloatFileDescription();
    const SimpleBulkIndex index = getBulkIndex(description);

    auto tempName = std::filesystem::temp_directory_path();
    tempName.append("WriterTest.sgy");

    copy(fr(fileName), fr(tempName.string()));
    testSimpleVolumeWithBulkReader(fr(tempName.string()), index);
    testSimpleVolumeWithMultiBulkReader(fr(tempName.string()), index);
    GenericFileManager::remove(fr(tempName.string()));
}

TEST_F(BulkReaderUnitTests, DISABLED_testRemoteWriter)
{

    const std::string sdName("sd://sntc/geophysics/SegyTest/TestSegyReader/TestWriter.sgy");
    const std::string fileName = getLocalFloatFileName();
    const SegyGleaner::Results description = getFloatFileDescription();
    const SimpleBulkIndex index = getBulkIndex(description);

    copy(fr(fileName), fr(sdName));
    testSimpleVolumeWithBulkReader(fr(sdName), index);
    testSimpleVolumeWithMultiBulkReader(fr(sdName), index);
    GenericFileManager::remove(fr(sdName));
}

TEST_F(BulkReaderUnitTests, testLocalFloatFile)
{
    const AutoCleanupTestFile tempFile{getLocalFloatFileName()};
    tempFile.create_index();
    const std::string fileName = tempFile.get_path();
    const SegyGleaner::Results description = getFloatFileDescription();
    const SimpleBulkIndex index = getBulkIndex(description);

    testRegularVolumeWithBulkReader(fr(fileName), index);
    testRegularVolumeWithMultiBulkReader(fr(fileName), index);
    testRegularVolumeWithSectionBulkReader(fr(fileName), index);
}

TEST_F(BulkReaderUnitTests, testLocalInt16File)
{
    const AutoCleanupTestFile tempFile{getLocalInt16FileName()};
    tempFile.create_index();
    const std::string fileName = tempFile.get_path();
    const SegyGleaner::Results description = getInt16FileDescription();
    const SimpleBulkIndex index = getBulkIndex(description);

    testRegularVolumeWithBulkReader(fr(fileName), index);
    testRegularVolumeWithMultiBulkReader(fr(fileName), index);
    testRegularVolumeWithSectionBulkReader(fr(fileName), index);
}

TEST_F(BulkReaderUnitTests, testLocalInt8File)
{
    const AutoCleanupTestFile tempFile{getLocalInt8FileName()};
    tempFile.create_index();
    const std::string fileName = tempFile.get_path();
    const SegyGleaner::Results description = getInt8FileDescription();
    const SimpleBulkIndex index = getBulkIndex(description);

    testRegularVolumeWithBulkReader(fr(fileName), index);
    testRegularVolumeWithMultiBulkReader(fr(fileName), index);
    testRegularVolumeWithSectionBulkReader(fr(fileName), index);
}

TEST_F(BulkReaderUnitTests, DISABLED_testRemoteFloatFile)
{

    const std::string sdName("sd://sntc/geophysics/SegyTest/TestSegyReader/Testdata32.sgy");
    const std::string fileName = getLocalFloatFileName();
    const SegyGleaner::Results description = getFloatFileDescription();
    const SimpleBulkIndex index = getBulkIndex(description);

    testRemoteVolume(fileName, sdName, index);
}

TEST_F(BulkReaderUnitTests, DISABLED_testRemoteInt16File)
{

    const std::string sdName("sd://sntc/geophysics/SegyTest/TestSegyReader/Testdata16.sgy");
    const std::string fileName = getLocalInt16FileName();
    const SegyGleaner::Results description = getInt16FileDescription();
    const SimpleBulkIndex index = getBulkIndex(description);

    testRemoteVolume(fileName, sdName, index);
}

TEST_F(BulkReaderUnitTests, DISABLED_testRemoteInt8File)
{

    const std::string sdName("sd://sntc/geophysics/SegyTest/TestSegyReader/Testdata8.sgy");
    const std::string fileName = getLocalInt8FileName();
    const SegyGleaner::Results description = getInt8FileDescription();
    const SimpleBulkIndex index = getBulkIndex(description);

    testRemoteVolume(fileName, sdName, index);
}

TEST_F(BulkReaderUnitTests, testGleaner)
{
    testGleanerResults(fr(getLocalFloatFileName()), getFloatFileDescription());
    testGleanerResults(fr(getLocalInt16FileName()), getInt16FileDescription());
    testGleanerResults(fr(getLocalInt8FileName()), getInt8FileDescription());
}
} // namespace segysdk::bulkaccess::tests
