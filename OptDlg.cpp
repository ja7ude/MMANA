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

#include "OptDlg.h"
#include "Mininec3.h"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
//TFreqDlgBox *FreqDlgBox;
LPCSTR CoreTypeT[]={
	"任意","はしご(600)","はしご(450)","リボン",
	"3C2V","5C2V","7C2V","10C2V","5D2V","8D2V","RG58/U","RG58A/U","5DFB","8DFB","10DFB","12DFB","5DSFA","8DSFA","10DSFA","12DSFA"
};
const float CorePT[]={0.98, 0.98, 0.98, 0.85, 0.67, 0.67, 0.67, 0.67, 0.67, 0.67, 0.67, 0.67, 0.81, 0.81, 0.80, 0.81, 0.83, 0.83, 0.88, 0.88, 0};
const float CoreZT[]={600, 600, 450, 300, 75, 75, 75, 75, 50, 50, 53.5, 50, 50, 50, 50, 50, 50, 50, 50, 50, 0};
//---------------------------------------------------------------------
__fastcall TOptDlgBox::TOptDlgBox(TComponent* AOwner)
	: TForm(AOwner)
{
	EnbCalc = 0;

	CoilL = NULLV;
	CoilW = 0.1;
	CoilS = 0.1;
	CoilD = 10.0;
	CoilN = 10;

	MT = 0;
	MZ = env.RO;
	MR = 0.0;
	MX = 0.0;
	MF = 14.050e6;
	MD1 = 0;
	MD2 = 0;
	MM1[0] = MM1[1] = MM2[0] = MM2[1] = 0;
	for( int i = 0; FreqTbl[i] != NULL; i++ ){
		TFreq->Items->Add(FreqTbl[i]);
	}
	MFreq->Items->Assign(TFreq->Items);
	CFreq->Items->Assign(TFreq->Items);
	SMEditFreq->Items->Assign(TFreq->Items);

	CoreType = 8;
	CoreP = CorePT[CoreType];
	CoreZ = CoreZT[CoreType];
	CoreL = 100;
	CoreLC = 0.0;

	QMRi1 = 50.0;
	QMXi1 = 0.0;
	QMRL1 = 50.0;
	QMXL1 = 0.0;
	QMZo = 50.0;
	QML = 0.5;
	QMA = 0;

	SMF = 0;
	QMRi2 = 50.0;
	QMRL2 = 75.0;
	QMXL2 = 0.0;
	QMZo1 = 75.0;
	QML1 = 0;
	QMZo2 = 50.0;
	QML2 = 0;
	QMSWR = 1.0;
	QMSWRL = 1.0;
	QMSWR2 = 1.0;

	SMR = 25;
	SMX = 0;
	SMZo = 600;
	SMCm = 0.95;
	SMFreq = 14.05;
	SMZi = 50;
	for( int i = 0; CorePT[i]; i++ ){
		CType->Items->Strings[i] = CoreTypeT[i];
	}
}
//---------------------------------------------------------------------
void __fastcall TOptDlgBox::SetValueItem(int n)
{
	char bf[256];

	EnbCalc = 0;
	if( n != 0 ) TFreq->Text = (Freq != NULLV) ? StrDbl(Freq / 1e6) : "";
	if( n != 1 ) TEditX->Text = (X != NULLV) ? StrDbl(X) : "";
	if( n != 2 ) TEditL->Text = (L != NULLV) ? StrDbl(L * 1e6) : "";
	if( n != 3 ) TEditC->Text = (C != NULLV) ? StrDbl(C * 1e12) : "";
	AnsiString as;
	if( Freq ){
		double rmd = 299.8e6/Freq;
		sprintf(bf, "λ  = %sm\r\n", StrDbl(rmd));
		as += bf;
		sprintf(bf, "1/2 = %sm\r\n", StrDbl(rmd/2));
		as += bf;
		sprintf(bf, "1/4 = %sm\r\n", StrDbl(rmd/4));
		as += bf;
		sprintf(bf, "5/8 = %sm", StrDbl(rmd*5/8));
		as += bf;
	}
	TStat->Caption = as;

	if( n != 10 ) AEditL->Text = (CoilL != NULLV) ? StrDbl(CoilL * 1e6) : "";
	if( n != 11 ) AEditN->Text = StrDbl(CoilN);
	if( n != 12 ) AEditD->Text = StrDbl(CoilD);
	if( n != 13 ) AEditW->Text = StrDbl(CoilW*10.0);
	if( n != 14 ) AEditS->Text = StrDbl(CoilS*10.0);
	ACoilLen->Caption = StrDbl(((CoilW + CoilS) * CoilN));

	if( n != 20 ) MFreq->Text = (MF != NULLV) ? StrDbl(MF / 1e6) : "";
	if( n != 21 ) MEditZ->Text = StrDbl(MZ);
	if( n != 22 ) MEditR->Text = StrDbl(MR);
	if( n != 23 ) MEditX->Text = StrDbl(MX);

	if( n != 30 ) CFreq->Text = StrDbl(CoreFreq * 1e-6);
	if( n != 31 ) CType->Text = CoreTypeT[CoreType];
	if( n != 32 ) CEditL->Text = StrDbl(CoreL);
	if( n != 33 ) CEditX->Text = StrDbl(CoreX);
	if( n != 34 ) CEditLC->Text = StrDbl(CoreLC);
	if( n != 35 ) CEditP->Text = StrDbl(CoreP);
	if( n != 36 ) CEditZ->Text = StrDbl(CoreZ);
	if( CoreX >= 0.0 ){
		CLCType->Caption = "L";
		CLCUnit->Caption = "uH";
	}
	else {
		CLCType->Caption = "C";
		CLCUnit->Caption = "pF";
	}
	if( CoreFo == NULLV ){
		CFo->Caption = "";
	}
	else {
		sprintf(bf, "Fo=%sMHz (1/4λ)", StrDbl(CoreFo));
		CFo->Caption = bf;
	}

	if( n != 40) QEdRi1->Text = StrDbl(QMRi1);
	if( n != 41) QEdXi1->Text = StrDbl(QMXi1);
	if( n != 42) QEdRL1->Text = StrDbl(QMRL1);
	if( n != 43) QEdXL1->Text = StrDbl(QMXL1);
	if( n != 44) QEdZo->Text = StrDbl(QMZo);
	if( n != 45) QEdL->Text = StrDbl(QML);
	if( n != 46) QEdA->Text = StrDbl(QMA);

	if( n != 50) QEdRi2->Text = StrDbl(QMRi2);
	if( n != 51) QEdRL2->Text = StrDbl(QMRL2);
	if( n != 52) QEdXL2->Text = StrDbl(QMXL2);
	if( n != 53) QEdZo1->Text = StrDbl(QMZo1);
	if( n != 54) QEdL1->Text = StrDbl(QML1);
	if( n != 55) QEdZo2->Text = StrDbl(QMZo2);
	if( n != 56) QEdL2->Text = StrDbl(QML2);
	sprintf(bf, "SWR(%u)=%s", int(QMZo), StrDbl3(QMSWR));
	QLSWR->Caption = bf;
	sprintf(bf, "SWR(%u)=%s", int(QMZo), StrDbl3(QMSWRL));
	QLSWRL->Caption = bf;
	sprintf(bf, "SWR(%u)=%s", int(QMRi2), StrDbl3(QMSWR2));
	QLSWR2->Caption = bf;

	if( n != 60) SMEditR->Text = StrDbl(SMR);
	if( n != 61) SMEditX->Text = StrDbl(SMX);
	if( n != 62) SMEditZo->Text = StrDbl(SMZo);
	if( n != 63) SMEditCm->Text = StrDbl(SMCm);
	if( n != 64) SMEditFreq->Text = StrDbl(SMFreq);
	if( n != 65) SMEditZi->Text = StrDbl(SMZi);
	StrText	st(512);
	if( SMF >= 1 ){
		st.Printf("L1 :%.4lfλ (%.1lf cm)\n", SML1, (29980.0/SMFreq)*SMCm*SML1);
		st.Printf("ZS :%s\r\n", StrCLX(SMR1, SMX1));
		if( ABS(SMXC1) >= 1e10 ){
			st.Printf("XS :∞Ω\r\n");
		}
		else if( SMXC1 >= 0 ){
			st.Printf("XS :%.2lfΩ (%.3lf uH)\r\n", SMXC1, 0.1591549*SMXC1/SMFreq);
		}
		else {
			st.Printf("XS :%.2lfΩ (%.3lf pF)\r\n", SMXC1, -159154.9/(SMFreq*SMXC1));
		}
		if( ABS(SMXC1) < 1e10 ){
			st.Printf("L2s:%.4lfλ (%.1lf cm)\n", SMLS1, (29980.0/SMFreq)*SMCm*SMLS1 );
			st.Printf("L2o:%.4lfλ (%.1lf cm)\n", SMLO1, (29980.0/SMFreq)*SMCm*SMLO1 );
		}
		else {
			st.Printf("L2 :不要\n\n" );
		}
		st.Printf("Zi :%s SWR:%.2lf\n", StrCLX(SMRi1, SMXi1), SMSWR1);
	}
	else {
		st.Printf("不明");
	}
	SMStat1->Caption = st.GetText();
	st.Clear();
	if( SMF >= 2 ){
		st.Printf("L1 :%.4lfλ (%.1lf cm)\n", SML2, (29980.0/SMFreq)*SMCm*SML2);
		st.Printf("ZS :%s\r\n", StrCLX(SMR2, SMX2));
		if( ABS(SMXC2) >= 1e10 ){
			st.Printf("XS :∞Ω\r\n");
		}
		else if( SMXC2 >= 0 ){
			st.Printf("XS :%.2lfΩ (%.3lf uH)\r\n", SMXC2, 0.1591549*SMXC2/SMFreq);
		}
		else {
			st.Printf("XS :%.2lfΩ (%.3lf pF)\r\n", SMXC2, -159154.9/(SMFreq*SMXC2));
		}
//	    st.Printf("XS :%sΩ\r\n", (ABS(SMXC2) >= 1e10) ? "∞":StrDbl2(SMXC2));
		if( ABS(SMXC1) < 1e10 ){
			st.Printf("L2s:%.4lfλ (%.1lf cm)\n", SMLS2, (29980.0/SMFreq)*SMCm*SMLS2 );
			st.Printf("L2o:%.4lfλ (%.1lf cm)\n", SMLO2, (29980.0/SMFreq)*SMCm*SMLO2 );
		}
		else {
			st.Printf("L2 :不要\n\n" );
		}
		st.Printf("Zi :%s SWR:%.2lf\n", StrCLX(SMRi2, SMXi2), SMSWR2);
	}
	else {
		st.Printf("不明");
	}
	SMStat2->Caption = st.GetText();

	EnbCalc = 1;
}

