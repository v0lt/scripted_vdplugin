/*
 * Copyright (C) 2025 v0lt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "stdafx.h"
#include "Helper.h"

std::wstring ConvertUtf8OrAnsiLinesToWide(const std::string_view sv)
{
	std::wstring wstr;
	size_t pos = 0;

	while (pos < sv.length()) {
		size_t k = sv.find_first_of('\n', pos);
		if (k != std::string::npos) {
			k++;
		} else {
			k = sv.length();
		}

		auto line = sv.data() + pos;
		auto line_size = k - pos;
		UINT codePage = CP_UTF8;
		int count = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, line, line_size, nullptr, 0);
		if (count == 0) {
			codePage = CP_ACP;
			count = MultiByteToWideChar(CP_ACP, 0, line, line_size, nullptr, 0);
		}

		auto size = wstr.length();
		wstr.resize(size + count);
		MultiByteToWideChar(codePage, 0, line, line_size, &wstr[size], count);

		pos = k;
	}

	return wstr;
}
