add_library(ApiTestFramework STATIC
    ApiTest.h
    ApiTestBase.h ApiTestBase.cpp
    ApiTestConfig.h
    ApiTestException.h
    ApiTestParamGenerator.h ApiTestParamGenerator.cpp
    ApiEnvironment.h ApiEnvironment.cpp
    CleanupFiles.h CleanupFiles.cpp
    ApiTestParams.h 
    ApiTestResult.h ApiTestResult.cpp
    ApiTestUtilities.h ApiTestUtilities.cpp
)

target_include_directories(ApiTestFramework
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
    ${jsoncpp_INCLUDE_PATH}
)

target_link_libraries(ApiTestFramework
PRIVATE
    segysdk
    gtest_main
    JsonSerializers
    ${JSON_LINK_LIBS}
)