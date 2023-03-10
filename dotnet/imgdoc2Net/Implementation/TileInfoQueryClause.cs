// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Implementation
{
    using System;
    using System.Collections.Generic;
    using System.Text;
    using ImgDoc2Net.Interfaces;

    /// <summary> Implementation of the ITileInfoQueryClause interface.</summary>
    public class TileInfoQueryClause : ITileInfoQueryClause
    {
        /// <summary> Gets the (mutable) pyramid level conditions list.</summary>
        ///
        /// <value> The (mutable) pyramid level conditions list.</value>
        public List<QueryClause> PyramidLevelConditionsModifiable { get; } = new List<QueryClause>();

        /// <inheritdoc/>
        public IEnumerable<QueryClause> PyramidLevelConditions => this.PyramidLevelConditionsModifiable;
    }
}
