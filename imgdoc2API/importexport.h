// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

// This file contains the definitions of the macros used to export/import the imgdoc2 API.
// * when running doxygen, the macro "LIBIMGDOC2_EXPORTS_FOR_DOXYGEN" and we use defines which are then shown in the documentation.
// * when compiling the imgdoc2 API, the macro "LIBIMGDOC2_EXPORTS" is defined and we use the __declspec(dllexport) attribute.
// * other cases are currently not worked out 

#ifdef LIBIMGDOC2_EXPORTS_FOR_DOXYGEN
    #define LIBIMGDOC2_STDCALL
#elif __GNUC__
    #if defined(__i386__)
        #define LIBIMGDOC2_STDCALL __attribute__((stdcall))
    #else
        #define LIBIMGDOC2_STDCALL 
    #endif
#else
    #define LIBIMGDOC2_STDCALL __stdcall
#endif

#ifdef LIBIMGDOC2_EXPORTS_FOR_DOXYGEN
    #define EXTERNAL_API(_returntype_) _returntype_
#elif LIBIMGDOC2_EXPORTS
    #ifdef __GNUC__
        #define LIBIMGDOC2_API __attribute__ ((visibility ("default")))
        #if defined(__i386__)
            #define EXTERNAL_API(_returntype_)  extern "C"  _returntype_ __attribute__ ((visibility ("default"))) __attribute__((stdcall))
        #else
            #define EXTERNAL_API(_returntype_)  extern "C"  _returntype_ __attribute__ ((visibility ("default"))) 
        #endif
    #else
        #define LIBIMGDOC2_API __declspec(dllexport)
        #define EXTERNAL_API(_returntype_)  extern "C"  _returntype_ LIBIMGDOC2_API  __stdcall 
    #endif
#else
    #ifdef __GNUC__
        #define LIBIMGDOC2_API
    #else
        #define LIBIMGDOC2_API __declspec(dllimport)
    #endif
#endif
