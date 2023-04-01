// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#pragma pack(push, 4)
struct PlaneNormalAndDistanceInterop
{
    double normal_x;
    double normal_y;
    double normal_z;
    double distance;
};
#pragma pack(pop)
