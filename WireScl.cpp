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

#include "WireScl.h"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
//TWireScaleDlg *WireScaleDlg;
//---------------------------------------------------------------------
__fastcall TWireScaleDlg::TWireScaleDlg(TComponent* AOwner)
	: TForm(AOwner)
{
	for( int i = 0; FreqTbl[i] != NULL; i++ ){
		TFreq->Items->Add(FreqTbl[i]);
	}
	tfq = 14.050;
	tsc = 1.0;
	EnbCalc = 0;
}

//---------------------------------------------------------------------------
int __fastcall TWireScaleDlg::Execute(ANTDEF *p)
{
	ap = p;
	tfq = ap->fq;
	tsc = 1.0;
	DispItem(-1);
	if( ShowModal() == IDOK ){
		if( tsc == 1.0 ) return FALSE;
		if( WireXSW->Checked || WireYSW->Checked || WireZSW->Checked ){
			ap->cfq = ap->fq = tfq;
		}
		int i;
		WDEF *wp = ap->wdef;
		for( i = 0; i < ap->wmax; i++, wp++ ){
			if( WireXSW->Checked ){
				wp->X1 *= tsc;
				wp->X2 *= tsc;
			}
			if( WireYSW->Checked ){
				wp->Y1 *= tsc;
				wp->Y2 *= tsc;
			}
			if( WireZSW->Checked ){
				wp->Z1 *= tsc;
				wp->Z2 *= tsc;
			}
			if( WireRSW->Checked ){
				if( wp->R > 0 ) wp->R *= tsc;
			}
		}
		return TRUE;
	}
	return FALSE;
}

//---------------------------------------------------------------------------
void __fastcall TWireScaleDlg::DispItem(int n)
{
	char	bf[64];

	EnbCalc = 0;
	sprintf(bf, "%s MHz", StrDbl(ap->fq));
	DFreq->Caption = bf;
	if( n != 0 ) TFreq->Text = StrDbl(tfq);
	if( n != 1 ) TSC->Text = StrDbl(tsc);
	EnbCalc = 1;
}

//---------------------------------------------------------------------
void __fastcall TWireScaleDlg::TFreqChange(TObject *Sender)
{
	if( !EnbCalc ) return;
	double	d;

	if( Calc(d, AnsiString(TFreq->Text).c_str()) == TRUE ){
		if( (d != tfq) && (d > 0.0) ){
			tfq = d;
			tsc = ap->fq / tfq;
			DispItem(0);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TWireScaleDlg::TSCChange(TObject *Sender)
{
	if( !EnbCalc ) return;
	double	d;

	if( Calc(d, AnsiString(TSC->Text).c_str()) == TRUE ){
		if( (d != tsc) && (d > 0.0) ){
			tsc = d;
			tfq = ap->fq / tsc;
			DispItem(1);
		}
	}
}
//---------------------------------------------------------------------------
