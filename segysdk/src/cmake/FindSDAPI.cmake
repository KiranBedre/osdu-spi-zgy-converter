# TODO needs a prebuilt SDAPI at specified location - replace with a real package when available
if(MSVC)
    set(NUGET_PACKAGES_LIB_DIR ${CMAKE_CURRENT_LIST_DIR}/../bin/${CMAKE_BUILD_TYPE})  # packages copied by install_packages.ps1
    set(SDAPI_LIB_DIR ${NUGET_PACKAGES_LIB_DIR})
    set(SDAPI_EXPECTED_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/../include/SDAPI )
else()
    # TODO SDAPI binary can be found in the OpenZGY binary directory - this is weird but true
    set(OPENZGY_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../../openzgy")
    if(EXISTS ${OPENZGY_ROOT})
        set(SDAPI_DIR ${OPENZGY_ROOT}/build/deploy/native/)
        execute_process(COMMAND bash "-c" "${OPENZGY_ROOT}/scripts/getdistro" OUTPUT_VARIABLE _tmp)
        string(REGEX REPLACE "[ \t\r\n].*$" "" OPENZGY_DISTRO_NAME ${_tmp})
        set(SDAPI_LIB_DIR ${SDAPI_DIR}/${OPENZGY_DISTRO_NAME}-gcc94) # TODO this is totally not cross-platform
        set(SDAPI_EXPECTED_INCLUDE_DIR "${OPENZGY_ROOT}/build/temp/Seismic-Drive/include")
    else() # default include and lib directories
        set(SDAPI_LIB_DIR ${CMAKE_CURRENT_LIST_DIR}/../lib CACHE STRING "path to sdapi libraries")
        set(SDAPI_EXPECTED_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/../include/SDAPI CACHE STRING "path to sdapi headers")
    endif()
endif()

message(STATUS "FindSDAPI:  CMAKE_BUILD_TYPE    ${CMAKE_BUILD_TYPE}")
if(MSVC) # TODO :(
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(SDAPI_LIB sdapi_3_d)

    else()
        set(SDAPI_LIB sdapi_3)
    endif()
    set(SDAPI_DLIB ${SDAPI_LIB}${CMAKE_SHARED_LIBRARY_SUFFIX})
else()
    set(SDAPI_LIB sdapi)
    set(SDAPI_DLIB libsdapi.so.3)
endif()
message(STATUS "FindSDAPI:  SDAPI_LIB    ${SDAPI_LIB}")

find_path(SDAPI_INCLUDE_DIR NAMES SDManager.h REQUIRED PATHS ${SDAPI_EXPECTED_INCLUDE_DIR})
find_library(SDAPI_LIBRARY NAMES ${SDAPI_LIB} PATHS ${SDAPI_LIB_DIR})
find_file(SDAPI_DLIBRARY NAMES ${SDAPI_DLIB} PATHS ${SDAPI_LIB_DIR})
mark_as_advanced(SDAPI_INCLUDE_DIR SDAPI_LIBRARY SDAPI_DLIBRARY)

message(STATUS "SDAPI include dir    ${SDAPI_INCLUDE_DIR}")
message(STATUS "SDAPI lib dir        ${SDAPI_LIB_DIR}")
message(STATUS "SDAPI shared library ${SDAPI_DLIBRARY}")
message(STATUS "SDAPI import library ${SDAPI_LIBRARY}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    SDAPI
    REQUIRED_VARS SDAPI_INCLUDE_DIR SDAPI_DLIBRARY SDAPI_LIBRARY
)

if(SDAPI_FOUND AND NOT TARGET SDAPI::SDAPI)
    if(SDAPI_DLIBRARY)
        add_library(SDAPI::SDAPI SHARED IMPORTED)
        set_target_properties(SDAPI::SDAPI PROPERTIES
            IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
            INTERFACE_INCLUDE_DIRECTORIES "${SDAPI_INCLUDE_DIR}"
        )
        target_link_directories(SDAPI::SDAPI
        INTERFACE
            "${SDAPI_LIB_DIR}"
        )
        if(MSVC)
            set_target_properties(SDAPI::SDAPI PROPERTIES
                IMPORTED_LOCATION "${SDAPI_DLIBRARY}"
                IMPORTED_IMPLIB "${SDAPI_LIBRARY}"
            )
        else()
            set_target_properties(SDAPI::SDAPI PROPERTIES
                IMPORTED_LOCATION "${SDAPI_LIBRARY}"
            )
        endif()
    else()
        message(FATAL_ERROR "SDAPI shared library was not found")
    endif()
endif()

function(sdapi_copy_runtime_dependencies_postbuild target)
# This is mostly needed for the development process if SDAPI was linked with dynamic libraries.
# If you build the code, the runtime DLLs are not copied to the target directory
# and without these DLLs, it is impossible to run unit tests or debug the executables.
# It can be solved with install, but install is typically not part of the development workflow.
# So, adding a post build event seemed the least intrusive way.
# Sadly, the post build event must be set up manually for the project using SDAPI.
if(MSVC)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        list(APPEND SDAPI_THIRDPARTY_DLL_NAMES
            libcurl-d
            lzmad
            zlibd1
        )
    else()
        list(APPEND SDAPI_THIRDPARTY_DLL_NAMES
            libcurl
            lzma
            zlib1
        )
    endif()

    list(APPEND SDAPI_THIRDPARTY_DLL_NAMES
        azure-core
        azure-storage-blobs
        azure-storage-common
        iconv-2
        libxml2
        ${SDAPI_LIB}
    )

    foreach(_tmp ${SDAPI_THIRDPARTY_DLL_NAMES})
        list(APPEND SDAPI_THIRDPARTY_DLLS ${SDAPI_LIB_DIR}/${_tmp}.dll)
    endforeach()

    get_target_property(_tmp ${target} LIBRARY_OUTPUT_DIRECTORY)
    set(_tmp ${_tmp}/${CMAKE_BUILD_TYPE})
    message("Copying SDAPI dependencies to [${_tmp}]")
    add_custom_command(
        TARGET ${target} POST_BUILD
        COMMAND_EXPAND_LISTS
        COMMAND ${CMAKE_COMMAND} -E make_directory "${_tmp}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SDAPI_THIRDPARTY_DLLS} "${_tmp}"
    )
endif()
endfunction()
