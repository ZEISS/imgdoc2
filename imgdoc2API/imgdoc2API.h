// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>
#include "importexport.h"
#include "errorcodes.h"
#include "tilecoordinateinterop.h"
#include "logicalpositioninfointerop.h"
#include "queryresultinterop.h"
#include "dimcoordinatequeryclauseinterop.h"
#include "tileinfoqueryclauseinterop.h"
#include "tileblobinfointerop.h"
#include "tilebaseinfointerop.h"
#include "statisticsinterop.h"
#include "rectangledoubleinterop.h"
#include "minmaxfortiledimensioninterop.h"
#include "tilecountperlayerinterop.h"

typedef std::intptr_t ObjectHandle;

static constexpr ObjectHandle kInvalidObjectHandle = 0;

typedef ObjectHandle HandleEnvironmentObject;
typedef ObjectHandle HandleCreateOptions;
typedef ObjectHandle HandleOpenExistingOptions;
typedef ObjectHandle HandleDoc;
typedef ObjectHandle HandleDocRead2D;
typedef ObjectHandle HandleDocWrite2D;

EXTERNAL_API(void)GetStatistics(ImgDoc2StatisticsInterop* statistics_interop);

EXTERNAL_API(HandleEnvironmentObject) CreateEnvironmentObject(
    std::intptr_t user_parameter, 
    void (*pfn_log)(std::intptr_t userparam, int level, const char* szMessage),
    bool (*pfn_is_level_active)(std::intptr_t userparam, int level),
    void (*pfn_report_fatal_error_and_exit)(std::intptr_t userparam, const char* szMessage));
EXTERNAL_API(void) DestroyEnvironmentObject(HandleEnvironmentObject handle);

// factory methods
EXTERNAL_API(HandleCreateOptions) CreateCreateOptions();
EXTERNAL_API(void) DestroyCreateOptions(HandleCreateOptions handle);

EXTERNAL_API(HandleOpenExistingOptions) CreateOpenExistingOptions();
EXTERNAL_API(void) DestroyOpenExistingOptions(HandleOpenExistingOptions handle);

EXTERNAL_API(ImgDoc2ErrorCode) CreateNewDocument(HandleCreateOptions create_options, HandleEnvironmentObject handle_environment_object, HandleDoc* document, ImgDoc2ErrorInformation* error_information);
EXTERNAL_API(ImgDoc2ErrorCode) OpenExistingDocument(HandleOpenExistingOptions open_existing_options, HandleEnvironmentObject handle_environment_object, HandleDoc* document, ImgDoc2ErrorInformation* error_information);
EXTERNAL_API(void) DestroyDocument(HandleDoc handle);

EXTERNAL_API(ImgDoc2ErrorCode) IDoc_GetReader2d(HandleDoc handle_document, HandleDocRead2D* document_read2d, ImgDoc2ErrorInformation* error_information);
EXTERNAL_API(void) DestroyReader2d(HandleDocRead2D handle);

EXTERNAL_API(ImgDoc2ErrorCode) IDoc_GetWriter2d(HandleDoc handle_document, HandleDocWrite2D* document_write2d, ImgDoc2ErrorInformation* error_information);
EXTERNAL_API(void) DestroyWriter2d(HandleDocWrite2D handle);

EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_SetFilename(HandleCreateOptions handle, const char* filename_utf8, ImgDoc2ErrorInformation* error_information);
EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_SetUseSpatialIndex(HandleCreateOptions handle, bool use_spatial_index, ImgDoc2ErrorInformation* error_information);
EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_AddIndexForDimension(HandleCreateOptions handle, char dimension, ImgDoc2ErrorInformation* error_information);
EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_SetUseBlobTable(HandleCreateOptions handle, bool use_blob_table, ImgDoc2ErrorInformation* error_information);

/// Get the property 'filename' from the CreateOptions-object (as an UTF8-encoded string).
/// On input, 'size' specifies the size of the buffer pointed to 'filename_utf8' in bytes. On return, the actual
/// number of bytes required is put here (including the terminating zero character).
/// If 'filename_utf8' is non-null, then at most as many bytes as indicated by 'size' (on input) are written.
/// \param          handle Handle identifying an CreateOptions-object.
/// \param [out]    filename_utf8        If non-null, the buffer where the string will be placed.
/// \param [in,out] size                 On input, the size of the buffer pointed to by 'filename_utf8'; on output the number of bytes actually required.
/// \param [out]    error_information    If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_GetFilename(HandleCreateOptions handle, char* filename_utf8, size_t* size, ImgDoc2ErrorInformation* error_information);

EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_GetUseSpatialIndex(HandleCreateOptions handle, bool* use_spatial_index, ImgDoc2ErrorInformation* error_information);
EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_GetUseBlobTable(HandleCreateOptions handle, bool* create_blob_table, ImgDoc2ErrorInformation* error_information);
EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_AddDimension(HandleCreateOptions handle, std::uint8_t dimension, ImgDoc2ErrorInformation* error_information);
EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_AddIndexedDimension(HandleCreateOptions handle, std::uint8_t dimension, ImgDoc2ErrorInformation* error_information);
EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_GetDimensions(HandleCreateOptions handle, std::uint8_t* dimension, size_t* elements_count, ImgDoc2ErrorInformation* error_information);
EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_GetIndexedDimensions(HandleCreateOptions handle, std::uint8_t* dimension, size_t* elements_count, ImgDoc2ErrorInformation* error_information);

EXTERNAL_API(ImgDoc2ErrorCode) OpenExistingOptions_SetFilename(HandleOpenExistingOptions handle, const char* filename_utf8, ImgDoc2ErrorInformation* error_information);

/// Get the property 'filename' from the OpenExistingOptions-object (as an UTF8-encoded string).
/// On input, 'size' specifies the size of the buffer pointed to 'filename_utf8' in bytes. On return, the actual
/// number of bytes required is put here (including the terminating zero character).
/// If 'filename_utf8' is non-null, then at most as many bytes as indicated by 'size' (on input) are written.
/// \param          handle Handle identifying an CreateOptions-object.
/// \param [out]    filename_utf8        If non-null, the buffer where the string will be placed.
/// \param [in,out] size                 On input, the size of the buffer pointed to by 'filename_utf8'; on output the number of bytes actually required.
/// \param [out]    error_information    If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) OpenExistingOptions_GetFilename(HandleOpenExistingOptions handle, char* filename_utf8, size_t* size, ImgDoc2ErrorInformation* error_information);

EXTERNAL_API(ImgDoc2ErrorCode) IDocWrite2d_AddTile(
    HandleDocWrite2D handle, 
    const TileCoordinateInterop* tile_coordinate_interop, 
    const LogicalPositionInfoInterop* logical_position_info_interop,
    const TileBaseInfoInterop* tile_base_info_interop,
    std::uint8_t data_type,
    const void* ptr_data,
    std::uint64_t size_data,
    imgdoc2::dbIndex* result_pk,
    ImgDoc2ErrorInformation* error_information);

EXTERNAL_API(ImgDoc2ErrorCode) IDocRead2d_Query(
    HandleDocRead2D handle, 
    const DimensionQueryClauseInterop* dim_coordinate_query_clause_interop,
    const TileInfoQueryClauseInterop* tile_info_query_clause_interop,
    QueryResultInterop* result, 
    ImgDoc2ErrorInformation* error_information);

EXTERNAL_API(ImgDoc2ErrorCode) IDocRead2d_ReadTileData(
    HandleDocRead2D handle,
    std::int64_t pk,
    std::intptr_t blob_output_handle,
    bool(LIBIMGDOC2_STDCALL* pfnReserve)(std::intptr_t /*blob_output_handle*/, std::uint64_t /*size*/), // NOLINT(readability/casting)
    bool(LIBIMGDOC2_STDCALL* pfnSetData)(std::intptr_t /*blob_output_handle*/, std::uint64_t /*offset*/, std::uint64_t /*size*/, const void* /*data*/), // NOLINT(readability/casting)
    ImgDoc2ErrorInformation* error_information);

EXTERNAL_API(ImgDoc2ErrorCode) IDocRead2d_GetTilesIntersectingRect(
    HandleDocRead2D handle,
    const RectangleDoubleInterop* query_rectangle,
    const DimensionQueryClauseInterop* dim_coordinate_query_clause_interop,
    const TileInfoQueryClauseInterop* tile_info_query_clause_interop,
    QueryResultInterop* result,
    ImgDoc2ErrorInformation* error_information);

