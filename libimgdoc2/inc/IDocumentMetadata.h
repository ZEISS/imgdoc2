// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <functional>
#include <limits>
#include <optional>
#include <type_traits>
#include "types.h"

namespace imgdoc2
{
    /// Values that represent the type of a document metadata item.
    enum class DocumentMetadataType : std::uint8_t
    {
        /// An enum constant representing the invalid option. This value is not legal for any call into the document metadata API.
        kInvalid = 0,

        /// An enum constant representing the default option.Default means that the type is determined from the value variant.
        /// The mapping is as follows: If the value variant is std::monostate, the type is set to Invalid. If the value variant is a string, the type is set to Text. 
        /// If the value variant is int32_t, the type is set to Int32. If the value variant is double, the type is set to Double. 
        kDefault,

        /// An enum constant representing the invalid option. This means that there is no value stored for this metadata item.
        kNull,

        /// An enum constant representing the 'text' option. The value variant must contain a string.
        kText,

        /// An enum constant representing the 'int32' option. The value variant must contain an int32.
        kInt32,

        /// An enum constant representing the 'JSON' option. The value variant must contain a string, and this string must be valid JSON.
        kJson,

        /// An enum constant representing the 'double' option. The value variant must contain a double.
        kDouble
    };

    /// Base interface for document metadata.
    class IDocumentMetadata
    {
    public:
        /// Defines an alias representing the metadata item variant. This variant can contain a string, an int32, a double or a std::monostate.
        typedef std::variant<std::string, std::int32_t, double, std::monostate> metadata_item_variant;

        virtual ~IDocumentMetadata() = default;

        // no copy and no move (-> https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c21-if-you-define-or-delete-any-copy-move-or-destructor-function-define-or-delete-them-all )
        IDocumentMetadata() = default;
        IDocumentMetadata(const IDocumentMetadata&) = delete;             // copy constructor
        IDocumentMetadata& operator=(const IDocumentMetadata&) = delete;  // copy assignment
        IDocumentMetadata(IDocumentMetadata&&) = delete;                  // move constructor
        IDocumentMetadata& operator=(IDocumentMetadata&&) = delete;       // move assignment
    };

    /// Values that represent different pieces of information that can be retrieved from a document metadata item.
    /// The values are a bitmask, so they can be combined using the bitwise OR operator.
    enum class DocumentMetadataItemFlags : std::uint8_t
    {
        None = 0,               ///< An enum constant representing the "none option", meaning that no information should be retrieved or is valid.
        kPrimaryKeyValid = 1,   ///< An enum constant representing the "primary key valid" option, meaning that the primary key is valid.
        kNameValid = 2,         ///< An enum constant representing the "name valid" option, meaning that the name is valid.
        kDocumentMetadataTypeAndValueValid = 4,  ///< An enum constant representing the "type and value valid" option, meaning that the "type and value" are to be retrieved or are valid.
        kCompletePath = 8,      ///< An enum constant representing the "complete path" option, meaning that the "complete path" is to be retrieved or is valid.

        kAll = kPrimaryKeyValid | kNameValid | kDocumentMetadataTypeAndValueValid,
        kAllWithCompletePath = kAll | kCompletePath
    };

    /// Bitwise 'or' operator for DocumentMetadataItemFlags. This is needed to be able to use the flags in a bitwise fashion.
    /// C.f. https://stackoverflow.com/a/34220050/522591.
    /// \param  x   A bit-field to process.
    /// \param  y   One or more bits to OR into the bit-field.
    /// \returns    The result of the operation.
    inline constexpr DocumentMetadataItemFlags operator|(DocumentMetadataItemFlags x, DocumentMetadataItemFlags y)
    {
        return static_cast<DocumentMetadataItemFlags>(static_cast<std::underlying_type_t<DocumentMetadataItemFlags>>(x) | static_cast<std::underlying_type_t<DocumentMetadataItemFlags>>(y));
    }

    /// Bitwise 'and' operator for DocumentMetadataItemFlags. This is needed to be able to use the flags in a bitwise fashion.
    /// C.f. https://stackoverflow.com/a/34220050/522591.
    /// \param  x   A bit-field to process.
    /// \param  y   A mask of bits to apply to the bit-field.
    /// \returns    The result of the operation.
    inline constexpr DocumentMetadataItemFlags operator&(DocumentMetadataItemFlags x, DocumentMetadataItemFlags y)
    {
        return static_cast<DocumentMetadataItemFlags>(static_cast<std::underlying_type_t<DocumentMetadataItemFlags>>(x) & static_cast<std::underlying_type_t<DocumentMetadataItemFlags>>(y));
    }

