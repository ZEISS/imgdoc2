// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

namespace imgdoc2
{
    /// This interface is representing a blob, a piece of consecutive memory. It is used for passing in blob-data
    /// into libimgdoc2.
    class IDataObjBase
    {
    public:
        /// Gets pointer to the data and its size. Passing in nullptr is valid if not interested in the respective return value.
        /// \param          p If non-null, the address of the data is put here.
        /// \param [in,out] s If non-null, the size of the data is put here.
        virtual void GetData(const void** p, size_t* s) const = 0;

        virtual ~IDataObjBase() = default;
    };
}
