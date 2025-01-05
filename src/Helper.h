#pragma once

#include <string>
#include <cstdio>

#if _DEBUG
template <typename... Args>
void DLog(const char* format, Args ...args)
{
	char buf[2000];

	sprintf_s(buf, format, args...);
	strcat_s(buf, "\n");

	OutputDebugStringA(buf);
};

template <typename... Args>
void DLog(const wchar_t* format, Args ...args)
{
	wchar_t buf[2000];

	swprintf_s(buf, format, args...);
	wcscat_s(buf, L"\n");

	OutputDebugStringW(buf);
};
#else
#define DLog(...) __noop
#endif

// std::string formatting like sprintf (C++11)
// https://stackoverflow.com/a/26221725

template<typename ... Args>
std::string string_format(const std::string& format, Args ... args)
{
	int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
	if (size_s <= 0) { throw std::runtime_error("Error during formatting."); }
	auto size = static_cast<size_t>(size_s);
	std::unique_ptr<char[]> buf(new char[size]);
	std::snprintf(buf.get(), size, format.c_str(), args ...);
	return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

template<typename ... Args>
std::wstring string_format(const std::wstring& format, Args ... args)
{
	int size_s = std::swprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
	if (size_s <= 0) { throw std::runtime_error("Error during formatting."); }
	auto size = static_cast<size_t>(size_s);
	std::unique_ptr<wchar_t[]> buf(new wchar_t[size]);
	std::swprintf(buf.get(), size, format.c_str(), args ...);
	return std::wstring(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}
