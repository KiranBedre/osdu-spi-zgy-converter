function(AddUnitTest TestName)
    cmake_parse_arguments(ARG "" "" "SOURCE_FILES;INCLUDE_DIRECTORIES;SYSTEM_INCLUDE_DIRECTORIES;LINK_LIBRARIES" ${ARGN})
    add_executable(${TestName} ${ARG_SOURCE_FILES})
    target_include_directories(${TestName}
        PRIVATE
            ${ARG_INCLUDE_DIRECTORIES}
            ${CMAKE_SEGYSDK_SOURCE_PATH}
    )
    target_include_directories(${TestName} SYSTEM
        PRIVATE
            ${ARG_SYSTEM_INCLUDE_DIRECTORIES}
    )
    target_link_libraries(${TestName}
        PRIVATE
            GTest::gtest_main
            ${ARG_LINK_LIBRARIES}
    )
    target_compile_definitions(${TestName}
        PRIVATE
            SEGYSDK_NO_DLL_PUBLIC
            TESTING
    )
    if(NOT MSVC)
        target_compile_options(${TestName}
            PRIVATE
                -O0
                -g
                --coverage
        )
        target_link_options(${TestName}
            PRIVATE
                --coverage
        )
    endif()
    add_test(NAME ${TestName} COMMAND ${TestName})
endfunction()
