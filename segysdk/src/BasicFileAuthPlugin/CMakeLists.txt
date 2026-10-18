message(STATUS "segysdk.auth_plugin.BasicFileAuthPlugin include dir    ${CMAKE_SEGY_INTERNAL_SOURCE_PATH}")
message(STATUS "segysdk.auth_plugin.BasicFileAuthPlugin include dir    ${CMAKE_SEGYSDK_SOURCE_PATH}")

add_library(segysdk.auth_plugin.BasicFile SHARED

	AuthPluginBasicFileImpl.h AuthPluginBasicFileImpl.cpp
	BasicFilePluginReferenceTemplate.cpp
	BasicFilePluginException.h
	BasicFileEnvironment.h  BasicFileEnvironment.cpp
	BasicFileVariableStringConsts.h
)

target_include_directories(segysdk.auth_plugin.BasicFile
PRIVATE
	${CMAKE_SEGY_INTERNAL_SOURCE_PATH}
	${CMAKE_SOURCE_DIR}
	${CMAKE_SEGYSDK_SOURCE_PATH}/include
	${jsoncpp_INCLUDE_PATH}
)

target_compile_definitions(segysdk.auth_plugin.BasicFile
PRIVATE
	SEGYSDK_BUILDING_DLL
)

target_link_libraries(segysdk.auth_plugin.BasicFile
PRIVATE
    segysdk.auth_plugin
    segysdk
    ${JSON_LINK_LIBS}
    ${CMAKE_THREAD_LIBS_INIT}
)

