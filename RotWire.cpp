//Copyright+LGPL

//-----------------------------------------------------------------------------------------------------------------------------------------------
// Copyright 1999-2013 Makoto Mori, Nobuyuki Oba
//-----------------------------------------------------------------------------------------------------------------------------------------------
// This file is part of MMANA.

// MMANA is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// MMANA is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License along with MMANA.  If not, see
// <http://www.gnu.org/licenses/>.
//-----------------------------------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "RotWire.h"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
//TRotWireDlg *RotWireDlg;
//---------------------------------------------------------------------
__fastcall TRotWireDlg::TRotWireDlg(TComponent* AOwner)
	: TForm(AOwner)
{
}
//---------------------------------------------------------------------------
int __fastcall TRotWireDlg::Execute(ANTDEF *ap)
{
	EditDeg->Text = "0";
	if( ShowModal() == IDOK ){
		double deg;
		if( Calc(deg, AnsiString(EditDeg->Text).c_str()) == FALSE ) return FALSE;	//ja7ude 1.0
		if( deg == 0 ) return FALSE;
		WDEF *wp = ap->wdef;
		int i;
		for( i = 0; i < ap->wmax; i++, wp++ ){
			RotWire(wp->X1, wp->Y1, wp->Z1, deg);
			RotWire(wp->X2, wp->Y2, wp->Z2, deg);
		}
		return TRUE;
	}
	return FALSE;
}

void __fastcall TRotWireDlg::RotWire(double &X, double &Y, double &Z, double deg)
{
	double	WL;
	switch(BSel->ItemIndex){
		case 0:		// Çwé≤
			WL = sqrt(Y*Y + Z*Z);
			deg += GetDeg(Y, Z);
			Y = WL * cos(deg*PAI/180.0);
			Z = WL * sin(deg*PAI/180.0);
			break;
		case 1:		// Çxé≤
			WL = sqrt(X*X + Z*Z);
			deg += GetDeg(X, Z);
			X = WL * cos(deg*PAI/180.0);
			Z = WL * sin(deg*PAI/180.0);
			break;
		case 2:		// Çyé≤
			WL = sqrt(X*X + Y*Y);
			deg += GetDeg(X, Y);
			X = WL * cos(deg*PAI/180.0);
			Y = WL * sin(deg*PAI/180.0);
			break;
	}
}
//---------------------------------------------------------------------
