project(segysdk.common)

add_library(${PROJECT_NAME} STATIC
	HeaderDataStruct.h HeaderDataStruct.cpp
	SegySession.h SegySession.cpp
	SegySessionFactory.h SegySessionFactory.cpp
	SegySessionImpl.h SegySessionImpl.cpp

	TraceNavigationData.h TraceNavigationData.cpp
	SegyServiceSession.h
	SegyServiceSessionImpl.h SegyServiceSessionImpl.cpp
	SegyServiceSessionFactory.h SegyServiceSessionFactory.cpp
)

configure_file(
	../SegySdkProject.h.in
	${CMAKE_SEGYSDK_SOURCE_PATH}/include/SegySdkProject.h
)

target_include_directories(${PROJECT_NAME}
PRIVATE
	${CMAKE_SEGYSDK_SOURCE_PATH}/include
	${CMAKE_SEGYSDK_SOURCE_PATH}
	${jsoncpp_INCLUDE_PATH}
	${CMAKE_SOURCE_DIR}
)

target_compile_definitions(${PROJECT_NAME}
PRIVATE
	SEGYSDK_BUILDING_DLL
)
