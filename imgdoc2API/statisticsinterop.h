// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>

#pragma pack(push, 4)
struct ImgDoc2StatisticsInterop
{
    std::uint32_t number_of_createoptions_objects_active;
    std::uint32_t number_of_openexistingoptions_objects_active;
    std::uint32_t number_of_document_objects_active;
    std::uint32_t number_of_reader2d_objects_active;
    std::uint32_t number_of_writer2d_objects_active;
};
#pragma pack(pop)
