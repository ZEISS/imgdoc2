// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once
#include "utilities.h"

namespace imgdoc2
{
    /// This structure defines the "logical position" of a tile in the 2D-document model.
    /// It is given by a (axis-aligned) rectangle (in the plane-pixel-coordinate-system) and
    /// the pyramid-level.
    struct LogicalPositionInfo
    {
        /// Default constructor, all properties are initialized to zero.
        LogicalPositionInfo() : posX(0), posY(0), width(0), height(0), pyrLvl(0)
        {}

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

        double posX;    ///< The x coordinate of the top left point.
        double posY;    ///< The y coordinate of the top left point.
        double width;   ///< The width.
        double height;  ///< The height.
        int pyrLvl;     ///< The pyramid level.

        /// Equality operator (where the coordinates are compared with an epsilon).
        /// \param  rhs The right hand side.
        /// \returns True if the parameters are considered equivalent.
        bool operator==(const LogicalPositionInfo& rhs) const
        {
            constexpr double DBLEPSILON = 1e-8;
            return this->pyrLvl == rhs.pyrLvl &&
                approximatelyEqual(this->posX, rhs.posX, DBLEPSILON) &&
                approximatelyEqual(this->posY, rhs.posY, DBLEPSILON) &&
                approximatelyEqual(this->width, rhs.width, DBLEPSILON) &&
                approximatelyEqual(this->height, rhs.height, DBLEPSILON);
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
        double posX;        ///< The x coordinate of the top left point.
        double posY;        ///< The y coordinate of the top left point.
        double posZ;        ///< The z coordinate of the top left point.
        double width;       ///< The width.
        double height;      ///< The height.
        double depth;       ///< The depth.
        int pyrLvl;         ///< The pyramid level.

        /// Equality operator (where the coordinates are compared with an epsilon).
        /// \param  rhs The right hand side.
        /// \returns True if the parameters are considered equivalent.
        bool operator==(const LogicalPositionInfo3D& rhs) const
        {
            constexpr double DBLEPSILON = 1e-8;
            return this->pyrLvl == rhs.pyrLvl &&
                approximatelyEqual(this->posX, rhs.posX, DBLEPSILON) &&
                approximatelyEqual(this->posY, rhs.posY, DBLEPSILON) &&
                approximatelyEqual(this->posZ, rhs.posZ, DBLEPSILON) &&
                approximatelyEqual(this->width, rhs.width, DBLEPSILON) &&
                approximatelyEqual(this->height, rhs.height, DBLEPSILON) &&
                approximatelyEqual(this->depth, rhs.depth, DBLEPSILON);
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
