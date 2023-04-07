// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once
#include <limits>
#include "utilities.h"

namespace imgdoc2
{
    /// This structure defines the "logical position" of a tile in the 2D-document model.
    /// It is given by a (axis-aligned) rectangle (in the plane-pixel-coordinate-system) and
    /// the pyramid-level.
    struct LogicalPositionInfo
    {
        /// Default constructor, all properties are initialized to invalid values.
        LogicalPositionInfo() = default;

        /// Constructor initializing the rectangle and setting the pyramid-level to zero.
        /// \param  x The x coordinate of the top left point.
        /// \param  y The y coordinate of the top left point.
        /// \param  w The width.
        /// \param  h The height.
        LogicalPositionInfo(double x, double y, double w, double h) : posX(x), posY(y), width(w), height(h), pyrLvl(0)
        {}

        /// Constructor.
        /// \param  x The x coordinate of the top left point.
        /// \param  y The y coordinate of the top left point.
        /// \param  w The width.
        /// \param  h The height.
        /// \param  pyrLvl The pyramid level.
        LogicalPositionInfo(double x, double y, double w, double h, int pyrLvl) : posX(x), posY(y), width(w), height(h), pyrLvl(pyrLvl)
        {}

        double posX{ std::numeric_limits<double>::quiet_NaN() };    ///< The x coordinate of the top left point.
        double posY{ std::numeric_limits<double>::quiet_NaN() };    ///< The y coordinate of the top left point.
        double width{ std::numeric_limits<double>::quiet_NaN() };   ///< The width.
        double height{ std::numeric_limits<double>::quiet_NaN() };  ///< The height.
        int pyrLvl{ std::numeric_limits<int>::min() };              ///< The pyramid level.

        /// Equality operator (where the coordinates are compared with an epsilon).
        /// \param  rhs The right hand side.
        /// \returns True if the parameters are considered equivalent.
        bool operator==(const LogicalPositionInfo& rhs) const
        {
            constexpr double kDoubleEpsilon = 1e-8;
            return this->pyrLvl == rhs.pyrLvl &&
                approximatelyEqual(this->posX, rhs.posX, kDoubleEpsilon) &&
                approximatelyEqual(this->posY, rhs.posY, kDoubleEpsilon) &&
                approximatelyEqual(this->width, rhs.width, kDoubleEpsilon) &&
                approximatelyEqual(this->height, rhs.height, kDoubleEpsilon);
        }

        /// Inequality operator (where the coordinates are compared with an epsilon).
        /// \param  rhs The right hand side.
        /// \returns True if the parameters are not considered equivalent.
        bool operator!=(const LogicalPositionInfo& rhs) const
        {
            return !this->operator==(rhs);
        }
    };

    /// This structure defines the "logical position" of a tile in the 3D-document model.
    /// It is given by a (axis-aligned) cuboid (in the plane-pixel-coordinate-system) and
    /// the pyramid-level.
    struct LogicalPositionInfo3D
    {
        double posX{ std::numeric_limits<double>::quiet_NaN() };        ///< The x coordinate of the top left point.
        double posY{ std::numeric_limits<double>::quiet_NaN() };        ///< The y coordinate of the top left point.
        double posZ{ std::numeric_limits<double>::quiet_NaN() };        ///< The z coordinate of the top left point.
        double width{ std::numeric_limits<double>::quiet_NaN() };       ///< The width.
        double height{ std::numeric_limits<double>::quiet_NaN() };      ///< The height.
        double depth{ std::numeric_limits<double>::quiet_NaN() };       ///< The depth.
        int pyrLvl{ std::numeric_limits<int>::min() };                  ///< The pyramid level.

        /// Default constructor, all properties are initialized to invalid values.
        LogicalPositionInfo3D() = default;

        /// Constructor initializing the cuboid and setting the pyramid-level to invalid values.
        /// \param  x The x coordinate of the top left point.
        /// \param  y The y coordinate of the top left point.
        /// \param  z The z coordinate of the top left point.
        /// \param  w The width.
        /// \param  h The height.
        /// \param  d The depth.
        LogicalPositionInfo3D(double x, double y, double z, double w, double h, double d) : posX(x), posY(y), posZ(z), width(w), height(h), depth(d), pyrLvl(0)
        {}

        /// Constructor.
        /// \param  x The x coordinate of the top left point.
        /// \param  y The y coordinate of the top left point.
        /// \param  z The z coordinate of the top left point.
        /// \param  w The width.
        /// \param  h The height.
        /// \param  d The depth.
        /// \param  pyrLvl The pyramid level.
        LogicalPositionInfo3D(double x, double y, double z, double w, double h, double d, int pyrLvl) : posX(x), posY(y), posZ(z), width(w), height(h), depth(d), pyrLvl(pyrLvl)
        {}

        /// Equality operator (where the coordinates are compared with an epsilon).
        /// \param  rhs The right hand side.
        /// \returns True if the parameters are considered equivalent.
        bool operator==(const LogicalPositionInfo3D& rhs) const
        {
            constexpr double kDoubleEpsilon = 1e-8;
            return this->pyrLvl == rhs.pyrLvl &&
                imgdoc2::approximatelyEqual(this->posX, rhs.posX, kDoubleEpsilon) &&
                imgdoc2::approximatelyEqual(this->posY, rhs.posY, kDoubleEpsilon) &&
                imgdoc2::approximatelyEqual(this->posZ, rhs.posZ, kDoubleEpsilon) &&
                imgdoc2::approximatelyEqual(this->width, rhs.width, kDoubleEpsilon) &&
                imgdoc2::approximatelyEqual(this->height, rhs.height, kDoubleEpsilon) &&
                imgdoc2::approximatelyEqual(this->depth, rhs.depth, kDoubleEpsilon);
        }

        /// Inequality operator (where the coordinates are compared with an epsilon).
        /// \param  rhs The right hand side.
        /// \returns True if the parameters are not considered equivalent.
        bool operator!=(const LogicalPositionInfo3D& rhs) const
        {
            return !this->operator==(rhs);
        }
    };
}
