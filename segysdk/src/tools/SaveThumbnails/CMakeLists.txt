project(SaveThumbnails)

find_package(PNG REQUIRED)

include(AddExecutable)

AddExecutable(${PROJECT_NAME}
    SOURCE_FILES
        Main.cpp
    SYSTEM_INCLUDE_DIRECTORIES
        ${jsoncpp_INCLUDE_PATH}
    LINK_LIBRARIES
        ThumbnailComponents
        ToolLogger
        ToolOptions
)

include(AddLibrary)

AddStaticLibrary(ThumbnailComponents
    SOURCE_FILES
        BitmapCreator.cpp
        Dispatcher.cpp
        Drive.cpp
        DriveConnector.cpp
        DriveConnectorLive.cpp
        DriveSegy.cpp
        DriveZgy.cpp
        PngCreator.cpp
        Types.cpp
    HEADER_FILES
        BitmapCreator.h
        Dispatcher.h
        Drive.h
        DriveConnector.h
        DriveConnectorLive.h
        DriveSegy.h
        DriveZgy.h
        PngCreator.h
        Types.h
    SYSTEM_INCLUDE_DIRECTORIES
        ${jsoncpp_INCLUDE_PATH}
    LINK_LIBRARIES
        ${JSON_LINK_LIBS}
        OpenZGY::OpenZGY
        PNG::PNG
        ToolError
        ToolFileCollection
        ToolParams
        ToolSegySession
        ToolStorageClient
        ToolStorageConnector
        segysdk
        segysdk.OsduMetadataUtilities
)
