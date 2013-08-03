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

#include "MoveDlg.h"
//--------------------------------------------------------------------- 
#pragma resource "*.dfm"
//TMoveDlgBox *MoveDlgBox;
//---------------------------------------------------------------------
__fastcall TMoveDlgBox::TMoveDlgBox(TComponent* AOwner)
	: TForm(AOwner)
{
}
//---------------------------------------------------------------------
// –³Œø‰»‚ÌƒAƒbƒvƒf[ƒg
void __fastcall TMoveDlgBox::UpdateView(void)
{
	if( ChkAll->Checked == TRUE ){
		ChkWith->Enabled = FALSE;
	}
	else {
		ChkWith->Enabled = TRUE;
	}
}
//---------------------------------------------------------------------
// •½sˆÚ“®‚ÌŽÀs
int __fastcall TMoveDlgBox::Execute(ANTDEF *ap, int Row, int Col)
{
	double d;

	if( exeenv.RmdSel ) Label2->Caption = "ƒÉ";
	Row--;
	WDEF *wp = &ap->wdef[Row];
	WDEF OldW;
	memcpy(&OldW, wp, sizeof(WDEF));
	switch(Col){
		case 1:
			d = wp->X1;
			break;
		case 2:
			d = wp->Y1;
			VectSel->ItemIndex = 1;
			break;
		case 3:
			d = wp->Z1;
			VectSel->ItemIndex = 2;
			break;
		case 4:
			d = wp->X2;
			break;
		case 5:
			d = wp->Y2;
			VectSel->ItemIndex = 1;
			break;
		case 6:
			d = wp->Z2;
			VectSel->ItemIndex = 2;
			break;
		default:
			d = 0.0;
			break;
	}
	EditLen->Text = StrDbl(-d);
	if( ShowModal() == IDOK ){
		if( Calc(d, AnsiString(EditLen->Text).c_str()) != TRUE ) return FALSE;
		if( !d ) return FALSE;
		d = SetRmdVal(d);
		if( ChkAll->Checked == TRUE ){
			wp = ap->wdef;
			int i;
			for( i = 0; i < ap->wmax; i++, wp++ ){
				switch(VectSel->ItemIndex){
					case 0:		// ‚wŽ²•ûŒü
						wp->X1 += d;
						wp->X2 += d;
						break;
					case 1:		// ‚xŽ²•ûŒü
						wp->Y1 += d;
						wp->Y2 += d;
						break;
					case 2:		// ‚yŽ²•ûŒü
						wp->Z1 += d;
						wp->Z2 += d;
						break;
				}
			}
		}
		else {
			switch(VectSel->ItemIndex){
				case 0:		// ‚wŽ²•ûŒü
					wp->X1 += d;
					wp->X2 += d;
					break;
				case 1:		// ‚xŽ²•ûŒü
					wp->Y1 += d;
					wp->Y2 += d;
					break;
				case 2:		// ‚yŽ²•ûŒü
					wp->Z1 += d;
					wp->Z2 += d;
					break;
			}
			if( ChkWith->Checked == TRUE ){
				AdjWireChen(ap->wdef, ap->wmax, wp, &OldW);
			}
		}
		return TRUE;
	}
	return FALSE;
}
//---------------------------------------------------------------------
void __fastcall TMoveDlgBox::ChkAllClick(TObject *Sender)
{
	UpdateView();
}
//---------------------------------------------------------------------------
