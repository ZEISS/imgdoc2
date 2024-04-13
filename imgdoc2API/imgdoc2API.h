// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>
#include "importexport.h"
#include "errorcodes.h"
#include "tilecoordinateinterop.h"
#include "logicalpositioninfointerop.h"
#include "logicalpositioninfo3dinterop.h"
#include "queryresultinterop.h"
#include "dimcoordinatequeryclauseinterop.h"
#include "tileinfoqueryclauseinterop.h"
#include "tileblobinfointerop.h"
#include "brickblobinfointerop.h"
#include "tilebaseinfointerop.h"
#include "brickbaseinfointerop.h"
#include "imgdoc2statisticsinterop.h"
#include "rectangledoubleinterop.h"
#include "cuboiddoubleinterop.h"
#include "minmaxfortiledimensioninterop.h"
#include "tilecountperlayerinterop.h"
#include "planenormalanddistanceinterop.h"
#include "versioninfointerop.h"
#include "allocationobject.h"

/** @file imgdoc2API.h
 *  The flat API for the imgdoc2 module.
 */

 /// Defines an alias representing the underlying type of an object handle. It is a pointer size value.
typedef std::intptr_t ObjectHandle;

static constexpr ObjectHandle kInvalidObjectHandle = 0; ///< (Immutable) Reserved value indicating an invalid object handle.

/// Defines an alias representing the handle of an environment object.
typedef ObjectHandle HandleEnvironmentObject;

/// Defines an alias representing the handle of an CreateOptions object.
typedef ObjectHandle HandleCreateOptions;

/// Defines an alias representing the handle of an OpenExistingOptions object.
typedef ObjectHandle HandleOpenExistingOptions;

/// Defines an alias representing the handle of an imgdoc2 document object.
typedef ObjectHandle HandleDoc;

/// Defines an alias representing the handle of an reader2d object.
typedef ObjectHandle HandleDocRead2D;

/// Defines an alias representing the handle of an writer2d object.
typedef ObjectHandle HandleDocWrite2D;

/// Defines an alias representing the handle of an reader3d object.
typedef ObjectHandle HandleDocRead3D;

/// Defines an alias representing the handle of an writer2d object.
typedef ObjectHandle HandleDocWrite3D;

/// Defines an alias representing a function pointer used for memory transfer operations. This function pointer is used with IDocRead2d_ReadTileData/IDocRead3d_ReadBrickData.
typedef bool(LIBIMGDOC2_STDCALL* MemTransferReserveFunctionPointer)(std::intptr_t /*blob_output_handle*/, std::uint64_t /*size*/); // NOLINT(readability/casting)

/// Defines an alias representing a function pointer used for memory transfer operations. This function pointer is used with IDocRead2d_ReadTileData/IDocRead3d_ReadBrickData.
typedef bool(LIBIMGDOC2_STDCALL* MemTransferSetDataFunctionPointer)(std::intptr_t /*blob_output_handle*/, std::uint64_t /*offset*/, std::uint64_t /*size*/, const void* /*data*/); // NOLINT(readability/casting)

/// Defines an alias representing a function pointer used for memory allocation (on the caller side).
typedef bool(LIBIMGDOC2_STDCALL* AllocMemoryFunctionPointer)(std::uint64_t size, AllocationObject* allocation_object); // NOLINT(readability/casting)

/// Gets version information. This function is used to retrieve the version information of the library.
/// Note that the specified allocate_memory_function is used to allocate memory for the strings in the VersionInfoInterop structure.
/// If the allocate_memory_function is null, then the strings will not be returned.
///
/// \param [out]    version_info                Structure where the version information will be placed.
/// \param          allocate_memory_function    If non-null, this "allocate memory function" will be used to allocate memory for the strings in the VersionInfoInterop structure
///  
/// \returns An error-code indicating success or failure of the operation.                                                                                       to be returned.
EXTERNAL_API(ImgDoc2ErrorCode) GetVersionInfo(VersionInfoInterop* version_info, AllocMemoryFunctionPointer allocate_memory_function);

/// Retrieve the 'ImgDoc2StatisticsInterop' structure, which contains various statistics about
/// the state of library.In particular, we keep track of how many outstanding object-handles we
/// have. This is useful for debugging purposes.
/// \param [out] statistics_interop     If non-null, the statistics is put here. If null, the call is a no-op.
EXTERNAL_API(void) GetStatistics(ImgDoc2StatisticsInterop* statistics_interop);

