#include "documentReadBase.h"
#include <algorithm>

using namespace std;
using namespace imgdoc2;

/*static*/void DocumentReadBase::GetEntityDimensionsInternal(const unordered_set<imgdoc2::Dimension>& tile_dimensions, imgdoc2::Dimension* dimensions, std::uint32_t& count)
{
    if (dimensions != nullptr)
    {
        copy_n(tile_dimensions.cbegin(), min(count, static_cast<uint32_t>(tile_dimensions.size())), dimensions);
    }

    count = static_cast<uint32_t>(tile_dimensions.size());
}
