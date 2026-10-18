project(segysdk.segy_indexer_service)

include(AddExecutable)

AddExecutable(${PROJECT_NAME}
    SOURCE_FILES
        DataPartition.cpp
        Params.cpp
        SegyIndexerService.cpp
    HEADER_FILES
        DataPartition.h
        Params.h
    SYSTEM_INCLUDE_DIRECTORIES
        ${jsoncpp_INCLUDE_PATH}
    LINK_LIBRARIES
        ${JSON_LINK_LIBS}
        segysdk
        segysdk.indexing_utilities
)

add_subdirectory(test)
