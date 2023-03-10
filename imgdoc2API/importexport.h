// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#ifdef __GNUC__
    #if defined(__i386__)
         #define LIBIMGDOC2_STDCALL __attribute__((stdcall))
    #else
        #define LIBIMGDOC2_STDCALL 
    #endif
#else
    #define LIBIMGDOC2_STDCALL __stdcall
#endif

#ifdef LIBIMGDOC2_EXPORTS
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

