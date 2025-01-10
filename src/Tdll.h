/*
 * Copyright (c) 2002 Milan Cutka
 * Copyright (c) 2003 Tobias Minich
 * Copyright (C) 2024-2025 v0lt
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _TDLL_H_
#define _TDLL_H_

class Tdll
{
public:
    HMODULE hdll;
    bool ok;

    Tdll(const wchar_t* dllName1, const wchar_t* pth)
    {
        hdll = LoadLibraryW(dllName1);
        if (!hdll) {
            wchar_t name[MAX_PATH], ext[MAX_PATH];
            _wsplitpath_s(dllName1, nullptr, 0, nullptr, 0, name, MAX_PATH, ext, MAX_PATH);

            wchar_t dllName2[MAX_PATH];
            _wmakepath_s(dllName2, nullptr, nullptr, name, ext);

            hdll = LoadLibraryW(dllName2);
            if (!hdll && pth) {
                wchar_t pomS[MAX_PATH];
                wcscpy_s(pomS, pth);
                wcscat_s(pomS, dllName2);
                hdll = LoadLibraryW(pomS);
            }
        }
        ok = (hdll != NULL);
    }

    ~Tdll()
    {
        if (hdll) {
            FreeLibrary(hdll);
        }
    }

    void loadFunction(void** fnc, const char* name)
    {
        *fnc = (void*)GetProcAddress(hdll, name);
        ok &= (*fnc != nullptr);
    }
};

#endif
