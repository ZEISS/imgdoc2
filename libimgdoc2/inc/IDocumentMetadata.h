#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <functional>
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

    enum class DocumentMetadataItemFlags : std::uint8_t
    {
        None = 0,
        PathValid = 1,
        DocumentMetadataTypeValid = 2,
        ValueValid = 4,
    };

    // -> https://stackoverflow.com/a/34220050/522591
    inline constexpr DocumentMetadataItemFlags operator|(DocumentMetadataItemFlags X, DocumentMetadataItemFlags Y)
    {
        return static_cast<DocumentMetadataItemFlags>(static_cast<unsigned int>(X) | static_cast<unsigned int>(Y));
    }

    inline DocumentMetadataItemFlags& operator|=(DocumentMetadataItemFlags& x, DocumentMetadataItemFlags y)
    {
        x = x | y;
        return x;
    }

    struct DocumentMetadataItem
    {
        DocumentMetadataItemFlags flags;
        std::string path;
        DocumentMetadataType type;
        IDocumentMetadata::metadata_item_variant value;
    };

    class IDocumentMetadataRead : public IDocumentMetadata
    {
    public:
        virtual ~IDocumentMetadataRead() = default;

        virtual imgdoc2::DocumentMetadataItem GetItem(imgdoc2::dbIndex idx, DocumentMetadataItemFlags flags) = 0;
        virtual imgdoc2::DocumentMetadataItem GetItemForPath(const std::string& path, imgdoc2::DocumentMetadataItemFlags flags) = 0;
        virtual void EnumerateItems(
            imgdoc2::dbIndex parent,
            bool recursive,
            DocumentMetadataItemFlags flags,
            std::function<bool(imgdoc2::dbIndex, const DocumentMetadataItem& item)> callback) = 0;
        virtual void EnumerateItems(
            const std::string& path,
            bool recursive,
            DocumentMetadataItemFlags flags,
            std::function<bool(imgdoc2::dbIndex, const DocumentMetadataItem& item)> callback) = 0;
    };

    class IDocumentMetadataWrite : public IDocumentMetadata
    {
    public:
        virtual ~IDocumentMetadataWrite() = default;

        /// This method updates or creates a node with the name as specified in the parameter 'name'
        /// as a child of the node specified by 'parent'. If 'create_node_if_not_exists' is true, the
        /// node with the name 'name' is created if it does not exist. If 'create_node_if_not_exists' is
        /// false, the mode is not created if it does not exist. In this case, the method throws an
        /// 'TODO: node_does_not_exist' exception.
        ///
        /// \param  parent                      The parent node. If this is std::nullopt, it identifies the root node.
        /// \param  create_node_if_not_exists   If true, the node is created if it does not exist.
        /// \param  name                        The name of the node to be updated or created.
        /// \param  type                        The type of the node.
        /// \param  value                       The value of the node.
        ///
        /// \returns    The primary_key of the updated or created node.
        virtual imgdoc2::dbIndex UpdateOrCreateItem(
                    std::optional<imgdoc2::dbIndex> parent,
                    bool create_node_if_not_exists,
                    const std::string& name,
                    DocumentMetadataType type,
                    const IDocumentMetadata::metadata_item_variant& value) = 0;

        virtual bool DeleteItem(
                    std::optional<imgdoc2::dbIndex> parent, 
                    bool recursively) = 0;
        virtual bool DeleteItemForPath(
                   const std::string& path,
                   bool recursively) = 0;
        virtual imgdoc2::dbIndex UpdateOrCreateItemForPath(
                    bool create_path_if_not_exists,
                    bool create_node_if_not_exists,
                    const std::string& path,
                    DocumentMetadataType type,
                    const IDocumentMetadata::metadata_item_variant& value) = 0;
    };
}
