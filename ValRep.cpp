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

#include "ValRep.h"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
//TValRepDlg *ValRepDlg;
//---------------------------------------------------------------------
__fastcall TValRepDlg::TValRepDlg(TComponent* AOwner)
	: TForm(AOwner)
{
}
//---------------------------------------------------------------------
// 検索と置き換えの実行
int __fastcall TValRepDlg::Execute(double &nd, double &od)
{
	double od1, od2;
	double nd1, nd2;

	int f = FALSE;
	RepSel->Enabled = FALSE;
	InvSel->Enabled = FALSE;
	EditOld->Text = StrDbl(od);
	EditNew->Text = StrDbl(od);
	Calc(od1, AnsiString(EditOld->Text).c_str());
	Calc(nd1, AnsiString(EditNew->Text).c_str());
	if( ShowModal() == IDOK ){
		Calc(od2, AnsiString(EditOld->Text).c_str());
		Calc(nd2, AnsiString(EditNew->Text).c_str());
		if( od1 != od2 ){
			od = od2;
		}
		if( nd1 != nd2 ){
			nd = nd2;
		}
		if( od != nd ) f = TRUE;
	}
	return f;
}
//---------------------------------------------------------------------
// 検索と置き換えの実行
int __fastcall TValRepDlg::Execute(ANTDEF *ap, int Row, int Col)
{
	Row--;
	Col--;
	WDEF *wp = &ap->wdef[Row];
	double d, nd, od, dd;
	int f = FALSE;
	switch(Col){
		case 0:		// X1
			d = wp->X1;
			dd = GetRmdVal(d);
			break;
		case 1:		// Y1
			d = wp->Y1;
			dd = GetRmdVal(d);
			break;
		case 2:		// Z1
			d = wp->Z1;
			dd = GetRmdVal(d);
			break;
		case 3:		// X2
			d = wp->X2;
			dd = GetRmdVal(d);
			break;
		case 4:		// Y2
			d = wp->Y2;
			dd = GetRmdVal(d);
			break;
		case 5:		// Z2
			d = wp->Z2;
			dd = GetRmdVal(d);
			break;
		case 6:		// R
			RepSel->ItemIndex = 0;
			InvSel->Checked = FALSE;
			RepSel->Enabled = FALSE;
			InvSel->Enabled = FALSE;
			d = wp->R;
			dd = GetRmdValD(d * 1000.0);
			break;
		case 7:		// Seg
			RepSel->ItemIndex = 0;
			InvSel->Checked = FALSE;
			RepSel->Enabled = FALSE;
			InvSel->Enabled = FALSE;
			EditOld->Text = Seg2Str(wp->SEG);
			EditNew->Text = Seg2Str(wp->SEG);
			break;
	}
	if( Col != 7 ){
		EditOld->Text = StrDbl(dd);
		EditNew->Text = StrDbl(dd);
		Calc(dd, AnsiString(EditOld->Text).c_str());
	}
	if( ShowModal() == IDOK ){
		int i;
		wp = ap->wdef;
		if( Col == 7 ){				// SEGの変更
			int oseg, nseg;
			if( Str2Seg(oseg, AnsiString(EditOld->Text).c_str()) != TRUE ) return FALSE;
			if( Str2Seg(nseg, AnsiString(EditNew->Text).c_str()) != TRUE ) return FALSE;
			if( oseg != nseg ){
				for( i = 0; i < ap->wmax; i++, wp++ ){
					if( wp->SEG == oseg ){
						wp->SEG = nseg;
						f = TRUE;
					}
				}
			}
			return f;
		}
		nd = d;
		Calc(od, AnsiString(EditOld->Text).c_str());
		Calc(nd, AnsiString(EditNew->Text).c_str());
		if( od == nd ) return FALSE;
		if( Col == 6 ){				// 半径の変更
			if( dd != od ){
				d = SetRmdValD(od)/1000.0;
			}
			for( i = 0; i < ap->wmax; i++, wp++ ){
				if( wp->R == d ){
					wp->R = SetRmdValD(nd)/1000.0;
					f = TRUE;
				}
			}
		}
		else {
			if( dd != od ){
				d = SetRmdVal(od);
			}
			nd = SetRmdVal(nd);
			for( i = 0; i < ap->wmax; i++, wp++ ){
				switch(RepSel->ItemIndex){
					case 0:
						RepJob(f, wp->X1, d, nd);
						RepJob(f, wp->Y1, d, nd);
						RepJob(f, wp->Z1, d, nd);
						RepJob(f, wp->X2, d, nd);
						RepJob(f, wp->Y2, d, nd);
						RepJob(f, wp->Z2, d, nd);
						break;
					case 1:
						RepJob(f, wp->X1, d, nd);
						RepJob(f, wp->X2, d, nd);
						break;
					case 2:
						RepJob(f, wp->Y1, d, nd);
						RepJob(f, wp->Y2, d, nd);
						break;
					case 3:
						RepJob(f, wp->Z1, d, nd);
						RepJob(f, wp->Z2, d, nd);
						break;
				}
			}
		}
	}
	return f;
}
//---------------------------------------------------------------------
// ひとつの座標の置き換え
void TValRepDlg::RepJob(int &f, double &t, double d, double nd)
{
	double dd = t;
	if( dd == d ){
		t = nd;
		f = TRUE;
	}
	else if( InvSel->Checked == TRUE ){
		dd = ABS(t);
		if( dd == ABS(d) ){
			if( d == 0.0 ){
				t = nd;
			}
			else {
				double s = SGN(d) * SGN(nd) * SGN(t);
				t = s * ABS(nd);
			}
			f = TRUE;
		}
	}
}
//---------------------------------------------------------------------
