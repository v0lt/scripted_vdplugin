/*
 * Copyright (C) 2024-2025 v0lt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <memory>
#include <string>
#include <format>
#include <stdexcept>

#if _DEBUG
template <typename... Args>
void DLog(const std::string_view format, Args ...args)
{
	std::string str = std::vformat(format, std::make_format_args(args...)) + '\n';

	OutputDebugStringA(str.c_str());
};

template <typename... Args>
void DLog(const std::wstring_view format, Args ...args)
{
	std::wstring str = std::vformat(format, std::make_wformat_args(args...)) + L'\n';

	OutputDebugStringW(str.c_str());
};
#else
#define DLog(...) __noop
#endif
