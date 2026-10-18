project(segysdk.semaphore)

include(AddLibrary)

AddStaticLibrary(${PROJECT_NAME}
    SOURCE_FILES
        Semaphore.cpp
    HEADER_FILES
        Semaphore.h
)

add_subdirectory(test)
