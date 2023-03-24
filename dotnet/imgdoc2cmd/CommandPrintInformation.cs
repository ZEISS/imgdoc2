// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace Imgdoc2cmd
{
    using System.Text;
    using SixLabors.Fonts;
    using SixLabors.ImageSharp;
    using SixLabors.ImageSharp.Drawing.Processing;
    using SixLabors.ImageSharp.Formats;
    using SixLabors.ImageSharp.PixelFormats;
    using SixLabors.ImageSharp.Processing;
    using ImgDoc2Net.Implementation;
    using ImgDoc2Net.Interfaces;

    /// <summary> 
    /// Implementation of the command "PrintInformation".
    /// This is intended to print all kind of global information about the document.
    /// </summary>
    internal partial class CommandPrintInformation : ICommand
    {
        public void Execute(Options options)
        {
            using var commandHelper = new CommandHelper();
            commandHelper.OpenSourceDocument(options.SourceDocument);

            var reader2d = commandHelper.Document.Get2dReader();

            Console.WriteLine($"Information on file {options.SourceDocument}:");
            Console.WriteLine();

            var boundingBox = reader2d.GetBoundingBox();
            Console.WriteLine($"Bounding-box:     {FormatBoundingBox(boundingBox)}");

            var dimensions = reader2d.GetTileDimensions();
            Console.WriteLine($"Tile-dimensions:  {FormatTileDimensions(dimensions)}");

            var minMax = reader2d.GetMinMaxForTileDimension(dimensions);
            Console.WriteLine($"range:            {FormatMinMax(minMax)}");
        }
    }

    internal partial class CommandPrintInformation
    {
        private static string FormatBoundingBox(Extent2d boundingBox)
        {
            if (boundingBox.IsValid)
            {
                return $"({boundingBox.MinX}, {boundingBox.MinY}) - ({boundingBox.MaxX}, {boundingBox.MaxY})";
            }

            return "<invalid>";
        }

        private static string FormatTileDimensions(IEnumerable<Dimension> dimensions)
        {
            StringBuilder sb = new StringBuilder(100);
            sb.AppendJoin(", ", dimensions.Select(d => d.Id));
            return sb.ToString();
        }

        private static string FormatMinMax(Dictionary<Dimension, (int Minimum, int Maximum)> minMaxForDimensions)
        {
            StringBuilder sb = new StringBuilder(100);
            foreach (var minMaxForDimension in minMaxForDimensions)
            {
                sb.AppendFormat("{0}{1}-{2}", minMaxForDimension.Key.Id, minMaxForDimension.Value.Minimum, minMaxForDimension.Value.Maximum);
            }

            return sb.ToString();
        }
    }
}
