// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <imgdoc2.h>
#include "importexport.h"
#include "logicalpositioninfointerop.h"
#include "tilecoordinateinterop.h"
#include "dimcoordinatequeryclauseinterop.h"
#include "tileinfoqueryclauseinterop.h"
#include "tilebaseinfointerop.h"
#include "tileblobinfointerop.h"
#include "rectangledoubleinterop.h"

class Utilities
{
public:
    static imgdoc2::TileCoordinate ConvertToTileCoordinate(const TileCoordinateInterop* tile_coordinate_interop);
    static imgdoc2::LogicalPositionInfo ConvertLogicalPositionInfoInteropToImgdoc2(const LogicalPositionInfoInterop& logical_position_info_interop);
    static LogicalPositionInfoInterop ConvertImgDoc2LogicalPositionInfoToInterop(const imgdoc2::LogicalPositionInfo& logical_position_info);
    static imgdoc2::CDimCoordinateQueryClause ConvertDimensionQueryRangeClauseInteropToImgdoc2(const DimensionQueryClauseInterop* dim_coordinate_query_clause_interop);
    static imgdoc2::CTileInfoQueryClause ConvertTileInfoQueryClauseInteropToImgdoc2(const TileInfoQueryClauseInterop* tile_info_query_clause_interop);
    static imgdoc2::TileBaseInfo ConvertTileBaseInfoInteropToImgdoc2(const TileBaseInfoInterop& tile_base_info_interop);
    static imgdoc2::DataTypes ConvertDatatypeEnumInterop(std::uint8_t data_type_interop);
    static imgdoc2::RectangleD ConvertRectangleDoubleInterop(const RectangleDoubleInterop& rectangle_interop);
    static TileBlobInfoInterop ConvertImgDoc2TileBlobInfoToInterop(const imgdoc2::TileBlobInfo& tile_blob_info);

    /// Attempts to convert information from a tile-coordinate object into a tile-coordinate-interop-structure.
    /// This method is expecting that the tile_coordinate_interop-struct is provided by the caller, and that the 
    /// "number_of_elements"-member is giving the size of the allocation. 
    /// \param          tile_coordinate         The tile coordinate object.
    /// \param [in,out] tile_coordinate_interop The tile coordinate interop.
    /// \returns {bool} True if it succeeds, false if it fails.
    static bool TryConvertToTileCoordinateInterop(const imgdoc2::ITileCoordinate* tile_coordinate, TileCoordinateInterop* tile_coordinate_interop);
public:
    class BlobOutputOnFunctionsDecorator : public imgdoc2::IBlobOutput
    {
    public:
        typedef bool(LIBIMGDOC2_STDCALL* fnReserve)(std::intptr_t /*blob_output_handle*/, std::uint64_t /*size*/); // NOLINT(readability/casting)
        typedef bool(LIBIMGDOC2_STDCALL* fnSetData)(std::intptr_t /*blob_output_handle*/, std::uint64_t /*offset*/, std::uint64_t /*size*/, const void* /*data*/); // NOLINT(readability/casting)
        BlobOutputOnFunctionsDecorator(std::intptr_t blob_output_handle, fnReserve fpnReserve, fnSetData fpnSetData) :
            blob_output_handle_(blob_output_handle), fpnReserve_(fpnReserve), fpnSetData_(fpnSetData)
        {}

        bool Reserve(size_t s) override
        {
            return this->fpnReserve_(this->blob_output_handle_, s);
        }

        bool SetData(size_t offset, size_t size, const void* data) override
        {
            return this->fpnSetData_(this->blob_output_handle_, offset, size, data);
        }
    private:
        std::intptr_t blob_output_handle_;
        fnReserve fpnReserve_;
        fnSetData fpnSetData_;
    };

    static imgdoc2::LogicalOperator ConvertToLogicalOperatorEnum(std::uint8_t value);
    static imgdoc2::ComparisonOperation ConvertToComparisonOperationEnum(std::uint8_t value);

    /// Copies the specified source string to the destination string. The destination
    /// string has the size specified by n. It is ensured that the destination will be
    /// null-terminated. If the source string is too long, it will be truncated.
    ///
    /// \param          src     The source string.
    /// \param [out]    dest    The destination string.
    /// \param          n       The maximum size of the destination string (including the null terminator).
    static void copy_string_to_fixed_size(const char* src, char* dest, size_t n);
};
