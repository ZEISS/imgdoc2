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
    }
}
