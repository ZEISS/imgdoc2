// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <vector>
#include <map>
#include <cstdint>
#include <limits>
#include "Intervals.h"

namespace imgdoc2
{
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
        /// one where Minimum is greater than Maximum (=an invalid Int32Interval). This can happen
        /// e.g. if the document is empty, or the coordinates are Null.
        ///
        /// \param  dimensions_to_query_for Vector containing the dimensions to be queried for.
        ///
        /// \returns    A map containing the min/max-information for the requested dimensions.
        virtual std::map<imgdoc2::Dimension, imgdoc2::Int32Interval> GetMinMaxForTileDimension(const std::vector<imgdoc2::Dimension>& dimensions_to_query_for) = 0;

        /// Gets the total number of tiles (or bricks) in the document.
        ///
        /// \returns The total tile count.
        virtual std::uint64_t GetTotalTileCount() = 0;

        /// Gets the total number of tile (or bricks) count per pyramid layer.
        ///
        /// \returns A map, where key is the pyramid layer number, and value is the total number of tiles (on this layer) in the document.
        virtual std::map<int, std::uint64_t> GetTileCountPerLayer() = 0;
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
