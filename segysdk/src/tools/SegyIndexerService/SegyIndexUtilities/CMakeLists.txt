
add_library(segysdk.indexing_utilities STATIC     
     FileCollectionUtilities.h  FileCollectionUtilities.cpp
     IndexingReturnStatus.h  IndexingReturnStatus.cpp
     SeismicTraceDataUtilities.h SeismicTraceDataUtilities.cpp
)

target_include_directories(segysdk.indexing_utilities
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
    ${jsoncpp_INCLUDE_PATH}
)
