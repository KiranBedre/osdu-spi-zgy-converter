add_library(segysdk.bin_grid_calculation_plugin STATIC
    BinGridCalculationInterface.h
    BinGridCalculationPluginException.h
    BinGridPluginParams.h BinGridPluginParams.cpp
    BinGridCalculationPlugin.h BinGridCalculationPlugin.cpp
)

target_include_directories(segysdk.bin_grid_calculation_plugin
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
	${jsoncpp_INCLUDE_PATH}
)

target_compile_definitions(segysdk.bin_grid_calculation_plugin 
PRIVATE
    SEGYSDK_BUILDING_DLL
)
