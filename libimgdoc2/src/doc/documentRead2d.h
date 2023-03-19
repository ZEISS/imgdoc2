// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <utility>
#include <memory>
#include <map>
#include <vector>
#include <imgdoc2.h>
#include "document.h"
#include "documentReadBase.h"
#include "ITileCoordinate.h"

class DocumentRead2d : public DocumentReadBase, public imgdoc2::IDocRead2d
{
public:
    explicit DocumentRead2d(std::shared_ptr<Document> document) : DocumentReadBase(std::move(document))
    {}

    // interface IDocQuery2d
    void ReadTileInfo(imgdoc2::dbIndex idx, imgdoc2::ITileCoordinateMutate* coord, imgdoc2::LogicalPositionInfo* info, imgdoc2::TileBlobInfo* tile_blob_info) override;
    void Query(const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause, const std::function<bool(imgdoc2::dbIndex)>& func) override;
    void GetTilesIntersectingRect(const imgdoc2::RectangleD& rect, const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause, const std::function<bool(imgdoc2::dbIndex)>& func) override;
    void ReadTileData(imgdoc2::dbIndex idx, imgdoc2::IBlobOutput* data) override;

    // interface IDocInfo
    void GetTileDimensions(imgdoc2::Dimension* dimensions, std::uint32_t& count) override;
    std::map<imgdoc2::Dimension, imgdoc2::CoordinateBounds> GetMinMaxForTileDimension(const std::vector<imgdoc2::Dimension>& dimensions_to_query_for) override;
   
private:
    //std::shared_ptr<IDbStatement> GetReadTileInfo_Statement(bool include_tile_coordinates, bool include_logical_position_info);
    std::shared_ptr<IDbStatement> GetReadTileInfo_Statement(bool include_tile_coordinates, bool include_logical_position_info, bool include_tile_blob_info);
    std::shared_ptr<IDbStatement> CreateQueryStatement(const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause);
    std::shared_ptr<IDbStatement> GetTilesIntersectingRectQueryWithSpatialIndex(const imgdoc2::RectangleD& rect);
    std::shared_ptr<IDbStatement> GetTilesIntersectingRectQuery(const imgdoc2::RectangleD& rect);
    std::shared_ptr<IDbStatement> GetTilesIntersectingRectQueryAndCoordinateAndInfoQueryClauseWithSpatialIndex(const imgdoc2::RectangleD& rect, const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause);
    std::shared_ptr<IDbStatement> GetTilesIntersectingRectQueryAndCoordinateAndInfoQueryClause(const imgdoc2::RectangleD& rect, const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause);
    std::shared_ptr<IDbStatement> GetReadDataQueryStatement(imgdoc2::dbIndex idx);

    std::shared_ptr<IDbStatement> CreateQueryMinMaxStatement(const std::vector<imgdoc2::Dimension>& dimensions);
};
