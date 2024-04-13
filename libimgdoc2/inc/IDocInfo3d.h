// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT
 
#pragma once

#include "IDocInfo.h"

namespace imgdoc2
{
    /// This interface is used for retrieving global information about the document (i.e. usually aggregated from
    /// the individual bricks) specific to the 3d-document.
    class IDocInfo3d : public IDocInfo
    {
    public:
        /// <summary>   Gets the extents of an axis-aligned bounding cuboid for all bricks. </summary>
        /// <param name="bounds_x"> [in,out] If non-null, the extent for the x-coordinate is put here. </param>
        /// <param name="bounds_y"> [in,out] If non-null, the extent for the y-coordinate is put here. </param>
        /// <param name="bounds_z"> [in,out] If non-null, the extent for the z-coordinate is put here. </param>
        virtual void GetBricksBoundingBox(imgdoc2::DoubleInterval* bounds_x, imgdoc2::DoubleInterval* bounds_y, imgdoc2::DoubleInterval* bounds_z) = 0;

        ~IDocInfo3d() override = default;

        // no copy and no move (-> https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c21-if-you-define-or-delete-any-copy-move-or-destructor-function-define-or-delete-them-all )
        IDocInfo3d() = default;
        IDocInfo3d(const IDocInfo3d&) = delete;             // copy constructor
        IDocInfo3d& operator=(const IDocInfo3d&) = delete;  // copy assignment
        IDocInfo3d(IDocInfo3d&&) = delete;                  // move constructor
        IDocInfo3d& operator=(IDocInfo3d&&) = delete;       // move assignment
    };
}
