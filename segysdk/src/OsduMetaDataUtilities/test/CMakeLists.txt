include(AddUnitTest)

AddUnitTest(UnitTests.OsduFileImagePng
    SOURCE_FILES
        ${CMAKE_SEGYSDK_SOURCE_PATH}/OsduMetaDataUtilities/OsduFileImagePng.cpp
        OsduFileImagePngTest.cpp
    SYSTEM_INCLUDE_DIRECTORIES
        ${jsoncpp_INCLUDE_PATH}
    LINK_LIBRARIES
        segysdk.json_parser
)
