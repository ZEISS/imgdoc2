#pragma once

#include <memory>
#include "IDocumentMetadata.h"
#include "Document.h"

class DocumentMetadataReader : public imgdoc2::IDocumentMetadataRead
{
private:
  std::shared_ptr<Document> document_;
public:
  DocumentMetadataReader(std::shared_ptr<Document> document) : document_(std::move(document)) {};
  virtual ~DocumentMetadataReader() = default;

  imgdoc2::IDocumentMetadata::metadata_item_variant GetItem(imgdoc2::dbIndex idx) override;

private:
    
};
