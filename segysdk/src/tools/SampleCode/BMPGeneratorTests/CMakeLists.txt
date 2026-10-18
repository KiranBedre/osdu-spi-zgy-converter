add_executable(SampleCode.BMPGeneratorTests 
     GenerateBMPTest.cpp
)

target_include_directories(SampleCode.BMPGeneratorTests 
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
    ${jsoncpp_INCLUDE_PATH}
)

target_link_libraries(SampleCode.BMPGeneratorTests 
PRIVATE 
    SampleCode.Common 
    SampleCode.BMPGenerator  
    segysdk
    SDAPI::SDAPI
    ${JSON_LINK_LIBS}
)
