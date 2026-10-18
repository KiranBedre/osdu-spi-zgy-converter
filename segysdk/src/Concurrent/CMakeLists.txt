add_library(segysdk.concurrent STATIC
	BufferQueue.cpp BufferQueue.h
	ThreadPool.cpp ThreadPool.h
)

target_include_directories(segysdk.concurrent
PRIVATE
	${CMAKE_SEGYSDK_SOURCE_PATH}/include
)

target_compile_definitions(segysdk.concurrent
PRIVATE
	SEGYSDK_BUILDING_DLL 
)
