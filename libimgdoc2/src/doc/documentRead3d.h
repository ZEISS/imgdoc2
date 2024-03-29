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

class DocumentRead3d : public DocumentReadBase, public imgdoc2::IDocRead3d
{
public:
    explicit DocumentRead3d(std::shared_ptr<Document> document) : DocumentReadBase(std::move(document))
    {}

    // interface IDocQuery3d
    void ReadBrickInfo(imgdoc2::dbIndex idx, imgdoc2::ITileCoordinateMutate* coordinate, imgdoc2::LogicalPositionInfo3D* info, imgdoc2::BrickBlobInfo* brick_blob_info) override;
    void Query(const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause, const std::function<bool(imgdoc2::dbIndex)>& func) override;
    void GetTilesIntersectingCuboid(const imgdoc2::CuboidD& cuboid, const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause, const std::function<bool(imgdoc2::dbIndex)>& func) override;
    void GetTilesIntersectingPlane(const imgdoc2::Plane_NormalAndDistD& plane, const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause, const std::function<bool(imgdoc2::dbIndex)>& func) override;
    void ReadBrickData(imgdoc2::dbIndex idx, imgdoc2::IBlobOutput* data) override;

    // interface IDocInfo
    void GetTileDimensions(imgdoc2::Dimension* dimensions, std::uint32_t& count) override;
    std::map<imgdoc2::Dimension, imgdoc2::Int32Interval> GetMinMaxForTileDimension(const std::vector<imgdoc2::Dimension>& dimensions_to_query_for) override;
    std::uint64_t GetTotalTileCount() override;
    std::map<int, std::uint64_t> GetTileCountPerLayer() override;

    // interface IDocInfo3d
    void GetBricksBoundingBox(imgdoc2::DoubleInterval* bounds_x, imgdoc2::DoubleInterval* bounds_y, imgdoc2::DoubleInterval* bounds_z) override;
private:
    std::shared_ptr<IDbStatement> GetReadBrickInfo_Statement(bool include_brick_coordinates, bool include_logical_position_info, bool include_brick_blob_info);
    std::shared_ptr<IDbStatement> CreateQueryStatement(const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause);
    std::shared_ptr<IDbStatement> GetTilesIntersectingCuboidQueryWithSpatialIndex(const imgdoc2::CuboidD& cuboid) const;
    std::shared_ptr<IDbStatement> GetTilesIntersectingCuboidQuery(const imgdoc2::CuboidD& cuboid);
    std::shared_ptr<IDbStatement> GetTilesIntersectingCuboidQueryAndCoordinateAndInfoQueryClauseWithSpatialIndex(const imgdoc2::CuboidD& cuboid, const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause);
    std::shared_ptr<IDbStatement> GetTilesIntersectingCuboidQueryAndCoordinateAndInfoQueryClause(const imgdoc2::CuboidD& cuboid, const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause);
    std::shared_ptr<IDbStatement> GetReadBrickDataQueryStatement(imgdoc2::dbIndex idx);

    std::shared_ptr<IDbStatement> GetTilesIntersectingWithPlaneQueryAndCoordinateAndInfoQueryClauseWithSpatialIndex(const imgdoc2::Plane_NormalAndDistD& plane, const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause) const; 
    std::shared_ptr<IDbStatement> GetTilesIntersectingWithPlaneQueryAndCoordinateAndInfoQueryClause(const imgdoc2::Plane_NormalAndDistD& plane, const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause) const;

    std::shared_ptr<IDbStatement> CreateQueryTilesBoundingBoxStatement(bool include_x, bool include_y, bool include_z) const;
};
