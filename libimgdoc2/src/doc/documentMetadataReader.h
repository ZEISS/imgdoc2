#pragma once

#include <memory>
#include "IDocumentMetadata.h"
#include "document.h"
#include "documentMetadataBase.h"

class DocumentMetadataReader : public DocumentMetadataBase, public imgdoc2::IDocumentMetadataRead
{
public:
    DocumentMetadataReader(std::shared_ptr<Document> document) : DocumentMetadataBase(std::move(document)) {}
    ~DocumentMetadataReader() override = default;

    imgdoc2::DocumentMetadataItem GetItem(imgdoc2::dbIndex idx, imgdoc2::DocumentMetadataItemFlags flags) override;
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
    std::shared_ptr<IDbStatement> CreateStatementForRetrievingItem(imgdoc2::DocumentMetadataItemFlags flags);
    std::shared_ptr<IDbStatement> CreateStatementForEnumerateAllItemsWithAncestorAndDataBind(bool recursive, std::optional<imgdoc2::dbIndex> parent);

    /// Retrieves a document-metadata-item object from a statement. The precondition is that the statement has been executed and the result is ready.
    /// We expect at column 0 the primary key, column 1 the Name, at column 2 the TypeDiscriminator, at column 3 the ValueDouble, at column 4 the ValueInteger and
    /// at column 5 the ValueString. 
    ///
    /// \param  statement   The statement.
    /// \param  flags       The flags.
    ///
    /// \returns    The document-metadata-item object populated with the information from the statement, as indicated by the flags.
    imgdoc2::DocumentMetadataItem RetrieveDocumentMetadataItemFromStatement(const std::shared_ptr<IDbStatement>& statement, imgdoc2::DocumentMetadataItemFlags flags);
};
