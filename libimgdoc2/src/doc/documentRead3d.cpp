// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include <variant>
#include <sstream>
#include <algorithm>
#include <map>
#include <vector>
#include "documentRead3d.h"
#include "../db/utilities.h"

using namespace std;
using namespace imgdoc2;

/*virtual*/void DocumentRead3d::ReadBrickInfo(imgdoc2::dbIndex idx, imgdoc2::ITileCoordinateMutate* coord, imgdoc2::LogicalPositionInfo3D* info, imgdoc2::BrickBlobInfo* brick_blob_info)
{
    throw logic_error("The method or operation is not implemented.");
}
//void ReadTileInfo(imgdoc2::dbIndex idx, imgdoc2::ITileCoordinateMutate* coord, imgdoc2::LogicalPositionInfo* info, imgdoc2::TileBlobInfo* tile_blob_info) override;
//void Query(const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause, const std::function<bool(imgdoc2::dbIndex)>& func) override;
//void GetTilesIntersectingRect(const imgdoc2::RectangleD& rect, const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause, const std::function<bool(imgdoc2::dbIndex)>& func) override;
//void ReadTileData(imgdoc2::dbIndex idx, imgdoc2::IBlobOutput* data) override;

// interface IDocInfo
/*virtual*/void DocumentRead3d::GetTileDimensions(imgdoc2::Dimension* dimensions, std::uint32_t& count)
{
    throw logic_error("The method or operation is not implemented.");
}

/*virtual*/std::map<imgdoc2::Dimension, imgdoc2::CoordinateBounds> DocumentRead3d::GetMinMaxForTileDimension(const std::vector<imgdoc2::Dimension>& dimensions_to_query_for)
{
    throw logic_error("The method or operation is not implemented.");
}
