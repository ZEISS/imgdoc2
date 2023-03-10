// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include "commandlineoptions.h"
#include "CLI/App.hpp"
#include "CLI/Formatter.hpp"
#include "CLI/Config.hpp"
#include <map>

using namespace std;

CmdlineOpts::CmdlineOpts()
{
}

bool CmdlineOpts::ParseArguments(int argc, char** argv)
{
    CLI::App app{ "convczi" };

    // specify the string-to-enum-mapping for "verbosity"
    std::map<std::string, CmdlineOpts::AddMode> map_string_to_add_mode
    {
        { "transaction-per-tile", AddMode::TransactionPerTile},
        { "single-transaction", AddMode::SingleTransaction},
    };

    string source_filename;
    string destination_filename;
    AddMode add_mode;
    app.add_option("-s,--source", source_filename, "The source CZI-file to be converted.")
        ->check(CLI::ExistingFile)
        ->required();
    app.add_option("-o,--output", destination_filename, "The destination file.")
        ->required();
    app.add_option("-m,--mode", add_mode, "Choose between different modes (how the operation is ran).")
        ->default_val(AddMode::TransactionPerTile)
        ->transform(CLI::CheckedTransformer(map_string_to_add_mode, CLI::ignore_case));

    try
    {
        app.parse(argc, argv);
    }
    catch (const CLI::ParseError& e)
    {
        app.exit(e);
        return false;
    }

    this->source_czi_filename_ = source_filename;
    this->destination_filename_ = destination_filename;
    this->mode_ = add_mode;

    return true;
}

const std::string& CmdlineOpts::GetCziFilename() const
{
    return this->source_czi_filename_;
}

const std::string& CmdlineOpts::GetDstFilename() const
{
    return this->destination_filename_;
}

CmdlineOpts::AddMode CmdlineOpts::GetMode() const
{
    return this->mode_;
}
