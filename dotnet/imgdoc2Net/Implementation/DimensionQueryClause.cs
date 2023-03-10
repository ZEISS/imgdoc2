// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Implementation
{
    using System.Collections.Generic;
    using ImgDoc2Net.Interfaces;

    /// <summary> Implementation of the IDimensionQueryClause interface..</summary>
    public class DimensionQueryClause : IDimensionQueryClause
    {
        private readonly List<DimensionCondition> conditions = new List<DimensionCondition>();

        /// <summary> Adds a condition.</summary>
        ///
        /// <param name="dimensionCondition"> The dimension condition.</param>
        public void AddCondition(DimensionCondition dimensionCondition)
        {
            this.conditions.Add(dimensionCondition);
        }

        /// <inheritdoc/>
        public IEnumerable<DimensionCondition> EnumConditions()
        {
            return this.conditions;
        }
    }
}
