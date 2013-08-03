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
#include <Printers.hpp>
#pragma hdrstop

#include "BwDisp.h"
#include "Main.h"
#include "MiniNec3.h"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
#define MATCHINFO	"マッチング: %s"
//TBwDispDlg *BwDispDlg;
//---------------------------------------------------------------------
__fastcall TBwDispDlg::TBwDispDlg(TComponent* AOwner)
	: TForm(AOwner)
{
	pColorTable = NULL;
	CalcF = FALSE;
	AntL = AntC = AntQ = AntW = AntF = 0.0;
	SaveFQ = 14.05;
	GZ.Init(PBoxZ);
	GS.Init(PBoxS);
	GG.Init(PBoxG);
	for( int i = 0; FreqTbl[i] != NULL; i++ ){
		EditCenter->Items->Add(FreqTbl[i]);
	}
	EnbSel = 0;
	FirstUD = FirstUDM = 0;
	DrawPtnH.SetRect(PBPtn->Canvas, 0, 0, PBPtn->Width / 2 - 2, PBPtn->Height);
	DrawPtnV.SetRect(PBPtn->Canvas, (PBPtn->Width/2) + 2, 0, PBPtn->Width, PBPtn->Height);
}
//---------------------------------------------------------------------------
void __fastcall TBwDispDlg::UpdateView(int bwflag)
{
	EnbSel = 0;
	int ff = TRUE;
	if( Page->ActivePage == TabSet ) ff = FALSE;
	if( bwflag != FALSE ){
		char	bf[32];
		sprintf(bf, "%u", int((bwct.fw*1000.0) + 0.5));
		BwSel->Text = bf;
	}
	int f = FALSE;
	if( bwct.bcnt != BWMAX ){
		TryBtn->Enabled = ff;
	}
	else {
		TryBtn->Enabled = FALSE;
		f = TRUE;
	}
	if( res.Bwc.bcnt == BWMAX ){
		MeasBtn->Enabled = FALSE;
		f = TRUE;
	}
	else {
		MeasBtn->Enabled = ff;
	}
	if( (bwct.bcnt < 2)|| (ABS(AntF - res.Bwc.fo) >= (res.Bwc.fo * 0.001)) ){
		GetFoBtn->Enabled = ff;
	}
	else {
		GetFoBtn->Enabled = FALSE;
	}
	if( Page->ActivePage == TabPtn ) f = FALSE;
	PrintBtn->Enabled = f;

	f = SelMatch->ItemIndex ? TRUE : FALSE;
	MatchCenter->Enabled = f;
	MatchFreq->Enabled = f && !MatchCenter->Checked;
	UDMatch->Enabled = f && !MatchCenter->Checked;

	HalfBtn->Enabled = bwct.bwsfull ? FALSE : ff;
	PBoxZ->Invalidate();
	PBoxS->Invalidate();
	PBoxG->Invalidate();
	PBPtn->Invalidate();
	Grid->Invalidate();
	EnbSel = 1;
}
//---------------------------------------------------------------------------
// マッチングデータのアップデート
void __fastcall TBwDispDlg::UpdateMatch(void)
{
	double	FQ;

	if( SelMatch->ItemIndex ){
		if( MatchCenter->Checked ){
			FQ = bwct.bw[BWMAX/2].FQ;
		}
		else {
			Calc(FQ, AnsiString(MatchFreq->Text).c_str());
		}
		if( FQ != MatchData.FQ ){
			MatchData.Flag = 0;
			MatchData.FQ = FQ;
		}
		if( !MatchData.Flag ){
			BWS *bp = res.SearchBW(&res.Bwc, MatchData.FQ);
			if( bp != NULL ){
				memcpy(&MatchData, bp, sizeof(MatchData));
			}
			if( !MatchData.Flag ){
				CalcF = TRUE;
				CWaitCursor	tw;
				exeenv.CalcLog = 0;
				ap->cfq = MatchData.FQ;
				if( MainWnd->SetStackAnt() == TRUE ){
					exeenv.CalcF = 1;
					CalCurrent();
					exeenv.CalcF = 0;
					RESL *rp = res.GetResP();
					MatchData.Flag = 1;
					MatchData.R = rp->R;
					MatchData.JX = rp->JX;
					MatchData.GA = rp->GAIN;
					MatchData.FB = rp->FB;
					memcpy(&res.Bwc.MatchData, &MatchData, sizeof(MatchData));
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
// 推測計算を行う
int __fastcall TBwDispDlg::CalcSim(void)
{
	res.UpdateCountBW();
	memcpy(&bwct, &res.Bwc, sizeof(BWC));
	if( bwct.bcnt < 2 ) return FALSE;
	int i, n, nn;
	double jx = MAXDOUBLE;
	for( n = i = 0; i < BWMAX; i++ ){			// JXが一番小さい点を得る
		if( bwct.bw[i].Flag ){
			if( jx > ABS(bwct.bw[i].JX) ){
				jx = ABS(bwct.bw[i].JX);
				n = i;
			}
		}
	}
	jx = MAXDOUBLE;
	for( nn = i = 0; i < BWMAX; i++ ){			// JXが二番目に小さい点を得る
		if( (n != i) && bwct.bw[i].Flag ){
			if( jx > ABS(bwct.bw[i].JX) ){
				jx = ABS(bwct.bw[i].JX);
				nn = i;
			}
		}
	}
	double ra = bwct.bw[n].R;
	double fa = bwct.bw[n].FQ * 1e6;
	double xa = bwct.bw[n].JX;
	double ga = bwct.bw[n].GA;
	double fba = bwct.bw[n].FB;
	double rb = bwct.bw[nn].R;
	double fb = bwct.bw[nn].FQ * 1e6;
	double xb = bwct.bw[nn].JX;
	double gb = bwct.bw[nn].GA;
	double fbb = bwct.bw[nn].FB;
	double L = ((fa * xa) - (fb * xb))/((fa*fa - fb*fb)*2*PAI);
	double x = (2*PAI*fa*L - xa);
	if( x == 0.0 ) x = NULLV;
	double C = 1/(2*PAI*fa*x);
	AntL = ABS(L * 1e6);
	AntC = ABS(C * 1e12);
	if( AntC == 0.0 ) AntC = NULLV;
	if( ra && C ){
		AntQ = sqrt(ABS(L/C))/ra;
	}
	else {
		AntQ = 99999.9;
	}
	if( C && L ){
		AntF = 1E-6/(2 * PAI * sqrt(ABS(L*C)));
	}
	else {
		AntF = 99999.9;
	}
	AntW = AntF*1e3/(AntQ ? AntQ : 1.0);
	BW *bp = bwct.bw;
	for( i = 0; i < BWMAX; i++, bp++ ){
		if( !bp->Flag ){
			bp->R = ra + (rb - ra)*(bp->FQ * 1e6 - fa)/(fb - fa);
			if( bp->R <= 1e-30 ) bp->R = 1e-30;
			bp->JX = 2*PAI*bp->FQ*1e6*L - (1/(2*PAI*bp->FQ*1e6*C));
			bp->GA = ga + (gb - ga)*(bp->FQ*1e6 - fa)/(fb - fa);
			bp->FB = fba + (fbb - fba)*(bp->FQ*1e6 - fa)/(fb - fa);
		}
	}
	UpdateMatch();
	bwct.bcnt = BWMAX;
	return TRUE;
}
//---------------------------------------------------------------------------
int __fastcall TBwDispDlg::Execute(ANTDEF *p, TColor *pc)
{
	pColorTable = pc;
	LimitSWRChange(NULL);
	UDFreq->Associate = EditCenter;
	UDMatch->Associate = MatchFreq;
	SaveRes.SetRes(&res);
	ap = p;
	SaveFQ = ap->cfq;
	SelMatch->ItemIndex = exeenv.BwMatch;
	UDFreq->Associate = NULL;
	UDFreq->Position = 50;
	EditCenter->Text = StrDbl(res.Bwc.fo);
	UDMatch->Associate = NULL;
	UDMatch->Position = 50;
	MatchFreq->Text = StrDbl(res.Bwc.MatchData.FQ);
	MatchCenter->Checked = res.Bwc.MatchCenter;
	HalfAddCnt->Text = res.Bwc.bwsdim;
	CalcSim();
	UpdateView(TRUE);
	ShowModal();
	double fq;
	Calc(fq, AnsiString(MatchFreq->Text).c_str());
	if( res.Bwc.MatchData.FQ != fq ){
		res.Bwc.MatchData.FQ = fq;
		res.Bwc.MatchData.Flag = 0;
	}
	res.Bwc.MatchCenter = MatchCenter->Checked;
	if( CalcF == TRUE ){
		ap->cfq = SaveFQ;
		if( ap->cfq && (MainWnd->SetStackAnt() == TRUE) ){
			res.IncResP();
			res.SetRes(&SaveRes);
			MainWnd->Grid1->Invalidate();
		}
	}
	return CalcF;
}
//---------------------------------------------------------------------
// 推測
void __fastcall TBwDispDlg::TryBtnClick(TObject *Sender)
{
	CWaitCursor tw;
	exeenv.CalcLog = 0;
	int N;
	if( !res.Bwc.bw[res.Bwc.bo].Flag ){
		N = res.Bwc.bo;
		ap->cfq = res.Bwc.bw[N].FQ;
		if( MainWnd->SetStackAnt() == TRUE ){
			res.IncResP();
			res.GetResP()->TYPE = env.type;
			res.GetResP()->HEIGHT = env.antheight;
			res.GetResP()->FBR = env.fbr;
			MainWnd->Grid1->RowCount = res.GetCnt() + (res.GetCnt() ? 1 : 2);
			exeenv.CalcF = 1;
			CalCurrent();
			exeenv.CalcF = 0;
			res.SetCalc(ap->Name);		// 計算完了フラグ
			MainWnd->Grid1->Invalidate();
			res.AddBWC(N);
		}
	}
	if( res.Bwc.bw[res.Bwc.bo].JX >= 0.0 ){		// インダクティブの時
		N = res.Bwc.bo - 1;
	}
	else {										// キャパシティブの時
		N = res.Bwc.bo + 1;
	}
	ap->cfq = res.Bwc.bw[N].FQ;
	if( MainWnd->SetStackAnt() == TRUE ){
		res.IncResP();
		res.GetResP()->TYPE = env.type;
		res.GetResP()->HEIGHT = env.antheight;
		res.GetResP()->FBR = env.fbr;
		MainWnd->Grid1->RowCount = res.GetCnt() + (res.GetCnt() ? 1 : 2);
		exeenv.CalcF = 1;
		CalCurrent();
		exeenv.CalcF = 0;
		res.SetCalc(ap->Name);		// 計算完了フラグ
		MainWnd->Grid1->Invalidate();
		res.AddBWC(N);
		CalcSim();
		UpdateView(TRUE);
	}
	CalcF = TRUE;
}
//---------------------------------------------------------------------------
// 全点計算
void __fastcall TBwDispDlg::MeasBtnClick(TObject *Sender)
{
	CalcF = TRUE;
	CWaitCursor	tw;
	exeenv.CalcLog = 0;
	int i;
	for( i = 0; i < BWMAX; i++ ){
		if( res.Bwc.bw[i].Flag != 1 ){
			ap->cfq = res.Bwc.bw[i].FQ;
			if( ap->cfq && (MainWnd->SetStackAnt() == TRUE) ){
				res.IncResP();
				res.GetResP()->TYPE = env.type;
				res.GetResP()->HEIGHT = env.antheight;
				res.GetResP()->FBR = env.fbr;
				MainWnd->Grid1->RowCount = res.GetCnt() + (res.GetCnt() ? 1 : 2);
				exeenv.CalcF = 1;
				CalCurrent();
				exeenv.CalcF = 0;
				res.SetCalc(ap->Name);		// 計算完了フラグ
				res.AddBWC(i);
			}
		}
	}
	CalcSim();
	UpdateView(TRUE);
}
//---------------------------------------------------------------------------
void __fastcall TBwDispDlg::HalfBtnClick(TObject *Sender)
{
	if( res.Bwc.bcnt < BWMAX ) MeasBtnClick(NULL);
	CalcF = TRUE;
	CWaitCursor tw;
	exeenv.CalcLog = 0;
	int i, j;
	for( i = 0; i < BWSMAX; i++ ){
		for( j = 0; j < res.Bwc.bwsdim; j++ ){
			if( !res.Bwc.bws[i][j].Flag ){
				ap->cfq = res.Bwc.bws[i][j].FQ;
				if( ap->cfq && (MainWnd->SetStackAnt() == TRUE) ){
					res.GetResP()->TYPE = env.type;
					res.GetResP()->HEIGHT = env.antheight;
					res.GetResP()->FBR = env.fbr;
					MainWnd->Grid1->RowCount = res.GetCnt() + (res.GetCnt() ? 1 : 2);
					exeenv.CalcF = 1;
					CalCurrent();
					exeenv.CalcF = 0;
					res.SetCalc(ap->Name);		// 計算完了フラグ
					res.AddBWS(i, j);
				}
			}
		}
	}
	CalcSim();
	UpdateView(TRUE);
}
//---------------------------------------------------------------------------
void __fastcall TBwDispDlg::PBoxZPaint(TObject *Sender)
{
	GZ.ClearView(PBoxZ);
	GZ.DrawAngle();
	GZ.DrawTitle(0, "R", clBlack);
	GZ.DrawTitle(1, "jx", clRed);

	GZ.Clear();
	int i, j;
	double r, jx;
	for( i = 0; i < BWMAX; i++ ){
		r = bwct.bw[i].R;
		jx = bwct.bw[i].JX;
		CalcZ(bwct.bw[i].FQ, r, jx);
		GZ.AddPoint(bwct.bw[i].FQ, r, bwct.bw[i].Flag);
		if( bwct.bwsfull && (i < BWSMAX) ){
			for( j = 0; j < bwct.bwsdim; j++ ){
				r = bwct.bws[i][j].R;
				jx = bwct.bws[i][j].JX;
				CalcZ(bwct.bws[i][j].FQ, r, jx);
				GZ.AddPoint(bwct.bws[i][j].FQ, r, bwct.bws[i][j].Flag);
			}
		}
	}
	GZ.AutoRenge(0.0);
	GZ.DrawXGage();
	if( bwct.bcnt != BWMAX ) return;
	GZ.DrawGage(0, clBlack);
	GZ.Plot(clBlack);
	GZ.DrawConst(AntL, AntC, AntQ, AntW, AntF);
	if( exeenv.BwMatch == 2 ){
		char bf[128];
		sprintf(bf, MATCHINFO, StrDbl(MatchData.FQ));
		GZ.DrawInfo(bf);
	}

	GZ.Clear();
	for( i = 0; i < BWMAX; i++ ){
		r = bwct.bw[i].R;
		jx = bwct.bw[i].JX;
		CalcZ(bwct.bw[i].FQ, r, jx);
		GZ.AddPoint(bwct.bw[i].FQ, jx, bwct.bw[i].Flag);
		if( bwct.bwsfull && (i < BWSMAX) ){
			for( j = 0; j < bwct.bwsdim; j++ ){
				r = bwct.bws[i][j].R;
				jx = bwct.bws[i][j].JX;
				CalcZ(bwct.bws[i][j].FQ, r, jx);
				GZ.AddPoint(bwct.bws[i][j].FQ, jx, bwct.bws[i][j].Flag);
			}
		}
	}
	GZ.AutoRenge(-MAXDOUBLE);
	GZ.AdjRengeZero();
	GZ.DrawGage(1, clRed);
	GZ.Plot(clRed);
}
//---------------------------------------------------------------------------
void __fastcall TBwDispDlg::PBoxSPaint(TObject *Sender)
{
	GS.ClearView(PBoxS);
	GS.DrawAngle();
	GS.DrawTitle(0, "SWR", clBlack);

	GS.Clear();
	GS.SetYLimit(SWRLimit);
	int i, j;
	double s;
	for( i = 0; i < BWMAX; i++ ){
		s = CalcSWR(bwct.bw[i].FQ, bwct.bw[i].R, bwct.bw[i].JX);
		slst[i].SWR = s;
		slst[i].FQ = bwct.bw[i].FQ;
		if( s > 1e4 ) s = 1e4;
		GS.AddPoint(bwct.bw[i].FQ, s, bwct.bw[i].Flag);
		if( bwct.bwsfull && (i < BWSMAX) ){
			for( j = 0; j < bwct.bwsdim; j++ ){
				s = CalcSWR(bwct.bws[i][j].FQ, bwct.bws[i][j].R, bwct.bws[i][j].JX);
				if( s > 1e4 ) s = 1e4;
				GS.AddPoint(bwct.bws[i][j].FQ, s, bwct.bws[i][j].Flag);
			}
		}
	}
	GS.AutoRenge(1.0);
	GS.DrawXGage();
	if( bwct.bcnt != BWMAX ) return;
	GS.DrawGage(0, clBlack);
	GS.Plot(clBlack);
	char bf[128];
	if( exeenv.BwMatch ){
		sprintf(bf, MATCHINFO, StrDbl(MatchData.FQ));
		GS.DrawInfo(bf);
	}
	else {
		sprintf(bf, "SWR基準Z: %s", StrCLXZ(env.RO, env.JXO));
		GS.DrawInfo(bf);
	}
	if( CheckSWRDisp()==TRUE ){
		GS.DrawInfoL(0, "⇔%.1lf KHz(SWR<1.5)", GetSWRWidth(1.5));
		GS.DrawInfoL(1, "⇔%.1lf KHz(SWR<2.0)", GetSWRWidth(2.0));
	}
}
//---------------------------------------------------------------------------
void __fastcall TBwDispDlg::PBoxGPaint(TObject *Sender)
{
	GG.ClearView(PBoxG);
	GG.DrawAngle();
	GG.DrawTitle(0, "GA", clBlack);
	GG.DrawTitle(1, "FB", clRed);

	GG.Clear();
	int i, j;
	for( i = 0; i < BWMAX; i++ ){
		GG.AddPoint(bwct.bw[i].FQ, bwct.bw[i].GA, bwct.bw[i].Flag);
		if( bwct.bwsfull && (i < BWSMAX) ){
			for( j = 0; j < bwct.bwsdim; j++ ){
				GG.AddPoint(bwct.bws[i][j].FQ, bwct.bws[i][j].GA, bwct.bws[i][j].Flag);
			}
		}
	}
	GG.AutoRenge(-MAXDOUBLE);
	GG.DrawXGage();
	if( bwct.bcnt != BWMAX ) return;
	GG.DrawGage(0, clBlack);
	GG.Plot(clBlack);

	GG.Clear();
	for( i = 0; i < BWMAX; i++ ){
		GG.AddPoint(bwct.bw[i].FQ, bwct.bw[i].FB, bwct.bw[i].Flag);
		if( bwct.bwsfull && (i < BWSMAX) ){
			for( j = 0; j < bwct.bwsdim; j++ ){
				GG.AddPoint(bwct.bws[i][j].FQ, bwct.bws[i][j].FB, bwct.bws[i][j].Flag);
			}
		}
	}
	GG.AutoRenge(-MAXDOUBLE);
	GG.DrawGage(1, clRed);
	GG.Plot(clRed);
}
//---------------------------------------------------------------------------
void __fastcall TBwDispDlg::BwSelChange(TObject *Sender)
{
	if( !EnbSel ) return;

	int dw;
	double ddw;
	if( CalcU(dw, AnsiString(BwSel->Text).c_str()) == TRUE ){
		ddw = double(dw)/1000.0;
		if( (dw >= 4) && (ddw != res.Bwc.fw) ){
			res.Bwc.fw = ddw;
			res.Bwc.bi = res.Bwc.fw / (BWMAX-1);
			double f = res.Bwc.fo - (res.Bwc.bi*2);
			int i, j;
			for( i = 0; i < BWMAX; i++, f += res.Bwc.bi ){
				if( RoundUp(f, 1000) != RoundUp(res.Bwc.bw[i].FQ, 1000) ){
					res.Bwc.bw[i].Flag = 0;
					res.Bwc.bw[i].FQ = RoundUp(f, 1000);
				}
			}
			res.SetBWSDim(res.Bwc.bwsdim);				// BWS周波数の設定
			BWS *bp;
			for( i = 0; i < BWMAX; i++ ){
				if( !res.Bwc.bw[i].Flag ){			// 基本データのコピー
					bp = res.SearchBW(&bwct, res.Bwc.bw[i].FQ);
					if( bp != NULL ){
						if( ((BW *)bp >= &bwct.bw[0]) && ((BW *)bp < &bwct.bw[BWMAX]) ){
							memcpy(&res.Bwc.bw[i], bp, sizeof(BW));
						}
						else {
							memcpy(&res.Bwc.bw[i], bp, sizeof(BWS));
							res.Bwc.bw[i].Flag = 2;
						}
					}
				}
				if( i < BWSMAX ){					// 詳細データのコピー
					for( j = 0; j < res.Bwc.bwsdim; j++ ){
						if( !res.Bwc.bws[i][j].Flag ){
							bp = res.SearchBW(&bwct, res.Bwc.bws[i][j].FQ);
							if( bp != NULL ){
								memcpy(&res.Bwc.bws[i][j], bp, sizeof(BWS));
							}
						}
					}
				}
			}
			CalcSim();
			UpdateView(FALSE);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TBwDispDlg::PrintBtnClick(TObject *Sender)
{
	if( MainWnd->PrintDialog->Execute() == TRUE ){
		char bf[128];
		TCanvas *cp = Printer()->Canvas;
		CWaitCursor tw;
		CGraph	gZ;
		CGraph	gS;
		CGraph	gG;
		Printer()->BeginDoc();
		cp->Font->Name = "ＭＳ ゴシック";
		TRect rc = Printer()->Canvas->ClipRect;
		int pw = rc.Right - rc.Left;
		int ph = rc.Bottom - rc.Top;
		cp->Font->Height = sqrt(pw * ph)/60;
		pw = (pw*19)/20;
		ph = (ph*19)/20;
		int xf = pw/40;
		int yf = ph/40;
		int fw = cp->TextWidth("A");
		int fh = cp->TextHeight("A");
		if( pw >= ph ){			// 横長の時
			cp->TextOut(xf+fw*6, yf+fh, VERSTR);
			cp->TextOut(xf+fw*6, yf+fh*3, MainWnd->AntName3->Caption);
			gZ.Init(cp, xf+pw/2, yf+0, xf+pw, yf+ph/2);
			gS.Init(cp, xf, yf+ph/2, xf+pw/2, yf+ph);
			gG.Init(cp, xf+pw/2, yf+ph/2, xf+pw, yf+ph);
		}
		else {					// 縦長の時
			cp->TextOut(xf+fw*6, yf+fh, VERSTR);
			cp->TextOut(xf+fw*6, yf+fh*2, MainWnd->AntName3->Caption);
			int y = yf+fh*2;
			ph -= y;
			gZ.Init(cp, xf, y, xf+pw, y+ph/3);
			gS.Init(cp, xf, y+ph/3, xf+pw, y+ph*2/3);
			gG.Init(cp, xf, y+ph*2/3, xf+pw, y+ph);
		}
		gZ.DrawAngle();
		gZ.DrawTitle(0, "R", clBlack);
		gZ.DrawTitle(1, "jx", clBlack);

		gZ.Clear();
		int i, j;
		double r, jx;
		for( i = 0; i < BWMAX; i++ ){
			r = bwct.bw[i].R;
			jx = bwct.bw[i].JX;
			CalcZ(bwct.bw[i].FQ, r, jx);
			gZ.AddPoint(bwct.bw[i].FQ, r, bwct.bw[i].Flag);
			if( bwct.bwsfull && (i < BWSMAX) ){
				for( j = 0; j < bwct.bwsdim; j++ ){
					r = bwct.bws[i][j].R;
					jx = bwct.bws[i][j].JX;
					CalcZ(bwct.bws[i][j].FQ, r, jx);
					gZ.AddPoint(bwct.bws[i][j].FQ, r, bwct.bws[i][j].Flag);
				}
			}
		}
		gZ.AutoRenge(0.0);
		gZ.DrawXGage();
		gZ.DrawGage(0, clBlack);
		gZ.Plot(clBlack);
		gZ.DrawConst(AntL, AntC, AntQ, AntW, AntF);
		if( exeenv.BwMatch == 2 ){
			sprintf(bf, MATCHINFO, StrDbl(MatchData.FQ));
			gZ.DrawInfo(bf);
		}

		gZ.Clear();
		for( i = 0; i < BWMAX; i++ ){
			r = bwct.bw[i].R;
			jx = bwct.bw[i].JX;
			CalcZ(bwct.bw[i].FQ, r, jx);
			gZ.AddPoint(bwct.bw[i].FQ, jx, bwct.bw[i].Flag);
			if( bwct.bwsfull && (i < BWSMAX) ){
				for( j = 0; j < bwct.bwsdim; j++ ){
					r = bwct.bws[i][j].R;
					jx = bwct.bws[i][j].JX;
					CalcZ(bwct.bws[i][j].FQ, r, jx);
					gZ.AddPoint(bwct.bws[i][j].FQ, jx, bwct.bws[i][j].Flag);
				}
			}
		}
		gZ.AutoRenge(-MAXDOUBLE);
		gZ.AdjRengeZero();
		gZ.DrawGage(1, clBlack);
		gZ.Plot(clBlack, psDashDot);

		gS.DrawAngle();
		gS.DrawTitle(0, "SWR", clBlack);

		gS.Clear();
		gS.SetYLimit(SWRLimit);
		double s;
		for( i = 0; i < BWMAX; i++ ){
			s = CalcSWR(bwct.bw[i].FQ, bwct.bw[i].R, bwct.bw[i].JX);
			slst[i].SWR = s;
			slst[i].FQ = bwct.bw[i].FQ;
			if( s > 1e4 ) s = 1e4;
			gS.AddPoint(bwct.bw[i].FQ, s, bwct.bw[i].Flag);
			if( bwct.bwsfull && (i < BWSMAX) ){
				for( j = 0; j < bwct.bwsdim; j++ ){
					s = CalcSWR(bwct.bws[i][j].FQ, bwct.bws[i][j].R, bwct.bws[i][j].JX);
					if( s > 1e4 ) s = 1e4;
					gS.AddPoint(bwct.bws[i][j].FQ, s, bwct.bws[i][j].Flag);
				}
			}
		}
		gS.AutoRenge(1.0);
		gS.DrawXGage();
		gS.DrawGage(0, clBlack);
		gS.Plot(clBlack);
		if( exeenv.BwMatch ){
			sprintf(bf, MATCHINFO, StrDbl(MatchData.FQ));
			gS.DrawInfo(bf);
		}
		else {
			sprintf(bf, "SWR基準: %s", StrCLXZ(env.RO, env.JXO));
			gS.DrawInfo(bf);
		}

		if( CheckSWRDisp()==TRUE ){
			gS.DrawInfoL(0, "⇔%.1lf KHz(SWR<1.5)", GetSWRWidth(1.5));
			gS.DrawInfoL(1, "⇔%.1lf KHz(SWR<2.0)", GetSWRWidth(2.0));
		}

		gG.DrawAngle();
		gG.DrawTitle(0, "GA", clBlack);
		gG.DrawTitle(1, "FB", clBlack);

		gG.Clear();
		for( i = 0; i < BWMAX; i++ ){
			gG.AddPoint(bwct.bw[i].FQ, bwct.bw[i].GA, bwct.bw[i].Flag);
			if( bwct.bwsfull && (i < BWSMAX) ){
				for( j = 0; j < bwct.bwsdim; j++ ){
					gG.AddPoint(bwct.bws[i][j].FQ, bwct.bws[i][j].GA, bwct.bws[i][j].Flag);
				}
			}
		}
		gG.AutoRenge(-MAXDOUBLE);
		gG.DrawXGage();
		gG.DrawGage(0, clBlack);
		gG.Plot(clBlack);

		gG.Clear();
		for( i = 0; i < BWMAX; i++ ){
			gG.AddPoint(bwct.bw[i].FQ, bwct.bw[i].FB, bwct.bw[i].Flag);
			if( bwct.bwsfull && (i < BWSMAX) ){
				for( j = 0; j < bwct.bwsdim; j++ ){
					gG.AddPoint(bwct.bws[i][j].FQ, bwct.bws[i][j].FB, bwct.bws[i][j].Flag);
				}
			}
		}
		gG.AutoRenge(-MAXDOUBLE);
		gG.DrawGage(1, clBlack);
		gG.Plot(clBlack, psDashDot);

		Printer()->EndDoc();
	}
}
//---------------------------------------------------------------------------
void __fastcall TBwDispDlg::KMOFFClick(TObject *Sender)
{
	exeenv.BwMatch = 0;
	PBoxZ->Invalidate();
	PBoxS->Invalidate();
	EnbSel = 0;
	SelMatch->ItemIndex = exeenv.BwMatch;
	UpdateMatch();
	EnbSel = 1;
}
//---------------------------------------------------------------------------
void __fastcall TBwDispDlg::KMSWRClick(TObject *Sender)
{
	exeenv.BwMatch = 1;
	PBoxZ->Invalidate();
	PBoxS->Invalidate();
	EnbSel = 0;
	SelMatch->ItemIndex = exeenv.BwMatch;
	UpdateMatch();
	EnbSel = 1;
}
//---------------------------------------------------------------------------
void __fastcall TBwDispDlg::KMZSWRClick(TObject *Sender)
{
	exeenv.BwMatch = 2;
	PBoxZ->Invalidate();
	PBoxS->Invalidate();
	EnbSel = 0;
	SelMatch->ItemIndex = exeenv.BwMatch;
	UpdateMatch();
	EnbSel = 1;
}
//---------------------------------------------------------------------------
void __fastcall TBwDispDlg::PopupMenuPopup(TObject *Sender)
{
	switch(exeenv.BwMatch){
		case 0:
			KMOFF->Checked = TRUE;
			KMSWR->Checked = FALSE;
			KMZSWR->Checked = FALSE;
			break;
		case 1:
			KMOFF->Checked = FALSE;
			KMSWR->Checked = TRUE;
			KMZSWR->Checked = FALSE;
			break;
		case 2:
			KMOFF->Checked = FALSE;
			KMSWR->Checked = FALSE;
			KMZSWR->Checked = TRUE;
			break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TBwDispDlg::AdjX(double f, double fo, double &X)
{
	if( X < 0.0 ){
		X *= (fo/f);
	}
	else {
		X *= (f/fo);
	}
}
//---------------------------------------------------------------------------
double __fastcall TBwDispDlg::CalcSWR(double f, double R, double jX)
{
	if( exeenv.BwMatch && MatchData.Flag ){
		double MR = MatchData.R;
		double MX = MatchData.JX;
		double MX1, MX2;
		double s;
		CLX ZL; ZL = CLX(R, jX);
		if( MR <= env.RO ){		// アンテナのインピーダンスが低いとき
			MX2 = -MX - sqrt(MR*env.RO - MR*MR);
			AdjX(f, MatchData.FQ, MX2);
			s = MX + MX2;
			if( s ){
				MX1 = -(MR*MR + s*s)/s;
				AdjX(f, MatchData.FQ, MX1);
			}
			else {
				MX1 = -MAXDOUBLE;
			}
			ZL += CLX(0, MX2);
			ZL = (ZL * CLX(0, MX1))/(ZL + CLX(0, MX1));
		}
		else {	 					// アンテナのインピーダンスが高いとき
			s = 4 * env.RO * env.RO * MX * MX - 4 * (env.RO - MR) * env.RO * (MR*MR + MX*MX);
			s = sqrt(s);
			double b = -2 * env.RO * MX;
			double v = 2 * (env.RO - MR);
			if( v ){
				MX1 = (b + s)/v;
				AdjX(f, MatchData.FQ, MX1);
			}
			else {
				MX1 = 0;
			}
			MX2 = -(MX1 * MR * MR + (MX + MX1)*MX*MX1)/(MR*MR+(MX+MX1)*(MX+MX1));
			AdjX(f, MatchData.FQ, MX2);
			ZL = (ZL * CLX(0, MX1))/(ZL + CLX(0, MX1));
			ZL += CLX(0, MX2);
		}
		return ::CalcSWR(ZL.r, ZL.j, env.RO, 0);
	}
	else {
		return ::CalcSWR(R, jX, env.RO, env.JXO);
	}
}
//---------------------------------------------------------------------------
void __fastcall TBwDispDlg::CalcZ(double f, double &R, double &jX)
{
	if( (exeenv.BwMatch == 2) && MatchData.Flag ){
		double MR = MatchData.R;
		double MX = MatchData.JX;
		double MX1, MX2;
		double MZ = env.RO;
		double s;
		CLX ZL; ZL = CLX(R, jX);
		if( MR <= MZ ){		// アンテナのインピーダンスが低いとき
			MX2 = -MX - sqrt(MR*MZ - MR*MR);
			AdjX(f, MatchData.FQ, MX2);
			s = MX + MX2;
			if( s ){
				MX1 = -(MR*MR + s*s)/s;
				AdjX(f, MatchData.FQ, MX1);
			}
			else {
				MX1 = -MAXDOUBLE;
			}
			ZL += CLX(0, MX2);
			ZL = (ZL * CLX(0, MX1))/(ZL + CLX(0, MX1));
		}
		else {	 					// アンテナのインピーダンスが高いとき
			s = 4 * MZ * MZ * MX * MX - 4 * (MZ - MR) * MZ * (MR*MR + MX*MX);
			s = sqrt(s);
			double b = -2 * MZ * MX;
			double v = 2 * (MZ - MR);
			if( v ){
				MX1 = (b + s)/v;
				AdjX(f, MatchData.FQ, MX1);
			}
			else {
				MX1 = 0;
			}
			MX2 = -(MX1 * MR * MR + (MX + MX1)*MX*MX1)/(MR*MR+(MX+MX1)*(MX+MX1));
			AdjX(f, MatchData.FQ, MX2);
			ZL = (ZL * CLX(0, MX1))/(ZL + CLX(0, MX1));
			ZL += CLX(0, MX2);
		}
		R = ZL.r;
		jX = ZL.j + env.JXO;
	}
}
//---------------------------------------------------------------------------
double __fastcall TBwDispDlg::GetSWRFreq(double sw, double f1, double f2, double s1, double s2)
{
	return f1 + ((f2-f1)*(sw-s1)/(s2-s1));
}
//---------------------------------------------------------------------------
double __fastcall TBwDispDlg::GetSWRWidth(double sw)
{
	double	fl, fh;

	int	i;
	for( i = slstpos - 1; i >= 0; i-- ){
		if( slst[i].SWR >= sw ){
			break;
		}
	}
	if( i < 0 ) i = 0;
	if( slst[i].SWR > slst[i+1].SWR ){
		fl = GetSWRFreq(sw, slst[i].FQ, slst[i+1].FQ, slst[i].SWR, slst[i+1].SWR);
	}
	else {
		fl = slst[i].FQ;
	}
	for( i = slstpos + 1; i < BWMAX; i++ ){
		if( slst[i].SWR >= sw ) break;
	}
	if( i >= BWMAX ) i = BWMAX - 1;
	if( slst[i].SWR > slst[i-1].SWR ){
		fh = GetSWRFreq(sw, slst[i-1].FQ, slst[i].FQ, slst[i-1].SWR, slst[i].SWR);
	}
	else {
		fh = slst[i].FQ;
	}
	return ABS(fh - fl)*1000.0;
}
//---------------------------------------------------------------------------
int __fastcall TBwDispDlg::CheckSWRDisp(void)
{
	double s = MAXDOUBLE;
	int i, pos;

	for( pos = i = 0; i < BWMAX; i++ ){
		if( slst[i].SWR < s ){
			s = slst[i].SWR;
			pos = i;
		}
	}
	if( (pos == 0) || (pos == BWMAX-1) ) return FALSE;
	if( s >= 1.5 ) return FALSE;
	slstpos = pos;		// 中心点を記憶
	return TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TBwDispDlg::SelMatchClick(TObject *Sender)
{
	if( !EnbSel ) return;
	exeenv.BwMatch = SelMatch->ItemIndex;
	UpdateView(TRUE);
}
//---------------------------------------------------------------------------
void __fastcall TBwDispDlg::PageChange(TObject *Sender)
{
	if( !EnbSel ) return;

	double	d;
	if( Calc(d, AnsiString(EditCenter->Text).c_str())==TRUE ){
		if( (d != res.Bwc.fo) && (d > 0.0) ){
			res.Bwc.fo = d;
			res.Bwc.fw += 1.0;
			BwSelChange(NULL);
		}
	}
	UpdateMatch();
	UpdateView(TRUE);
}
//---------------------------------------------------------------------------
void __fastcall TBwDispDlg::UDFreqClick(TObject *Sender, TUDBtnType Button)
{
	if( !FirstUD ){
		FirstUD++;
		return;
	}
	double	d = 14.050;

	if( Calc(d, AnsiString(EditCenter->Text).c_str()) != TRUE ){
		d = 14.050;
	}
	if( d < 1.0 ) d = 1.0;
	double dw = GetNormal(d * 0.001);
	if( Button == btNext ){
		d += dw;
	}
	else if( Button == btPrev ){
		d -= dw;
	}
	if( d < 1.0 ) d = 1.0;
	EditCenter->Text = StrDbl(d);
}
//---------------------------------------------------------------------------
void __fastcall TBwDispDlg::UDFreqChanging(TObject *Sender, bool &AllowChange)
{
	UDFreq->Position = 50;
}
//---------------------------------------------------------------------------
void __fastcall TBwDispDlg::UDMatchClick(TObject *Sender, TUDBtnType Button)
{
	if( !FirstUDM ){
		FirstUDM++;
		return;
	}
	double	d = 14.050;

	if( Calc(d, AnsiString(MatchFreq->Text).c_str()) != TRUE ){
		d = 14.050;
	}
	if( d < 1.0 ) d = 1.0;
	double dw = GetNormal(d * 0.001);
	if( Button == btNext ){
		d += dw;
	}
	else if( Button == btPrev ){
		d -= dw;
	}
	if( d < 1.0 ) d = 1.0;
	MatchFreq->Text = StrDbl(d);
}
//---------------------------------------------------------------------------
void __fastcall TBwDispDlg::UDMatchChanging(TObject *Sender, bool &AllowChange)
{
	UDMatch->Position = 50;
}
//---------------------------------------------------------------------------
// 共振周波数を計算
void __fastcall TBwDispDlg::GetFoBtnClick(TObject *Sender)
{
	if( YesNoMB("この計算は非常に時間がかかる場合があります.\r\n\r\nほんまにやりますか？") != IDYES ) return;
	double	deff;
	for( int i = 0; i < 20; i++ ){
		if( bwct.bcnt >= 2 ){
			deff = ABS(AntF - res.Bwc.fo);
			if( (deff < (res.Bwc.fo * 0.001))||(ABS(res.Bwc.bw[2].JX) < 0.1) ) break;
			if( AntF > (res.Bwc.fo * 2) ) AntF = res.Bwc.fo * 2;
			if( AntF < 0.5 ) AntF = SaveFQ * 5;
			res.Bwc.fo = RoundUp(AntF, 1000);
			deff = GetNormal(res.Bwc.fo * 0.005) * 4;
			BwSel->Text = deff * 1000.0;
			res.Bwc.fw += 1.0;
			BwSelChange(NULL);
		}
		TryBtnClick(NULL);
	}
	EditCenter->Text = StrDbl(res.Bwc.fo);
	UpdateView(TRUE);
}
//---------------------------------------------------------------------------
void __fastcall TBwDispDlg::GridDrawCell(TObject *Sender, int Col, int Row,
	TRect &Rect, TGridDrawState State)
{
	if( (Row >= 1) && (Col >= 1) && (ActiveControl != Grid) ){
		Grid->Canvas->Brush->Color = clWindow;
		Grid->Canvas->Font->Color = clBlack;
	}
	Grid->Canvas->FillRect(Rect);
	int X = Rect.Left + 1;
	int Y = Rect.Top + 2;

	char	bf[64];
	if( Row ){
		Row--;
		BW	*bp = &res.Bwc.bw[Row];
		bf[0] = 0;
		switch(Col){
			case 0:
				strcpy(bf, StrDbl3(bp->FQ));
				break;
			case 1:
				if( bp->Flag ) strcpy(bf, StrDbl1(bp->R));
				break;
			case 2:
				if( bp->Flag ) strcpy(bf, StrDbl1(bp->JX));
				break;
			case 3:
				if( bp->Flag ) strcpy(bf, StrDbl1(bp->GA));
				break;
			case 4:
				if( bp->Flag ) strcpy(bf, StrDbl1(bp->FB));
				break;
			case 5:
				switch(bp->Flag){
					case 1:
						if( bp->PFlag ) strcpy(bf, "○");
						break;
					case 2:
						strcpy(bf, "--");
						break;
				}
				break;
		}
		if( pColorTable != NULL ){
			Grid->Canvas->Font->Color = pColorTable[Row];
		}
		Grid->Canvas->TextOut(X, Y, bf);
	}
	else {		// タイトル
		LPCSTR	_tt[]={
			"Freq","R","jX","Ga","F/B","ON"
		};
		Grid->Canvas->TextOut(X+3, Y, _tt[Col]);
	}
}
//---------------------------------------------------------------------------
// パターン図の表示
void __fastcall TBwDispDlg::PBPtnPaint(TObject *Sender)
{
	DrawPtnH.DrawAngle(2);
	DrawPtnV.DrawAngle(2);
	if( !res.Bwc.bcnt ) return;

	int i, j;
	double Max = -MAXDOUBLE;
	for( i = 0; i < 5; i++ ){
		if( res.Bwc.bw[i].Flag == 1 ){
			if( Max < res.Bwc.bw[i].GA ) Max = res.Bwc.bw[i].GA;
		}
	}
	DrawPtnH.SetMaxDB(Max);
	DrawPtnV.SetMaxDB(Max);
	for( i = 0; i < 5; i++ ){
		if( (res.Bwc.bw[i].Flag == 1) && res.Bwc.bw[i].PFlag ){
			if( RGPtn->ItemIndex < 3 ){
				for( j = 0; j < 180; j++ ){
					PtnH[j] = float(res.Bwc.bw[i].PtnH[RGPtn->ItemIndex][j]) / 256.0;
				}
				for( j = 0; j < 91; j++ ){
					PtnV[j] = float(res.Bwc.bw[i].PtnV[RGPtn->ItemIndex][j]) / 256.0;
				}
			}
			else {
				for( j = 0; j < 180; j++ ){
					PtnH[j] = float(res.Bwc.bw[i].PtnH[0][j]) / 256.0;
				}
				for( j = 0; j < 91; j++ ){
					PtnV[j] = float(res.Bwc.bw[i].PtnV[0][j]) / 256.0;
				}
			}
			DrawPtnH.DrawPtn(PtnH, 2, pColorTable[i]);
			DrawPtnV.DrawPtn(PtnV, 2, pColorTable[i]);
			if( RGPtn->ItemIndex == 3 ){
				for( j = 0; j < 180; j++ ){
					PtnH[j] = float(res.Bwc.bw[i].PtnH[1][j]) / 256.0;
				}
				for( j = 0; j < 91; j++ ){
					PtnV[j] = float(res.Bwc.bw[i].PtnV[1][j]) / 256.0;
				}
				DrawPtnH.DrawPtn(PtnH, 2, pColorTable[i]);
				DrawPtnV.DrawPtn(PtnV, 2, pColorTable[i]);
			}
		}
	}
	int	X, Y;
	DrawPtnV.GetBottomPosition(X, Y);
	char bf[64];
	sprintf( bf, "%s(dBi) = 0dB", StrDbl1(Max));
	int FH = PBPtn->Canvas->TextHeight(bf);
	int Sop = ::SetBkMode(PBPtn->Canvas->Handle, TRANSPARENT);
	Y += FH/2;
	PBPtn->Canvas->TextOut(X, Y, bf);
	::SetBkMode(PBPtn->Canvas->Handle, Sop);
}
//---------------------------------------------------------------------------
void __fastcall TBwDispDlg::RGPtnClick(TObject *Sender)
{
	PBPtn->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TBwDispDlg::GridClick(TObject *Sender)
{
	if( Grid->Row ){
		BW *bp = &res.Bwc.bw[Grid->Row - 1];
		if( bp->Flag != 1 ) return;
		if( Grid->Col == 5 ){
			bp->PFlag = bp->PFlag ? 0 : 1;
		}
		else {
			int i;
			for( i = 0; i < 5; i++ ){
				res.Bwc.bw[i].PFlag = ((Grid->Row - 1) == i) ? 1 : 0;
			}
		}
		UpdateView(TRUE);
	}
}
//---------------------------------------------------------------------------
void __fastcall TBwDispDlg::ColorBtnClick(TObject *Sender)
{
	if( Grid->Row ){
		if( ColorDialog->Execute() == TRUE ){
			pColorTable[Grid->Row-1] = ColorDialog->Color;
			UpdateView(TRUE);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TBwDispDlg::MatchCenterClick(TObject *Sender)
{
	UpdateView(TRUE);
}
//---------------------------------------------------------------------------
void __fastcall TBwDispDlg::LimitSWRChange(TObject *Sender)
{
	SWRLimit = -1.0;
	if( Calc(SWRLimit, AnsiString(LimitSWR->Text).c_str()) == TRUE ){
		if( SWRLimit <= 1.0 ){
			SWRLimit = -1;
			LimitSWR->Text = "∞";
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TBwDispDlg::HalfAddCntChange(TObject *Sender)
{
	int d;

	if( CalcU(d, AnsiString(HalfAddCnt->Text).c_str()) == TRUE ){
		if( d != res.Bwc.bwsdim ){
			if( (d >= 1)&&(d <= BWSDIMMAX) ){
				res.SetBWSDim(d);
				int i, j;
				BWS *bp;
				for( i = 0; i < BWSMAX; i++ ){
					for( j = 0; j < res.Bwc.bwsdim; j++ ){
						if( !res.Bwc.bws[i][j].Flag ){
							bp = res.SearchBW(&bwct, res.Bwc.bws[i][j].FQ);
							if( bp != NULL ){
								memcpy(&res.Bwc.bws[i][j], bp, sizeof(BWS));
							}
						}
					}
				}
				CalcSim();
				UpdateView(FALSE);
			}
			else {
				HalfAddCnt->Text = res.Bwc.bwsdim;
			}
		}
	}
}
//---------------------------------------------------------------------------
