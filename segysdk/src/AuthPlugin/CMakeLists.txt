add_library(segysdk.auth_plugin STATIC
	AuthPluginCInterface.h
	AuthPluginCFunctionType.h
	AuthPluginInterface.cpp AuthPluginInterface.h
	AuthPluginFactory.cpp AuthPluginFactory.h
	AuthPluginLibWrapperImpl.cpp AuthPluginLibWrapperImpl.h
	AuthPluginBasicImpl.cpp AuthPluginBasicImpl.h
	AuthPluginException.h
)

target_include_directories(segysdk.auth_plugin
PRIVATE
	${CMAKE_SEGYSDK_SOURCE_PATH}/include
)

target_compile_definitions(segysdk.auth_plugin
PRIVATE
	SEGYSDK_BUILDING_DLL
)

add_library(segysdk.auth_plugin.template SHARED
	AuthPluginReferenceTemplate.cpp
	AuthPluginCInterface.h
	AuthPluginInterface.cpp AuthPluginInterface.h
	AuthPluginBasicImpl.cpp AuthPluginBasicImpl.h
)

target_include_directories(segysdk.auth_plugin.template
PRIVATE
	${CMAKE_SEGYSDK_SOURCE_PATH}/include
)

target_compile_definitions(segysdk.auth_plugin.template
PRIVATE
	SEGYSDK_BUILDING_DLL
)

target_link_libraries(segysdk.auth_plugin.template
PRIVATE
	segysdk
)
