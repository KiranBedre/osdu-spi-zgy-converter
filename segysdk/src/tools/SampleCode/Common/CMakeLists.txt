add_library(SampleCode.Common STATIC
     EnvironmentVariables.h  EnvironmentVariables.cpp
     FileCollectionUtilities.h  FileCollectionUtilities.cpp
     JsonToolKit.h          JsonToolKit.cpp
     JsonParseException.h
     SessionUtilities.h   SessionUtilities.cpp
)

target_include_directories(SampleCode.Common
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
    ${jsoncpp_INCLUDE_PATH}
)

add_executable(SampleCode.CommonTest
    CommonTest.cpp   
)

target_include_directories(SampleCode.CommonTest
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
    ${jsoncpp_INCLUDE_PATH}
)

target_link_libraries(SampleCode.CommonTest  
PRIVATE 
    SampleCode.Common 
    segysdk
    ${JSON_LINK_LIBS}
)