/// Retrieve the "tile-info" of the specified tile. This function is corresponding to the
/// IDocRead2::ReadTileInfo-method.
/// \param           handle                         The handle of the read2d-object.
/// \param           pk                             The key of the tile to be read.
/// \param [in,out] tile_coordinate_interop 
/// If non-null, the retrieve tile-coordinate-information is put here.
/// On input, the property "number_of_elements" of the TileCoordinateInterop-struct must be valid and indicate
/// how much space is available. On output, the property "number_of_elements" is set to the actual number of 
/// elements. If the space is insufficient, then the return value is "..." TODO
/// \param [in,out] logical_position_info_interop   If non-null, the logical position information is put here.
/// \param [in,out] tile_blob_info_interop          If non-null, the "tile blob info" information is put here.
/// \param           error_information              If non-null, in case of an error, additional information describing the error are put here.
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) IDocRead2d_ReadTileInfo(
    HandleDocRead2D handle,
    std::int64_t pk,
    TileCoordinateInterop* tile_coordinate_interop,
    LogicalPositionInfoInterop* logical_position_info_interop,
    TileBlobInfoInterop* tile_blob_info_interop,
    ImgDoc2ErrorInformation* error_information);

/// Get the tile-dimensions used in the document. On input, the parameter 'count' must give the
/// size of the memory pointed to by 'dimensions' (= the number of elements in there). On output,
/// the actual number of elements available is put into 'count'. At most, the initial number 
/// of elements are copied to the 'dimension' buffer.
///
/// \param           handle                         The handle of the read2d-object.
/// \param           dimensions                     Pointer to a buffer.
/// \param           count                          On input, the size of the array pointed to by 'dimensions', on output the actual number of dimension available.
/// \param           error_information              If non-null, in case of an error, additional information describing the error are put here.
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) IDocInfo_GetTileDimensions(
    HandleDocRead2D handle,
    imgdoc2::Dimension* dimensions,
    std::uint32_t* count,
    ImgDoc2ErrorInformation* error_information);

EXTERNAL_API(ImgDoc2ErrorCode) IDocInfo_GetMinMaxForTileDimensions(
    HandleDocRead2D handle,
    const imgdoc2::Dimension* dimensions,
    std::uint32_t count,
    MinMaxForTilePositionsInterop* result,
    ImgDoc2ErrorInformation* error_information);

/// Retrieve the "axis aligned bounding box" (covering all tiles). This function is corresponding to the
/// method 'IDocInfo2d::GetTilesBoundingBox'. If the values cannot be retrieved (e.g. if the document 
/// is empty), then the min-values will be set to 'std::numeric_limits<double>::max()' and the max-values
/// to 'std::numeric_limits<double>::lowest()'.
///
/// \param          handle            The handle of the read2d-object.
/// \param [in,out] min_x             If non-null, the minimum x coordinate is put here.
/// \param [in,out] max_x             If non-null, the maximum x coordinate is put here.
/// \param [in,out] min_y             If non-null, the minimum y coordinate is put here.
/// \param [in,out] max_y             If non-null, the maximum y coordinate is put here.
/// \param [in,out] error_information If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) IDocInfo_GetBoundingBoxForTiles(
    HandleDocRead2D handle,
    double* min_x,
    double* max_x,
    double* min_y,
    double* max_y,
    ImgDoc2ErrorInformation* error_information);

/// Get the total number of tiles in the document. This function is corresponding to the method
/// 'IDocInfo::GetTotalTileCount'.
///
/// \param          handle              he handle of the read2d-object.
/// \param [out]    total_tile_count    The total number of tiles (must be non-null).
/// \param [in,out] error_information   If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns    An ImgDoc2ErrorCode.
EXTERNAL_API(ImgDoc2ErrorCode) IDocInfo_GetTotalTileCount(
        HandleDocRead2D handle,
        std::uint64_t* total_tile_count, 
        ImgDoc2ErrorInformation* error_information);

/// Get the number of tiles per layer. This function is corresponding to the method 'IDocInfo::GetTileCountPerLayer'.
/// On input, the field 'element_count_allocated' of the 'tile_count_per_layer_interop' must be set to the number of elements
/// allocated in the structure, i.e. how many elements can be set in the array 'tile_count_per_layer_interop->tile_count_per_layer'.
/// On output, the field 'element_count_available' is set to the actual number of elements available. If this number is greater than
/// 'element_count_allocated' it means that not all results could be returned. In this case, only 'element_count_allocated' elements
/// are written to the array. The caller can then allocate a larger array and call this function again. 
///
/// \param          handle                          The handle.
/// \param [in,out] tile_count_per_layer_interop    If non-null, the tile count per layer interop.
/// \param [in,out] error_information               If non-null, information describing the error.
///
/// \returns    An ImgDoc2ErrorCode.
EXTERNAL_API(ImgDoc2ErrorCode) IDocInfo_GetTileCountPerLayer(
        HandleDocRead2D handle,
        TileCountPerLayerInterop* tile_count_per_layer_interop,
        ImgDoc2ErrorInformation* error_information);
