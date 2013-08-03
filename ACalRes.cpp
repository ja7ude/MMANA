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

#include "ACalRes.h"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
//TACalResDlg *ACalResDlg;
//---------------------------------------------------------------------
__fastcall TACalResDlg::TACalResDlg(TComponent* AOwner)
	: TForm(AOwner)
{
	EntryAlignControl();

	mmax = 0;
	memset(mval, 0, sizeof(mval));
	pACal = NULL;
	CurRow = -1;
	MaxV = MaxH = MaxD = MaxG = 0;
	memset(PtnH, 0, sizeof(PtnH));
	memset(PtnV, 0, sizeof(PtnV));
	DrawPtnH.SetRect(PBox->Canvas, 0, 0, PBox->Width / 2 - 2, PBox->Height);
	DrawPtnV.SetRect(PBox->Canvas, (PBox->Width/2) + 2, 0, PBox->Width, PBox->Height);
}
//---------------------------------------------------------------------
int __fastcall TACalResDlg::Execute(CACal *p)
{
	pACal = p;
	mmax = pACal->GetMV();
	int i;
	MVAL *mp = mval;
	for( i = 0; i < mmax; i++, mp++ ){
		pACal->ReadMV(mp, i);
	}
	Grid->RowCount = mmax + 1;
	char bf[256];
	pACal->SetTradeStr(bf);
	CalCond->Caption = bf;
	if( !pACal->atrg.TF ){
		if( pACal->amax >= 2 ){
			Grid->ColCount = Grid->ColCount + pACal->amax - 1;
		}
	}
	Sort(0);
	ScrollIndex(GetCurIndex());
	SetupPtn(Grid->Row);

	// ダイアログボックスを開く
	if( ShowModal() == IDOK ){
		if( Grid->Row ){
			int Row = Grid->Row - 1;
			for( i = 0; i < pACal->amax; i++ ){
				pACal->SetPara(i, mval[Row].AD[i]);
			}
			pACal->WList.Update();
			return TRUE;
		}
	}
	return FALSE;
}
//---------------------------------------------------------------------
void __fastcall TACalResDlg::GridDrawCell(TObject *Sender, int Col, int Row,
	TRect &Rect, TGridDrawState State)
{
	char	bf[64];
	char	bf2[32];

	Grid->Canvas->FillRect(Rect);
	int X = Rect.Left + 4;
	int Y = Rect.Top + 2;

	if( Row ){
		Row--;
		bf[0] = 0;
		switch(Col){
			case 0:		// No.
				sprintf(bf, "%u", Row + 1);
				break;
			case 1:		// Z
				strcpy(bf2, StrDbl2(ABS(mval[Row].CJX)));
				sprintf( bf, "%s %c j%s", StrDbl1(mval[Row].CRR), (mval[Row].CJX < 0.0)?'-':'+', bf2 );
				break;
			case 2:		// SWR
				strcpy(bf, StrDbl2(mval[Row].CSWR));
				break;
			case 3:		// Gain
				strcpy(bf, StrDbl2(mval[Row].CG));
				break;
			case 4:		// F/B
				strcpy(bf, StrDbl2(mval[Row].CFB));
				break;
			case 5:		// Elv
				strcpy(bf, StrDbl1(mval[Row].CELV));
				break;
			default:	// 評価値
				if( !pACal->atrg.TF ){
					strcpy(bf, StrDbl(mval[Row].AD[Col-6]));
				}
				else if( Col == 6 ){
					strcpy(bf, StrDbl(mval[Row].V/pACal->atrg.VM));
				}
				break;
		}
		Grid->Canvas->TextOut(X, Y, bf);
	}
	else {		// タイトル
		LPCSTR	_tt[]={
			"No.","Z(Ω)","SWR(50)","Ga(dBi)","F/B(dB)","Elev.°","評価値"
		};
		if( (Col < 6) || (pACal->atrg.TF) ){
			Grid->Canvas->TextOut(X, Y, _tt[Col]);
		}
		else {
			sprintf(bf, "V%d", Col - 5 );
			Grid->Canvas->TextOut(X, Y, bf);
		}
	}
}
//---------------------------------------------------------------------------
// ソートセル
void __fastcall TACalResDlg::SortSelChange(TObject *Sender)
{
	if( !strcmp(AnsiString(SortSel->Text).c_str(), "評価値") ){
		Sort(0);
	}
	else if( !strcmp(AnsiString(SortSel->Text).c_str(), "jX") ){
		Sort(1);
	}
	else if( !strcmp(AnsiString(SortSel->Text).c_str(), "SWR") ){
		Sort(2);
	}
	else if( !strcmp(AnsiString(SortSel->Text).c_str(), "Gain") ){
		Sort(3);
	}
	else if( !strcmp(AnsiString(SortSel->Text).c_str(), "F/B") ){
		Sort(4);
	}
	else if( !strcmp(AnsiString(SortSel->Text).c_str(), "Elev.") ){
		Sort(5);
	}
	SetupPtn(Grid->Row);
	ScrollIndex(GetCurIndex());
	Grid->SetFocus();
	Grid->Invalidate();
}
//---------------------------------------------------------------------------
// ソート
void __fastcall TACalResDlg::Sort(int n)
{
	// 結果のソート
	int	gap, i, j;
	MVAL	mv;
	for( gap = mmax/2; gap > 0; gap /= 2 ){
		for( i = gap; i < mmax; i++ ){
			for( j = i - gap; j >= 0; j -= gap ){
				switch(n){
					case 0:
						if( mval[j].V >= mval[j+gap].V ) goto _nx;
						break;
					case 1:
						if( ABS(mval[j].CJX) <= ABS(mval[j+gap].CJX) ) goto _nx;
						break;
					case 2:
						if( mval[j].CSWR <= mval[j+gap].CSWR ) goto _nx;
						break;
					case 3:
						if( mval[j].CG >= mval[j+gap].CG ) goto _nx;
						break;
					case 4:
						if( mval[j].CFB >= mval[j+gap].CFB ) goto _nx;
						break;
					case 5:
						if( mval[j].CELV <= mval[j+gap].CELV ) goto _nx;
						break;
				}
				memcpy(&mv, &mval[j], sizeof(MVAL));
				memcpy(&mval[j], &mval[j+gap], sizeof(MVAL));
				memcpy(&mval[j+gap], &mv, sizeof(MVAL));
			}
			_nx:;
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TACalResDlg::PBoxPaint(TObject *Sender)
{
	DrawPtnH.DrawAngle(2);
	DrawPtnV.DrawAngle(2);
	DrawPtnH.SetMaxDB(MaxG);
	DrawPtnV.SetMaxDB(MaxG);
	DrawPtnH.DrawPtn(PtnH, 2);
	DrawPtnV.DrawPtn(PtnV, 2);

	int	X, Y;
	DrawPtnV.GetBottomPosition(X, Y);
	Y += 4;
	char bf[64];
	sprintf( bf, "%s(dBi) = 0dB", StrDbl2(MaxD));
	int Sop = ::SetBkMode(PBox->Canvas->Handle, TRANSPARENT);
	PBox->Canvas->TextOut(X, Y, bf);
	::SetBkMode(PBox->Canvas->Handle, Sop);
}

//---------------------------------------------------------------------------
// サイズ変更コントロールの登録
void __fastcall TACalResDlg::EntryAlignControl(void)
{
	RECT	rc;
	rc.left = 0;
	rc.top = 0;
	rc.right = 639;
	rc.bottom = 599;

	AlignList.EntryControl(OKBtn, &rc, OKBtn->Font);
	AlignList.EntryControl(CancelBtn, &rc, CancelBtn->Font);
	AlignList.EntryControl(CalCond, &rc, CalCond->Font);
	AlignList.EntryControl(Grid, &rc, Grid->Font);
	AlignList.EntryControl(SortSel, &rc, SortSel->Font);
	AlignList.EntryControl(Label1, &rc, Label1->Font);
	AlignList.EntryControl(PBox, &rc, PBox->Font);

	AlignGrid.InitGrid(Grid);

	int CX = ::GetSystemMetrics(SM_CXFULLSCREEN);
	int CY = ::GetSystemMetrics(SM_CYFULLSCREEN);
//	int CX = ::GetSystemMetrics(SM_CXSCREEN);
//	int CY = ::GetSystemMetrics(SM_CYSCREEN);
	if( CX < Width ){
		Left = 0;
		Width = CX;
	}
	if( CY < Height ){
		Top = 0;
		Height = CY;
	}
	FormCenter(this, CX, CY);
	if( Owner != NULL ){
		WindowState = ((TForm *)Owner)->WindowState;
	}
}
//---------------------------------------------------------------------------
void __fastcall TACalResDlg::FormResize(TObject *Sender)
{
	AlignList.NewAlign(this);
	AlignGrid.NewAlign(Grid);

	DrawPtnH.SetRect(PBox->Canvas, 0, 0, PBox->Width / 2 - 2, PBox->Height);
	DrawPtnV.SetRect(PBox->Canvas, (PBox->Width/2) + 2, 0, PBox->Width, PBox->Height);
	PBox->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TACalResDlg::SetupPtn(int Row)
{
	CurRow = Row;
	Row--;
	MaxD = mval[Row].CG;
	int i;
	float	d;
	MaxV = MaxH = -MAXDOUBLE;
	for( i = 0; i < 180; i++ ){
		d = float(mval[Row].PtnH[i])/256.0;
		PtnH[i] = d;
		if( MaxH < d ) MaxH = d;
	}
	for( i = 0; i < 91; i++ ){
		d = float(mval[Row].PtnV[i])/256.0;
		PtnV[i] = d;
		if( MaxV < d ) MaxV = d;
	}
	MaxG = MaxV;
	if( MaxG < MaxH ) MaxG = MaxH;
	PBox->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TACalResDlg::GridSelectCell(TObject *Sender, int Col, int Row,
	bool &CanSelect)
{
	if( Row ){
		if( CurRow != Row){
			SetupPtn(Row);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TACalResDlg::OKBtnClick(TObject *Sender)
{
	if( Grid->Row ){
		if( YesNoMB("アンテナ定義がこの時の状態に更新されます. 実行しますか?" ) == IDYES ){
			ModalResult = IDOK;
		}
	}
}
//---------------------------------------------------------------------------
// 現在のインデックスを得る
int __fastcall TACalResDlg::GetCurIndex(void)
{
	double dnow[AVMAX];
	int i, j;
	for( i = 0; i < pACal->amax; i++ ){
		dnow[i] = RoundUpStr(pACal->GetNowValue(i));
	}
	MVAL *mp = mval;
	for( i = 0; i < mmax; i++, mp++ ){
		for( j = 0; j < pACal->amax; j++ ){
			if( dnow[j] != RoundUpStr(mp->AD[j]) ) break;
		}
		if( j >= pACal->amax ) return i;
	}
#if 0
	FILE *fp;
	fp = fopen("e:\\Test.Txt", "wt");
	for( i = 0; i < pACal->amax; i++ ){
		fprintf(fp, "%10f ", dnow[i]);
	}
	fprintf(fp, "\n");
	mp = mval;
	for( i = 0; i < mmax; i++, mp++ ){
		for( j = 0; j < pACal->amax; j++ ){
			fprintf(fp, "%10f ", float(mp->AD[j]));
		}
		fprintf( fp, "\n");
	}
	fclose(fp);
	WarningMB("最適化シートと現在のアンテナ定義が一致しません.\r\n\r\n更新ボタンを押すと最適化シートの内容が優先されます.");
#endif
	return 0;
}
//---------------------------------------------------------------------------
// インデックス位置にスクロールする
void __fastcall TACalResDlg::ScrollIndex(int n)
{
	int bt = Grid->TopRow - 1;
	int be = bt + (Grid->GridHeight/Grid->DefaultRowHeight) - 2;

	if( (bt < n)||(be > n) ){
		bt = n - (be - bt)/2;
		if( bt < 0 ) bt = 0;
		Grid->TopRow = bt + 1;
	}
	Grid->Row = n + 1;
}

//---------------------------------------------------------------------------
