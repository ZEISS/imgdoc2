// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include "IDimCoordinateQueryClause.h"
#include <vector>
#include <map>
#include <set>

namespace imgdoc2
{
  /// A simple implementation of the IDimCoordinateQueryClause interface.
  class CDimCoordinateQueryClause : public IDimCoordinateQueryClause
  {
  private:
    std::map<imgdoc2::Dimension, std::vector<RangeClause>> rangeClauses;
    std::set<imgdoc2::Dimension> dims;
  public:
    /// Adds a range clause for the specified dimension.
    /// \param  d      The dimension.
    /// \param  clause The clause.
    void AddRangeClause(imgdoc2::Dimension d, const RangeClause& clause)
    {
      this->rangeClauses[d].push_back(clause);
      this->dims.insert(d);
    }

    //! @copydoc imgdoc2::IDimCoordinateQueryClause::GetTileDimsForClause() const
    const std::set<imgdoc2::Dimension>& GetTileDimsForClause() const override
    {
      return this->dims;
    }

    //! @copydoc imgdoc2::IDimCoordinateQueryClause::GetRangeClause(imgdoc2::Dimension) const
    const std::vector<RangeClause>* GetRangeClause(imgdoc2::Dimension d) const override
    {
      const auto& c = this->rangeClauses.find(d);
      if (c != this->rangeClauses.cend())
      {
        return &c->second;
      }

      return nullptr;
    }
  };
}
