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

#include "GrpWire.h"
#include "WComb.h"
#include "ValRep.h"
#include "Main.h"
#include "Print.h"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
//TGrpWireDlg *GrpWireDlg;
//---------------------------------------------------------------------
__fastcall TGrpWireDlg::TGrpWireDlg(TComponent* AOwner)
	: TForm(AOwner)
{
	Edit = 0;
	SelOnly = 0;
	SelWire = -1;
	AntXc = AntYc = 0.0;
	pACal = NULL;
	TBarZDeg->Enabled = exeenv.Ant3D ? TRUE : FALSE;
	Ant3D->Checked = exeenv.Ant3D;
    EleWidthAll->Checked = exeenv.EleWidthAll;
	EntryAlignControl();
}
//---------------------------------------------------------------------------
int __fastcall TGrpWireDlg::Execute(ANTDEF *ap)
{
	SelOnly = 0;
	pAnt = ap;
	pACal = NULL;
	ChkRmd->Checked = exeenv.RmdSel;
	ChkIntval->Checked = exeenv.IntPos;
	Caption = pAnt->Name;
	WList.SetWire(ap);
	Grid->RowCount = WList.gmax + 2;
	AllViewBtnClick(NULL);
	ActiveControl = Grid;
	UpdateUIAll();
#if SAMECHECK
_try:;
#endif
	int r = ShowModal();
	exeenv.RmdSel = ChkRmd->Checked;
    exeenv.EleWidthAll = EleWidthAll->Checked;
	if( !(Edit & 1) ){
		if( Edit & 2 ) return TRUE;
		return FALSE;
	}
	else if( r != IDOK ){
		if( YesNoMB("エレメント編集でワイヤ定義が変更されています.\r\n\r\n内容を参照元シートに反映しますか？") != IDYES ){
			if( Edit & 2 ) return TRUE;
			return FALSE;
		}
#if SAMECHECK
		if( IsSameElement() == TRUE ) goto _try;
#endif
	}
	WList.Update();
	return TRUE;
}
//---------------------------------------------------------------------------
int __fastcall TGrpWireDlg::IsEntry(int Pos, int SubPos)
{
	if( pACal == NULL ) return FALSE;
	AVAL *ap = pACal->aval;
	for( int i = 0; i < pACal->amax; i++, ap++ ){
		if( ap->Type == avELE ){
			if( (ap->Pos == Pos) && (ap->SubPos == SubPos) ){
				return TRUE;
			}
			if( (ap->Pos == Pos) && (ap->SubPos >= 6) ){
            	if( (ap->SubPos - 5) == SubPos ) return TRUE;
            }
		}
	}
	return FALSE;
}
//---------------------------------------------------------------------------
int __fastcall TGrpWireDlg::IsEntry(int Wno)
{
	if( pACal == NULL ) return FALSE;
	AVAL *ap = pACal->aval;
	int i;
	for( i = 0; i < pACal->amax; i++, ap++ ){
		if( (ap->Type == avWIRE) || (ap->Type == avWIREP) ){
			if( Wno == ap->Pos ) return TRUE;
		}
	}
	return FALSE;
}

