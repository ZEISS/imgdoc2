// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interfaces
{
    using System;
    using System.Collections.Generic;

    /// <summary>   
    /// This interface is used to give read-access to the document. 
    /// </summary>
    public interface IRead2d : IDisposable
    {
        /// <summary> Queries the tiles table.</summary>
        /// <param name="queryClause">         The query clause (dealing with dimension indexes).</param>
        /// <param name="tileInfoQueryClause"> The query clause (dealing with other "per tile data").</param>
        /// <param name="queryOptions">        (Optional) Options for controlling the operation.</param>
        /// <returns> List of tiles satisfying to the search criterion. In order to check whether the result is complete,
        ///           use the queryOptions argument.</returns>
        List<long> Query(IDimensionQueryClause queryClause, ITileInfoQueryClause tileInfoQueryClause, QueryOptions queryOptions = null);

        /// <summary> Reads tile data for the specified tile.</summary>
        /// <param name="key"> The primary key.</param>
        /// <returns> The tile data blob.</returns>
        byte[] ReadTileData(long key);

        /// <summary> Queries for tiles intersecting with the specified rectangle (and satisfying the other criteria).</summary>
        ///
        /// <param name="rectangle">           The rectangle.</param>
        /// <param name="queryClause">         The query clause (dealing with dimension indexes).</param>
        /// <param name="tileInfoQueryClause"> The query clause (dealing with other "per tile data").</param>
        /// <param name="queryOptions">        (Optional) Options for controlling the operation.</param>
        /// <returns> List of tiles satisfying to the search criterion. In order to check whether the result is complete,
        ///           use the queryOptions argument.</returns>
        List<long> QueryTilesIntersectingRect(Rectangle rectangle, IDimensionQueryClause queryClause, ITileInfoQueryClause tileInfoQueryClause, QueryOptions queryOptions = null);

        /// <summary> 
        /// Reads tile information for the specified tile. There are three pieces of information which can be retrieved by this method,
        /// namely the tile-coordinate, the logical position and the tile-blob-info.
        /// </summary>
        /// <remarks>
        /// Unfortunately, it seems Doxygen is unable to generate documentation for this ValueType-syntax 
        /// (-> https://github.com/doxygen/doxygen/issues/7633).
        /// </remarks>
        ///
        /// <param name="key"> The primary key.</param>
        ///
        /// <returns> The tile information.</returns>
        (ITileCoordinate coordinate, LogicalPosition logicalPosition, TileBlobInfo tileBlobInfo) ReadTileInfo(long key);

        /// <summary> 
        /// Reads tile-coordinate information for the specified tile.
        /// </summary>
        ///
        /// <param name="key"> The primary key.</param>
        ///
        /// <returns> The tile information.</returns>
        ITileCoordinate ReadTileCoordinate(long key);

        /// <summary> Reads logical position information for the specified tile..</summary>
        ///
        /// <param name="key"> The primary key.</param>
        ///
        /// <returns> The tile's logical position.</returns>
        LogicalPosition ReadTileLogicalPosition(long key);
    }
}
