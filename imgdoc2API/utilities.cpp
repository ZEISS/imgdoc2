// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include "utilities.h"
#include <algorithm>

using namespace imgdoc2;
using namespace std;

/*static*/imgdoc2::TileCoordinate Utilities::ConvertToTileCoordinate(const TileCoordinateInterop* tile_coordinate_interop)
{
    TileCoordinate tile_coordinate;
    for (int i = 0; i < tile_coordinate_interop->number_of_elements; ++i)
    {
        tile_coordinate.Set(
            tile_coordinate_interop->values[i].dimension,
            tile_coordinate_interop->values[i].value);
    }

    return tile_coordinate;
}

/*static*/imgdoc2::LogicalPositionInfo Utilities::ConvertLogicalPositionInfoInteropToImgdoc2(const LogicalPositionInfoInterop& logical_position_info_interop)
{
    const LogicalPositionInfo logical_position_info
    {
        logical_position_info_interop.position_x,
        logical_position_info_interop.position_y,
        logical_position_info_interop.width,
        logical_position_info_interop.height,
        logical_position_info_interop.pyramid_level
    };

    return logical_position_info;
}

/*static*/LogicalPositionInfoInterop Utilities::ConvertImgDoc2LogicalPositionInfoToInterop(const LogicalPositionInfo& logical_position_info)
{
    LogicalPositionInfoInterop logical_position_info_interop;
    logical_position_info_interop.position_x = logical_position_info.posX;
    logical_position_info_interop.position_y = logical_position_info.posY;
    logical_position_info_interop.width = logical_position_info.width;
    logical_position_info_interop.height = logical_position_info.height;
    logical_position_info_interop.pyramid_level = logical_position_info.pyrLvl;
    return logical_position_info_interop;
}

/*static*/TileBlobInfoInterop Utilities::ConvertImgDoc2TileBlobInfoToInterop(const imgdoc2::TileBlobInfo& tile_blob_info)
{
    TileBlobInfoInterop tile_blob_info_interop;
    tile_blob_info_interop.tile_base_info.pixelWidth = tile_blob_info.base_info.pixelWidth;
    tile_blob_info_interop.tile_base_info.pixelHeight = tile_blob_info.base_info.pixelHeight;
    tile_blob_info_interop.tile_base_info.pixelType = tile_blob_info.base_info.pixelType;
    tile_blob_info_interop.data_type = static_cast<underlying_type_t<decltype(tile_blob_info.data_type)>>(tile_blob_info.data_type);

    // static assertion ensuring that the interop-type and the underlying enum-type are the same
    static_assert(std::is_same_v<decltype(tile_blob_info_interop.data_type), underlying_type_t<decltype(tile_blob_info.data_type)>>, "This should be the same");

    return tile_blob_info_interop;
}

/*static*/imgdoc2::CDimCoordinateQueryClause Utilities::ConvertDimensionQueryRangeClauseInteropToImgdoc2(const DimensionQueryClauseInterop* dim_coordinate_query_clause_interop)
{
    CDimCoordinateQueryClause query_clause;
    for (int i = 0; i < dim_coordinate_query_clause_interop->element_count; ++i)
    {
        query_clause.AddRangeClause(
            dim_coordinate_query_clause_interop->query_range_clause[i].dimension,
            IDimCoordinateQueryClause::RangeClause{ dim_coordinate_query_clause_interop->query_range_clause[i].start, dim_coordinate_query_clause_interop->query_range_clause[i].end });
    }

    return query_clause;
}

/*static*/imgdoc2::CTileInfoQueryClause Utilities::ConvertTileInfoQueryClauseInteropToImgdoc2(const TileInfoQueryClauseInterop* tile_info_query_clause_interop)
{
    CTileInfoQueryClause tile_info_query_clause;
    for (int i = 0; i < tile_info_query_clause_interop->pyramid_level_condition_count; ++i)
    {
        LogicalOperator logical_operator = ConvertToLogicalOperatorEnum(tile_info_query_clause_interop->pyramid_level_condition[i].logical_operator);
        ComparisonOperation  comparison_operation = ConvertToComparisonOperationEnum(tile_info_query_clause_interop->pyramid_level_condition[i].comparison_operator);
        int value = tile_info_query_clause_interop->pyramid_level_condition[i].value;
        tile_info_query_clause.AddPyramidLevelCondition(logical_operator, comparison_operation, value);
    }

    return tile_info_query_clause;
}

/*static*/imgdoc2::TileBaseInfo Utilities::ConvertTileBaseInfoInteropToImgdoc2(const TileBaseInfoInterop& tile_base_info_interop)
{
    TileBaseInfo tile_base_info;
    tile_base_info.pixelWidth = tile_base_info_interop.pixelWidth;
    tile_base_info.pixelHeight = tile_base_info_interop.pixelHeight;
    tile_base_info.pixelType = tile_base_info_interop.pixelType;
    return tile_base_info;
}

/*static*/ bool Utilities::TryConvertToTileCoordinateInterop(const ITileCoordinate* tile_coordinate, TileCoordinateInterop* tile_coordinate_interop)
{
    int number_of_elements_in_source = 0;
    tile_coordinate->EnumCoordinates(
        [&](imgdoc2::Dimension dimension, int value)->bool
        {
            if (number_of_elements_in_source < tile_coordinate_interop->number_of_elements)
            {
                tile_coordinate_interop->values[number_of_elements_in_source] = DimensionAndValueInterop{ dimension, value };
            }

            ++number_of_elements_in_source;
            return true;
        });

    if (number_of_elements_in_source <= tile_coordinate_interop->number_of_elements)
    {
        tile_coordinate_interop->number_of_elements = number_of_elements_in_source;
        return true;
    }

    return false;
}

/*static*/imgdoc2::DataTypes Utilities::ConvertDatatypeEnumInterop(std::uint8_t data_type_interop)
{
    // TODO(JBL): add error handling
    return static_cast<imgdoc2::DataTypes>(data_type_interop);
}

/*static*/imgdoc2::RectangleD Utilities::ConvertRectangleDoubleInterop(const RectangleDoubleInterop& rectangle_interop)
{
    return RectangleD(rectangle_interop.x, rectangle_interop.y, rectangle_interop.width, rectangle_interop.height);
}

imgdoc2::LogicalOperator Utilities::ConvertToLogicalOperatorEnum(std::uint8_t value)
{
    if (value > static_cast<underlying_type_t<LogicalOperator>>(LogicalOperator::MaxValue))
    {
        throw invalid_argument("invalid value for enum \"LogicalOperator\".");
    }

    return static_cast<LogicalOperator>(value);
}

imgdoc2::ComparisonOperation Utilities::ConvertToComparisonOperationEnum(std::uint8_t value)
{
    if (value > static_cast<underlying_type_t<ComparisonOperation>>(ComparisonOperation::MaxValue))
    {
        throw invalid_argument("invalid value for enum \"ComparisonOperation\".");
    }

    return static_cast<ComparisonOperation>(value);
}

void Utilities::copy_string_to_fixed_size(const char* src, char* dest, size_t n)
{
    const size_t length_of_source = strlen(src);
    const size_t length_to_copy = min(length_of_source, n - 1);
    memcpy(dest, src, length_to_copy);
    dest[length_to_copy] = '\0';
}