/// Create a new environment object. The environment-object created here will route certain actions to the
/// function pointers given here.
/// IMPORTANT: The environment object created here must have a lifetime greater than any of its usages.
///
/// \param          user_parameter                  A user parameter (which gets passed to the callback function following).
/// \param [in]     pfn_log                         If non-null, a function pointer which gets called for providing logging functionality.
/// \param [in]     pfn_is_level_active             If non-null, a function pointer which gets called to query if the specified logging level is active.
/// \param [in]     pfn_report_fatal_error_and_exit If non-null, a function pointer which gets called in case of a fatal internal error. This function is expected to *not* return, but terminate execution.
///
/// \returns A handle representing the new environment object.
EXTERNAL_API(HandleEnvironmentObject) CreateEnvironmentObject(
    std::intptr_t user_parameter, 
    void (LIBIMGDOC2_STDCALL * pfn_log)(std::intptr_t /*userparam*/, int /*level*/, const char* /*szMessage*/),
    bool (LIBIMGDOC2_STDCALL * pfn_is_level_active)(std::intptr_t /*userparam*/, int /*level*/),
    void (LIBIMGDOC2_STDCALL * pfn_report_fatal_error_and_exit)(std::intptr_t /*userparam*/, const char* /*szMessage*/));

/// Destroys the environment object described by the specified handle.
/// NB: it is the caller's responsibility to ensure that the object is not in use when destroying it.
///
/// \param  handle The environment object handle.
/// \param [out]    error_information         If non-null, in case of an error, additional information describing the error are put here.
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) DestroyEnvironmentObject(HandleEnvironmentObject handle, ImgDoc2ErrorInformation* error_information);

/// Create a new "CreateOptions" object.
/// \returns A handle representing the new CreateOptions-object.
EXTERNAL_API(HandleCreateOptions) CreateCreateOptions();

/// Destroys the CreateOptions-object described by the specified handle.
/// \param  handle The CreateOptions-object handle.
/// \param [out]    error_information         If non-null, in case of an error, additional information describing the error are put here.
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) DestroyCreateOptions(HandleCreateOptions handle, ImgDoc2ErrorInformation* error_information);

/// Create a new "OpenExistingOptions" object.
/// \returns A handle representing the new OpenExistingOptions-object.
EXTERNAL_API(HandleOpenExistingOptions) CreateOpenExistingOptions();

/// Destroys the OpenExistingOptions-object described by the specified handle.
/// \param  handle The OpenExistingOptions-object handle.
/// \param [out]    error_information         If non-null, in case of an error, additional information describing the error are put here.
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) DestroyOpenExistingOptions(HandleOpenExistingOptions handle, ImgDoc2ErrorInformation* error_information);

/// Creates a new document object. A handle representing the newly created object (in case of success)
/// is returned with the 'document'-argument.
///
/// \param          create_options            Handle of the CreateOptions object.
/// \param          handle_environment_object Optional handle of an environment object.
/// \param [out]    document                  In case of success, the handle representing the document-object is put here.
/// \param [out]    error_information         If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) CreateNewDocument(HandleCreateOptions create_options, HandleEnvironmentObject handle_environment_object, HandleDoc* document, ImgDoc2ErrorInformation* error_information);

/// Open an existing document (i.e. from some persistent storage) and creates a new document object representing it. A handle representing the newly created object (in case of success)
/// is returned with the 'document'-argument.
///
/// \param          open_existing_options     Handle of the CreateOptions object.
/// \param          handle_environment_object Optional handle of an environment object.
/// \param [out]    document                  In case of success, the handle representing the document-object is put here.
/// \param [out]    error_information         If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) OpenExistingDocument(HandleOpenExistingOptions open_existing_options, HandleEnvironmentObject handle_environment_object, HandleDoc* document, ImgDoc2ErrorInformation* error_information);

/// Destroy the specified document.
/// \param          handle                    Handle of a document object (which is to be destroyed).
/// \param [out]    error_information         If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) DestroyDocument(HandleDoc handle, ImgDoc2ErrorInformation* error_information);

/// Method operating on a document-object: try to create a reader-2d-object on the document.
///
/// \param          handle_document   Handle representing the document.
/// \param [out] document_read2d      In case of success, the handle representing the reader-2d-object is put here.
/// \param [out] error_information    If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) IDoc_GetReader2d(HandleDoc handle_document, HandleDocRead2D* document_read2d, ImgDoc2ErrorInformation* error_information);

/// Destroy the specified reader-2d-object.
/// \param       handle               Handle of a reader-2d-object (which is to be destroyed).
/// \param [out] error_information    If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) DestroyReader2d(HandleDocRead2D handle, ImgDoc2ErrorInformation* error_information);

/// Method operating on a document-object: try to create a reader-3d-object on the document.
///
/// \param       handle_document      Handle representing the document.
/// \param [out] document_read3d      In case of success, the handle representing the reader-3d-object is put here.
/// \param [out] error_information    If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) IDoc_GetReader3d(HandleDoc handle_document, HandleDocRead2D* document_read3d, ImgDoc2ErrorInformation* error_information);

/// Destroy the specified reader-3d-object.
/// \param       handle               Handle of a reader-3d-object (which is to be destroyed).
/// \param [out] error_information    If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) DestroyReader3d(HandleDocRead2D handle, ImgDoc2ErrorInformation* error_information);

