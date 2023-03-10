// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interfaces
{
    /// <summary> Values that represent the comparison operation.</summary>
    public enum QueryComparisonOperator : byte
    {
        /// <summary> An enum constant representing the invalid option.</summary>
        Invalid = 0,

        /// <summary> An enum constant representing the "is equal to" comparison operation.</summary>
        Equal,

        /// <summary> An enum constant representing the "is not equal to" comparison operation.</summary>
        NotEqual,

        /// <summary> An enum constant representing the "is less than" comparison operation.</summary>
        LessThan,

        /// <summary> An enum constant representing the "is less than or equal" comparison operation.</summary>
        LessThanOrEqual,

        /// <summary> An enum constant representing the "is greater than" comparison operation.</summary>
        GreaterThan,

        /// <summary> An enum constant representing the "is greater than or equal" comparison operation.</summary>
        GreaterThanOrEqual,
    }
}
