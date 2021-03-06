//Copyright (C) 2008  KeyMagic Project
//http://keymagic.googlecode.com
//
//This program is free software; you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#ifndef STRTYPEFUNC_H_
#define STRTYPEFUNC_H_

#pragma once

#include <windows.h>
#include <string>

#ifdef UNICODE
typedef std::wstring km_string;
#else
typedef std::string km_string;
#endif

extern errno_t km_strcat(_Inout_z_cap_(_Size) TCHAR * _Dst, _In_ rsize_t _Size, _In_z_ const TCHAR * _Src);

#endif
