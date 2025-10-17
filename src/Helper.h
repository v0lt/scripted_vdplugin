/*
 * Copyright (C) 2024-2025 v0lt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <stdexcept>

#if _DEBUG
template <typename... Args>
void DLog(const std::string_view format, Args ...args)
{
	std::string str;
	if (sizeof...(Args)) {
		str = std::vformat(format, std::make_format_args(args...)) + '\n';
	} else {
		str.assign(format);
		str += '\n';
	}
	OutputDebugStringA(str.c_str());
};

template <typename... Args>
void DLog(const std::wstring_view format, Args ...args)
{
	std::wstring wstr;
	if (sizeof...(Args)) {
		wstr = std::vformat(format, std::make_wformat_args(args...)) + L'\n';
	} else {
		wstr.assign(format);
		wstr += L'\n';
	}
	OutputDebugStringW(wstr.c_str());
};
#else
#define DLog(...) __noop
#endif

std::wstring ConvertUtf8OrAnsiLinesToWide(const std::string_view sv);