/// Method operating on a document-object: try to create a writer-2d-object on the document.
///
/// \param       handle_document      Handle representing the document.
/// \param [out] document_write2d     In case of success, the handle representing the writer-2d-object is put here.
/// \param [out] error_information    If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) IDoc_GetWriter2d(HandleDoc handle_document, HandleDocWrite2D* document_write2d, ImgDoc2ErrorInformation* error_information);

/// Destroy the specified reader-2d-object.
/// \param  handle Handle of a reader-2d-object (which is to be destroyed).
/// \param [out] error_information    If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) DestroyWriter2d(HandleDocWrite2D handle, ImgDoc2ErrorInformation* error_information);

/// Method operating on a document-object: try to create a writer-3d-object on the document.
///
/// \param       handle_document      Handle representing the document.
/// \param [out] document_write3d     In case of success, the handle representing the writer-3d-object is put here.
/// \param [out] error_information    If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) IDoc_GetWriter3d(HandleDoc handle_document, HandleDocWrite2D* document_write3d, ImgDoc2ErrorInformation* error_information);

/// Destroy the specified writer-3d-object.
/// \param  handle Handle of a writer-3d-object (which is to be destroyed).
/// \param [out] error_information    If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) DestroyWriter3d(HandleDocWrite3D handle, ImgDoc2ErrorInformation* error_information);

/// Method operating on a CreateOptions-object: Set the document type.
///
/// \param          handle                The handle of the CreateOptions object.
/// \param          document_type_interop The document type interop.
/// \param [in,out] error_information     If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_SetDocumentType(HandleCreateOptions handle, std::uint8_t document_type_interop, ImgDoc2ErrorInformation* error_information);

/// Method operating on a CreateOptions-object: Set the filename.
///
/// \param          handle            The handle of the CreateOptions object.
/// \param          filename_utf8     The filename (given as an UTF-8 encoded string).
/// \param [in,out] error_information If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_SetFilename(HandleCreateOptions handle, const char* filename_utf8, ImgDoc2ErrorInformation* error_information);

/// Method operating on a CreateOptions-object: Set the flag whether to use a spatial index.
///
/// \param          handle            The handle of the CreateOptions object.
/// \param          use_spatial_index Flag indicating whether a spatial index is to be constructed.
/// \param [in,out] error_information If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_SetUseSpatialIndex(HandleCreateOptions handle, bool use_spatial_index, ImgDoc2ErrorInformation* error_information);

/// Method operating on a CreateOptions-object: Specify a dimension for which an index is to be created.
///
/// \param          handle            The handle of the CreateOptions object.
/// \param          dimension         Dimension for which an index is to be created.
/// \param [in,out] error_information If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_AddIndexForDimension(HandleCreateOptions handle, char dimension, ImgDoc2ErrorInformation* error_information);

/// Method operating on a CreateOptions-object: Specify whether a "blob-table" is to be created within the database.
///
/// \param          handle            The handle of the CreateOptions object.
/// \param          use_blob_table    Boolean whether a blob-table is to be created.
/// \param [in,out] error_information If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_SetUseBlobTable(HandleCreateOptions handle, bool use_blob_table, ImgDoc2ErrorInformation* error_information);

/// Method operating on a CreateOptions-object: Get the property 'filename' from the CreateOptions-object (as an UTF8-encoded string).
/// On input, 'size' specifies the size of the buffer pointed to 'filename_utf8' in bytes. On return, the actual
/// number of bytes required is put here (including the terminating zero character).
/// If 'filename_utf8' is non-null, then at most as many bytes as indicated by 'size' (on input) are written.
/// \param          handle               Handle identifying an CreateOptions-object.
/// \param [out]    filename_utf8        If non-null, the buffer where the string will be placed.
/// \param [in,out] size                 On input, the size of the buffer pointed to by 'filename_utf8'; on output the number of bytes actually required.
/// \param [out]    error_information    If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_GetFilename(HandleCreateOptions handle, char* filename_utf8, size_t* size, ImgDoc2ErrorInformation* error_information);

/// Method operating on a CreateOptions-object: get the document type setting from the CreateOptions object.
///
/// \param          handle                The handle of the CreateOptions object.
/// \param [out]    document_type_interop If non-null, the document type is put here.
/// \param [out]    error_information     If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_GetDocumentType(HandleCreateOptions handle, std::uint8_t* document_type_interop, ImgDoc2ErrorInformation* error_information);

/// Method operating on a CreateOptions-object: get whether a spatial index is to be constructed.
///
/// \param          handle                The handle of the CreateOptions object.
/// \param [out]    use_spatial_index     If non-null, a boolean indicating whether a spatial index is to be constructed is put here.
/// \param [out]    error_information     If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_GetUseSpatialIndex(HandleCreateOptions handle, bool* use_spatial_index, ImgDoc2ErrorInformation* error_information);

/// Method operating on a CreateOptions-object: get whether a blob-table is to be constructed.
///
/// \param          handle                The handle of the CreateOptions object.
/// \param [out]    create_blob_table     If non-null, a boolean indicating whether a blob-table is to be constructed is put here.
/// \param [out]    error_information     If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_GetUseBlobTable(HandleCreateOptions handle, bool* create_blob_table, ImgDoc2ErrorInformation* error_information);

