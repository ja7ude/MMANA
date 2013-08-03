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

#include "WireCad.h"
#include "WireEdit.h"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
//TWireCadDlg *WireCadDlg;
const int GridTable[]={
	1, 2, 5, 10, 20, 25, 50, 100, 200, 250, 500, 1000, 2000, 2500, 5000, 10000, 20000, 50000, 100000, 200000, 500000, 1000000, 0
};
const LPCSTR FilterTable[]={"全部","同じ面","グループ",NULL};
static CViewPos	vpos[4];
//---------------------------------------------------------------------
__fastcall TWireCadDlg::TWireCadDlg(TComponent* AOwner)
	: TForm(AOwner)
{
	EnbSetup = 0;
	HSBAll->Align = alBottom;
	VSBAll->Align = alRight;
	PBoxALL->Align = alClient;
	HSBXY->Align = alBottom;
	VSBXY->Align = alRight;
	PBoxXY->Align = alClient;
	HSBXZ->Align = alBottom;
	VSBXZ->Align = alRight;
	PBoxXZ->Align = alClient;
	HSBYZ->Align = alBottom;
	VSBYZ->Align = alRight;
	PBoxYZ->Align = alClient;
	HSBAll->SetParams(50, 0, 100);
	VSBAll->SetParams(50, 0, 100);
	HSBXY->SetParams(50, 0, 100);
	VSBXY->SetParams(50, 0, 100);
	HSBXZ->SetParams(50, 0, 100);
	VSBXZ->SetParams(50, 0, 100);
	HSBYZ->SetParams(50, 0, 100);
	VSBYZ->SetParams(50, 0, 100);
	HSBXc = VSBYc = 0.0;
	ScrollF = 0;
	SBSC = 0.5;
	Filter = 0;
	TimerCmd = 0;
	SelWire = -1;
	Grid = 0.5;
	GridX = GridY = 0;
	Mode = 0;		// 0:選択, 1:直線, 2:ループ
	AntDeg = double(TBDEG->Position);
	AntDeg *= (PAI / 180.0);
	AntZDeg = double(TBZDEG->Position);
	AntZDeg *= (PAI / 180.0);
	AntXc = AntYc = 0.0;
	AntSC = double(TBSC->Position) / 20.0;
	AntSC = AntSC * AntSC * AntSC * AntSC;
	ScrXc = int(PBoxALL->Width/2 + (AntXc * AntSC));
	ScrYc = int(PBoxALL->Height/2 + (AntYc * AntSC));
	AntMX = AntMY = 0;
	DownF = 0;
	MoveF = 0;
	EntryAlignControl();
	EnbSetup = 0;
	SetupSB();
	int i;
	for( i = 0; GridTable[i]; i++ ){
		GridWidth->Items->Add(StrDbl(double(GridTable[i])/100.0));
	}
	for( i = 0; FilterTable[i]!=NULL; i++ ){
		SelFilter->Items->Add(FilterTable[i]);
	}
	SelFilter->Text = FilterTable[0];
	TBZDEG->Enabled = exeenv.Ant3D ? TRUE : FALSE;
	K9->Checked = exeenv.Ant3D;
}
//---------------------------------------------------------------------------
// ページの変更
void __fastcall TWireCadDlg::UpdateView(void)
{
	if( Page->ActivePage == TabAll ){
		TBDEG->Visible = TRUE;
		TBZDEG->Visible = TRUE;
		Label5->Visible = TRUE;
		LCursor->Visible = FALSE;
		K9->Enabled = TRUE;
	}
	else {
		TBDEG->Visible = FALSE;
		TBZDEG->Visible = FALSE;
		Label5->Visible = FALSE;
		LCursor->Visible = TRUE;
		K9->Enabled = FALSE;
	}
}
//---------------------------------------------------------------------------
// 同一ワイヤを探す
int __fastcall TWireCadDlg::IsWire(double x1, double y1, double z1, double x2, double y2, double z2)
{
	int i;
	for( i = 0; i < WMAX; i++ ){
		if( !wdef[i].PNo ) continue;
		if( (x1==wdef[i].X1)&&(y1==wdef[i].Y1)&&(z1==wdef[i].Z1)&&(x2==wdef[i].X2)&&(y2==wdef[i].Y2)&&(z2==wdef[i].Z2) ){
			return 1;
		}
		else if( (x1==wdef[i].X2)&&(y1==wdef[i].Y2)&&(z1==wdef[i].Z2)&&(x2==wdef[i].X1)&&(y2==wdef[i].Y1)&&(z2==wdef[i].Z1) ){
			return 1;
		}
	}
	return 0;
}
int __fastcall TWireCadDlg::IsWire(WDEF &wd)
{
	return IsWire(wd.X1, wd.Y1, wd.Z1, wd.X2, wd.Y2, wd.Z2);
}
//---------------------------------------------------------------------------
// 接続しているか調べる
int __fastcall TWireCadDlg::IsCon(int wno, double x, double y, double z)
{
	if( !wdef[wno].PNo ) return 0;
	if( (x == wdef[wno].X1)&&(y == wdef[wno].Y1)&&(z == wdef[wno].Z1) ) return 1;
	if( (x == wdef[wno].X2)&&(y == wdef[wno].Y2)&&(z == wdef[wno].Z2) ) return 1;
	return 0;
}
//---------------------------------------------------------------------------
// 接続しているワイヤを探す
int __fastcall TWireCadDlg::IsConnect(int wno, double x, double y, double z)
{
	int i;

	for( i = 0; i < wmax; i++ ){
		if( !wdef[i].PNo ) continue;
		if( i != wno ){
			if( IsCon(i, x, y, z) ) return 1;
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
// 接続しているワイヤを探す
void __fastcall TWireCadDlg::Connect(int w, double X, double Y, double Z)
{
	for( int i = 0; i < wmax; i++ ){
		if( !wdef[i].PNo ) continue;
		if( glist[i]==0xff ){
			if( (wdef[i].X1 == X) && (wdef[i].Y1 == Y) && (wdef[i].Z1 == Z) ){
				glist[i] = BYTE(w);
				Connect(w, wdef[i].X2, wdef[i].Y2, wdef[i].Z2);
			}
			else if( (wdef[i].X2 == X) && (wdef[i].Y2 == Y) && (wdef[i].Z2 == Z) ){
				glist[i] = BYTE(w);
				Connect(w, wdef[i].X1, wdef[i].Y1, wdef[i].Z1);
			}
		}
	}
}
//---------------------------------------------------------------------------
// ワイヤグループを登録する
void __fastcall TWireCadDlg::EntryGroup(void)
{
	memset(glist, 0xff, sizeof(glist));
	if( SelWire == -1 ) return;

	if( Filter == 1 ){
		int i;
		for( i = 0; i < wmax; i++ ){
			if( !wdef[i].PNo ) continue;
			if( Page->ActivePage == TabXY ){
				if( (wdef[i].Z1 == wdef[SelWire].Z1) && (wdef[i].Z2 == wdef[SelWire].Z2) ) glist[i] = 0;
			}
			else if( Page->ActivePage == TabXZ ){
				if( (wdef[i].Y1 == wdef[SelWire].Y1) && (wdef[i].Y2 == wdef[SelWire].Y2) ) glist[i] = 0;
			}
			else if( Page->ActivePage == TabYZ ){
				if( (wdef[i].X1 == wdef[SelWire].X1) && (wdef[i].X2 == wdef[SelWire].X2) ) glist[i] = 0;
			}
			else if( wdef[SelWire].X1 == wdef[SelWire].X2 ){
				if( (wdef[i].X1 == wdef[SelWire].X1) && (wdef[i].X2 == wdef[SelWire].X2) ) glist[i] = 0;
			}
			else if( wdef[SelWire].Y1 == wdef[SelWire].Y2 ){
				if( (wdef[i].Y1 == wdef[SelWire].Y1) && (wdef[i].Y2 == wdef[SelWire].Y2) ) glist[i] = 0;
			}
			else if( wdef[SelWire].Z1 == wdef[SelWire].Z2 ){
				if( (wdef[i].Z1 == wdef[SelWire].Z1) && (wdef[i].Z2 == wdef[SelWire].Z2) ) glist[i] = 0;
			}
		}
	}
	else {
		Connect(0, wdef[SelWire].X1, wdef[SelWire].Y1, wdef[SelWire].Z1);
		Connect(0, wdef[SelWire].X2, wdef[SelWire].Y2, wdef[SelWire].Z2);
	}
}
//---------------------------------------------------------------------------
// 接続点のセグメントを調整する
void __fastcall TWireCadDlg::AdjSeg(int wno)
{
	if( (wno < 0) || (wno > wmax) ) return;
	int i, f;

	for( i = f = 0; i < wmax; i++ ){
		if( !wdef[i].PNo ) continue;
		if( i != wno ){
			if( IsCon(i, wdef[wno].X1, wdef[wno].Y1, wdef[wno].Z1) ){
				f = 1;
				wdef[i].SEG = -1;
			}
			if( IsCon(i, wdef[wno].X2, wdef[wno].Y2, wdef[wno].Z2) ){
				f = 1;
				wdef[i].SEG = -1;
			}
		}
	}
	wdef[wno].SEG = f ? -1 : 0;
	if( wno == SelWire ) DispSelWire();
	return;
}
//---------------------------------------------------------------------------
// ワイヤグループに属するか調べる
int __fastcall TWireCadDlg::IsGroup(int wno)
{
	return (glist[wno] == 0xff) ? 0 : 1;
}
//---------------------------------------------------------------------------
// ワイヤフィルタ
int __fastcall TWireCadDlg::IsFilter(int wno)
{
	if( !wdef[wno].PNo ) return 0;
	if( Filter ){
		return IsGroup(wno);
	}
	else {
		return 1;
	}
}
//---------------------------------------------------------------------------
TPaintBox* __fastcall TWireCadDlg::GetPB(void)
{
	if( Page->ActivePage == TabAll ){
		return PBoxALL;
	}
	else if( Page->ActivePage == TabXY ){
		return PBoxXY;
	}
	else if( Page->ActivePage == TabXZ ){
		return PBoxXZ;
	}
	else {
		return PBoxYZ;
	}
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::DispCursor(void)
{
	StrText	st(512);

	double x = double(AntMX - ScrXc)/AntSC;
	double y = double(ScrYc - AntMY)/AntSC;
	if( Page->ActivePage == TabXY ){
		st.Printf("X : %s\n", StrDbl(x));
		st.Printf("Y : %s", StrDbl(y));
	}
	else if( Page->ActivePage == TabXZ ){
		st.Printf("X : %s\n", StrDbl(x));
		st.Printf("Z : %s", StrDbl(y));
	}
	else if( Page->ActivePage == TabYZ ){
		st.Printf("Y : %s\n", StrDbl(x));
		st.Printf("Z : %s", StrDbl(y));
	}
	else {
		st.Printf("");
	}
	LCursor->Caption = st.GetText();
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::DispSelWire(void)
{
	if( SelWire != -1 ){
		memcpy(&wdisp, &wdef[SelWire], sizeof(wdisp));
	}
	DispWire();
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::DispWire(void)
{
	StrText	st(512);
	if( SelWire != -1 ){
		st.Printf("Wire No.%d\n", (Mode && DownF) ? wmax + 1 : SelWire + 1);
		st.Printf("X1: %s\n", StrDbl(wdisp.X1));
		st.Printf("Y1: %s\n", StrDbl(wdisp.Y1));
		st.Printf("Z1: %s\n", StrDbl(wdisp.Z1));
		st.Printf("X2: %s\n", StrDbl(wdisp.X2));
		st.Printf("Y2: %s\n", StrDbl(wdisp.Y2));
		st.Printf("Z2: %s\n", StrDbl(wdisp.Z2));
		st.Printf("R : %s\n", StrDbl(wdisp.R));
		st.Printf("SEG:%s\n", Seg2Str(wdisp.SEG));
		double XX = wdisp.X2-wdisp.X1;
		double YY = wdisp.Y2-wdisp.Y1;
		double ZZ = wdisp.Z2-wdisp.Z1;
		double D = sqrt(XX*XX+YY*YY+ZZ*ZZ);
		st.Printf("Len:%s", StrDbl(D));
		if( Page->ActivePage == TabXY ){
			st.Printf("\nDeg:%s", StrDbl(GetDeg(wdisp.Y2-wdisp.Y1, wdisp.X2-wdisp.X1)));
		}
		else if( Page->ActivePage == TabXZ ){
			st.Printf("\nDeg:%s", StrDbl(GetDeg(wdisp.Z2-wdisp.Z1, wdisp.X2-wdisp.X1)));
		}
		else if( Page->ActivePage == TabYZ ){
			st.Printf("\nDeg:%s", StrDbl(GetDeg(wdisp.Z2-wdisp.Z1, wdisp.Y2-wdisp.Y1)));
		}
	}
	else {
		st.Printf("");
	}
	LWire->Caption = st.GetText();
}
//---------------------------------------------------------------------------
double __fastcall TWireCadDlg::GetXYZ(int sw)
{
	double	d, ad;
	int		i, f;

	ad = 0.0;
	for( f = i = 0; i < wmax; i++ ){
		if( !IsFilter(i) ) continue;
		switch(sw){
			case 0:
				d = wdef[i].X1;
				if( d != wdef[i].X2 ) return 0.0;
				break;
			case 1:
				d = wdef[i].Y1;
				if( d != wdef[i].Y2 ) return 0.0;
				break;
			case 2:
				d = wdef[i].Z1;
				if( d != wdef[i].Z2 ) return 0.0;
				break;
		}
		if( f ){
			if( d != ad ) return 0.0;
		}
		else {
			ad = d;
			f = 1;
		}
	}
	return ad;
}
//---------------------------------------------------------------------------
int __fastcall TWireCadDlg::CheckFree(int N)
{
	int	i, n;

	n = WMAX - wmax;
	if( n >= N ) return TRUE;
	for( i = 0; i < wmax; i++ ){
		if( !wdef[i].PNo ) n++;
	}
	return ( n >= N ) ? TRUE : FALSE;
}
//---------------------------------------------------------------------------
int __fastcall TWireCadDlg::GetNewWire(void)
{
	int i;

	for( i = 0; i < wmax; i++ ){
		if( !wdef[i].PNo ) return i;
	}
	wmax++;
	return i;
}
//---------------------------------------------------------------------------
int __fastcall TWireCadDlg::Execute(ANTDEF *p, int &Sel)
{
	int i;

	UpdateView();
	pAnt = p;
	wmax = p->wmax;
	if( wmax ) memcpy(&wdef, p->wdef, sizeof(wdef));
	for( i = 0; i < wmax; i++ ){
		wdef[i].PNo = 1;
	}
	SBSel->Down = TRUE;
	SBViewClick(NULL);	// 全景表示
	AdjGrid(1);
	RestView(3);
	if( (Sel >= 0)&&(Sel < wmax) ){
		SelWire = Sel;
		DispSelWire();
	}
	EnbSetup = 1;
	int r = ShowModal();
	if( SelWire != -1 ) Sel = SelWire;
	MemoryView(3);
	if( r == IDOK ){
		pAnt->wmax = 0;
		for( i = 0; i < wmax; i++ ){
			if( wdef[i].PNo ){
				if( SelWire == i ) Sel = pAnt->wmax;
				memcpy(&pAnt->wdef[pAnt->wmax], &wdef[i], sizeof(WDEF));
				pAnt->wmax++;
			}
		}
		return TRUE;
	}
	else {
		return FALSE;
	}
}
//---------------------------------------------------------------------------
// サイズ変更コントロールの登録
void __fastcall TWireCadDlg::EntryAlignControl(void)
{
	RECT	rc;
	rc.left = 0;
	rc.top = 0;
	rc.right = 799;
	rc.bottom = 599;

	AlignList.EntryControl(OKBtn, &rc, OKBtn->Font);
	AlignList.EntryControl(CancelBtn, &rc, CancelBtn->Font);
	AlignList.EntryControl(Page, &rc, Page->Font);
	AlignList.EntryControl(TBSC, &rc, NULL);
	AlignList.EntryControl(TBDEG, &rc, NULL);
	AlignList.EntryControl(TBZDEG, &rc, NULL);
	AlignList.EntryControl(Label1, &rc, Label1->Font);
	AlignList.EntryControl(Label5, &rc, Label5->Font);
	AlignList.EntryControl(SBSel, &rc, NULL);
	AlignList.EntryControl(SBLine, &rc, NULL);
	AlignList.EntryControl(SBLoop, &rc, NULL);
	AlignList.EntryControl(SBView, &rc, NULL);
	AlignList.EntryControl(SBOrg, &rc, NULL);
	AlignList.EntryControl(GroupBox1, &rc, GroupBox1->Font);
	AlignList.EntryControl(ChkGrid, &rc, ChkGrid->Font);
	AlignList.EntryControl(ChkAutoGrid, &rc, ChkAutoGrid->Font);
	AlignList.EntryControl(GridWidth, &rc, GridWidth->Font);
	AlignList.EntryControl(Label2, &rc, Label2->Font);
	AlignList.EntryControl(Label3, &rc, Label3->Font);
	AlignList.EntryControl(LCursor, &rc, LCursor->Font);
	AlignList.EntryControl(LWire, &rc, LWire->Font);
	AlignList.EntryControl(Label4, &rc, Label4->Font);
	AlignList.EntryControl(SelFilter, &rc, SelFilter->Font);

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
//---------------------------------------------------------------------
void __fastcall TWireCadDlg::FormResize(TObject *Sender)
{
	AlignList.NewAlign(this);
	TPaintBox *tp = GetPB();
	AntSC = double(TBSC->Position) / 20.0;
	AntSC = AntSC * AntSC * AntSC * AntSC;
	ScrXc = int(tp->Width/2 + (AntXc * AntSC));
	ScrYc = int(tp->Height/2 + (AntYc * AntSC));
	Page->Repaint();
	SetupSB();
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::DrawDot(TPaintBox *tp, int X, int Y, int SW)
{
	int	Width = tp->Canvas->Pen->Width;
//	TColor Col = tp->Canvas->Pen->Color;
//	tp->Canvas->Pen->Color = clGreen;
	tp->Canvas->Pen->Width = 1;
	if( SW ){		// 接続点
		tp->Canvas->MoveTo(X-3, Y-3); tp->Canvas->LineTo(X+3, Y+3);
		tp->Canvas->MoveTo(X+3, Y-3); tp->Canvas->LineTo(X-3, Y+3);
	}
	else {			// 独立点
		tp->Canvas->MoveTo(X-3, Y-3);
		tp->Canvas->LineTo(X+3, Y-3); tp->Canvas->LineTo(X+3, Y+3);
		tp->Canvas->LineTo(X-3, Y+3); tp->Canvas->LineTo(X-3, Y-3);
	}
//	tp->Canvas->Pen->Color = Col;
	tp->Canvas->Pen->Width = Width;
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::DrawNot(TPaintBox *tp, int x1, int y1, int x2, int y2)
{
	if( Clip.Clip(x1, y1, x2, y2) == TRUE ){
		tp->Canvas->Pen->Width = 1;
		tp->Canvas->MoveTo(x1, y1);
		tp->Canvas->Pen->Style = psDot;
		int Rop = ::SetROP2(tp->Canvas->Handle, R2_NOT);
		tp->Canvas->LineTo(x2, y2);
		::SetROP2(tp->Canvas->Handle, Rop);
		tp->Canvas->Pen->Style = psSolid;
	}
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::DrawNot(TPaintBox *tp, double x1, double y1, double x2, double y2)
{
	int X1, Y1, X2, Y2;

	X1 = int((x1 * AntSC)) + ScrXc;
	Y1 = ScrYc - int((y1 * AntSC));
	X2 = int((x2 * AntSC)) + ScrXc;
	Y2 = ScrYc - int((y2 * AntSC));
	if( Clip.Clip(X1, Y1, X2, Y2) == TRUE ){
		tp->Canvas->Pen->Width = 1;
		tp->Canvas->Pen->Style = psDot;
		int Rop = ::SetROP2(tp->Canvas->Handle, R2_NOT);
		tp->Canvas->MoveTo(X1, Y1);
		tp->Canvas->LineTo(X2, Y2);
		::SetROP2(tp->Canvas->Handle, Rop);
		tp->Canvas->Pen->Style = psSolid;
	}
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::DrawNotLoop(TPaintBox *tp, double x1, double y1, double x2, double y2)
{
	int X, Y;

	tp->Canvas->Pen->Width = 1;
	X = int((x1 * AntSC)) + ScrXc;
	Y = ScrYc - int((y1 * AntSC));
	ClipXY(X, Y);
	tp->Canvas->MoveTo(X, Y);

	X = int((x2 * AntSC)) + ScrXc;
	Y = ScrYc - int((y1 * AntSC));
	ClipXY(X, Y);
	tp->Canvas->Pen->Style = psDot;
	int Rop = ::SetROP2(tp->Canvas->Handle, R2_NOT);
	tp->Canvas->LineTo(X, Y);

	X = int((x2 * AntSC)) + ScrXc;
	Y = ScrYc - int((y2 * AntSC));
	ClipXY(X, Y);
	tp->Canvas->Pen->Style = psDot;
	::SetROP2(tp->Canvas->Handle, R2_NOT);
	tp->Canvas->LineTo(X, Y);

	X = int((x1 * AntSC)) + ScrXc;
	Y = ScrYc - int((y2 * AntSC));
	ClipXY(X, Y);
	tp->Canvas->Pen->Style = psDot;
	::SetROP2(tp->Canvas->Handle, R2_NOT);
	tp->Canvas->LineTo(X, Y);

	X = int((x1 * AntSC)) + ScrXc;
	Y = ScrYc - int((y1 * AntSC));
	ClipXY(X, Y);
	tp->Canvas->Pen->Style = psDot;
	::SetROP2(tp->Canvas->Handle, R2_NOT);
	tp->Canvas->LineTo(X, Y);

	tp->Canvas->Pen->Style = psSolid;
	::SetROP2(tp->Canvas->Handle, Rop);
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::DrawGage(TPaintBox *tp)
{
	double x = double(tp->Width)/(8.0*AntSC);
	double xw;
	int i;
	for( i = 0; GridTable[i]; i++ ){
		xw = GridTable[i]/1000.0;
		if( x < xw ) break;
	}
	xw = RoundUpStr(xw);
	int Xw = xw*AntSC;

	LPCSTR p = "0";
	int Fw1 = tp->Canvas->TextWidth(p);
	int Fh = tp->Canvas->TextHeight(p);
	p = StrDbl(xw);
	int Fw2 = tp->Canvas->TextWidth(p);

	TRect	rc;
	rc.Left = 0;
	rc.Top = 0;
	rc.Right = Xw + 2 + (Fw1+Fw2)/2;
	rc.Bottom = Fh + 10;
	tp->Canvas->Brush->Color = clWhite;
	tp->Canvas->FillRect(rc);
	tp->Canvas->Pen->Color = clBlack;
	int X = rc.Left + 1 + Fw1/2;
	tp->Canvas->MoveTo(X, Fh+2);
	tp->Canvas->LineTo(X, Fh+7);
	tp->Canvas->LineTo(X+Xw, Fh+7);
	tp->Canvas->LineTo(X+Xw, Fh+2);
	tp->Canvas->MoveTo(X+(Xw/2), Fh+2);
	tp->Canvas->LineTo(X+(Xw/2), Fh+7);

	tp->Canvas->TextOut(rc.Left+1, 1, "0");
	tp->Canvas->TextOut(X+Xw-(Fw2/2), 1, p);
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::DrawGrid(TPaintBox *tp)
{
	TRect	rc;

	rc.Left = 0;
	rc.Top = 0;
	rc.Right = tp->Width;
	rc.Bottom = tp->Height;
	tp->Canvas->Brush->Color = clWhite;
	tp->Canvas->FillRect(rc);

	if( ScrollF ) return;
	if( ChkGrid->Checked != TRUE ){
		DrawGage(tp);
		return;
	}

	int		X, Y, n;
	double	x, y;
	for( y = 0, n = 0; y < 10000; y += Grid, n++ ){
		Y = ScrYc - int(((y + GridY) * AntSC));
		if( Y > tp->Height ) continue;
		tp->Canvas->Pen->Color = (n % 5) ? clLtGray : clGreen;
		tp->Canvas->MoveTo(0, Y);
		tp->Canvas->LineTo(tp->Width, Y);
		if( Y < 0 ) break;
	}
	for( y = -Grid, n = 1; y > -10000; y -= Grid, n++ ){
		Y = ScrYc - int(((y + GridY) * AntSC));
		if( Y < 0 ) continue;
		tp->Canvas->Pen->Color = (n % 5) ? clLtGray : clGreen;
		tp->Canvas->MoveTo(0, Y);
		tp->Canvas->LineTo(tp->Width, Y);
		if( Y > tp->Height ) break;
	}
	for( x = 0, n = 0; x < 10000; x += Grid, n++ ){
		X = int(((x + GridX) * AntSC)) + ScrXc;
		if( X < 0 ) continue;
		tp->Canvas->Pen->Color = (n % 5) ? clLtGray : clGreen;
		tp->Canvas->MoveTo(X, 0);
		tp->Canvas->LineTo(X, tp->Height);
		if( X > tp->Width ) break;
	}
	for( x = -Grid, n = 1; x > -10000; x -= Grid, n++ ){
		X = int(((x + GridX) * AntSC)) + ScrXc;
		if( X > tp->Width ) continue;
		tp->Canvas->Pen->Color = (n % 5) ? clLtGray : clGreen;
		tp->Canvas->MoveTo(X, 0);
		tp->Canvas->LineTo(X, tp->Height);
		if( X < 0 ) break;
	}
	DrawGage(tp);
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::AdjLoop(double &X1, double &Y1, double &X2, double &Y2)
{
	double	d;

	if( X2 < X1 ){
		d = X2;
		X2 = X1;
		X1 = d;
	}
	if( Y2 > Y1 ){
		d = Y2;
		Y2 = Y1;
		Y1 = d;
	}
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::NormalX(double &X)
{
	if( ChkGrid->Checked != TRUE ) return;
	if( Grid <= 0.0 ) return;
	X = RoundUp(X-GridX, 1/Grid) + GridX;
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::NormalY(double &Y)
{
	if( ChkGrid->Checked != TRUE ) return;
	if( Grid <= 0.0 ) return;
	Y = RoundUp(Y-GridY, 1/Grid) + GridY;
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::NormalXY(double &X, double &Y)
{
	if( ChkGrid->Checked != TRUE ) return;
	if( Grid <= 0.0 ) return;
	X = RoundUp(X-GridX, 1/Grid) + GridX;
	Y = RoundUp(Y-GridY, 1/Grid) + GridY;
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::NormalXXYY(double X1, double Y1, double &X2, double &Y2)
{
	if( ChkGrid->Checked != TRUE ) return;
	if( Grid <= 0.0 ) return;
	if( X1 == X2 ){
		Y2 = RoundUp(Y2-GridY, 1/Grid) + GridY;
	}
	else if( Y1 == Y2 ){
		X2 = RoundUp(X2-GridX, 1/Grid) + GridX;
	}
	else {
		X2 = RoundUp(X2-GridX, 1/Grid) + GridX;
		Y2 = RoundUp(Y2-GridY, 1/Grid) + GridY;
	}
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::PBoxALLPaint(TObject *Sender)
{
	TRect	rc;

	rc.Left = 0;
	rc.Top = 0;
	rc.Right = PBoxALL->Width;
	rc.Bottom = PBoxALL->Height;
	Clip.SetClip(-100, -100, rc.Right + 100, rc.Bottom + 100);
	PBoxALL->Canvas->Brush->Color = clWhite;
	PBoxALL->Canvas->FillRect(rc);

	if( !ScrollF ) DrawGage(PBoxALL);

	double x, y;
	int		X,Y, X2, Y2;

	PBoxALL->Canvas->Pen->Color = clBlue;
	Calc3DXY(x, y, AntDeg, AntZDeg, 0, 0, 0);				// Y
	X = int(x) + ScrXc;
	Y = ScrYc - int(y);
	PBoxALL->Canvas->MoveTo(X, Y);
	Calc3DXY(x, y, AntDeg, AntZDeg, 0, PBoxALL->Width/2-16, 0);
	X = int(x) + ScrXc;
	Y = ScrYc - int(y);
	PBoxALL->Canvas->LineTo(X, Y);
	int Sop = ::SetBkMode(PBoxALL->Canvas->Handle, TRANSPARENT);
	PBoxALL->Canvas->TextOut(X, Y, "Y");

	Calc3DXY(x, y, AntDeg, AntZDeg, 0, 0, 0);				// Z
	X = int(x) + ScrXc;
	Y = ScrYc - int(y);
	PBoxALL->Canvas->MoveTo(X, Y);
	Calc3DXY(x, y, AntDeg, AntZDeg, 0, 0, PBoxALL->Height);
	X = int(x) + ScrXc;
	Y = ScrYc - int(y);
	PBoxALL->Canvas->LineTo(X, Y);
	::SetBkMode(PBoxALL->Canvas->Handle, TRANSPARENT);
	PBoxALL->Canvas->TextOut(X+4, 0, "Z");

	Calc3DXY(x, y, AntDeg, AntZDeg, 0, 0, 0);				// X
	X = int(x) + ScrXc;
	Y = ScrYc - int(y);
	PBoxALL->Canvas->MoveTo(X, Y);
	Calc3DXY(x, y, AntDeg, AntZDeg, PBoxALL->Height/2, 0, 0);
	X = int(x) + ScrXc;
	Y = ScrYc - int(y);
	PBoxALL->Canvas->LineTo(X, Y);
	::SetBkMode(PBoxALL->Canvas->Handle, TRANSPARENT);
	PBoxALL->Canvas->TextOut(X, Y, "X");

	// ワイヤの表示
	PBoxALL->Canvas->Pen->Color = clBlack;
	int i;
	PBoxALL->Canvas->Pen->Width = 2;
	for( i = 0; i < wmax; i++ ){
		if( !IsFilter(i) ) continue;
		PBoxALL->Canvas->Pen->Color = ((i == SelWire) ? clRed : clBlack);
		Calc3DXY(x, y, AntDeg, AntZDeg, wdef[i].X1, wdef[i].Y1, wdef[i].Z1);
		X = int((x * AntSC)) + ScrXc;
		Y = ScrYc - int((y * AntSC));
		Calc3DXY(x, y, AntDeg, AntZDeg, wdef[i].X2, wdef[i].Y2, wdef[i].Z2);
		X2 = int((x * AntSC)) + ScrXc;
		Y2 = ScrYc - int((y * AntSC));
		Clip.Line(PBoxALL->Canvas, X, Y, X2, Y2);
		if( !ScrollF ){
			ClipXY(X, Y);
			ClipXY(X2, Y2);
			DrawDot(PBoxALL, X, Y, IsConnect(i, wdef[i].X1, wdef[i].Y1, wdef[i].Z1));
			DrawDot(PBoxALL, X2, Y2, IsConnect(i, wdef[i].X2, wdef[i].Y2, wdef[i].Z2));
		}
	}
	PBoxALL->Canvas->Pen->Width = 1;
	::SetBkMode(PBoxALL->Canvas->Handle, Sop);
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::TBSCChange(TObject *Sender)
{
	if( !EnbSetup ) return;
	TPaintBox *tp = GetPB();
	AntSC = double(TBSC->Position) / 20.0;
	AntSC = AntSC * AntSC * AntSC * AntSC;
	ScrXc = int(tp->Width/2 + (AntXc * AntSC));
	ScrYc = int(tp->Height/2 + (AntYc * AntSC));
	AdjGrid(1);
	tp->Invalidate();
	SetupSB();
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::TBDEGChange(TObject *Sender)
{
	if( !EnbSetup ) return;
	AntDeg = double(TBDEG->Position);
	AntDeg *= (PAI / 180.0);
	AntZDeg = double(TBZDEG->Position);
	AntZDeg *= (PAI / 180.0);
	PBoxALL->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::PBoxXYPaint(TObject *Sender)
{
	Clip.SetClip(-100, -100, PBoxXY->Width + 100, PBoxXY->Height + 100);
	DrawGrid(PBoxXY);

	int		X,Y, X2, Y2;

	PBoxXY->Canvas->Pen->Color = clBlue;
	PBoxXY->Canvas->MoveTo(ScrXc, ScrYc);				// Y
	PBoxXY->Canvas->LineTo(ScrXc, 0);
	int Sop = ::SetBkMode(PBoxXY->Canvas->Handle, TRANSPARENT);
	PBoxXY->Canvas->TextOut(ScrXc + 4, 4, "Y");

	PBoxXY->Canvas->MoveTo(ScrXc, ScrYc);				// X
	PBoxXY->Canvas->LineTo(PBoxXY->Width, ScrYc);
	::SetBkMode(PBoxXY->Canvas->Handle, TRANSPARENT);
	PBoxXY->Canvas->TextOut(PBoxXY->Width-16, ScrYc+4, "X");

	// ワイヤの表示
	PBoxXY->Canvas->Pen->Color = clBlack;
	int i;
	PBoxXY->Canvas->Pen->Width = 2;
	for( i = 0; i < wmax; i++ ){
		if( !IsFilter(i) ) continue;
		PBoxXY->Canvas->Pen->Color = ((i == SelWire) ? clRed : clBlack);
		X = int((wdef[i].X1 * AntSC)) + ScrXc;
		Y = ScrYc - int((wdef[i].Y1 * AntSC));
		X2 = int((wdef[i].X2 * AntSC)) + ScrXc;
		Y2 = ScrYc - int((wdef[i].Y2 * AntSC));
		Clip.Line(PBoxXY->Canvas, X, Y, X2, Y2);
		if( !ScrollF ){
			ClipXY(X, Y);
			ClipXY(X2, Y2);
			DrawDot(PBoxXY, X, Y, IsConnect(i, wdef[i].X1, wdef[i].Y1, wdef[i].Z1));
			DrawDot(PBoxXY, X2, Y2, IsConnect(i, wdef[i].X2, wdef[i].Y2, wdef[i].Z2));
		}
	}
	PBoxXY->Canvas->Pen->Width = 1;
	::SetBkMode(PBoxXY->Canvas->Handle, Sop);
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::PBoxXZPaint(TObject *Sender)
{
	Clip.SetClip(-100, -100, PBoxXZ->Width + 100, PBoxXZ->Height + 100);
	DrawGrid(PBoxXZ);
	int		X,Y, X2, Y2;

	PBoxXZ->Canvas->Pen->Color = clBlue;
	PBoxXZ->Canvas->MoveTo(ScrXc, ScrYc);				// Z
	PBoxXZ->Canvas->LineTo(ScrXc, 0);
	int Sop = ::SetBkMode(PBoxXZ->Canvas->Handle, TRANSPARENT);
	PBoxXZ->Canvas->TextOut(ScrXc + 4, 4, "Z");

	PBoxXZ->Canvas->MoveTo(ScrXc, ScrYc);				// X
	PBoxXZ->Canvas->LineTo(PBoxXZ->Width, ScrYc);
	::SetBkMode(PBoxXZ->Canvas->Handle, TRANSPARENT);
	PBoxXZ->Canvas->TextOut(PBoxXZ->Width-16, ScrYc+4, "X");

	// ワイヤの表示
	PBoxXZ->Canvas->Pen->Color = clBlack;
	int i;
	PBoxXZ->Canvas->Pen->Width = 2;
	for( i = 0; i < wmax; i++ ){
		if( !IsFilter(i) ) continue;
		PBoxXZ->Canvas->Pen->Color = ((i == SelWire) ? clRed : clBlack);
		X = int((wdef[i].X1 * AntSC)) + ScrXc;
		Y = ScrYc - int((wdef[i].Z1 * AntSC));
		X2 = int((wdef[i].X2 * AntSC)) + ScrXc;
		Y2 = ScrYc - int((wdef[i].Z2 * AntSC));
		Clip.Line(PBoxXZ->Canvas, X, Y, X2, Y2);
		if( !ScrollF ){
			ClipXY(X, Y);
			ClipXY(X2, Y2);
			DrawDot(PBoxXZ, X, Y, IsConnect(i, wdef[i].X1, wdef[i].Y1, wdef[i].Z1));
			DrawDot(PBoxXZ, X2, Y2, IsConnect(i, wdef[i].X2, wdef[i].Y2, wdef[i].Z2));
		}
	}
	PBoxXZ->Canvas->Pen->Width = 1;
	::SetBkMode(PBoxXZ->Canvas->Handle, Sop);
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::PBoxYZPaint(TObject *Sender)
{
	Clip.SetClip(-100, -100, PBoxYZ->Width + 100, PBoxYZ->Height + 100);
	DrawGrid(PBoxYZ);

	int		X,Y, X2, Y2;

	PBoxYZ->Canvas->Pen->Color = clBlue;
	PBoxYZ->Canvas->MoveTo(ScrXc, ScrYc);				// Z
	PBoxYZ->Canvas->LineTo(ScrXc, 0);
	int Sop = ::SetBkMode(PBoxYZ->Canvas->Handle, TRANSPARENT);
	PBoxYZ->Canvas->TextOut(ScrXc + 4, 4, "Z");

	PBoxYZ->Canvas->MoveTo(ScrXc, ScrYc);				// Y
	PBoxYZ->Canvas->LineTo(PBoxYZ->Width, ScrYc);
	::SetBkMode(PBoxYZ->Canvas->Handle, TRANSPARENT);
	PBoxYZ->Canvas->TextOut(PBoxYZ->Width-16, ScrYc+4, "Y");

	// ワイヤの表示
	PBoxYZ->Canvas->Pen->Color = clBlack;
	int i;
	PBoxYZ->Canvas->Pen->Width = 2;
	for( i = 0; i < wmax; i++ ){
		if( !IsFilter(i) ) continue;
		PBoxYZ->Canvas->Pen->Color = ((i == SelWire) ? clRed : clBlack);
		X = int((wdef[i].Y1 * AntSC)) + ScrXc;
		Y = ScrYc - int((wdef[i].Z1 * AntSC));
		X2 = int((wdef[i].Y2 * AntSC)) + ScrXc;
		Y2 = ScrYc - int((wdef[i].Z2 * AntSC));
		Clip.Line(PBoxYZ->Canvas, X, Y, X2, Y2);
		if( !ScrollF ){
			ClipXY(X, Y);
			ClipXY(X2, Y2);
			DrawDot(PBoxYZ, X, Y, IsConnect(i, wdef[i].X1, wdef[i].Y1, wdef[i].Z1));
			DrawDot(PBoxYZ, X2, Y2, IsConnect(i, wdef[i].X2, wdef[i].Y2, wdef[i].Z2));
		}
	}
	PBoxYZ->Canvas->Pen->Width = 1;
	::SetBkMode(PBoxYZ->Canvas->Handle, Sop);
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::SBSelClick(TObject *Sender)
{
	Mode = 0;
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::SBLineClick(TObject *Sender)
{
	Mode = 1;
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::SBLoopClick(TObject *Sender)
{
	Mode = 2;
}
//---------------------------------------------------------------------------
// 全景表示ボタン
void __fastcall TWireCadDlg::SBViewClick(TObject *Sender)
{
	double	XL, XH, YL, YH, D, x, y;
	int		i, pos;

	pos = 0;
	XL = 1e10; XH = -1e10;
	YL = 1e10; YH = -1e10;
	if( Page->ActivePage == TabAll ){
		for( i = 0; i < wmax; i++ ){
			if( !IsFilter(i) ) continue;
			Calc3DXY(x, y, AntDeg, AntZDeg, wdef[i].X1, wdef[i].Y1, wdef[i].Z1);
			if( XL > x ) XL = x;
			if( XH < x ) XH = x;
			if( YL > y ) YL = y;
			if( YH < y ) YH = y;
			Calc3DXY(x, y, AntDeg, AntZDeg, wdef[i].X2, wdef[i].Y2, wdef[i].Z2);
			if( XL > x ) XL = x;
			if( XH < x ) XH = x;
			if( YL > y ) YL = y;
			if( YH < y ) YH = y;
			pos = 1;
		}
	}
	else if( Page->ActivePage == TabXY ){
		for( i = 0; i < wmax; i++ ){
			if( !IsFilter(i) ) continue;
			if( XL > wdef[i].X1 ) XL = wdef[i].X1;
			if( XH < wdef[i].X1 ) XH = wdef[i].X1;
			if( YL > wdef[i].Y1 ) YL = wdef[i].Y1;
			if( YH < wdef[i].Y1 ) YH = wdef[i].Y1;
			if( XL > wdef[i].X2 ) XL = wdef[i].X2;
			if( XH < wdef[i].X2 ) XH = wdef[i].X2;
			if( YL > wdef[i].Y2 ) YL = wdef[i].Y2;
			if( YH < wdef[i].Y2 ) YH = wdef[i].Y2;
			pos = 1;
		}
	}
	else if( Page->ActivePage == TabXZ ){
		for( i = 0; i < wmax; i++ ){
			if( !IsFilter(i) ) continue;
			if( XL > wdef[i].X1 ) XL = wdef[i].X1;
			if( XH < wdef[i].X1 ) XH = wdef[i].X1;
			if( YL > wdef[i].Z1 ) YL = wdef[i].Z1;
			if( YH < wdef[i].Z1 ) YH = wdef[i].Z1;
			if( XL > wdef[i].X2 ) XL = wdef[i].X2;
			if( XH < wdef[i].X2 ) XH = wdef[i].X2;
			if( YL > wdef[i].Z2 ) YL = wdef[i].Z2;
			if( YH < wdef[i].Z2 ) YH = wdef[i].Z2;
			pos = 1;
		}
	}
	else {
		for( i = 0; i < wmax; i++ ){
			if( !IsFilter(i) ) continue;
			if( XL > wdef[i].Y1 ) XL = wdef[i].Y1;
			if( XH < wdef[i].Y1 ) XH = wdef[i].Y1;
			if( YL > wdef[i].Z1 ) YL = wdef[i].Z1;
			if( YH < wdef[i].Z1 ) YH = wdef[i].Z1;
			if( XL > wdef[i].Y2 ) XL = wdef[i].Y2;
			if( XH < wdef[i].Y2 ) XH = wdef[i].Y2;
			if( YL > wdef[i].Z2 ) YL = wdef[i].Z2;
			if( YH < wdef[i].Z2 ) YH = wdef[i].Z2;
			pos = 1;
		}
	}
	TPaintBox *tp = GetPB();
	if( pos ){
		AntXc = -(XL + XH)/2;
		AntYc = (YL + YH)/2;
		D = ABS(XH - XL);
		if( D < ABS(YH - YL) ) D = ABS(YH - YL);
		D *= 1.1;
		if( D > 0.0 ){
			D = (tp->Height)/D;
			pos = int(sqrt(sqrt(D)) * 20.0);
			if( pos > TBSC->Max ){
				pos = TBSC->Max;
			}
		}
		else {
			pos = TBSC->Max;
		}
	}
	else {
		AntXc = AntYc = 0;
		GridX = GridY = 0;
		pos = 50;
	}
	TBSC->Position = pos;
	AntSC = double(TBSC->Position) / 20.0;
	AntSC = AntSC * AntSC * AntSC * AntSC;
	ScrXc = int(tp->Width/2 + (AntXc * AntSC));
	ScrYc = int(tp->Height/2 + (AntYc * AntSC));
	SetupSB();
	AdjGrid(1);
	tp->Invalidate();
}
//---------------------------------------------------------------------------
// ポップアップメニューの有効化
void __fastcall TWireCadDlg::PopupMenuPopup(TObject *Sender)
{
	K13->Enabled = vpos[0].Flag ? TRUE : FALSE;
	K14->Enabled = vpos[1].Flag ? TRUE : FALSE;
	K15->Enabled = vpos[2].Flag ? TRUE : FALSE;
	KU1->Enabled = SaveWire.IsSwap() ? TRUE : FALSE;
	if( SelWire != -1 ){	// アイテム選択状態
		POINT	PP, BP, EP;
		PP.x = AntMX; PP.y = AntMY;
		// ワイヤの検索
		if( Page->ActivePage == TabAll ){
			double x, y;
			Calc3DXY(x, y, AntDeg, AntZDeg, wdef[SelWire].X1, wdef[SelWire].Y1, wdef[SelWire].Z1);
			BP.x = int((x * AntSC)) + ScrXc;
			BP.y = ScrYc - int((y * AntSC));
			Calc3DXY(x, y, AntDeg, AntZDeg, wdef[SelWire].X2, wdef[SelWire].Y2, wdef[SelWire].Z2);
			EP.x = int((x * AntSC)) + ScrXc;
			EP.y = ScrYc - int((y * AntSC));
		}
		else if( Page->ActivePage == TabXY ){
			BP.x = int((wdef[SelWire].X1 * AntSC)) + ScrXc;
			BP.y = ScrYc - int((wdef[SelWire].Y1 * AntSC));
			EP.x = int((wdef[SelWire].X2 * AntSC)) + ScrXc;
			EP.y = ScrYc - int((wdef[SelWire].Y2 * AntSC));
		}
		else if( Page->ActivePage == TabXZ ){
			BP.x = int((wdef[SelWire].X1 * AntSC)) + ScrXc;
			BP.y = ScrYc - int((wdef[SelWire].Z1 * AntSC));
			EP.x = int((wdef[SelWire].X2 * AntSC)) + ScrXc;
			EP.y = ScrYc - int((wdef[SelWire].Z2 * AntSC));
		}
		else if( Page->ActivePage == TabYZ ){
			BP.x = int((wdef[SelWire].Y1 * AntSC)) + ScrXc;
			BP.y = ScrYc - int((wdef[SelWire].Z1 * AntSC));
			EP.x = int((wdef[SelWire].Y2 * AntSC)) + ScrXc;
			EP.y = ScrYc - int((wdef[SelWire].Z2 * AntSC));
		}
		SelCmd = IsLine(PP, BP, EP);
		KX11->Enabled = TRUE;
		K11->Enabled = TRUE;
		K10->Enabled = TRUE;
		K2->Enabled = TRUE;
		K3->Enabled = TRUE;
		KS1->Enabled = TRUE;
		switch(SelCmd){
			case 1:
				if( IsConnect(SelWire, wdef[SelWire].X1, wdef[SelWire].Y1, wdef[SelWire].Z1) ){
					K1->Enabled = FALSE;
					K5->Enabled = TRUE;
				}
				else {
					K1->Enabled = TRUE;
					K5->Enabled = FALSE;
				}
				return;
			case 2:
				if( IsConnect(SelWire, wdef[SelWire].X2, wdef[SelWire].Y2, wdef[SelWire].Z2) ){
					K1->Enabled = FALSE;
					K5->Enabled = TRUE;
				}
				else {
					K1->Enabled = TRUE;
					K5->Enabled = FALSE;
				}
				return;
			case 3:
				K5->Enabled = FALSE;
				K1->Enabled = FALSE;
				return;
			default:
				break;
		}
	}
	KX11->Enabled = FALSE;
	K5->Enabled = FALSE;
	K1->Enabled = FALSE;
	K2->Enabled = FALSE;
	K3->Enabled = FALSE;
	K10->Enabled = FALSE;
	K11->Enabled = FALSE;
	KS1->Enabled = FALSE;
}
//---------------------------------------------------------------------------
// この位置を中心にする
void __fastcall TWireCadDlg::K4Click(TObject *Sender)
{
	TPaintBox *tp = GetPB();
	AntXc += double((tp->Width/2) - AntMX)/AntSC;
	AntYc += double((tp->Height/2) - AntMY)/AntSC;
	ScrXc = int(tp->Width/2 + (AntXc * AntSC));
	ScrYc = int(tp->Height/2 + (AntYc * AntSC));
	tp->Invalidate();
	SetupSB();
}
//---------------------------------------------------------------------------
// 座標原点を中心にする
void __fastcall TWireCadDlg::K8Click(TObject *Sender)
{
	TPaintBox *tp = GetPB();
	AntXc = AntYc = 0.0;
	ScrXc = int(tp->Width/2 + (AntXc * AntSC));
	ScrYc = int(tp->Height/2 + (AntYc * AntSC));
	tp->Invalidate();
	SetupSB();
}
//---------------------------------------------------------------------------
// 近接端点に接続
void __fastcall TWireCadDlg::K1Click(TObject *Sender)
{
	if( SelWire == -1 ) return;

	double	&X = (SelCmd == 1) ? wdef[SelWire].X1 : wdef[SelWire].X2;
	double	&Y = (SelCmd == 1) ? wdef[SelWire].Y1 : wdef[SelWire].Y2;
	double	&Z = (SelCmd == 1) ? wdef[SelWire].Z1 : wdef[SelWire].Z2;
	double	&Xo = (SelCmd == 1) ? wdef[SelWire].X2 : wdef[SelWire].X1;
	double	&Yo = (SelCmd == 1) ? wdef[SelWire].Y2 : wdef[SelWire].Y1;
	double	&Zo = (SelCmd == 1) ? wdef[SelWire].Z2 : wdef[SelWire].Z1;
	int i;
	double	XX, YY, ZZ;
	double D;
	double MD = MAXDOUBLE;
	int mw = -1;
	int mf;
	for( i = 0; i < wmax; i++ ){
		if( !IsFilter(i) ) continue;
		if( i != SelWire ){
			if( ((Xo!=wdef[i].X1)||(Yo!=wdef[i].Y1)||(Zo!=wdef[i].Z1))&& (!IsWire(wdef[i].X1, wdef[i].Y1, wdef[i].Z1, Xo, Yo, Zo)) ){
				XX = wdef[i].X1 - X;
				YY = wdef[i].Y1 - Y;
				ZZ = wdef[i].Z1 - Z;
				D = sqrt(XX*XX+YY*YY+ZZ*ZZ);
				if( D < MD ){
					MD = D;
					mw = i;
					mf = 0;
				}
			}
			if( ((Xo!=wdef[i].X2)||(Yo!=wdef[i].Y2)||(Zo!=wdef[i].Z2))&& (!IsWire(wdef[i].X2, wdef[i].Y2, wdef[i].Z2, Xo, Yo, Zo)) ){
				XX = wdef[i].X2 - X;
				YY = wdef[i].Y2 - Y;
				ZZ = wdef[i].Z2 - Z;
				D = sqrt(XX*XX+YY*YY+ZZ*ZZ);
				if( D < MD ){
					MD = D;
					mw = i;
					mf = 1;
				}
			}
		}
	}
	if( mw == -1 ) return;
	SaveWire.SetData(SelWire, wmax, wdef);
	if( mf ){
		X = wdef[mw].X2;
		Y = wdef[mw].Y2;
		Z = wdef[mw].Z2;
	}
	else {
		X = wdef[mw].X1;
		Y = wdef[mw].Y1;
		Z = wdef[mw].Z1;
	}
	AdjSeg(mw);
	DispSelWire();
	GetPB()->Invalidate();
}
//---------------------------------------------------------------------------
// 端点を分離
void __fastcall TWireCadDlg::K5Click(TObject *Sender)
{
	if( SelWire == -1 ) return;

	SaveWire.SetData(SelWire, wmax, wdef);
	double XX = wdef[SelWire].X2 - wdef[SelWire].X1;
	double YY = wdef[SelWire].Y2 - wdef[SelWire].Y1;
	double ZZ = wdef[SelWire].Z2 - wdef[SelWire].Z1;
	if( SelCmd == 1 ){
		wdef[SelWire].X1 += (0.001 * SGN(XX));
		wdef[SelWire].Y1 += (0.001 * SGN(YY));
		wdef[SelWire].Z1 += (0.001 * SGN(ZZ));
	}
	else {
		wdef[SelWire].X2 -= (0.001 * SGN(XX));
		wdef[SelWire].Y2 -= (0.001 * SGN(YY));
		wdef[SelWire].Z2 -= (0.001 * SGN(ZZ));
	}
	AdjSeg(SelWire);
	DispSelWire();
	GetPB()->Invalidate();
}
//---------------------------------------------------------------------------
// ワイヤの分割
void __fastcall TWireCadDlg::PartWire(int n)
{
	if( SelWire == -1 ) return;
	if( wmax >= WMAX ) return;
	if( CheckFree(n - 1) != TRUE ) return;

	SaveWire.SetData(SelWire, wmax, wdef);
	memcpy(&wsave, &wdef[SelWire], sizeof(wsave));
	memcpy(&wdisp, &wdef[SelWire], sizeof(wdisp));
	double	XX = (wsave.X2 - wsave.X1)/double(n);
	double	YY = (wsave.Y2 - wsave.Y1)/double(n);
	double	ZZ = (wsave.Z2 - wsave.Z1)/double(n);
	wdef[SelWire].X2 = wsave.X1 + XX;
	wdef[SelWire].Y2 = wsave.Y1 + YY;
	wdef[SelWire].Z2 = wsave.Z1 + ZZ;
	wdisp.X1 = wdef[SelWire].X2;
	wdisp.Y1 = wdef[SelWire].Y2;
	wdisp.Z1 = wdef[SelWire].Z2;
	int i, w;
	for( i = 0; i < (n - 2); i++ ){
		wdisp.X2 = wdisp.X1 + XX;
		wdisp.Y2 = wdisp.Y1 + YY;
		wdisp.Z2 = wdisp.Z1 + ZZ;
		w = GetNewWire();
		memcpy(&wdef[w], &wdisp, sizeof(wdisp));
		glist[w] = 0;
		AdjSeg(w);
		wdisp.X1 = wdisp.X2;
		wdisp.Y1 = wdisp.Y2;
		wdisp.Z1 = wdisp.Z2;
	}
	wdisp.X2 = wsave.X2;
	wdisp.Y2 = wsave.Y2;
	wdisp.Z2 = wsave.Z2;
	w = GetNewWire();
	memcpy(&wdef[w], &wdisp, sizeof(wdisp));
	glist[w] = 0;
	AdjSeg(w);
	DispSelWire();
	GetPB()->Invalidate();
}
//---------------------------------------------------------------------------
// ワイヤの分割
void __fastcall TWireCadDlg::KP2Click(TObject *Sender)
{
	PartWire(2);
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::KP3Click(TObject *Sender)
{
	PartWire(3);
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::KP4Click(TObject *Sender)
{
	PartWire(4);
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::KP5Click(TObject *Sender)
{
	PartWire(5);
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::KP6Click(TObject *Sender)
{
	PartWire(6);
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::KP7Click(TObject *Sender)
{
	PartWire(7);
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::KP8Click(TObject *Sender)
{
	PartWire(8);
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::KP9Click(TObject *Sender)
{
	PartWire(9);
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::KP10Click(TObject *Sender)
{
	PartWire(10);
}
//---------------------------------------------------------------------------
// このワイヤを削除
void __fastcall TWireCadDlg::K3Click(TObject *Sender)
{
	if( SelWire == -1 ) return;
	SaveWire.SetData(SelWire, wmax, wdef);
	wdef[SelWire].PNo = 0;
	SelWire = -1;
	DispSelWire();
	GetPB()->Invalidate();
}
//---------------------------------------------------------------------------
// グリッド原点を初期化
void __fastcall TWireCadDlg::K7Click(TObject *Sender)
{
	GridX = GridY = 0;
	GetPB()->Invalidate();
}
//---------------------------------------------------------------------------
// グリッド原点をワイヤ端点にする
void __fastcall TWireCadDlg::KX11Click(TObject *Sender)
{
	if( SelWire == -1 ) return;

	if( Page->ActivePage == TabXY ){
		GridX = (SelCmd == 2) ? wdef[SelWire].X2 : wdef[SelWire].X1;
		GridY = (SelCmd == 2) ? wdef[SelWire].Y2 : wdef[SelWire].Y1;
	}
	else if( Page->ActivePage == TabXZ ){
		GridX = (SelCmd == 2) ? wdef[SelWire].X2 : wdef[SelWire].X1;
		GridY = (SelCmd == 2) ? wdef[SelWire].Z2 : wdef[SelWire].Z1;
	}
	else if( Page->ActivePage == TabYZ ){
		GridX = (SelCmd == 2) ? wdef[SelWire].Y2 : wdef[SelWire].Y1;
		GridY = (SelCmd == 2) ? wdef[SelWire].Z2 : wdef[SelWire].Z1;
	}
	GetPB()->Invalidate();
}
//---------------------------------------------------------------------------
// センタリング
void __fastcall TWireCadDlg::K10Click(TObject *Sender)
{
	if( SelWire == -1 ) return;

	double *X1, *Y1, *X2, *Y2;
	double w;
	if( Page->ActivePage == TabXY ){
		X1 = &wdef[SelWire].X1;
		Y1 = &wdef[SelWire].Y1;
		X2 = &wdef[SelWire].X2;
		Y2 = &wdef[SelWire].Y2;
	}
	else if( Page->ActivePage == TabXZ ){
		X1 = &wdef[SelWire].X1;
		Y1 = &wdef[SelWire].Z1;
		X2 = &wdef[SelWire].X2;
		Y2 = &wdef[SelWire].Z2;
	}
	else if( Page->ActivePage == TabYZ ){
		X1 = &wdef[SelWire].Y1;
		Y1 = &wdef[SelWire].Z1;
		X2 = &wdef[SelWire].Y2;
		Y2 = &wdef[SelWire].Z2;
	}
	else {
		return;
	}
	SaveWire.SetData(SelWire, wmax, wdef);
	if( ABS(*X2 - *X1) > ABS(*Y2 - *Y1) ){
		w = *X2 - *X1;
		*X1 = -w/2;
		*X2 = w/2;
	}
	else {
		w = *Y2 - *Y1;
		*Y1 = -w/2;
		*Y2 = w/2;
	}
	AdjWireChen(wdef, wmax, wdisp.X1, wdisp.Y1, wdisp.Z1, wdef[SelWire].X1, wdef[SelWire].Y1, wdef[SelWire].Z1);
	AdjWireChen(wdef, wmax, wdisp.X2, wdisp.Y2, wdisp.Z2, wdef[SelWire].X2, wdef[SelWire].Y2, wdef[SelWire].Z2);
	GetPB()->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::AdjGrid(int sw)
{
	if( ChkGrid->Checked != TRUE ) return;
	double	grid = double(GetPB()->Width)/(AntSC * 100.0);
	if( (sw && ChkAutoGrid->Checked == TRUE) || (grid > Grid) ){
		int i;
		for( i = 0; GridTable[i]; i++ ){
			Grid = double(GridTable[i])/100.0;
			if( grid <= Grid ) break;
		}
		Grid = RoundUpStr(Grid);
		EnbSetup = 0;
		GridWidth->Text = StrDbl(Grid);
		EnbSetup = 1;
		GetPB()->Invalidate();
	}
	else if( Calc(grid, AnsiString(GridWidth->Text).c_str()) == TRUE ){	// ja7ude 1.0
		if( grid != Grid ){
			EnbSetup = 0;
			GridWidth->Text = StrDbl(Grid);
			EnbSetup = 1;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::GridWidthChange(TObject *Sender)
{
	if( !EnbSetup ) return;

	double	d;
	if( Calc(d, AnsiString(GridWidth->Text).c_str()) == TRUE ){
		if( (d > 0.0) && (d != Grid) ){
			Grid = d;
			if( ChkGrid->Checked == TRUE ){
				TimerCmd = 0;
				Timer->Enabled = TRUE;
				AdjGrid(0);
				GetPB()->Invalidate();
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::ChkGridClick(TObject *Sender)
{
	if( !EnbSetup ) return;

	AdjGrid(1);
	GetPB()->Invalidate();
}
//---------------------------------------------------------------------------
// ページの変更
void __fastcall TWireCadDlg::PageChange(TObject *Sender)
{
	UpdateView();
	SetupSB();
	DispCursor();
	DispWire();
}
//---------------------------------------------------------------------------
// 表示フィルタの選択
void __fastcall TWireCadDlg::SelFilterChange(TObject *Sender)
{
	if( !EnbSetup ) return;
	for( Filter = 0; FilterTable[Filter]!=NULL; Filter++ ){
		if( !strcmp(AnsiString(SelFilter->Text).c_str(), FilterTable[Filter]) ) break;
	}
	if( FilterTable[Filter] == NULL ){
		Filter = 0;
		EnbSetup = 0;
		SelFilter->Text = FilterTable[0];
		EnbSetup = 1;
	}
	EntryGroup();
	GetPB()->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::TimerTimer(TObject *Sender)
{
	int MWX = 0;
	int MWY = 0;
	TPaintBox *tp;
	TShiftState Shift;
	switch(TimerCmd){
		case 0:		// グリッドの補正
			Timer->Enabled = FALSE;
			AdjGrid(0);
			break;
		case 1:		// ビューの移動
			if( !DownF ){
				Timer->Enabled = FALSE;
			}
			tp = GetPB();
			if( AntMX < 0 ){
				MWX = AntMX;
			}
			else if( AntMX >= tp->Width ){
				MWX = AntMX - tp->Width;
			}
			if( AntMY < 0 ){
				MWY = AntMY;
			}
			else if( AntMY >= tp->Height ){
				MWY = AntMY - tp->Height;
			}
			if( MWX || MWY ){
				ScrXc -= MWX;
				ScrYc -= MWY;
				SaveMX -= MWX;
				SaveMY -= MWY;
				AntXc = double(ScrXc - tp->Width/2)/AntSC;
				AntYc = double(ScrYc - tp->Height/2)/AntSC;
				SetupSB();
				if( Page->ActivePage == TabAll ){
					PBoxALLMouseMove(NULL, Shift, AntMX, AntMY);
					PBoxALLPaint(NULL);
					if( (Mode == 1) || (MoveF >= 10) ){
						DrawNot(PBoxALL, SaveMX, SaveMY, AntMX, AntMY);
					}
				}
				else if( Page->ActivePage == TabXY ){
					PBoxXYMouseMove(NULL, Shift, AntMX, AntMY);
					PBoxXYPaint(NULL);
					if( Mode ){				// 新規作成
						switch(Mode){
							case 1:
								DrawNot(PBoxXY, wdisp.X1, wdisp.Y1, wdisp.X2, wdisp.Y2);
								break;
							case 2:
								DrawNotLoop(PBoxXY, wdisp.X1, wdisp.Y1, wdisp.X2, wdisp.Y2);
								break;
						}
					}
					else if( MoveF >= 10 ){		// 変更
						DrawNot(PBoxXY, wdisp.X1, wdisp.Y1, wdisp.X2, wdisp.Y2);
					}
				}
				else if( Page->ActivePage == TabXZ ){
					PBoxXZMouseMove(NULL, Shift, AntMX, AntMY);
					PBoxXZPaint(NULL);
					if( Mode ){				// 新規作成
						switch(Mode){
							case 1:
								DrawNot(PBoxXZ, wdisp.X1, wdisp.Z1, wdisp.X2, wdisp.Z2);
								break;
							case 2:
								DrawNotLoop(PBoxXZ, wdisp.X1, wdisp.Z1, wdisp.X2, wdisp.Z2);
								break;
						}
					}
					else if( MoveF >= 10 ){		// 変更
						DrawNot(PBoxXZ, wdisp.X1, wdisp.Z1, wdisp.X2, wdisp.Z2);
					}
				}
				else if( Page->ActivePage == TabYZ ){
					PBoxYZMouseMove(NULL, Shift, AntMX, AntMY);
					PBoxYZPaint(NULL);
					if( Mode ){				// 新規作成
						switch(Mode){
							case 1:
								DrawNot(PBoxYZ, wdisp.Y1, wdisp.Z1, wdisp.Y2, wdisp.Z2);
								break;
							case 2:
								DrawNotLoop(PBoxYZ, wdisp.Y1, wdisp.Z1, wdisp.Y2, wdisp.Z2);
								break;
						}
					}
					else if( MoveF >= 10 ){		// 変更
						DrawNot(PBoxYZ, wdisp.Y1, wdisp.Z1, wdisp.Y2, wdisp.Z2);
					}
				}
				DispWire();
			}
			break;
		case 2:		// スクロール中
			if( ScrollF ){
				ScrollF--;
				if( !ScrollF ){
					Timer->Enabled = FALSE;
					GetPB()->Invalidate();
				}
			}
			break;
		case 3:		// スクロール中、上に張り付き
			VSBYc += SBSC * 5;
			AntYc = VSBYc + (50.0 * SBSC);
			Scroll();
			break;
		case 4:		// スクロール中、下に張り付き
			VSBYc -= SBSC * 5;
			AntYc = VSBYc - (50.0 * SBSC);
			Scroll();
			break;
		case 5:		// スクロール中、左に張り付き
			HSBXc += SBSC * 5;
			AntXc = HSBXc + (50.0 * SBSC);
			Scroll();
			break;
		case 6:		// スクロール中、右に張り付き
			HSBXc -= SBSC * 10;
			AntXc = HSBXc - (50.0 * SBSC);
			Scroll();
			break;
		default:
			Timer->Enabled = FALSE;
			break;
	}
}
//---------------------------------------------------------------------------
// マウスによるワイヤの選択
int __fastcall TWireCadDlg::SelectWireAllSub(int X, int Y)
{
	// ワイヤの検索
	double x, y;
	POINT	PP, BP, EP;
	PP.x = X; PP.y = Y;
	int i;
	int mw = 9;
	int w, mi;
	for( i = 0; i < wmax; i++ ){
		if( !IsFilter(i) ) continue;
		Calc3DXY(x, y, AntDeg, AntZDeg, wdef[i].X1, wdef[i].Y1, wdef[i].Z1);
		BP.x = int((x * AntSC)) + ScrXc;
		BP.y = ScrYc - int((y * AntSC));
		Calc3DXY(x, y, AntDeg, AntZDeg, wdef[i].X2, wdef[i].Y2, wdef[i].Z2);
		EP.x = int((x * AntSC)) + ScrXc;
		EP.y = ScrYc - int((y * AntSC));
		for( w = 1; w < mw; w++ ){
			if( IsLine(PP, BP, EP, w) != 0 ){
				mw = w;
				mi = i;
				break;
			}
		}
	}
	if( mw != 9 ){
		return mi;
	}
	else {
		return -1;
	}
}
//---------------------------------------------------------------------------
// マウスによるワイヤの選択
int __fastcall TWireCadDlg::SelectWireAll(int X, int Y)
{
	SelWire = SelectWireAllSub(X, Y);
	if( SelWire != -1 ){
		memcpy(&wdisp, &wdef[SelWire], sizeof(wdisp));
	}
	DispWire();
	return TRUE;
}
//---------------------------------------------------------------------------
// マウスによるワイヤの選択
int __fastcall TWireCadDlg::SelectWireXY(int X, int Y)
{
	POINT	PP, BP, EP;
	PP.x = X; PP.y = Y;
	// ワイヤの検索
	int i;
	int mw = 9;
	int w, mi;
	for( i = 0; i < wmax; i++ ){
		if( !IsFilter(i) ) continue;
		BP.x = int((wdef[i].X1 * AntSC)) + ScrXc;
		BP.y = ScrYc - int((wdef[i].Y1 * AntSC));
		EP.x = int((wdef[i].X2 * AntSC)) + ScrXc;
		EP.y = ScrYc - int((wdef[i].Y2 * AntSC));
		for( w = 1; w < mw; w++ ){
			if( IsLine(PP, BP, EP, w) != 0 ){
				mw = w;
				mi = i;
				break;
			}
		}
	}
	if( mw != 9 ){
		SelWire = mi;
		memcpy(&wdisp, &wdef[SelWire], sizeof(wdisp));
	}
	else {
		SelWire = -1;
	}
	DispWire();
	return TRUE;
}
//---------------------------------------------------------------------------
// マウスによるワイヤの選択
int __fastcall TWireCadDlg::SelectWireXZ(int X, int Y)
{
	POINT	PP, BP, EP;
	PP.x = X; PP.y = Y;
	// ワイヤの検索
	int i;
	int mw = 9;
	int w, mi;
	for( i = 0; i < wmax; i++ ){
		if( !IsFilter(i) ) continue;
		BP.x = int((wdef[i].X1 * AntSC)) + ScrXc;
		BP.y = ScrYc - int((wdef[i].Z1 * AntSC));
		EP.x = int((wdef[i].X2 * AntSC)) + ScrXc;
		EP.y = ScrYc - int((wdef[i].Z2 * AntSC));
		for( w = 1; w < mw; w++ ){
			if( IsLine(PP, BP, EP, w) != 0 ){
				mw = w;
				mi = i;
				break;
			}
		}
	}
	if( mw != 9 ){
		SelWire = mi;
		memcpy(&wdisp, &wdef[SelWire], sizeof(wdisp));
	}
	else {
		SelWire = -1;
	}
	DispWire();
	return TRUE;
}
//---------------------------------------------------------------------------
// マウスによるワイヤの選択
int __fastcall TWireCadDlg::SelectWireYZ(int X, int Y)
{
	POINT	PP, BP, EP;
	PP.x = X; PP.y = Y;
	// ワイヤの検索
	int i;
	int mw = 9;
	int w, mi;
	for( i = 0; i < wmax; i++ ){
		if( !IsFilter(i) ) continue;
		BP.x = int((wdef[i].Y1 * AntSC)) + ScrXc;
		BP.y = ScrYc - int((wdef[i].Z1 * AntSC));
		EP.x = int((wdef[i].Y2 * AntSC)) + ScrXc;
		EP.y = ScrYc - int((wdef[i].Z2 * AntSC));
		for( w = 1; w < mw; w++ ){
			if( IsLine(PP, BP, EP, w) != 0 ){
				mw = w;
				mi = i;
				break;
			}
		}
	}
	if( mw != 9 ){
		SelWire = mi;
		memcpy(&wdisp, &wdef[SelWire], sizeof(wdisp));
	}
	else {
		SelWire = -1;
	}
	DispWire();
	return TRUE;
}
//---------------------------------------------------------------------------
int __fastcall TWireCadDlg::IsWireHitAll(int w, int X, int Y)
{
	POINT	PP, BP, EP;
	PP.x = X; PP.y = Y;
	double x, y;
	Calc3DXY(x, y, AntDeg, AntZDeg, wdef[w].X1, wdef[w].Y1, wdef[w].Z1);
	BP.x = int((x * AntSC)) + ScrXc;
	BP.y = ScrYc - int((y * AntSC));
	Calc3DXY(x, y, AntDeg, AntZDeg, wdef[w].X2, wdef[w].Y2, wdef[w].Z2);
	EP.x = int((x * AntSC)) + ScrXc;
	EP.y = ScrYc - int((y * AntSC));
	return IsLine(PP, BP, EP);
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::PBoxALLMouseDown(TObject *Sender,
	TMouseButton Button, TShiftState Shift, int X, int Y)
{
	AntMX = X;
	AntMY = Y;
	if( Button != mbLeft ) return;
	if( Mode && (Mode != 1) ) return;
	if( wmax < 2 ) return;
	int Sel = SelWire;
	if( Mode ){
		Sel = SelectWireAllSub(X, Y);
	}
	if( Sel == -1 ) return;
	POINT	PP, BP, EP;
	PP.x = X; PP.y = Y;
	double x, y;
	Calc3DXY(x, y, AntDeg, AntZDeg, wdef[Sel].X1, wdef[Sel].Y1, wdef[Sel].Z1);
	BP.x = int((x * AntSC)) + ScrXc;
	BP.y = ScrYc - int((y * AntSC));
	Calc3DXY(x, y, AntDeg, AntZDeg, wdef[Sel].X2, wdef[Sel].Y2, wdef[Sel].Z2);
	EP.x = int((x * AntSC)) + ScrXc;
	EP.y = ScrYc - int((y * AntSC));
	MoveF = IsLine(PP, BP, EP);
	memcpy(&wsave, &wdef[Sel], sizeof(wsave));
	memcpy(&wdisp, &wdef[Sel], sizeof(wdisp));
	switch(MoveF){
		case 1:
			if( Mode ){
				SaveMX = X;
				SaveMY = Y;
			}
			else {
				SaveMX = EP.x;
				SaveMY = EP.y;
			}
			DownF = 1;
			break;
		case 2:
			if( Mode ){
				wdisp.X1 = wdisp.X2;
				wdisp.Y1 = wdisp.Y2;
				wdisp.Z1 = wdisp.Z2;
				SaveMX = X;
				SaveMY = Y;
			}
			else {
				SaveMX = BP.x;
				SaveMY = BP.y;
			}
			DownF = 1;
			break;
		default:
			MoveF = 0;
			break;
	}
	if( Mode ) MoveF = 0;
	TimerCmd = 1;
	Timer->Enabled = TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::PBoxALLMouseUp(TObject *Sender,
	TMouseButton Button, TShiftState Shift, int X, int Y)
{
	int		w;

	if( !DownF ) return;
	DownF = 0;
	if( Button != mbLeft ) return;

	// ワイヤの検索
	double x, y;
	int i, XX, YY;
	int D;
	int MD = MAXINT;
	int Sel = -1;
	int cmd;
	for( i = 0; i < wmax; i++ ){
		if( !IsFilter(i) ) continue;
		Calc3DXY(x, y, AntDeg, AntZDeg, wdef[i].X1, wdef[i].Y1, wdef[i].Z1);
		XX = int((x * AntSC)) + ScrXc - X;
		YY = ScrYc - int((y * AntSC)) - Y;
		D = (XX * XX) + (YY * YY);
		if( MD > D ){
			MD = D;
			cmd = 1;
			Sel = i;
		}
		Calc3DXY(x, y, AntDeg, AntZDeg, wdef[i].X2, wdef[i].Y2, wdef[i].Z2);
		XX = int((x * AntSC)) + ScrXc - X;
		YY = ScrYc - int((y * AntSC)) - Y;
		D = (XX * XX) + (YY * YY);
		if( MD > D ){
			MD = D;
			cmd = 2;
			Sel = i;
		}
	}
	if( Sel == -1 ){
		MoveF = 0;
		return;
	}
	if( Mode ){					// 新規ワイヤの登録
		MoveF = 0;
		if( Mode == 1 ){
			if( CheckFree(1) == TRUE ){
				switch(cmd){
					case 1:
						wdisp.X2 = wdef[Sel].X1;
						wdisp.Y2 = wdef[Sel].Y1;
						wdisp.Z2 = wdef[Sel].Z1;
						break;
					case 2:
						wdisp.X2 = wdef[Sel].X2;
						wdisp.Y2 = wdef[Sel].Y2;
						wdisp.Z2 = wdef[Sel].Z2;
						break;
				}
				if( ((wdisp.X1!=wdisp.X2)||(wdisp.Y1!=wdisp.Y2)||(wdisp.Z1!=wdisp.Z2)) && (!IsWire(wdisp)) ){
					SaveWire.SetData(SelWire, wmax, wdef);
					wdisp.SEG = -1;
					w = GetNewWire();
					memcpy(&wdef[w], &wdisp, sizeof(wdisp));
					glist[w] = 0;
					AdjSeg(w);
					SelWire = w;
				}
			}
		}
		DispSelWire();
		PBoxALL->Invalidate();
	}
	else if( MoveF >= 10 ){		// ワイヤの座標変更
		w = 0;
		switch(MoveF){
			case 11:
				switch(cmd){
					case 1:
						wdisp.X1 = wdef[Sel].X1;
						wdisp.Y1 = wdef[Sel].Y1;
						wdisp.Z1 = wdef[Sel].Z1;
						break;
					case 2:
						wdisp.X1 = wdef[Sel].X2;
						wdisp.Y1 = wdef[Sel].Y2;
						wdisp.Z1 = wdef[Sel].Z2;
						break;
				}
				if( ((wdisp.X1!=wdisp.X2)||(wdisp.Y1!=wdisp.Y2)||(wdisp.Z1!=wdisp.Z2)) && (!IsWire(wdisp)) ){
					SaveWire.SetData(SelWire, wmax, wdef);
					AdjWireChen(wdef, wmax, wsave.X1, wsave.Y1, wsave.Z1, wdisp.X1, wdisp.Y1, wdisp.Z1);
					w = 1;
				}
				break;
			case 12:
				switch(cmd){
					case 1:
						wdisp.X2 = wdef[Sel].X1;
						wdisp.Y2 = wdef[Sel].Y1;
						wdisp.Z2 = wdef[Sel].Z1;
						break;
					case 2:
						wdisp.X2 = wdef[Sel].X2;
						wdisp.Y2 = wdef[Sel].Y2;
						wdisp.Z2 = wdef[Sel].Z2;
						break;
				}
				if( ((wdisp.X1!=wdisp.X2)||(wdisp.Y1!=wdisp.Y2)||(wdisp.Z1!=wdisp.Z2)) && (!IsWire(wdisp)) ){
					SaveWire.SetData(SelWire, wmax, wdef);
					AdjWireChen(wdef, wmax, wsave.X2, wsave.Y2, wsave.Z2, wdisp.X2, wdisp.Y2, wdisp.Z2);
					w = 1;
				}
				break;
		}
		if( w ){
			memcpy(&wdef[SelWire], &wdisp, sizeof(wdisp));
			AdjSeg(SelWire);
		}
		PBoxALL->Invalidate();
	}
	MoveF = 0;
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::PBoxALLMouseMove(TObject *Sender,
	TShiftState Shift, int X, int Y)
{
	if( DownF ){				// マウスボタン押し中
		SetShiftStat(Shift);
		if( Mode == 1 ){		// 新規作成
			DrawNot(PBoxALL, SaveMX, SaveMY, AntMX, AntMY);
			AntMX = X;
			AntMY = Y;
			DrawNot(PBoxALL, SaveMX, SaveMY, AntMX, AntMY);
		}
		else if( MoveF ){		// 変更
			if( MoveF >= 10 ){
				DrawNot(PBoxALL, SaveMX, SaveMY, AntMX, AntMY);
			}
			else {
				if( ABS(AntMX - SaveMX) >= 8 ){
					MoveF += 10;
				}
				else if( ABS(AntMY - SaveMY) >= 8 ){
					MoveF += 10;
				}
			}
			AntMX = X;
			AntMY = Y;
			if( MoveF >= 10 ){
				DrawNot(PBoxALL, SaveMX, SaveMY, AntMX, AntMY);
			}
		}
	}
	else if( (Mode == 1) || (SelWire != -1) ){	// マウスボタン離し中
		int Sel = SelWire;
		if( Mode ){
			Sel = SelectWireAllSub(X, Y);
		}
		if( (wmax >= 2) && (Sel != -1) ){
			// ワイヤの検索
			int r = IsWireHitAll(Sel, X, Y);
			switch(r){
				case 1:
					PBoxALL->Cursor = crCross;
					break;
				case 2:
					PBoxALL->Cursor = crCross;
					break;
				default:
					PBoxALL->Cursor = crArrow;
					break;
			}
		}
		else {
			PBoxALL->Cursor = crArrow;
		}
	}
	else {
		PBoxALL->Cursor = crArrow;
	}
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::PBoxALLClick(TObject *Sender)
{
	if( (MoveF >= 10) || Mode ){
		PBoxALL->Invalidate();
	}
	else if( SelectWireAll(AntMX, AntMY) == TRUE ){
		PBoxALL->Invalidate();
	}
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::AdjAngle(double &X1, double &Y1, double &X2, double &Y2)
{
	if( ABS(X2 - X1) > ABS(Y2 - Y1) ){
		Y2 = Y1;
	}
	else {
		X2 = X1;
	}
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::PBoxXYMouseDown(TObject *Sender,
	TMouseButton Button, TShiftState Shift, int X, int Y)
{
	AntMX = X;
	AntMY = Y;
	if( Button != mbLeft ) return;
	DownF = 1;
	if( Mode ){
		memcpy(&wsave, &wdef[0], sizeof(wsave));
		if( !wsave.R ) wsave.R = 0.0008;
		wsave.PNo = 1;
		wsave.X2 = wsave.X1 = double(AntMX - ScrXc)/AntSC;
		wsave.Y2 = wsave.Y1 = double(ScrYc - AntMY)/AntSC;
		wsave.Z1 = wsave.Z2 = GetXYZ(2);
		memcpy(&wdisp, &wsave, sizeof(wdisp));
		SaveMX = X;
		SaveMY = Y;
		switch(Mode){
			case 1:
				DrawNot(PBoxXY, wdisp.X1, wdisp.Y1, wdisp.X2, wdisp.Y2);
				break;
			case 2:
				DrawNotLoop(PBoxXY, wdisp.X1, wdisp.Y1, wdisp.X2, wdisp.Y2);
				break;
		}
	}
	else if( SelWire != -1 ){
		POINT	PP, BP, EP;
		PP.x = X; PP.y = Y;
		BP.x = int((wdef[SelWire].X1 * AntSC)) + ScrXc;
		BP.y = ScrYc - int((wdef[SelWire].Y1 * AntSC));
		EP.x = int((wdef[SelWire].X2 * AntSC)) + ScrXc;
		EP.y = ScrYc - int((wdef[SelWire].Y2 * AntSC));
		MoveF=IsLine(PP, BP, EP);
		switch(MoveF){
			case 1:
			case 2:
			case 3:
				memcpy(&wsave, &wdef[SelWire], sizeof(wsave));
				memcpy(&wdisp, &wdef[SelWire], sizeof(wdisp));
				SaveMX = X;
				SaveMY = Y;
				break;
			default:
				MoveF = 0;
				break;
		}
	}
	TimerCmd = 1;
	Timer->Enabled = TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::PBoxXYMouseUp(TObject *Sender, TMouseButton Button,
	TShiftState Shift, int X, int Y)
{
	double	xx, yy;
	int		w;

	DownF = 0;
	if( Button != mbLeft ) return;
	if( Mode ){					// 新規ワイヤの登録
		NormalXY(wdisp.X1, wdisp.Y1);
		NormalXY(wdisp.X2, wdisp.Y2);
		if( (wdisp.X1 == wdisp.X2) && (wdisp.Y1 == wdisp.Y2) ){
			PBoxXY->Invalidate();
			MoveF = 0;
			return;
		}
		if( Mode == 1 ){
			if( CheckFree(1) == TRUE ){
				if( !IsWire(wdisp) ){
					SaveWire.SetData(SelWire, wmax, wdef);
					wdisp.SEG = 0;
					w = GetNewWire();
					memcpy(&wdef[w], &wdisp, sizeof(wdisp));
					glist[w] = 0;
					AdjSeg(w);
					SelWire = w;
				}
			}
		}
		else {
			if( CheckFree(4) == TRUE ){
				SaveWire.SetData(SelWire, wmax, wdef);
				AdjLoop(wdisp.X1, wdisp.Y1, wdisp.X2, wdisp.Y2);
				wdisp.SEG = -1;
				w = GetNewWire();
				memcpy(&wdef[w], &wdisp, sizeof(wdisp));
				wdef[w].Y1 = wdisp.Y2;
				glist[w] = 0;
				AdjSeg(w);
				w = GetNewWire();
				memcpy(&wdef[w], &wdisp, sizeof(wdisp));
				wdef[w].X2 = wdisp.X1;
				glist[w] = 0;
				AdjSeg(w);
				w = GetNewWire();
				memcpy(&wdef[w], &wdisp, sizeof(wdisp));
				wdef[w].Y2 = wdisp.Y1;
				glist[w] = 0;
				AdjSeg(w);
				w = GetNewWire();
				memcpy(&wdef[w], &wdisp, sizeof(wdisp));
				wdef[w].X1 = wdisp.X2;
				glist[w] = 0;
				AdjSeg(w);
				SelWire = w;
			}
		}
		DispSelWire();
		PBoxXY->Invalidate();
	}
	else if( MoveF >= 10 ){		// ワイヤの座標変更
		SaveWire.SetData(SelWire, wmax, wdef);
		switch(MoveF){
			case 11:
				if( ShiftF != 2 ){
					NormalXXYY(wdisp.X2, wdisp.Y2, wdisp.X1, wdisp.Y1);
				}
				AdjWireChen(wdef, wmax, wsave.X1, wsave.Y1, wsave.Z1, wdisp.X1, wdisp.Y1, wdisp.Z1);
				break;
			case 12:
				if( ShiftF != 2 ){
					NormalXXYY(wdisp.X1, wdisp.Y1, wdisp.X2, wdisp.Y2);
				}
				AdjWireChen(wdef, wmax, wsave.X2, wsave.Y2, wsave.Z2, wdisp.X2, wdisp.Y2, wdisp.Z2);
				break;
			case 13:		// 平衡移動
				xx = wsave.X2 - wsave.X1;
				yy = wsave.Y2 - wsave.Y1;
				NormalXXYY(wsave.X1, wsave.Y1, wdisp.X1, wdisp.Y1);
				wdisp.X2 = wdisp.X1 + xx;
				wdisp.Y2 = wdisp.Y1 + yy;
				AdjWireChen(wdef, wmax, wsave.X1, wsave.Y1, wsave.Z1, wdisp.X1, wdisp.Y1, wdisp.Z1);
				AdjWireChen(wdef, wmax, wsave.X2, wsave.Y2, wsave.Z2, wdisp.X2, wdisp.Y2, wdisp.Z2);
				break;
		}
		memcpy(&wdef[SelWire], &wdisp, sizeof(wdisp));
//		AdjSeg(SelWire);
		PBoxXY->Invalidate();
	}
	MoveF = 0;
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::SetShiftStat(TShiftState Shift)
{
	ShiftF = 0;
	TShiftState	S1;
	TShiftState S2 = Shift;
	S1 << ssShift;
	S2 *= S1;
	if( S1 == S2 ){
		ShiftF = 1;
	}
	else {
		S1.Clear();
		S1 << ssCtrl;
		Shift *= S1;
		if( S1 == Shift ){
			ShiftF = 2;
		}
	}
}
//---------------------------------------------------------------------------
// 円補正をする
void __fastcall TWireCadDlg::AdjPie(double X1, double Y1, double X2, double Y2, double &X, double &Y)
{
	double deg = GetDeg(X - X1, Y - Y1);
	double XX = X2 - X1;
	double YY = Y2 - Y1;
	double d = sqrt(XX*XX + YY*YY);
	deg = RoundUp(deg, 1)*PAI/180.0;
	X = X1 + (d * cos(deg));
	Y = Y1 + (d * sin(deg));
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::PBoxXYMouseMove(TObject *Sender, TShiftState Shift,
	int X, int Y)
{
	AntMX = X;
	AntMY = Y;
	DispCursor();
	if( DownF ){				// マウスボタン押し中
		SetShiftStat(Shift);
		if( Mode ){				// 新規作成
			switch(Mode){
				case 1:
					DrawNot(PBoxXY, wdisp.X1, wdisp.Y1, wdisp.X2, wdisp.Y2);
					break;
				case 2:
					DrawNotLoop(PBoxXY, wdisp.X1, wdisp.Y1, wdisp.X2, wdisp.Y2);
					break;
			}
			double	x, y;
			x = double(AntMX - SaveMX)/AntSC;
			y = double(SaveMY - AntMY)/AntSC;
			wdisp.X2 = wsave.X2 + x; wdisp.Y2 = wsave.Y2 + y;
			switch(Mode){
				case 1:
					if( ShiftF ){
						AdjAngle(wdisp.X1, wdisp.Y1, wdisp.X2, wdisp.Y2);
					}
					DrawNot(PBoxXY, wdisp.X1, wdisp.Y1, wdisp.X2, wdisp.Y2);
					break;
				case 2:
					DrawNotLoop(PBoxXY, wdisp.X1, wdisp.Y1, wdisp.X2, wdisp.Y2);
					break;
			}
		}
		else if( MoveF ){		// 変更
			if( MoveF >= 10 ){
				DrawNot(PBoxXY, wdisp.X1, wdisp.Y1, wdisp.X2, wdisp.Y2);
			}
			else {
				if( ABS(AntMX - SaveMX) >= 8 ){
					MoveF += 10;
				}
				else if( ABS(AntMY - SaveMY) >= 8 ){
					MoveF += 10;
				}
			}
			double	x, y;
			x = double(AntMX - SaveMX)/AntSC;
			y = double(SaveMY - AntMY)/AntSC;
			switch(MoveF){
				case 11:			// 始点を移動
					wdisp.X1 = wsave.X1 + x; wdisp.Y1 = wsave.Y1 + y;
					if( ShiftF == 1 ){
						AdjAngle(wdisp.X2, wdisp.Y2, wdisp.X1, wdisp.Y1);
					}
					else if( ShiftF == 2 ){
						AdjPie(wsave.X2, wsave.Y2, wsave.X1, wsave.Y1, wdisp.X1, wdisp.Y1);
					}
					break;
				case 12:			// 終点を移動
					wdisp.X2 = wsave.X2 + x; wdisp.Y2 = wsave.Y2 + y;
					if( ShiftF == 1 ){
						AdjAngle(wdisp.X1, wdisp.Y1, wdisp.X2, wdisp.Y2);
					}
					else if( ShiftF == 2 ){
						AdjPie(wsave.X1, wsave.Y1, wsave.X2, wsave.Y2, wdisp.X2, wdisp.Y2);
					}
					break;
				case 13:			// 全体を移動
					wdisp.X1 = wsave.X1 + x; wdisp.Y1 = wsave.Y1 + y;
					wdisp.X2 = wsave.X2 + x; wdisp.Y2 = wsave.Y2 + y;
					if( ShiftF ){
						if( ABS(x) > ABS(y) ){
							wdisp.Y1 = wsave.Y1;
							wdisp.Y2 = wsave.Y2;
						}
						else {
							wdisp.X1 = wsave.X1;
							wdisp.X2 = wsave.X2;
						}
					}
					break;
			}
			if( MoveF >= 10 ){
				DrawNot(PBoxXY, wdisp.X1, wdisp.Y1, wdisp.X2, wdisp.Y2);
			}
		}
	}
	else if( Mode ){			// マウスボタン放し中の新規作成
		PBoxXY->Cursor = crCross;
	}
	else if( SelWire != -1 ){	// マウスボタン離し中
		// ワイヤの検索
		POINT	PP, BP, EP;
		PP.x = X; PP.y = Y;
		BP.x = int((wdef[SelWire].X1 * AntSC)) + ScrXc;
		BP.y = ScrYc - int((wdef[SelWire].Y1 * AntSC));
		EP.x = int((wdef[SelWire].X2 * AntSC)) + ScrXc;
		EP.y = ScrYc - int((wdef[SelWire].Y2 * AntSC));
		int r = IsLine(PP, BP, EP);
		switch(r){
			case 1:
				PBoxXY->Cursor = crCross;
				break;
			case 2:
				PBoxXY->Cursor = crCross;
				break;
			case 3:
				PBoxXY->Cursor = crDrag;
				break;
			default:
				PBoxXY->Cursor = crArrow;
				break;
		}
	}
	else {
		PBoxXY->Cursor = crArrow;
	}
	DispWire();
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::PBoxXYClick(TObject *Sender)
{
	if( (MoveF >= 10) || Mode ){
		PBoxXY->Invalidate();
	}
	else if( SelectWireXY(AntMX, AntMY) == TRUE ){
		PBoxXY->Invalidate();
	}
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::PBoxXZMouseDown(TObject *Sender,
	TMouseButton Button, TShiftState Shift, int X, int Y)
{
	AntMX = X;
	AntMY = Y;
	if( Button != mbLeft ) return;
	DownF = 1;
	if( Mode ){
		memcpy(&wsave, &wdef[0], sizeof(wsave));
		if( !wsave.R ) wsave.R = 0.0008;
		wsave.PNo = 1;
		wsave.X2 = wsave.X1 = double(AntMX - ScrXc)/AntSC;
		wsave.Z2 = wsave.Z1 = double(ScrYc - AntMY)/AntSC;
		wsave.Y1 = wsave.Y2 = GetXYZ(1);
		memcpy(&wdisp, &wsave, sizeof(wdisp));
		SaveMX = X;
		SaveMY = Y;
		switch(Mode){
			case 1:
				DrawNot(PBoxXZ, wdisp.X1, wdisp.Z1, wdisp.X2, wdisp.Z2);
				break;
			case 2:
				DrawNotLoop(PBoxXZ, wdisp.X1, wdisp.Z1, wdisp.X2, wdisp.Z2);
				break;
		}
	}
	else if( SelWire != -1 ){
		POINT	PP, BP, EP;
		PP.x = X; PP.y = Y;
		BP.x = int((wdef[SelWire].X1 * AntSC)) + ScrXc;
		BP.y = ScrYc - int((wdef[SelWire].Z1 * AntSC));
		EP.x = int((wdef[SelWire].X2 * AntSC)) + ScrXc;
		EP.y = ScrYc - int((wdef[SelWire].Z2 * AntSC));
		MoveF=IsLine(PP, BP, EP);
		switch(MoveF){
			case 1:
			case 2:
			case 3:
				memcpy(&wsave, &wdef[SelWire], sizeof(wsave));
				memcpy(&wdisp, &wdef[SelWire], sizeof(wdisp));
				SaveMX = X;
				SaveMY = Y;
				break;
			default:
				MoveF = 0;
				break;
		}
	}
	TimerCmd = 1;
	Timer->Enabled = TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::PBoxXZMouseUp(TObject *Sender, TMouseButton Button,
	TShiftState Shift, int X, int Y)
{
	double	xx, yy;
	int		w;

	DownF = 0;
	if( Button != mbLeft ) return;
	if( Mode ){					// 新規ワイヤの登録
		NormalXY(wdisp.X1, wdisp.Z1);
		NormalXY(wdisp.X2, wdisp.Z2);
		if( (wdisp.X1 == wdisp.X2) && (wdisp.Z1 == wdisp.Z2) ){
			PBoxXZ->Invalidate();
			MoveF = 0;
			return;
		}
		if( Mode == 1 ){
			if( CheckFree(1) == TRUE ){
				if( !IsWire(wdisp) ){
					SaveWire.SetData(SelWire, wmax, wdef);
					wdisp.SEG = 0;
					w = GetNewWire();
					memcpy(&wdef[w], &wdisp, sizeof(wdisp));
					glist[w] = 0;
					AdjSeg(w);
					SelWire = w;
				}
			}
		}
		else {
			if( CheckFree(4) == TRUE ){
				SaveWire.SetData(SelWire, wmax, wdef);
				AdjLoop(wdisp.X1, wdisp.Z1, wdisp.X2, wdisp.Z2);
				wdisp.SEG = -1;
				w = GetNewWire();
				memcpy(&wdef[w], &wdisp, sizeof(wdisp));
				wdef[w].Z1 = wdisp.Z2;
				glist[w] = 0;
				AdjSeg(w);
				w = GetNewWire();
				memcpy(&wdef[w], &wdisp, sizeof(wdisp));
				wdef[w].X2 = wdisp.X1;
				glist[w] = 0;
				AdjSeg(w);
				w = GetNewWire();
				memcpy(&wdef[w], &wdisp, sizeof(wdisp));
				wdef[w].Z2 = wdisp.Z1;
				glist[w] = 0;
				AdjSeg(w);
				w = GetNewWire();
				memcpy(&wdef[w], &wdisp, sizeof(wdisp));
				wdef[w].X1 = wdisp.X2;
				glist[w] = 0;
				AdjSeg(w);
				SelWire = w;
			}
		}
		DispSelWire();
		PBoxXZ->Invalidate();
	}
	else if( MoveF >= 10 ){		// ワイヤの座標変更
		SaveWire.SetData(SelWire, wmax, wdef);
		switch(MoveF){
			case 11:
				if( ShiftF != 2 ){
					NormalXXYY(wdisp.X2, wdisp.Z2, wdisp.X1, wdisp.Z1);
				}
				AdjWireChen(wdef, wmax, wsave.X1, wsave.Y1, wsave.Z1, wdisp.X1, wdisp.Y1, wdisp.Z1);
				break;
			case 12:
				if( ShiftF != 2 ){
					NormalXXYY(wdisp.X1, wdisp.Z1, wdisp.X2, wdisp.Z2);
				}
				AdjWireChen(wdef, wmax, wsave.X2, wsave.Y2, wsave.Z2, wdisp.X2, wdisp.Y2, wdisp.Z2);
				break;
			case 13:		// 平衡移動
				xx = wsave.X2 - wsave.X1;
				yy = wsave.Z2 - wsave.Z1;
				NormalXXYY(wsave.X1, wsave.Z1, wdisp.X1, wdisp.Z1);
				wdisp.X2 = wdisp.X1 + xx;
				wdisp.Z2 = wdisp.Z1 + yy;
				AdjWireChen(wdef, wmax, wsave.X1, wsave.Y1, wsave.Z1, wdisp.X1, wdisp.Y1, wdisp.Z1);
				AdjWireChen(wdef, wmax, wsave.X2, wsave.Y2, wsave.Z2, wdisp.X2, wdisp.Y2, wdisp.Z2);
				break;
		}
		memcpy(&wdef[SelWire], &wdisp, sizeof(wdisp));
//		AdjSeg(SelWire);
		PBoxXZ->Invalidate();
	}
	MoveF = 0;
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::PBoxXZMouseMove(TObject *Sender, TShiftState Shift,
	int X, int Y)
{
	AntMX = X;
	AntMY = Y;
	DispCursor();
	if( DownF ){				// マウスボタン押し中
		SetShiftStat(Shift);
		if( Mode ){				// 新規作成
			switch(Mode){
				case 1:
					DrawNot(PBoxXZ, wdisp.X1, wdisp.Z1, wdisp.X2, wdisp.Z2);
					break;
				case 2:
					DrawNotLoop(PBoxXZ, wdisp.X1, wdisp.Z1, wdisp.X2, wdisp.Z2);
					break;
			}
			double	x, y;
			x = double(AntMX - SaveMX)/AntSC;
			y = double(SaveMY - AntMY)/AntSC;
			wdisp.X2 = wsave.X2 + x; wdisp.Z2 = wsave.Z2 + y;
			switch(Mode){
				case 1:
					if( ShiftF ){
						AdjAngle(wdisp.X1, wdisp.Z1, wdisp.X2, wdisp.Z2);
					}
					DrawNot(PBoxXZ, wdisp.X1, wdisp.Z1, wdisp.X2, wdisp.Z2);
					break;
				case 2:
					DrawNotLoop(PBoxXZ, wdisp.X1, wdisp.Z1, wdisp.X2, wdisp.Z2);
					break;
			}
		}
		else if( MoveF ){		// 変更
			if( MoveF >= 10 ){
				DrawNot(PBoxXZ, wdisp.X1, wdisp.Z1, wdisp.X2, wdisp.Z2);
			}
			else {
				if( ABS(AntMX - SaveMX) >= 8 ){
					MoveF += 10;
				}
				else if( ABS(AntMY - SaveMY) >= 8 ){
					MoveF += 10;
				}
			}
			double	x, y;
			x = double(AntMX - SaveMX)/AntSC;
			y = double(SaveMY - AntMY)/AntSC;
			switch(MoveF){
				case 11:			// 始点を移動
					wdisp.X1 = wsave.X1 + x; wdisp.Z1 = wsave.Z1 + y;
					if( ShiftF == 1 ){
						AdjAngle(wdisp.X2, wdisp.Z2, wdisp.X1, wdisp.Z1);
					}
					else if( ShiftF == 2 ){
						AdjPie(wsave.X2, wsave.Z2, wsave.X1, wsave.Z1, wdisp.X1, wdisp.Z1);
					}
					break;
				case 12:			// 終点を移動
					wdisp.X2 = wsave.X2 + x; wdisp.Z2 = wsave.Z2 + y;
					if( ShiftF == 1 ){
						AdjAngle(wdisp.X1, wdisp.Z1, wdisp.X2, wdisp.Z2);
					}
					else if( ShiftF == 2 ){
						AdjPie(wsave.X1, wsave.Z1, wsave.X2, wsave.Z2, wdisp.X2, wdisp.Z2);
					}
					break;
				case 13:			// 全体を移動
					wdisp.X1 = wsave.X1 + x; wdisp.Z1 = wsave.Z1 + y;
					wdisp.X2 = wsave.X2 + x; wdisp.Z2 = wsave.Z2 + y;
					if( ShiftF ){
						if( ABS(x) > ABS(y) ){
							wdisp.Z1 = wsave.Z1;
							wdisp.Z2 = wsave.Z2;
						}
						else {
							wdisp.X1 = wsave.X1;
							wdisp.X2 = wsave.X2;
						}
					}
					break;
			}
			if( MoveF >= 10 ){
				DrawNot(PBoxXZ, wdisp.X1, wdisp.Z1, wdisp.X2, wdisp.Z2);
			}
		}
	}
	else if( Mode ){			// マウスボタン放し中の新規作成
		PBoxXZ->Cursor = crCross;
	}
	else if( SelWire != -1 ){	// マウスボタン離し中
		// ワイヤの検索
		POINT	PP, BP, EP;
		PP.x = X; PP.y = Y;
		BP.x = int((wdef[SelWire].X1 * AntSC)) + ScrXc;
		BP.y = ScrYc - int((wdef[SelWire].Z1 * AntSC));
		EP.x = int((wdef[SelWire].X2 * AntSC)) + ScrXc;
		EP.y = ScrYc - int((wdef[SelWire].Z2 * AntSC));
		int r = IsLine(PP, BP, EP);
		switch(r){
			case 1:
				PBoxXZ->Cursor = crCross;
				break;
			case 2:
				PBoxXZ->Cursor = crCross;
				break;
			case 3:
				PBoxXZ->Cursor = crDrag;
				break;
			default:
				PBoxXZ->Cursor = crArrow;
				break;
		}
	}
	else {
		PBoxXZ->Cursor = crArrow;
	}
	DispWire();
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::PBoxXZClick(TObject *Sender)
{
	if( (MoveF >= 10) || Mode ){
		PBoxXZ->Invalidate();
	}
	else if( SelectWireXZ(AntMX, AntMY) == TRUE ){
		PBoxXZ->Invalidate();
	}
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::PBoxYZMouseDown(TObject *Sender,
	TMouseButton Button, TShiftState Shift, int X, int Y)
{
	AntMX = X;
	AntMY = Y;
	if( Button != mbLeft ) return;
	DownF = 1;
	if( Mode ){
		memcpy(&wsave, &wdef[0], sizeof(wsave));
		if( !wsave.R ) wsave.R = 0.0008;
		wsave.PNo = 1;
		wsave.Y2 = wsave.Y1 = double(AntMX - ScrXc)/AntSC;
		wsave.Z2 = wsave.Z1 = double(ScrYc - AntMY)/AntSC;
		wsave.X1 = wsave.X2 = GetXYZ(0);
		memcpy(&wdisp, &wsave, sizeof(wdisp));
		SaveMX = X;
		SaveMY = Y;
		switch(Mode){
			case 1:
				DrawNot(PBoxYZ, wdisp.Y1, wdisp.Z1, wdisp.Y2, wdisp.Z2);
				break;
			case 2:
				DrawNotLoop(PBoxYZ, wdisp.Y1, wdisp.Z1, wdisp.Y2, wdisp.Z2);
				break;
		}
	}
	else if( SelWire != -1 ){
		POINT	PP, BP, EP;
		PP.x = X; PP.y = Y;
		BP.x = int((wdef[SelWire].Y1 * AntSC)) + ScrXc;
		BP.y = ScrYc - int((wdef[SelWire].Z1 * AntSC));
		EP.x = int((wdef[SelWire].Y2 * AntSC)) + ScrXc;
		EP.y = ScrYc - int((wdef[SelWire].Z2 * AntSC));
		MoveF=IsLine(PP, BP, EP);
		switch(MoveF){
			case 1:
			case 2:
			case 3:
				memcpy(&wsave, &wdef[SelWire], sizeof(wsave));
				memcpy(&wdisp, &wdef[SelWire], sizeof(wdisp));
				SaveMX = X;
				SaveMY = Y;
				break;
			default:
				MoveF = 0;
				break;
		}
	}
	TimerCmd = 1;
	Timer->Enabled = TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::PBoxYZMouseUp(TObject *Sender, TMouseButton Button,
	TShiftState Shift, int X, int Y)
{
	double	xx, yy;
	int		w;

	DownF = 0;
	if( Button != mbLeft ) return;
	if( Mode ){					// 新規ワイヤの登録
		NormalXY(wdisp.Y1, wdisp.Z1);
		NormalXY(wdisp.Y2, wdisp.Z2);
		if( (wdisp.Y1 == wdisp.Y2) && (wdisp.Z1 == wdisp.Z2) ){
			PBoxYZ->Invalidate();
			MoveF = 0;
			return;
		}
		if( Mode == 1 ){
			if( CheckFree(1) == TRUE ){
				if( !IsWire(wdisp) ){
					SaveWire.SetData(SelWire, wmax, wdef);
					wdisp.SEG = 0;
					w = GetNewWire();
					memcpy(&wdef[w], &wdisp, sizeof(wdisp));
					glist[w] = 0;
					AdjSeg(w);
					SelWire = w;
				}
			}
		}
		else {
			if( CheckFree(4) == TRUE ){
				SaveWire.SetData(SelWire, wmax, wdef);
				AdjLoop(wdisp.Y1, wdisp.Z1, wdisp.Y2, wdisp.Z2);
				wdisp.SEG = -1;
				w = GetNewWire();
				memcpy(&wdef[w], &wdisp, sizeof(wdisp));
				wdef[w].Z1 = wdisp.Z2;
				glist[w] = 0;
				AdjSeg(w);
				w = GetNewWire();
				memcpy(&wdef[w], &wdisp, sizeof(wdisp));
				wdef[w].Y2 = wdisp.Y1;
				glist[w] = 0;
				AdjSeg(w);
				w = GetNewWire();
				memcpy(&wdef[w], &wdisp, sizeof(wdisp));
				wdef[w].Z2 = wdisp.Z1;
				glist[w] = 0;
				AdjSeg(w);
				w = GetNewWire();
				memcpy(&wdef[w], &wdisp, sizeof(wdisp));
				wdef[w].Y1 = wdisp.Y2;
				glist[w] = 0;
				AdjSeg(w);
				SelWire = w;
			}
		}
		DispSelWire();
		PBoxYZ->Invalidate();
	}
	else if( MoveF >= 10 ){		// ワイヤの座標変更
		SaveWire.SetData(SelWire, wmax, wdef);
		switch(MoveF){
			case 11:
				if( ShiftF != 2 ){
					NormalXXYY(wdisp.Y2, wdisp.Z2, wdisp.Y1, wdisp.Z1);
				}
				AdjWireChen(wdef, wmax, wsave.X1, wsave.Y1, wsave.Z1, wdisp.X1, wdisp.Y1, wdisp.Z1);
				break;
			case 12:
				if( ShiftF != 2 ){
					NormalXXYY(wdisp.Y1, wdisp.Z1, wdisp.Y2, wdisp.Z2);
				}
				AdjWireChen(wdef, wmax, wsave.X2, wsave.Y2, wsave.Z2, wdisp.X2, wdisp.Y2, wdisp.Z2);
				break;
			case 13:		// 平衡移動
				xx = wsave.Y2 - wsave.Y1;
				yy = wsave.Z2 - wsave.Z1;
				NormalXXYY(wsave.Y1, wsave.Z1, wdisp.Y1, wdisp.Z1);
				wdisp.Y2 = wdisp.Y1 + xx;
				wdisp.Z2 = wdisp.Z1 + yy;
				AdjWireChen(wdef, wmax, wsave.X1, wsave.Y1, wsave.Z1, wdisp.X1, wdisp.Y1, wdisp.Z1);
				AdjWireChen(wdef, wmax, wsave.X2, wsave.Y2, wsave.Z2, wdisp.X2, wdisp.Y2, wdisp.Z2);
				break;
		}
		memcpy(&wdef[SelWire], &wdisp, sizeof(wdisp));
//		AdjSeg(SelWire);
		PBoxYZ->Invalidate();
	}
	MoveF = 0;
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::PBoxYZMouseMove(TObject *Sender, TShiftState Shift,
	int X, int Y)
{
	AntMX = X;
	AntMY = Y;
	DispCursor();
	if( DownF ){				// マウスボタン押し中
		SetShiftStat(Shift);
		if( Mode ){				// 新規作成
			switch(Mode){
				case 1:
					DrawNot(PBoxYZ, wdisp.Y1, wdisp.Z1, wdisp.Y2, wdisp.Z2);
					break;
				case 2:
					DrawNotLoop(PBoxYZ, wdisp.Y1, wdisp.Z1, wdisp.Y2, wdisp.Z2);
					break;
			}
			double	x, y;
			x = double(AntMX - SaveMX)/AntSC;
			y = double(SaveMY - AntMY)/AntSC;
			wdisp.Y2 = wsave.Y2 + x; wdisp.Z2 = wsave.Z2 + y;
			switch(Mode){
				case 1:
					if( ShiftF ){
						AdjAngle(wdisp.Y1, wdisp.Z1, wdisp.Y2, wdisp.Z2);
					}
					DrawNot(PBoxYZ, wdisp.Y1, wdisp.Z1, wdisp.Y2, wdisp.Z2);
					break;
				case 2:
					DrawNotLoop(PBoxYZ, wdisp.Y1, wdisp.Z1, wdisp.Y2, wdisp.Z2);
					break;
			}
		}
		else if( MoveF ){		// 変更
			if( MoveF >= 10 ){
				DrawNot(PBoxYZ, wdisp.Y1, wdisp.Z1, wdisp.Y2, wdisp.Z2);
			}
			else {
				if( ABS(AntMX - SaveMX) >= 8 ){
					MoveF += 10;
				}
				else if( ABS(AntMY - SaveMY) >= 8 ){
					MoveF += 10;
				}
			}
			double	x, y;
			x = double(AntMX - SaveMX)/AntSC;
			y = double(SaveMY - AntMY)/AntSC;
			switch(MoveF){
				case 11:			// 始点を移動
					wdisp.Y1 = wsave.Y1 + x; wdisp.Z1 = wsave.Z1 + y;
					if( ShiftF == 1 ){
						AdjAngle(wdisp.Y2, wdisp.Z2, wdisp.Y1, wdisp.Z1);
					}
					else if( ShiftF == 2 ){
						AdjPie(wsave.Y2, wsave.Z2, wsave.Y1, wsave.Z1, wdisp.Y1, wdisp.Z1);
					}
					break;
				case 12:			// 終点を移動
					wdisp.Y2 = wsave.Y2 + x; wdisp.Z2 = wsave.Z2 + y;
					if( ShiftF == 1 ){
						AdjAngle(wdisp.Y1, wdisp.Z1, wdisp.Y2, wdisp.Z2);
					}
					else if( ShiftF == 2 ){
						AdjPie(wsave.Y1, wsave.Z1, wsave.Y2, wsave.Z2, wdisp.Y2, wdisp.Z2);
					}
					break;
				case 13:			// 全体を移動
					wdisp.Y1 = wsave.Y1 + x; wdisp.Z1 = wsave.Z1 + y;
					wdisp.Y2 = wsave.Y2 + x; wdisp.Z2 = wsave.Z2 + y;
					if( ShiftF ){
						if( ABS(x) > ABS(y) ){
							wdisp.Z1 = wsave.Z1;
							wdisp.Z2 = wsave.Z2;
						}
						else {
							wdisp.Y1 = wsave.Y1;
							wdisp.Y2 = wsave.Y2;
						}
					}
					break;
			}
			if( MoveF >= 10 ){
				DrawNot(PBoxYZ, wdisp.Y1, wdisp.Z1, wdisp.Y2, wdisp.Z2);
			}
		}
	}
	else if( Mode ){			// マウスボタン放し中の新規作成
		PBoxYZ->Cursor = crCross;
	}
	else if( SelWire != -1 ){	// マウスボタン離し中
		// ワイヤの検索
		POINT	PP, BP, EP;
		PP.x = X; PP.y = Y;
		BP.x = int((wdef[SelWire].Y1 * AntSC)) + ScrXc;
		BP.y = ScrYc - int((wdef[SelWire].Z1 * AntSC));
		EP.x = int((wdef[SelWire].Y2 * AntSC)) + ScrXc;
		EP.y = ScrYc - int((wdef[SelWire].Z2 * AntSC));
		int r = IsLine(PP, BP, EP);
		switch(r){
			case 1:
				PBoxYZ->Cursor = crCross;
				break;
			case 2:
				PBoxYZ->Cursor = crCross;
				break;
			case 3:
				PBoxYZ->Cursor = crDrag;
				break;
			default:
				PBoxYZ->Cursor = crArrow;
				break;
		}
	}
	else {
		PBoxYZ->Cursor = crArrow;
	}
	DispWire();
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::PBoxYZClick(TObject *Sender)
{
	if( (MoveF >= 10) || Mode ){
		PBoxYZ->Invalidate();
	}
	else if( SelectWireYZ(AntMX, AntMY) == TRUE ){
		PBoxYZ->Invalidate();
	}
}
//---------------------------------------------------------------------------
// 編集
void __fastcall TWireCadDlg::K11Click(TObject *Sender)
{
	if( SelWire == -1 ) return;

	TWireEditDlg *Box = new TWireEditDlg(this);

	CSaveWire	sw;
	sw.SetData(SelWire, wmax, wdef);
	if( Box->Execute(wdef, SelWire, wmax) == TRUE ){
		SaveWire.Align(sw);
		DispSelWire();
		GetPB()->Invalidate();
	}
	delete Box;
}
//---------------------------------------------------------------------------
// 画面状態の記憶
void __fastcall TWireCadDlg::MemoryView(int n)
{
	CViewPos *vp = &vpos[n];
	vp->Flag = 1;
	vp->Xc = AntXc;
	vp->Yc = AntYc;
	vp->Gf = ChkGrid->Checked;
	vp->Gw = Grid;
	vp->Gx = GridX;
	vp->Gy = GridY;
	vp->Sel = SelWire;
	vp->Pos = TBSC->Position;
	vp->Deg = TBDEG->Position;
	vp->ZDeg = TBZDEG->Position;
	vp->Mode = Mode;
	vp->Filter = Filter;
	if( Page->ActivePage == TabAll ){
		vp->Page = 0;
	}
	else if( Page->ActivePage == TabXY ){
		vp->Page = 1;
	}
	else if( Page->ActivePage == TabXZ ){
		vp->Page = 2;
	}
	else if( Page->ActivePage == TabYZ ){
		vp->Page = 3;
	}
}
//---------------------------------------------------------------------------
// 画面状態の呼び出し
void __fastcall TWireCadDlg::RestView(int n)
{
	CViewPos *vp = &vpos[n];
	if( !vp->Flag ) return;
	EnbSetup = 0;
	AntXc = vp->Xc;
	AntYc = vp->Yc;
	ChkGrid->Checked = FALSE;
	Grid = vp->Gw;
	GridX = vp->Gx;
	GridY = vp->Gy;
	GridWidth->Text = StrDbl(Grid);
	SelWire = vp->Sel;
	if( SelWire != -1 ){
		if( (SelWire >= wmax) || (!wdef[SelWire].PNo) ){
			SelWire = -1;
		}
	}
	TBSC->Position = vp->Pos;
	TBDEG->Position = vp->Deg;
	TBZDEG->Position = vp->ZDeg;
	AntDeg = double(vp->Deg);
	AntDeg *= (PAI / 180.0);
	AntZDeg = double(vp->ZDeg);
	AntZDeg *= (PAI / 180.0);
	switch(vp->Page){
		case 0:
			Page->ActivePage = TabAll;
			break;
		case 1:
			Page->ActivePage = TabXY;
			break;
		case 2:
			Page->ActivePage = TabXZ;
			break;
		case 3:
			Page->ActivePage = TabYZ;
			break;
	}
	UpdateView();
	Mode = vp->Mode;
	switch(Mode){
		case 0:
			SBSel->Down = TRUE;
			break;
		case 1:
			SBLine->Down = TRUE;
			break;
		case 2:
			SBLoop->Down = TRUE;
			break;
	}
	TPaintBox *tp = GetPB();
	AntSC = double(TBSC->Position) / 20.0;
	AntSC = AntSC * AntSC * AntSC * AntSC;
	ScrXc = int(tp->Width/2 + (AntXc * AntSC));
	ScrYc = int(tp->Height/2 + (AntYc * AntSC));
	ChkGrid->Checked = vp->Gf;
	Filter = vp->Filter;
	SelFilter->Text = FilterTable[Filter];
	EntryGroup();
	DispSelWire();
	tp->Invalidate();
	SetupSB();
	EnbSetup = 1;
}
//---------------------------------------------------------------------------
// 画面Ａに記憶
void __fastcall TWireCadDlg::K17Click(TObject *Sender)
{
	MemoryView(0);
}
//---------------------------------------------------------------------------
// 画面Ｂに記憶
void __fastcall TWireCadDlg::K18Click(TObject *Sender)
{
	MemoryView(1);
}
//---------------------------------------------------------------------------
// 画面Ｃに記憶
void __fastcall TWireCadDlg::K19Click(TObject *Sender)
{
	MemoryView(2);
}
//---------------------------------------------------------------------------
// 画面Ａを呼び出し
void __fastcall TWireCadDlg::K13Click(TObject *Sender)
{
	RestView(0);
}
//---------------------------------------------------------------------------
// 画面Ｂを呼び出し
void __fastcall TWireCadDlg::K14Click(TObject *Sender)
{
	RestView(1);
}
//---------------------------------------------------------------------------
// 画面Ｃを呼び出し
void __fastcall TWireCadDlg::K15Click(TObject *Sender)
{
	RestView(2);
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::SetupSB(void)
{
	int enb = EnbSetup;
	EnbSetup = 0;
	HSBXc = AntXc;
	VSBYc = AntYc;
	SBSC = 25.0/AntSC;
	HSBAll->Position = 50;
	HSBXY->Position = 50;
	HSBXZ->Position = 50;
	HSBYZ->Position = 50;
	VSBAll->Position = 50;
	VSBXY->Position = 50;
	VSBXZ->Position = 50;
	VSBYZ->Position = 50;
	EnbSetup = enb;
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::Scroll(void)
{
	TPaintBox *tp = GetPB();
	ScrXc = int(tp->Width/2 + (AntXc * AntSC));
	ScrYc = int(tp->Height/2 + (AntYc * AntSC));
	ScrollF = 5;
	if( Timer->Enabled != TRUE ){
		if( TimerCmd < 2 ){
			TimerCmd = 2;
		}
		Timer->Enabled = TRUE;
	}
	tp->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::ScrollEdge(TScrollBar *sp)
{
	int d = sp->Position;
	if( (d != 0) && (d != 100) ){
		TimerCmd = 2;
		return;
	}
	if( sp->Kind == sbHorizontal ){		// H
		TimerCmd = (d == 0) ? 5 : 6;
	}
	else {								// V
		TimerCmd = (d == 0) ? 3 : 4;
	}
	if( Timer->Enabled != TRUE ){
		Timer->Enabled = TRUE;
	}
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::HSBAllChange(TObject *Sender)
{
	if( !EnbSetup ) return;
	ScrollEdge(HSBAll);

	double d = 50.0 - double(HSBAll->Position);

	AntXc = HSBXc + (d * SBSC);
	Scroll();
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::VSBAllChange(TObject *Sender)
{
	if( !EnbSetup ) return;
	ScrollEdge(VSBAll);

	double d = 50.0 - double(VSBAll->Position);

	AntYc = VSBYc + (d * SBSC);
	Scroll();
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::HSBXYChange(TObject *Sender)
{
	if( !EnbSetup ) return;
	ScrollEdge(HSBXY);

	double d = 50.0 - double(HSBXY->Position);

	AntXc = HSBXc + (d * SBSC);
	Scroll();
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::VSBXYChange(TObject *Sender)
{
	if( !EnbSetup ) return;
	ScrollEdge(VSBXY);

	double d = 50.0 - double(VSBXY->Position);

	AntYc = VSBYc + (d * SBSC);
	Scroll();
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::HSBXZChange(TObject *Sender)
{
	if( !EnbSetup ) return;
	ScrollEdge(HSBXZ);

	double d = 50.0 - double(HSBXZ->Position);

	AntXc = HSBXc + (d * SBSC);
	Scroll();
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::VSBXZChange(TObject *Sender)
{
	if( !EnbSetup ) return;
	ScrollEdge(VSBXZ);

	double d = 50.0 - double(VSBXZ->Position);

	AntYc = VSBYc + (d * SBSC);
	Scroll();
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::HSBYZChange(TObject *Sender)
{
	if( !EnbSetup ) return;
	ScrollEdge(HSBYZ);

	double d = 50.0 - double(HSBYZ->Position);

	AntXc = HSBXc + (d * SBSC);
	Scroll();
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::VSBYZChange(TObject *Sender)
{
	if( !EnbSetup ) return;
	ScrollEdge(VSBYZ);

	double d = 50.0 - double(VSBYZ->Position);

	AntYc = VSBYc + (d * SBSC);
	Scroll();
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::HSBAllScroll(TObject *Sender,
	TScrollCode ScrollCode, int &ScrollPos)
{
	if( ScrollCode == scEndScroll ){
		ScrollF = 0;
		TimerCmd = 0;
		Timer->Enabled = FALSE;
		GetPB()->Invalidate();
	}
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::KU1Click(TObject *Sender)
{
	if( SaveWire.IsSwap() ){
		SaveWire.SwapData(SelWire, wmax, wdef);
		DispSelWire();
		GetPB()->Invalidate();
	}
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::KS1Click(TObject *Sender)
{
	if( wmax && (SelWire != -1) ){
		SaveWire.SetData(SelWire, wmax, wdef);
		double X = wdef[SelWire].X1;
		double Y = wdef[SelWire].Y1;
		double Z = wdef[SelWire].Z1;
		wdef[SelWire].X1 = wdef[SelWire].X2;
		wdef[SelWire].Y1 = wdef[SelWire].Y2;
		wdef[SelWire].Z1 = wdef[SelWire].Z2;
		wdef[SelWire].X2 = X;
		wdef[SelWire].Y2 = Y;
		wdef[SelWire].Z2 = Z;
		DispSelWire();
		GetPB()->Invalidate();
	}
}
//---------------------------------------------------------------------------
void __fastcall TWireCadDlg::K9Click(TObject *Sender)
{
	exeenv.Ant3D = exeenv.Ant3D ? 0 : 1;
	TBZDEG->Enabled = exeenv.Ant3D ? TRUE : FALSE;
	K9->Checked = exeenv.Ant3D;
	if( Page->ActivePage == TabAll ){
		SBViewClick(NULL);
	}
}
//---------------------------------------------------------------------------
