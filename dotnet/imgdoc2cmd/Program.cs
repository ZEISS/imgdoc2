// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

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