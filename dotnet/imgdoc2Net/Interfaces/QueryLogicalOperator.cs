// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interfaces
{
    /// <summary> Values that represent "logical operator" which combines two comparison operations.</summary>
    public enum QueryLogicalOperator : byte
    {
        /// <summary> 
        /// An enum constant representing the invalid option. This is only legal for the condition with index 0.
        /// </summary>
        Invalid = 0,

        /// <summary> 
        /// An enum constant representing the AND option, i.e. this condition is combined with a logical AND with the previous condition.
        /// </summary>
        And,

        /// <summary> 
        /// An enum constant representing the OR option, i.e. this condition is combined with a logical OR with the previous condition.
        /// </summary>
        Or,
    }
}
