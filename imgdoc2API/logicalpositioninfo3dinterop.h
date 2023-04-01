// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#pragma pack(push, 4)
struct LogicalPositionInfo3DInterop
{
    double position_x;
    double position_y;
    double position_z;
    double width;
    double height;
    double depth;
    int pyramid_level;
};
#pragma pack(pop)
