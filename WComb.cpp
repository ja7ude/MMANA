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

#include "WComb.h"
//--------------------------------------------------------------------- 
#pragma resource "*.dfm"
//TWCombDlg *WCombDlg;
//---------------------------------------------------------------------
__fastcall TWCombDlg::TWCombDlg(TComponent* AOwner)
	: TForm(AOwner)
{
	Grid->ColCount = PPMAX * 2 + 3;
}
//---------------------------------------------------------------------
int __fastcall TWCombDlg::Execute(ANTDEF *ap)
{
	pAnt = ap;
	Grid->RowCount = ap->pmax + 2;
	int pmax = ap->pmax;
    memcpy(pdef, ap->pdef, sizeof(pdef));
    if( ShowModal() == IDOK ){
		if( pmax != ap->pmax ) return TRUE;
        if( memcmp(pdef, ap->pdef, sizeof(pdef)) ) return TRUE;
    }
    return FALSE;
}
//---------------------------------------------------------------------
void __fastcall TWCombDlg::GridGetText(LPSTR t, long Col, long Row)
{
	LPCSTR	_tt[]={"⇔","⇒","⇔ *","⇒ *"};

	Row--;
	*t = 0;
	if( pAnt == NULL ) return;
	PDEF *pp = &pAnt->pdef[Row];
	switch(Col){
		case 0:
			if( Row < pAnt->pmax ){
				sprintf(t, "%d", Row+1 );
            }
            else {
				strcpy(t, "新規");
            }
			break;
		case 1:
			if( Row < pAnt->pmax ) strcpy(t, StrDbl(pp->RR*1000.0));
			break;
		case 2:
			if( Row < pAnt->pmax ) strcpy(t, _tt[pp->Type]);
           	break;
        default:
			if( Row >= pAnt->pmax ) break;
			Col -= 3;
            if( Col & 1 ){
				Col /= 2;
                if( pp->L[Col] ) strcpy(t, StrDbl(pp->R[Col]*1000.0));
            }
            else {
				Col /= 2;
                if( pp->L[Col] ) strcpy(t, StrDbl(pp->L[Col]));
            }
          	break;
	}
}
//---------------------------------------------------------------------
void __fastcall TWCombDlg::GridDrawCell(TObject *Sender, int Col,
	int Row, TRect &Rect, TGridDrawState State)
{
	char	bf[64];

	Grid->Canvas->FillRect(Rect);
	int X = Rect.Left + 4;
	int Y = Rect.Top + 2;

	if( Row ){
		GridGetText(bf, Col, Row);
		Grid->Canvas->TextOut(X, Y, bf);
	}
	else {		// タイトル
		LPCSTR	_tt[]={
			"No.","R(mm)","Type"
		};
		if( Col < 3 ){
			Grid->Canvas->TextOut(X, Y, _tt[Col]);
        }
        else {
			Col -= 3;
			if( Col & 1 ){
	            sprintf(bf, "R%d(mm)", Col/2);
            }
            else {
	            sprintf(bf, "L%d(m)", Col/2);
            }
			Grid->Canvas->TextOut(X, Y, bf);
        }
	}
}
//---------------------------------------------------------------------------
void __fastcall TWCombDlg::GridGetEditText(TObject *Sender, int ACol,
	int ARow, UnicodeString &Value)
{
	char	bf[64];

	if( ACol == 2 ){
		Grid->EditorMode = FALSE;
		int X = Left + (Width - ClientWidth)/2 + Grid->Left;
		int Y = Top + (Height - ClientHeight)/2 + Grid->Top;
		X += Grid->ColWidths[0] + Grid->ColWidths[1];
		Y += ((Grid->Row - Grid->TopRow) * Grid->RowHeights[0]);
		PopupType->Popup(X, Y);
    }
    else {
		GridGetText(bf, ACol, ARow);
		Value = bf;
    }
}
//---------------------------------------------------------------------------
void __fastcall TWCombDlg::NewLine(int Row)
{
	if( (Row >= pAnt->pmax) && (Row < PTMAX) ){
		PDEF *pp = &pAnt->pdef[pAnt->pmax];
		pAnt->pmax++;
		pp->RR = -0.001;
        pp->Type = 2;
        pp->L[0] = 1.8;
        pp->R[0] = 0.015;
        pp->L[1] = 1.8;
		pp->R[1] = 0.0125;
        pp->L[2] = 99999.9;
        pp->R[2] = 0.010;
        pp->R[3] = pp->R[4] = pp->R[5] = pp->R[6] = pp->R[7] = pp->R[8] = pp->R[9] = 0.007;
        Grid->RowCount = pAnt->pmax + 2;
    }
}
//---------------------------------------------------------------------------
void __fastcall TWCombDlg::GridSetEditText(TObject *Sender, int ACol,
	int ARow, const UnicodeString Value)
{
	double	d;

	if( pAnt == NULL ) return;
	if( Grid->EditorMode == TRUE ) return;
	if( ARow ){
		ARow--;
		PDEF *pp = &pAnt->pdef[ARow];
		switch(ACol){
			case 1:		// RR(mm)
				if( Calc(d, AnsiString(Value).c_str()) == TRUE ){
					if( d != 0.0 ){
						if( d >= 0.0 ) d = -d;
						NewLine(ARow);
						pp->RR = d / 1000.0;
						Grid->Invalidate();
					}
				}
				break;
			case 2:		// Type
				break;
			default:
				ACol-=3;
				if( ACol & 1 ){
					ACol/=2;
					if( Calc(d, AnsiString(Value).c_str()) == TRUE ){
						if( d != 0.0 ){
							pp->R[ACol] = ABS(d) / 1000.0;
							Grid->Invalidate();
                        }
					}
                }
                else {
					ACol/=2;
					if( Calc(d, AnsiString(Value).c_str()) == TRUE ){
						if( d >= 0 ){
							pp->L[ACol] = ABS(d);
							Grid->Invalidate();
						}
					}
				}
				break;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TWCombDlg::SetType(int type)
{
	if( Grid->Row ){
		int Row = Grid->Row - 1;
		NewLine(Row);
		pAnt->pdef[Row].Type = type;
		Grid->Invalidate();
	}
}
//---------------------------------------------------------------------------
void __fastcall TWCombDlg::K1Click(TObject *Sender)
{
	SetType(0);
}
//---------------------------------------------------------------------------
void __fastcall TWCombDlg::K2Click(TObject *Sender)
{
	SetType(1);
}
//---------------------------------------------------------------------------
void __fastcall TWCombDlg::K3Click(TObject *Sender)
{
	SetType(2);
}
//---------------------------------------------------------------------------
void __fastcall TWCombDlg::K4Click(TObject *Sender)
{
	SetType(3);
}
//---------------------------------------------------------------------------
void __fastcall TWCombDlg::DelBtnClick(TObject *Sender)
{
	if( Grid->Row ){
		int Row = Grid->Row - 1;
		if( Row < pAnt->pmax ){
			int i;
			for( i = Row; i < (pAnt->pmax - 1); i++ ){
				memcpy(&pAnt->pdef[i], &pAnt->pdef[i+1], sizeof(PDEF));
			}
			memset(&pAnt->pdef[i], 0, sizeof(PDEF));
			pAnt->pmax--;
			Grid->RowCount = pAnt->pmax + 2;
			Grid->Invalidate();
        }
    }
    Grid->SetFocus();
}
//---------------------------------------------------------------------------