/// Method operating on a CreateOptions-object: add a dimension.
///
/// \param          handle                The handle of the CreateOptions object.
/// \param [out]    dimension             The dimension to be added.
/// \param [out]    error_information     If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_AddDimension(HandleCreateOptions handle, std::uint8_t dimension, ImgDoc2ErrorInformation* error_information);

/// Method operating on a CreateOptions-object: add a dimension for which an index is to be created. Or, more
/// exactly: mark that for the specified dimension that an index is to created.
///
/// \param          handle                The handle of the CreateOptions object.
/// \param [out]    dimension             The dimension to for which an index is to be created.
/// \param [out]    error_information     If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_AddIndexedDimension(HandleCreateOptions handle, std::uint8_t dimension, ImgDoc2ErrorInformation* error_information);

/// Method operating on a CreateOptions-object: get the list of dimension. On input, the value where 'elements_count' is pointing
/// to must contain the size of the array where 'dimension' is pointing to (in units of elements); on output the actual number of
/// elements available is put here, and at most as many elements that were initially indicated are placed at the 'dimension' array.
///
/// \param          handle            The handle of the CreateOptions object.
/// \param [out]    dimension         If non-null, the set of dimensions is placed (up to the count that was given with 'elements_count').
/// \param [in,out] elements_count    On input, the number of elements that can be places into the array 'dimension'; on output the total number of elements available.
/// \param [out]    error_information If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_GetDimensions(HandleCreateOptions handle, std::uint8_t* dimension, size_t* elements_count, ImgDoc2ErrorInformation* error_information);

/// Method operating on a CreateOptions-object: get the list of dimensions for which an index is to be created. 
/// On input, the value where 'elements_count' is pointing
/// to must contain the size of the array where 'dimension' is pointing to (in units of elements); on output the actual number of
/// elements available is put here, and at most as many elements that were initially indicated are placed at the 'dimension' array.
///
/// \param          handle            The handle of the CreateOptions object.
/// \param [out]    dimension         If non-null, the set of dimensions is placed (up to the count that was given with 'elements_count').
/// \param [in,out] elements_count    On input, the number of elements that can be places into the array 'dimension'; on output the total number of elements available.
/// \param [out]    error_information If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_GetIndexedDimensions(HandleCreateOptions handle, std::uint8_t* dimension, size_t* elements_count, ImgDoc2ErrorInformation* error_information);

/// Method operating on a OpenExistingOptions-object: Set the filename.
///
/// \param          handle            The handle of the CreateOptions object.
/// \param          filename_utf8     The filename (given as an UTF-8 encoded string).
/// \param [in,out] error_information If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) OpenExistingOptions_SetFilename(HandleOpenExistingOptions handle, const char* filename_utf8, ImgDoc2ErrorInformation* error_information);

/// Method operating on a OpenExistingOptions-object: Get the property 'filename' from the OpenExistingOptions-object (as an UTF8-encoded string).
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

/// Method operating on a writer2d-object: Add a tile to an image2d-document. On success, a key for the newly added tile is returned ('result_pk').
///
/// \param          handle                        The write2d-object.
/// \param          tile_coordinate_interop       The interop-structure containing the coordinate information.
/// \param          logical_position_info_interop The interop-structure containing the logical position information.
/// \param          tile_base_info_interop        The interop-structure containing the 'base tile information' information.
/// \param          data_type                     The data type of the type.
/// \param          ptr_data                      Pointer to the bitmap data of the tile.
/// \param          size_data                     Size of the memory pointer to by 'ptr_data'.
/// \param [out]    result_pk                     If non-null and in case of success, the primary key of the resulting data-set it put here.
/// \param [out]    error_information             If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
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

/// Method operating on a reader2d-object: query the tiles table. The two query clauses are
/// used to filter the tiles. The first clause is used to filter the tiles by their
/// coordinates, the second by other "per tile data". Matching tiles are returned in the
/// 'result' structure.
///
/// \param          handle                              The reader2d object.
/// \param          dim_coordinate_query_clause_interop The interop-structure containing the coordinate query clause.
/// \param          tile_info_query_clause_interop      The interop-structure containing the tile-info query clause.
/// \param [in,out] result                              The result structure.
/// \param [out]    error_information                   If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) IDocRead2d_Query(
    HandleDocRead2D handle, 
    const DimensionQueryClauseInterop* dim_coordinate_query_clause_interop,
    const TileInfoQueryClauseInterop* tile_info_query_clause_interop,
    QueryResultInterop* result, 
    ImgDoc2ErrorInformation* error_information);

