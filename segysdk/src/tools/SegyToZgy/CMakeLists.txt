include(import_cpprestsdk)
find_package(Threads)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/VersionInfo.h.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/VersionInfo.h"
)

add_executable(SegyToZgy
    SegyToZgy.cpp
    VersionInfo.h
)

add_subdirectory(library)
add_subdirectory(tests)

target_include_directories(SegyToZgy
        PRIVATE
        ${CMAKE_SEGYSDK_SOURCE_PATH}/include
        ${CMAKE_SEGYSDK_SOURCE_PATH}
        ${OPENZGY_EXPECTED_INCLUDE_DIR}
        ${jsoncpp_INCLUDE_PATH}
        ${vcpkg_INCLUDE_PATH}
        ${CMAKE_SOURCE_DIR}
)

target_link_libraries(SegyToZgy
    PRIVATE
        segytozgy.conversion
        segysdk
        SDAPI::SDAPI
        OpenZGY::OpenZGY
        ${CPPRESTSDK_LINK_LIBS}
        ${JSON_LINK_LIBS}
        ${CMAKE_THREAD_LIBS_INIT}
)
