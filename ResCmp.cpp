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

#include "ResCmp.h"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
//TResCmpDlg *ResCmpDlg;
//---------------------------------------------------------------------
__fastcall TResCmpDlg::TResCmpDlg(TComponent* AOwner)
	: TForm(AOwner)
{
	EntryAlignControl();
	Cnt = 0;
	MaxG = MaxH = MaxV = 0.0;
	pColorTable = NULL;

	DrawPtnH.SetRect(PBox->Canvas, 0, 0, PBox->Width / 2 - 2, PBox->Height);
	DrawPtnV.SetRect(PBox->Canvas, (PBox->Width/2) + 2, 0, PBox->Width, PBox->Height);
	DelBtnClick(NULL);
}
//---------------------------------------------------------------------------
int __fastcall TResCmpDlg::Execute(TColor *pct)
{
	pColorTable = pct;
	if( Res[0].CopyLastRes(&res) ){
		Fname[0] = "現状";
		Cnt = 1;
		CalcMaxDB();
		Grid->RowCount = Cnt + 1;
		UpdateUIAll();
	}
	if( ShowModal() == IDOK ){
		return TRUE;
	}
	else {
		return FALSE;
	}
}
//---------------------------------------------------------------------
void __fastcall TResCmpDlg::PBoxPaint(TObject *Sender)
{
	DrawPtnH.DrawAngle();
	DrawPtnV.DrawAngle();
	if( !Cnt ) return;
	DrawPtnH.SetMaxDB(MaxG);
	DrawPtnV.SetMaxDB(MaxG);
	for( int i = 0; i < Cnt; i++ ){
		if( WaveSel->ItemIndex == 3 ){
			DrawPtnH.DrawPtn(Res[i].PtnH[0], Res[i].PtnF, pColorTable[i]);
			DrawPtnV.DrawPtn(Res[i].PtnV[0], Res[i].PtnF, pColorTable[i]);
			DrawPtnH.DrawPtn(Res[i].PtnH[1], Res[i].PtnF, pColorTable[i]);
			DrawPtnV.DrawPtn(Res[i].PtnV[1], Res[i].PtnF, pColorTable[i]);
		}
		else {
			DrawPtnH.DrawPtn(Res[i].PtnH[WaveSel->ItemIndex], Res[i].PtnF, pColorTable[i]);
			DrawPtnV.DrawPtn(Res[i].PtnV[WaveSel->ItemIndex], Res[i].PtnF, pColorTable[i]);
		}
	}
	int	X, Y;
	DrawPtnV.GetBottomPosition(X, Y);
	char bf[64];
	sprintf( bf, "%s(dBi) = 0dB", StrDbl2(MaxG));
	int FH = PBox->Canvas->TextHeight(bf);
	int Sop = ::SetBkMode(PBox->Canvas->Handle, TRANSPARENT);
	Y += FH/2;
	PBox->Canvas->TextOut(X, Y, bf);
	::SetBkMode(PBox->Canvas->Handle, Sop);
}
//---------------------------------------------------------------------------
void __fastcall TResCmpDlg::GridDrawCell(TObject *Sender, int Col, int Row,
	TRect &Rect, TGridDrawState State)
{
	LPCSTR	_ty[]={"自由","完全GND","ﾘｱﾙGND"};
	char	bf[256];

	if( (Row >= 1) && (Col >= 1) && (ActiveControl != Grid) ){
		Grid->Canvas->Brush->Color = clWindow;
		Grid->Canvas->Font->Color = clBlack;
	}
	Grid->Canvas->FillRect(Rect);
	int X = Rect.Left + 4;
	int Y = Rect.Top + 2;

	if( Row ){
		if( !Cnt ) return;
		Row--;
		bf[0] = 0;
		RESL	*rp = Res[Row].GetResP();
		double	g;
		switch(Col){
			case 0:
				sprintf( bf, "%d", Row + 1 );
				break;
			case 1:
				strcpy(bf, StrDbl3(rp->FQ));
				break;
			case 2:
				strcpy(bf, StrDbl3(rp->R));
				break;
			case 3:
				strcpy(bf, StrDbl3(rp->JX));
				break;
			case 4:
				if( (rp->RO != env.RO)||(rp->JXO != env.JXO) ){
					strcpy(bf, StrDbl2(CalcSWR(rp->R, rp->JX, env.RO, env.JXO)));
				}
				else {
					strcpy(bf, StrDbl2(rp->SWR));
				}
				break;
			case 5:	// 相対利得
				if( rp->GAIN != NULLV ){
					if( rp->TYPE ){
						strcpy(bf, "---");
					}
					else {
						g = rp->GAIN - 2.15;
						if( g < -999 ) g = -999;
						strcpy(bf, StrDbl2(g));
					}
				}
				break;
			case 6:	// 絶対利得
				g = rp->GAIN;
				if( g < -999 ) g = -999;
				strcpy(bf, StrDbl2(g));
				break;
			case 7:
				strcpy(bf, StrDbl2(rp->FB));
				break;
			case 8:
				strcpy(bf, StrDbl1(rp->ELV));
				break;
			case 9:		// 条件
				strcpy(bf, _ty[rp->TYPE]);
				break;
			case 10:	// 地上高
				if( rp->TYPE ){
					strcpy(bf, StrDbl(rp->HEIGHT));
				}
				break;
			case 11:	// 偏ぱ
				strcpy(bf, rp->MAXVP > rp->MAXHP ? "垂直":"水平");
				break;
			case 12:	// 名前
				strcpy(bf, Fname[Row].c_str());
				SetFileName(bf);
				break;
			case 13:	// 名前
				strcpy(bf, Res[Row].Name);
				break;
		}
		if( pColorTable != NULL ){
			Grid->Canvas->Font->Color = pColorTable[Row];
		}
		Grid->Canvas->TextOut(X, Y, bf);
	}
	else {		// タイトル
		LPCSTR	_tt[]={
			"No.","Freq","R","jX","SWR","Gh","Ga","F/B","Elev.","条件","地上高","偏波","ﾌｧｲﾙ","名前"
		};
		Grid->Canvas->TextOut(X, Y, _tt[Col]);
	}
}
//---------------------------------------------------------------------------
// 最大ゲインの調整
void __fastcall TResCmpDlg::CalcMaxDB(void)
{
	MaxH = Res[0].MaxH;
	MaxV = Res[0].MaxV;
	for( int i = 1; i < Cnt; i++ ){
		if( MaxH < Res[i].MaxH ) MaxH = Res[i].MaxH;
		if( MaxV < Res[i].MaxV ) MaxV = Res[i].MaxV;
	}
	MaxG = MaxV;
	if( MaxG < MaxH ) MaxG = MaxH;
}
//---------------------------------------------------------------------------
// ファイルのロード
int __fastcall TResCmpDlg::LoadResFile(LPCSTR pName)
{
	FILE		*fp;
	char		bf[32];

	if( (fp = fopen(pName, "rb"))!=NULL ){
		fread(bf, 1, 16, fp);
		if( strcmp(bf, RESSTR) ){
			ErrorMB(ILLFMT, pName);
			fclose(fp);
			return FALSE;// フォーマット異常
		}
		ANTDEF *ap = new ANTDEF;
		ENVDEF *ep = new ENVDEF;
		memcpy(ep, &env, sizeof(ENVDEF));
		AnsiString rem;
		ReadAntFromFp(ap, rem, fp);
		delete ap;
		memcpy(&env, ep, sizeof(ENVDEF));
		delete ep;
		Res[Cnt].LoadCurData(fp);
		Cnt++;
		fclose(fp);
		return TRUE;
	}
	else {
		ErrorMB("'%s'が見つかりません.", pName);
	}
	return FALSE;
}

