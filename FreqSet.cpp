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

#include "FreqSet.h"
#include "Main.h"
#include "MiniNec3.h"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
//TFreqSetDlg *FreqSetDlg;
//---------------------------------------------------------------------
__fastcall TFreqSetDlg::TFreqSetDlg(TComponent* AOwner)
	: TForm(AOwner)
{
	CanCalc = FALSE;
	pAnt = NULL;
}
//---------------------------------------------------------------------
int __fastcall TFreqSetDlg::Execute(ANTDEF *ap)
{
	pAnt = ap;

	BgnFreq->Text = StrDbl(res.Bwc.bw[0].FQ);
	EndFreq->Text = StrDbl(res.Bwc.bw[4].FQ);
	MatchFreq->Text = StrDbl(res.Bwc.fo);
	FreqIntval->Text = StrDbl(res.Bwc.bi*500.0);
	MatchSel->ItemIndex = exeenv.BwMatch ? (res.Bwc.MatchCenter ? 1 : 2) : 0;
	DispCount();
	UpdateUIAll();

	if( ShowModal() == IDOK ){
		switch(MatchSel->ItemIndex){
			case 0:
				exeenv.BwMatch = 0;
				break;
			case 1:
				if( !exeenv.BwMatch ) exeenv.BwMatch = 1;
				res.Bwc.MatchCenter = 1;
				break;
			case 2:
				if( !exeenv.BwMatch ) exeenv.BwMatch = 1;
				res.Bwc.MatchCenter = 0;
				break;
		}
		return TRUE;
	}
	return FALSE;
}
//---------------------------------------------------------------------
void __fastcall TFreqSetDlg::UpdateUIAll(void)
{
	int f;

	OKBtn->Enabled = CanCalc;
	if( MatchSel->ItemIndex == 2 ){
		f = TRUE;
	}
	else {
		f = FALSE;
	}
	Label6->Enabled = f;
	Label7->Enabled = f;
	MatchFreq->Enabled = f;
}
//---------------------------------------------------------------------
// 計算範囲の表示
void __fastcall TFreqSetDlg::DispCount(void)
{
	double	Bgn, End;
	double	Intval;

	if( Calc(Bgn, AnsiString(BgnFreq->Text).c_str()) == TRUE ){
		if( Calc(End, AnsiString(EndFreq->Text).c_str()) == TRUE ){
			if( Calc(Intval, AnsiString(FreqIntval->Text).c_str()) == TRUE ){
				if( (Bgn < End) && (Intval > 0.0) ){
					Intval = RoundUp(Intval/1000, 1000);
					int Count = int(RoundUp((End - Bgn)/Intval, 1000));
					LCount->Caption = Count + 1;
					CanCalc = TRUE;
					return;
				}
			}
		}
	}
	LCount->Caption = "計算不可";
	CanCalc = FALSE;
}

