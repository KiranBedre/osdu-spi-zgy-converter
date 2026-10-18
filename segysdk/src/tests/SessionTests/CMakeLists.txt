enable_testing()

add_executable(UnitTests.SegySession
    SessionTest.cpp
)

target_link_libraries(UnitTests.SegySession
PRIVATE
    segysdk
    gtest_main
    ${JSON_LINK_LIBS}
)

target_include_directories(UnitTests.SegySession
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
    ${jsoncpp_INCLUDE_PATH}
)

include(GoogleTest)
gtest_discover_tests(UnitTests.SegySession)

add_test(
    NAME SegySessionTests
    COMMAND UnitTests.SegySession
)