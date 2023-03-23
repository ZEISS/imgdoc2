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

        /// <summary>   Query if this object is valid. </summary>
        /// <returns>   True if valid, false if not. </returns>
        [[nodiscard]] bool IsValid() const
        {
            return this->minimum_value <= this->maximum_value;
        }

        /// <summary>   Equality operator. </summary>
        /// <remarks>   In case the both are 'not valid', then they are considered equal (irrespective of the actual values). </remarks>
        /// <param name="rhs">  The right hand side. </param>
        /// <returns>   True if the parameters are considered equivalent. </returns>
        bool operator==(const CoordinateBounds& rhs) const
        {
            if (this->minimum_value == rhs.minimum_value && this->maximum_value == rhs.maximum_value)
            {
                return true;
            }

            // if both are invalid, then we consider them as equal - otherwise, they are unequal
            return !this->IsValid() && !rhs.IsValid();
        }

        /// <summary>   Inequality operator. </summary>
        /// <remarks>   In case the both are 'not valid', then they are considered equal (irrespective of the actual values). </remarks>
        /// <param name="rhs">  The right hand side. </param>
        /// <returns>   True if the parameters are not considered equivalent. </returns>
        bool operator!=(const CoordinateBounds& rhs) const
        {
            return !(*this == rhs);
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
        /// If the minimum/maximum cannot be determined (for a dimension), then the result will be
        /// one where Minimum is greater than Maximum (=an invalid CoordinateBounds). This can happen
        /// e.g. if the document is empty, or the coordinates are Null.
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

        // no copy and no move (-> https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c21-if-you-define-or-delete-any-copy-move-or-destructor-function-define-or-delete-them-all )
        IDocInfo() = default;
        IDocInfo(const IDocInfo&) = delete;             // copy constructor
        IDocInfo& operator=(const IDocInfo&) = delete;  // copy assignment
        IDocInfo(IDocInfo&&) = delete;                  // move constructor
        IDocInfo& operator=(IDocInfo&&) = delete;       // move assignment
    };
}
