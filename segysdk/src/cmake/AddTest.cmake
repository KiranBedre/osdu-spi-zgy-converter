function(AddTest TestSuiteName)
    set(multiValueArgs TEST_SOURCE_FILES EXTRA_INCLUDE_DIRECTORIES EXTRA_LINK_LIBRARIES GTEST_OPTIONS)
    set(options NOT_BASE_GTEST)
    cmake_parse_arguments(ARG "${options}" "" "${multiValueArgs}" ${ARGN})
    add_executable(${TestSuiteName} ${ARG_TEST_SOURCE_FILES})
    target_include_directories(${TestSuiteName}
        PRIVATE
            ${CMAKE_SEGYSDK_SOURCE_PATH}
            ${CMAKE_SEGYSDK_SOURCE_PATH}/include
            ${ARG_EXTRA_INCLUDE_DIRECTORIES}
    )
    target_link_libraries(${TestSuiteName}
        PRIVATE
            segysdk
            gtest_main
            ${ARG_EXTRA_LINK_LIBRARIES}
    )
    if (NOT ARG_NOT_BASE_GTEST OR INCLUDE_API_TESTS)
        gtest_discover_tests(${TestSuiteName} ${ARG_GTEST_OPTIONS})
    endif ()
endfunction()