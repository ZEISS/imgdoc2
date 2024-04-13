// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

using Microsoft.VisualBasic.CompilerServices;

namespace Imgdoc2cmd
{
    using ImgDoc2Net.Implementation;
    using McMaster.Extensions.CommandLineUtils;
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Text.RegularExpressions;
    using System.Threading.Tasks;

    /// <summary> This object gathers the command line options.</summary>
    internal partial class Options
    {
        /// <summary> Values that represent the available commands.</summary>
        public enum CommandType
        {
            None,
            QueryForTilesAndReport,
            QueryForTilesAndSaveTiles,
            CreateSyntheticDocument,
            PrintInformation
        }

        /// <summary> Values that represent the "unit" in which the overlap is given.</summary>
        public enum OverlapUnit
        {
            /// <summary> An enum constant representing the unit 'pixel', i.e. the value gives the number of pixels for which the tiles overlap.</summary>
            Pixel,

            /// <summary> An enum constant representing the unit 'percentage', i.e. the value gives the overlap as a percentage of the width/height of a tile.</summary>
            Percentage
        }
    }

    internal partial class Options
    {
        private CommandType command;
        private string sourceDocument = string.Empty;
        private string dimensionQuery = string.Empty;
        private string destinationFolder = string.Empty;
        private string coordinateBounds = string.Empty;
        private string outputFilename = string.Empty;
        private (int tileWidth, int tileHeight) tileSize = (1024, 1024);
        private (int tilesColumnCount, int tilesRowCount) tilesRowsColumns = (1, 1);
        private float tilesOverlap = 0.0f;
        private OverlapUnit tilesOverlapUnit = OverlapUnit.Percentage;

        /// <summary> Gets the command.</summary>
        /// <value> The command.</value>
        public CommandType Command => this.command;

        /// <summary> Gets the filename of the source document.</summary>
        /// <value> The source document.</value>
        public string SourceDocument => this.sourceDocument;

        public string DimensionQuery => this.dimensionQuery;

        public string DestinationFolder => this.destinationFolder;

        public string CoordinateBounds => this.coordinateBounds;

        public string OutputFilename => this.outputFilename;

        public (int tileWidth, int tileHeight) TileSize => this.tileSize;

        public (int tilesColumnCount, int tilesRowCount) TilesRowsColumnsCount => this.tilesRowsColumns;

        /// <summary> Gets the value which determins the overlap of the tiles. The unit is given by "TilesOverlapUnit".</summary>
        /// <value> The tiles overlap.</value>
        public float TilesOverlap => this.tilesOverlap;

        /// <summary> Gets the 'unit' in which the tiles overlap is given.</summary>
        /// <value> The tiles overlap unit.</value>
        public OverlapUnit TilesOverlapUnit => this.tilesOverlapUnit;
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
            app.ExtendedHelpText = GetExtendedHelpText();

            var commandOption = app.Option("-c|--command <COMMAND>", $"The command to execute. Possible values are {GetListOfCommands()}.", CommandOptionType.SingleValue);
            commandOption.DefaultValue = "None";

            var sourceFileOption = app.Option("-s|--source <FILENAME>", "The source document to open", CommandOptionType.SingleValue);
            sourceFileOption.DefaultValue = string.Empty;

            var dimensionQueryOption = app.Option("-q|--dimension-query <QUERYSTRING>", "The dimension query clause, in the format e.g. \"A3,5B-3,1m9,10\".", CommandOptionType.SingleValue);
            dimensionQueryOption.DefaultValue = string.Empty;

            var destinationDirectoryOption = app.Option("-d|--destination-folder <FOLDER>", "Destination folder", CommandOptionType.SingleValue);
            destinationDirectoryOption.DefaultValue = string.Empty;

            var boundsOption = app.Option("-b|--coordinate-bounds <BOUNDS>", "The coordinate range, it is given in the format e.g. \"Z1T1-10\".", CommandOptionType.SingleValue);
            boundsOption.DefaultValue = string.Empty;

            var outputFileOption = app.Option("-o|--output-document <FILENAME>", "The output file", CommandOptionType.SingleValue);
            outputFileOption.DefaultValue = string.Empty;

            var tileSizeOption = app.Option("-t|--tile-size <TILESIZE>", "Specify the size of a tile in pixels. Format is 'width x height', e.g. '1024x1024'.", CommandOptionType.SingleValue);

            var tilesRowsColumns = app.Option("-r|--tiles-rows-columns <TILESROWSCOLUMNS>", "Specify the number of tiles in rows and columns. Format is 'rows x columns', e.g. '2x3'.", CommandOptionType.SingleValue);

            var tilesOverlapDefinition = app.Option("-v|--tiles-overlap <OVERLAP>", "Specify the overlap for tiling. The value can be given either in percentage ('%') or in pixels ('px'). ", CommandOptionType.SingleValue);

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

            this.coordinateBounds = boundsOption.Value() ?? string.Empty;

            this.outputFilename = outputFileOption.Value() ?? string.Empty;

            if (tileSizeOption.HasValue())
            {
                if (!TryParseTileSize(tileSizeOption.Value(), out this.tileSize))
                {
                    throw new ArgumentException($"Invalid <TILESIZE> given: {tileSizeOption.Value()}.");
                }
            }

