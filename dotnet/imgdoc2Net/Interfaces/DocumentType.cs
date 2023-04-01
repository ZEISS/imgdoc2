// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interfaces
{
    /// <summary> 
    /// Values that represent document types.
    /// This enumeration must be in sync with the unmanaged type "DocumentType" in imgdoc2.h.
    /// </summary>
    public enum DocumentType : byte
    {
        Invalid = 0,

        Image2d,

        Image3d,
    }
}
