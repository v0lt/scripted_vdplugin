/*
 * Copyright (C) 2024-2025 v0lt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#if _DEBUG

inline void DLog(std::string_view sv)
{
	std::string str (sv);
	str += '\n';
	OutputDebugStringA(str.c_str());
}

inline void DLog(std::wstring_view sv)
{
	std::wstring wstr(sv);
	wstr += L'\n';
	OutputDebugStringW(wstr.c_str());
}

template <typename... Args>
inline void DLog(std::format_string<Args...> fmt, Args&&... args)
{
	std::string str = std::format(fmt, std::forward<Args>(args)...);
	str += '\n';
	OutputDebugStringA(str.c_str());
};

template <typename... Args>
inline void DLog(std::wformat_string<Args...> fmt, Args&&... args)
{
	std::wstring wstr = std::format(fmt, std::forward<Args>(args)...);
	wstr += L'\n';
	OutputDebugStringW(wstr.c_str());
};

#else
#define DLog(...) __noop
#endif

std::wstring ConvertUtf8OrAnsiLinesToWide(const std::string_view sv);
