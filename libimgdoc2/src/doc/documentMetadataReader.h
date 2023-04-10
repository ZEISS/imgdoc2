#pragma once

#include <memory>
#include "IDocumentMetadata.h"
#include "Document.h"
#include "documentMetadataBase.h"

class DocumentMetadataReader : public DocumentMetadataBase, public imgdoc2::IDocumentMetadataRead
{
private:
  //std::shared_ptr<Document> document_;
public:
  DocumentMetadataReader(std::shared_ptr<Document> document) : DocumentMetadataBase(std::move(document)) {}
  ~DocumentMetadataReader() override = default;

  //imgdoc2::IDocumentMetadata::metadata_item_variant GetItem(imgdoc2::dbIndex idx) override;
  imgdoc2::DocumentMetadataItem GetItem(imgdoc2::dbIndex idx, imgdoc2::DocumentMetadataItemFlags flags) override;
  imgdoc2::DocumentMetadataItem GetItemForPath(const std::string& path, imgdoc2::DocumentMetadataItemFlags flags) override;
  void EnumerateItems(
    imgdoc2::dbIndex parent,
    bool recursive,
    imgdoc2::DocumentMetadataItemFlags flags,
    std::function<bool(imgdoc2::dbIndex, const imgdoc2::DocumentMetadataItem& item)> callback) override;
  void EnumerateItems(
    const std::string& path,
    bool recursive,
    imgdoc2::DocumentMetadataItemFlags flags,
    std::function<bool(imgdoc2::dbIndex, const imgdoc2::DocumentMetadataItem& item)> callback) override;
};
