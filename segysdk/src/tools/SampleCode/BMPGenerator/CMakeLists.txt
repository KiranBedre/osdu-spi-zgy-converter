add_library(SampleCode.BMPGenerator STATIC
     
    Colour.h  Colour.cpp
    BMPFileHeader.h  BMPFileHeader.cpp
    BMPInformationHeader.h  BMPInformationHeader.cpp
    BMPImageSection.h  BMPImageSection.cpp
)

target_include_directories(SampleCode.BMPGenerator
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
    ${jsoncpp_INCLUDE_PATH}
)
