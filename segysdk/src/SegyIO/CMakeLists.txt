# Add SegyIO library
add_library(segysdk.segyio STATIC
	AsyncActivity.h  AsyncActivity.cpp
	BinaryHeader.h  BinaryHeader.cpp
	ConversionDetails.h
	DerivedHeader.h DerivedHeader.cpp
	EbcdicToAscii.h EbcdicToAscii.cpp
	AsciiToEbcdic.h AsciiToEbcdic.cpp
	FileHeaderDescription.h  FileHeaderDescription.cpp
	FileHeaderDefinitionFactory.h  FileHeaderDefinitionFactory.cpp
	HeaderDataType.h
	HeaderDefinition.h  HeaderDefinition.cpp
	HeaderDefinitionProvider.h  HeaderDefinitionProvider.cpp
	HeaderOverride.h HeaderOverride.cpp
	JsonFileUtilities.h JsonFileUtilities.cpp
	Primitives.h  Primitives.cpp
	SampleFormat.h
	ScalarUsageStrategy.h
	SegyDefinitionProvider.h SegyDefinitionProvider.cpp
	SegyFile.h  SegyFile.cpp
	SegyGleaner.h  SegyGleaner.cpp
	SegyTrace.h  SegyTrace.cpp
	TextualHeader.h  TextualHeader.cpp
	SegyTraceScanner.h  SegyTraceScanner.cpp
	TraceScannerBase.h TraceScannerBase.cpp
	TraceScanner.h  TraceScanner.cpp
	TraceSequentialScanner.h  TraceSequentialScanner.cpp
	TraceConstScanner.h TraceConstScanner.cpp
	TraceHeaderReader.h  TraceHeaderReader.cpp
	ValueConverter.h  ValueConverter.cpp
	VerticalDomain.h
	VerticalUOM.h
	SegyIOException.h

	ValueConversion/ByteConversion.h  ValueConversion/ByteConversion.cpp
	ValueConversion/ByteConverter.h  ValueConversion/ByteConverter.cpp
	ValueConversion/ByteValuePosition.h
	ValueConversion/HeaderValueArrayExtractor.h  ValueConversion/HeaderValueArrayExtractor.cpp
	ValueConversion/HeaderValueExtractor.h  ValueConversion/HeaderValueExtractor.cpp
	ValueConversion/SampleValueConverter.h  ValueConversion/SampleValueConverter.cpp
	ValueConversion/ScalarValueExtractor.h   ValueConversion/ScalarValueExtractor.cpp
	ValueConversion/ValueExtractor.h  ValueConversion/ValueExtractor.cpp
)

target_include_directories(segysdk.segyio
PRIVATE
	${CMAKE_SEGYSDK_SOURCE_PATH}
	${CMAKE_SEGYSDK_SOURCE_PATH}/include
	${jsoncpp_INCLUDE_PATH}
	${CMAKE_SOURCE_DIR}
)

target_compile_definitions(segysdk.segyio
PRIVATE
	SEGYSDK_BUILDING_DLL
)
