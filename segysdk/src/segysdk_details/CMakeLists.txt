add_library(segysdk.details STATIC
	JSONNumericalFormatStrings.h
	JSONScalingEnums.h
	JSONTraceHeaderDefStrings.h
	SegyException.h SegyException.cpp
	DLL_EXPORT.h
)

target_include_directories(segysdk.details
PRIVATE
	${CMAKE_SEGYSDK_SOURCE_PATH}/include
	${jsoncpp_INCLUDE_PATH}
)

target_compile_definitions(segysdk.details
PRIVATE
	SEGYSDK_BUILDING_DLL 
)
