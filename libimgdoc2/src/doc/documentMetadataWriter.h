#pragma once

#include <memory>
#include "IDocumentMetadata.h"
#include "documentMetadataBase.h"
#include "document.h"

class DocumentMetadataWriter : public DocumentMetadataBase, public imgdoc2::IDocumentMetadataWrite
{
private:
    //std::shared_ptr<Document> document_;

    /*enum class DataType
    {
        null = 0,
        int32 = 1,
        uint32 = 2,
        doublefloat = 3,
        singlefloat = 4,
        utf8string = 5,
        json = 6,
    };*/
public:
    DocumentMetadataWriter(std::shared_ptr<Document> document) : DocumentMetadataBase(std::move(document)) {}
    ~DocumentMetadataWriter() override = default;

    /*imgdoc2::dbIndex AddItem(std::optional<imgdoc2::dbIndex> parent, std::string name, imgdoc2::DocumentMetadataType type, const imgdoc2::IDocumentMetadata::metadata_item_variant& value) override;

    void GetNodeFromPath(std::string path);*/
    imgdoc2::dbIndex UpdateOrCreateItem(
                   std::optional<imgdoc2::dbIndex> parent,
                   bool create_node_if_not_exists,
                   const std::string& name,
                   imgdoc2::DocumentMetadataType type,
                   const IDocumentMetadata::metadata_item_variant& value) override;
    bool DeleteItem(
                std::optional<imgdoc2::dbIndex> parent,
                bool recursively) override;
    bool DeleteItemForPath(
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
    std::shared_ptr<IDbStatement> CreateStatementForUpdateOrCreateItem(bool create_node_if_not_exists, std::optional<imgdoc2::dbIndex> parent, const std::string& name,
                DatabaseDataTypeValue database_data_type_value,
                const IDocumentMetadata::metadata_item_variant& value);
    std::shared_ptr<IDbStatement> CreateQueryForNameAndAncestorIdStatement(const std::string& name, std::optional<imgdoc2::dbIndex> parent);
};
