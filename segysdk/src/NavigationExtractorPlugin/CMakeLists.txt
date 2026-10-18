add_library(segysdk.navigation_extractor_plugin STATIC
    InputParams.h                           InputParams.cpp
    NavigationExtractorInterface.h
    NavigationExtractorPluginException.h
    NavigationExtractorPlugin.h             NavigationExtractorPlugin.cpp
    NavigationExtractorPluginException.h
    StatusCodes.h
    NavigationExtractorCInterface.h
)

target_include_directories(segysdk.navigation_extractor_plugin 
PRIVATE
    ${CMAKE_SEGY_INTERNAL_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
    ${jsoncpp_INCLUDE_PATH}
)

target_compile_definitions(segysdk.navigation_extractor_plugin 
PRIVATE 
    SEGYSDK_BUILDING_DLL 
) 
