function(AddLibrary LibraryName IsSharedLibrary)
    cmake_parse_arguments(ARG "NOT_BUILDING_DLL;INSTALL" "" "SOURCE_FILES;HEADER_FILES;INCLUDE_DIRECTORIES;SYSTEM_INCLUDE_DIRECTORIES;LINK_LIBRARIES;COMPILE_DEFINITIONS" ${ARGN})
    if(${IsSharedLibrary})
        add_library(${LibraryName} SHARED ${ARG_SOURCE_FILES})
    else()
        add_library(${LibraryName} STATIC ${ARG_SOURCE_FILES})
    endif()
    target_include_directories(${LibraryName}
        PRIVATE
            ${ARG_INCLUDE_DIRECTORIES}
            ${CMAKE_SEGYSDK_SOURCE_PATH}
            ${CMAKE_SEGYSDK_SOURCE_PATH}/include
    )
    target_include_directories(${LibraryName} SYSTEM
        PRIVATE
            ${ARG_SYSTEM_INCLUDE_DIRECTORIES}
    )
    target_link_libraries(${LibraryName}
        PRIVATE
            ${ARG_LINK_LIBRARIES}
    )
    target_compile_definitions(${LibraryName} PRIVATE ${ARG_COMPILE_DEFINITIONS})
    if(NOT ARG_NOT_BUILDING_DLL)
        target_compile_definitions(${LibraryName} PRIVATE SEGYSDK_BUILDING_DLL)
    endif()
    add_custom_target(${LibraryName}-headers SOURCES ${ARG_HEADER_FILES})
    if(ARG_INSTALL)
        install(TARGETS ${LibraryName}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        )
    endif()
endfunction()

function(AddStaticLibrary LibraryName)
    AddLibrary(${LibraryName} FALSE ${ARGN})
endfunction()

function(AddSharedLibrary LibraryName)
    AddLibrary(${LibraryName} TRUE ${ARGN})
endfunction()