//---------------------------------------------------------------------
int __fastcall TOptDlgBox::Execute(double freq, double r, double x, double l, double c)
{
	char bf[128];

	SMFreq = freq;
	Freq = freq * 1e6;
	X = ABS(x);
	L = l * 1e-6;
	C = c * 1e-12;
	if( L && C ){
		TFixFreq->Checked = FALSE;
		CalcTank(3);
	}
	else {
		TFixFreq->Checked = TRUE;
		if( X ){
			CalcTank(0);
		}
		else if( L ){
			CalcTank(1);
		}
		else if( C ){
			CalcTank(2);
		}
		else {
			CalcTank(0);
		}
	}
	if( L ){
		CoilL = L;
		CalcCoil(0);
	}
	CoreFreq = Freq;
	EnbCalc = 0;
	if( C && (C!=NULLV) ){
		CStub->ItemIndex = 0;		// 開放スタブ
		CoreX = -1;
		CoreLC = C * 1e12;
		CalcCore(0);
	}
	else if( L && (L!=NULLV) ){
		CStub->ItemIndex = 1;		// 短絡スタブ
		CoreX = 1;
		CoreLC = L * 1e6;
		CalcCore(0);
	}
	else {
		CStub->ItemIndex = 0;		// 開放スタブ
		CoreX = -1;
		CoreLC = 0;
		CalcCore(0);
	}
	CalcMatch();
	MF = Freq;
	MR = r;
	MX = x;
	if( r || x ){
		SMR = QMRL1 = QMRL2 = r;
		SMX = QMXL1 = QMXL2 = x;
	}
	sprintf(bf, "%u", env.fbr & 0x0000ffff);
	EEditFBR->Text = bf;
	sprintf(bf, "%u", (env.fbr >> 16) & 0x0000ffff);
	EEditFBRV->Text = bf;
	EEditRO->Text = StrDbl(env.RO);
	EditJXO->Text = StrDbl(env.JXO);

	CBCurDir->Checked = exeenv.CurDir;
	RGFixAngle->ItemIndex = exeenv.FixFreeAngle;
	EditPls->Text = env.pmax;
	EditRecentMax->Text = exeenv.RecentMax;
	RecentMAA->Checked = exeenv.RecentMAA;
	EditFont->Text = env.FontSize;
	SetValueItem(-1);
	if( ShowModal() == IDOK ){
		exeenv.CurDir = CBCurDir->Checked;
		exeenv.FixFreeAngle = RGFixAngle->ItemIndex;
		env.fbr = 0;
		int di;
		double	d;
		if( CalcU(di, AnsiString(EEditFBR->Text).c_str()) == TRUE ){
			if( (di >= 0) && (di <= 359) ){
				env.fbr = di;
			}
		}
		if( CalcU(di, AnsiString(EEditFBRV->Text).c_str()) == TRUE ){
			if( (di >= 0) && (di <= 179) ){
				env.fbr |= (di << 16);
			}
		}
		if( Calc(d, AnsiString(EEditRO->Text).c_str()) == TRUE ){
			if( d > 0.0 ){
				env.RO = d;
			}
		}
		if( Calc(d, AnsiString(EditJXO->Text).c_str()) == TRUE ){
			env.JXO = d;
		}
		if( CalcU(di, AnsiString(EditRecentMax->Text).c_str()) == TRUE ){
			if( (di >= 0) && (di <= RECMENUMAX) ){
				exeenv.RecentMax = di;
			}
		}
		exeenv.RecentMAA = RecentMAA->Checked;
		if( Calc(d, AnsiString(EditFont->Text).c_str()) == TRUE ){
			env.FontSize = int(d);
		}
		if( CalcU(di, AnsiString(EditPls->Text).c_str()) == TRUE ){
			if( (di > 0) && (di <= 8192) ){
				if( di != env.pmax ){
					DeleteNEC();
					env.pmax = InitNEC(di);
					if( env.pmax != di ){
						WarningMB("メモリが十分に確保できませんでした.\r\n\r\n処理できる最大パルス数は %d です.", env.pmax);
					}
					return 2;
				}
			}
		}
		return TRUE;
	}
	return FALSE;
}
//---------------------------------------------------------------------
void __fastcall TOptDlgBox::CalcTank(int sw)
{
	double w = 2 * PAI * Freq;
	switch(sw){
		case 0:		// X -> L, C
			L = NULLV;
			C = NULLV;
			if( w * X){
				L = (X / w);
				C = 1 / (X * w);
			}
			break;
		case 1:		// L -> X, C
			X = NULLV;
			C = NULLV;
			if( w * X ){
				X = (w * L);
				C = 1 / (X * w);
			}
			break;
		case 2:		// C -> X, L
			X = NULLV;
			L = NULLV;
			if( w * C ){
				X = 1 / (C * w);
				L = X / w;
			}
			break;
		case 3:		// L,C -> Freq
			Freq = NULLV;
			X = NULLV;
			if( L * C ){
				Freq = 1/(2 * PAI * sqrt(C * L));
				w = 2 * PAI * Freq;
				X = (w * L);
			}
			break;
	}
}

