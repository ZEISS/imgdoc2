// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace Imgdoc2cmd
{
    using McMaster.Extensions.CommandLineUtils;
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;

    internal partial class Options
    {
        public enum CommandType
        {
            None,
            QueryForTilesAndReport,
            QueryForTilesAndSaveTiles
        }
    }

    internal partial class Options
    {
        private CommandType command;
        private string sourceDocument = string.Empty;
        private string dimensionQuery = string.Empty;
        private string destinationFolder = string.Empty;

        public CommandType Command => this.command;
        public string SourceDocument => this.sourceDocument;
        public string DimensionQuery => this.dimensionQuery;
        public string DestinationFolder => this.destinationFolder;
    }

    internal partial class Options
    {
        /// <summary> 
        /// Parse the command line arguments. If an error is encountered (e.g. the arguments are 
        /// determined to be invalid), then an exception is thrown. If the return value is true,
        /// then operation should continue (with the options provided then). If the return value
        /// is false, then the operation should not continue and terminate immediately. This is
        /// e.g. the case if the help text was requested, where the required actions are done
        /// inside this method.
        /// </summary>
        ///
        /// <exception cref="ArgumentException"> Thrown when one or more arguments have unsupported or
        ///     illegal values.</exception>
        ///
        /// <param name="arguments"> The command line arguments.</param>
        ///
        /// <returns> True if it succeeds and the program's operation should continue; false if there is nothing to do.</returns>
        public bool ParseCommandLineArguments(string[] arguments)
        {
            var app = new CommandLineApplication();

            app.HelpOption();

            var commandOption = app.Option("-c|--command <COMMAND>", "The command to execute", CommandOptionType.SingleValue);
            commandOption.DefaultValue = "None";

            var sourceFileOption = app.Option("-s|--source <FILENAME>", "The source document to open", CommandOptionType.SingleValue);
            sourceFileOption.DefaultValue = string.Empty;

            var dimensionQueryOption = app.Option("-q|--dimension-query <QUERYSTRING>", "The dimension query", CommandOptionType.SingleValue);
            dimensionQueryOption.DefaultValue = string.Empty;

            var destinationDirectoryOption = app.Option("-d|--destination-folder <FOLDER>", "Destination folder", CommandOptionType.SingleValue);
            destinationDirectoryOption.DefaultValue = string.Empty;

            app.OnExecute(() => { });

            app.Execute(arguments);

            if (app.OptionHelp?.HasValue() ?? false)
            {
                return false;
            }

            if (!TryParseCommandType(commandOption.Value() ?? string.Empty, out this.command))
            {
                throw new ArgumentException($"Invalid 'command' argument given -> {commandOption.Value() ?? string.Empty}.");
            }

            if (sourceFileOption.HasValue())
            {
                this.sourceDocument = sourceFileOption.Value() ?? string.Empty;
            }

            this.dimensionQuery = dimensionQueryOption.Value()?.Trim() ?? string.Empty;

            this.destinationFolder = destinationDirectoryOption.Value() ?? string.Empty;

            return true;
        }

        private static bool TryParseCommandType(string text, out Options.CommandType command)
        {
            return Enum.TryParse<Options.CommandType>(text, true, out command);
        }
    }
}
