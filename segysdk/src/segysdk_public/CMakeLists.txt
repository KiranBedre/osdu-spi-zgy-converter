include(import_cpprestsdk)
find_package (Threads REQUIRED)

add_library(segysdk SHARED
	api.h
	segy_c_api.h segy_c_api.cpp
	internal_api.h

	${PROJECT_BINARY_DIR}/VersionInfo.rc
)

add_library(segysdk::segysdk ALIAS segysdk)

target_include_directories(segysdk
PRIVATE
	${CMAKE_SEGYSDK_SOURCE_PATH}
	${CMAKE_SEGYSDK_SOURCE_PATH}/include
	${jsoncpp_INCLUDE_PATH}
	${vcpkg_INCLUDE_PATH}
	${CMAKE_SOURCE_DIR}
)

target_link_libraries(segysdk
PRIVATE
	segysdk.common
	segysdk.bulkaccess
	-Wl,-whole-archive
	segysdk.bulkaccess.whole
	-Wl,-no-whole-archive
	segysdk.segyindex
	segysdk.segyio
	segysdk.segyformat
	segysdk.fileio
	segysdk.concurrent
	segysdk.auth_plugin
	segysdk.logger
	segysdk.library_loader
	segysdk.utilities
	segysdk.details
	segysdk.compute_polygon_plugin
	segysdk.navigation_extractor_plugin
	segysdk.OsduMetadataUtilities
	segysdk.statistics_generator_plugin
	segysdk.bin_grid_calculation_plugin
	segysdk.request
	segysdk.telemetry
	segysdk.semaphore
	SDAPI::SDAPI
	${JSON_LINK_LIBS}
	${CPPRESTSDK_LINK_LIBS}
	${CMAKE_THREAD_LIBS_INIT}
)

if(WIN32)
target_link_libraries(segysdk
PUBLIC
	Rpcrt4.lib
)
elseif(UNIX)
target_link_libraries(segysdk
PUBLIC
	uuid
)
endif()

target_compile_definitions(segysdk
PRIVATE
	SEGYSDK_BUILDING_DLL
)

install(
TARGETS
	segysdk
	segysdk.common
	segysdk.bulkaccess
	segysdk.json_parser
	segysdk.segyindex
	segysdk.segyio
	segysdk.segyformat
	segysdk.fileio
	segysdk.concurrent
	segysdk.auth_plugin
	segysdk.logger
	segysdk.library_loader
	segysdk.utilities
	segysdk.details
	segysdk.compute_polygon_plugin
	segysdk.navigation_extractor_plugin
	segysdk.OsduMetadataUtilities
	segysdk.statistics_generator_plugin
	segysdk.bin_grid_calculation_plugin

	EXPORT segysdkTargets
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include
)

install(
	EXPORT segysdkTargets
	FILE segysdkTargets.cmake
	NAMESPACE segysdk::
	DESTINATION lib/cmake/segysdk
)

# set(LIBRARY_INSTALL_DIR lib)
set(INCLUDE_INSTALL_DIR include)
set(LIB_INSTALL_DIR lib)

include(CMakePackageConfigHelpers)

set(cmake_package_name segysdk)
set(targets_export_name ${cmake_package_name}Targets CACHE INTERNAL "")
set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated" CACHE INTERNAL "")
set(cmake_files_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${cmake_package_name}")
set(version_file "${generated_dir}/${cmake_package_name}ConfigVersion.cmake")

configure_package_config_file(
	segysdkConfig.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/segysdkConfig.cmake
	INSTALL_DESTINATION ${cmake_files_install_dir}
	PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR
)

write_basic_package_version_file(
	segysdkConfigVersion.cmake
	VERSION ${PACKAGE_VERSION}
	COMPATIBILITY AnyNewerVersion
)

install(
	FILES ${CMAKE_CURRENT_BINARY_DIR}/segysdkConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/segysdkConfigVersion.cmake
	DESTINATION ${cmake_files_install_dir}
)
