// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interfaces
{
    using System;

    /// <summary>   
    /// This interface is used to give read-access to the document3D. 
    /// </summary>
    public interface IRead3d : IDisposable, IDocInfo3d, IDocQuery3d
    {
    }
}