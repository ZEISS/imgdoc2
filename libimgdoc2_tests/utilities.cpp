#include "utilities.h"
#include <sstream>

using namespace std;

std::string GenerateUniqueSharedInMemoryFileNameForSqlite(const char* filename, int line_number)
{
    // the macro __FILE__ contains the full path to the file, here we remove everything before the last slash (or backslash)
    const size_t length_filename = strlen(filename);

    const char* filename_without_path = filename;
    if (length_filename > 0)
    {
        filename_without_path += length_filename - 1;
        for (; filename_without_path > filename; --filename_without_path)
        {
            if (*filename_without_path == '/' || *filename_without_path == '\\')
            {
                ++filename_without_path;
                break;
            }
        }
    }

    ostringstream string_stream;
    string_stream << "file:" << filename_without_path << line_number << "memdb?mode=memory&cache=shared";
    return string_stream.str();
}
