#include "documentMetadataReader.h"

using namespace std;
using namespace imgdoc2;

/*virtual*/imgdoc2::DocumentMetadataItem DocumentMetadataReader::GetItem(imgdoc2::dbIndex idx, imgdoc2::DocumentMetadataItemFlags flags)
{
    throw runtime_error("DocumentMetadataReader::GetItem");
}

/*virtual*/imgdoc2::DocumentMetadataItem DocumentMetadataReader::GetItemForPath(const std::string& path, imgdoc2::DocumentMetadataItemFlags flags)
{
    auto pks_of_path = this->GetNodeIdsForPath(path);

    throw runtime_error("DocumentMetadataReader::GetItemForPath");
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
