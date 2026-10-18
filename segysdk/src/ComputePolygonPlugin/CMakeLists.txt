add_library(segysdk.compute_polygon_plugin STATIC
    ComputePolygonInterface.h
    ComputePolygonPluginException.h
    ComputePolygonPlugin.h ComputePolygonPlugin.cpp
)

target_include_directories(segysdk.compute_polygon_plugin
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
	${jsoncpp_INCLUDE_PATH}
)

target_compile_definitions(segysdk.compute_polygon_plugin 
PRIVATE
    SEGYSDK_BUILDING_DLL
)
