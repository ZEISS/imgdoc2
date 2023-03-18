#include "database_utilities.h"

using namespace imgdoc2;

/*static*/imgdoc2::DocumentType DbUtilities::GetDocumentTypeFromDocTypeField(const std::string& field)
{
    if (field == GetDocTypeValueForDocumentType(DocumentType::kImage2d))
    {
        return DocumentType::kImage2d;
    }
    else if (field == GetDocTypeValueForDocumentType(DocumentType::kImage3d))
    {
        return DocumentType::kImage3d;
    }
    else
    {
        return DocumentType::kInvalid;
    }
}

/*static*/const char* DbUtilities::GetDocTypeValueForDocumentType(imgdoc2::DocumentType document_type)
{
    switch (document_type)
    {
    case DocumentType::kImage2d:
        return "Tiles2D";
    case DocumentType::kImage3d:
        return "Bricks3D";
    default:
        return nullptr;
    }
}
