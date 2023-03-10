// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <vector>
#include <set>
#include "ITileCoordinate.h"
#include "types.h"

namespace imgdoc2
{
    /// This interface describes a query clause for the dimension-columns.
    /// It is important that this object is idempotent - meaning if the methods are 
    /// called multiple times, an implementation is required to give identical results.
    class IDimCoordinateQueryClause
    {
    public:
        /// A range clause means that the value must be greater than or equal to the
        /// 'start' field and less than or equal than the 'end'. Use int-min for start
        /// in order to have only a "less than or equal" comparison, and int-max for
        /// "greater or equal".
        struct RangeClause
        {
            int start;  ///< The start value of the range (inclusive).
            int end;    ///< The end value of the range (inclusive).
        };

        /// Gets the set of dimensions for which there is a clause present. It is important that the order
        /// of elements is idempotent (as of course the content itself).
        /// \returns The set of dimensions for which there are clauses.
        [[nodiscard]] virtual const std::set<imgdoc2::Dimension>& GetTileDimsForClause() const = 0;

        /// Gets range clauses for the specified dimension if they exist. Otherwise, null is returned.
        /// \param  d The dimension to query the range clauses for.
        /// \returns Null if it fails, else the range clauses.
        [[nodiscard]] virtual const std::vector<RangeClause>* GetRangeClause(imgdoc2::Dimension d) const = 0;

        virtual ~IDimCoordinateQueryClause() = default;
    };
}
