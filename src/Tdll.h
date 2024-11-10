/*
 * Copyright (c) 2002 Milan Cutka
 * Copyright (c) 2003 Tobias Minich
 * Copyright (C) 2024 v0lt
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _TDLL_H_
#define _TDLL_H_

class Tdll
{
public:
    bool ok;
    Tdll(const char* dllName1, const char* pth)
    {
        hdll = LoadLibraryA(dllName1);
        if (!hdll) {
            char name[MAX_PATH], ext[MAX_PATH];
            _splitpath_s(dllName1, nullptr, 0, nullptr, 0, name, MAX_PATH, ext, MAX_PATH);

            char dllName2[MAX_PATH];
            _makepath_s(dllName2, nullptr, nullptr, name, ext);

            hdll = LoadLibraryA(dllName2);
            if (!hdll && pth) {
                char pomS[MAX_PATH];
                strcpy_s(pomS, pth);
                strcat_s(pomS, dllName2);
                //sprintf(pomS,"%s%s",pth,dllName2);
                hdll = LoadLibraryA(pomS);
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
    HMODULE hdll;
    void loadFunction(void** fnc, const char* name)
    {
        *fnc = NULL;
        *fnc = (void*)GetProcAddress(hdll, name);
        ok &= (*fnc != NULL);
    }
};

#endif
