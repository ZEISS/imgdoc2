// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <string>
#include <imgdoc2.h> 

class DbUtilities
{
public:
    static imgdoc2::DocumentType GetDocumentTypeFromDocTypeField(const std::string& field);
    static const char* GetDocTypeValueForDocumentType(imgdoc2::DocumentType document_type);
};
