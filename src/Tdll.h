/*
 * Copyright (c) 2002 Milan Cutka
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
 
// ************************************************************************
// *** Assimilated for VirtualDubMod from http://ffdshow.sf.net			***
// *** Tobias Minich, Feb 2003											***
// ************************************************************************


#ifndef _TDLL_H_
#define _TDLL_H_

// ************************************************************************
// *** Assimilated for VirtualDubMod									***
// *** Tobias Minich, Feb 2003											***
// BEGIN ******************************************************************
//#include "Tconfig.h"
// END ********************************************************************

class Tdll
{
public:
 bool ok;
 Tdll(const char *dllName1,const char *pth)
  {
   hdll=LoadLibraryA(dllName1);
   if (!hdll)
    {
     char name[MAX_PATH],ext[MAX_PATH];
     _splitpath(dllName1,NULL,NULL,name,ext);
     char dllName2[MAX_PATH];
     _makepath(dllName2,NULL,NULL,name,ext);
     hdll=LoadLibraryA(dllName2);
     if (!hdll && pth)
      {
       char pomS[MAX_PATH];
       strcpy(pomS,pth);
       strcat(pomS,dllName2);
       //sprintf(pomS,"%s%s",pth,dllName2);
       hdll=LoadLibraryA(pomS);
      }
    }
   ok=(hdll!=NULL);
  }
 ~Tdll()
  {
   if (hdll) FreeLibrary(hdll);
  }
 HMODULE hdll;
 void loadFunction(void **fnc,const char *name)
  {
   *fnc=NULL;
   *fnc=(void*)GetProcAddress(hdll,name);
   ok&=(*fnc!=NULL);
  }
};

#endif