//---------------------------------------------------------------------
int __fastcall TFreqSetDlg::MakeFile(void)
{
	char	bf[256];
	char	FileName[256];

	if( *MainWnd->antFname.c_str() ){
		strcpy(bf, MainWnd->antFname.c_str());
		SetEXT(bf, ".csv");
		sprintf(FileName, "%s%s", ResDir, GetFileName(bf));
	}
	else {
		sprintf(FileName, "%s無題", ResDir);
	}
	MainWnd->SaveDialog->Title = "周波数特性ファイルを作成";
	MainWnd->SaveDialog->Filter = "CSVファイル(*.csv)|*.csv|";
	MainWnd->SaveDialog->FileName = FileName;
	MainWnd->SaveDialog->DefaultExt = "csv";
	MainWnd->SaveDialog->InitialDir = ResDir;
	if( MainWnd->SaveDialog->Execute() == TRUE ){
		strcpy(FileName, AnsiString(MainWnd->SaveDialog->FileName).c_str());
		return MakeFile(FileName);
	}
	return FALSE;
}
//---------------------------------------------------------------------------
static void __fastcall AdjX(double f, double fo, double &X)
{
	if( X < 0.0 ){
		X *= (fo/f);
	}
	else {
		X *= (f/fo);
	}
}
//---------------------------------------------------------------------
double __fastcall TFreqSetDlg::CalcSWR(BWS *rp, BWS *mp)
{
	double	R = rp->R;
	double	jX = rp->JX;
	if( mp->FQ != 0.0 ){
		double MR = mp->R;
		double MX = mp->JX;
		double MX1, MX2;
		double s;
		CLX ZL; ZL = CLX(R, jX);
		if( MR <= env.RO ){		// アンテナのインピーダンスが低いとき
			MX2 = -MX - sqrt(MR*env.RO - MR*MR);
			AdjX(rp->FQ, mp->FQ, MX2);
			s = MX + MX2;
			if( s ){
				MX1 = -(MR*MR + s*s)/s;
				AdjX(rp->FQ, mp->FQ, MX1);
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
				AdjX(rp->FQ, mp->FQ, MX1);
			}
			else {
				MX1 = 0;
			}
			MX2 = -(MX1 * MR * MR + (MX + MX1)*MX*MX1)/(MR*MR+(MX+MX1)*(MX+MX1));
			AdjX(rp->FQ, mp->FQ, MX2);
			ZL = (ZL * CLX(0, MX1))/(ZL + CLX(0, MX1));
			ZL += CLX(0, MX2);
		}
		return ::CalcSWR(ZL.r, ZL.j, env.RO, 0);
	}
	else {
		return ::CalcSWR(R, jX, env.RO, env.JXO);
	}
}
//---------------------------------------------------------------------
int __fastcall TFreqSetDlg::CalcOne(BWS *rp)
{
	int i, j;
	BWS *bp = res.SearchBW(&res.Bwc, rp->FQ);
	if( bp != NULL ){
		memcpy(rp, bp, sizeof(BWS));
		return TRUE;
	}
	pAnt->cfq = rp->FQ;
	if( MainWnd->SetStackAnt() == TRUE ){
		exeenv.CalcF = 0;
		CalCurrent();
		rp->Flag = 1;
		rp->R = res.GetResP()->R;
		rp->JX = res.GetResP()->JX;
		rp->GA = res.GetResP()->GAIN;
		rp->FB = res.GetResP()->FB;
		for( i = 0; i < BWMAX; i++ ){		// 計算済みデータを格納
			if( !res.Bwc.bw[i].Flag && (res.Bwc.bw[i].FQ == rp->FQ) ){
				res.AddBWC(i);
			}
			else if( i < BWSMAX ){
				for( j = 0; j < res.Bwc.bwsdim; j++ ){
					if( !res.Bwc.bws[i][j].Flag && (res.Bwc.bws[i][j].FQ == rp->FQ) ){
						res.AddBWS(i, j);
					}
				}
			}
		}
		if( !res.Bwc.MatchData.Flag && (res.Bwc.MatchData.FQ == rp->FQ) ){
			memcpy(&res.Bwc.MatchData, rp, sizeof(BWS));
		}
		return TRUE;
	}
	else {
		return FALSE;
	}
}
//---------------------------------------------------------------------
int __fastcall TFreqSetDlg::MakeFile(LPCSTR pName)
{
	int r = FALSE;
	FILE *fp;
	double	Bgn;
	double	End;
	double	Intval;
	double	cfq;
	BWS		CurData;
	BWS		MatchData;
	char	bf[128];

	if( Calc(Bgn, AnsiString(BgnFreq->Text).c_str()) != TRUE ) return FALSE;
	if( Calc(End, AnsiString(EndFreq->Text).c_str()) != TRUE ) return FALSE;
	if( Calc(Intval, AnsiString(FreqIntval->Text).c_str()) != TRUE ) return FALSE;
	Intval /= 1000.0;
	switch(MatchSel->ItemIndex){
		case 0:
			MatchData.FQ = 0.0;
			break;
		case 1:
			MatchData.FQ = RoundUp((Bgn + End)/2.0, 1000);
			break;
		case 2:
			if( Calc(MatchData.FQ, AnsiString(MatchFreq->Text).c_str()) != TRUE ) return FALSE;
			break;
	}
	CWaitCursor tw;
	if( (fp = fopen(pName, "wt")) != NULL ){
		exeenv.CalcLog = 0;
		SaveFQ = pAnt->cfq;
		SaveRes.SetRes(&res);
		if( MatchData.FQ != 0.0 ){
			CalcOne(&MatchData);
			sprintf(bf, "%sMHz", StrDbl(MatchData.FQ));
			if( MatchSel->ItemIndex == 2 ){
				memcpy(&res.Bwc.MatchData, &MatchData, sizeof(BWS));
			}
		}
		else {
			sprintf(bf, "%sΩ", StrCLXZ(env.RO, env.JXO));
		}
		fprintf(fp, "Freq(MHz),R(Ω),jX(Ω),SWR(%s),Ga(dBi),F/B(dB)\n", bf);
		for( cfq = Bgn; RoundUp(cfq, 1000) <= End; cfq += Intval ){
			CurData.FQ = RoundUp(cfq, 1000);
			if( CurData.FQ == MatchData.FQ ){
				memcpy(&CurData, &MatchData, sizeof(CurData));
			}
			else {
				if( CalcOne(&CurData) == FALSE ) break;
			}
			fprintf(fp, "%s,", StrDbl3(CurData.FQ) );
			fprintf(fp, "%s,", StrDbl2(CurData.R) );
			fprintf(fp, "%s,", StrDbl2(CurData.JX) );
			fprintf(fp, "%s,", StrDbl2(CalcSWR(&CurData, &MatchData)) );
			fprintf(fp, "%s,", StrDbl2(CurData.GA) );
			fprintf(fp, "%s\n", StrDbl2(CurData.FB) );
		}
		fclose(fp);
		pAnt->cfq = SaveFQ;
		if( pAnt->cfq && (MainWnd->SetStackAnt() == TRUE) ){
			res.SetRes(&SaveRes);
			MainWnd->Grid1->Invalidate();
		}
		r = TRUE;
	}
	else {
		ErrorMB("'%s'を作成できません.", pName);
	}
	return r;
}
//---------------------------------------------------------------------
void __fastcall TFreqSetDlg::BgnFreqChange(TObject *Sender)
{
	DispCount();
	UpdateUIAll();
}
//---------------------------------------------------------------------------
void __fastcall TFreqSetDlg::MatchSelClick(TObject *Sender)
{
	UpdateUIAll();
}
//---------------------------------------------------------------------------
void __fastcall TFreqSetDlg::OKBtnClick(TObject *Sender)
{
	if( MakeFile() == TRUE ){
		ModalResult = mrOk;
	}
}
//---------------------------------------------------------------------------
