#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <optional>
#include "types.h"

namespace imgdoc2
{

    enum class DocumentMetadataType : std::uint8_t
    {
        Default,
        Text,
        Int32,
        Json,
        Double
    };

    class IDocumentMetadata
    {
    public:
        typedef std::variant<std::string, std::int32_t, double, std::monostate> metadata_item_variant;
    };

    class IDocumentMetadataRead : public IDocumentMetadata
    {
    public:
        virtual ~IDocumentMetadataRead() = default;

        virtual IDocumentMetadata::metadata_item_variant GetItem(imgdoc2::dbIndex idx) = 0;
    };

    class IDocumentMetadataWrite : public IDocumentMetadata
    {
    public:
        virtual ~IDocumentMetadataWrite() = default;

        virtual imgdoc2::dbIndex AddItem(
                    std::optional<imgdoc2::dbIndex> parent, 
                    std::string name,
                    DocumentMetadataType type,
                    const IDocumentMetadata::metadata_item_variant& value) = 0;
    };
}