//---------------------------------------------------------------------
void __fastcall TOptDlgBox::TEditXChange(TObject *Sender)
{
	if( !EnbCalc ) return;
	double	d;
	if( Calc(d, AnsiString(TEditX->Text).c_str()) == TRUE ){
		if( d != X ){
			X = ABS(d);
			CalcTank(0);
			SetValueItem(1);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::TEditLChange(TObject *Sender)
{
	if( !EnbCalc ) return;
	double	d;
	if( Calc(d, AnsiString(TEditL->Text).c_str()) == TRUE ){
		d *= 1e-6;
		if( (d != L) && (d > 0) && (d < 100000.0e-6) ){
			L = d;
			if( TFixFreq->Checked == TRUE ){	// F固定
				CalcTank(1);
			}
			else {
				CalcTank(3);
			}
			SetValueItem(2);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::TEditCChange(TObject *Sender)
{
	if( !EnbCalc ) return;
	double	d;
	if( Calc(d, AnsiString(TEditC->Text).c_str() )== TRUE ){
		d *= 1e-12;
		if( (d != C) && (d > 0) && (d < 100000.0e-12) ){
			C = d;
			if( TFixFreq->Checked == TRUE ){	// F固定
				CalcTank(2);
			}
			else {
				CalcTank(3);
			}
			SetValueItem(3);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::TFreqChange(TObject *Sender)
{
	if( !EnbCalc ) return;
	double	d;
	if( Calc(d, AnsiString(TFreq->Text).c_str() )== TRUE ){
		d *= 1e6;
		if( (d != Freq) && (d > 0) && (d < 100000.0e6) ){
			Freq = d;
			CalcTank(0);
			SetValueItem(0);
		}
	}
}
//---------------------------------------------------------------------------
// 直径(CoilD)と線径(CoilW)、ピッチ(CoilS)は固定
void __fastcall TOptDlgBox::CalcCoil(int sw)
{
	double	B;
	double	LL = CoilL * 1e6;
	if( CoilL == NULLV ) LL = 0;
	switch(sw){
		case 0:		// N -> L
			B = 102*CoilN*(CoilW+CoilS)/CoilD;
			CoilL = (CoilD * CoilN * CoilN)/(B + 45);
			CoilL = CoilL * 1e-6;
			break;
		case 1:		// L -> N
			B = 102*LL*(CoilW+CoilS)/CoilD;
			CoilN = (B + sqrt(B*B + 180*CoilD*LL))/(2*CoilD);
			break;
		case 2:		// L -> D
			B = 408 * CoilN * CoilN * LL * CoilN * (CoilW + CoilS);
			CoilD = ((45 * LL) + sqrt(45*45*LL*LL + B))/(2*CoilN*CoilN);
			break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::AEditLChange(TObject *Sender)
{
	if( !EnbCalc ) return;
	double	d;
	if( Calc(d, AnsiString(AEditL->Text).c_str() )== TRUE ){
		d *= 1e-6;
		if( (d != CoilL) && (d > 0) && (d < 100000.0e-6) ){
			CoilL = d;
			if( ACalSel->ItemIndex ){
				CalcCoil(ACalSel->ItemIndex);
			}
			else {
				CalcCoil(1);
			}
			SetValueItem(10);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::AEditNChange(TObject *Sender)
{
	if( !EnbCalc ) return;
	double	d;
	if( Calc(d, AnsiString(AEditN->Text).c_str() )== TRUE ){
		if( (d != CoilN) && (d > 0) ){
			CoilN = d;
			if( ACalSel->ItemIndex != 1 ){
				CalcCoil(ACalSel->ItemIndex);
			}
			else {
				CalcCoil(2);
			}
			CalcCoil(1);
			SetValueItem(11);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::AEditDChange(TObject *Sender)
{
	if( !EnbCalc ) return;
	double	d;
	if( Calc(d, AnsiString(AEditD->Text).c_str())== TRUE ){
		if( (d != CoilD) && (d > 0) ){
			CoilD = d;
			if( ACalSel->ItemIndex != 2 ){
				CalcCoil(ACalSel->ItemIndex);
			}
			else {
				CalcCoil(1);
			}
			SetValueItem(12);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::AEditWChange(TObject *Sender)
{
	if( !EnbCalc ) return;
	double	d;
	if( Calc(d, AnsiString(AEditW->Text).c_str())== TRUE ){
		d *= 0.1;
		if( (d != CoilW) && (d > 0) ){
			CoilW = d;
			CalcCoil(ACalSel->ItemIndex);
			SetValueItem(13);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::AEditSChange(TObject *Sender)
{
	if( !EnbCalc ) return;
	double	d;
	if( Calc(d, AnsiString(AEditS->Text).c_str())== TRUE ){
		d *= 0.1;
		if( (d != CoilS) && (d >= 0) ){
			CoilS = d;
			CalcCoil(ACalSel->ItemIndex);
			SetValueItem(14);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::PageChange(TObject *Sender)
{
	double d;
	if( Page->ActivePage == TabTank ){
		if( CoilL != L ){
			TEditL->Text = (CoilL != NULLV) ? StrDbl(CoilL * 1e6) : "";
		}
		if( MF != Freq ){
			TFreq->Text = (MF != NULLV) ? StrDbl(MF * 1e-6) : "";
		}
	}
	else if( Page->ActivePage == TabCoil ){
		if( CoilL != L ){
			AEditL->Text = (L != NULLV) ? StrDbl(L * 1e6) : "";
		}
	}
	else if( Page->ActivePage == TabMatch ){
		EnbCalc = 0;
		CoilL = L;
		if( MF != Freq ){
			MFreq->Text = (Freq != NULLV) ? StrDbl(Freq * 1e-6) : "";
			MF = Freq;
		}
		if( Calc(d, AnsiString(EEditRO->Text).c_str()) == TRUE ){
			if( d != MZ ){
				MEditZ->Text = StrDbl(d);
				MZ = d;
			}
		}
		CalcMatch();
		PBoxM->Invalidate();
		SetValueItem(-1);
	}
	else if( Page->ActivePage == TabQM ){
		if( Calc(d, AnsiString(EEditRO->Text).c_str()) == TRUE ){
			QMRi2 = d;
		}
		CalcZi(QMRi1, QMXi1, QMZo, QML, QMRL1, QMXL1, QMA);
		CalcQMSWR();
		SetValueItem(-1);
	}
	else if( Page->ActivePage == TabStub ){
		if( Calc(d, AnsiString(EEditRO->Text).c_str()) == TRUE ){
			SMZi = d;
		}
		CalcSM();
		SetValueItem(-1);
	}
	else if( Page->ActivePage == TabCore ){
		if( CoreFreq != Freq ){
			CFreq->Text = StrDbl(Freq * 1e-6);
		}
	}
}
//---------------------------------------------------------------------------
// マッチング回路の表示
void __fastcall TOptDlgBox::PBoxMPaint(TObject *Sender)
{
	if( !MF ) return;
	if( !(MR * MZ) ) return;

	char	bf[32];

	TCanvas *tp = PBoxM->Canvas;
	int X1 = MT ? 180 : 30;		// 並列Ｃの位置
	int X11 = 110;				// 直列Ｌの位置
	int Y1 = 30;
	int Y2 = 120;
	int YC = (Y1+Y2)/2;
	int FH = tp->TextHeight("C");
	int FW = tp->TextWidth("C");
	tp->MoveTo(10, Y1);tp->LineTo(X11-FW, Y1);			// 上のライン左
	tp->MoveTo(X11+FW, Y1);tp->LineTo(290, Y1);		// 上のライン右（中なし）
	tp->MoveTo(10, Y2);tp->LineTo(290, Y2);				// 下のライン
	if( MD1 ){		// 並列コンデンサ
		tp->MoveTo(X1, Y1);tp->LineTo(X1, YC-FH);		// 縦のライン上
		tp->MoveTo(X1, YC+FH);tp->LineTo(X1, Y2);		// 縦のライン下
		FW = tp->TextWidth(MM1);
		tp->TextOut(X1 - FW/2, YC - (FH/2), MM1);
		sprintf(bf, "%.3lf%s", MD1, MM1[0]=='C' ? "pF":"uH");
		tp->TextOut(X1+FW+FW, YC - FH/2, bf);
		sprintf(bf, "%.3lfΩ", MX1);
		tp->TextOut(X1+FW+FW, YC + FH/2, bf);
	}
	if( MD2 ){		// 直列リアクタンス
		FW = tp->TextWidth(MM2);
		tp->TextOut(X11 - FW/2, Y1 - (FH/2), MM2);
		sprintf(bf, "%.3lf%s", MD2, MM2[0]=='C' ? "pF":"uH");
		FW = tp->TextWidth(bf);
		tp->TextOut(X11-FW/2, Y1-FH-FH/2, bf);
		sprintf(bf, "%.3lfΩ", MX2);
		FW = tp->TextWidth(bf);
		tp->TextOut(X11-FW/2, Y1+FH/2, bf);
	}
	else {
		tp->MoveTo(X11-FW, Y1);tp->LineTo(X11+FW, Y1);
	}
}
//---------------------------------------------------------------------------
// リアクタンス打ち消し回路の変更
void __fastcall TOptDlgBox::MXSelClick(TObject *Sender)
{
	CalcMatch();
	SetValueItem(-1);
	PBoxM->Invalidate();
}
//---------------------------------------------------------------------------
// マッチング定数を得る
void __fastcall TOptDlgBox::CalcMatch(void)
{
	double s;
	double w = 2 * PAI * MF;
	if( !w ) return;
	if( !(MR * MZ) ) return;

	if( (!MX) && (MR == MZ) ){	// マッチング不要
		MD1 = 0;
		MD2 = 0;
		return;
	}
	else if( MR <= MZ ){		// アンテナのインピーダンスが低いとき
		MT = 0;
		if( MXSel->ItemIndex ){
			MX2 = -MX - sqrt(MR*MZ - MR*MR);
		}
		else {
			MX2 = -MX + sqrt(MR*MZ - MR*MR);
		}
		s = MX + MX2;
		if( s ){
			MX1 = -(MR*MR + s*s)/s;
		}
		else {
			MX1 = -MAXDOUBLE;
		}
	}
	else {	 					// アンテナのインピーダンスが高いとき
		MT = 1;
		s = 4 * MZ * MZ * MX * MX - 4 * (MZ - MR) * MZ * (MR*MR + MX*MX);
		if( s < 0 ) return;
		s = sqrt(s);
		double b = -2 * MZ * MX;
		double v = 2 * (MZ - MR);
		if( v ){
			if( MXSel->ItemIndex ){
				MX1 = (b - s)/v;
			}
			else {
				MX1 = (b + s)/v;
			}
		}
		else {
			MX1 = 0;
		}
		MX2 = -(MX1 * MR * MR + (MX + MX1)*MX*MX1)/(MR*MR+(MX+MX1)*(MX+MX1));
	}
	if( MX1 >= 0 ){
		MM1[0] = 'L';
		MD1 = MX1*1e6/w;
	}
	else if( MX1 == -MAXDOUBLE ){
		MM1[0] = 'C';
		MD1 = 0;
	}
	else {
		MM1[0] = 'C';
		MD1 = -1e12/(w * MX1);
	}
	if( MX2 >= 0 ){
		MM2[0] = 'L';
		MD2 = MX2*1e6/w;
	}
	else {
		MM2[0] = 'C';
		MD2 = -1e12/(w * MX2);
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::MFreqChange(TObject *Sender)
{
	if( !EnbCalc ) return;
	double	d;
	if( Calc(d, AnsiString(MFreq->Text).c_str())== TRUE ){
		d *= 1e6;
		if( (d != MF) && (d > 0) ){
			MF = d;
			CalcMatch();
			SetValueItem(20);
			PBoxM->Invalidate();
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::MEditRChange(TObject *Sender)
{
	if( !EnbCalc ) return;
	double	d;
	if( Calc(d, AnsiString(MEditR->Text).c_str())== TRUE ){
		if( (d != MR) && (d > 0) ){
			MR = d;
			CalcMatch();
			SetValueItem(22);
			PBoxM->Invalidate();
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::MEditXChange(TObject *Sender)
{
	if( !EnbCalc ) return;
	double	d;
	if( Calc(d, AnsiString(MEditX->Text).c_str())== TRUE ){
		if( d != MX ){
			MX = d;
			CalcMatch();
			SetValueItem(23);
			PBoxM->Invalidate();
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::MEditZChange(TObject *Sender)
{
	if( !EnbCalc ) return;
	double	d;
	if( Calc(d, AnsiString(MEditZ->Text).c_str())== TRUE ){
		if( (d != MZ) && (d > 0) ){
			MZ = d;
			CalcMatch();
			SetValueItem(21);
			PBoxM->Invalidate();
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TOptDlgBox::CalcCore(int n)
{
	double	deg, rmd;

	rmd = 299.8e6 * CoreP/CoreFreq;
	switch(n){
		case 0:		// 現在のLC値からＸとＬを得る
			if( CoreX >= 0 ){		// Lとして計算
				CoreX = 2 * PAI * CoreFreq * CoreLC * 1e-6;
			}
			else {					// Cとして計算
				if( CoreLC ){
					CoreX = -1/(2 * PAI * CoreFreq * CoreLC * 1e-12);
				}
				else {
					CoreX = -9.999999e37;
				}
			}
			CalcCore(2);
			break;
		case 1:		// ＸからLC値と長さを計算
			if( CoreX >= 0 ){		// Ｌを計算
				CoreLC = CoreX * 1e6/(2 * PAI * CoreFreq);
			}
			else {					// Ｃを計算
				CoreLC = -1e12/(2 * PAI * CoreFreq * CoreX);
			}
		case 2:	// Ｘから長さＬを計算
			if( CStub->ItemIndex ){		// ショートスタブ
				deg = atan(CoreX / CoreZ) * 180.0 / PAI;
			}
			else {						// 開放スタブ
				if( !CoreX ){
					deg = 90.0;
				}
				else {
					deg = atan(-CoreZ / CoreX) * 180.0 / PAI;
				}
			}
			CoreL = deg * rmd * 100.0/ 360.0;
			if( CoreL < 0.0 ){
				CoreL += (rmd * 100.0/ 2);
			}
			break;
		case 3:		// 長さからＸとＬＣを計算
			if( CStub->ItemIndex ){		// ショートスタブ
				CoreX = CoreZ * tan(2*PAI*CoreL/(rmd*100));
			}
			else {						// 開放スタブ
				if( CoreL ){
					CoreX = -CoreZ * 1/tan(2*PAI*CoreL/(rmd*100));
				}
				else {
					CoreX = -9.999999e37;
				}
			}
			if( CoreX >= 0 ){		// Ｌを計算
				CoreLC = CoreX * 1e6/(2 * PAI * CoreFreq);
			}
			else {					// Ｃを計算
				CoreLC = -1e12/(2 * PAI * CoreFreq * CoreX);
			}
			break;
	}
	if( CoreL >= 1e-5 ){
		CoreFo = 299.8 * CoreP/ (4 * (CoreL * 0.01));
	}
	else {
		CoreFo = NULLV;
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::CFreqChange(TObject *Sender)
{
	if( !EnbCalc ) return;

	double d;
	if( Calc(d, AnsiString(CFreq->Text).c_str()) == TRUE ){
		if( d * 1e6 != CoreFreq ){
			CoreFreq = d * 1e6;
			CalcCore(0);
			SetValueItem(30);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::CTypeChange(TObject *Sender)
{
	if( !EnbCalc ) return;

	for( CoreType = 0; CorePT[CoreType]; CoreType++ ){
		if( !strcmp(AnsiString(CType->Text).c_str(), CoreTypeT[CoreType]) ) break;
	}
	if( !CorePT[CoreType] ) CoreType = 0;
	if( CoreType ){
		CoreP = CorePT[CoreType];
		CoreZ = CoreZT[CoreType];
		CalcCore(0);
		CEditP->Enabled = FALSE;
		CEditZ->Enabled = FALSE;
	}
	else {
		CEditP->Enabled = TRUE;
		CEditZ->Enabled = TRUE;
	}
	SetValueItem(31);
}
//---------------------------------------------------------------------------
// 短縮率
void __fastcall TOptDlgBox::CEditPChange(TObject *Sender)
{
	if( !EnbCalc ) return;

	double d;
	if( Calc(d, AnsiString(CEditP->Text).c_str()) == TRUE ){
		if( (CoreP != d) && (d > 0) ){
			CoreP = d;
			CalcCore(0);
			SetValueItem(35);
		}
	}
}
//---------------------------------------------------------------------------
// Ｚ
void __fastcall TOptDlgBox::CEditZChange(TObject *Sender)
{
	if( !EnbCalc ) return;

	double d;
	if( Calc(d, AnsiString(CEditZ->Text).c_str() ) == TRUE ){
		if( (CoreZ != d) && (d > 0) ){
			CoreZ = d;
			CalcCore(0);
			SetValueItem(36);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::CEditLChange(TObject *Sender)
{
	if( !EnbCalc ) return;

	double d;
	if( Calc(d, AnsiString(CEditL->Text).c_str()) == TRUE ){
		if( (CoreL != d) && (d >= 0) ){
			CoreL = d;
			CalcCore(3);
			SetValueItem(32);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::CEditXChange(TObject *Sender)
{
	if( !EnbCalc ) return;

	double d;
	if( Calc(d, AnsiString(CEditX->Text).c_str() ) == TRUE ){
		if( d != CoreX ){
			CoreX = d;
			CalcCore(1);
			SetValueItem(33);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::CEditLCChange(TObject *Sender)
{
	if( !EnbCalc ) return;

	double d;
	if( Calc(d, AnsiString(CEditLC->Text).c_str() ) == TRUE ){
		if( d != CoreLC ){
			CoreLC = d;
			CalcCore(0);
			SetValueItem(34);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::CStubClick(TObject *Sender)
{
	if( !EnbCalc ) return;

	CalcCore(3);
	SetValueItem(-1);
}
//---------------------------------------------------------------------------
// ZL -> Zi フィーダのインピーダンスの計算
//
//			   ZL cos(βL) + jZo sin(βL)
//	Zi = Zo * -------------------------
//			   Zo cos(βL) + jZL sin(βL)
//
//  β = 2π／λ
//
void __fastcall TOptDlgBox::CalcZi(double &Ri, double &Xi, double Zo, double L, double RL, double XL)
{
	if( L >= 1.0 ) L = fmod(L, 1.0);
	L *= 2 * PAI;
	double cosbl = cos(L);
	double sinbl = sin(L);

	CLX ZL; ZL = CLX(RL, XL);
	CLX Jo; Jo = CLX(0, 1) * Zo * sinbl;
	CLX	JL; JL = CLX(0, 1) * ZL * sinbl;
	CLX Zi; Zi = Zo * ((ZL * cosbl) + Jo)/((Zo * cosbl) + JL);
	Ri = Zi.r;
	Xi = Zi.j;
}
//---------------------------------------------------------------------------
// ZL -> Zi フィーダのインピーダンスの計算
//
//			   Zi cos(βL) - jZo sin(βL)
//	ZL = Zo * -------------------------
//			   Zo cos(βL) - jZi sin(βL)
//
//  β = 2π／λ
//
void __fastcall TOptDlgBox::CalcZL(double &RL, double &XL, double Zo, double L, double Ri, double Xi)
{
	if( L >= 1.0 ) L = fmod(L, 1.0);
	L *= 2 * PAI;
	double cosbl = cos(L);
	double sinbl = sin(L);

	CLX Zi; Zi = CLX(Ri, Xi);
	CLX Jo; Jo = CLX(0, 1) * Zo * sinbl;
	CLX	Ji; Ji = CLX(0, 1) * Zi * sinbl;
	CLX ZL; ZL = Zo * ((Zi * cosbl) - Jo)/((Zo * cosbl) - Ji);
	RL = ZL.r;
	XL = ZL.j;
}
//---------------------------------------------------------------------------
// ZL -> Zi フィーダのインピーダンスの計算（減衰を考慮）
//
//			   ZL cosh(γL) + Zo sinh(γL)
//	Zi = Zo * -------------------------
//			   Zo cosh(γL) + ZL sinh(γL)
//
//  γ = α＋jβ
//  β = 2π／λ
//
//  a  = αＬ
//
//  coshγＬ = coshαＬcosβＬ ＋ jsinhαＬsinβＬ
//  sinhγＬ = cosβＬsinhαＬ ＋ jsinβＬcoshαＬ
//
void __fastcall TOptDlgBox::CalcZi(double &Ri, double &Xi, double Zo, double L, double RL, double XL, double a)
{
	a /= 8.685889638;
	if( L >= 1.0 ) L = fmod(L, 1.0);
	L *= 2 * PAI;
	CLX cosHG; cosHG = CLX(cosh(a)*cos(L), sinh(a)*sin(L));
	CLX sinHG; sinHG = CLX(cos(L)*sinh(a), sin(L)*cosh(a));

	CLX ZL; ZL = CLX(RL, XL);
	CLX Zi; Zi = Zo * (ZL * cosHG + Zo * sinHG)/(Zo * cosHG + ZL * sinHG);
	Ri = Zi.r;
	Xi = Zi.j;
}
//---------------------------------------------------------------------------
// ZL -> Zi フィーダのインピーダンスの計算（減衰を考慮）
//
//			   Zi cosh(γL) - Zo sinh(γL)
//	ZL = Zo * -------------------------
//			   Zo cosh(γL) - Zi sinh(γL)
//
//
void __fastcall TOptDlgBox::CalcZL(double &RL, double &XL, double Zo, double L, double Ri, double Xi, double a)
{
	a /= 8.685889638;
	if( L >= 1.0 ) L = fmod(L, 1.0);
	L *= 2 * PAI;
	CLX cosHG; cosHG = CLX(cosh(a)*cos(L), sinh(a)*sin(L));
	CLX sinHG; sinHG = CLX(cos(L)*sinh(a), sin(L)*cosh(a));

	CLX Zi; Zi = CLX(Ri, Xi);
	CLX ZL; ZL = Zo * (Zi * cosHG - Zo * sinHG)/(Zo * cosHG - Zi * sinHG);
	RL = ZL.r;
	XL = ZL.j;
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::QEdRi1Change(TObject *Sender)
{
	if( !EnbCalc ) return;

	double d;
	if( Calc(d, AnsiString(QEdRi1->Text).c_str() ) == TRUE ){
		if( d != QMRi1 ){
			QMRi1 = d;
			CalcZL(QMRL1, QMXL1, QMZo, QML, QMRi1, QMXi1, QMA);
			CalcQMSWR();
			SetValueItem(40);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::QEdXi1Change(TObject *Sender)
{
	if( !EnbCalc ) return;

	double d;
	if( Calc(d, AnsiString(QEdXi1->Text).c_str() ) == TRUE ){
		if( d != QMXi1 ){
			QMXi1 = d;
			CalcZL(QMRL1, QMXL1, QMZo, QML, QMRi1, QMXi1, QMA);
			CalcQMSWR();
			SetValueItem(41);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::QEdRL1Change(TObject *Sender)
{
	if( !EnbCalc ) return;

	double d;
	if( Calc(d, AnsiString(QEdRL1->Text).c_str() ) == TRUE ){
		if( d != QMRL1 ){
			QMRL1 = d;
			CalcZi(QMRi1, QMXi1, QMZo, QML, QMRL1, QMXL1, QMA);
			CalcQMSWR();
			SetValueItem(42);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::QEdXL1Change(TObject *Sender)
{
	if( !EnbCalc ) return;

	double d;
	if( Calc(d, AnsiString(QEdXL1->Text).c_str() ) == TRUE ){
		if( d != QMXL1 ){
			QMXL1 = d;
			CalcZi(QMRi1, QMXi1, QMZo, QML, QMRL1, QMXL1, QMA);
			CalcQMSWR();
			SetValueItem(43);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::QEdZoChange(TObject *Sender)
{
	if( !EnbCalc ) return;

	double d;
	if( Calc(d, AnsiString(QEdZo->Text).c_str() ) == TRUE ){
		if( (d > 0.0) && (d != QMZo) ){
			QMZo = d;
			if( QMSel->Checked == TRUE ){
				CalcZL(QMRL1, QMXL1, QMZo, QML, QMRi1, QMXi1, QMA);
			}
			else {
				CalcZi(QMRi1, QMXi1, QMZo, QML, QMRL1, QMXL1, QMA);
			}
			CalcQMSWR();
			SetValueItem(44);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::QEdLChange(TObject *Sender)
{
	if( !EnbCalc ) return;

	double d;
	if( Calc(d, AnsiString(QEdL->Text).c_str() ) == TRUE ){
		if( d != QML ){
			QML = d;
			if( QMSel->Checked == TRUE ){
				CalcZL(QMRL1, QMXL1, QMZo, QML, QMRi1, QMXi1, QMA);
			}
			else {
				CalcZi(QMRi1, QMXi1, QMZo, QML, QMRL1, QMXL1, QMA);
			}
			CalcQMSWR();
			SetValueItem(45);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::QEdAChange(TObject *Sender)
{
	if( !EnbCalc ) return;

	double d;
	if( Calc(d, AnsiString(QEdA->Text).c_str() ) == TRUE ){
		if( d != QMA ){
			QMA = d;
			if( QMSel->Checked == TRUE ){
				CalcZL(QMRL1, QMXL1, QMZo, QML, QMRi1, QMXi1, QMA);
			}
			else {
				CalcZi(QMRi1, QMXi1, QMZo, QML, QMRL1, QMXL1, QMA);
			}
			CalcQMSWR();
			SetValueItem(46);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::CalcQML(void)
{
	double L;
	double Ri, Xi, Ri2, Xi2;
	double S;
	double MS = MAXDOUBLE;
	double AS = MAXDOUBLE;
	double ML = 0;
	for( L = 0; L <= 0.5; L += 0.01 ){
		CalcZi(Ri, Xi, QMZo2, L, QMRL2, QMXL2);
		CalcZi(Ri2, Xi2, QMZo1, L, Ri, Xi);
		S = CalcSWR(Ri2, Xi2, QMRi2, 0);
		if( S < MS ){
			MS = S;
			ML = L;
		}
		else if( AS < S ){
			if( MS < 1.5 ) break;
		}
		AS = S;
	}
	AS = MAXDOUBLE;
	double EL = ML + 0.01;
	for( L = ML - 0.01; L < EL; L += 0.00001 ){
		CalcZi(Ri, Xi, QMZo2, L, QMRL2, QMXL2);
		CalcZi(Ri2, Xi2, QMZo1, L, Ri, Xi);
		S = CalcSWR(Ri2, Xi2, QMRi2, 0);
		if( S < MS ){
			MS = S;
			ML = L;
		}
		else if( AS < S ){
			if( MS < 1.05 ) break;
		}
		AS = S;
	}
	if( ML < 0 ) ML = 0.5 + ML;
	QML1 = QML2 = ML;
	QMSWR2 = MS;
	for( int i = 0; (MS >= 1.001) && (i < 30); i++ ){
		CalcZi(Ri, Xi, QMZo2, QML2, QMRL2, QMXL2);
		ML = QML1;
		for( L = QML1+0.0001; L <= 0.5; L += 0.0001 ){
			CalcZi(Ri2, Xi2, QMZo1, L, Ri, Xi);
			S = CalcSWR(Ri2, Xi2, QMRi2, 0);
			if( S < MS ){
				MS = S;
				ML = L;
			}
			else {
				break;
			}
		}
		for( L = QML1-0.0001; L >= 0; L -= 0.0001 ){
			CalcZi(Ri2, Xi2, QMZo1, L, Ri, Xi);
			S = CalcSWR(Ri2, Xi2, QMRi2, 0);
			if( S < MS ){
				MS = S;
				ML = L;
			}
			else {
				break;
			}
		}
		if( ML < 0 ) ML = 0.5 + ML;
		QML1 = ML;
		QMSWR2 = MS;
		ML = QML2;
		for( L = QML2+0.0001; L <= 0.5; L += 0.0001 ){
			CalcZi(Ri, Xi, QMZo2, L, QMRL2, QMXL2);
			CalcZi(Ri2, Xi2, QMZo1, QML1, Ri, Xi);
			S = CalcSWR(Ri2, Xi2, QMRi2, 0);
			if( S < MS ){
				MS = S;
				ML = L;
			}
			else {
				break;
			}
		}
		for( L = QML2-0.0001; L >= 0; L -= 0.0001 ){
			CalcZi(Ri, Xi, QMZo2, L, QMRL2, QMXL2);
			CalcZi(Ri2, Xi2, QMZo1, QML1, Ri, Xi);
			S = CalcSWR(Ri2, Xi2, QMRi2, 0);
			if( S < MS ){
				MS = S;
				ML = L;
			}
			else {
				break;
			}
		}
		if( ML < 0 ) ML = 0.5 + ML;
		QML2 = ML;
		QMSWR2 = MS;
	}
}

void __fastcall TOptDlgBox::CalcQMSWR(void)
{
	double	Ri, Xi, Ri2, Xi2;

	QMSWR = CalcSWR(QMRi1, QMXi1, QMZo, 0);
	QMSWRL = CalcSWR(QMRL1, QMXL1, QMZo, 0);
	CalcZi(Ri, Xi, QMZo2, QML2, QMRL2, QMXL2);
	CalcZi(Ri2, Xi2, QMZo1, QML1, Ri, Xi);
	QMSWR2 = CalcSWR(Ri2, Xi2, QMRi2, 0);
}

void __fastcall TOptDlgBox::CopyQM(void)
{
	CalcZi(QMRL1, QMXL1, QMZo2, QML2, QMRL2, QMXL2);
	QMZo = QMZo1;
	QML = QML1;
	CalcZi(QMRi1, QMXi1, QMZo, QML, QMRL1, QMXL1, QMA);
}
//---------------------------------------------------------------------------

void __fastcall TOptDlgBox::QEdRi2Change(TObject *Sender)
{
	if( !EnbCalc ) return;

	double d;
	if( Calc(d, AnsiString(QEdRi2->Text).c_str() ) == TRUE ){
		if( d != QMRi2 ){
			QMRi2 = d;
			CopyQM();
			CalcQMSWR();
			SetValueItem(50);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::QEdRL2Change(TObject *Sender)
{
	if( !EnbCalc ) return;

	double d;
	if( Calc(d, AnsiString(QEdRL2->Text).c_str() ) == TRUE ){
		if( d != QMRL2 ){
			QMRL2 = d;
			CopyQM();
			CalcQMSWR();
			SetValueItem(51);
		}
	}

}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::QEdXL2Change(TObject *Sender)
{
	if( !EnbCalc ) return;

	double d;
	if( Calc(d, AnsiString(QEdXL2->Text).c_str() ) == TRUE ){
		if( d != QMXL2 ){
			QMXL2 = d;
			CopyQM();
			CalcQMSWR();
			SetValueItem(52);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::QEdZo1Change(TObject *Sender)
{
	if( !EnbCalc ) return;

	double d;
	if( Calc(d, AnsiString(QEdZo1->Text).c_str() ) == TRUE ){
		if( (d > 0.0) && (d != QMZo1) ){
			QMZo1 = d;
			if( QMZo1 == QMZo2 ){
				QMZo2 = (QMZo1 <= 60) ? 75 : 50;
			}
			CopyQM();
			CalcQMSWR();
			SetValueItem(53);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::QEdL1Change(TObject *Sender)
{
	if( !EnbCalc ) return;

	double d;
	if( Calc(d, AnsiString(QEdL1->Text).c_str() ) == TRUE ){
		if( d != QML1 ){
			QML1 = d;
			CopyQM();
			CalcQMSWR();
			SetValueItem(54);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::QEdZo2Change(TObject *Sender)
{
	if( !EnbCalc ) return;

	double d;
	if( Calc(d, AnsiString(QEdZo2->Text).c_str() ) == TRUE ){
		if( (d > 0.0) && (d != QMZo2) ){
			QMZo2 = d;
			if( QMZo1 == QMZo2 ){
				QMZo1 = (QMZo2 <= 60) ? 75 : 50;
			}
			CopyQM();
			CalcQMSWR();
			SetValueItem(55);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::QEdL2Change(TObject *Sender)
{
	if( !EnbCalc ) return;

	double d;
	if( Calc(d, AnsiString(QEdL2->Text).c_str() ) == TRUE ){
		if( d != QML2 ){
			QML2 = d;
			CopyQM();
			CalcQMSWR();
			SetValueItem(56);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::QMBtnClick(TObject *Sender)
{
	CalcQML();
	CopyQM();
	CalcQMSWR();
	SetValueItem(-1);
}
//---------------------------------------------------------------------------
double __fastcall TOptDlgBox::CalcSMStub(double L, int sw)
{
	CLX	Zi;
	CLX	Z;
	double	X;
	double	S;

	CalcZi(Zi.r, Zi.j, SMZo, L, SMR, SMX);
	if( Zi.j ){
		X = -(Zi.r * Zi.r + Zi.j * Zi.j)/Zi.j;
		Z = 1/((1/Zi) + (1/CLX(0,X)));
	}
	else {
		X = MAXDOUBLE;
		Z = Zi;
	}

	double LS, LO;
	LS = atan(X/SMZo)/(2 * PAI);
	if( LS < 0.0 ) LS += 0.5;
	if( !X ){
		LO = 0.5;
	}
	else {
		LO = atan(-SMZo/X)/(2*PAI);
	}
	if( LO < 0.0 ) LO += 0.5;
	S = CalcSWR(Z.r, Z.j, SMZi, 0);
	if( sw ){
		if( sw == 1 ){
			SML1 = L;
			SMR1 = Zi.r;
			SMX1 = Zi.j;
			SMLS1 = LS;
			SMLO1 = LO;
			SMXC1 = X;
			SMSWR1 = S;
			SMRi1 = Z.r;
			SMXi1 = Z.j;
		}
		else {
			SML2 = L;
			SMR2 = Zi.r;
			SMX2 = Zi.j;
			SMLS2 = LS;
			SMLO2 = LO;
			SMXC2 = X;
			SMSWR2 = S;
			SMRi2 = Z.r;
			SMXi2 = Z.j;
		}
	}
	return S;
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::CalcSM(void)
{
	double L;
	double AS = 9e100;
	double AL = NULLV;
	double S;
	SMF = 0;
	for( L = 0.0; L <= 0.5; L+=0.0001 ){
		S = CalcSMStub(L, 0);
		if( AS < S ){
			if( (AL!=NULLV) && (AS < 1.5) ){
				SMF++;
				S = CalcSMStub(AL, SMF);
			}
			AL = NULLV;
		}
		else {
			AL = L;
		}
		AS = S;
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::SMEditRChange(TObject *Sender)
{
	if( !EnbCalc ) return;

	double d;
	if( Calc(d, AnsiString(SMEditR->Text).c_str() ) == TRUE ){
		if( (d > 0.0) && (d != SMR) ){
			SMR = d;
			SMF = 0;
			SetValueItem(60);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::SMEditXChange(TObject *Sender)
{
	if( !EnbCalc ) return;

	double d;
	if( Calc(d, AnsiString(SMEditX->Text).c_str() ) == TRUE ){
		if( d != SMX ){
			SMX = d;
			SMF = 0;
			SetValueItem(61);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::SMEditZoChange(TObject *Sender)
{
	if( !EnbCalc ) return;

	double d;
	if( Calc(d, AnsiString(SMEditZo->Text).c_str() ) == TRUE ){
		if( (d > 0.0) && (d != SMZo) ){
			SMZo = d;
			SMF = 0;
			SetValueItem(62);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::SMEditCmChange(TObject *Sender)
{
	if( !EnbCalc ) return;

	double d;
	if( Calc(d, AnsiString(SMEditCm->Text).c_str() ) == TRUE ){
		if( (d > 0.0) && (d != SMCm) ){
			SMCm = d;
			SetValueItem(63);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::SMCalcBtnClick(TObject *Sender)
{
	CalcSM();
	SetValueItem(-1);
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::SMEditFreqChange(TObject *Sender)
{
	if( !EnbCalc ) return;

	double d;
	if( Calc(d, AnsiString(SMEditFreq->Text).c_str() ) == TRUE ){
		if( (d > 0.0) && (d != SMFreq) ){
			SMFreq = d;
			SetValueItem(64);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::SMEditZiChange(TObject *Sender)
{
	if( !EnbCalc ) return;

	double d;
	if( Calc(d, AnsiString(SMEditZi->Text).c_str() ) == TRUE ){
		if( (d > 0.0) && (d != SMZi) ){
			SMZi = d;
			SMF = 0;
			SetValueItem(65);
		}
	}
}
//---------------------------------------------------------------------------
// ヘアピンマッチを求める
void __fastcall TOptDlgBox::AutojXClick(TObject *Sender)
{
	double R, MRO;

	if( (Calc(R, AnsiString(EEditRO->Text).c_str()) == TRUE) && (R > 0.0) ){
		UnicodeString MTZ = R < 50 ? "50" : StrDbl(R*2); //ja7ude 1.0
		if( InputQuery("MMANA", "整合目標のZを入力", MTZ) == TRUE ){
			if( (Calc(MRO, AnsiString(MTZ).c_str()) == TRUE) && (MRO > R) ){
				EditJXO->Text = StrDbl(-(sqrt((MRO/R)-1) * R));
			}
			else {
				EditJXO->Text = (MRO == R) ? "0":"";
			}
		}
	}
}
//---------------------------------------------------------------------------
