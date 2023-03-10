# coding style              {#coding_style}

## C++ code

For C++ code, the following guidelines and rules should be followed:

* The [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) is to be adopted - wherever possible, feasible and  applicable. 
  In those points we choose to deviate from the Google C++ Style Guide: 
  * [Line Length](https://google.github.io/styleguide/cppguide.html#Line_Length): lines longer than 80 characters are considered fine.
  * [Spaces vs. Tabs](https://google.github.io/styleguide/cppguide.html#Spaces_vs._Tabs): indent size is 4 spaces
  * [#define Guard](https://google.github.io/styleguide/cppguide.html#The__define_Guard): `#pragma once` is also fine
* For formatting, the codebase contains [.editorconfig](https://editorconfig.org/) files which define many aspects of code formatting.
  If possible, an editor which can understand those files should be used.
* The recommendations of the [C++ Core Guidelines](http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines) are to be adopted - wherever possible, feasible and  applicable.
  Usage of the [GSL: Guidelines support library](http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-gsl) is encouraged.
* The code should compile without warnings and objections with [clang-tidy](https://clang.llvm.org/extra/clang-tidy/). The build action
  contains a clang-tidy build. The code-base contains .clang-tidy files defining the enabled checks.
* The build action also includes running [cpplint](https://github.com/cpplint/cpplint#readme) on the C++ code (with some customization, c.f. the file .mega-linter.yml), and 
  this should pass without findings.
* There should not be any warnings with the regular builds - on all supported compilers.

## C# code

* Formatting rules for C#-code are given in an [.editorconfig files](https://editorconfig.org/) file.
* .NET projects are configured to use [StyleCop.Analyzers](https://github.com/DotNetAnalyzers/StyleCopAnalyzers/blob/master/DOCUMENTATION.md) and 
  [NETAnalyzers](https://learn.microsoft.com/en-us/dotnet/fundamentals/code-analysis/overview?tabs=net-7). Code should compile without warnings with 
  the given configuration of those analyzers.
 
