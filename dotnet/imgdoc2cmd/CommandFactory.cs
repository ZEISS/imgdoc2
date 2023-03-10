// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace Imgdoc2cmd
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;

    internal class CommandFactory
    {
        public static ICommand Create(Options options)
        {
            switch (options.Command)
            {
                case Options.CommandType.QueryForTilesAndReport:
                    return new CommandQueryForTilesAndReport();
                case Options.CommandType.QueryForTilesAndSaveTiles:
                    return new CommandQueryForTilesAndSaveTiles();
            }

            throw new ArgumentException("Unknown command");
        }
    }
}
