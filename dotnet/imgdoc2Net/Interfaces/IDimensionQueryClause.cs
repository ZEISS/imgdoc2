// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interfaces
{
    using System.Collections.Generic;
    using ImgDoc2Net.Implementation;

    /// <summary>  This interface describes a query clause for the dimension-columns. </summary>
    public interface IDimensionQueryClause
    {
        /// <summary> Enumerates the dimension clauses.</summary>
        ///
        /// <returns> An enumerator that allows foreach to be used to process the dimension clauses collection.</returns>
        IEnumerable<DimensionCondition> EnumConditions();
    }
}