//---------------------------------------------------------------------------
// ファイルのロードメニュー
void __fastcall TResCmpDlg::LoadBtnClick(TObject *Sender)
{
	if( Cnt >= RESMAX ) return;
	OpenDialog->Title = "計算結果を選択";
	OpenDialog->Filter = "MMANA 計算結果(*.mab)|*.mab|";
	OpenDialog->FileName = "";
	OpenDialog->DefaultExt = "mab";
	OpenDialog->InitialDir = ResDir;
	if( OpenDialog->Execute() == TRUE ){
		for( int i = 0; i < Cnt; i++ ){
			if( !strcmp(Fname[i].c_str(), AnsiString(OpenDialog->FileName).c_str()) ){
				return;
			}
		}
		Fname[Cnt] = OpenDialog->FileName;
		if( LoadResFile(AnsiString(OpenDialog->FileName).c_str()) == TRUE ){
			CalcMaxDB();
			Grid->RowCount = Cnt + 1;
			UpdateUIAll();
			SetDirName(ResDir, AnsiString(OpenDialog->FileName).c_str());
			Grid->Invalidate();
			PBox->Invalidate();
		}
	}
}
//---------------------------------------------------------------------------
// 更新アイテムの無効化
void __fastcall TResCmpDlg::UpdateUIAll(void)
{
	if( Cnt ){
		DelBtn->Enabled = TRUE;
		if( Cnt >= RESMAX ){
			LoadBtn->Enabled = FALSE;
		}
		else {
			LoadBtn->Enabled = TRUE;
		}
		if( Grid->Row ){
			ColBtn->Enabled = TRUE;
		}
		else {
			ColBtn->Enabled = FALSE;
		}
	}
	else {
		DelBtn->Enabled = FALSE;
		LoadBtn->Enabled = TRUE;
		ColBtn->Enabled = FALSE;
	}
}

