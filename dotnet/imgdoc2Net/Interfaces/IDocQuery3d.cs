// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interfaces
{
    using System;
    using System.Collections.Generic;

    public interface IDocQuery3d
    {
        /// <summary> Queries the bricks table.</summary>
        /// <param name="queryClause">         The query clause (dealing with dimension indexes).</param>
        /// <param name="tileInfoQueryClause"> The query clause (dealing with other "per tile data").</param>
        /// <param name="queryOptions">        (Optional) Options for controlling the operation.</param>
        /// <returns> List of bricks satisfying to the search criterion. In order to check whether the result is complete,
        ///           use the queryOptions argument.</returns>
        List<long> Query(IDimensionQueryClause queryClause, ITileInfoQueryClause tileInfoQueryClause, QueryOptions queryOptions = null);

        /// <summary> Reads brick data for the specified brick.</summary>
        /// <param name="key"> The primary key.</param>
        /// <returns> The brick data blob.</returns>
        byte[] ReadBrickData(long key);

        /// <summary> Queries for bricks intersecting with the specified cuboid (and satisfying the other criteria).</summary>
        ///
        /// <param name="cuboid">              The cuboid.</param>
        /// <param name="queryClause">         The query clause (dealing with dimension indexes).</param>
        /// <param name="tileInfoQueryClause"> The query clause (dealing with other "per tile data").</param>
        /// <param name="queryOptions">        (Optional) Options for controlling the operation.</param>
        /// <returns> List of bricks satisfying to the search criterion. In order to check whether the result is complete,
        ///           use the queryOptions argument.</returns>
        List<long> QueryTilesIntersectingRect(Cuboid cuboid, IDimensionQueryClause queryClause, ITileInfoQueryClause tileInfoQueryClause, QueryOptions queryOptions = null);

        /// <summary> Queries for bricks intersecting with the specified plane (and satisfying the other criteria).</summary>
        ///
        /// <param name="plane">               The plane.</param>
        /// <param name="queryClause">         The query clause (dealing with dimension indexes).</param>
        /// <param name="tileInfoQueryClause"> The query clause (dealing with other "per tile data").</param>
        /// <param name="queryOptions">        (Optional) Options for controlling the operation.</param>
        /// <returns> List of bricks satisfying to the search criterion. In order to check whether the result is complete,
        ///           use the queryOptions argument.</returns>
        List<long> QueryTilesIntersectingPlane(PlaneHesse plane, IDimensionQueryClause queryClause, ITileInfoQueryClause tileInfoQueryClause, QueryOptions queryOptions = null);

        /// <summary> 
        /// Reads brick information for the specified brick. There are three pieces of information which can be retrieved by this method,
        /// namely the tile-coordinate, the logical position and the tile-blob-info.
        /// </summary>
        ///
        /// <param name="key"> The primary key.</param>
        ///
        /// <returns> The brick information.</returns>
        (ITileCoordinate coordinate, LogicalPosition3d logicalPosition, BrickBlobInfo brickBlobInfo) ReadBrickInfo(long key);

        /// <summary> 
        /// Reads brick-coordinate information for the specified brick.
        /// </summary>
        ///
        /// <param name="key"> The primary key.</param>
        ///
        /// <returns> The brick coordinate information.</returns>
        ITileCoordinate ReadBrickCoordinate(long key);

        /// <summary> Reads logical position information for the specified brick.</summary>
        ///
        /// <param name="key"> The primary key.</param>
        ///
        /// <returns> The brick's logical position.</returns>
        LogicalPosition3d ReadBrickLogicalPosition(long key);
    }
}
