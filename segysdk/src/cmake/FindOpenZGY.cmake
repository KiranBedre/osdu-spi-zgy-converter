find_package(SDAPI REQUIRED)

if(MSVC)
    set(NUGET_PACKAGES_LIB_DIR ${CMAKE_CURRENT_LIST_DIR}/../bin/${CMAKE_BUILD_TYPE})  # packages copied by install_packages.ps1
    set(OPENZGY_LIB_DIR ${NUGET_PACKAGES_LIB_DIR})
    set(OPENZGY_EXPECTED_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/../include )
else()
    set(OPENZGY_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../openzgy")
    if(EXISTS ${OPENZGY_ROOT})
        set(OPENZGY_DIR ${OPENZGY_ROOT}/build/deploy/native/)
        execute_process(COMMAND bash "-c" "${OPENZGY_ROOT}/scripts/getdistro" OUTPUT_VARIABLE _tmp)
        string(REGEX REPLACE "[ \t\r\n].*$" "" OPENZGY_DISTRO_NAME ${_tmp})
        set(OPENZGY_LIB_DIR ${OPENZGY_DIR}/${OPENZGY_DISTRO_NAME}-gcc94) # TODO this is totally not cross-platform
        set(OPENZGY_EXPECTED_INCLUDE_DIR "${OPENZGY_ROOT}/build/deploy/native/include")
    else() # default include and lib directories
        set(OPENZGY_LIB_DIR ${CMAKE_CURRENT_LIST_DIR}/../lib CACHE STRING "path to openzgy libraries")
        set(OPENZGY_EXPECTED_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/../include CACHE STRING "path to openzgy headers")
    endif()
endif()

set(OPENZGY_DLIB ${CMAKE_SHARED_LIBRARY_PREFIX}openzgy${CMAKE_SHARED_LIBRARY_SUFFIX})

find_path(OPENZGY_INCLUDE_DIR NAMES openzgy/api.h REQUIRED PATHS ${OPENZGY_EXPECTED_INCLUDE_DIR})
find_library(OPENZGY_LIBRARY NAMES openzgy PATHS ${OPENZGY_LIB_DIR})
find_file(OPENZGY_DLIBRARY NAMES ${OPENZGY_DLIB} PATHS ${OPENZGY_LIB_DIR})
mark_as_advanced(OPENZGY_INCLUDE_DIR OPENZGY_LIBRARY OPENZGY_DLIBRARY)

message(STATUS "OpenZGY include dir    ${OPENZGY_INCLUDE_DIR}")
message(STATUS "OpenZGY lib dir        ${OPENZGY_LIB_DIR}")
message(STATUS "OpenZGY shared library ${OPENZGY_DLIBRARY}")
message(STATUS "OpenZGY import library ${OPENZGY_LIBRARY}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    OpenZGY
    REQUIRED_VARS OPENZGY_INCLUDE_DIR OPENZGY_DLIBRARY OPENZGY_LIBRARY
)

if(OPENZGY_FOUND AND NOT TARGET OpenZGY::OpenZGY)
    if(OPENZGY_DLIBRARY)
        add_library(OpenZGY::OpenZGY SHARED IMPORTED)
        set_target_properties(OpenZGY::OpenZGY PROPERTIES
            IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
            INTERFACE_INCLUDE_DIRECTORIES "${OPENZGY_INCLUDE_DIR}"
        )
        target_link_libraries(OpenZGY::OpenZGY
        INTERFACE
            SDAPI::SDAPI
        )
        target_link_directories(OpenZGY::OpenZGY
        INTERFACE
            "${OPENZGY_LIB_DIR}"
        )
        if(MSVC)
            set_target_properties(OpenZGY::OpenZGY PROPERTIES
                IMPORTED_LOCATION "${OPENZGY_DLIBRARY}"
                IMPORTED_IMPLIB "${OPENZGY_LIBRARY}"
            )
        else()
            set_target_properties(OpenZGY::OpenZGY PROPERTIES
                IMPORTED_LOCATION "${OPENZGY_LIBRARY}"
            )
        endif()
    else()
        message(FATAL_ERROR "OpenZGY shared library was not found")
    endif()
endif()

function(openzgy_copy_runtime_dependencies_postbuild target)
# This is mostly needed for the development process if OpenZGY was linked with dynamic libraries.
# If you build the code, the runtime DLLs are not copied to the target directory
# and without these DLLs, it is impossible to run unit tests or debug the executables.
# It can be solved with install, but install is typically not part of the development workflow.
# So, adding a post build event seemed the least intrusive way.
# Sadly, the post build event must be set up manually for the project using OpenZGY.
if(MSVC)
    list(APPEND OPENZGY_THIRDPARTY_DLL_NAMES
        openzgy
        zfp
    )

    foreach(_tmp ${OPENZGY_THIRDPARTY_DLL_NAMES})
        list(APPEND OPENZGY_THIRDPARTY_DLLS ${OPENZGY_LIB_DIR}/${_tmp}.dll)
    endforeach()

    get_target_property(_tmp ${target} LIBRARY_OUTPUT_DIRECTORY)
    set(_tmp ${_tmp}/${CMAKE_BUILD_TYPE})
    message("Copying OpenZGY dependencies to [${_tmp}]")
    add_custom_command(
        TARGET ${target} POST_BUILD
        COMMAND_EXPAND_LISTS
        COMMAND ${CMAKE_COMMAND} -E make_directory "${_tmp}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OPENZGY_THIRDPARTY_DLLS} "${_tmp}"
    )
endif()
endfunction()
