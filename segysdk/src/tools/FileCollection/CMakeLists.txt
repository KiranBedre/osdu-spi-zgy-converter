project(ToolFileCollection)

include(AddLibrary)

AddStaticLibrary(${PROJECT_NAME}
    SOURCE_FILES
        FileCollection.cpp
    HEADER_FILES
        FileCollection.h
)
