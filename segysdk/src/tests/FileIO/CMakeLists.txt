add_executable(UnitTests.FileIO
    PagedFileReaderTests.cpp
    ReadCacheTests.cpp
    SDFileManagerTest.cpp
)

target_include_directories(UnitTests.FileIO
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
    ${jsoncpp_INCLUDE_PATH}
)

target_link_libraries(UnitTests.FileIO 
PRIVATE 
    segysdk
    gtest_main 
    SDAPI::SDAPI
    ${JSON_LINK_LIBS}
)

include(GoogleTest)
gtest_discover_tests(UnitTests.FileIO)

add_test(
    NAME UnitTests.FileIO
    COMMAND UnitTests.FileIO
)