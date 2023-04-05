// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interfaces
{
    using System;

    /// <summary>   
    /// This interface is used to give read-access to the document. 
    /// </summary>
    public interface IRead2d : IDisposable, IDocInfo2d, IDocQuery2d
    {
    }
}
