// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <memory>
#include <utility>
#include <string>
#include <vector>
#include "IDocumentMetadata.h"
#include "documentMetadataBase.h"
#include "document.h"

class DocumentMetadataWriter : public DocumentMetadataBase, public imgdoc2::IDocumentMetadataWrite
{
public:
    DocumentMetadataWriter() = delete;
    explicit DocumentMetadataWriter(std::shared_ptr<Document> document) : DocumentMetadataBase(std::move(document)) {}
    ~DocumentMetadataWriter() override = default;

    imgdoc2::dbIndex UpdateOrCreateItem(
                   std::optional<imgdoc2::dbIndex> parent,
                   bool create_node_if_not_exists,
                   const std::string& name,
                   imgdoc2::DocumentMetadataType type,
                   const IDocumentMetadata::metadata_item_variant& value) override;
    std::uint64_t DeleteItem(
                std::optional<imgdoc2::dbIndex> primary_key,
                bool recursively) override;
    std::uint64_t DeleteItemForPath(
               const std::string& path,
               bool recursively) override;
    imgdoc2::dbIndex UpdateOrCreateItemForPath(
                bool create_path_if_not_exists,
                bool create_node_if_not_exists,
                const std::string& path,
                imgdoc2::DocumentMetadataType type,
                const IDocumentMetadata::metadata_item_variant& value) override;
private:
    void CheckNodeNameAndThrowIfInvalid(const std::string& name);
    std::shared_ptr<IDbStatement> CreateStatementForUpdateOrCreateItemAndBindData(bool create_node_if_not_exists, std::optional<imgdoc2::dbIndex> parent, const std::string& name,
                DatabaseDataTypeValue database_data_type_value,
                const IDocumentMetadata::metadata_item_variant& value);
    std::shared_ptr<IDbStatement> CreateQueryForNameAndAncestorIdStatement(const std::string& name, std::optional<imgdoc2::dbIndex> parent);

    void CreateMissingNodesOnPath(const std::vector<std::string_view>& path_parts, std::vector<imgdoc2::dbIndex>& pks_existing);

    std::shared_ptr<IDbStatement> CreateStatementForDeleteItemAndBindData(bool recursively, std::optional<imgdoc2::dbIndex> parent);
};
