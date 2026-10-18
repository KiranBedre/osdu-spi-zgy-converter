project(ToolParams)

include(AddLibrary)

AddStaticLibrary(${PROJECT_NAME}
    SOURCE_FILES
        Params.cpp
    HEADER_FILES
        Params.h
)
