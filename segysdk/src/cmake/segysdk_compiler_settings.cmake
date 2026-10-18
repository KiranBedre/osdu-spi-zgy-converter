set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (NOT CMAKE_BUILD_TYPE)
       set(CMAKE_BUILD_TYPE "Debug")
endif()

option(TREAT_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
message(STATUS "TREAT_WARNINGS_AS_ERRORS: ${TREAT_WARNINGS_AS_ERRORS}")

if(MSVC)
    string (REGEX REPLACE "/W[0-4] " " " CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    add_compile_options(/permissive- /Wall
        # disabled warnings:
        /wd4514 # 'function' : unreferenced inline function has been removed
        /wd4623 # 'derived class' : default constructor was implicitly defined as deleted because a base class default constructor is inaccessible or deleted
        /wd4625 # 'derived class' : copy constructor was implicitly defined as deleted because a base class copy constructor is inaccessible or deleted
        /wd4626 # 'derived class' : assignment operator was implicitly defined as deleted because a base class assignment operator is inaccessible or deleted
        /wd4668 # 'symbol' is not defined as a preprocessor macro, replacing with '0' for 'directive'
        /wd4710 # 'function' : function not inlined
        /wd4820 # 'bytes' bytes padding added after construct 'member_name'
        /wd5026 # 'type': move constructor was implicitly defined as deleted
        /wd5027 # 'type': move assignment operator was implicitly defined as deleted
        /wd5045 # Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
        /wd5220 # 'name': a non-static data member with a volatile qualified type no longer implies that compiler generated copy/move constructors and copy/move assignment operators are not trivial
        /wd4577 # 'noexcept' used with no exception handling mode specified; termination on exception is not guaranteed. Specify /EHsc
        /wd4251 # 'dll-interface' class needs to have dll-interface to be used by clients of struct ...
        /wd4464 #  relative include path contains '..'
    ) # TODO determine which warnings can be safely ignored
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    set(NUGET_PACKAGES_LIB_DIR ${PROJECT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})  # packages copied by install_packages.ps1
    link_directories(${NUGET_PACKAGES_LIB_DIR})
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    add_compile_options(-fPIC)
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-unknown-pragmas)
    if(TREAT_WARNINGS_AS_ERRORS)
        add_compile_options(-Werror)
    endif()
    if(IGNORE_WARNING_INFINITE_RECURSION)
        add_compile_options(-Wno-error=infinite-recursion)
    endif()

    if($ENV{SEGYTOZGY_COLLECT_CODE_COVERAGE})
        SET(CMAKE_CXX_FLAGS "-g -O0 -Wall -fprofile-arcs -ftest-coverage")
        SET(CMAKE_C_FLAGS "-g -O0 -Wall -W -fprofile-arcs -ftest-coverage")
        SET(CMAKE_EXE_LINKER_FLAGS "-fprofile-arcs -ftest-coverage")
    endif()
endif()

include(${CMAKE_CURRENT_LIST_DIR}/crossplatform.cmake)
