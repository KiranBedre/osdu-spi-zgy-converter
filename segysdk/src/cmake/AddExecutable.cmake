function(AddExecutable ExecutableName)
    cmake_parse_arguments(ARG "" "" "SOURCE_FILES;HEADER_FILES;INCLUDE_DIRECTORIES;SYSTEM_INCLUDE_DIRECTORIES;PRIVATE_LINK_LIBRARIES;LINK_LIBRARIES" ${ARGN})
    add_executable(${ExecutableName} ${ARG_SOURCE_FILES})
    add_custom_target(${ExecutableName}-headers SOURCES ${ARG_HEADER_FILES})
    target_include_directories(${ExecutableName}
        PRIVATE
            ${ARG_INCLUDE_DIRECTORIES}
            ${CMAKE_SEGYSDK_SOURCE_PATH}
            ${CMAKE_SEGYSDK_SOURCE_PATH}/include
    )
    target_include_directories(${ExecutableName} SYSTEM
        PRIVATE
            ${ARG_SYSTEM_INCLUDE_DIRECTORIES}
    )
    target_link_libraries(${ExecutableName}
        PRIVATE
            ${ARG_PRIVATE_LINK_LIBRARIES}
    )
    target_link_libraries(${ExecutableName}
        PUBLIC
            ${ARG_LINK_LIBRARIES}
    )
    install(TARGETS ${ExecutableName} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endfunction()