//---------------------------------------------------------------------------
void __fastcall TResCmpDlg::DelBtnClick(TObject *Sender)
{
	Cnt = 0;
	Grid->RowCount = Cnt + 2;
	UpdateUIAll();
	Grid->Invalidate();
	PBox->Invalidate();
}
//---------------------------------------------------------------------------
// 色変更ボタン
void __fastcall TResCmpDlg::ColBtnClick(TObject *Sender)
{
	if( Grid->Row ){
		if( ColorDialog->Execute() == TRUE ){
			pColorTable[Grid->Row-1] = ColorDialog->Color;
			UpdateUIAll();
			Grid->Invalidate();
			PBox->Invalidate();
		}
	}
}
//---------------------------------------------------------------------------
// サイズ変更コントロールの登録
void __fastcall TResCmpDlg::EntryAlignControl(void)
{
	RECT	rc;
	rc.left = 0;
	rc.top = 0;
	rc.right = 799;
	rc.bottom = 599;

	AlignList.EntryControl(OKBtn, &rc, OKBtn->Font);
	AlignList.EntryControl(PBox, &rc, PBox->Font);
	AlignList.EntryControl(Grid, &rc, Grid->Font);
	AlignList.EntryControl(LoadBtn, &rc, LoadBtn->Font);
	AlignList.EntryControl(DelBtn, &rc, DelBtn->Font);
	AlignList.EntryControl(ColBtn, &rc, ColBtn->Font);
	AlignList.EntryControl(WaveSel, &rc, WaveSel->Font);

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
void __fastcall TResCmpDlg::FormResize(TObject *Sender)
{
	AlignList.NewAlign(this);
	AlignGrid.NewAlign(Grid);

	DrawPtnH.SetRect(PBox->Canvas, 0, 0, PBox->Width / 2 - 2, PBox->Height);
	DrawPtnV.SetRect(PBox->Canvas, (PBox->Width/2) + 2, 0, PBox->Width, PBox->Height);
	PBox->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TResCmpDlg::WaveSelClick(TObject *Sender)
{
	PBox->Invalidate();
}
//---------------------------------------------------------------------------