/// Method operating on a reader2d-object: read the pixel data the specified tile. The data transfer is done in
/// the following way: 'blob_output_handle' is an opaque pointer-size parameter (which is not used by the function
/// itself), which will be passed to the callback-functions 'pfnReserve' and 'pfnSetData'.  When the data is to be
/// transferred, the function "pfnReserve" is called and notifies the receiver about the size of the data. After
/// this call, next the function "pfnSetData" is called (potentially multiple times), passing in size and offset.
/// Size and offset are guaranteed to not exceed the size reported before (with 'pfnReserve'). Within 'pfnSetData',
/// the data must be copied to some caller-managed buffer - the pointer is only guaranteed to be valid within the
/// call to 'pfnSetData.
///
/// \param          handle                                  The reader2d object.
/// \param          pk                                      The primary key of the tile for which to retrieve the pixel data.
/// \param          blob_output_handle                      An opaque pointer size value (which gets passed to the two callback functions).
/// \param          pfnReserve                              Function pointer to function which will be called reporting the required size.
/// \param          pfnSetData                              Function pointer to function which will be called delivering the data. 
/// \param [out]    error_information                       If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns    An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) IDocRead2d_ReadTileData(
    HandleDocRead2D handle,
    std::int64_t pk,
    std::intptr_t blob_output_handle,
    MemTransferReserveFunctionPointer pfnReserve,
    MemTransferSetDataFunctionPointer pfnSetData,
    ImgDoc2ErrorInformation* error_information);

/// Method operating on a reader2d-object: query the tiles table. The three query clauses are
/// used to filter the tiles. The first clause is used to filter the tiles by their
/// coordinates, the second by other "per tile data" and there is third geometric clause filtering
/// tiles which have an intersection with the specified 'query_rectangle'. Those clauses are logically ANDed.
/// Matching tiles are returned in the 'result' structure.
///
/// \param          handle                              The reader2d object.
/// \param          query_rectangle                     The query rectangle.
/// \param          dim_coordinate_query_clause_interop The interop-structure containing the coordinate query clause.
/// \param          tile_info_query_clause_interop      The interop-structure containing the tile-info query clause.
/// \param [out]    result                              The result structure.
/// \param [out]    error_information                   If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns    An error-code indicating success or failure of the operation.
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
/// \param [in,out] tile_coordinate_interop         If non-null, the retrieve tile-coordinate-information is put here.
///                                                 On input, the property "number_of_elements" of the TileCoordinateInterop-struct must be valid and indicate
///                                                 how much space is available. On output, the property "number_of_elements" is set to the actual number of 
///                                                 elements. 
/// \param [in,out] logical_position_info_interop   If non-null, the logical position information is put here.
/// \param [in,out] tile_blob_info_interop          If non-null, the "tile blob info" information is put here.
/// \param          error_information               If non-null, in case of an error, additional information describing the error are put here.
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) IDocRead2d_ReadTileInfo(
    HandleDocRead2D handle,
    std::int64_t pk,
    TileCoordinateInterop* tile_coordinate_interop,
    LogicalPositionInfoInterop* logical_position_info_interop,
    TileBlobInfoInterop* tile_blob_info_interop,
    ImgDoc2ErrorInformation* error_information);

/// Method operating on a writer3d-object: Add a brick to an image3d-document. On success, a key for the newly added brick is returned ('result_pk').
///
/// \param          handle                        The writer3d-object.
/// \param          tile_coordinate_interop       The interop-structure containing the coordinate information.
/// \param          logical_position_info_interop The interop-structure containing the logical position 3D information.
/// \param          brick_base_info_interop       The interop-structure containing the 'base brick information' information.
/// \param          data_type                     The data type of the type.
/// \param          ptr_data                      Pointer to the bitmap data of the brick.
/// \param          size_data                     Size of the memory pointer to by 'ptr_data'.
/// \param [out]    result_pk                     If non-null and in case of success, the primary key of the resulting data-set it put here.
/// \param [out]    error_information             If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) IDocWrite3d_AddBrick(
    HandleDocWrite3D handle,
    const TileCoordinateInterop* tile_coordinate_interop,
    const LogicalPositionInfo3DInterop* logical_position_info_interop,
    const BrickBaseInfoInterop* brick_base_info_interop,
    std::uint8_t data_type,
    const void* ptr_data,
    std::uint64_t size_data,
    imgdoc2::dbIndex* result_pk,
    ImgDoc2ErrorInformation* error_information);

// ------ IDocQuery3d ------

/// Method operating on a reader3d-object: reads tile information for the specified brick. There are three 
/// pieces of information which can be retrieved by this method, namely the tile-coordinate, the logical position 
/// and the tile-blob-info. If the respective pointers are null, the information will not be retrieved.
///
/// \param          handle                          The reader3d-object.
/// \param          pk                              The primary key of the brick to be read.
/// \param [out] tile_coordinate_interop            If non-null and the operation is successful, the tile-coordinate will be put here.
/// \param [out] logical_position_info3d_interop    If non-null and the operation is successful, the logical position will be put here.
/// \param [out] brick_blob_info_interop            If non-null and the operation is successful, the brick-blob-info will be put here.
/// \param [out] error_information                  If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) IDocRead3d_ReadBrickInfo(
    HandleDocRead3D handle,
    std::int64_t pk,
    TileCoordinateInterop* tile_coordinate_interop,
    LogicalPositionInfo3DInterop* logical_position_info3d_interop,
    BrickBlobInfoInterop* brick_blob_info_interop,
    ImgDoc2ErrorInformation* error_information);

