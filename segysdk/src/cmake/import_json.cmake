message(CHECK_START "Locating jsoncpp ... ")
if (MSVC)
    if(DEFINED ENV{VCPKG_HOME})
        if(NOT DEFINED JSONCPP_DIR)
            set(JSONCPP_DIR "$ENV{VCPKG_HOME}/installed/x64-windows")
            set(ENV{JSONCPP_DIR} "${JSONCPP_DIR}")
        endif()
    endif()
endif()

find_package(jsoncpp REQUIRED)
if(NOT DEFINED jsoncpp_FOUND)
    message(CHECK_FAIL "Unable to locate jsoncpp")
endif()

# To handle conflict with json.h both found in both products below: 
# /usr/include/jsoncpp/json/json.h
# /usr/include/cpprest/json.h
if (NOT DEFINED jsoncpp_INCLUDE_PATH)
    if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")  
    	set(jsoncpp_INCLUDE_PATH "/usr/include/jsoncpp/")
    else()
    	set(jsoncpp_INCLUDE_PATH "$ENV{VCPKG_HOME}/installed/x64-windows/include/")
    endif()
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
set (JSON_LINK_LIBS jsoncpp)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
set (JSON_LINK_LIBS JsonCpp::JsonCpp)
endif()

message(STATUS "json version:           jsoncpp_VERSION_MAJOR   [${jsoncpp_VERSION_MAJOR}]")
message(STATUS "json package version:   jsoncpp_VERSION_MINOR   [${jsoncpp_VERSION_MINOR}]")
message(STATUS "json include:           jsoncpp_INCLUDE_PATH   [${jsoncpp_INCLUDE_PATH}]")
message(STATUS "json libs:              JSON_LINK_LIBS   [${JSON_LINK_LIBS}]")