            if (tilesRowsColumns.HasValue())
            {
                if (!TryParseTilesRowsColumns(tilesRowsColumns.Value(), out this.tilesRowsColumns))
                {
                    throw new ArgumentException($"Invalid <TILESROWSCOLUMNS> given: {tilesRowsColumns.Value()}.");
                }
            }

            if (tilesOverlapDefinition.HasValue())
            {
                if (!TryParseTilesOverlap(tilesOverlapDefinition.Value(), out this.tilesOverlap, out this.tilesOverlapUnit))
                {
                    throw new ArgumentException($"Invalid <OVERLAP> given: {tilesOverlapDefinition.Value()}.");
                }
            }

            return true;
        }

        private static bool TryParseCommandType(string text, out Options.CommandType command)
        {
            return Enum.TryParse<Options.CommandType>(text, true, out command);
        }

        private static bool TryParseTilesOverlap(string? text, out float overlap, out Options.OverlapUnit unit)
        {
            overlap = 0.0f;
            unit = Options.OverlapUnit.Percentage;
            if (text == null)
            {
                return false;
            }

            const string pattern = @"\s*(\d+(?:\.\d+)?)\s*(px|%)\s*";
            var m = Regex.Match(text, pattern);
            if (m.Success)
            {
                if (float.TryParse(m.Groups[1].Value, out overlap))
                {
                    if (m.Groups[2].Value == "%")
                    {
                        unit = Options.OverlapUnit.Percentage;
                    }
                    else if (m.Groups[2].Value == "px")
                    {
                        unit = Options.OverlapUnit.Pixel;
                    }

                    return true;
                }
            }

            return false;
        }

        /// <summary> Attempts to parse a string in the format '&lt;integer&gt; x &lt;integer&gt;' from the given string.</summary>
        /// <param name="text">     The string to parse.</param>
        /// <param name="tileSize"> The two integers.</param>
        /// <returns> True if it succeeds, false if it fails.</returns>
        private static bool TryParseTileSize(string? text, out (int, int) tileSize)
        {
            return TryParseIntxInt(text, out tileSize);
        }

        private static bool TryParseTilesRowsColumns(string? text, out (int, int) rowsColumnsCount)
        {
            return TryParseIntxInt(text, out rowsColumnsCount);
        }

        private static bool TryParseIntxInt(string? text, out (int, int) firstSecond)
        {
            firstSecond = (0, 0);
            if (text == null)
            {
                return false;
            }

            const string pattern = @"\s*(\d+)\s*x\s*(\d+)";
            var m = Regex.Match(text, pattern);
            if (m.Success)
            {
                return int.TryParse(m.Groups[1].Value, out firstSecond.Item1) && int.TryParse(m.Groups[2].Value, out firstSecond.Item2);
            }

            return false;

        }

        private static string GetListOfCommands()
        {
            // TODO(JBL): we could create this list programmatically
            return "PrintInformation, QueryForTilesAndReport, QueryForTilesAndSaveTiles, CreateSyntheticDocument";
        }

        private static string GetExtendedHelpText()
        {
            string text = @"
command ""CreateSyntheticDocument"":
 This command will create a new document with the filename as given with the '-o|--output-document' option.
 The bounds for the document is specified with the '-b|--coordinate-bounds' option. The size of the individual
 tiles can be set with the '-t|--tile-size' option. It is also possible to specify the number of rows and columns
 for a checker-board arrangement of tiles in a plane with the '-r|--tiles-rows-columns' option. In this case,
 the overlap between tiles can be specified with the '-v|--tiles-overlap' option.

command ""QueryForTilesAndReport"":
 This command will use the query specified with the '-q|--dimension-query' option and print information (about all matching tiles).

command ""QueryForTilesAndSaveTiles"":
 This command will use the query specified with the '-q|--dimension-query' option and save matching tiles (as PNG) to the folder specified with the '-d|--destination-folder' option.

command ""PrintInformation"":
 Use this command to print information about the document.
";


            var versionInfo = ImgDoc2Global.GetVersionInfo();

            text +=
                $"\n\nversion of native imgdoc2:\nversion: {versionInfo.NativeLibraryVersion.Major}.{versionInfo.NativeLibraryVersion.Minor}.{versionInfo.NativeLibraryVersion.Patch}\n";
            text += $"compiler: {versionInfo.NativeLibraryVersion.CompilerIdentification}\n";
            text += $"build type: {versionInfo.NativeLibraryVersion.BuildType}\n";
            text += $"repository: {versionInfo.NativeLibraryVersion.RepositoryUrl}\n";
            text += $"branch: {versionInfo.NativeLibraryVersion.RepositoryBranch}\n";
            text += $"tag: {versionInfo.NativeLibraryVersion.RepositoryTag}\n";
            text += $"\n\nversion of managed imgdoc2 library:\nversion: {versionInfo.ManagedImgDoc2LibraryVersionInfo.Major}.{versionInfo.ManagedImgDoc2LibraryVersionInfo.Minor}.{versionInfo.ManagedImgDoc2LibraryVersionInfo.Patch}.{versionInfo.ManagedImgDoc2LibraryVersionInfo.Revision}\n";
            text += $"build type: {versionInfo.ManagedImgDoc2LibraryVersionInfo.BuildType}\n";

            return text;
        }
    }
}