    /// Bitwise 'ones complement' operator for DocumentMetadataItemFlags. This is needed to be able to use the flags in a bitwise fashion.
    /// \param  x   The DocumentMetadataItemFlags to process.
    /// \returns    The result of the operation.
    inline constexpr DocumentMetadataItemFlags operator~(DocumentMetadataItemFlags x)
    {
        return static_cast<DocumentMetadataItemFlags>(~static_cast<std::underlying_type_t<DocumentMetadataItemFlags>>(x));
    }

    /// This structure is used to return information about a document metadata item. The flags field specifies which pieces of information are valid.
    struct DocumentMetadataItem
    {
        DocumentMetadataItemFlags flags{ DocumentMetadataItemFlags::None };             ///< The flags indicating which pieces of information are valid.
        imgdoc2::dbIndex primary_key{ std::numeric_limits<imgdoc2::dbIndex>::max() };   ///< The primary key of the metadata item. Check the flags field to see if this is valid.
        std::string name;                                                               ///< The name of the metadata item. Check the flags field to see if this is valid.
        std::string complete_path;                                                      ///< The complete path of the metadata item. Check the flags field to see if this is valid.
        DocumentMetadataType type{ DocumentMetadataType::kInvalid };                    ///< The type of the metadata item. Check the flags field to see if this is valid.
        IDocumentMetadata::metadata_item_variant value;                                 ///< The value of the metadata item. Check the flags field to see if this is valid.
    };

    /// The interface for read-only access to document metadata.
    class IDocumentMetadataRead : public IDocumentMetadata
    {
    public:
        ~IDocumentMetadataRead() override = default;

        /// Get the item identified by the specified key. The argument 'flags' specifies which pieces of information should be retrieved.
        /// Only the information specified in the flags can be expected to be valid in the returned DocumentMetadataItem.
        /// If the item does not exist, an exception of type imgdoc2::non_existing_item_exception is thrown.
        ///
        /// \param  primary_key     The key of the metadata item to be retrieved.
        /// \param  flags           The flags.
        ///
        /// \returns    The item.
        virtual imgdoc2::DocumentMetadataItem GetItem(imgdoc2::dbIndex primary_key, DocumentMetadataItemFlags flags) = 0;

        /// Get the item identified by the specified path. The argument 'flags' specifies which pieces of information should be retrieved.
        /// Only the information specified in the flags can be expected to be valid in the returned DocumentMetadataItem.
        /// If the path does not exist or is invalid, an exception of type imgdoc2::invalid_path_exception is thrown.
        ///
        /// \param  path    The path of the item to be retrieved.
        /// \param  flags   The flags.
        ///
        /// \returns    The item.
        virtual imgdoc2::DocumentMetadataItem GetItemForPath(const std::string& path, imgdoc2::DocumentMetadataItemFlags flags) = 0;

        /// Enumerate items for which the specified node 'parent' is the ancestor. If recursive is false, then only the direct children of the specified parent are enumerated.
        /// If recursive is true, then all descendants of the specified parent are enumerated.
        /// If the specified parent is not valid (nullopt), then all items are enumerated.
        /// IF the specified parent is valid, but does not exist, an exception of type imgdoc2::non_existing_item_exception is thrown.
        ///
        /// \param  parent      The parent node for which the children are to be enumerated. If nullopt, then all items are enumerated.
        /// \param  recursive   False to enumerate only the direct children of the specified parent, true to enumerate all descendants of the specified parent.
        /// \param  flags       The flags.
        /// \param  func        The items found are reported to this function. If it returns false, the enumeration is stopped.
        virtual void EnumerateItems(
            std::optional<imgdoc2::dbIndex> parent,
            bool recursive,
            DocumentMetadataItemFlags flags,
            const std::function<bool(imgdoc2::dbIndex, const DocumentMetadataItem& item)>& func) = 0;

        /// Enumerate items below the specified path. If recursive is false, then only the direct children of the specified path are enumerated.
        /// If recursive is true, then all descendants of the specified parent are enumerated.
        /// If the path is empty, then all items are enumerated.
        /// If the specified path does not exists, an exception of type imgdoc2::invalid_path_exception is thrown.
        ///
        /// \param  path        The path of the parent.
        /// \param  recursive   False to enumerate only the direct children of the specified parent, true to enumerate all descendants of the specified parent.
        /// \param  flags       The flags.
        /// \param  func        The items found are reported to this function. If it returns false, the enumeration is stopped.
        virtual void EnumerateItemsForPath(
            const std::string& path,
            bool recursive,
            DocumentMetadataItemFlags flags,
            const std::function<bool(imgdoc2::dbIndex, const DocumentMetadataItem& item)>& func) = 0;

