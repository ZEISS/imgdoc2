// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once
#include "IDocQuery3d.h"
#include "IDocInfo3d.h"

namespace imgdoc2
{
    /// This interface is providing read-only access to a 3D-document.
    class IDocRead3d : public imgdoc2::IDocQuery3d, public imgdoc2::IDocInfo3d
    {
    public:
        virtual ~IDocRead3d() = default;
    public:
        // no copy and no move (-> https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c21-if-you-define-or-delete-any-copy-move-or-destructor-function-define-or-delete-them-all )
        IDocRead3d() = default;
        IDocRead3d(const IDocRead3d&) = delete;             // copy constructor
        IDocRead3d& operator=(const IDocRead3d&) = delete;  // copy assignment
        IDocRead3d(IDocRead3d&&) = delete;                  // move constructor
        IDocRead3d& operator=(IDocRead3d&&) = delete;       // move assignment
    };
}
