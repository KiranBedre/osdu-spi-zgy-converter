
add_executable(SampleCode.MetadataBrowser
     MetadataBrowser.cpp
)

target_include_directories(SampleCode.MetadataBrowser
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
    ${jsoncpp_INCLUDE_PATH}
)

target_link_libraries(SampleCode.MetadataBrowser
PRIVATE 
    SampleCode.Common  
    segysdk
    ${JSON_LINK_LIBS}
)
