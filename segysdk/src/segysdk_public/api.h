/*
Copyright 2021-2022 Schlumberger

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

/** @file
 * @brief  api.h is meant to be included as the main include file for the SEG-Y SDK. As an end-user of the library, you should only include this file.
 *
 */

#pragma once

#include "../AuthPlugin/AuthPluginFactory.h"
#include "../BulkAccess/SegyBulkReader.h"
#include "../BulkAccess/Segy2DReader.h"
#include "../ComputePolygonPlugin/ComputePolygonPlugin.h"
#include "../Concurrent/BufferQueue.h"
#include "../Concurrent/ThreadPool.h"
#include "../FileIO/CacheSizeOption.h"
#include "../FileIO/FileReader.h"
#include "../FileIO/FileReferenceUtilities.h"
#include "../FileIO/GenericFileManager.h"
#include "../FileIO/GenericFileReaderFactory.h"
#include "../FileIO/SDCredentials.h"
#include "../FileIO/SequentialFileReader.h"
#include "../LibraryLoader/PluginConfig.h"
#include "../Logger/Logger.h"
#include "../Logger/LoggerSettings.h"
#include "../Logger/ProgressReporting.h"
#include "../NavigationExtractorPlugin/NavigationExtractorPlugin.h"
#include "../SegyFormat/External/LoadContextFromDelfiJson.h"
#include "../SegyFormat/External/LoadContextFromJson.h"
#include "../SegyFormat/External/LoadContextFromOSDUJson.h"
#include "../SegyFormat/External/SegyContext.h"
#include "../SegyIO/AsciiToEbcdic.h"
#include "../SegyIO/HeaderDefinitionProvider.h"
#include "../SegyIO/SampleFormat.h"
#include "../SegyIO/TraceSequentialScanner.h"
#include "../SegyIO/VerticalDomain.h"
#include "../SegyIO/VerticalUOM.h"
#include "../SegyIndex/HorizontalExtent.h"
#include "../SegyIndex/IndexField.h"
#include "../SegyIndex/SegyIndex.h"
#include "../SegyIndex/SegyIndexFactory.h"
#include "../SegyIndex/SeismicCubeExtents.h"
#include "../SegyIndex/Utilities/TraceIndexMap.h"
#include "../SegyIndex/Utilities/TraceIndexStorage.h"
#include "../SegyIndex/VerticalExtent.h"
#include "../Utilities/Adapters.h"
#include "../Utilities/BinGrid.h"
#include "../Utilities/BrickSize.h"
#include "../Utilities/Environment.h"
#include "../Utilities/GuidGenerator.h"
#include "../Utilities/InputParser.h"
#include "../Utilities/JsonBinGrid.h"
#include "../Utilities/Options.h"
#include "../Utilities/SafeTime.h"
#include "../Utilities/SessionUtilities.h"
#include "../Utilities/Settings.h"
#include "../Utilities/TokenSourceFactory.h"
#include "../Utilities/ZgyOutputParams.h"
#include "../Utilities/TokenSourceFactory.h"
#include "../Utilities/auth/TokenSource.h"
#include "../Utilities/osdu/OsduFieldNameLiterals.h"
#include "../Utilities/osdu/OsduFileCollectionSegy100.h"
#include "../Utilities/osdu/OsduObject.h"
#include "../Utilities/osdu/OsduSeismicTraceData140.h"
#include "../Utilities/osdu/OsduSeismicTraceDataFactory.h"
#include "../Utilities/osdu/OsduWorkProduct100.h"
#include "../Utilities/osdu/WellKnownTypes.h"
#include "../segysdk/SegySession.h"
#include "../segysdk/SegySessionFactory.h"
#include "../segysdk/SegyServiceSession.h"
#include "../segysdk/SegyServiceSessionFactory.h"
#include "../segysdk_details/SegyException.h"
#include "../OsduMetaDataUtilities/OsduMetafileApi.h"
