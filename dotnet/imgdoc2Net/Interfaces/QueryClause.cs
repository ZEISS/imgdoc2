// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interfaces
{
    /// <summary> This defines a query clause.</summary>
    public struct QueryClause
    {
        /// <summary> Gets or sets the logical operator.</summary>
        ///
        /// <value> The logical operator.</value>
        public QueryLogicalOperator LogicalOperator { get; set; }

        /// <summary> Gets or sets the comparison operator.</summary>
        ///
        /// <value> The comparison operator.</value>
        public QueryComparisonOperator ComparisonOperator { get; set; }

        /// <summary> Gets or sets the value.</summary>
        ///
        /// <value> The value.</value>
        public int Value { get; set; }
    }
}