//---------------------------------------------------------------------------
int __fastcall TGrpWireDlg::IsSelect(int &Pos, int &SubPos, int Row, int Col)
{
	if( Row && (Row <= WList.gmax) ){
		Row--;
		switch(Col){
			case 2:		// 間隔
				if( exeenv.IntPos ){
					if( WList.Intval[Row] ){
						Pos = Row;
						SubPos = 0;
						if( IsEntry(Pos, SubPos) == TRUE ) return FALSE;
						return TRUE;
					}
				}
				else {
					if( Row >= 0 ){
						Pos = Row;
						SubPos = 5;
						if( IsEntry(Pos, SubPos) == TRUE ) return FALSE;
						return TRUE;
					}
				}
				break;
			case 3:		// 横幅
				if( WList.List[Row].GetWidth() ){
					Pos = Row;
					SubPos = 1;
					if( IsEntry(Pos, SubPos) == TRUE ) return FALSE;
					return TRUE;
				}
				break;
			case 4:		// 縦幅
				if( WList.List[Row].GetHeight() ){
					Pos = Row;
					SubPos = 2;
					if( IsEntry(Pos, SubPos) == TRUE ) return FALSE;
					return TRUE;
				}
				break;
			case 5:		// 全周
				if( WList.List[Row].GetRound() ){
					Pos = Row;
					SubPos = 3;
					if( IsEntry(Pos, SubPos) == TRUE ) return FALSE;
					return TRUE;
				}
				break;
			case 6:		// Ｒ
				if( WList.List[Row].GetR() ){
					Pos = Row;
					SubPos = 4;
					if( IsEntry(Pos, SubPos) == TRUE ) return FALSE;
					return TRUE;
				}
				break;
		}
	}
	return FALSE;
}
//---------------------------------------------------------------------------
int __fastcall TGrpWireDlg::Select(int &Pos, int &SubPos, ANTDEF *ap, CACal *pA)
{
	SelOnly = 1;
	pAnt = ap;
	pACal = pA;
	ChkRmd->Enabled = FALSE;
	ChkIntval->Checked = exeenv.IntPos;
	int RmdSel = exeenv.RmdSel;
	exeenv.RmdSel = 0;
	TabSheet1->Caption = "エレメント選択";
	TabSheet2->Caption = "ワイヤ選択";
	Caption = pAnt->Name;
	WList.SetWire(ap);
	Grid->RowCount = WList.gmax + 2;
	Grid->Options >> goEditing;
	Grid->Options << goDrawFocusSelected;
	if( Pos >= 0 ){
		if( SubPos < 0 ){
			SelWire = Pos;
		}
		else {
			Grid->Row = Pos + 1;
			Grid->Col = (SubPos < 5) ? SubPos + 2 : 2;
		}
	}
	AllViewBtnClick(NULL);
	ActiveControl = Grid;
	UpdateUIAll();
	if( ShowModal() == IDOK ){
		exeenv.RmdSel = RmdSel;
	    exeenv.EleWidthAll = EleWidthAll->Checked;
		if( Page->ActivePage == TabSheet1 ){
			if( IsSelect(Pos, SubPos, Grid->Row, Grid->Col) == TRUE ){
				if( EleWidthAll->Checked && (SubPos >= 1) && (SubPos <= 3) ){
					SubPos += 5;
                }
				return TRUE;
			}
		}
		else {
			if( SelWire != -1 ){
				Pos = SelWire;
				SubPos = -1;
				return TRUE;
			}
		}
	}
	exeenv.RmdSel = RmdSel;
    exeenv.EleWidthAll = EleWidthAll->Checked;
	return FALSE;
}
//---------------------------------------------------------------------------
void __fastcall TGrpWireDlg::UpdateUIAll(void)
{
	K6->Enabled = SelOnly ? FALSE : TRUE;
	if( (ActiveControl == Grid) && (!SelOnly) ){
		if( (Grid->Row - 1)!= WList.BaseW ){
			K2->Enabled = TRUE;
		}
		else {
			K2->Enabled = FALSE;
		}
		K3->Enabled = TRUE;
		K1->Enabled = TRUE;
		K9->Enabled = TRUE;
		if( (Grid->Col >= 2) && (Grid->Col <= 7) ){
			K13->Enabled = TRUE;
		}
		else {
			K13->Enabled = FALSE;
		}
	}
	else {
		K2->Enabled = FALSE;
		K3->Enabled = FALSE;
		K1->Enabled = FALSE;
		K9->Enabled = FALSE;
		K13->Enabled = FALSE;
	}
	KCSV1->Enabled = WList.gmax ? TRUE : FALSE;
	K14->Enabled = WList.gmax ? TRUE : FALSE;
	if( Page->ActivePage == TabSheet1 ){
		if( SelOnly ){
			HelpStat->Caption = "最適化する変数を選択して下さい.";
			if(ActiveControl == Grid){
				int Pos, SubPos;
				if( IsSelect(Pos, SubPos, Grid->Row, Grid->Col) == TRUE ){
					OKBtn->Enabled = TRUE;
				}
				else {
					OKBtn->Enabled = FALSE;
				}
			}
			else {
				OKBtn->Enabled = FALSE;
			}
		}
		else {
			HelpStat->Caption = "マウスの右ボタンをクリックするとメニューが開きます.";
		}
	}
	else {
		if( SelOnly ){
			HelpStat->Caption = "最適化するワイヤをクリックして下さい.";
			if( SelWire != -1 ){
				OKBtn->Enabled = TRUE;
			}
			else {
				OKBtn->Enabled = FALSE;
			}
		}
		else {
			HelpStat->Caption = "マウス左ボタン:ワイヤデータ表示，右ボタン:中心位置変更";
		}
	}
}
//---------------------------------------------------------------------------
// グループ定義のセル位置のテキストを返す
void __fastcall TGrpWireDlg::GridGetText(LPSTR t, long Col, long Row)
{
	LPCSTR _tt[]={
		"不明", "垂直DP", "垂直□", "垂直◇", "垂直△", "垂直▽", "垂直○", "垂直ﾙｰﾌﾟ", "水平DP", "水平□", "水平◇", "水平△", "水平▽", "水平ﾙｰﾌﾟ","水平系","垂直系"
	};

	*t = 0;
	if( Row ){
		Row--;
		switch(Col){
			case 0:
				if( Row < WList.gmax ){
					sprintf( t, "%3d", Row+1);
				}
				else {
					strcpy(t, "新規");
				}
				break;
			case 1:		// 種類
				if( Row < WList.gmax ){
					strcpy(t, _tt[WList.List[Row].Type]);
				}
				break;
			case 2:		// 間隔(m)
				if( Row < WList.gmax ){
					if( exeenv.IntPos ){
						if( Row == WList.BaseW ){
							strcpy(t, "基準位置");
						}
						else {
							strcpy(t, StrDbl(GetRmdVal(WList.Intval[Row])));
						}
					}
					else {
						strcpy(t, StrDbl(GetRmdVal(WList.GetXPosition(Row))));
					}
				}
				break;
			case 3:		// 横幅(m)
				if( Row < WList.gmax ){
					strcpy(t, StrDbl(GetRmdVal(WList.List[Row].GetWidth())));
				}
				break;
			case 4:		// 縦幅(m)
				if( Row < WList.gmax ){
					strcpy(t, StrDbl(GetRmdVal(WList.List[Row].GetHeight())));
				}
				break;
			case 5:		// 全周(m)
				if( Row < WList.gmax ){
					strcpy(t, StrDbl(GetRmdVal(WList.List[Row].GetRound())));
				}
				break;
			case 6:		// R(mm).
				if( Row < WList.gmax ){
					if( WList.List[Row].EnbR ){
						strcpy(t, StrDbl(GetRmdValD(WList.List[Row].GetR()*1000.0)));
					}
					else {
						strcpy(t, "複合");
					}
				}
				break;
			case 7:
				if( Row < WList.gmax ){
					if( WList.List[Row].EnbSeg ){
						strcpy(t, Seg2Str(WList.List[Row].GetSEG()));
					}
					else {
						strcpy(t, "複合");
					}
				}
				break;
			case 8:			// ワイヤ数
				if( Row < WList.gmax ){
					sprintf(t, "%u", WList.List[Row].bcnt);
				}
				break;
		}
	}
}
//---------------------------------------------------------------------
void __fastcall TGrpWireDlg::GridTitleText(LPSTR t, long Col, long Row)
{
	LPCSTR	_tt1[]={
		"No.","形式",NULL,"横幅(m)","縦幅(m)",NULL,"R(mm)","Seg.","ﾜｲﾔ数"
	};
	LPCSTR	_tt2[]={
		"No.","形式",NULL,"横幅(λ)","縦幅(λ)",NULL,"R(λ)","Seg.","ﾜｲﾔ数"
	};
	_tt1[2] = exeenv.IntPos  ? "間隔(m)":"位置(m)";
	_tt2[2] = exeenv.IntPos  ? "間隔(λ)":"位置(λ)";
	_tt1[5] = WList.List[Row-1].EnbXw() ? "Ｘ幅(m)":"全周(m)";
	_tt2[5] = WList.List[Row-1].EnbXw() ? "Ｘ幅(λ)":"全周(λ)";
	strcpy(t, ChkRmd->Checked == TRUE ? _tt2[Col] : _tt1[Col]);
}
//---------------------------------------------------------------------
void __fastcall TGrpWireDlg::GridDrawCell(TObject *Sender, int Col, int Row,
	TRect &Rect, TGridDrawState State)
{
	char	bf[64];

	Grid->Canvas->FillRect(Rect);
	int X = Rect.Left + 1;
	int Y = Rect.Top + 1;

	if( Row ){
		if( (Col == 2)&&((Row-1)==WList.BaseW) ) Grid->Canvas->Font->Color = clRed;
		GridGetText(bf, Col, Row);
		if( Col <= 6 ){
			Col -= 2;
			if( !exeenv.IntPos && !Col ) Col = 5;
			if( IsEntry(Row-1, Col) == TRUE ){
				char bbf[64];
				strcpy(bbf, bf);
				bf[0] = '*';
				strcpy(&bf[1], bbf);
			}
		}
		Grid->Canvas->TextOut(X, Y, bf);
	}
	else {		// タイトル
		GridTitleText(bf, Col, Grid->Row);
		X += 8;
		Grid->Canvas->TextOut(X, Y, bf);
	}

}
//---------------------------------------------------------------------------
// 編集前テキストを返す
void __fastcall TGrpWireDlg::GridGetEditText(TObject *Sender, int ACol,
	int ARow, UnicodeString &Value)
{
	char	bf[64];

	if( ACol == 1 ){
		Grid->EditorMode = FALSE;
		int X = Left + (Width - ClientWidth)/2 + TabSheet1->Left;
		int Y = Top + (Height - ClientHeight)/2 + TabSheet1->Top;
		X += Grid->ColWidths[0] + Grid->ColWidths[1];
		Y += ((Grid->Row - Grid->TopRow) * Grid->RowHeights[0]);
		PopupMenu->Popup(X, Y);
	}
	else {
		GridGetText(bf, ACol, ARow);
		Value = bf;
	}
}

