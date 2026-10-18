project(UnitTests.SegyIndexerService)

include(AddUnitTest)

AddUnitTest(${PROJECT_NAME}
    SOURCE_FILES
        ${CMAKE_SEGYSDK_SOURCE_PATH}/tools/SegyIndexerService/SegyIndexing/DataPartition.cpp
        DataPartitionTest.cpp
    SYSTEM_INCLUDE_DIRECTORIES
        ${jsoncpp_INCLUDE_PATH}
    LINK_LIBRARIES
        ${JSON_LINK_LIBS}
        segysdk.json_parser
)
