#include "documentMetadataReader.h"

using namespace std;
using namespace imgdoc2;

/*virtual*/imgdoc2::DocumentMetadataItem DocumentMetadataReader::GetItem(imgdoc2::dbIndex idx, imgdoc2::DocumentMetadataItemFlags flags)
{
    auto statement = this->CreateStatementForRetrievingItem(flags);
    statement->BindInt64(1, idx);

    DocumentMetadataItem item;


    return item;
    //throw runtime_error("DocumentMetadataReader::GetItem");
}

/*virtual*/imgdoc2::DocumentMetadataItem DocumentMetadataReader::GetItemForPath(const std::string& path, imgdoc2::DocumentMetadataItemFlags flags)
{
    imgdoc2::dbIndex idx;
    const bool success = this->TryMapPathAndGetTerminalNode(path, &idx);
    if (success)
    {
        return this->GetItem(idx, flags);
    }

    throw runtime_error("Error in DocumentMetadataReader::GetItemForPath");
}

void DocumentMetadataReader::EnumerateItems(
  imgdoc2::dbIndex parent,
  bool recursive,
  DocumentMetadataItemFlags flags,
  std::function<bool(imgdoc2::dbIndex, const DocumentMetadataItem& item)> callback)
{
    throw runtime_error("DocumentMetadataReader::EnumerateItems");
}

void DocumentMetadataReader::EnumerateItems(
  const std::string& path,
  bool recursive,
  DocumentMetadataItemFlags flags,
  std::function<bool(imgdoc2::dbIndex, const DocumentMetadataItem& item)> callback)
{
    throw runtime_error("DocumentMetadataReader::EnumerateItems");
}

std::shared_ptr<IDbStatement> DocumentMetadataReader::CreateStatementForRetrievingItem(imgdoc2::DocumentMetadataItemFlags flags)
{
    ostringstream string_stream;
    string_stream << "SELECT Name,TypeDiscriminator,ValueDouble,ValueInteger,ValueString FROM" << "METADATA" << "] WHERE " <<
        "[" << "Pk" << "] = ?1;";
    auto statement = this->document_->GetDatabase_connection()->PrepareStatement(string_stream.str());
    return statement;
}
