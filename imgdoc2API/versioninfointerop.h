// SPDX-FileCopyrightText: 2024 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include "allocationobject.h"

#pragma pack(push, 4)
struct VersionInfoInterop
{
    int major;
    int minor;
    int revision;

    AllocationObject compiler_identification;
    AllocationObject build_type;
    AllocationObject repository_url;
    AllocationObject repository_branch;
    AllocationObject repository_tag;
};
#pragma pack(pop)
