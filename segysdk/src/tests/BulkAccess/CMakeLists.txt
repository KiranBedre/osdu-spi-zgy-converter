add_executable(UnitTests.BulkReader
    Runner.cpp
    TestAsyncReader.cpp
    TestBulkReader.cpp
    Test2DReader.cpp
)

# TODO use SDAPI as CMake library
target_include_directories(UnitTests.BulkReader
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
    ${jsoncpp_INCLUDE_PATH}
)

target_link_libraries(UnitTests.BulkReader
PRIVATE
    segysdk.testing
    segysdk
    gtest_main
    ${JSON_LINK_LIBS}
    SDAPI::SDAPI
)

include(GoogleTest)
gtest_discover_tests(UnitTests.BulkReader)

add_test(
    NAME BulkAccess 
    COMMAND UnitTests.BulkReader
)