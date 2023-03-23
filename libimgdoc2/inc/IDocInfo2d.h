#pragma once

#include "IDocInfo.h"

namespace imgdoc2
{
    /// This interface is used for retrieving global information (about the document (i.e. usually aggregated from
    /// the individual tiles) specific to the 2d-document.
    class IDocInfo2d : public IDocInfo
    {
    public:
        /// <summary>   Gets the extents of an axis-aligned bounding box for all tiles. </summary>
        /// <param name="bounds_x"> [in,out] If non-null, the extent for the x-coordinate is put here. </param>
        /// <param name="bounds_y"> [in,out] If non-null, the extent for the y-coordinate is put here. </param>
        virtual void GetTilesBoundingBox(imgdoc2::DoubleInterval* bounds_x, imgdoc2::DoubleInterval* bounds_y) = 0;

        ~IDocInfo2d() override = default;

        // no copy and no move (-> https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c21-if-you-define-or-delete-any-copy-move-or-destructor-function-define-or-delete-them-all )
        IDocInfo2d() = default;
        IDocInfo2d(const IDocInfo2d&) = delete;             // copy constructor
        IDocInfo2d& operator=(const IDocInfo2d&) = delete;  // copy assignment
        IDocInfo2d(IDocInfo2d&&) = delete;                  // move constructor
        IDocInfo2d& operator=(IDocInfo2d&&) = delete;       // move assignment
    };
}
