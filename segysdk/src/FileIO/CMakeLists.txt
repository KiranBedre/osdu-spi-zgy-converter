add_library(segysdk.fileio STATIC
	BufferedFileReaderBase.h BufferedFileReaderBase.cpp
	BufferedFileReader.h BufferedFileReader.cpp
	CachedFileReader.h CachedFileReader.cpp
	CacheSizeOption.h CacheSizeOption.cpp
	DiskFileManager.h DiskFileManager.cpp
	DiskFileReader.h DiskFileReader.cpp
	DiskFileWriter.h DiskFileWriter.cpp
	FileIOException.h
	FileAccessParams.h FileAccessParams.cpp
	FileManager.h FileManager.cpp
	FileReader.h FileReader.cpp
	FileReaderOptions.h FileReaderOptions.cpp
	FileReferenceUtilities.h  FileReferenceUtilities.cpp
	FileReference.h FileReference.cpp
	FileWriter.h FileWriter.cpp
	GenericFileManager.h GenericFileManager.cpp
	GenericFileReader.h  GenericFileReader.cpp
	GenericFileReaderImpl.h   GenericFileReaderImpl.cpp
	GenericFileReaderFactory.h  GenericFileReaderFactory.cpp
	MultiBufferedFileReader.h MultiBufferedFileReader.cpp
	PagedFileReader.h PagedFileReader.cpp
	PipelineFileReaderBase.h PipelineFileReaderBase.cpp
	ReadCache.h ReadCache.cpp
	SDConnection.h SDConnection.cpp
	SDCredentials.h SDCredentials.cpp
	SDFileManager.h SDFileManager.cpp
	SDFileReader.h SDFileReader.cpp
	SDFileWriter.h SDFileWriter.cpp
	SequentialFileReader.h SequentialFileReader.cpp
	SectionFileReader.h SectionFileReader.cpp
	Utilities/ReaderUnit.h Utilities/ReaderUnit.cpp
	WriteMode.h
)

target_include_directories(segysdk.fileio
PRIVATE
	${CMAKE_SEGYSDK_SOURCE_PATH}/include
	${jsoncpp_INCLUDE_PATH}
	${CMAKE_SOURCE_DIR}
)

target_compile_definitions(segysdk.fileio
PRIVATE
	SEGYSDK_BUILDING_DLL
)

sdapi_copy_runtime_dependencies_postbuild(segysdk.fileio)

