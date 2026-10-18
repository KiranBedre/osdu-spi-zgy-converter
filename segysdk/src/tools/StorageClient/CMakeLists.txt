project(ToolStorageClient)

include(AddLibrary)

AddStaticLibrary(${PROJECT_NAME}
    SOURCE_FILES
        StorageClient.cpp
        StorageClientLive.cpp
    HEADER_FILES
        StorageClient.h
        StorageClientLive.h
    SYSTEM_INCLUDE_DIRECTORIES
        ${jsoncpp_INCLUDE_PATH}
)
