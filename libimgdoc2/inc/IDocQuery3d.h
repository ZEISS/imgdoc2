// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once
#include "LogicalPositionInfo.h"
#include "BrickBaseInfo.h"
#include "ITileCoordinate.h"
#include "IDimCoordinateQueryClause.h"
#include "ITIleInfoQueryClause.h"
#include "IBlobOutput.h"

namespace imgdoc2
{
    /// This interface is providing read-only access to the 3d-document.
    class IDocQuery3d
    {
    public:
        virtual ~IDocQuery3d() = default;

        /// Reads tile information for the specified brick. There are three pieces of information which can be retrieved by this method,
        /// namely the tile-coordinate, the logical position and the tile-blob-info. If the respective pointers are null, the information
        /// will not be retrieved.
        /// If the row for the specified primary key does not exist, an exception of type "imgdoc2::non_existing_tile_exception" will be
        /// thrown.
        ///
        /// \param          idx             The primary key of the tile.
        /// \param [out]    coordinate      If non-null and the operation is successful, the tile-coordinate will be put here.
        /// \param [out]    info            If non-null and the operation is successful, the logical position will be put here.
        /// \param [out]    brick_blob_info If non-null and the operation is successful, the brick-blob-info will be put here.
        virtual void ReadBrickInfo(imgdoc2::dbIndex idx, imgdoc2::ITileCoordinateMutate* coordinate, imgdoc2::LogicalPositionInfo3D* info, imgdoc2::BrickBlobInfo* brick_blob_info) = 0;

        /// Query the tiles table. The two query clauses are used to filter the tiles. The first clause is used to filter the tiles by their
        /// coordinates, the second by other "per tile data". The functor is called for each tile which matches the query. If the functor
        /// returns false, the enumeration is canceled, and no more calls to the functor will occur anymore. The two query clauses are
        /// logically ANDed together.
        /// \param clause        The query clause (dealing with dimension indexes).
        /// \param tileInfoQuery The query clause (dealing with other "per tile data").
        /// \param func          A functor which we will be called, passing in the index of tiles matching the query. If the functor returns false, the enumeration is canceled, and no
        ///                      more calls to the functor will occur anymore.
        virtual void Query(const imgdoc2::IDimCoordinateQueryClause* clause, const imgdoc2::ITileInfoQueryClause* tileInfoQuery, const std::function<bool(imgdoc2::dbIndex)>& func) = 0;

        /// Gets tiles intersecting the specified cuboid (and satisfying the other criteria).
        /// \param  cuboid            The cuboid.
        /// \param  coordinate_clause The coordinate clause.
        /// \param  tileinfo_clause   The tileinfo clause.
        /// \param  func              A functor which we will be called, passing in the index of tiles matching the query. If the functor returns false, the enumeration is canceled, and no
        ///                           more calls to the functor will occur anymore.
        virtual void GetTilesIntersectingCuboid(const imgdoc2::CuboidD& cuboid, const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause, const std::function<bool(imgdoc2::dbIndex)>& func) = 0;

        /// Gets tiles intersecting with the specified plane (and satisfying the other criteria).
        ///
        /// \param   plane               The plane.
        /// \param   coordinate_clause   The coordinate clause.
        /// \param   tileinfo_clause     The tileinfo clause.
        /// \param   func                A functor which we will be called, passing in the index of tiles matching the query. If the functor returns false, the enumeration is canceled, and no
        ///                              more calls to the functor will occur any more.
        virtual void GetTilesIntersectingPlane(const imgdoc2::Plane_NormalAndDistD& plane, const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause, const std::function<bool(imgdoc2::dbIndex)>& func) = 0;

        // /// Reads the brick data for the specified brick.
        // /// \param          idx  The primary key of the brick for which the brick data is to be read.
        // /// \param [in]     data The object which is receiving the blob data.
        virtual void ReadBrickData(imgdoc2::dbIndex idx, imgdoc2::IBlobOutput* data) = 0;
    public:
        // no copy and no move (-> https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c21-if-you-define-or-delete-any-copy-move-or-destructor-function-define-or-delete-them-all )
        IDocQuery3d() = default;
        IDocQuery3d(const IDocQuery3d&) = delete;             // copy constructor
        IDocQuery3d& operator=(const IDocQuery3d&) = delete;  // copy assignment
        IDocQuery3d(IDocQuery3d&&) = delete;                  // move constructor
        IDocQuery3d& operator=(IDocQuery3d&&) = delete;       // move assignment
    };
}
