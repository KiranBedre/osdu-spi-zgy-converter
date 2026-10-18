include(GoogleTest)

add_executable(UnitTests.PublicUtilities
    SafeTimeTests.cpp
)

target_include_directories(UnitTests.PublicUtilities
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
    ${jsoncpp_INCLUDE_PATH}
)

target_link_libraries(UnitTests.PublicUtilities 
PRIVATE 
	segysdk
	gtest_main 
	${JSON_LINK_LIBS}
)

gtest_discover_tests(UnitTests.PublicUtilities)

add_executable(UnitTests.StringUtilities
    StringUtilitiesTests.cpp
)

target_include_directories(UnitTests.StringUtilities
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
)

target_link_libraries(UnitTests.StringUtilities 
PRIVATE 
    segysdk
    gtest_main
    ${JSON_LINK_LIBS}
)

gtest_discover_tests(UnitTests.StringUtilities)

add_executable(UnitTests.FileNameUtilities
    FileNameTests.cpp
)

target_include_directories(UnitTests.FileNameUtilities
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
)

target_link_libraries(UnitTests.FileNameUtilities 
PRIVATE 
    segysdk
    gtest_main
)

gtest_discover_tests(UnitTests.FileNameUtilities)

add_executable(UnitTests.EnvironmentVariableUtilities
    EnvironmentVariableTests.cpp
)

target_include_directories(UnitTests.EnvironmentVariableUtilities
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
)

target_link_libraries(UnitTests.EnvironmentVariableUtilities
PRIVATE 
    segysdk
    gtest_main
)

gtest_discover_tests(UnitTests.EnvironmentVariableUtilities)

add_executable(UnitTests.FileSystemUtilities
    FilePathTests.cpp
)

target_include_directories(UnitTests.FileSystemUtilities
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
)

target_link_libraries(UnitTests.FileSystemUtilities 
PRIVATE 
    segysdk
    gtest_main
)

gtest_discover_tests(UnitTests.FileSystemUtilities)


add_test(
    NAME UnitTests.PublicUtilities
    COMMAND UnitTests.PublicUtilities
)

add_test(
    NAME UnitTests.StringUtilities
    COMMAND UnitTests.StringUtilities
)

add_test(
    NAME UnitTests.FileNameUtilities
    COMMAND UnitTests.FileNameUtilities
)

add_test(
    NAME UnitTests.EnvironmentVariableUtilities
    COMMAND UnitTests.EnvironmentVariableUtilities
)

add_test(
    NAME UnitTests.FileSystemUtilities
    COMMAND UnitTests.FileSystemUtilities
)