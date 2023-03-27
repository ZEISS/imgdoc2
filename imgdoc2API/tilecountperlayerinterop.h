// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT 

#pragma once

#include <cstdint>

#pragma pack(push, 4)

/// This struct gathers a pyramid-layer index and a count of tiles. It is used for interop.
struct PerLayerTileCountInterop
{
    std::int32_t layer_index;   ///< The pyramid layer index.
    std::uint64_t tile_count;   ///< The number of tiles on this layer.
};

/// This struct is used for interop with the 'IDocInfo_GetTileCountPerLayer'-API.
struct TileCountPerLayerInterop
{
    /// The number of elements in the array 'pyramid_layer_and_tile_count' - i.e. the number of elements
    /// for which space is allocated.
    std::uint32_t element_count_allocated;            

    /// On input, this number is not used. On output, it contains the number of available results. 
    /// This number may be larger than 'element_count_allocated', and if this is the case, it
    /// indicates that not all results could be returned.
    /// The number of valid items in 'pyramid_layer_and_tile_count' in any case is the minimum
    /// of 'element_count_allocated' and 'element_count_available'.
    std::uint32_t element_count_available;

    PerLayerTileCountInterop pyramid_layer_and_tile_count[];    ///< Array of elements.
};

#pragma pack(pop)
