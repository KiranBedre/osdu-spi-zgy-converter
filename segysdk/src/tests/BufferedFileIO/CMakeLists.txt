find_package(SDAPI REQUIRED)

add_executable(UnitTests.BufferedFileIo
    BufferedFileTestUtils.h BufferedFileTestUtils.cpp
    TestBufferedFileReader.cpp
    TestMultiBufferedFileReader.cpp
    TestSequentialFileReader.cpp
    TestSectionFileReader.cpp
)

target_include_directories(UnitTests.BufferedFileIo
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
    ${jsoncpp_INCLUDE_PATH}
)

target_link_libraries(UnitTests.BufferedFileIo
PRIVATE
    segysdk
    gtest_main
    SDAPI::SDAPI
    ${CMAKE_THREAD_LIBS_INIT}    
    ${JSON_LINK_LIBS}
)

include(GoogleTest)
gtest_discover_tests(UnitTests.BufferedFileIo)

add_test(
    NAME BufferedFileIO 
    COMMAND UnitTests.BufferedFileIo
)