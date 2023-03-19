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
#include "ITileCoordinate.h"

class DocumentRead3d : public imgdoc2::IDocRead3d
{
private:
    std::shared_ptr<Document> document_;
public:
    explicit DocumentRead3d(std::shared_ptr<Document> document) : document_(std::move(document))
    {}

    // interface IDocQuery3d
    void ReadBrickInfo(imgdoc2::dbIndex idx, imgdoc2::ITileCoordinateMutate* coord, imgdoc2::LogicalPositionInfo3D* info, imgdoc2::BrickBlobInfo* brick_blob_info) override;
    //void ReadTileInfo(imgdoc2::dbIndex idx, imgdoc2::ITileCoordinateMutate* coord, imgdoc2::LogicalPositionInfo* info, imgdoc2::TileBlobInfo* tile_blob_info) override;
    void Query(const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause, const std::function<bool(imgdoc2::dbIndex)>& func) override;
    //void GetTilesIntersectingRect(const imgdoc2::RectangleD& rect, const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause, const std::function<bool(imgdoc2::dbIndex)>& func) override;
    //void ReadTileData(imgdoc2::dbIndex idx, imgdoc2::IBlobOutput* data) override;

    void GetTilesIntersectingCuboid(const imgdoc2::CuboidD& cuboid, const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause, const std::function<bool(imgdoc2::dbIndex)>& func) override;
    void GetTilesIntersectingPlane(const imgdoc2::Plane_NormalAndDistD& plane, const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause, const std::function<bool(imgdoc2::dbIndex)>& func) override;


    // interface IDocInfo
    void GetTileDimensions(imgdoc2::Dimension* dimensions, std::uint32_t& count) override;
    std::map<imgdoc2::Dimension, imgdoc2::CoordinateBounds> GetMinMaxForTileDimension(const std::vector<imgdoc2::Dimension>& dimensions_to_query_for) override;

private:
    std::shared_ptr<IDbStatement> GetReadBrickInfo_Statement(bool include_tile_coordinates, bool include_logical_position_info, bool include_tile_blob_info);
    std::shared_ptr<IDbStatement> CreateQueryStatement(const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause);
    std::shared_ptr<IDbStatement> GetTilesIntersectingCuboidQueryWithSpatialIndex(const imgdoc2::CuboidD& cuboid);
    std::shared_ptr<IDbStatement> GetTilesIntersectingCuboidQuery(const imgdoc2::CuboidD& cuboid);
    std::shared_ptr<IDbStatement> GetTilesIntersectingCuboidQueryAndCoordinateAndInfoQueryClauseWithSpatialIndex(const imgdoc2::CuboidD& cuboid, const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause);
    std::shared_ptr<IDbStatement> GetTilesIntersectingCuboidQueryAndCoordinateAndInfoQueryClause(const imgdoc2::CuboidD& cuboid, const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause);
    //std::shared_ptr<IDbStatement> GetReadDataQueryStatement(imgdoc2::dbIndex idx);

    //std::shared_ptr<IDbStatement> CreateQueryMinMaxStatement(const std::vector<imgdoc2::Dimension>& dimensions);

    [[nodiscard]] const std::shared_ptr<imgdoc2::IHostingEnvironment>& GetHostingEnvironment() const { return this->document_->GetHostingEnvironment(); }
};
