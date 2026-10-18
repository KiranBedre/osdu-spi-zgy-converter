find_package(OpenZGY REQUIRED)

add_library(segytozgy.conversion STATIC 
    BrickedConversion.h  BrickedConversion.cpp
    ConversionUnit.h 
    JsonLoader.h  JsonLoader.cpp
    OpenZGYFileProperties.h  OpenZGYFileProperties.cpp
    SegyHeaderIds.h
    SegyInputGeometry.h  SegyInputGeometry.cpp
    SegyInputParameters.h  SegyInputParameters.cpp
    ZgyMetaData.h  ZgyMetaData.cpp
    ZgyOptions.h  ZgyOptions.cpp
    ZgyOptionsDelfiCommandLine.h  ZgyOptionsDelfiCommandLine.cpp
    ZgyOptionsOsdu.h  ZgyOptionsOsdu.cpp
    crs/CoordinateReferenceSystemResolver.hpp  crs/CoordinateReferenceSystemResolver.cpp
    crs/PersistableReferenceCrs.h  crs/PersistableReferenceCrs.cpp
    crs/WktCoordinateSystem.h  crs/WktCoordinateSystem.cpp
    osdu/OsduFileCollectionOpenZGY100.h  osdu/OsduFileCollectionOpenZGY100.cpp

)

target_include_directories(segytozgy.conversion 
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
    ${jsoncpp_INCLUDE_PATH}
    ${vcpkg_INCLUDE_PATH}
)

openzgy_copy_runtime_dependencies_postbuild(segytozgy.conversion)

if(WIN32)
target_link_libraries(segytozgy.conversion 
PUBLIC 
    Rpcrt4.lib 
)
elseif(UNIX)
target_link_libraries(segytozgy.conversion 
PUBLIC 
    uuid 
)
endif()
