message(STATUS "segysdk.auth_plugin.Azure include dir    ${CMAKE_SEGY_INTERNAL_SOURCE_PATH}")
message(STATUS "segysdk.auth_plugin.Azure include dir    ${CMAKE_SEGYSDK_SOURCE_PATH}")

find_package(azure-identity-cpp CONFIG REQUIRED)

add_library(segysdk.auth_plugin.Azure SHARED
	AzureAuthPluginImpl.h AzureAuthPluginImpl.cpp
	AzureAuthPluginTemplate.cpp
)

target_include_directories(segysdk.auth_plugin.Azure
PRIVATE
	${CMAKE_SEGY_INTERNAL_SOURCE_PATH}
	${CMAKE_SOURCE_DIR}
	${CMAKE_SEGYSDK_SOURCE_PATH}/include
)

target_compile_definitions(segysdk.auth_plugin.Azure
PRIVATE
	SEGYSDK_BUILDING_DLL
)

target_link_libraries(segysdk.auth_plugin.Azure
PRIVATE
    segysdk.auth_plugin
    segysdk
    Azure::azure-identity
    ${CMAKE_THREAD_LIBS_INIT}
)
