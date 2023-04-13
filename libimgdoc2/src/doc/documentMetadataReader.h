// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <memory>
#include <utility>
#include <string>
#include "IDocumentMetadata.h"
#include "document.h"
#include "documentMetadataBase.h"

/// Implementation of the IDocumentMetadataRead interface.
class DocumentMetadataReader : public DocumentMetadataBase, public imgdoc2::IDocumentMetadataRead
{
public:
    DocumentMetadataReader() = delete;
    explicit DocumentMetadataReader(std::shared_ptr<Document> document) : DocumentMetadataBase(std::move(document)) {}
    ~DocumentMetadataReader() override = default;

    imgdoc2::DocumentMetadataItem GetItem(imgdoc2::dbIndex primary_key, imgdoc2::DocumentMetadataItemFlags flags) override;
    imgdoc2::DocumentMetadataItem GetItemForPath(const std::string& path, imgdoc2::DocumentMetadataItemFlags flags) override;
    void EnumerateItems(
      std::optional<imgdoc2::dbIndex> parent,
      bool recursive,
      imgdoc2::DocumentMetadataItemFlags flags,
      const std::function<bool(imgdoc2::dbIndex, const imgdoc2::DocumentMetadataItem& item)>& func) override;
    void EnumerateItemsForPath(
      const std::string& path,
      bool recursive,
      imgdoc2::DocumentMetadataItemFlags flags,
      const std::function<bool(imgdoc2::dbIndex, const imgdoc2::DocumentMetadataItem& item)>& func) override;

private:
    void InternalEnumerateItems(
        std::optional<imgdoc2::dbIndex> parent,
        const std::string& path_of_parent,
        bool recursive,
        imgdoc2::DocumentMetadataItemFlags flags,
        const std::function<bool(imgdoc2::dbIndex, const imgdoc2::DocumentMetadataItem& item)>& func);

    std::shared_ptr<IDbStatement> CreateStatementForRetrievingItem(imgdoc2::DocumentMetadataItemFlags flags);

    /// Creates statement which gives the items for which the given item is an ancestor. If recursive is false, then
    /// only items for which the given item is the direct parent are returned. If recursive is true, then all items
    /// for which the given item is an ancestor are returned.
    /// The result of this statements gives in column 0 the primary key of the item, in column 1 the name of the item, in column 2 the type discriminator, 
    /// in column 3 the value double, in column 4 the value integer and in column 5 the value string.
    /// If the include_path flag is true, then the complete path of the item is included in the result as column 6.
    ///
    /// \param  recursive       True to query only items for the 'parent' is the direct ancestor, false to include all items (for which the 'parent' is direct or indirect ancestor).
    /// \param  include_path    True to include a sixth column containing the complete path of the item; false otherwise.
    /// \param  parent          The node to search for (being an ancestor). If nullopt, this means "root node".
    ///
    /// \returns    The statement.
    std::shared_ptr<IDbStatement> CreateStatementForEnumerateAllItemsWithAncestorAndDataBind(bool recursive, bool include_path, std::optional<imgdoc2::dbIndex> parent);

    /// Retrieves a document-metadata-item object from a statement. The precondition is that the statement has been executed and the result is ready.
    /// We expect at column 0 the primary key, column 1 the Name, at column 2 the TypeDiscriminator, at column 3 the ValueDouble, at column 4 the ValueInteger and
    /// at column 5 the ValueString. 
    ///
    /// \param  statement       The statement.
    /// \param  flags           The flags.
    /// \param  path_to_prepend Used only in case where 'flags' includes 'DocumentMetadataItemFlags::kCompletePath' - a path to be prepended
    ///                         (because the query gives only the path relative to the parent node).
    ///
    /// \returns    The document-metadata-item object populated with the information from the statement, as indicated by the flags.
    imgdoc2::DocumentMetadataItem RetrieveDocumentMetadataItemFromStatement(const std::shared_ptr<IDbStatement>& statement, imgdoc2::DocumentMetadataItemFlags flags, const string& path_to_prepend);

    /// Retrieve the full path for the specified node. The returned boolean indicates whether the path could be retrieved.
    /// If e.g. the node does not exist, the path is not set and the function returns false.
    ///
    /// \param          node_id Primary key of the node for which the path should be retrieved.
    /// \param [out]    path    If successful, the path for the specified node.
    ///
    /// \returns    True if it succeeds, false if it fails.
    bool GetPathForNode(imgdoc2::dbIndex node_id, std::string& path);
};
