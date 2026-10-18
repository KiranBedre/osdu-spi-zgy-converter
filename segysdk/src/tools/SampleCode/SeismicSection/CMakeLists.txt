
add_executable(SampleCode.SeismicSectionBMP 
     ColourTable.h   ColourTable.cpp
     GenerateSeismicSection.cpp

)

target_include_directories(SampleCode.SeismicSectionBMP
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
    ${jsoncpp_INCLUDE_PATH}
)

target_link_libraries(SampleCode.SeismicSectionBMP
PRIVATE 
    SampleCode.Common 
    SampleCode.BMPGenerator  
    segysdk
    ${JSON_LINK_LIBS}
)
