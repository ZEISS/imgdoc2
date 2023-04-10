#pragma once

#include <memory>
#include "IDocumentMetadata.h"
#include "document.h"

class DocumentMetadataWriter : public imgdoc2::IDocumentMetadataWrite
{
private:
    std::shared_ptr<Document> document_;

    enum class DataType
    {
        null = 0,
        int32 = 1,
        uint32 = 2,
        doublefloat = 3,
        singlefloat = 4,
        utf8string = 5,
        json = 6,
    };
public:
    DocumentMetadataWriter(std::shared_ptr<Document> document) : document_(std::move(document)) {};
    virtual ~DocumentMetadataWriter() = default;

    imgdoc2::dbIndex AddItem(std::optional<imgdoc2::dbIndex> parent, std::string name, imgdoc2::DocumentMetadataType type, const imgdoc2::IDocumentMetadata::metadata_item_variant& value) override;

    void GetNodeFromPath(std::string path);
private:
    DataType DetermineDataType(imgdoc2::DocumentMetadataType type, const imgdoc2::IDocumentMetadata::metadata_item_variant& value);
};
