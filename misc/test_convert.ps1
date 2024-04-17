# SPDX-FileCopyrightText: 2024 Carl Zeiss Microscopy GmbH
#
# SPDX-License-Identifier: MIT

# Note: The scripts in this folder are work-in-progress and contain absolute paths. 
#       At this point, they are not intended to be used as-is.

# Define the path to the executable
$executablePath = "D:\dev\MyGitHub\ptahmose_imgdoc2\out\build\x64-Debug\convczi\convczi.exe"

# Define the arguments
# If you have multiple arguments, separate them with spaces, e.g., "arg1 arg2 arg3"
$arguments = @("-s","j:\libczi\Example_TMA1_Zeb1_SPRR2_Ck19_S100-1-1-1-1.czi", "-o", "j:\libczi\Example_TMA1_Zeb1_SPRR2_Ck19_S100-1-1-1-1.db")

# Execute the executable with arguments
& $executablePath $arguments
