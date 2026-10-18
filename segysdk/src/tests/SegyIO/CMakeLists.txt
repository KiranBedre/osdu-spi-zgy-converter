add_executable(UnitTests.SegyIO
    BinaryHeaderTests.cpp
    FileHeaderDescriptionUnitTests.cpp
    TextualHeaderTests.cpp
)

target_include_directories(UnitTests.SegyIO
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
    ${jsoncpp_INCLUDE_PATH}
    ${vcpkg_INCLUDE_PATH}
)

target_link_libraries(UnitTests.SegyIO 
PRIVATE 
    segysdk
    gtest_main
    ${JSON_LINK_LIBS}
    ${CPPRESTSDK_LINK_LIBS}
)

include(GoogleTest)
gtest_discover_tests(UnitTests.SegyIO)

add_test(
    NAME UnitTests.SegyIO
    COMMAND UnitTests.SegyIO
)