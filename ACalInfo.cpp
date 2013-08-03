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

#include "ACalInfo.h"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
//TACalInfoBox *ACalInfoBox;
//---------------------------------------------------------------------
__fastcall TACalInfoBox::TACalInfoBox(TComponent* AOwner)
	: TForm(AOwner)
{
}
//---------------------------------------------------------------------
void __fastcall TACalInfoBox::UpdateUIEditTerg(void)
{
    if( MatchSel->ItemIndex == 2 ){
		EditTergR->Enabled = TRUE;
		EditTergJX->Enabled = TRUE;
    }
    else {
		EditTergR->Enabled = FALSE;
		EditTergJX->Enabled = FALSE;
    }
}
//---------------------------------------------------------------------
int __fastcall TACalInfoBox::Execute(CACal *p)
{
	ATRG &at = p->atrg;
	double d;

    UDSWR->Associate = EditSWR;
	UDGain->Position = short(at.MaxGain);
	UDFB->Position = short(at.MaxFB);
	UDJX->Position = short(at.MinJX);
	UDSWR->Associate = NULL;
	UDSWR->Position = 50;
    EditSWR->Text = StrDbl(at.MinSWR);
	MatchSel->ItemIndex = at.Match;
	UDFBH->Position = short(env.fbr & 0x0000ffff);
	UDFBV->Position = short((env.fbr >> 16) & 0x0000ffff);
	EditSWRZ->Text = StrDbl(env.RO);
	EditSWRJX->Text = StrDbl(env.JXO);
	EditTergR->Text = (at.TergR == NULLV) ? "*" : StrDbl(at.TergR);
    EditTergJX->Text = (at.TergJX == NULLV) ? "*" : StrDbl(at.TergJX);
	if( at.IType ){
		RBIMIN->Checked = TRUE;
    }
    else {
		RBIMAX->Checked = TRUE;
    }
	EditCur->Text = at.IPLUS;
	UpdateUIEditTerg();
	if( ShowModal() == IDOK ){
		if( Calc(d, AnsiString(EditGain->Text).c_str()) == TRUE ){
			if( d > 0.0 ){
				at.MaxGain = d;
			}
		}
		if( Calc(d, AnsiString(EditFB->Text).c_str()) == TRUE ){
			if( d > 0.0 ){
				at.MaxFB = d;
			}
		}
		if( Calc(d, AnsiString(EditJX->Text).c_str()) == TRUE ){
			if( d >= 0.0 ){
				at.MinJX = d;
			}
		}
		if( Calc(d, AnsiString(EditSWR->Text).c_str()) == TRUE ){
			if( d >= 1.0 ){
				at.MinSWR = d;
            }
        }
		at.Match = MatchSel->ItemIndex;
		if( *EditTergR->Text.c_str() == '*' ){
			at.TergR = NULLV;
        }
		else if( Calc(d, AnsiString(EditTergR->Text).c_str()) == TRUE ){
			if( d >= 0.0 ){
				at.TergR = d;
            }
        }
		if( *AnsiString(EditTergJX->Text).c_str() == '*' ){
			at.TergJX = NULLV;
		}
		else if( Calc(d, AnsiString(EditTergJX->Text).c_str()) == TRUE ){
			at.TergJX = d;
        }
		env.fbr = UDFBH->Position;
		env.fbr |= UDFBV->Position << 16;
		if( Calc(d, AnsiString(EditSWRZ->Text).c_str()) == TRUE ){
			if( d > 0.0 ){
				env.RO = d;
			}
		}
		if( Calc(d, AnsiString(EditSWRJX->Text).c_str()) == TRUE ){
			env.JXO = d;
		}
		at.IType = RBIMIN->Checked;
		ZSCopy(at.IPLUS, AnsiString(EditCur->Text).c_str(), 8);
		return TRUE;
    }
	return FALSE;
}
//---------------------------------------------------------------------

void __fastcall TACalInfoBox::UDSWRClick(TObject *Sender, TUDBtnType Button)
{
	double	d = 1.0;

	if( Calc(d, AnsiString(EditSWR->Text).c_str()) != TRUE ){
		d = 1.0;
    }
    if( d < 1.0 ) d = 1.0;
	if( Button == btNext ){
		d += 0.1;
    }
    else if( Button == btPrev ){
		d -= 0.1;
    }
	if( d < 1.0 ) d = 1.0;
    EditSWR->Text = StrDbl(d);
}
//---------------------------------------------------------------------------
void __fastcall TACalInfoBox::UDSWRChanging(TObject *Sender, bool &AllowChange)
{
	UDSWR->Position = 50;
}
//---------------------------------------------------------------------------
void __fastcall TACalInfoBox::MatchSelClick(TObject *Sender)
{
	UpdateUIEditTerg();
}
//---------------------------------------------------------------------------

void __fastcall TACalInfoBox::AutoJXClick(TObject *Sender)
{
	double R, MRO;

	if( (Calc(R, AnsiString(EditSWRZ->Text).c_str()) == TRUE) && (R > 0.0) ){
		UnicodeString MTZ = R < 50 ? "50" : StrDbl(R*2);     //ja7ude 1.0
		if( InputQuery("MMANA", "®‡–Ú•W‚ÌZ‚ð“ü—Í", MTZ) == TRUE ){
			if( (Calc(MRO, AnsiString(MTZ).c_str()) == TRUE) && (MRO > R) ){
				EditSWRJX->Text = StrDbl(-(sqrt((MRO/R)-1) * R));
			}
            else {
				EditSWRJX->Text = (MRO == R) ? "0":"";
            }
	    }
    }
}
//---------------------------------------------------------------------------
