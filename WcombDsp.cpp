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

#include "WcombDsp.h"
//--------------------------------------------------------------------- 
#pragma resource "*.dfm"
//TWCombDspDlg *WCombDspDlg;
//---------------------------------------------------------------------
__fastcall TWCombDspDlg::TWCombDspDlg(TComponent* AOwner)
	: TForm(AOwner)
{
}
//---------------------------------------------------------------------
void __fastcall TWCombDspDlg::Execute(ANTDEF *ap, PDEF *pp, WDEF *wp, int n)
{
	wmax = CombWire(wdef, ap, pp, wp);
	char bf[128];
    sprintf(bf, "Wire No.%d    組み合わせワイヤ数 = %d", n, wmax);
	LStat->Caption = bf;
	int i;
	for( wp = wdef, i = 0; i < wmax; i++, wp++ ){
		double xx = wp->X2 - wp->X1;
		double yy = wp->Y2 - wp->Y1;
		double zz = wp->Z2 - wp->Z1;
		Len[i] = sqrt(xx*xx + yy*yy + zz*zz);
    }
	Grid->RowCount = wmax + 1;
	ShowModal();
}

void __fastcall TWCombDspDlg::GridDrawCell(TObject *Sender, int Col, int Row,
	TRect &Rect, TGridDrawState State)
{
	char	bf[64];

	Grid->Canvas->FillRect(Rect);
	int X = Rect.Left + 4;
	int Y = Rect.Top + 2;

	if( Row ){
		Row--;
		WDEF *wp = &wdef[Row];
		switch(Col){
			case 0:
				sprintf( bf, "%d", Row+1 );
				break;
			case 1:
				strcpy(bf, StrDbl(wp->X1));
				break;
			case 2:
				strcpy(bf, StrDbl(wp->Y1));
				break;
			case 3:
				strcpy(bf, StrDbl(wp->Z1));
				break;
			case 4:
				strcpy(bf, StrDbl(wp->X2));
				break;
			case 5:
				strcpy(bf, StrDbl(wp->Y2));
				break;
			case 6:
				strcpy(bf, StrDbl(wp->Z2));
				break;
			case 7:
				strcpy(bf, StrDbl(wp->R*1000.0));
				break;
			case 8:
				strcpy(bf, StrDbl(Len[Row]));
				break;
		}
		Grid->Canvas->TextOut(X, Y, bf);
	}
	else {		// タイトル
		LPCSTR	_tt[]={
			"No.","X1(m)","Y1(m)","Z1(m)","X2(m)","Y2(m)","Z2(m)","R(mm)","L(m)",
		};
		Grid->Canvas->TextOut(X, Y, _tt[Col]);
	}
}
//---------------------------------------------------------------------------
