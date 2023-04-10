#include "documentMetadataWriter.h"

using namespace std;
using namespace imgdoc2;

static int DetermineDiscriminator(const imgdoc2::IDocumentMetadata::metadata_item_variant& value)
{
    if (std::holds_alternative<std::string>(value))
    {
        return 1;
    }
    else if (std::holds_alternative<int>(value))
    {
        return 2;
    }
    else if (std::holds_alternative<double>(value))
    {
        return 3;
    }
    //else if (std::holds_alternative<json>(value))
    //{
    //    return 4;
    //}
    else
    {
        throw std::invalid_argument("Unknown metadata item type");
    }
}

/*virtual*/imgdoc2::dbIndex DocumentMetadataWriter::AddItem(std::optional<imgdoc2::dbIndex> parent_id, std::string name, imgdoc2::DocumentMetadataType type, const imgdoc2::IDocumentMetadata::metadata_item_variant& value)
{
    ostringstream string_stream;
    string_stream << "INSERT INTO [" << "METADATA" << "] (" <<
        "[" << "Name" << "]," <<
        "[" << "AncestorId" << "]," <<
        "[" << "TypeDiscriminator" << "]," <<
        "[" << "ValueDouble" << "]," <<
        "[" << "ValueInteger" << "]," <<
        "[" << "ValueString" << "]," <<
        "[" << "ValueJson" << "]) " <<
        "VALUES(" << "?1, ?2, ?3, ?4, ?5, ?6, ?7" << "); ";

    int discriminator = DetermineDiscriminator(value);

    const auto statement = this->document_->GetDatabase_connection()->PrepareStatement(string_stream.str());
    int binding_index = 1;
    statement->BindString(binding_index++, name);
    if (parent_id.has_value())
    {
        statement->BindInt64(binding_index++, parent_id.value());
    }
    else
    {
        statement->BindNull(binding_index++);
    }

    statement->BindInt32(binding_index++, discriminator);
    if (discriminator == 3)
    {
        statement->BindDouble(binding_index++, std::get<double>(value));
    }
    else
    {
        statement->BindNull(binding_index++);
    }

    if (discriminator == 2)
    {
        statement->BindInt32(binding_index++, std::get<int>(value));
    }
    else
    {
        statement->BindNull(binding_index++);
    }

    if (discriminator == 1)
    {
        statement->BindString(binding_index++, std::get<std::string>(value));
    }
    else
    {
        statement->BindNull(binding_index++);
    }

    statement->BindNull(binding_index++);

    auto id = this->document_->GetDatabase_connection()->ExecuteAndGetLastRowId(statement.get());
    return id;
}

DocumentMetadataWriter::DataType DocumentMetadataWriter::DetermineDataType(imgdoc2::DocumentMetadataType type, const imgdoc2::IDocumentMetadata::metadata_item_variant& value)
{
    if (!std::holds_alternative<monostate>(value))
    {
        return DataType::null;
    }

    switch (type)
    {
        case DocumentMetadataType::Text:
            // in this case the value must contain a string
            if (!std::holds_alternative<std::string>(value))
            {
                throw std::invalid_argument("The value must be a string");
            }

            return DataType::utf8string;
        case DocumentMetadataType::Int32:
            // in this case the value must contain an integer
            if (!std::holds_alternative<int>(value))
            {
                throw std::invalid_argument("The value must be an integer");
            }

            return DataType::int32;
        case DocumentMetadataType::Double:
            // in this case the value must contain a double
            if (!std::holds_alternative<double>(value))
            {
                throw std::invalid_argument("The value must be a double");
            }

            return DataType::doublefloat;
        case DocumentMetadataType::Json:
            // in this case the value must contain a string
            if (!std::holds_alternative<string>(value))
            {
                throw std::invalid_argument("The value must be a string");
            }

            return DataType::json;
        case DocumentMetadataType::Default:
            if (!std::holds_alternative<std::string>(value))
            {
                return DataType::utf8string;
            }
            else if (!std::holds_alternative<int>(value))
            {
                return DataType::int32;
            }
            else if (!std::holds_alternative<double>(value))
            {
                return DataType::doublefloat;
            }
            else
            {
                throw std::invalid_argument("Unknown metadata item type");
            }
    }
}


/*
 with recursive paths(id, name, path, level) as (
    select Pk, Name, Name, 0 from METADATA where AncestorId is null AND Name='Node1'
    union
    select METADATA.Pk, METADATA.name, paths.path || '/' || METADATA.name, level+1
    from METADATA join paths where METADATA.AncestorId = paths.id AND 
         ((level = 0 AND METADATA.Name="Node1_1") OR
          (level = 1 AND METADATA.Name="Node1_1_2"))
)
select id, path, level from paths            
 */
