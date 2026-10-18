add_executable(UnitTests.segysdk.PopulateOsduFileList
    PopulateOsduFileListTest.cpp 
)

target_include_directories(UnitTests.segysdk.PopulateOsduFileList
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
    ${jsoncpp_INCLUDE_PATH}
)

target_link_libraries(UnitTests.segysdk.PopulateOsduFileList  
PRIVATE 
    segysdk.indexing_utilities
    segysdk
    ${JSON_LINK_LIBS}
)
