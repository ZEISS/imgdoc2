#include "documentMetadataReader.h"


/*virtual*/imgdoc2::IDocumentMetadata::metadata_item_variant DocumentMetadataReader::GetItem(imgdoc2::dbIndex idx)
{
    throw runtime_error("DocumentMetadataReader::GetItem");
}
