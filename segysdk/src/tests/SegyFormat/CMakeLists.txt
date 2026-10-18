enable_testing()

add_executable(UnitTests.SegyFormat
    JsonLoaderTestDataProvider.h  JsonLoaderTestDataProvider.cpp
    LoadContextFromOSDUJsonUnitTests.cpp
    SegyFormatUnitTests.cpp
)

target_link_libraries(UnitTests.SegyFormat
PRIVATE
    segysdk
    gtest_main
    ${JSON_LINK_LIBS}
)

target_include_directories(UnitTests.SegyFormat
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
    ${jsoncpp_INCLUDE_PATH}
)

include(GoogleTest)
gtest_discover_tests(UnitTests.SegyFormat)

add_test(
    NAME UnitTests.SegyFormat
    COMMAND UnitTests.SegyFormat
)