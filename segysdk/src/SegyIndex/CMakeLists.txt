add_library(segysdk.segyindex STATIC

	CollationOffset.h
	CompleteGridHandler.h CompleteGridHandler.cpp
	Coordinate.h Coordinate.cpp
	DataGrid.h
	FileLayoutSpecification.h FileLayoutSpecification.cpp
	Geometry3D.h Geometry3D.cpp
	GeometryCalculationData.h  GeometryCalculationData.cpp
	GridBuilderData.h
	GridCalculator.h  GridCalculator.cpp
	GridCell.h
	HorizontalExtent.h HorizontalExtent.cpp
	IndexBuilderCollatedData.h
	IndexBuilderData.h IndexBuilderData.cpp
	IndexBuilderDataManager.h  IndexBuilderDataManager.cpp
	IndexBuilderDataReader.h   IndexBuilderDataReader.cpp
	IndexBuilderDataWriter.h   IndexBuilderDataWriter.cpp
	IndexField.h IndexField.cpp
	JSONIndexStringConsts.h
	Range.h
	SeismicCubeExtents.h SeismicCubeExtents.cpp
	SegyIndex.h SegyIndex.cpp
	SegyIndexConfig.h
	SegyIndexData.h SegyIndexData.cpp
	SegyIndexFactory.h SegyIndexFactory.cpp
	SegyIndexGenerator.h SegyIndexGenerator.cpp
	SegyIndexImpl.h SegyIndexImpl.cpp
	SegyIndexMultiFileStruct.h
	SegyIndexStorage.h SegyIndexStorage.cpp
	SegyIndexTraceInfo.h SegyIndexTraceInfo.cpp
	StatsGenerator.h  StatsGenerator.cpp
	CubeExtent.h
	TraceExtent.h
	Vector.h   Vector.cpp
	VerticalExtent.h VerticalExtent.cpp
	Utilities/TraceRangeRecord.h
	Utilities/TraceRange.h Utilities/TraceRange.cpp
	Utilities/TraceIndexStorage.h Utilities/TraceIndexStorage.cpp
	Utilities/TraceIndexMap.h Utilities/TraceIndexMap.cpp
	Utilities/TraceRangeFactoryBase.h Utilities/TraceRangeFactoryBase.cpp
	Utilities/TraceRangeFactory.h Utilities/TraceRangeFactory.cpp
	Utilities/MissingTraceRangeFactory.h   Utilities/MissingTraceRangeFactory.cpp

	2DIndexing/SegyLine2D.h 2DIndexing/SegyLine2D.cpp
	2DIndexing/SeismicLineExtents.h 2DIndexing/SeismicLineExtents.cpp
	2DIndexing/IndexingTraceData2DInfo.h 2DIndexing/IndexingTraceData2DInfo.cpp
)

target_include_directories(segysdk.segyindex
PRIVATE
	${CMAKE_SEGYSDK_SOURCE_PATH}/include
	${jsoncpp_INCLUDE_PATH}
	${CMAKE_SOURCE_DIR}
)

target_compile_definitions(segysdk.segyindex
PRIVATE
	SEGYSDK_BUILDING_DLL
)
