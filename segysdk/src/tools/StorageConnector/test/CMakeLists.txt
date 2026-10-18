project(UnitTests.ToolStorageConnector)

include(AddUnitTest)

AddUnitTest(${PROJECT_NAME}
    SOURCE_FILES
        ${CMAKE_SEGYSDK_SOURCE_PATH}/tools/StorageConnector/StorageConnector.cpp
        ${CMAKE_SEGYSDK_SOURCE_PATH}/tools/StorageClient/StorageClient.cpp
        StorageClient.cpp
        StorageConnectorTest.cpp
    SYSTEM_INCLUDE_DIRECTORIES
        ${jsoncpp_INCLUDE_PATH}
    LINK_LIBRARIES
        ${JSON_LINK_LIBS}
        ToolContext
        ToolError
        ToolFileCollection
        ToolParams
        segysdk
        segysdk.OsduMetadataUtilities
)
