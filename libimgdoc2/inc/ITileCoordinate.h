// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <functional>
#include <vector>
#include "types.h"

namespace imgdoc2
{
    /// Definition of the interface representing a "tile coordinate". This interface is immutable and does not
    /// allow to mutate the data.
    class ITileCoordinate
    {
    public:
        /// Attempts to get the value for the specified dimensions.
        /// \param          dim         The dimension to query.
        /// \param [in,out] coordVal    If non-null and the call is successful, the coordinate value is put here.
        /// \returns True if it succeeds, false if it fails.
        virtual bool TryGetCoordinate(imgdoc2::Dimension dim, int* coordVal) const = 0;

        /// Enum the dimensions and the coordinates represented by this object.
        /// \param  f A functor which is called for each item, passing in the dimension and the coordinate. If the functor
        ///           returns false, the enumeration is ended.
        virtual void EnumCoordinates(const std::function<bool(imgdoc2::Dimension, int)>& f) const = 0;

        virtual ~ITileCoordinate() = default;

        /// Enum the dimensions represented in this object.
        /// \param  f A functor which is called for each dimension. If the functor
        ///           returns false, the enumeration is ended.
        inline void EnumDimensions(const std::function<bool(imgdoc2::Dimension)>& f) const
        {
            this->EnumCoordinates([&](imgdoc2::Dimension d, int v)->bool {return f(d); });
        }

        /// Gets a vector with the dimensions contained in this object.
        /// \returns The dimensions.
        [[nodiscard]] std::vector<imgdoc2::Dimension> GetDimensions() const
        {
            std::vector<imgdoc2::Dimension> vec;
            this->EnumDimensions(
                [&vec](imgdoc2::Dimension d)->bool 
                {
                    vec.push_back(d); 
                    return true; 
                });

            return vec;
        }

        /// Queries if the specified value is a legal dimension. Legal dimensions are a-z and A-Z.
        /// \param  dimension The dimension.
        /// \returns True if the dimension is valid, false if not.
        [[nodiscard]] inline static bool IsValidDimension(imgdoc2::Dimension dimension)
        {
            return imgdoc2::IsDimensionValid(dimension);
        }
    };

    /// This interface provides modify access to the object and allows to mutate the object.
    class ITileCoordinateMutate : public ITileCoordinate
    {
    public:
        /// Clears this object to its blank/initial state.
        virtual void Clear() = 0;

        /// Sets the specified coordinate value for the specified dimension.
        /// \param  d     The dimension.
        /// \param  value The value.
        virtual void Set(imgdoc2::Dimension d, int value) = 0;

        ~ITileCoordinateMutate() override = default;
    };
}
