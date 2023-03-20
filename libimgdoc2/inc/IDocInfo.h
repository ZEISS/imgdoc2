// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <vector>
#include <map>
#include <cstdint>
#include <limits>

namespace imgdoc2
{
    /// This structure gathers a minimum and maximum value. If minimum is greater than maximum, this means 
    /// that the bounds is invalid.
    struct CoordinateBounds
    {
        std::int32_t minimum_value{ std::numeric_limits<std::int32_t>::max() };  ///< The minimum value.
        std::int32_t maximum_value{ std::numeric_limits<std::int32_t>::min() };  ///< The maximum value.

        bool IsValid() const {
            return this->minimum_value <= this->maximum_value;
        }
    };

    /// This interface is used for retrieving information about the document.
    class IDocInfo
    {
    public:
        virtual ~IDocInfo() = default;

        /// Gets the tile dimensions used in the document.
        ///
        /// \param  dimensions  If non-null, the dimensions are put into the array pointed to.
        /// \param  count       On input, the size of the array 'dimension' (number of elements). On output, the number of tile dimensions available.
        virtual void GetTileDimensions(imgdoc2::Dimension* dimensions, std::uint32_t& count) = 0;

        /// Gets minimum and maximum value for specified tile dimension.
        ///
        /// \param  dimensions_to_query_for Vector containing the dimensions to be queried for.
        ///
        /// \returns    A map containing the min/max-information for the requested dimensions.
        virtual std::map<imgdoc2::Dimension, imgdoc2::CoordinateBounds> GetMinMaxForTileDimension(const std::vector<imgdoc2::Dimension>& dimensions_to_query_for) = 0;

    public:
        std::vector<imgdoc2::Dimension> GetTileDimensions()
        {
            std::uint32_t count = 0;
            this->GetTileDimensions(nullptr, count);
            std::vector<imgdoc2::Dimension> dimensions(count);
            this->GetTileDimensions(dimensions.data(), count);
            return dimensions;
        }
    };
}
