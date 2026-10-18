add_library(segysdk.segyformat STATIC
	External/BinaryHeaderField.h External/BinaryHeaderField.cpp
	External/InterpretedBinaryHeader.h 
	External/InterpretedTraceHeder.h
	External/LoadContextFromDelfiJson.h External/LoadContextFromDelfiJson.cpp
	External/LoadContextFromJson.h External/LoadContextFromJson.cpp
	External/LoadContextFromOSDUJson.h External/LoadContextFromOSDUJson.cpp
	External/OsduLegalLiterals.h
	External/OsduOverrideByConstantValueLiterals.h
	External/OsduOverrideByFormulaLiterals.h
	External/OsduOverrideNodeLiterals.h
	External/OSDUReferenceData.h External/OSDUReferenceData.cpp
	External/OsduVectorHeaderMappingLiterals.h
	External/SegyBinaryHeaderOverrides.h 
	External/SegyBinaryHeaderStringConsts.h 
	External/SegyContext.h External/SegyContext.cpp
	External/SegyHeaderField.h External/SegyHeaderField.cpp 
	External/SegyOverrideField.h 
	External/SegyTraceHeaderStringConsts.h
	External/TraceHeaderField.h External/TraceHeaderField.cpp 
	External/UoMStringConsts.h

	Internal/FileHeaderDefinitionStringConstsBinaryHeader.h
	Internal/FileHeaderDefinitionStringConstsTraceHeader.h
)

target_include_directories(segysdk.segyformat 
PRIVATE
	${CMAKE_SEGYSDK_SOURCE_PATH}/include
	${jsoncpp_INCLUDE_PATH}
)

target_compile_definitions(segysdk.segyformat
PRIVATE
	SEGYSDK_BUILDING_DLL 
)
