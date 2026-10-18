include(GoogleTest)

add_executable(UnitTests.Logging
    LoggingTests.cpp
)
target_include_directories(UnitTests.Logging
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${jsoncpp_INCLUDE_PATH}
)
target_link_libraries(UnitTests.Logging
PRIVATE
	segysdk
    gtest_main
)
if(WIN32)
    target_link_libraries(UnitTests.Logging
        PUBLIC
        Rpcrt4.lib
    )
elseif(UNIX)
    target_link_libraries(UnitTests.Logging
        PUBLIC
        uuid
    )
endif()
gtest_discover_tests(UnitTests.Logging)

add_executable(UnitTests.MessagePool
    MessagePoolTests.cpp
)
target_include_directories(UnitTests.MessagePool
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
)
target_link_libraries(UnitTests.MessagePool
PRIVATE
	segysdk
    gtest_main
)
gtest_discover_tests(UnitTests.MessagePool)

add_executable(UnitTests.GsmModule
    GsmModuleTests.cpp
)
target_include_directories(UnitTests.GsmModule
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
    ${jsoncpp_INCLUDE_PATH}
)
target_link_libraries(UnitTests.GsmModule
PRIVATE
	segysdk
    gtest_main
)
gtest_discover_tests(UnitTests.GsmModule)

add_test(
    NAME UnitTests.Logging
    COMMAND UnitTests.Logging
)

add_test(
    NAME UnitTests.MessagePool
    COMMAND UnitTests.MessagePool
)

add_test(
    NAME UnitTests.GsmModule
    COMMAND UnitTests.GsmModule
)
