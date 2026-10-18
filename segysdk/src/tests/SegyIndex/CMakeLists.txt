add_executable(UnitTests.SegyIndex
    Expected.h
    FakeFileManager.h
    GridCalculatorUnitTests.cpp
    IndexBuilderDataManagerTests.cpp
    IndexedFileNamesUnitTests.cpp
    IndexGenerationUnitTests.cpp
    IndexRestoreUnitTests.cpp
    Index2DGenerationUnitTests.cpp
    Index2DLoading.cpp
    JsonUnpacker.h JsonUnpacker.cpp
    MiscSegyIndexImplTests.cpp
    Parameters.h
    Units.h Units.cpp
    UnitTestDataProvider.h UnitTestDataProvider.cpp
    VectorUnitTests.cpp
    TraceRangeIndexUnitTests.cpp
    TraceRangeFactoryUnitTests.cpp
    TraceRangeGapsUnitTests.cpp
    TraceRangeTestUtils.h TraceRangeTestUtils.cpp
)

target_include_directories(UnitTests.SegyIndex
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
    ${jsoncpp_INCLUDE_PATH}
)

target_link_libraries(UnitTests.SegyIndex
PRIVATE
    segysdk
    segysdk.testing
    gtest_main
    ${JSON_LINK_LIBS}
)

include(GoogleTest)
gtest_discover_tests(UnitTests.SegyIndex)

# This one is cloud based and, thus, is included as E2E test
add_executable(UnitTests.IndexedFileNamesCloud
    IndexedFileNamesCloudUnitTests.cpp
)

target_include_directories(UnitTests.IndexedFileNamesCloud
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
    ${jsoncpp_INCLUDE_PATH}
)

target_link_libraries(UnitTests.IndexedFileNamesCloud
PRIVATE
    segysdk
    segysdk.testing
    gtest_main
    ${JSON_LINK_LIBS}
)

add_test(
    NAME UnitTests.SegyIndex
    COMMAND UnitTests.SegyIndex
)
