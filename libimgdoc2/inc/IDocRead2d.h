// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once
#include "IDocQuery2d.h"
#include "IDocInfo.h"

namespace imgdoc2
{
    /// This interface is providing read-only access to a 2D-document.
    class IDocRead2d : public IDocQuery2d, public IDocInfo
    {
    public:
        virtual ~IDocRead2d() = default;
    public:
        // no copy and no move (-> https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c21-if-you-define-or-delete-any-copy-move-or-destructor-function-define-or-delete-them-all )
        IDocRead2d() = default;
        IDocRead2d(const IDocRead2d&) = delete;             // copy constructor
        IDocRead2d& operator=(const IDocRead2d&) = delete;  // copy assignment
        IDocRead2d(IDocRead2d&&) = delete;                  // move constructor
        IDocRead2d& operator=(IDocRead2d&&) = delete;       // move assignment
    };
}
