// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <algorithm>
#include <map>
#include <vector>
#include "ITileCoordinate.h"

namespace imgdoc2
{
    /// A structure combining a dimension and a value.
    struct DimensionAndValue
    {
        imgdoc2::Dimension dimension;	///< The dimension.
        int value;			///< The value (for this dimension).
    };

    /// Implementation of "tile coordinate object".
    class TileCoordinate : public ITileCoordinateMutate
    {
    private:
        std::vector<DimensionAndValue> coordinates;
    public:
        /// Default constructor, this reserves size for 5 dimensions.
        TileCoordinate() : TileCoordinate(5) {}

        /// Constructor allowing to reserve size of the specified number of dimension. This is purely
        /// an opportunity for performance optimization, preventing a re-allocation should more dimensions
        /// be added than specified here.
        /// \param  reservedCapacity The reserved capacity for the number of dimensions.
        explicit TileCoordinate(size_t reservedCapacity)
        {
            this->coordinates.reserve(reservedCapacity);
        }

        /// Constructor initializing the object with the specified list.
        /// \param  list The initialization list.
        TileCoordinate(std::initializer_list<DimensionAndValue> list) : TileCoordinate(list.size())
        {
            for (auto d : list)
            {
                this->Set(d.dimension, d.value);
            }
        }

        void Set(imgdoc2::Dimension d, int value) override
        {
            const auto it = std::find_if(
                this->coordinates.begin(),
                this->coordinates.end(),
                [=](const DimensionAndValue& s) -> bool
                {
                    return s.dimension == d;
                });
            if (it != this->coordinates.end())
            {
                it->value = value;
            }
            else
            {
                this->coordinates.push_back(DimensionAndValue{ d, value });
            }
        }

        void Clear() override
        {
            this->coordinates.clear();
        }

    public:
        bool TryGetCoordinate(imgdoc2::Dimension dim, int* coordVal) const override
        {
            const auto it = std::find_if(this->coordinates.cbegin(), this->coordinates.cend(), [=](const DimensionAndValue& s) { return s.dimension == dim; });
            if (it != this->coordinates.cend())
            {
                if (coordVal != nullptr)
                {
                    *coordVal = it->value;
                }

                return true;
            }

            return false;
        }

        void EnumCoordinates(const std::function<bool(imgdoc2::Dimension, int)>& f) const override
        {
            for (auto it = this->coordinates.cbegin(); it != this->coordinates.cend(); ++it)
            {
                const bool b = f(it->dimension, it->value);
                if (b == false)
                {
                    break;
                }
            }
        }
    };
}
