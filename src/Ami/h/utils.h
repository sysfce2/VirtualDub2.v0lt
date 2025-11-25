// Ami - Language resource compiler for VirtualDub
//
// Copyright (C) 2013 Avery Lee
// Copyright (C) 2025 v0lt
//
// SPDX-License-Identifier: GPL-2.0-or-later
//

#ifndef f_VD2_AMI_UTILS_H
#define f_VD2_AMI_UTILS_H

#include <string>

std::string ANSIify(const std::wstring& unicode);
void warning(const char* format, ...);
void warning(const wchar_t* format, ...);
void __declspec(noreturn) fatal(const char* format, ...);
void __declspec(noreturn) fatal(const wchar_t* format, ...);
void __declspec(noreturn) fatal_internal(const char* fname, const int line);

std::basic_string<unsigned char> ConvertToSCSU(const std::wstring& s);

#endif