//---------------------------------------------------------------------------
// 新規編集のチェックと作成
void __fastcall TGrpWireDlg::GridNewLine(int Row, int type /*=cwHDP*/)
{
	if( (Row >= WList.gmax) && (Row < CWMAX) ){
		WList.CreateWire(Row, type);
		Grid->RowCount = WList.gmax + 2;
		Grid->Invalidate();
		Edit |= 1;
	}
}

//---------------------------------------------------------------------------
// 編集テキストをから値を設定する
void __fastcall TGrpWireDlg::GridSetEditText(TObject *Sender, int ACol,
	int ARow, const UnicodeString Value)
{
	if( Grid->EditorMode == TRUE ) return;
	double	d;
	int		di;

	if( ARow ){
		ARow--;
		switch(ACol){
			case 1:		// 種類
				break;
			case 2:		// 間隔(m)
				if( Calc(d, AnsiString(Value).c_str()) == TRUE){
					if( d >= 0.0 ){
						d = SetRmdVal(d);
						if( exeenv.IntPos ){
							if( WList.BaseW == ARow ) break;
							GridNewLine(ARow);
							WList.Intval[ARow] = d;
						}
						else {
							if( !ARow ) break;
							GridNewLine(ARow);
							WList.SetXPosition(ARow, d);
						}
						Edit |= 1;
						Grid->Invalidate();
					}
				}
				break;
			case 3:		// 横幅(m)
				if( Calc(d, AnsiString(Value).c_str()) == TRUE){
					if( d >= 0.001 ){
						GridNewLine(ARow, cwHDP);
						WList.List[ARow].SetWidth(EleWidthAll->Checked, SetRmdVal(d));
						Edit |= 1;
						Grid->Invalidate();
					}
				}
				break;
			case 4:		// 縦幅(m)
				if( Calc(d, AnsiString(Value).c_str()) == TRUE){
					if( d >= 0.001 ){
						GridNewLine(ARow, cwVDP);
						WList.List[ARow].SetHeight(EleWidthAll->Checked, SetRmdVal(d));
						Edit |= 1;
						Grid->Invalidate();
					}
				}
				break;
			case 5:		// 全周(m)
				if( Calc(d, AnsiString(Value).c_str()) == TRUE){
					if( (d >= 0.001) ){
						GridNewLine(ARow, cwVSQ);
						WList.List[ARow].SetRound(EleWidthAll->Checked, SetRmdVal(d));
						Edit |= 1;
						Grid->Invalidate();
					}
				}
				break;
			case 6:		// R(mm).
				if( WList.List[ARow].EnbR ){
					if( Calc(d, AnsiString(Value).c_str()) == TRUE){
						GridNewLine(ARow);
						WList.List[ARow].SetR(SetRmdValD(d)/1000.0);
						Edit |= 1;
					}
				}
				break;
			case 7:		// Seg
				if( WList.List[ARow].EnbSeg ){
					if( Str2Seg(di, AnsiString(Value).c_str()) == TRUE ){
						GridNewLine(ARow);
						WList.List[ARow].SetSeg(di);
						Edit |= 1;
					}
				}
				break;
		}
	}
}
//---------------------------------------------------------------------------
// 編集の変更時の処理
void __fastcall TGrpWireDlg::SetModify(void)
{
	Edit |= 1;
	Grid->RowCount = WList.gmax + 2;
	Grid->Invalidate();
}