/// Method operating on a reader3d-object: The two query clauses are used to filter the tiles. The first clause is used to filter the tiles by their
/// coordinates, the second by other "per tile data".
///
/// \param          handle                              The reader3d-object.
/// \param          dim_coordinate_query_clause_interop Interop-structure containing the query clause dealing with dimension indexes.
/// \param          tile_info_query_clause_interop      Interop-structure query clause dealing with other "per tile data".
/// \param [in,out] result                              The result structure.
/// \param [out]    error_information                   If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) IDocRead3d_Query(
    HandleDocRead3D handle,
    const DimensionQueryClauseInterop* dim_coordinate_query_clause_interop,
    const TileInfoQueryClauseInterop* tile_info_query_clause_interop,
    QueryResultInterop* result,
    ImgDoc2ErrorInformation* error_information);

/// Method operating on a reader3d-object: Gets tiles intersecting the specified cuboid (and satisfying the other criteria).
///
/// \param          handle                              The reader3d-object.
/// \param          query_cuboid                        The query cuboid.
/// \param          dim_coordinate_query_clause_interop Interop-structure containing the query clause dealing with dimension indexes.
/// \param          tile_info_query_clause_interop      Interop-structure query clause dealing with other "per tile data".
/// \param [out]    result                              The result structure.
/// \param [out]    error_information                   If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) IDocRead3d_GetBricksIntersectingCuboid(
    HandleDocRead3D handle,
    const CuboidDoubleInterop* query_cuboid,
    const DimensionQueryClauseInterop* dim_coordinate_query_clause_interop,
    const TileInfoQueryClauseInterop* tile_info_query_clause_interop,
    QueryResultInterop* result,
    ImgDoc2ErrorInformation* error_information);

/// Method operating on a reader3d-object: Gets tiles intersecting with the specified plane (and satisfying the other criteria).
///
/// \param          handle                              The reader3d-object.
/// \param          plane_normal_and_distance_interop   Parametrization of the plane.
/// \param          dim_coordinate_query_clause_interop Interop-structure containing the query clause dealing with dimension indexes.
/// \param          tile_info_query_clause_interop      Interop-structure query clause dealing with other "per tile data".
/// \param [out]    result                              The result structure.
/// \param [out]    error_information                   If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) IDocRead3d_GetBricksIntersectingPlane(
    HandleDocRead3D handle,
    const PlaneNormalAndDistanceInterop* plane_normal_and_distance_interop,
    const DimensionQueryClauseInterop* dim_coordinate_query_clause_interop,
    const TileInfoQueryClauseInterop* tile_info_query_clause_interop,
    QueryResultInterop* result,
    ImgDoc2ErrorInformation* error_information);

/// Method operating on a reader3d-object: reads the brick data for the specified brick.
/// The data transfer is done in
/// the following way: 'blob_output_handle' is an opaque pointer-size parameter (which is not used by the function
/// itself), which will be passed to the callback-functions 'pfnReserve' and 'pfnSetData'.  When the data is to be
/// transferred, the function "pfnReserve" is called and notifies the receiver about the size of the data. After
/// this call, next the function "pfnSetData" is called (potentially multiple times), passing in size and offset.
/// Size and offset are guaranteed to not exceed the size reported before (with 'pfnReserve'). Within 'pfnSetData',
/// the data must be copied to some caller-managed buffer - the pointer is only guaranteed to be valid within the
/// call to 'pfnSetData.
///
/// \param          handle                                  The reader3d-object.
/// \param          pk                                      The primary key of the brick to be read.
/// \param          blob_output_handle                      An opaque pointer size value (which gets passed to the two callback functions).
/// \param          pfnReserve                              Function pointer to function which will be called reporting the required size.
/// \param          pfnSetData                              Function pointer to function which will be called delivering the data. 
/// \param [out]    error_information                       If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) IDocRead3d_ReadBrickData(
    HandleDocRead3D handle,
    std::int64_t pk,
    std::intptr_t blob_output_handle,
    MemTransferReserveFunctionPointer pfnReserve,
    MemTransferSetDataFunctionPointer pfnSetData,
    ImgDoc2ErrorInformation* error_information);

/// Get the tile-dimensions used in the document. On input, the parameter 'count' must give the
/// size of the memory pointed to by 'dimensions' (= the number of elements in there). On output,
/// the actual number of elements available is put into 'count'. At most, the initial number 
/// of elements are copied to the 'dimension' buffer.
/// Note that this function is only applicable to 3d-documents (and offers the same functionality as IDocInfo3d_GetTileDimensions for 3d-documents).
///
/// \param           handle                         The handle of the read2d-object.
/// \param           dimensions                     Pointer to a buffer.
/// \param           count                          On input, the size of the array pointed to by 'dimensions', on output the actual number of dimension available.
/// \param           error_information              If non-null, in case of an error, additional information describing the error are put here.
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) IDocInfo2d_GetTileDimensions(
    HandleDocRead2D handle,
    imgdoc2::Dimension* dimensions,
    std::uint32_t* count,
    ImgDoc2ErrorInformation* error_information);

