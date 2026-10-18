message(STATUS "segysdk.OsduMetadataUtilities CMAKE_SEGYSDK_SOURCE_PATH           [${CMAKE_SEGYSDK_SOURCE_PATH}]")
message(STATUS "segysdk.OsduMetadataUtilities jsoncpp_INCLUDE_PATH                [${jsoncpp_INCLUDE_PATH}]")

add_library(segysdk.OsduMetadataUtilities STATIC

    OsduMetafileApi.h
    OsduPersistableMetadataBase.h       OsduPersistableMetadataBase.cpp
    OsduMetadataBase.h                  OsduMetadataBase.cpp
    OsduMetadataStructBase.h            OsduMetadataStructBase.cpp
    OsduMetadataGeoJsonGeometryElementStructBase.h   OsduMetadataGeoJsonGeometryElementStructBase.cpp

    # Test: MetadataHelperTests.cpp
    OsduMetadataHelper.h                OsduMetadataHelper.cpp

    # Test: OsduAbstractCommonResourcesMetadataTests.cpp
    OsduAbstractCommonResourcesLiterals.h
    OsduAbstractCommonResources.h       OsduAbstractCommonResources.cpp
    OsduAbstractCommonResourcesStruct.h OsduAbstractCommonResourcesStruct.cpp

    # Test: osduHistogramtests.cpp (ToDo: Work in progress)
    OsduAbstractContentLiterals.h
    OsduAbstractContent.h               OsduAbstractContent.cpp
    OsduAbstractContentStruct.h         OsduAbstractContentStruct.cpp

    # Test: OsduAbstractDatasetTests.cpp (ToDo: Work in progress)
    OsduAbstractDatasetLiterals.h
    OsduAbstractDataset.h               OsduAbstractDataset.cpp
    OsduAbstractDatasetStruct.h         OsduAbstractDatasetStruct.cpp

    # Test: NA
    OsduAbstractFileLiterals.h
    OsduAbstractFile.h                  OsduAbstractFile.cpp
    OsduAbstractFileStruct.h            OsduAbstractFileStruct.cpp

    # Test: OsduAbstractFileSourceInfoTests.cpp
    OsduAbstractFileSourceInfoLiterals.h
    OsduAbstractFileSourceInfoStruct.h      OsduAbstractFileSourceInfoStruct.cpp
    OsduAbstractFileSourceInfo.h            OsduAbstractFileSourceInfo.cpp

    # Test: OsduAbstractFileSourceInfoCollectionTests.cpp
    OsduAbstractFileCollectionLiterals.h
    OsduAbstractFileCollectionStruct.h      OsduAbstractFileCollectionStruct.cpp
    OsduAbstractFileCollection.h            OsduAbstractFileCollection.cpp

    # Test: OsduAclMetadataTests.cpp
    OsduAclLiterals.h
    OsduAcl.h                               OsduAcl.cpp
    OsduAclStruct.h                         OsduAclStruct.cpp

    # Test: OsduAncestryMetadataTests.cpp
    OsduAncestryLiterals.h
    OsduAncestry.h                          OsduAncestry.cpp
    OsduAncestryStruct.h                    OsduAncestryStruct.cpp

    # Test: NA (Will be tested when we have a case for it)
    OsduExtensionPropertiesLiterals.h
    OsduExtensionPropertiesStruct.h         OsduExtensionPropertiesStruct.cpp
    OsduExtensionProperties.h               OsduExtensionProperties.cpp

    OsduExtensionPropertyArray.h            OsduExtensionPropertyArray.cpp
    OsduExtensionPropertyArrayStruct.h      OsduExtensionPropertyArrayStruct.cpp

    # Test: OsduSeismicHistogramTests
    OsduGenericFileDataLiterals.h
    OsduGenericFileData.h                   OsduGenericFileData.cpp
    OsduGenericFileDataStruct.h             OsduGenericFileDataStruct.cpp

    # Test: OsduFileCollectionSegyTests
    OsduGenericFileCollectionDataLiterals.h
    OsduGenericFileCollectionData.h         OsduGenericFileCollectionData.cpp
    OsduGenericFileCollectionDataStruct.h   OsduGenericFileCollectionDataStruct.cpp

    # Test: NA
    OsduGenericFileLiterals.h
    OsduGenericFile.h                       OsduGenericFile.cpp
    OsduGenericFileStruct.h                 OsduGenericFileStruct.cpp

    # Test: osduFileCollectionSegyTests
    OsduGenericFileCollectionLiterals.h
    OsduGenericFileCollection.h             OsduGenericFileCollection.cpp
    OsduGenericFileCollectionStruct.h       OsduGenericFileCollectionStruct.cpp

    # Test: OsduLegalMetadataTests.cpp
    OsduLegalLiterals.h
    OsduLegal.h                             OsduLegal.cpp
    OsduLegalStruct.h                       OsduLegalStruct.cpp

    # Test: NA
    OsduMetaArray.h                         OsduMetaArray.cpp
    OsduMetaArrayStruct.h                   OsduMetaArrayStruct.cpp

     # Test: NA
    OsduMetaElement.h                       OsduMetaElement.cpp
    OsduMetaElementStruct.h                 OsduMetaElementStruct.cpp
    OsduMetaLiterals.h

     # Test: NA
    OsduTagArray.h                          OsduTagArray.cpp
    OsduTagArrayStruct.h                    OsduTagArrayStruct.cpp

     # Test: NA
    OsduTagElement.h                        OsduTagElement.cpp
    OsduTagElementStruct.h                  OsduTagElementStruct.cpp
    OsduTagLiterals.h

    # Test: OsduSeismicHistogramTests.cpp
    OsduFileHistogramLiterals.h
    OsduFileHistogram.h                     OsduFileHistogram.cpp
    OsduFileHistogramStruct.h               OsduFileHistogramStruct.cpp

    # Test: OsduSeismicHistogramTests.cpp
    OsduFileHistogramDataLiterals.h
    OsduFileHistogramData.h                 OsduFileHistogramData.cpp
    OsduFileHistogramDataStruct.h           OsduFileHistogramDataStruct.cpp

    # Test: OsduSeismicHistogramTests.cpp
    OsduContentSeismicHistogramLiterals.h
    OsduContentSeismicHistogram.h           OsduContentSeismicHistogram.cpp
    OsduContentSeismicHistogramStruct.h     OsduContentSeismicHistogramStruct.cpp

    # Test: OsduSeismicHistogramTests.cpp
    OsduHistogramLiterals.h
    OsduHistogram.h                         OsduHistogram.cpp
    OsduHistogramStruct.h                   OsduHistogramStruct.cpp

    # Test: OsduVectorHeaderMappingTests
    OsduVectorHeaderMappingLiterals.h
    OsduVectorHeaderMapping.h               OsduVectorHeaderMapping.cpp
    OsduVectorHeaderMappingStruct.h         OsduVectorHeaderMappingStruct.cpp

    OsduVectorHeaderMappingElement.h        OsduVectorHeaderMappingElement.cpp
    OsduVectorHeaderMappingElementStruct.h  OsduVectorHeaderMappingElementStruct.cpp

    # Test: OsduVectorHeaderMappingTests
    OsduOverridesBinaryHeaderLiterals.h
    OsduOverridesBinaryHeader.h             OsduOverridesBinaryHeader.cpp
    OsduOverridesBinaryHeaderStruct.h       OsduOverridesBinaryHeaderStruct.cpp

    # Test: OsduVectorHeaderMappingTests
    OsduOverridesTraceHeaderLiterals.h
    OsduOverridesTraceHeaderElement.h       OsduOverridesTraceHeaderElement.cpp
    OsduOverridesTraceHeaderElementStruct.h OsduOverridesTraceHeaderElementStruct.cpp
    OsduOverridesTraceHeader.h              OsduOverridesTraceHeader.cpp
    OsduOverridesTraceHeaderStruct.h        OsduOverridesTraceHeaderStruct.cpp

    # Test: OsduVectorHeaderMappingTests
    OsduOverrideByConstantValueLiterals.h
    OsduOverrideByConstantValue.h           OsduOverrideByConstantValue.cpp
    OsduOverrideByConstantValueStruct.h     OsduOverrideByConstantValueStruct.cpp

    # Test: OsduVectorHeaderMappingTests
    OsduOverrideByFormulaLiterals.h
    OsduOverrideByFormula.h                 OsduOverrideByFormula.cpp
    OsduOverrideByFormulaStruct.h           OsduOverrideByFormulaStruct.cpp


    # Test: OsduFileCollectionSegyTests
    OsduFileCollectionSegyLiterals.h
    OsduFileCollectionSegy.h                OsduFileCollectionSegy.cpp
    OsduFileCollectionSegyStruct.h          OsduFileCollectionSegyStruct.cpp

    # Test: OsduFileCollectionSegyTests


    OsduFileCollectionSegyData.h            OsduFileCollectionSegyData.cpp
    OsduFileCollectionSegyDataStruct.h      OsduFileCollectionSegyDataStruct.cpp

    # Test: OsduFileCollectionSegyTests.cpp
    OsduAbstractSeismicPrecisionLiterals.h
    OsduAbstractSeismicPrecision.h          OsduAbstractSeismicPrecision.cpp
    OsduAbstractSeismicPrecisionStruct.h    OsduAbstractSeismicPrecisionStruct.cpp

    # Test: OsduFileCollectionOpenZgyTests
    OsduFileCollectionOpenZgyLiterals.h
    OsduFileCollectionOpenZgy.h                OsduFileCollectionOpenZgy.cpp
    OsduFileCollectionOpenZgyStruct.h          OsduFileCollectionOpenZgyStruct.cpp

    # Test: OsduFileCollectionOpenZgyTests
    OsduFileCollectionOpenZgyDataLiterals.h
    OsduFileCollectionOpenZgyData.h            OsduFileCollectionOpenZgyData.cpp
    OsduFileCollectionOpenZgyDataStruct.h      OsduFileCollectionOpenZgyDataStruct.cpp

    # Test: OsduFileCollectionOpenZgyTests
    OsduAbstractCompressionInfoLiterals.h
    OsduAbstractCompressionInfo.h               OsduAbstractCompressionInfo.cpp
    OsduAbstractCompressionInfoStruct.h         OsduAbstractCompressionInfoStruct.cpp

    # Test: OsduSeismicTraceDataTests
    OsduSeismicTraceDataLiterals.h
    OsduSeismicTraceData.h                  OsduSeismicTraceData.cpp
    OsduSeismicTraceDataStruct.h            OsduSeismicTraceDataStruct.cpp

    # Test: OsduSeismicTraceDataTests
    OsduSeismicTraceDataSpecificPropertiesLiterals.h
    OsduSeismicTraceDataSpecificProperties.h            OsduSeismicTraceDataSpecificProperties.cpp
    OsduSeismicTraceDataSpecificPropertiesStruct.h      OsduSeismicTraceDataSpecificPropertiesStruct.cpp

    # Test: OsduSeismicTraceDataTests
    OsduAnyCrsGeoJsonGeometryElementLiterals.h
    OsduAnyCrsGeoJsonGeometryElement.h                  OsduAnyCrsGeoJsonGeometryElement.cpp
    OsduAnyCrsGeoJsonGeometryElementStruct.h            OsduAnyCrsGeoJsonGeometryElementStruct.cpp

    # Test: OsduSeismicTraceDataTests
    OsduAnyCrsGeoJsonFeatureLiterals.h
    OsduAnyCrsGeoJsonFeature.h                          OsduAnyCrsGeoJsonFeature.cpp
    OsduAnyCrsGeoJsonFeatureStruct.h                    OsduAnyCrsGeoJsonFeatureStruct.cpp

    # Test: OsduSeismicTraceDataTests
    OsduGeoJsonPointPropertiesElementLiterals.h
    OsduGeoJsonPointPropertiesElement.h                 OsduGeoJsonPointPropertiesElement.cpp
    OsduGeoJsonPointPropertiesElementStruct.h           OsduGeoJsonPointPropertiesElementStruct.cpp

    OsduGeoJsonLabelPropertiesElement.h                 OsduGeoJsonLabelPropertiesElement.cpp
    OsduGeoJsonLabelPropertiesElementStruct.h           OsduGeoJsonLabelPropertiesElementStruct.cpp

    OsduGeoJsonFeaturePropertiesElementLiterals.h
    OsduGeoJsonFeaturePropertiesElement.h              OsduGeoJsonFeaturePropertiesElement.cpp
    OsduGeoJsonFeaturePropertiesElementStruct.h        OsduGeoJsonFeaturePropertiesElementStruct.cpp

    OsduGeoJson2DPointsPropertiesElementLiterals.h
    OsduGeoJson2DPointsPropertiesElement.h             OsduGeoJson2DPointsPropertiesElement.cpp
    OsduGeoJson2DPointsPropertiesElementStruct.h       OsduGeoJson2DPointsPropertiesElementStruct.cpp

    # Test: OsduSeismicTraceDataTests
    OsduAbstractWorkProductComponentLiterals.h
    OsduAbstractWorkProductComponent.h                  OsduAbstractWorkProductComponent.cpp
    OsduAbstractWorkProductComponentStruct.h            OsduAbstractWorkProductComponentStruct.cpp

    # Test: OsduSeismicTraceDataTests
    OsduStackRangesLiterals.h
    OsduStackRanges.h                                   OsduStackRanges.cpp
    OsduStackRangesStruct.h                             OsduStackRangesStruct.cpp

    # Test: OsduSeismicTraceDataTests
    OsduTimeLapseLiterals.h
    OsduTimeLapse.h                                     OsduTimeLapse.cpp
    OsduTimeLapseStruct.h                               OsduTimeLapseStruct.cpp

    # Test: OsduSeismicTraceDataTests
    OsduProcessingParametersLiterals.h
    OsduProcessingParameters.h                          OsduProcessingParameters.cpp
    OsduProcessingParametersStruct.h                    OsduProcessingParametersStruct.cpp

    # Test: OsduSeismicTraceDataTests
    OsduLineageAssertionLiterals.h
    OsduLineageAssertion.h                              OsduLineageAssertion.cpp
    OsduLineageAssertionStruct.h                        OsduLineageAssertionStruct.cpp

    # Test: OsduSeismicTraceDataTests
    OsduAbstractSpatialLocationLiterals.h
    OsduAbstractSpatialLocation.h                       OsduAbstractSpatialLocation.cpp
    OsduAbstractSpatialLocationStruct.h                 OsduAbstractSpatialLocationStruct.cpp

    # Test: OsduSeismicTraceDataTests
    OsduAbstractAnyCrsFeatureCollectionLiterals.h
    OsduAbstractAnyCrsFeatureCollection.h               OsduAbstractAnyCrsFeatureCollection.cpp
    OsduAbstractAnyCrsFeatureCollectionStruct.h         OsduAbstractAnyCrsFeatureCollectionStruct.cpp

    # Test: OsduSeismicTraceDataTests
    OsduAbstractFeatureCollectionLiterals.h
    OsduAbstractFeatureCollection.h                     OsduAbstractFeatureCollection.cpp
    OsduAbstractFeatureCollectionStruct.h               OsduAbstractFeatureCollectionStruct.cpp

    # Test: OsduSeismicTraceDataTests
    OsduGeoJsonFeatureLiterals.h
    OsduGeoJsonFeature.h                                OsduGeoJsonFeature.cpp
    OsduGeoJsonFeatureStruct.h                          OsduGeoJsonFeatureStruct.cpp

    # Test: OsduSeismicTraceDataTests
    OsduGeoJsonFeatureElementLiterals.h
    OsduGeoJsonFeatureElement.h                         OsduGeoJsonFeatureElement.cpp
    OsduGeoJsonFeatureElementStruct.h                   OsduGeoJsonFeatureElementStruct.cpp


    # Test: OsduSeismicTraceDataTests
    OsduGeoJsonFeaturePropertiesLiterals.h
    OsduGeoJsonFeatureProperties.h                      OsduGeoJsonFeatureProperties.cpp
    OsduGeoJsonFeaturePropertiesStruct.h                OsduGeoJsonFeaturePropertiesStruct.cpp

    # Test: OsduSeismicTraceDataTests
    OsduGeoJsonGeometryElementLiterals.h
    OsduGeoJsonGeometryElement.h                        OsduGeoJsonGeometryElement.cpp
    OsduGeoJsonGeometryElementStruct.h                  OsduGeoJsonGeometryElementStruct.cpp

    # Test: OsduSeismicTraceDataTests
    OsduGeoJsonGeometryTypeLiterals.h
    OsduGeoJsonGeometryType.h                           OsduGeoJsonGeometryType.cpp
    OsduGeoJsonGeometryTypeStruct.h                     OsduGeoJsonGeometryTypeStruct.cpp

    # Test: OsduSeismicTraceDataTests
    OsduAbstractGeoContextLiterals.h
    OsduAbstractGeoContext.h                            OsduAbstractGeoContext.cpp
    OsduAbstractGeoContextStruct.h                      OsduAbstractGeoContextStruct.cpp

    # Test: OsduSeismicTraceDataTests
    OsduAbstractWPCGroupTypeLiterals.h
    OsduAbstractWPCGroupType.h                          OsduAbstractWPCGroupType.cpp
    OsduAbstractWPCGroupTypeStruct.h                    OsduAbstractWPCGroupTypeStruct.cpp

    # Test: OsduSeismicTraceDataTests
    OsduAbstractArtefactLiterals.h
    OsduAbstractArtefact.h                              OsduAbstractArtefact.cpp
    OsduAbstractArtefactStruct.h                        OsduAbstractArtefactStruct.cpp

    # Test: OsduSeismicTraceDataTests
    OsduAbstractTechnicalAssuranceLiterals.h
    OsduAbstractTechnicalAssurance.h                    OsduAbstractTechnicalAssurance.cpp
    OsduAbstractTechnicalAssuranceStruct.h              OsduAbstractTechnicalAssuranceStruct.cpp

    # Test: OsduSeismicTraceDataTests
    OsduAbstractNameAliasLiterals.h
    OsduAbstractNameAlias.h                             OsduAbstractNameAlias.cpp
    OsduAbstractNameAliasStruct.h                       OsduAbstractNameAliasStruct.cpp

    # Test: OsduSeismicTraceDataTests
    OsduAbstractReviewerLiterals.h
    OsduAbstractReviewer.h                              OsduAbstractReviewer.cpp
    OsduAbstractReviewerStruct.h                        OsduAbstractReviewerStruct.cpp

    # Test: OsduSeismicTraceDataTests
    OsduAbstractAcceptableUsageLiterals.h
    OsduAbstractAcceptableUsage.h                       OsduAbstractAcceptableUsage.cpp
    OsduAbstractAcceptableUsageStruct.h                 OsduAbstractAcceptableUsageStruct.cpp

    # Test: OsduSeismicLineGeometryTests
    OsduSeismicLineGeometryLiterals.h
    OsduSeismicLineGeometry.h                           OsduSeismicLineGeometry.cpp
    OsduSeismicLineGeometryStruct.h                     OsduSeismicLineGeometryStruct.cpp

    # Test: OsduSeismicLineGeometryTests
    OsduSeismicLineGeometryDataLiterals.h
    OsduSeismicLineGeometryData.h                       OsduSeismicLineGeometryData.cpp
    OsduSeismicLineGeometryDataStruct.h                 OsduSeismicLineGeometryDataStruct.cpp

    # Test: OsduSeismicTraceNavigationTests
    OsduSeismicTraceNavigationLiterals.h
    OsduSeismicTraceNavigation.h                        OsduSeismicTraceNavigation.cpp
    OsduSeismicTraceNavigationStruct.h                  OsduSeismicTraceNavigationStruct.cpp

    # Test: OsduSeismicTraceNavigationTests
    OsduLineSegmentLiterals.h
    OsduLineSegment.h                                   OsduLineSegment.cpp
    OsduLineSegmentStruct.h                             OsduLineSegmentStruct.cpp

    # Test: OsduSeismicTraceNavigationTests
    OsduPointRangeLiterals.h
    OsduPointRange.h                                    OsduPointRange.cpp
    OsduPointRangeStruct.h                              OsduPointRangeStruct.cpp

    # Test: OsduSeismicTraceNavigationTests
    OsduShotPointCommonMidPointRatioLiterals.h
    OsduShotPointCommonMidPointRatio.h                  OsduShotPointCommonMidPointRatio.cpp
    OsduShotPointCommonMidPointRatioStruct.h            OsduShotPointCommonMidPointRatioStruct.cpp

    # Test: OsduSeismicTraceNavigationTests
    OsduGenericLabelsLiterals.h
    OsduGenericLabels.h                                 OsduGenericLabels.cpp
    OsduGenericLabelsStruct.h                           OsduGenericLabelsStruct.cpp
    OsduGenericLabels

    # Test: OsduFileSeismicLineGeometryTests
    OsduFileSeismicLineGeometryLiterals.h
    OsduFileSeismicLineGeometry.h                       OsduFileSeismicLineGeometry.cpp
    OsduFileSeismicLineGeometryStruct.h                 OsduFileSeismicLineGeometryStruct.cpp

    # Test: OsduFileSeismicLineGeometryTests
    OsduFileSeismicLineGeometryDataLiterals.h
    OsduFileSeismicLineGeometryData.h                   OsduFileSeismicLineGeometryData.cpp
    OsduFileSeismicLineGeometryDataStruct.h             OsduFileSeismicLineGeometryDataStruct.cpp

    # No special test available. To be used in other metadata classes
    OsduCommonMetadataDataLiterals.h
    OsduCommonMetadataData.h                            OsduCommonMetadataData.cpp
    OsduCommonMetadataDataStruct.h                      OsduCommonMetadataDataStruct.cpp

    OsduCommonMetadataLiterals.h
    OsduCommonMetadata.h                                OsduCommonMetadata.cpp
    OsduCommonMetadataStruct.h                          OsduCommonMetadataStruct.cpp

    # Test: OsduSeismic2DInterpretationSetTests
    OsduAbstractMasterLiterals.h
    OsduAbstractMaster.h                                OsduAbstractMaster.cpp
    OsduAbstractMasterStruct.h                          OsduAbstractMasterStruct.cpp

    # Test: OsduSeismic2DInterpretationSetTests
    OsduAbstractProjectLiterals.h
    OsduAbstractProject.h                               OsduAbstractProject.cpp
    OsduAbstractProjectStruct.h                         OsduAbstractProjectStruct.cpp

    # Test: OsduSeismic2DInterpretationSetTests
    OsduFundsAuthorizationLiterals.h
    OsduFundsAuthorization.h                            OsduFundsAuthorization.cpp
    OsduFundsAuthorizationStruct.h                      OsduFundsAuthorizationStruct.cpp

    # Test: OsduSeismic2DInterpretationSetTests
    OsduContractorLiterals.h
    OsduContractor.h                                    OsduContractor.cpp
    OsduContractorStruct.h                              OsduContractorStruct.cpp

    # Test: OsduSeismic2DInterpretationSetTests
    OsduPersonalLiterals.h
    OsduPersonal.h                                      OsduPersonal.cpp
    OsduPersonalStruct.h                                OsduPersonalStruct.cpp

    # Test: OsduSeismic2DInterpretationSetTests
    OsduProjectSpecificationLiterals.h
    OsduProjectSpecification.h                          OsduProjectSpecification.cpp
    OsduProjectSpecificationStruct.h                    OsduProjectSpecificationStruct.cpp

    # Test: OsduSeismic2DInterpretationSetTests
    OsduProjectStateLiterals.h
    OsduProjectState.h                                  OsduProjectState.cpp
    OsduProjectStateStruct.h                            OsduProjectStateStruct.cpp

    # Test: OsduSeismic2DInterpretationSetTests
    OsduAbstractProjectActivityLiterals.h
    OsduAbstractProjectActivity.h                       OsduAbstractProjectActivity.cpp
    OsduAbstractProjectActivityStruct.h                 OsduAbstractProjectActivityStruct.cpp

    # Test: OsduSeismic2DInterpretationSetTests
    OsduAbstractActivityParameterLiterals.h
    OsduAbstractActivityParameter.h                     OsduAbstractActivityParameter.cpp
    OsduAbstractActivityParameterStruct.h               OsduAbstractActivityParameterStruct.cpp

    # Test: OsduSeismic2DInterpretationSetTests
    OsduParameterKeyLiterals.h
    OsduParameterKey.h                                  OsduParameterKey.cpp
    OsduParameterKeyStruct.h                            OsduParameterKeyStruct.cpp

    # Test: OsduSeismic2DInterpretationSetTests
    OsduAbstractActivityStateLiterals.h
    OsduAbstractActivityState.h                         OsduAbstractActivityState.cpp
    OsduAbstractActivityStateStruct.h                   OsduAbstractActivityStateStruct.cpp

    # Test: OsduSeismic2DInterpretationSetTests
    OsduSeismic2DInterpretationSetLiterals.h
    OsduSeismic2DInterpretationSet.h                    OsduSeismic2DInterpretationSet.cpp
    OsduSeismic2DInterpretationSetStruct.h              OsduSeismic2DInterpretationSetStruct.cpp

    # Test: OsduSeismic2DInterpretationSetTests
    OsduSeismic2DInterpretationSetDataLiterals.h
    OsduSeismic2DInterpretationSetData.h                OsduSeismic2DInterpretationSetData.cpp
    OsduSeismic2DInterpretationSetDataStruct.h          OsduSeismic2DInterpretationSetDataStruct.cpp

    # Test: OsduSeismic2DInterpretationSetTests
    OsduSeismicLineGeometryRefLiterals.h
    OsduSeismicLineGeometryRef.h                        OsduSeismicLineGeometryRef.cpp
    OsduSeismicLineGeometryRefStruct.h                  OsduSeismicLineGeometryRefStruct.cpp

    # Test: OsduWorkProductTests
    OsduWorkProductLiterals.h
    OsduWorkProduct.h                                   OsduWorkProduct.cpp
    OsduWorkProductStruct.h                             OsduWorkProductStruct.cpp

    # Test: OsduWorkProductTests
    OsduWorkProductDataLiterals.h
    OsduWorkProductData.h                               OsduWorkProductData.cpp
    OsduWorkProductDataStruct.h                         OsduWorkProductDataStruct.cpp

    # Test: OsduSeismicBinGridTests
    OsduSeismicBinGridLiterals.h
    OsduSeismicBinGrid.h                                OsduSeismicBinGrid.cpp
    OsduSeismicBinGridStruct.h                          OsduSeismicBinGridStruct.cpp

    # Test: OsduSeismicBinGridTests
    OsduSeismicBinGridDataLiterals.h
    OsduSeismicBinGridData.h                            OsduSeismicBinGridData.cpp
    OsduSeismicBinGridDataStruct.h                      OsduSeismicBinGridDataStruct.cpp

    # Test: OsduSeismicBinGridTests
    OsduAbstractBinGridLiterals.h
    OsduAbstractBinGrid.h                               OsduAbstractBinGrid.cpp
    OsduAbstractBinGridStruct.h                         OsduAbstractBinGridStruct.cpp

    OsduFileImagePng.h                                  OsduFileImagePng.cpp
    OsduFileImagePngLiterals.h
)

target_include_directories(segysdk.OsduMetadataUtilities
    PRIVATE
        ${CMAKE_SEGYSDK_SOURCE_PATH}
        ${OPENZGY_INCLUDE_DIR}
        ${jsoncpp_INCLUDE_PATH}
)

target_compile_definitions(segysdk.OsduMetadataUtilities
    PRIVATE
        SEGYSDK_BUILDING_DLL
)

target_link_libraries(segysdk.OsduMetadataUtilities
    PUBLIC
        segysdk.json_parser
)

add_subdirectory(test)
