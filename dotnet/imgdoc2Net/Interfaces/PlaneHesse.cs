// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interfaces
{
    /// <summary> Parametrization of a 2D-plane in 3d-space in Hesse's normal form (https://en.wikipedia.org/wiki/Hesse_normal_form).</summary>
    public class PlaneHesse
    {
        /// <summary> The x component of the normal vector of the plane.</summary>
        public double NormalX { get; set; }

        /// <summary> The y component of the normal vector of the plane.</summary>
        public double NormalY { get; set; }

        /// <summary> The z component of the normal vector of the plane.</summary>
        public double NormalZ { get; set; }

        /// <summary> The distance of the plane to the origin.</summary>
        public double Distance { get; set; }
    }
}
