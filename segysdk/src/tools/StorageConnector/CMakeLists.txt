project(ToolStorageConnector)

include(AddLibrary)

AddStaticLibrary(${PROJECT_NAME}
    SOURCE_FILES
        StorageConnector.cpp
    HEADER_FILES
        StorageConnector.h
    LINK_LIBRARIES
        ${JSON_LINK_LIBS}
        ToolContext
        ToolError
        ToolFileCollection
        ToolStorageClient
        segysdk.OsduMetadataUtilities
        segysdk.json_parser
    SYSTEM_INCLUDE_DIRECTORIES
        ${jsoncpp_INCLUDE_PATH}
)

add_subdirectory(test)
