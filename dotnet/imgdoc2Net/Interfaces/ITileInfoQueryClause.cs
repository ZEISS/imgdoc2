// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interfaces
{
    using System.Collections.Generic;

    /// <summary>  Interface defining the query-clause on "tile-info".</summary>
    public interface ITileInfoQueryClause
    {
        /// <summary> Gets the query clauses for the pyramid level.</summary>
        ///
        /// <value> The pyramid level conditions.</value>
        IEnumerable<QueryClause> PyramidLevelConditions { get; }
    }
}
