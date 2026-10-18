enable_testing()

add_executable(UnitTests.SegyServiceSession
    ServiceSessionTest.cpp
)

target_link_libraries(UnitTests.SegyServiceSession
PRIVATE
    segysdk
    segysdk.testing
    gtest_main
    ${JSON_LINK_LIBS}
)

target_include_directories(UnitTests.SegyServiceSession
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
    ${jsoncpp_INCLUDE_PATH}
)

include(GoogleTest)
gtest_discover_tests(UnitTests.SegyServiceSession)

add_test(
    NAME SegyServiceSessionTests
    COMMAND UnitTests.SegyServiceSession
)