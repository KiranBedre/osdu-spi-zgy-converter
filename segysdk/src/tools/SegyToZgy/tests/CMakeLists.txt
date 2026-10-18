include(GoogleTest)

add_executable(UnitTests.SegyToZgyModules 
    AuthTests.cpp
    BinGridTests.cpp
    EnvironmentTests.cpp
    OsduRecordsTests.cpp
    SegyInputParametersTests.cpp
    OsduSeismicTraceDataTests.cpp
)

target_include_directories(UnitTests.SegyToZgyModules
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
    ${jsoncpp_INCLUDE_PATH}
    ${vcpkg_INCLUDE_PATH}
)

target_link_libraries(UnitTests.SegyToZgyModules 
PRIVATE 
    segytozgy.conversion 
    segysdk
    segysdk.testing
    gtest_main 
    ${JSON_LINK_LIBS}
    SDAPI::SDAPI
    OpenZGY::OpenZGY
    ${CPPRESTSDK_LINK_LIBS}
)

gtest_discover_tests(UnitTests.SegyToZgyModules)