        // no copy and no move (-> https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c21-if-you-define-or-delete-any-copy-move-or-destructor-function-define-or-delete-them-all )
        IDocumentMetadataRead() = default;
        IDocumentMetadataRead(const IDocumentMetadataRead&) = delete;             // copy constructor
        IDocumentMetadataRead& operator=(const IDocumentMetadataRead&) = delete;  // copy assignment
        IDocumentMetadataRead(IDocumentMetadataRead&&) = delete;                  // move constructor
        IDocumentMetadataRead& operator=(IDocumentMetadataRead&&) = delete;       // move assignment
    };

    /// The interface for write access to document metadata.
    class IDocumentMetadataWrite : public IDocumentMetadata
    {
    public:
        ~IDocumentMetadataWrite() override = default;

        /// This method updates or creates a node with the name as specified in the parameter 'name'
        /// as a child of the node specified by 'parent'. If 'create_node_if_not_exists' is true, the
        /// node with the name 'name' is created if it does not exist. If 'create_node_if_not_exists' is
        /// false, the mode is not created if it does not exist. In this case, the method throws an
        /// non_existing_item_exception.
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

        /// Updates or creates a node specified by the path 'path'. 'create_node_if_not_exists' controls if
        /// the node is created if it does not exist. This controls the behavior if the last element of the
        /// path. If 'create_path_if_not_exists' is true, all nodes in the path are created if they do not
        /// exist.
        ///
        /// \param  create_path_if_not_exists   True to create nodes in the path if they do not exist. This refers to all nodes in the path except the last one.
        /// \param  create_node_if_not_exists   True to create node if not exists. This refers to the last node in the path.
        /// \param  path                        The path of the node.
        /// \param  type                        The type of the node.
        /// \param  value                       The value of the node.
        ///
        /// \returns    The primary_key of the updated or created node.
        virtual imgdoc2::dbIndex UpdateOrCreateItemForPath(
            bool create_path_if_not_exists,
            bool create_node_if_not_exists,
            const std::string& path,
            DocumentMetadataType type,
            const IDocumentMetadata::metadata_item_variant& value) = 0;

        /// Deletes the item specified by 'primary_key'. If 'recursively' is true, all child nodes are
        /// also deleted. If 'recursively' is false, the node is only deleted if it has no child nodes.
        /// The method returns the number of deleted nodes - it does not throw an exception if the
        /// primary key does not exist or if the node has child nodes and 'recursively' is false.
        /// Note that the root node cannot be deleted - however, it is possible to delete all child nodes of the root node.
        /// 
        /// \param  primary_key Key of the node to be deleted. If this the optional has no value, this means "the root".
        /// \param  recursively True if all child nodes should be deleted, false if only the node itself should be deleted.
        ///
        /// \returns    The number of deleted nodes as a result by this call.
        virtual std::uint64_t DeleteItem(
                    std::optional<imgdoc2::dbIndex> primary_key,
                    bool recursively) = 0;

        /// Deletes the item specified by 'path'. If 'recursively' is true, all child nodes are
        /// also deleted. If 'recursively' is false, the node is only deleted if it has no child nodes.
        /// The method returns the number of deleted nodes - it does not throw an exception if the
        /// primary key does not exist or if the node has child nodes and 'recursively' is false.
        /// Note that the root node cannot be deleted - however, it is possible to delete all child nodes of the root node.
        /// 
        /// \param  path        The path of the node to be deleted. If this is an empty string, it identifies the "root".
        /// \param  recursively True if all child nodes should be deleted, false if only the node itself should be deleted.
        ///
        /// \returns    The number of deleted nodes as a result by this call.
        virtual std::uint64_t DeleteItemForPath(
                   const std::string& path,
                   bool recursively) = 0;

        // no copy and no move (-> https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c21-if-you-define-or-delete-any-copy-move-or-destructor-function-define-or-delete-them-all )
        IDocumentMetadataWrite() = default;
        IDocumentMetadataWrite(const IDocumentMetadataWrite&) = delete;             // copy constructor
        IDocumentMetadataWrite& operator=(const IDocumentMetadataWrite&) = delete;  // copy assignment
        IDocumentMetadataWrite(IDocumentMetadataWrite&&) = delete;                  // move constructor
        IDocumentMetadataWrite& operator=(IDocumentMetadataWrite&&) = delete;       // move assignment
    };
}
