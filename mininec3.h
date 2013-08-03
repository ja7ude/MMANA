//Copyright+LGPL

//-------------------------------------------------------------------------------------------------------------------------------------------
// Copyright 1999-2013 Makoto Mori
//-----------------------------------------------------------------------------------------------------------------------------------------------
// This file is part of MMANA.

// MMANA is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// MMANA is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License along with MMANA.  If not, see
// <http://www.gnu.org/licenses/>.
//-----------------------------------------------------------------------------------------------------------------------------------------------

#ifndef MiniNec3H
#define MiniNec3H
#include <math.h>
#include "ComLib.h"
//---------------------------------------------------------------------------
int InitNEC(int max);
void DeleteNEC(void);
int NewNEC(UINT max);
int InitAntData(ANTDEF *ap);
void CalCurrent(void);
void CalCurrentDR(void);
int GetPlusMax(void);
void GetSegPos(double &x, double &y, double &z, int Plus);
void GetSegPosReal(double &x, double &y, double &z, int Plus);
void SaveCurrent(void);
void SaveNear(void);
void SaveFar(void);
void CalcNewPtn(void);
//---------------------------------------------------------------------------
#endif