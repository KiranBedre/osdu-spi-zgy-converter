
add_executable(SampleCode.SegyIndexer     
     SegyIndexer.cpp
)

target_include_directories(SampleCode.SegyIndexer
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
    ${jsoncpp_INCLUDE_PATH}
)

target_link_libraries(SampleCode.SegyIndexer
PRIVATE 
    SampleCode.Common  
    segysdk
    ${JSON_LINK_LIBS}
)
