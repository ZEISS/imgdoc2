// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

using ImgDoc2Net.Implementation;

namespace Imgdoc2cmd
{
    internal class Program
    {
        static int Main(string[] args)
        {
            Options options = new Options();

            try
            {
                if (!options.ParseCommandLineArguments(args))
                {
                    return 0;
                }

                if (options.Command == Options.CommandType.None)
                {
                    Console.Error.WriteLine("No command specified.");
                    return 1;
                }
            }
            catch (Exception exception)
            {
                Console.Error.WriteLine(exception.Message);
                return 10;
            }

            var command = CommandFactory.Create(options);

            command.Execute(options);

            return 0;
        }
    }
}