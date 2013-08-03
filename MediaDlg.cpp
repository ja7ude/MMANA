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

#include "MediaDlg.h"
//--------------------------------------------------------------------- 
#pragma resource "*.dfm"
//TMediaDlgBox *MediaDlgBox;
//---------------------------------------------------------------------
__fastcall TMediaDlgBox::TMediaDlgBox(TComponent* AOwner)
	: TForm(AOwner)
{
}
//---------------------------------------------------------------------------
// ダイアログボックスを開く
int __fastcall TMediaDlgBox::Execute(void)
{
	char	bf[64];

	memcpy(&lenv, &env, sizeof(lenv));
	GScrFlag->Checked = lenv.mtype;
	sprintf(bf, "%u", lenv.gscrcnt );
	RadCnt->Text = bf;
	RadR->Text = StrDbl(lenv.gscrr);
	Grid->RowCount = lenv.mmax + 2;
	UpdateUIAll();
	if( ShowModal() == IDOK ){
		lenv.mtype = GScrFlag->Checked;
		CalcU(lenv.gscrcnt, AnsiString(RadCnt->Text).c_str());
		Calc(lenv.gscrr, AnsiString(RadR->Text).c_str() );
		memcpy(&env, &lenv, sizeof(lenv));
		return TRUE;
	}
	return FALSE;
}
//---------------------------------------------------------------------------
// 更新する
void __fastcall TMediaDlgBox::UpdateUIAll(void)
{
	if( GScrFlag->Checked ){
		RadCnt->Enabled = TRUE;
		RadR->Enabled = TRUE;
		Label1->Enabled = TRUE;
		Label3->Enabled = TRUE;
	}
	else {
		RadCnt->Enabled = FALSE;
		RadR->Enabled = FALSE;
		Label1->Enabled = FALSE;
		Label3->Enabled = FALSE;
	}
	if( ActiveControl == Grid ){
		K1->Enabled = TRUE;
		K2->Enabled = TRUE;
	}
	else {
		K1->Enabled = FALSE;
		K2->Enabled = FALSE;
	}
}
//---------------------------------------------------------------------------
// ワイヤー定義のセル位置のテキストを返す
void __fastcall TMediaDlgBox::GridGetText(LPSTR t, long Col, long Row)
{
	*t = 0;
	if( Row ){
		Row--;
		switch(Col){
			case 0:
				if( Row < lenv.mmax ){
					sprintf( t, "%3d", Row+1);
				}
				else {
					strcpy(t, "新規");
				}
				break;
			case 1:
				if( Row < lenv.mmax ){
					strcpy(t, StrDbl(lenv.rel[Row]));
				}
				break;
			case 2:
				if( Row < lenv.mmax ){
					strcpy(t, StrDbl(lenv.cond[Row]*1000.0));
				}
				break;
			case 3:
				if( Row < lenv.mmax ){
					strcpy(t, StrDbl(lenv.intval[Row]));
				}
				break;
			case 4:
				if( Row < lenv.mmax ){
					strcpy(t, StrDbl(lenv.height[Row]));
				}
				break;
		}
	}
}
//---------------------------------------------------------------------
void __fastcall TMediaDlgBox::GridDrawCell(TObject *Sender, int Col, int Row,
	TRect &Rect, TGridDrawState State)
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
		LPCSTR _tt1[]={
			"No.","誘電率","導電率(mS/m)","X距離(m)","高さ(m)"
		};
		LPCSTR _tt2[]={
			"No.","誘電率","導電率(mS/m)","半径(m)","高さ(m)"
		};
		Grid->Canvas->TextOut(X, Y, GScrFlag->Checked ? _tt2[Col]:_tt1[Col]);
	}
}
//---------------------------------------------------------------------------
void __fastcall TMediaDlgBox::GScrFlagClick(TObject *Sender)
{
	UpdateUIAll();
	Grid->Invalidate();
}
//---------------------------------------------------------------------------
// 新規作成
void __fastcall TMediaDlgBox::GridNewLine(int Row)
{
	if( Row >= lenv.mmax ){
		lenv.mmax++;
		if( lenv.mmax < EMAX ){
			lenv.intval[lenv.mmax] = NULLV;
			lenv.height[lenv.mmax] = NULLV;
		}
		Grid->RowCount = lenv.mmax + 2;
		Grid->Invalidate();
	}
}
//---------------------------------------------------------------------------
void __fastcall TMediaDlgBox::GridSetEditText(TObject *Sender, int ACol,
	int ARow, const UnicodeString Value)
{
	double	d;

	if( Grid->EditorMode == TRUE ) return;
	if( ARow ){
		ARow--;
		switch(ACol){
			case 1:		// DIE（導）
				if( Calc(d, AnsiString(Value).c_str()) == TRUE){
					lenv.rel[ARow] = d;
					GridNewLine(ARow);
				}
				break;
			case 2:		// COND（誘電）
				if( Calc(d, AnsiString(Value).c_str()) == TRUE){
					lenv.cond[ARow] = d/1000.0;
					GridNewLine(ARow);
				}
				break;
			case 3:		// XorR(m)
				if( *Value.c_str() ){
					if( Calc(d, AnsiString(Value).c_str()) == TRUE){
						lenv.intval[ARow] = d;
						GridNewLine(ARow);
					}
				}
				else if( ARow >= (lenv.mmax - 1) ){
					lenv.intval[ARow] = NULLV;
					GridNewLine(ARow);
				}
				break;
			case 4:		// H(m)
				if( *Value.c_str() ){
					if( Calc(d, AnsiString(Value).c_str()) == TRUE){
						lenv.height[ARow] = d;
						GridNewLine(ARow);
					}
				}
				else if( ARow >= (lenv.mmax - 1) ){
					lenv.height[ARow] = NULLV;
					GridNewLine(ARow);
				}
				break;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMediaDlgBox::GridGetEditText(TObject *Sender, int ACol,
	int ARow, UnicodeString &Value)
{
	char	bf[64];

	GridGetText(bf, ACol, ARow);
	Value = bf;
}
//---------------------------------------------------------------------------
// この行に削除
void __fastcall TMediaDlgBox::K1Click(TObject *Sender)
{
	int	Row, i;
	if( Grid->Row && (Grid->Row <= lenv.mmax) ){
		Row = Grid->Row - 1;
		for( i = Row; i < (lenv.mmax - 1); i++ ){
			memcpy(&lenv.rel[i], &lenv.rel[i+1], sizeof(double));
			memcpy(&lenv.cond[i], &lenv.cond[i+1], sizeof(double));
			memcpy(&lenv.intval[i], &lenv.intval[i+1], sizeof(double));
			memcpy(&lenv.height[i], &lenv.height[i+1], sizeof(double));
		}
		lenv.rel[i] = 0;
		lenv.cond[i] = 0;
		lenv.intval[i] = NULLV;
		lenv.height[i] = NULLV;
		lenv.mmax--;
		Grid->RowCount = lenv.mmax + 2;
		Grid->Invalidate();
	}
}
//---------------------------------------------------------------------------
// この行に挿入
void __fastcall TMediaDlgBox::K2Click(TObject *Sender)
{
	int	Row, i;

	if( Grid->Row && (Grid->Row <= lenv.mmax) && (lenv.mmax < (EMAX-1)) ){
		Row = Grid->Row - 1;
		for( i = lenv.mmax; i > Row; i-- ){
			memcpy(&lenv.rel[i], &lenv.rel[i-1], sizeof(double));
			memcpy(&lenv.cond[i], &lenv.cond[i-1], sizeof(double));
			memcpy(&lenv.intval[i], &lenv.intval[i-1], sizeof(double));
			memcpy(&lenv.height[i], &lenv.height[i-1], sizeof(double));
		}
		lenv.rel[i] = 0;
		lenv.cond[i] = 0;
		lenv.intval[i] = NULLV;
		lenv.height[i] = NULLV;
		lenv.mmax++;
		Grid->RowCount = lenv.mmax + 2;
		Grid->Invalidate();
	}
}
//---------------------------------------------------------------------------
void __fastcall TMediaDlgBox::GridClick(TObject *Sender)
{
	UpdateUIAll();	
}
//---------------------------------------------------------------------------
