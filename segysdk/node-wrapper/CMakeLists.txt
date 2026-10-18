cmake_minimum_required(VERSION 3.9)
cmake_policy(SET CMP0042 NEW)
set (CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../src/cmake" ${CMAKE_MODULE_PATH})

project (segyApi)

include(import_json)

include_directories(
	${CMAKE_JS_INC}
	/src/segysdk/src
)

file(GLOB SOURCE_FILES 
	"src/segySessionNode.h" 
	"src/segySessionNode.cc" 
	"src/segyApi.cc"
)

add_library(segyApi SHARED 
	${SOURCE_FILES} 
	${CMAKE_JS_SRC}
)

set_target_properties(segyApi 
PROPERTIES 
	PREFIX "" 
	SUFFIX ".node"
)

target_link_libraries(segyApi 
	${CMAKE_JS_LIB} 
	/src/segysdk/src/out/bin/Release/libsegysdk.so
	${JSON_LINK_LIBS}
)

# Include Node-API wrappers
execute_process(COMMAND node -p "require('node-addon-api').include"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE NODE_ADDON_API_DIR
        )
string(REGEX REPLACE "[\r\n\"]" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})

target_include_directories(segyApi 
PRIVATE 
	${NODE_ADDON_API_DIR}	
	${jsoncpp_INCLUDE_PATH}
)

# define NAPI_VERSION
add_definitions(-DNAPI_VERSION=8)
