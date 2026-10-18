cmake_minimum_required (VERSION 3.14)

include(${CMAKE_CURRENT_LIST_DIR}/cmake/version.cmake)

enable_testing()
project (SegySdkProject VERSION ${SEGYSDK_VERSION} LANGUAGES CXX)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake" ${CMAKE_MODULE_PATH})

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(CMAKE_GTEST_DISCOVER_TESTS_DISCOVERY_MODE PRE_TEST) # Workaround for a race condition - [probably] dependency DLLs are not copied to binaries directory at build time fast enough during the parallel build
# For Windows: Prevent overriding the parent project's compiler/linker settings
if(MSVC)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()
FetchContent_MakeAvailable(googletest)

include(segysdk_compiler_settings)

find_package(SDAPI REQUIRED)
find_package(OpenZGY REQUIRED)

if (MSVC)
    message(STATUS "segysdk root: Building on MSVC")
    if(DEFINED ENV{VCPKG_HOME})

        if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
            set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_HOME}/scripts/buildsystems/vcpkg.cmake")
        endif()
    else()
        message(FATAL_ERROR "VCPKG_HOME environment variable must be set on windows")
    endif()

    message("-- VCPKG_HOME:                 [$ENV{VCPKG_HOME}]")
    message("-- CMAKE_TOOLCHAIN_FILE:       [${CMAKE_TOOLCHAIN_FILE}]")
else()
    message(STATUS "segysdk root: Building on Linux")
endif()

include(CTest)
include(import_json)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    message("Building Debug config")
else()
    message("Building Release config")
endif()

message(STATUS  "segysdk root:  Architecture:             [${CMAKE_SYSTEM_NAME}]")

set(BUILD_SHARED_LIBS ON)
set(CMAKE_SEGYSDK_SOURCE_PATH ${CMAKE_CURRENT_LIST_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# Debug and Release output directories are defined here to work exactly the same with multi-config CMake generators
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/Debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/Debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib/Release)

configure_file(VersionInfo.rc.in VersionInfo.rc)

add_subdirectory(segysdk)
add_subdirectory(BulkAccess)
add_subdirectory(Concurrent)
add_subdirectory(AuthPlugin)

option(BUILD_AZURE_AUTH_PLUGIN "Build the Azure authentication plugin (.so)" OFF)
if(BUILD_AZURE_AUTH_PLUGIN)
    add_subdirectory(AzureAuthPlugin)
endif()

add_subdirectory(BasicFileAuthPlugin)
add_subdirectory(BinGridCalculationPlugin)
add_subdirectory(ComputePolygonPlugin)
add_subdirectory(NavigationExtractorPlugin)
add_subdirectory(StatisticsGeneratorPlugin)
add_subdirectory(FileIO)
add_subdirectory(Logger)
add_subdirectory(LibraryLoader)
add_subdirectory(SegyFormat)
add_subdirectory(SegyIndex)
add_subdirectory(SegyIO)
add_subdirectory(OsduMetaDataUtilities)
add_subdirectory(tests)
add_subdirectory(tools)
add_subdirectory(Utilities)
add_subdirectory(segysdk_details)
add_subdirectory(segysdk_public)
add_subdirectory(Request)
add_subdirectory(Telemetry)
add_subdirectory(Semaphore)