//---------------------------------------------------------------------------
// 水平DP
void __fastcall TGrpWireDlg::KS1Click(TObject *Sender)
{
	WList.SetNewType(Grid->Row - 1, cwHDP);
	SetModify();
}
//---------------------------------------------------------------------------
// 垂直DP
void __fastcall TGrpWireDlg::KS2Click(TObject *Sender)
{
	WList.SetNewType(Grid->Row - 1, cwVDP);
	SetModify();
}
//---------------------------------------------------------------------------
// 垂直□
void __fastcall TGrpWireDlg::KS3Click(TObject *Sender)
{
	WList.SetNewType(Grid->Row - 1, cwVSQ);
	SetModify();
}
//---------------------------------------------------------------------------
// 垂直◇
void __fastcall TGrpWireDlg::KS4Click(TObject *Sender)
{
	WList.SetNewType(Grid->Row - 1, cwVDM);
	SetModify();
}
//---------------------------------------------------------------------------
// 垂直▽
void __fastcall TGrpWireDlg::KS5Click(TObject *Sender)
{
	WList.SetNewType(Grid->Row - 1, cwVITA);
	SetModify();
}
//---------------------------------------------------------------------------
// 垂直△
void __fastcall TGrpWireDlg::KS6Click(TObject *Sender)
{
	WList.SetNewType(Grid->Row - 1, cwVTA);
	SetModify();
}
//---------------------------------------------------------------------------
// 垂直○
void __fastcall TGrpWireDlg::K5Click(TObject *Sender)
{
	WList.SetNewType(Grid->Row - 1, cwVPIE);
	SetModify();
}
//---------------------------------------------------------------------------
// 垂直双□
void __fastcall TGrpWireDlg::K7Click(TObject *Sender)
{
	WList.SetNewType(Grid->Row - 1, cwVSQ2);
	SetModify();
}
//---------------------------------------------------------------------------
// 垂直双▽
void __fastcall TGrpWireDlg::K8Click(TObject *Sender)
{
	WList.SetNewType(Grid->Row - 1, cwVTA2);
	SetModify();
}
//---------------------------------------------------------------------------
// 水平□
void __fastcall TGrpWireDlg::KS7Click(TObject *Sender)
{
	WList.SetNewType(Grid->Row - 1, cwHSQ);
	SetModify();
}
//---------------------------------------------------------------------------
// 水平□
void __fastcall TGrpWireDlg::KS8Click(TObject *Sender)
{
	WList.SetNewType(Grid->Row - 1, cwHDM);
	SetModify();
}
//---------------------------------------------------------------------------
// 水平▽
void __fastcall TGrpWireDlg::KS9Click(TObject *Sender)
{
	WList.SetNewType(Grid->Row - 1, cwHTA);
	SetModify();
}
//---------------------------------------------------------------------------
// 水平△
void __fastcall TGrpWireDlg::KS10Click(TObject *Sender)
{
	WList.SetNewType(Grid->Row - 1, cwHITA);
	SetModify();
}
//---------------------------------------------------------------------------
// ポップアップ更新ハンドラ
void __fastcall TGrpWireDlg::PopupMenuPopup(TObject *Sender)
{
	UpdateUIAll();
}
//---------------------------------------------------------------------------
// この行を削除
void __fastcall TGrpWireDlg::K2Click(TObject *Sender)
{
	if( Grid->Row && (Grid->Row <= WList.gmax) ){
		WList.DeleteItem(Grid->Row-1);
		SetModify();
	}
}
//---------------------------------------------------------------------------
// この行に挿入
void __fastcall TGrpWireDlg::K3Click(TObject *Sender)
{
	if( Grid->Row && (WList.gmax < CWMAX) ){
		WList.InsertItem(Grid->Row-1);
		SetModify();
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TGrpWireDlg::PBoxPaint(TObject *Sender)
{
	TRect	rc;

	PBox->Canvas->Pen->Width = 1;
	rc.Left = 0;
	rc.Top = 0;
	rc.Right = PBox->Width;
	rc.Bottom = PBox->Height;
	Clip.SetClip(-100, -100, rc.Right + 100, rc.Bottom + 100);
	PBox->Canvas->Brush->Color = clWhite;
	PBox->Canvas->FillRect(rc);
	double sc = double(TBarSC->Position) / 20.0;
	sc = sc * sc * sc * sc;
	int		Xc = int(PBox->Width/2 + (AntXc * sc));
	int		Yc = int(PBox->Height/2 + (AntYc * sc));

	PBox->Canvas->Pen->Color = clWhite;
	double x, y;
	int		X,Y, X2, Y2;
	double deg = double(TBarDeg->Position);
	deg *= (PAI / 180.0);
	double zdeg = double(TBarZDeg->Position);
	zdeg *= (PAI / 180.0);

	PBox->Canvas->Pen->Color = clLtGray;
	Calc3DXY(x, y, deg, zdeg, 0, 0, 0);				// Y
	X = int(x) + Xc;
	Y = Yc - int(y);
	PBox->Canvas->MoveTo(X, Y);
	Calc3DXY(x, y, deg, zdeg, 0, PBox->Width/2-16, 0);
	X = int(x) + Xc;
	Y = Yc - int(y);
	PBox->Canvas->LineTo(X, Y);
	int Sop = ::SetBkMode(PBox->Canvas->Handle, TRANSPARENT);
	PBox->Canvas->TextOut(X, Y, "Y");

	Calc3DXY(x, y, deg, zdeg, 0, 0, 0);				// Z
	X = int(x) + Xc;
	Y = Yc - int(y);
	PBox->Canvas->MoveTo(X, Y);
	Calc3DXY(x, y, deg, zdeg, 0, 0, PBox->Height);
	X = int(x) + Xc;
	Y = Yc - int(y);
	PBox->Canvas->LineTo(X, Y);
	::SetBkMode(PBox->Canvas->Handle, TRANSPARENT);
	PBox->Canvas->TextOut(X+4, 0, "Z");

	Calc3DXY(x, y, deg, zdeg, 0, 0, 0);				// X
	X = int(x) + Xc;
	Y = Yc - int(y);
	PBox->Canvas->MoveTo(X, Y);
	Calc3DXY(x, y, deg, zdeg, PBox->Height/2, 0, 0);
	X = int(x) + Xc;
	Y = Yc - int(y);
	PBox->Canvas->LineTo(X, Y);
	::SetBkMode(PBox->Canvas->Handle, TRANSPARENT);
	PBox->Canvas->TextOut(X, Y, "X");

	// ワイヤの表示
	int i, j;
	WREF	*rp;
	double	bx = 0.0;
	for( i = WList.BaseW; i >= 0; i-- ){
		PBox->Canvas->Pen->Color = (i == WList.BaseW) ? clRed : clBlack;
		bx += WList.Intval[i];
		for( rp = WList.List[i].pB, j = 0; j < WList.List[i].bcnt; j++, rp++ ){
			if( SelOnly ){
				PBox->Canvas->Pen->Color = (IsEntry(rp->ref) == TRUE) ? clRed : clBlack;
			}
			PBox->Canvas->Pen->Width = (rp->ref == SelWire) ? 2 : 1;
			Calc3DXY(x, y, deg, zdeg, rp->wp->X1 - bx, rp->wp->Y1, rp->wp->Z1);
			X = int((x * sc)) + Xc;
			Y = Yc - int((y * sc));
			Calc3DXY(x, y, deg, zdeg, rp->wp->X2 - bx, rp->wp->Y2, rp->wp->Z2);
			X2 = int((x * sc)) + Xc;
			Y2 = Yc - int((y * sc));
			if( Clip.Clip(X, Y, X2, Y2) == TRUE ){
				PBox->Canvas->MoveTo(X, Y);
				PBox->Canvas->LineTo(X2, Y2);
			}
		}
	}
	PBox->Canvas->Pen->Color = clBlack;
	bx = 0.0;
	for( i = WList.BaseW + 1; i < WList.gmax; i++ ){
		bx += WList.Intval[i];
		for( rp = WList.List[i].pB, j = 0; j < WList.List[i].bcnt; j++, rp++ ){
			if( SelOnly ){
				PBox->Canvas->Pen->Color = (IsEntry(rp->ref) == TRUE) ? clRed : clBlack;
			}
			PBox->Canvas->Pen->Width = (rp->ref == SelWire) ? 2 : 1;
			Calc3DXY(x, y, deg, zdeg, rp->wp->X1 + bx, rp->wp->Y1, rp->wp->Z1);
			X = int((x * sc)) + Xc;
			Y = Yc - int((y * sc));
			Calc3DXY(x, y, deg, zdeg, rp->wp->X2 + bx, rp->wp->Y2, rp->wp->Z2);
			X2 = int((x * sc)) + Xc;
			Y2 = Yc - int((y * sc));
			if( Clip.Clip(X, Y, X2, Y2) == TRUE ){
				PBox->Canvas->MoveTo(X, Y);
				PBox->Canvas->LineTo(X2, Y2);
			}
		}
	}
	if( SelWire >= 0 ){
    	if( SelOnly ){
			ANTDEF	*ap = (pACal != NULL) ? pACal->AP : WList.ap;
			DrawWirePara(PBox, ap, SelWire);
        }
        else {
			DrawWirePara(PBox, &WList, SelWire);
        }
	}
	::SetBkMode(PBox->Canvas->Handle, Sop);
}
//---------------------------------------------------------------------------
void __fastcall TGrpWireDlg::TBarDegChange(TObject *Sender)
{
	PBox->Invalidate();
}
//---------------------------------------------------------------------------
// マウスによるワイヤの選択
int __fastcall TGrpWireDlg::SelectWire(int X, int Y)
{
	// ワイヤの検索
	double sc = double(TBarSC->Position) / 20.0;
	sc = sc * sc * sc * sc;
	int	Xc = int(PBox->Width/2 + (AntXc * sc));
	int	Yc = int(PBox->Height/2 + (AntYc * sc));
	double x, y;
	double deg = double(TBarDeg->Position);
	deg *= (PAI / 180.0);
	double zdeg = double(TBarZDeg->Position);
	zdeg *= (PAI / 180.0);
	POINT	PP, BP, EP;
	PP.x = X; PP.y = Y;

	int i, j;
	WREF	*rp;
	double	bx = 0.0;
	int mw = 9;
	int w, mi;
	for( i = WList.BaseW; i >= 0; i-- ){
		bx += WList.Intval[i];
		for( rp = WList.List[i].pB, j = 0; j < WList.List[i].bcnt; j++, rp++ ){
			Calc3DXY(x, y, deg, zdeg, rp->wp->X1 - bx, rp->wp->Y1, rp->wp->Z1);
			BP.x = int((x * sc)) + Xc;
			BP.y = Yc - int((y * sc));
			Calc3DXY(x, y, deg, zdeg, rp->wp->X2 - bx, rp->wp->Y2, rp->wp->Z2);
			EP.x = int((x * sc)) + Xc;
			EP.y = Yc - int((y * sc));
			for( w = 1; w < mw; w++ ){
				if( IsLine(PP, BP, EP, w) != 0 ){
					mw = w;
					mi = rp->ref;
					break;
				}
			}
		}
	}
	bx = 0.0;
	for( i = WList.BaseW + 1; i < WList.gmax; i++ ){
		bx += WList.Intval[i];
		for( rp = WList.List[i].pB, j = 0; j < WList.List[i].bcnt; j++, rp++ ){
			Calc3DXY(x, y, deg, zdeg, rp->wp->X1 + bx, rp->wp->Y1, rp->wp->Z1);
			BP.x = int((x * sc)) + Xc;
			BP.y = Yc - int((y * sc));
			Calc3DXY(x, y, deg, zdeg, rp->wp->X2 + bx, rp->wp->Y2, rp->wp->Z2);
			EP.x = int((x * sc)) + Xc;
			EP.y = Yc - int((y * sc));
			for( w = 1; w < mw; w++ ){
				if( IsLine(PP, BP, EP, w) != 0 ){
					mw = w;
					mi = rp->ref;
					break;
				}
			}
		}
	}
	if( mw != 9 ){
		SelWire = mi;
		return TRUE;
	}
	return FALSE;
}
//---------------------------------------------------------------------------
void __fastcall TGrpWireDlg::PBoxMouseDown(TObject *Sender, TMouseButton Button,
	TShiftState Shift, int X, int Y)
{
	if( Button == mbLeft ){
		if( SelectWire(X, Y) == TRUE ){
			UpdateUIAll();
			PBox->Invalidate();
			if( !SelOnly ){
				Grid->Row = WList.GetEleNo(SelWire) + 1;
			}
		}
		return;
	}
	else {
		double sc = double(TBarSC->Position) / 20.0;
		sc = sc * sc * sc * sc;
		AntXc += double((PBox->Width/2) - X)/sc;
		AntYc += double((PBox->Height/2) - Y)/sc;
		PBox->Invalidate();
	}
}
//---------------------------------------------------------------------------
void __fastcall TGrpWireDlg::OrgBtnClick(TObject *Sender)
{
	AntXc = AntYc = 0.0;
	PBox->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TGrpWireDlg::AllViewBtnClick(TObject *Sender)
{
	double	XL, XH, YL, YH, D, x, y;
	int		i, pos;

	if( !WList.gmax ) return;
	double deg = double(TBarDeg->Position);
	deg *= (PAI / 180.0);
	double zdeg = double(TBarZDeg->Position);
	zdeg *= (PAI / 180.0);
	XL = MAXDOUBLE; XH = -MAXDOUBLE;
	YL = MAXDOUBLE; YH = -MAXDOUBLE;
	int j;
	WREF	*rp;
	double	bx = 0.0;
	pos = 0;
	for( i = WList.BaseW; i >= 0; i-- ){
		bx += WList.Intval[i];
		for( rp = WList.List[i].pB, j = 0; j < WList.List[i].bcnt; j++, rp++ ){
			Calc3DXY(x, y, deg, zdeg, rp->wp->X1 - bx, rp->wp->Y1, rp->wp->Z1);
			if( XL > x ) XL = x;
			if( XH < x ) XH = x;
			if( YL > y ) YL = y;
			if( YH < y ) YH = y;
			Calc3DXY(x, y, deg, zdeg, rp->wp->X2 - bx, rp->wp->Y2, rp->wp->Z2);
			if( XL > x ) XL = x;
			if( XH < x ) XH = x;
			if( YL > y ) YL = y;
			if( YH < y ) YH = y;
			pos = 1;
		}
	}
	bx = 0.0;
	for( i = WList.BaseW + 1; i < WList.gmax; i++ ){
		bx += WList.Intval[i];
		for( rp = WList.List[i].pB, j = 0; j < WList.List[i].bcnt; j++, rp++ ){
			Calc3DXY(x, y, deg, zdeg, rp->wp->X1 + bx, rp->wp->Y1, rp->wp->Z1);
			if( XL > x ) XL = x;
			if( XH < x ) XH = x;
			if( YL > y ) YL = y;
			if( YH < y ) YH = y;
			Calc3DXY(x, y, deg, zdeg, rp->wp->X2 + bx, rp->wp->Y2, rp->wp->Z2);
			if( XL > x ) XL = x;
			if( XH < x ) XH = x;
			if( YL > y ) YL = y;
			if( YH < y ) YH = y;
			pos = 1;
		}
	}
	if( pos ){
		AntXc = -(XL + XH)/2;
		AntYc = (YL + YH)/2;
		D = ABS(XH - XL);
		if( D < ABS(YH - YL) ) D = ABS(YH - YL);
		D *= 1.2;
		if( D > 0.0 ){
			D = (PBox->Height)/D;
			pos = int(sqrt(sqrt(D)) * 20.0);
			if( pos > TBarSC->Max ){
				pos = TBarSC->Max;
			}
		}
		else {
			pos = TBarSC->Max;
		}
	}
	else {
		AntXc = AntYc = 0;
		pos = 50;
	}
	TBarSC->Position = pos;
#if 0
	D = ABS(XL);
	if( D < ABS(XH) ) D = ABS(XH);
	if( D < ABS(YL) ) D = ABS(YL);
	if( D < ABS(YH) ) D = ABS(YH);
	D *= 1.1;
	if( D > 0.0 ){
		D = (PBox->Height/2) / D;
		pos = int(sqrt(sqrt(D)) * 20.0);
		if( pos > TBarSC->Max ){
			pos = TBarSC->Max;
		}
	}
	else {
		pos = TBarSC->Max;
	}
	TBarSC->Position = pos;
	AntXc = AntYc = 0.0;
#endif
	PBox->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TGrpWireDlg::PageChange(TObject *Sender)
{
	UpdateUIAll();
}
//---------------------------------------------------------------------------
// サイズ変更コントロールの登録
void __fastcall TGrpWireDlg::EntryAlignControl(void)
{
	RECT	rc;
	rc.left = 0;
	rc.top = 0;
	rc.right = 799;
	rc.bottom = 599;

	AlignList.EntryControl(OKBtn, &rc, OKBtn->Font);
	AlignList.EntryControl(CancelBtn, &rc, CancelBtn->Font);
	AlignList.EntryControl(Page, &rc, Page->Font);
	AlignList.EntryControl(Grid, &rc, Grid->Font);
	AlignList.EntryControl(HelpStat, &rc, HelpStat->Font);
	AlignList.EntryControl(PBox, &rc, PBox->Font);
	AlignList.EntryControl(TBarDeg, &rc, NULL);
	AlignList.EntryControl(TBarZDeg, &rc, NULL);
	AlignList.EntryControl(TBarSC, &rc, NULL);
	AlignList.EntryControl(AllViewBtn, &rc, AllViewBtn->Font);
	AlignList.EntryControl(OrgBtn, &rc, OrgBtn->Font);
	AlignList.EntryControl(Label1, &rc, Label1->Font);
	AlignList.EntryControl(Label2, &rc, Label2->Font);
	AlignList.EntryControl(Label3, &rc, Label2->Font);
	AlignList.EntryControl(ChkRmd, &rc, ChkRmd->Font);
	AlignList.EntryControl(ChkIntval, &rc, ChkIntval->Font);
	AlignList.EntryControl(Ant3D, &rc, Ant3D->Font);
	AlignList.EntryControl(EleWidthAll, &rc, EleWidthAll->Font);
	AlignList.EntryControl(EleWidthEdge, &rc, EleWidthEdge->Font);

	AlignGrid.InitGrid(Grid);

	int CX = ::GetSystemMetrics(SM_CXFULLSCREEN);
	int CY = ::GetSystemMetrics(SM_CYFULLSCREEN);
//	int CX = ::GetSystemMetrics(SM_CXSCREEN);
//	int CY = ::GetSystemMetrics(SM_CYSCREEN);
	if( (CX < Width)||(CY < Height) ){
		Top = 0;
		Left = 0;
		Width = CX;
		Height = CY;
	}
	FormCenter(this, CX, CY);
	if( Owner != NULL ){
		WindowState = ((TForm *)Owner)->WindowState;
	}
}
//---------------------------------------------------------------------------
void __fastcall TGrpWireDlg::FormResize(TObject *Sender)
{
	AlignList.NewAlign(this);
	AlignGrid.NewAlign(Grid);

	PBox->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TGrpWireDlg::GridSelectCell(TObject *Sender, int Col, int Row,
	bool &CanSelect)
{
	Grid->Invalidate();
	if( SelOnly ){
		int Pos, SubPos;
		if( IsSelect(Pos, SubPos, Row, Col) == TRUE ){
			OKBtn->Enabled = TRUE;
		}
		else {
			OKBtn->Enabled = FALSE;
		}
	}
	else if( Row <= WList.gmax ){
		SelWire = WList.List[Row-1].pB->ref;
	}
}
//---------------------------------------------------------------------------
// 電卓
void __fastcall TGrpWireDlg::K4Click(TObject *Sender)
{
	if( DoCalc() == FALSE ){
		K4->Enabled = FALSE;
	}
}
#if SAMECHECK
int __fastcall TGrpWireDlg::IsSameElement(int i, int j)
{
	if( WList.List[i].bcnt != WList.List[j].bcnt ) return FALSE;
	WREF	*rp, *tp;

	int k;
	rp = WList.List[i].pB;
	tp = WList.List[j].pB;
	for( k = 0; k < WList.List[i].bcnt; k++, rp++, tp++ ){
		if( rp->wp->X1 != tp->wp->X1 ) return FALSE;
		if( rp->wp->Y1 != tp->wp->Y1 ) return FALSE;
		if( rp->wp->Z1 != tp->wp->Z1 ) return FALSE;
		if( rp->wp->X2 != tp->wp->X2 ) return FALSE;
		if( rp->wp->Y2 != tp->wp->Y2 ) return FALSE;
		if( rp->wp->Z2 != tp->wp->Z2 ) return FALSE;
	}
	return TRUE;
}
//---------------------------------------------------------------------------
// 同一間隔の同一エレメントが存在しないかチェックする
int __fastcall TGrpWireDlg::IsSameElement(void)
{
	int i, j;

	for( i = WList.BaseW; i > 0; i-- ){		// 後方にチェック
		for( j = i - 1; (j >= 0) && (!WList.Intval[j]); j-- ){
			if( IsSameElement(i, j) == TRUE ){
				WarningMB("No.%d と No.%d はまったく同一のエレメントです", i+1, j+1);
				return TRUE;
			}
		}
	}
	for( i = WList.BaseW; i < (WList.gmax - 1); i++ ){		// 前方にチェック
		for( j = i + 1; (j < WList.gmax) && (!WList.Intval[j]); j++ ){
			if( IsSameElement(i, j) == TRUE ){
				WarningMB("No.%d と No.%d はまったく同一のエレメントです", i+1, j+1);
				return TRUE;
			}
		}
	}
	return FALSE;
}
#endif
//---------------------------------------------------------------------------
// ＯＫボタンが押された時
void __fastcall TGrpWireDlg::OKBtnClick(TObject *Sender)
{
#if SAMECHECK
	if( Edit & 1 ){
		if( IsSameElement() == TRUE ) return;
	}
#endif
	ModalResult = IDOK;
}
//---------------------------------------------------------------------------

void __fastcall TGrpWireDlg::ChkRmdClick(TObject *Sender)
{
	exeenv.RmdSel = ChkRmd->Checked;
	Grid->Invalidate();
}
//---------------------------------------------------------------------------
// 間隔・位置表現
void __fastcall TGrpWireDlg::ChkIntvalClick(TObject *Sender)
{
	exeenv.IntPos = ChkIntval->Checked;
	Grid->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TGrpWireDlg::Ant3DClick(TObject *Sender)
{
	exeenv.Ant3D = Ant3D->Checked;
	TBarZDeg->Enabled = exeenv.Ant3D ? TRUE : FALSE;
	AllViewBtnClick(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TGrpWireDlg::K6Click(TObject *Sender)
{
	TWCombDlg *Box = new TWCombDlg(this);

	if( Box->Execute(pAnt) == TRUE ){
		AdjPdef(pAnt);
		Edit |= 2;
	}
	delete Box;
}
//---------------------------------------------------------------------------
void __fastcall TGrpWireDlg::K10Click(TObject *Sender)
{
	AllRoundUp(100);
}
//---------------------------------------------------------------------------
void __fastcall TGrpWireDlg::K11Click(TObject *Sender)
{
	AllRoundUp(1000);
}
//---------------------------------------------------------------------------
void __fastcall TGrpWireDlg::K12Click(TObject *Sender)
{
	AllRoundUp(10000);
}
//---------------------------------------------------------------------------
void __fastcall TGrpWireDlg::AllRoundUp(double sc)
{
	if( YesNoMB("エレメントの小数点を丸めます. ほんまにやりますか？") != IDYES ) return;
	CWGroup *gp = WList.List;
	int i;
	double R;
	for( i = 0; i < WList.gmax; i++, gp++ ){
		if( gp->EnbXw() ){
			if( gp->GetWidth() ){
				gp->SetWidth(EleWidthAll->Checked, SetRmdVal(RoundUp(GetRmdVal(gp->GetWidth()), sc)));
			}
			if( gp->GetHeight() ){
				gp->SetHeight(EleWidthAll->Checked, SetRmdVal(RoundUp(GetRmdVal(gp->GetHeight()), sc)));
			}
			if( gp->GetRound() ){
				gp->SetRound(EleWidthAll->Checked, SetRmdVal(RoundUp(GetRmdVal(gp->GetRound()), sc)));
			}
		}
		else if( gp->GetRound() ){
			gp->SetRound(EleWidthAll->Checked, SetRmdVal(RoundUp(GetRmdVal(gp->GetRound()), sc)));
		}
		if( WList.GetXPosition(i) ){
			WList.SetXPosition(i, SetRmdVal(RoundUp(GetRmdVal(WList.GetXPosition(i)), sc)));
		}
		if( gp->EnbR && (gp->GetR() > 0.0) ){
			if( exeenv.RmdSel ){
				R = SetRmdVal(RoundUp(GetRmdVal(gp->GetR()), sc));
				if( !R ) R = SetRmdVal(1/sc);
			}
			else {
				R = RoundUp(gp->GetR(), sc * 1000.0);
				if( !R ) R = 1/sc*1000.0;
			}
			gp->SetR(R);
		}
	}
	Edit |= 1;
	Grid->Invalidate();
}
//---------------------------------------------------------------------------
// 検索と置換
void __fastcall TGrpWireDlg::K13Click(TObject *Sender)
{
	char	bf[256];
	double	nd, od, dd;

	GridGetText(bf, Grid->Col, Grid->Row);
	if( Calc(od, bf) == TRUE ){
		TValRepDlg *Box = new TValRepDlg(this);

		if( Box->Execute(nd, od) == TRUE ){
			Grid->EditorMode = FALSE;
			AnsiString as = StrDbl(nd);
			int i;
			for( i = 0; i < WList.gmax; i++ ){
				GridGetText(bf, Grid->Col, i+1);
				if( Calc(dd, bf) == TRUE ){
					if( od == dd ){
						GridSetEditText(NULL, Grid->Col, i + 1, as);
					}
				}
			}
			Grid->Invalidate();
		}
		delete Box;
	}
}
//---------------------------------------------------------------------------
// CSVファイルの作成
void __fastcall TGrpWireDlg::KCSV1Click(TObject *Sender)
{
	char	bf[512];
	AnsiString	FileName;

	MainWnd->SaveDialog->Title = "CSVテキストファイルに保存";
	MainWnd->SaveDialog->Filter = "CSVファイル(*.csv)|*.csv|";
	if( *MainWnd->antFname.c_str() ){
		strcpy(bf, MainWnd->antFname.c_str());
		SetEXT(bf, ".csv");
		MainWnd->SaveDialog->FileName = GetFileName(bf);
	}
	else {
		MainWnd->SaveDialog->FileName = "無題";
	}
	MainWnd->SaveDialog->DefaultExt = "csv";
	MainWnd->SaveDialog->InitialDir = ResDir;
	if( MainWnd->SaveDialog->Execute() == TRUE ){
		FileName = MainWnd->SaveDialog->FileName;
        FILE *fp;

		CWaitCursor	tw;
        if( (fp = fopen(FileName.c_str(), "wt"))!=NULL ){
			int		IntPos = exeenv.IntPos;
			int i, j, n;

           	for( j = n = 0; j < 10; j++ ){
				n = (j >= 3) ? j - 1 : j;
				exeenv.IntPos = (j == 3) ? 1 : 0;
				GridTitleText(bf, n, 1);
				if( j ) fprintf(fp, ",");
				fprintf(fp, "%s", bf);
            }
			fprintf(fp, "\n");
			for( i = 1; i <= WList.gmax; i++ ){
            	for( j = 0; j < 10; j++ ){
					n = (j >= 3) ? j - 1 : j;
					exeenv.IntPos = (j == 3) ? 1 : 0;
		            GridGetText(bf, n, i);
					if( j ) fprintf(fp, ",");
					fprintf(fp, "%s", bf);
                }
                fprintf(fp, "\n");
            }
			exeenv.IntPos = IntPos;
			if( fclose(fp) ){
				ErrorMB("'%s'の作成に失敗しました.", FileName.c_str());
            }
        }
        else {
			ErrorMB("'%s'を作成できません.", FileName.c_str());
        }
	}
}
//---------------------------------------------------------------------------
void __fastcall TGrpWireDlg::K14Click(TObject *Sender)
{
	int		IntPos = exeenv.IntPos;
	CWaitCursor	tw;
	TPrintDlgBox *Box = new TPrintDlgBox(this);

    char	bf[512];

	AnsiString	DocTitle;

	if( *MainWnd->antFname.c_str() ){
		DocTitle = GetFileName(MainWnd->antFname.c_str());
	}
	else {
		DocTitle = "無題";
	}

	Box->SetDocTitle(DocTitle.c_str());

    const int _wt[]={		// 各セルの配分
		4, 10, 10, 10, 10, 10, 10, 8, 8, 6
    };
	int i, j, n;
    exeenv.IntPos = 0;
	Box->SetMargin(5, 3, 3, 3);		// マージンの設定
    for( i = n = 0; i < 10; i++ ){
        Box->SetWidth(i, _wt[i]);
		n = (i >= 3) ? i - 1 : i;
		exeenv.IntPos = (i == 3) ? 1 : 0;
		GridTitleText(bf, n, 1);
		Box->SetTitle(i, bf);
    }
	for( i = 1; i <= WList.gmax; i++ ){
       	for( j = n = 0; j < 10; j++ ){
			n = (j >= 3) ? j - 1 : j;
			exeenv.IntPos = (j == 3) ? 1 : 0;
            GridGetText(bf, n, i);
            Box->SetString(i-1, j, bf);
        }
    }
	exeenv.IntPos = IntPos;
    tw.Delete();
	Box->Execute();
    delete Box;
}
//---------------------------------------------------------------------------
