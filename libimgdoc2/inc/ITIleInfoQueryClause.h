// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <functional>
#include <vector>
#include <cstdint>

namespace imgdoc2
{
    /// Values that represent "logical operator" which combines two comparison operations.
    enum class LogicalOperator : std::uint8_t
    {
        Invalid = 0,    ///< An enum constant representing the invalid option. This is only legal for the condition with index 0.
        And,            ///< An enum constant representing the AND option, i.e. this condition is combined with a logical AND with the previous condition.
        Or,             ///< An enum constant representing the OR option, i.e. this condition is combined with a logical OR with the previous condition.

        MaxValue = Or   ///< This must be equal to the largest numerical value in the enumeration.
    };

    /// Values that represent the comparison operation.
    enum class ComparisonOperation : std::uint8_t
    {
        Invalid = 0,            ///< An enum constant representing the invalid option.
        Equal,                  ///< An enum constant representing the "is equal to" comparison operation.
        NotEqual,               ///< An enum constant representing the "is not equal to" comparison operation.
        LessThan,               ///< An enum constant representing the "is less than" comparison operation.
        LessThanOrEqual,        ///< An enum constant representing the "is less than or equal" comparison operation.
        GreaterThan,            ///< An enum constant representing the "is greater than" comparison operation.
        GreaterThanOrEqual,     ///< An enum constant representing the "is greater than or equal" comparison operation.

        MaxValue = GreaterThanOrEqual  ///< This must be equal to the largest numerical value in the enumeration.
    };

    /// Interface defining the query-clause on "tile-info".
    class ITileInfoQueryClause
    {
    public:
        /// Gets a condition for the pyramid-level property. The conditions on this property are to be numbered 0 to n-1, where
        /// n is the number of conditions. If calling this method with an argument "no" larger than n-1, the method must return
        /// false. This method is required to be idempotent, if called multiple times (with same arguments) it must give the
        /// same result.
        /// Note that the logical operator of the condition with index 0 is unused.
        /// \param          no                      The 0-based index of the condition being queried.
        /// \param [out]    logical_operator        If non-null and the operation is successful, the logical operator is placed here.
        /// \param [out]    comparison_operation    If non-null and the operation is successful, the comparison operation is placed here.
        /// \param [out]    value                   If non-null and the operation is successful, the value for the comparison operation is placed here.
        /// \returns True if it succeeds, false if it fails.
        virtual bool GetPyramidLevelCondition(int no, LogicalOperator* logical_operator, ComparisonOperation* comparison_operation, int* value) const = 0;

        virtual ~ITileInfoQueryClause() = default;
    };
}
