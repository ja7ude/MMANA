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

#include "ACalMult.h"
//--------------------------------------------------------------------- 
#pragma resource "*.dfm"
//TACalMultDlg *ACalMultDlg;
//---------------------------------------------------------------------
__fastcall TACalMultDlg::TACalMultDlg(TComponent* AOwner)
	: TForm(AOwner)
{
}
//---------------------------------------------------------------------------
int __fastcall TACalMultDlg::Execute(CACal *p)
{
	pACal = p;
	UpdateView();
	Weight->Checked = pACal->atrg.bweight;
	int r = ShowModal();
	pACal->atrg.bweight = Weight->Checked;
	return r;
}
//---------------------------------------------------------------------------
// セル位置のテキストを返す
void __fastcall TACalMultDlg::GridGetText(LPSTR t, long Col, long Row)
{
	*t = 0;
	if( Row ){
		Row--;
		BDEF	*bp = &pACal->atrg.bdef[Row];
		switch(Col){
			case 0:		// 番号
				if( Row < pACal->atrg.bmax ){
					sprintf( t, "%2d", Row+1);
				}
				else {
					strcpy(t, "新規");
				}
				break;
			case 1:		// 周波数
				if( Row < pACal->atrg.bmax ){
					strcpy(t, StrDbl(bp->fq));
				}
				break;
			default:
				if( Row < pACal->atrg.bmax ){
					int n = (Col-2)/3;
					switch((Col-2)%3){
						case 0:
							strcpy(t, bp->cdef[n].PLUS);
							break;
						case 1:
							if( bp->cdef[n].PLUS[0] ){
								strcpy(t, StrDbl(bp->cdef[n].DEG));
							}
							break;
						case 2:
							if( bp->cdef[n].PLUS[0] ){
								strcpy(t, StrDbl(bp->cdef[n].EV));
							}
							break;
					}
				}
				break;
		}
	}
}
//---------------------------------------------------------------------
void __fastcall TACalMultDlg::GridDrawCell(TObject *Sender, int Col, int Row,
	TRect &Rect, TGridDrawState State)
{
	char	bf[64];

	Grid->Canvas->FillRect(Rect);
	int X = Rect.Left + 1;
	int Y = Rect.Top + 1;

	if( Row ){
		GridGetText(bf, Col, Row);
		Grid->Canvas->TextOut(X, Y, bf);
	}
	else {		// タイトル
		LPCSTR	_tt[]={
			"No.","周波数"
		};
		X += 8;
		if( Col < 2 ){
			Grid->Canvas->TextOut(X, Y, _tt[Col]);
		}
		else {
			Row = (Col-2)/3;
			switch((Col-2)%3){
				case 0:
					sprintf(bf, "ﾊﾟﾙｽ%u", Row + 1);
					break;
				case 1:
					sprintf(bf, "位相%u", Row + 1);
					break;
				case 2:
					sprintf(bf, "電圧%u", Row + 1);
					break;
			}
			Grid->Canvas->TextOut(X, Y, bf);
		}
	}
}
//---------------------------------------------------------------------------
// ビューの更新
void __fastcall TACalMultDlg::UpdateView(void)
{
	if( pACal->atrg.bmax >= 2 ) {
		DelBtn->Enabled = TRUE;
	}
	else {
		DelBtn->Enabled = FALSE;
	}
	Grid->RowCount = pACal->atrg.bmax + 2;
	Grid->Invalidate();
}
//---------------------------------------------------------------------------
// 新規バンドの作成
void __fastcall TACalMultDlg::GridNewLine(int Row)
{
	if( (Row >= pACal->atrg.bmax) && (Row < BMAX) ){
		BDEF *bp = &pACal->atrg.bdef[pACal->atrg.bmax];
		pACal->atrg.bmax++;
		memcpy(bp, bp-1, sizeof(BDEF));
		UpdateView();
	}
}
//---------------------------------------------------------------------------
void __fastcall TACalMultDlg::GridGetEditText(TObject *Sender, int ACol,
	int ARow, UnicodeString &Value)
{
	char	bf[64];

	GridGetText(bf, ACol, ARow);
	Value = bf;
}
//---------------------------------------------------------------------------
void __fastcall TACalMultDlg::GridSetEditText(TObject *Sender, int ACol,
	int ARow, const UnicodeString Value)
{
	double	d;

	if( ARow ){
		ARow--;
		BDEF	*bp = &pACal->atrg.bdef[ARow];
		switch(ACol){
			case 0:		// 番号
				break;
			case 1:		// 周波数
				if( Calc(d, AnsiString(Value).c_str()) == TRUE){
					if( (d > 1.5) && (d < 999999.9) ){
						GridNewLine(ARow);
						bp->fq = d;
						Grid->Invalidate();
					}
				}
				break;
			default:
				int n = (ACol-2)/3;
				switch((ACol-2)%3){
					case 0:
						GridNewLine(ARow);
						ZSCopy(bp->cdef[n].PLUS, AnsiString(Value).c_str(), 8);
						Grid->Invalidate();
						break;
					case 1:
						if( Calc(d, AnsiString(Value).c_str()) == TRUE){
							if( (d >= 0.0) && (d < 360.0) ){
								GridNewLine(ARow);
								bp->cdef[n].DEG = d;
								Grid->Invalidate();
							}
						}
						break;
					case 2:
						if( Calc(d, AnsiString(Value).c_str()) == TRUE){
							if( d > 0.0 ){
								GridNewLine(ARow);
								bp->cdef[n].EV = d;
								Grid->Invalidate();
							}
						}
						break;
				}
				break;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TACalMultDlg::DelBtnClick(TObject *Sender)
{
	int	Row, i;
	if( Grid->Row && (Grid->Row <= pACal->atrg.bmax) ){
		Row = Grid->Row - 1;
		for( i = Row; i < (pACal->atrg.bmax - 1); i++ ){
			memcpy(&pACal->atrg.bdef[i], &pACal->atrg.bdef[i+1], sizeof(BDEF));
		}
		memset(&pACal->atrg.bdef[i], 0, sizeof(BDEF));
		pACal->atrg.bmax--;
		UpdateView();
	}
}
//---------------------------------------------------------------------------