/// Get the tile-dimensions used in the document. On input, the parameter 'count' must give the
/// size of the memory pointed to by 'dimensions' (= the number of elements in there). On output,
/// the actual number of elements available is put into 'count'. At most, the initial number 
/// of elements are copied to the 'dimension' buffer.
/// Note that this function is only applicable to 3d-documents (and offers the same functionality as IDocInfo2d_GetTileDimensions for 2d-documents).
///
/// \param           handle                         The handle of the read3d-object.
/// \param           dimensions                     Pointer to a buffer.
/// \param           count                          On input, the size of the array pointed to by 'dimensions', on output the actual number of dimension available.
/// \param           error_information              If non-null, in case of an error, additional information describing the error are put here.
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) IDocInfo3d_GetTileDimensions(
    HandleDocRead3D handle,
    imgdoc2::Dimension* dimensions,
    std::uint32_t* count,
    ImgDoc2ErrorInformation* error_information);

/// Method operating on a reader2d-object: Retrieve the max and min for the coordinates
/// in the given dimensions. 'count' gives the number of elements in the array pointed to
/// by 'dimensions' and it gives the size of the output array 'result' (in units of elements).
///
/// \param          handle              The handle of the reader2d-object.
/// \param          dimensions          Pointer to an array of dimensions which are queried. The number of elements is given by 'count'.
/// \param          count               Number of elements in the 'dimensions' array and also the size of the output array 'result'
/// \param [out]    result              Pointer to an array where on successful return the requested information is put.
/// \param [out]    error_information   If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) IDocInfo2d_GetMinMaxForTileDimensions(
    HandleDocRead2D handle,
    const imgdoc2::Dimension* dimensions,
    std::uint32_t count,
    MinMaxForTilePositionsInterop* result,
    ImgDoc2ErrorInformation* error_information);

/// Method operating on a reader3d-object: Retrieve the max and min for the coordinates
/// in the given dimensions. 'count' gives the number of elements in the array pointed to
/// by 'dimensions' and it gives the size of the output array 'result' (in units of elements).
///
/// \param          handle              The handle of the reader3d-object.
/// \param          dimensions          Pointer to an array of dimensions which are queried. The number of elements is given by 'count'.
/// \param          count               Number of elements in the 'dimensions' array and also the size of the output array 'result'
/// \param [out]    result              Pointer to an array where on successful return the requested information is put.
/// \param [out]    error_information   If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) IDocInfo3d_GetMinMaxForTileDimensions(
    HandleDocRead3D handle,
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
EXTERNAL_API(ImgDoc2ErrorCode) IDocInfo2d_GetBoundingBoxForTiles(
    HandleDocRead2D handle,
    double* min_x,
    double* max_x,
    double* min_y,
    double* max_y,
    ImgDoc2ErrorInformation* error_information);

/// Retrieve the "axis aligned bounding box" (covering all bricks). This function is corresponding to the
/// method 'IDocInfo3d::GetBricksBoundingBox'. If the values cannot be retrieved (e.g. if the document
/// is empty), then the min-values will be set to 'std::numeric_limits<double>::max()' and the max-values
/// to 'std::numeric_limits<double>::lowest()'.
///
/// \param          handle            The handle of the read3d-object.
/// \param [in,out] min_x             If non-null, the minimum x coordinate is put here.
/// \param [in,out] max_x             If non-null, the maximum x coordinate is put here.
/// \param [in,out] min_y             If non-null, the minimum y coordinate is put here.
/// \param [in,out] max_y             If non-null, the maximum y coordinate is put here.
/// \param [in,out] min_z             If non-null, the minimum z coordinate is put here.
/// \param [in,out] max_z             If non-null, the maximum z coordinate is put here.
/// \param [in,out] error_information If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) IDocInfo3d_GetBoundingBoxForBricks(
    HandleDocRead3D handle,
    double* min_x,
    double* max_x,
    double* min_y,
    double* max_y,
    double* min_z,
    double* max_z,
    ImgDoc2ErrorInformation* error_information);

/// Get the total number of tiles in the document. This function is corresponding to the method
/// 'IDocInfo::GetTotalTileCount'.
/// Note that this function is only applicable to 2d-documents (and offers the same functionality as IDocInfo3d_GetTotalTileCount for 3d-documents).
///
/// \param          handle              he handle of the read2d-object.
/// \param [out]    total_tile_count    The total number of tiles (must be non-null).
/// \param [in,out] error_information   If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) IDocInfo2d_GetTotalTileCount(
        HandleDocRead2D handle,
        std::uint64_t* total_tile_count, 
        ImgDoc2ErrorInformation* error_information);

