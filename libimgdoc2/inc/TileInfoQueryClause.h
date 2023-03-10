// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <vector>
#include <tuple>
#include "ITIleInfoQueryClause.h"

using namespace std;

namespace imgdoc2
{
    /// A simplistic implementation of the ITileInfoQueryClause interface.
    class CTileInfoQueryClause : public imgdoc2::ITileInfoQueryClause
    {
    private:
        std::vector<std::tuple< LogicalOperator, ComparisonOperation, int>> pyramid_level_conditionals;
    public:
        /// Default constructor.
        CTileInfoQueryClause() = default;

        //! @copydoc imgdoc2::ITileInfoQueryClause::GetPyramidLevelCondition(int,LogicalOperator*,ComparisonOperation*,int*) const
        bool GetPyramidLevelCondition(int no, LogicalOperator* logical_operator, ComparisonOperation* comparison_operation, int* value) const override
        {
            if (no >= this->pyramid_level_conditionals.size())
            {
                return false;
            }

            const auto& element = this->pyramid_level_conditionals.at(no);
            if (logical_operator != nullptr)
            {
                *logical_operator = get<0>(element);
            }

            if (comparison_operation != nullptr)
            {
                *comparison_operation = get<1>(element);
            }

            if (value != nullptr)
            {
                *value = get<2>(element);
            }

            return true;
        }

        /// Adds a condition for the pyramid level. This condition is added to the end of the internal list of conditions.
        /// \param  logical_operator            The logical operator.
        /// \param  comparison_operation        The comparison operation.
        /// \param  value                       The value.
        void AddPyramidLevelCondition(LogicalOperator logical_operator, ComparisonOperation comparison_operation, int value)
        {
            this->pyramid_level_conditionals.emplace_back(make_tuple(logical_operator, comparison_operation, value));
        }
    };
}
