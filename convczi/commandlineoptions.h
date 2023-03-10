// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once
#include <string>

/// This class is responsible for parsing the command-line arguments
/// and providing access to the resulting information.
class CmdlineOpts
{
public:
    /// Values that represent "different modes of operation"
    /// (of how the data is added to the imgdoc2-document).
    enum class AddMode
    {
        TransactionPerTile, ///< Each tile added is within its own transaction.

        SingleTransaction   ///< One transaction for the whole operation.
    };
private:
    std::string source_czi_filename_;
    std::string destination_filename_;
    AddMode mode_{ AddMode::TransactionPerTile };
public:
    /// Default constructor.
    CmdlineOpts();

    /// Parse the command line arguments.
    /// \param      argc The number of argument.
    /// \param [in] argv The command line parameters.
    /// \returns True if the parsing was successful *and* the operation should continue; false otherwise.
    bool ParseArguments(int argc, char** argv);

    /// Gets the source CZI filename.
    /// \returns The souce CZI filename.
    const std::string& GetCziFilename() const;

    /// Gets the destination CZI filename.
    /// \returns The destination CZI filename.
    const std::string& GetDstFilename() const;

    AddMode GetMode() const;
};