/// Get the total number of tiles in the document. This function is corresponding to the method
/// 'IDocInfo::GetTotalTileCount'.
/// Note that this function is only applicable to 3d-documents (and offers the same functionality as IDocInfo2d_GetTotalTileCount for 2d-documents).
///
/// \param          handle              he handle of the read3d-object.
/// \param [out]    total_tile_count    The total number of tiles (must be non-null).
/// \param [in,out] error_information   If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) IDocInfo3d_GetTotalTileCount(
        HandleDocRead3D handle,
        std::uint64_t* total_tile_count,
        ImgDoc2ErrorInformation* error_information);

/// Get the number of tiles per layer. This function is corresponding to the method 'IDocInfo::GetTileCountPerLayer'.
/// On input, the field 'element_count_allocated' of the 'tile_count_per_layer_interop' must be set to the number of elements
/// allocated in the structure, i.e. how many elements can be set in the array 'tile_count_per_layer_interop->tile_count_per_layer'.
/// On output, the field 'element_count_available' is set to the actual number of elements available. If this number is greater than
/// 'element_count_allocated' it means that not all results could be returned. In this case, only 'element_count_allocated' elements
/// are written to the array. The caller can then allocate a larger array and call this function again. 
/// Note that this function is only applicable to 2d-documents (and offers the same functionality as IDocInfo3d_GetTileCountPerLayer for 3d-documents).
///
/// \param          handle                          The handle.
/// \param [in,out] tile_count_per_layer_interop    If non-null, the tile count per layer interop.
/// \param [in,out] error_information               If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) IDocInfo2d_GetTileCountPerLayer(
        HandleDocRead2D handle,
        TileCountPerLayerInterop* tile_count_per_layer_interop,
        ImgDoc2ErrorInformation* error_information);

/// Get the number of tiles per layer. This function is corresponding to the method 'IDocInfo::GetTileCountPerLayer'.
/// On input, the field 'element_count_allocated' of the 'tile_count_per_layer_interop' must be set to the number of elements
/// allocated in the structure, i.e. how many elements can be set in the array 'tile_count_per_layer_interop->tile_count_per_layer'.
/// On output, the field 'element_count_available' is set to the actual number of elements available. If this number is greater than
/// 'element_count_allocated' it means that not all results could be returned. In this case, only 'element_count_allocated' elements
/// are written to the array. The caller can then allocate a larger array and call this function again. 
/// Note that this function is only applicable to 3d-documents (and offers the same functionality as IDocInfo2d_GetTileCountPerLayer for 2d-documents).
///
/// \param          handle                          The handle.
/// \param [in,out] tile_count_per_layer_interop    If non-null, the tile count per layer interop.
/// \param [in,out] error_information               If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) IDocInfo3d_GetTileCountPerLayer(
        HandleDocRead3D handle,
        TileCountPerLayerInterop* tile_count_per_layer_interop,
        ImgDoc2ErrorInformation* error_information);

/// Method operating on a writer2d-object: start a transaction.
/// \param          handle                          The handle of a writer2d object.
/// \param [in,out] error_information               If non-null, in case of an error, additional information describing the error are put here.
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) IDocWrite2d_BeginTransaction(
    HandleDocWrite2D handle,
    ImgDoc2ErrorInformation* error_information);

/// Method operating on a writer2d-object: commit a transaction.
/// \param          handle                          The handle of a writer2d object.
/// \param [in,out] error_information               If non-null, in case of an error, additional information describing the error are put here.
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) IDocWrite2d_CommitTransaction(
    HandleDocWrite2D handle,
    ImgDoc2ErrorInformation* error_information);

/// Method operating on a writer2d-object: rollback a transaction.
/// \param          handle                          The handle of a writer2d object.
/// \param [in,out] error_information               If non-null, in case of an error, additional information describing the error are put here.
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) IDocWrite2d_RollbackTransaction(
    HandleDocWrite2D handle,
    ImgDoc2ErrorInformation* error_information);

/// Method operating on a writer3d-object: start a transaction.
/// \param          handle                          The handle of a writer3d object.
/// \param [in,out] error_information               If non-null, in case of an error, additional information describing the error are put here.
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) IDocWrite3d_BeginTransaction(
    HandleDocWrite3D handle,
    ImgDoc2ErrorInformation* error_information);

/// Method operating on a writer3d-object: commit a transaction.
/// \param          handle                          The handle of a writer3d object.
/// \param [in,out] error_information               If non-null, in case of an error, additional information describing the error are put here.
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) IDocWrite3d_CommitTransaction(
    HandleDocWrite3D handle,
    ImgDoc2ErrorInformation* error_information);

/// Method operating on a writer3d-object: rollback a transaction.
/// \param          handle                          The handle of a writer3d object.
/// \param [in,out] error_information               If non-null, in case of an error, additional information describing the error are put here.
/// \returns An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) IDocWrite3d_RollbackTransaction(
    HandleDocWrite3D handle,
    ImgDoc2ErrorInformation* error_information);
