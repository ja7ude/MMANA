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

//---------------------------------------------------------------------------
//#include <vcl.h>
#pragma hdrstop

#include "Main.h"
#include "MiniNec3.h"
#include "TextEdit.h"
#include "VerDsp.h"
#include "MediaDlg.h"
#include "ResCmp.h"
#include "GrpWire.h"
#include "ACalBox.h"
#include "ACalRes.h"
#include "OptDlg.h"
#include "WireScl.h"
#include "BwDisp.h"
#include "ValRep.h"
#include "MoveDlg.h"
#include "NearSet.h"
#include "FarSet.h"
#include "RotWire.h"
#include "WireCad.h"
#include "WireEdit.h"
#include "StackDlg.h"
#include "WComb.h"
#include "WcombDsp.h"
#include "FreqSet.h"
//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TMainWnd *MainWnd;

//---------------------------------------------------------------------------
static void LogPrint(char *ct, ...)
{
	va_list	pp;
	char	bf[256];

	va_start(pp, ct);
	vsprintf( bf, ct, pp );
	va_end(pp);
	MainWnd->CalMemo->Lines->Add(bf);
}

//---------------------------------------------------------------------------
__fastcall TMainWnd::TMainWnd(TComponent* Owner)
	: TForm(Owner)
{
	FirstInit = TRUE;
	int i;
	EntryAlignControl();
	pACal = NULL;
	pCalAnt = &ant;
	Application->OnIdle = OnIdle;

	for( int i = 0; FreqTbl[i] != NULL; i++ ){
		Freq->Items->Add(FreqTbl[i]);
	}
	DoFreq->Items->Assign(Freq->Items);
	if( ParamCount() >= 1 ){
		SetDirName(BgnDir, AnsiString(ParamStr(0)).c_str());	//ja7ude 1.0
	}
	else {
		SetCurDir(BgnDir, sizeof(BgnDir));
	}
	strcpy(AntDir, BgnDir);
	strcpy(ResDir, BgnDir);
	antSave = "";

	memset(&env, 0, sizeof(env));
	env.antheight = 20.0;
	env.mmax = 1;
	for( i = 0; i < EMAX; i++ ){
		env.rel[i] = 5.0;		// 誘電
		env.cond[i] = 1.0E-3;	// 導
		env.intval[i] = NULLV;
		env.height[i] = NULLV;
	}
	env.gscrcnt = 100;
	env.gscrr = 5;
	env.fbr = 120 + (60<<16);
	env.RO = 50.0;
	env.JXO = 0.0;
	env.WireRoss = 0;
	env.pmax = 1280;
	if( ::GetSystemMetrics(SM_CXFULLSCREEN) <= 650 ) env.FontSize = 1;
	WireSel->Text = "無損失";

	memset(&exeenv, 0, sizeof(exeenv));
	exeenv.AntXc = exeenv.AntYc = 0;
	exeenv.CalcDisp = 1;
	exeenv.rmd = 1.0;
	exeenv.Wave = 3;
	exeenv.BwMatch = 0;
	exeenv.IntPos = 1;
	exeenv.Ant3D = 1;
	exeenv.CurDir = 0;
	exeenv.FixFreeAngle = 1;
	exeenv.RecentMax = 4;
	exeenv.RecentMAA = 1;
	WaveSel->ItemIndex = exeenv.Wave;
	exeenv.WindowState = wsNormal;

	ResColors[0] = clBlack;	//	黒色
	ResColors[1] = clMaroon;	//	栗色
	ResColors[2] = clGreen;	//	緑色
	ResColors[3] = clOlive;	//	黄緑色
	ResColors[4] = clNavy;	//	濃紺色
	ResColors[5] = clPurple;	//	紫色
	ResColors[6] = clTeal;	//	暗青緑色
//clGray	灰色
//clSilver	銀色
	ResColors[7] = clRed;	//	赤色
	ResColors[8] = clLime;	//	濃緑色
	ResColors[9] = clBlue;	//	青色
//clFuchsia	赤紫色
//clAqua	空色

	ReadRegister();

	if( int(WindowState) != exeenv.WindowState ) WindowState = TWindowState(exeenv.WindowState);
	env.pmax = InitNEC(env.pmax);
	Ant3D->Checked = exeenv.Ant3D;
	UpdateRecentMenu();
	InitAntDef();
	antFname = "";
	DrawPtnH.SetRect(PBoxPtn->Canvas, 0, 0, PBoxPtn->Width / 2 - 2, PBoxPtn->Height);
	DrawPtnV.SetRect(PBoxPtn->Canvas, (PBoxPtn->Width/2) + 2, 0, PBoxPtn->Width, PBoxPtn->Height);
}

//---------------------------------------------------------------------------
// 終了時のデストラクタ
__fastcall TMainWnd::~TMainWnd(void)
{
	if( pACal != NULL ) delete pACal;
	if( pCalAnt != &ant ) delete pCalAnt;
	DeleteNEC();
}

//---------------------------------------------------------------------------
// アイドル処理  (done == FALSE then  no-message pump)
void __fastcall TMainWnd::OnIdle(TObject *Sender, bool &Done)
{
	if( exeenv.CalcF ){		// 浮動少数点エラーが発生した時
		exeenv.CalcF = 0;
		DisCalcUI();
	}
	switch(CalcSel->ItemIndex){
		case 0:
			AntHeight->Enabled = FALSE;
			Label11->Enabled = FALSE;
			GndSetBtn->Visible = FALSE;
			break;
		case 1:
			AntHeight->Enabled = TRUE;
			Label11->Enabled = TRUE;
			GndSetBtn->Visible = FALSE;
			break;
		case 2:
			AntHeight->Enabled = TRUE;
			Label11->Enabled = TRUE;
			GndSetBtn->Visible = TRUE;
			break;
	}
	if( DspCur->Checked && res.IsCalc() ){
		TBarCur->Enabled = TRUE;
	}
	else {
		TBarCur->Enabled = FALSE;
	}
	Screen->Cursor = crDefault;
#if 0
	if( pWaitControl != NULL ){
		RestWaitCursor();
	}
#endif
	if( res.IsCalc() ){	// 計算結果保存
		K11->Enabled = TRUE;
		KC2->Enabled = TRUE;		// 電流ファイルの作成
		K31->Enabled = TRUE;		// 近距離電界強度ファイルの作成
		K32->Enabled = TRUE;		// 遠距離電界強度ファイルの作成
		K49->Enabled = TRUE;		// 周波数特性ファイルの作成
	}
	else {
		K11->Enabled = FALSE;
		KC2->Enabled = FALSE;
		K31->Enabled = FALSE;		// 近距離電界強度ファイルの作成
		K32->Enabled = FALSE;		// 遠距離電界強度ファイルの作成
		K49->Enabled = FALSE;		// 周波数特性ファイルの作成
	}
	bool f = FALSE;
	if( Page->ActivePage == TabSheet1 ){
#if 1
		if( (ActiveControl == Grid2)||(ActiveControl == Grid3)||(ActiveControl == Grid4) ){
			f = TRUE;
		}
#else
		if( ActiveControl == Grid2){
			f = TRUE;
			if( Grid2->Color != clWhite ) Grid2->Color = clWhite;
		}
		else {
			if( Grid2->Color != clBtnFace ) Grid2->Color = clBtnFace;
		}
		if( ActiveControl == Grid3){
			f = TRUE;
			if( Grid3->Color != clWhite ) Grid3->Color = clWhite;
		}
		else {
			if( Grid3->Color != clBtnFace ) Grid3->Color = clBtnFace;
		}
		if( ActiveControl == Grid4){
			f = TRUE;
			if( Grid4->Color != clWhite ) Grid4->Color = clWhite;
		}
		else {
			if( Grid4->Color != clBtnFace ) Grid4->Color = clBtnFace;
		}
#endif
	}
	if( (f == TRUE) && (ActiveControl == Grid2) && (Grid2->Row <= ant.wmax) ){
		K27->Enabled = TRUE;		// 検索と置換
		KR1->Enabled = TRUE;		// 検索と置換
		K35->Enabled = TRUE;		// 始点と終点の入れ替え
	}
	else {
		K27->Enabled = FALSE;		// 検索と置換
		KR1->Enabled = FALSE;		// 検索と置換
		K35->Enabled = FALSE;		// 始点と終点の入れ替え
	}
	K7->Enabled = f;
	K9->Enabled = f;
	K13->Enabled = f;
	K14->Enabled = f;
	f = FALSE;
	if( (Page->ActivePage == TabSheet1)||(Page->ActivePage == TabSheet2) ){
		if( Grid2->Row <= ant.wmax ) f = TRUE;
	}
	KT1->Enabled = f;		// 三角編集
	KT2->Enabled = f;		// 三角編集
	KT3->Enabled = f;		// 三角編集
	K28->Enabled = f;		// 平行移動
	KM1->Enabled = f;		// 平行移動
	KM2->Enabled = f;		// 平行移動
	KS5->Enabled = f;		// 始点と終点の入れ替え
	if( f ){
		WDEF *wp = &ant.wdef[Grid2->Row-1];
		f = FALSE;
		if( wp->R < 0.0 ){
			if( FindPP(&ant, wp->R) != NULL ){
				f = TRUE;
			}
		}
	}
	K47->Enabled = f;		// 組み合わせワイヤ展開表示
	K48->Enabled = f;		// 組み合わせワイヤ展開表示
	if( ant.wmax ){
		K4->Enabled = TRUE;
		K5->Enabled = TRUE;
		AllViewBtn->Enabled = TRUE;
		if( ant.cmax ){
			CalTrgBtn->Enabled = TRUE;
			ACalBtn->Enabled = TRUE;
			K8->Enabled = TRUE;
		}
		else {
			CalTrgBtn->Enabled = FALSE;
			ACalBtn->Enabled = FALSE;
			K8->Enabled = FALSE;
		}
		if( (pACal == NULL) || (!pACal->GetMV()) ){
			ACalResBtn->Enabled = FALSE;
			K15->Enabled = FALSE;
		}
		else {
			ACalResBtn->Enabled = TRUE;
			K15->Enabled = TRUE;
		}
		if( (pACal == NULL) || (!pACal->amax) ){
			K20->Enabled = FALSE;	// 最適化シートを保存
		}
		else {
			K20->Enabled = TRUE;	// 最適化シートを保存
		}
		KXY->Enabled = TRUE;		// 座標の交換
		KS4->Enabled = TRUE;		// 拡大・縮小
		K22->Enabled = TRUE;		// 丸める
		K33->Enabled = TRUE;		// 全ワイヤの回転
		K37->Enabled = TRUE;        // スタックの展開
		StackBtn->Enabled = TRUE;        // スタックの展開
		K38->Enabled = TRUE;	// 記憶
	}
	else {
		K4->Enabled = FALSE;
		K5->Enabled = FALSE;
		AllViewBtn->Enabled = FALSE;
		CalTrgBtn->Enabled = FALSE;
		ACalBtn->Enabled = FALSE;
		ACalResBtn->Enabled = FALSE;
		K8->Enabled = FALSE;
		K15->Enabled = FALSE;
		K20->Enabled = FALSE;	// 最適化シートを保存
		KXY->Enabled = FALSE;	// 座標の交換
		KS4->Enabled = FALSE;	// 拡大・縮小
		K22->Enabled = FALSE;	// 丸める
		K33->Enabled = FALSE;	// 全ワイヤの回転
		K37->Enabled = FALSE;	// スタックの展開
		StackBtn->Enabled = FALSE;        // スタックの展開
		K38->Enabled = FALSE;	// 記憶
	}
	K36->Enabled = antDef.IsEmpty() ? FALSE : TRUE;	// 最適化前状態に戻す
	K39->Enabled = antSave.IsEmpty() ? FALSE : TRUE;	// 記憶した状態に戻す
	TBarZDeg->Enabled = exeenv.Ant3D ? TRUE : FALSE;
	if( res.Bwc.bcnt ){
		BwBtn->Enabled = TRUE;	// 周波数特性ボタン
	}
	else {
		BwBtn->Enabled = FALSE;	// 周波数特性ボタン
	}
	if( res.PtnF ){
		PrintBtn->Enabled = TRUE;	// 印刷ボタン
		EleBtn->Enabled = TRUE;		// 仰角ボタン
	}
	else {
		PrintBtn->Enabled = FALSE;	// 印刷ボタン
		EleBtn->Enabled = FALSE;		// 仰角ボタン
	}
	if( exeenv.RmdSel != ChkRmd->Checked ){
		ChkRmd->Checked = exeenv.RmdSel;
		Grid2->Invalidate();
	}
	if( exeenv.pCurFile != NULL ){
		delete exeenv.pCurFile;
		exeenv.pCurFile = NULL;
	}
	if( (exeenv.pNearFile != NULL) && (exeenv.pNearFile->fp!=NULL) ){
		fclose(exeenv.pNearFile->fp);
		delete exeenv.pNearFile;
		exeenv.pNearFile = NULL;
	}
	if( (exeenv.pFarFile != NULL) && (exeenv.pFarFile->fp!=NULL) ){
		fclose(exeenv.pFarFile->fp);
		delete exeenv.pFarFile;
		exeenv.pFarFile = NULL;
	}
	if( antRem.IsEmpty() ){
		DocBtn->Visible = FALSE;
	}
	else {
		DocBtn->Visible = TRUE;
	}
	if( exeenv.Ant3D != Ant3D->Checked ){
		Ant3D->Checked = exeenv.Ant3D;
		AllViewBtnClick(NULL);
	}
	KMMANAWebW1->Enabled = WebRef.IsHTML();
}
//---------------------------------------------------------------------------
// ウイエトカーソルを設定する
void __fastcall TMainWnd::SetWaitCursor(void)
{
	Screen->Cursor = crHourGlass;
}
//---------------------------------------------------------------------------
// レジストリのコピー
void __fastcall TMainWnd::RegCopy(void)
{
	char	fm1[256];
	FILE	*fp;

	::GetWindowsDirectory(fm1, 222);
	if( LastC(fm1) != '\\' ) strcat(fm1, "\\");
	strcat(fm1, "MMANA.INI");
	if( (fp = fopen(fm1, "rb"))!=NULL ){		// C:\Windows\MMANA.iniが存在する時
		fclose(fp);
		char	fm2[256];
		sprintf(fm2, "%sMmana.ini", BgnDir);
		::CopyFile(fm1, fm2, TRUE);
		unlink(fm1);
	}
}
//---------------------------------------------------------------------------
// レジストリからの読み出し
void __fastcall TMainWnd::ReadRegister(void)
{
	RegCopy();
	char	bf[256];
	int i;

	sprintf(bf, "%sMmana.ini", BgnDir);
	TIniFile	*pIniFile = new TIniFile(bf);

// ディレクトリ環境
	AnsiString	as;
	as = pIniFile->ReadString("Directory", "ANT", AntDir);
	strncpy(AntDir, as.c_str(), sizeof(AntDir));
	as = pIniFile->ReadString("Directory", "RES", ResDir);
	strncpy(ResDir, as.c_str(), sizeof(ResDir));

// 計算環境
	env.type = pIniFile->ReadInteger("Env", "Type", env.type);
	env.antheight = ReadDoubleIniFile(pIniFile, "Env", "AntHeight", env.antheight);
	env.mmax = pIniFile->ReadInteger("Env", "MediaMax", env.mmax);
	env.mtype = pIniFile->ReadInteger("Env", "MediaType", env.mtype);
	env.gscrcnt = pIniFile->ReadInteger("Env", "RadialCount", env.gscrcnt);
	env.gscrr = ReadDoubleIniFile(pIniFile, "Env", "RadialRadius", env.gscrr);
	for( i = 0; i < EMAX; i++ ){
		sprintf(bf, "Dieleciric%d", i+1);
		env.rel[i] = ReadDoubleIniFile(pIniFile, "Env", bf, env.rel[i] );
		sprintf(bf, "Conductivity%d", i+1);
		env.cond[i] = ReadDoubleIniFile(pIniFile, "Env", bf, env.cond[i] );
		sprintf(bf, "Coord%d", i+1);
		env.intval[i] = ReadDoubleIniFile(pIniFile, "Env", bf, env.intval[i] );
		sprintf(bf, "Height%d", i+1);
		env.height[i] = ReadDoubleIniFile(pIniFile, "Env", bf, env.height[i] );
	}
	env.pmax = pIniFile->ReadInteger("Env", "PMAX", env.pmax);
	env.FontSize = pIniFile->ReadInteger("Env", "FontSize", env.FontSize);
// 表示
	exeenv.Ant3D = pIniFile->ReadInteger("Job", "Ant3D", exeenv.Ant3D);
	exeenv.IntPos = pIniFile->ReadInteger("Job", "IntPos", exeenv.IntPos);
	exeenv.CurDir = pIniFile->ReadInteger("Job", "CurDir", exeenv.CurDir);
	exeenv.FixFreeAngle = pIniFile->ReadInteger("Job", "FixFreeAngle", exeenv.FixFreeAngle);
	exeenv.WindowState = pIniFile->ReadInteger("Job", "WindowState", exeenv.WindowState);

// 最新のファイル
	exeenv.RecentMax = pIniFile->ReadInteger("Recent File", "Max", exeenv.RecentMax);
	exeenv.RecentMAA = pIniFile->ReadInteger("Recent File", "OnlyMAA", exeenv.RecentMAA);
	for( i = 0; i < RECMENUMAX; i++ ){
		sprintf(bf, "File%d", i + 1);
		as = pIniFile->ReadString("Recent File", bf, "");
		RecentMenu.SetItemText(i, as.c_str());
	}

	delete pIniFile;
}

//---------------------------------------------------------------------------
// レジストリへの書き込み
void __fastcall TMainWnd::WriteRegister(void)
{
	int i;
	char	bf[256];
	sprintf(bf, "%sMmana.ini", BgnDir);
	TIniFile	*pIniFile = new TIniFile(bf);

// ディレクトリ環境
	pIniFile->WriteString("Directory", "ANT", AntDir);
	pIniFile->WriteString("Directory", "RES", ResDir);
// 計算環境
	pIniFile->WriteInteger("Env", "Type", env.type);
	WriteDoubleIniFile(pIniFile, "Env", "AntHeight", env.antheight);
	pIniFile->WriteInteger("Env", "MediaMax", env.mmax);
	pIniFile->WriteInteger("Env", "MediaType", env.mtype);
	pIniFile->WriteInteger("Env", "RadialCount", env.gscrcnt);
	WriteDoubleIniFile(pIniFile, "Env", "RadialRadius", env.gscrr);
	for( i = 0; i < EMAX; i++ ){
		sprintf(bf, "Dieleciric%d", i+1);
		WriteDoubleIniFile(pIniFile, "Env", bf, env.rel[i] );
		sprintf(bf, "Conductivity%d", i+1);
		WriteDoubleIniFile(pIniFile, "Env", bf, env.cond[i] );
		sprintf(bf, "Coord%d", i+1);
		WriteDoubleIniFile(pIniFile, "Env", bf, env.intval[i] );
		sprintf(bf, "Height%d", i+1);
		WriteDoubleIniFile(pIniFile, "Env", bf, env.height[i] );
	}
	pIniFile->WriteInteger("Env", "PMAX", env.pmax);
	pIniFile->WriteInteger("Env", "FontSize", env.FontSize);
// 表示
	pIniFile->WriteInteger("Job", "Ant3D", exeenv.Ant3D);
	pIniFile->WriteInteger("Job", "IntPos", exeenv.IntPos);
	pIniFile->WriteInteger("Job", "CurDir", exeenv.CurDir);
	pIniFile->WriteInteger("Job", "FixFreeAngle", exeenv.FixFreeAngle);
	pIniFile->WriteInteger("Job", "WindowState", WindowState);
// 最新のファイル
	pIniFile->WriteInteger("Recent File", "Max", exeenv.RecentMax);
	pIniFile->WriteInteger("Recent File", "OnlyMAA", exeenv.RecentMAA);
	for( i = 0; i < RecentMenu.GetCount(); i++ ){
		sprintf(bf, "File%d", i + 1);
		pIniFile->WriteString("Recent File", bf, RecentMenu.GetItemText(i));
	}
	delete pIniFile;
}

//---------------------------------------------------------------------------
// アンテナ定義アイテムの表示
void __fastcall TMainWnd::UpdateCount(void)
{
	char bf[128];

	sprintf(bf, "Wire %u本", ant.wmax);
	WireCnt->Caption = bf;
	sprintf(bf, "給電点 %u個", ant.cmax);
	CurCnt->Caption = bf;
	sprintf(bf, "集中定数 %u個", ant.lmax);
	LoadCnt->Caption = bf;

	if( (ant.StackH==1)&&(ant.StackV==1) ){
		strcpy(bf,"スタック");
	}
	else {
		sprintf(bf, "ｽﾀｯｸ %ux%u", ant.StackH, ant.StackV);
	}
	StackBtn->Caption = bf;
}
//---------------------------------------------------------------------------
// ウインドウ内のすべてのアイテムを更新する
void __fastcall TMainWnd::UpdateAllViews(void)
{
	UpdateCount();
	Invalidate();
	Grid1->Invalidate();
	Grid2->Invalidate();
	Grid3->Invalidate();
	Grid4->Invalidate();
	PBoxAnt->Invalidate();
	PBoxPtn->Invalidate();
}
//---------------------------------------------------------------------------
// アンテナデータを初期化する
void __fastcall TMainWnd::InitAntDef(void)
{
	if( pACal != NULL ){
		delete pACal;
		pACal = NULL;
	}
	antRem = "";
	antDef = "";
	memset(&ant, 0, sizeof(ant));
	ant.cauto = 1;
	ant.lenb = 1;
	ant.fq = 14.050;
	ant.DM1 = 400;
	ant.DM2 = 40;
	ant.EC = 1;
	ant.SC = 2.0;
	ant.StackVT = 0;
	ant.StackH = ant.StackV = 1;
	exeenv.rmd = 299.8/ant.fq;
    exeenv.EleWidthAll = 0;
	ant.StackHW = ant.StackVW = exeenv.rmd;
	env.fbr = 120 + (60<<16);
	env.RO = 50.0;
	env.JXO = 0.0;
	res.ClearBWC();
	for( int i = 0; i < LMAX; i++ ){
		for( int j = 0; j < SMAX; j++ ){
			ant.ldef[i].A[j] = NULLV;
			ant.ldef[i].B[j] = NULLV;
		}
	}
	if( pCalAnt != &ant ) delete pCalAnt;
	pCalAnt = &ant;
	SetAntDef();
	UpdateCount();

#if 0
	ant.pdef[0].RR = -0.001;
	ant.pdef[0].Type = 0;
	ant.pdef[0].L[0] = 2;
	ant.pdef[0].R[0] = 0.015;
	ant.pdef[0].L[1] = 1.5;
	ant.pdef[0].R[1] = 0.010;
	ant.pdef[0].L[2] = 1.5;
	ant.pdef[0].R[2] = 0.005;
	ant.pdef[0].L[3] = 1.5;
	ant.pdef[0].R[3] = 0.005;
	ant.pdef[0].L[4] = 1.5;
	ant.pdef[0].R[4] = 0.005;
#endif
}

void __fastcall TMainWnd::SetAntDef(void)
{
	char	bf[64];

	AntName->Text = ant.Name;
	AntName2->Caption = ant.Name;
	AntName3->Caption = ant.Name;
	AntName4->Caption = res.Name;
	if( ant.fq ){
		sprintf( bf, "%.3lf", ant.fq );
	}
	else {
		bf[0] = 0;
	}
	exeenv.rmd = 299.8/ant.fq;
	Freq->Text = bf;
	DoFreq->Text = bf;
	sprintf(bf, "%u", ant.DM1);
	EditDM1->Text = bf;
	sprintf(bf, "%u", ant.DM2);
	EditDM2->Text = bf;
	sprintf(bf, "%u", ant.EC);
	EditEC->Text = bf;
	EditSC->Text = StrDbl(ant.SC);
	Grid2->RowCount = ant.wmax + 2;
	Grid3->RowCount = ant.cmax + 2;
	Grid4->RowCount = ant.lmax + 2;
	AutoVol->Checked = ant.cauto;
	EnbLoad->Checked = ant.lenb;
	if( *antFname.c_str() ){
		sprintf(bf,"MMANA - %s", antFname.c_str() );
	}
	else {
		strcpy(bf, "MMANA - 無題");
	}
	MainWnd->Caption = bf;

	CalcSel->ItemIndex = env.type;
	AntHeight->Text = StrDbl(env.antheight);
	WireSel->Text = WireSel->Items->Strings[env.WireRoss];
}

void __fastcall TMainWnd::GetAntDef(void)
{
	int		i, j;

	strncpy(ant.Name, AnsiString(AntName->Text).c_str(), sizeof(ant.Name));	//ja7ude 1.0
	double	d;
	Calc(d, AnsiString(Freq->Text).c_str());		//ja7ude 1.0
	if( d > 0 ) ant.fq = d;
	Calc(d, AnsiString(DoFreq->Text).c_str());      //ja7ude 1.0
	if( d > 0 ) ant.cfq = d;
	CalcU(i, AnsiString(EditDM1->Text).c_str());    //ja7ude 1.0
	if( i > 0 ) ant.DM1 = i;
	CalcU(i, AnsiString(EditDM2->Text).c_str());    //ja7ude 1.0
	if( i > 0 ) ant.DM2 = i;
	CalcU(i, AnsiString(EditEC->Text).c_str());     //ja7ude 1.0
	if( (i > 0) && (i < 128) ){
		ant.EC = i;
	}
	else {
		EditEC->Text = ant.EC;
	}
	Calc(d, AnsiString(EditSC->Text).c_str());	//ja7ude 1.0
	if( (d > 1.0) && (d <= 3.0) ){
		ant.SC = d;
	}
	else {
		EditSC->Text = StrDbl(ant.SC);
	}

	ant.cauto = AutoVol->Checked;
	ant.lenb = EnbLoad->Checked;
	for( i = 0; i < ant.lmax; i++ ){
		if( ant.ldef[i].TYPE == 2 ){
			for( j = SMAX-1; j >= 0; j-- ){
				if( (ant.ldef[i].A[j] != NULLV) || (ant.ldef[i].B[j] != NULLV) ) break;
			}
			ant.ldef[i].SN = j+1;
		}
	}

	env.type = CalcSel->ItemIndex;
	Calc(env.antheight, AnsiString(AntHeight->Text).c_str());               //ja7ude 1.0

	if( !strcmp(AnsiString(WireSel->Text).c_str(), "無損失") ){             //ja7ude 1.0
		env.WireRoss = 0;
	}
	else if( !strcmp(AnsiString(WireSel->Text).c_str(), "銅線") ){          //ja7ude 1.0
		env.WireRoss = 1;
	}
	else if( !strcmp(AnsiString(WireSel->Text).c_str(), "銅パイプ") ){      //ja7ude 1.0
		env.WireRoss = 2;
	}
	else if( !strcmp(AnsiString(WireSel->Text).c_str(), "アルミ線") ){      //ja7ude 1.0
		env.WireRoss = 3;
	}
	else if( !strcmp(AnsiString(WireSel->Text).c_str(), "アルミパイプ") ){  //ja7ude 1.0
		env.WireRoss = 4;
	}
	else if( !strcmp(AnsiString(WireSel->Text).c_str(), "鉄線") ){          //ja7ude 1.0
		env.WireRoss = 5;
	}
	else if( !strcmp(AnsiString(WireSel->Text).c_str(), "鉄パイプ") ){      //ja7ude 1.0
		env.WireRoss = 6;
	}
	else {
		env.WireRoss = 0;
		WireSel->Text = "無損失";
	}
}
//---------------------------------------------------------------------------
// ワイヤー定義のセル位置のテキストを返す
void __fastcall TMainWnd::Grid2GetText(LPSTR t, long Col, long Row)
{
	*t = 0;
	if( Row ){
		Row--;
		switch(Col){
			case 0:
				if( Row < ant.wmax ){
					sprintf( t, "%3d", Row+1);
				}
				else {
					strcpy(t, "新規");
				}
				break;
			case 1:		// X1(m)
				if( Row < ant.wmax ){
					strcpy(t, StrDbl(GetRmdVal(ant.wdef[Row].X1)));
				}
				break;
			case 2:		// Y1(m)
				if( Row < ant.wmax ){
					strcpy(t, StrDbl(GetRmdVal(ant.wdef[Row].Y1)));
				}
				break;
			case 3:		// Z1(m)
				if( Row < ant.wmax ){
					strcpy(t, StrDbl(GetRmdVal(ant.wdef[Row].Z1)));
				}
				break;
			case 4:		// X2(m)
				if( Row < ant.wmax ){
					strcpy(t, StrDbl(GetRmdVal(ant.wdef[Row].X2)));
				}
				break;
			case 5:		// Y2(m)
				if( Row < ant.wmax ){
					strcpy(t, StrDbl(GetRmdVal(ant.wdef[Row].Y2)));
				}
				break;
			case 6:		// Z2(m)
				if( Row < ant.wmax ){
					strcpy(t, StrDbl(GetRmdVal(ant.wdef[Row].Z2)));
				}
				break;
			case 7:		// R(cm)
				if( Row < ant.wmax ){
					strcpy(t, StrDbl(GetRmdValD(ant.wdef[Row].R*1000.0)));
				}
				break;
			case 8:		// SEG
				if( Row < ant.wmax ){
					strcpy(t, Seg2Str(ant.wdef[Row].SEG));
				}
				break;
		}
	}
}

//---------------------------------------------------------------------------
// 給電定義のセル位置のテキストを返す
void __fastcall TMainWnd::Grid3GetText(LPSTR t, long Col, long Row)
{
	if( Row ){
		Row--;
		*t = 0;
		switch(Col){
			case 0:
				if( Row < ant.cmax ){
					sprintf( t, "%3d", Row+1);
				}
				else {
					strcpy(t, "新規");
				}
				break;
			case 1:		// PLUS
				if( Row < ant.cmax ){
					sprintf( t, "%s", ant.cdef[Row].PLUS );
				}
				break;
			case 2:		// 位相
				if( Row < ant.cmax ){
					strcpy(t, StrDbl(ant.cdef[Row].DEG));
				}
				break;
			case 3:		// 電圧
				if( Row < ant.cmax ){
					strcpy(t, StrDbl(ant.cdef[Row].EV));
				}
				break;
		}
	}
}

//---------------------------------------------------------------------------
// ロード定義のセル位置のテキストを返す
void __fastcall TMainWnd::Grid4GetText(LPSTR t, long Col, long Row)
{
	LPCSTR	_ttp[]={"LC","R+jX","S"};

	if( Row ){
		Row--;
		*t = 0;
		switch(Col){
			case 0:
				if( Row < ant.lmax ){
					sprintf( t, "%3d", Row+1);
				}
				else {
					strcpy(t, "新規");
				}
				break;
			case 1:		// パルス
				if( Row < ant.lmax ){
					sprintf(t, "%s", ant.ldef[Row].PLUS );
				}
				break;
			case 2:		// 種類
				if( Row < ant.lmax ){
					strcpy(t, _ttp[ant.ldef[Row].TYPE] );
				}
				break;
			case 3:		// S
				if( Row < ant.lmax ){
					strcpy(t, StrDblE(ant.ldef[Row].A[0]));
				}
				break;
			case 4:		// B0
				if( Row < ant.lmax ){
					strcpy(t, StrDblE(ant.ldef[Row].B[0]));
				}
				break;
			case 5:		// A1
				if( Row < ant.lmax ){
					if( ant.ldef[Row].TYPE != 1 ){
						strcpy(t, StrDblE(ant.ldef[Row].A[1]));
					}
				}
				break;
			case 6:		// B1
				if( Row < ant.lmax ){
					switch(ant.ldef[Row].TYPE){
						case 0:		// L,C,Q - Freq
							if( ant.ldef[Row].A[0] && ant.ldef[Row].B[0] ){
								double f = 1.0 / (PAI2 * sqrt(ant.ldef[Row].A[0] * ant.ldef[Row].B[0] * 1.0e-18));
								strcpy(t, StrDblE(f*1.0e-6));
							}
							break;
						case 1:
							break;
						case 2:		// S-DOMAIN
							strcpy(t, StrDblE(ant.ldef[Row].B[1]));
							break;
					}
				}
				break;
			default:
				if( Row < ant.lmax ){
					if( ant.ldef[Row].TYPE == 2 ){
						Col = (Col - 3);
						if( Col & 1 ){
							strcpy(t, StrDblE(ant.ldef[Row].B[Col/2]));
						}
						else {
							strcpy(t, StrDblE(ant.ldef[Row].A[Col/2]));
						}
					}
				}
				break;
		}
	}
}
//---------------------------------------------------------------------------
// 結果のグリッド表示
void __fastcall TMainWnd::Grid1DrawCell(TObject *Sender, int Col, int Row,
	TRect &Rect, TGridDrawState State)
{
	LPCSTR	_ty[]={"自由","完全GND","ﾘｱﾙGND"};
	char	bf[64];

	Grid1->Canvas->FillRect(Rect);
	int X = Rect.Left + 4;
	int Y = Rect.Top + 2;

	if( Row ){
		Row--;
		bf[0] = 0;
		RESL	*rp = res.GetResP(Row);
		double	g;
		if( Row >= res.GetCnt() ){
			return;
		}
		switch(Col){
			case 0:
				sprintf( bf, "%d", res.GetCnt() - Row );
				break;
			case 1:
				if( rp->FQ != NULLF ) strcpy(bf, StrDbl3(rp->FQ));
				break;
			case 2:
				if( rp->R != NULLF ) strcpy(bf, StrDbl3(rp->R));
				break;
			case 3:
				if( rp->JX != NULLF ) strcpy(bf, StrDbl3(rp->JX));
				break;
			case 4:
				if( rp->R != NULLF ){
					if( (rp->RO != env.RO)||(rp->JXO != env.JXO) ){
						strcpy(bf, StrDbl2(CalcSWR(rp->R, rp->JX, env.RO, env.JXO)));
					}
					else {
						strcpy(bf, StrDbl2(rp->SWR));
					}
				}
				break;
			case 5:
				if( rp->GAIN != NULLF ){
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
			case 6:
				if( rp->GAIN != NULLF ){
					g = rp->GAIN;
					if( g < -999 ) g = -999;
					strcpy(bf, StrDbl2(g));
				}
				break;
			case 7:
				if( rp->FB != NULLF ) strcpy(bf, StrDbl2(rp->FB));
				break;
			case 8:
				if( rp->ELV != NULLF ) strcpy(bf, StrDbl1(rp->ELV));
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
				if( rp->MAXVP != NULLF ){
					strcpy(bf, (rp->MAXVP > rp->MAXHP) ? "垂直":"水平");
				}
				break;
		}
		Grid1->Canvas->TextOut(X, Y, bf);
	}
	else {		// タイトル
		LPCSTR	_tt[]={
			"No.","Freq MHz","R (Ω)","jX (Ω)","SWR 50","Gh dBd","Ga dBi","F/B dB","Elev.°","条件","地上高","偏波"
		};
		if( Col == 4 ){
			sprintf(bf, "SWR %.lf%s", env.RO, env.JXO ? "*":"");
			Grid1->Canvas->TextOut(X, Y, bf);
		}
		else {
			Grid1->Canvas->TextOut(X, Y, _tt[Col]);
		}
	}
}

//---------------------------------------------------------------------------
// ワイヤー定義のグリッド表示
/*
void __fastcall TForm1::StringGrid1DrawCell(TObject *Sender, int ACol,
      int ARow, TRect &Rect, TGridDrawState State)
*/

void __fastcall TMainWnd::Grid2DrawCell(TObject *Sender, int Col,     //JA7UDE 1.0
	int Row, TRect &Rect, TGridDrawState State)
{
	char	bf[64];

	if( (Row >= 1) && (Col >= 1) && (ActiveControl != Grid2) ){
		Grid2->Canvas->Brush->Color = clWindow;
		Grid2->Canvas->Font->Color = clBlack;
	}
	Grid2->Canvas->FillRect(Rect);
	int X = Rect.Left + 1;
	int Y = Rect.Top + 1;

	if( Row ){
		Grid2GetText(bf, Col, Row);
		Grid2->Canvas->TextOut(X, Y, bf);
	}
	else {		// タイトル
		LPCSTR	_tt1[]={
			"No.","X1(m)","Y1(m)","Z1(m)","X2(m)","Y2(m)","Z2(m)","R(mm)","Seg."
		};
		LPCSTR	_tt2[]={
			"No.","X1(λ)","Y1(λ)","Z1(λ)","X2(λ)","Y2(λ)","Z2(λ)","R(λ)","Seg."
		};
		X += 8;
		Grid2->Canvas->TextOut(X, Y, exeenv.RmdSel ? _tt2[Col] : _tt1[Col]);
	}
}
//---------------------------------------------------------------------------
// 給電点定義 グリッド表示
void __fastcall TMainWnd::Grid3DrawCell(TObject *Sender, int Col, int Row,
	TRect &Rect, TGridDrawState State)
{
	char	bf[64];

	if( (Row >= 1) && (Col >= 1) && (ActiveControl != Grid3) ){
		Grid3->Canvas->Brush->Color = clWindow;
		Grid3->Canvas->Font->Color = clBlack;
	}
	Grid3->Canvas->FillRect(Rect);
	int X = Rect.Left + 1;
	int Y = Rect.Top + 1;

	if( Row ){
		Grid3GetText(bf, Col, Row);
		Grid3->Canvas->TextOut(X, Y, bf);
	}
	else {		// タイトル
		LPCSTR	_tt[]={
			"No.","PULSE","位相(°)","電圧(V)"
		};
		X += 8;
		Grid3->Canvas->TextOut(X, Y, _tt[Col]);
	}
}
//---------------------------------------------------------------------------
// ロード定義 グリッド表示
void __fastcall TMainWnd::Grid4DrawCell(TObject *Sender, int Col, int Row,
	TRect &Rect, TGridDrawState State)
{
	char	bf[64];

	if( (Row >= 1) && (Col >= 1) && (ActiveControl != Grid4) ){
		Grid4->Canvas->Brush->Color = clWindow;
		Grid4->Canvas->Font->Color = clBlack;
	}
	Grid4->Canvas->FillRect(Rect);
	int X = Rect.Left + 1;
	int Y = Rect.Top + 1;

	if( Row ){
		Grid4GetText(bf, Col, Row);
		Grid4->Canvas->TextOut(X, Y, bf);
	}
	else {		// タイトル
		LPCSTR	_tt[][15]={
			{"No.","PULSE","種類","L(uH)","C(pF)","Q","f(MHz)","","","","","","","",""},
			{"No.","PULSE","種類","R(Ω)","jX(Ω)","","","","","","","","","",""},
			{"No.","PULSE","種類"},
		};
		X += 4;
		int N = 0;
		if( Grid4->Row ){
			N = ant.ldef[Grid4->Row - 1].TYPE;
		}
		if( N == 2 ){		// S-DOMAIN
			if( Col < 3 ){
				Grid4->Canvas->TextOut(X, Y, _tt[N][Col]);
			}
			else {
				sprintf( bf, "%c%d", (((Col-3) & 1) ? 'B':'A'), ((Col-3)/2) );
				Grid4->Canvas->TextOut(X, Y, bf);
			}
		}
		else {
			Grid4->Canvas->TextOut(X, Y, _tt[N][Col]);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainWnd::Grid4SelectCell(TObject *Sender, int Col, int Row,
	bool &CanSelect)
{
	Grid4->Invalidate();
}
//---------------------------------------------------------------------------
// ワイヤー定義 新規ワイヤーの作成
int __fastcall TMainWnd::Grid2NewLine(int Row)
{
	ant.Edit = ant.Flag = 1;
	if( (Row >= ant.wmax) && (Row < WMAX) ){
		if( !ant.wdef[Row].R ){
			ant.wdef[Row].R = Row ? ant.wdef[Row-1].R : 0.0008;
		}
		if( !ant.wdef[Row].SEG ){
			ant.wdef[Row].SEG = Row ? ant.wdef[Row-1].SEG : 0;
		}
		ant.wmax++;
		UpdateCount();
		Grid2->RowCount = ant.wmax + 2;
		Grid2->Invalidate();
		return TRUE;
	}
	return FALSE;
}
//---------------------------------------------------------------------------
// ワイヤー定義編集テキストから値を設定
void __fastcall TMainWnd::Grid2SetEditText(TObject *Sender, int ACol,
	int ARow, const UnicodeString Value)
{
	double	d;
	int		di;
	WDEF	OldW;

	if( Grid2->EditorMode == TRUE ) return;
	if( ARow ){
		ARow--;
		memcpy(&OldW, &ant.wdef[ARow], sizeof(WDEF));
		switch(ACol){
			case 1:		// X1(m)
				if( Calc(d, AnsiString(Value).c_str()) == TRUE ){	//ja7ude 1.0
					ant.wdef[ARow].X1 = SetRmdVal(d);
					if( (Grid2NewLine(ARow) == FALSE) && (ChkWith->Checked == TRUE) ){
						AdjWireChen(ant.wdef, ant.wmax, &ant.wdef[ARow], &OldW);
						Grid2->Invalidate();
					}
				}
				break;
			case 2:		// Y1(m)
				if( Calc(d, AnsiString(Value).c_str()) == TRUE ){	//ja7ude 1.0
					ant.wdef[ARow].Y1 = SetRmdVal(d);
					if( (Grid2NewLine(ARow) == FALSE) && (ChkWith->Checked == TRUE) ){
						AdjWireChen(ant.wdef, ant.wmax, &ant.wdef[ARow], &OldW);
						Grid2->Invalidate();
					}
				}
				break;
			case 3:		// Z1(m)
				if( Calc(d, AnsiString(Value).c_str()) == TRUE ){	//ja7ude 1.0
					ant.wdef[ARow].Z1 = SetRmdVal(d);
					if( (Grid2NewLine(ARow) == FALSE) && (ChkWith->Checked == TRUE) ){
						AdjWireChen(ant.wdef, ant.wmax, &ant.wdef[ARow], &OldW);
						Grid2->Invalidate();
					}
				}
				break;
			case 4:		// X2(m)
				if( Calc(d, AnsiString(Value).c_str()) == TRUE ){	//ja7ude 1.0
					ant.wdef[ARow].X2 = SetRmdVal(d);
					if( (Grid2NewLine(ARow) == FALSE) && (ChkWith->Checked == TRUE) ){
						AdjWireChen(ant.wdef, ant.wmax, &ant.wdef[ARow], &OldW);
						Grid2->Invalidate();
					}
				}
				break;
			case 5:		// Y2(m)
				if( Calc(d, AnsiString(Value).c_str()) == TRUE ){	//ja7ude 1.0
					ant.wdef[ARow].Y2 = SetRmdVal(d);
					if( (Grid2NewLine(ARow) == FALSE) && (ChkWith->Checked == TRUE) ){
						AdjWireChen(ant.wdef, ant.wmax, &ant.wdef[ARow], &OldW);
						Grid2->Invalidate();
					}
				}
				break;
			case 6:		// Z2(m)
				if( Calc(d, AnsiString(Value).c_str()) == TRUE ){	//ja7ude 1.0
					ant.wdef[ARow].Z2 = SetRmdVal(d);
					if( (Grid2NewLine(ARow) == FALSE) && (ChkWith->Checked == TRUE) ){
						AdjWireChen(ant.wdef, ant.wmax, &ant.wdef[ARow], &OldW);
						Grid2->Invalidate();
					}
				}
				break;
			case 7:		// R(cm)
				if( Calc(d, AnsiString(Value).c_str()) == TRUE ){	//ja7ude 1.0
					ant.wdef[ARow].R = SetRmdValD(d) / 1000.0;
					Grid2NewLine(ARow);
				}
				break;
			case 8:		// SEG
				if( Str2Seg(di, AnsiString(Value).c_str()) == TRUE ){	//ja7ude 1.0
					ant.wdef[ARow].SEG = di;
					Grid2NewLine(ARow);
				}
				break;
		}
	}
}
//---------------------------------------------------------------------------
// ワイヤー定義編集前テキストを返す
/*
void __fastcall TForm2::StringGrid1GetEditText(TObject *Sender, int ACol, int ARow,
		  UnicodeString &Value)
*/
void __fastcall TMainWnd::Grid2GetEditText(TObject *Sender, int ACol,   //JA7UDE 1.0
	int ARow, UnicodeString &Value)
{
	char	bf[64];

	Grid2GetText(bf, ACol, ARow);
	Value = bf;
}
//---------------------------------------------------------------------------
// 給電定義 電圧自動設定
void __fastcall TMainWnd::SetAutoVol(void)
{
	if( AutoVol->Checked == TRUE ){
		for( int i = 0; i < ant.cmax; i++ ){
			ant.cdef[i].EV = 1.0/ant.cmax;
		}
	}
}
//---------------------------------------------------------------------------
// 給電定義 新規作成
void __fastcall TMainWnd::Grid3NewLine(int Row)
{
	ant.Edit = ant.Flag = 1;
	if( (Row >= ant.cmax) && (Row < CMAX) ){
		ant.cmax++;
		UpdateCount();
		Grid3->RowCount = ant.cmax + 2;
		SetAutoVol();
		Grid3->Invalidate();
	}
}
//---------------------------------------------------------------------------
// 給電定義編集テキストから値を設定
void __fastcall TMainWnd::Grid3SetEditText(TObject *Sender, int ACol,   //ja7ude 1.0
	int ARow, const UnicodeString Value)
{
	double	d;

	if( Grid3->EditorMode == TRUE ) return;
	if( ARow ){
		ARow--;
		switch(ACol){
			case 1:		// PLUS
				if( !Value.IsEmpty() ){
					ZSCopy(ant.cdef[ARow].PLUS, AnsiString(Value).c_str(), 8);
					Grid3NewLine(ARow);
				}
				break;
			case 2:		// 位相
				if( Calc(d, AnsiString(Value).c_str()) == TRUE ){
					ant.cdef[ARow].DEG = d;
					Grid3NewLine(ARow);
				}
				break;
			case 3:		// 電圧
				if( AutoVol->Checked != TRUE ){
					if( Calc(d, AnsiString(Value).c_str()) == TRUE ){
						ant.cdef[ARow].EV = d;
						Grid3NewLine(ARow);
					}
				}
				break;
		}
	}
}
//---------------------------------------------------------------------------
// 給電定義編集前テキストを返す
void __fastcall TMainWnd::Grid3GetEditText(TObject *Sender, int ACol,
	int ARow, UnicodeString &Value)
{
	char	bf[64];

	Grid3GetText(bf, ACol, ARow);
	Value = bf;
}
//---------------------------------------------------------------------------
// 集中定数 LC
void __fastcall TMainWnd::KS1Click(TObject *Sender)
{
	if( Grid4->Row ){
		LDEF *lp = &ant.ldef[Grid4->Row-1];
		if( Grid4->Row > ant.lmax ){
			Grid4NewLine(Grid4->Row-1);
			lp->TYPE = -1;
		}
		if( lp->TYPE != 0 ){
			lp->TYPE = 0;
			lp->A[0] = 0;
			lp->B[0] = 0;
			lp->A[1] = 0;
			Grid4->Invalidate();
		}
	}
}
//---------------------------------------------------------------------------
// 集中定数 R+jX
void __fastcall TMainWnd::KS2Click(TObject *Sender)
{
	if( Grid4->Row ){
		LDEF *lp = &ant.ldef[Grid4->Row-1];
		if( Grid4->Row > ant.lmax ){
			Grid4NewLine(Grid4->Row-1);
			lp->TYPE = -1;
		}
		if( lp->TYPE != 1 ){
			lp->TYPE = 1;
			Grid4->Invalidate();
		}
	}
}
//---------------------------------------------------------------------------
// 集中定数 S
void __fastcall TMainWnd::KS3Click(TObject *Sender)
{
	if( Grid4->Row ){
		LDEF *lp = &ant.ldef[Grid4->Row-1];
		if( Grid4->Row > ant.lmax ){
			Grid4NewLine(Grid4->Row-1);
			lp->TYPE = -1;
		}
		if( lp->TYPE != 2 ){
			lp->TYPE = 2;
			for( int i = 0; i < SMAX; i++ ){
				lp->A[i] = NULLV;
				lp->B[i] = NULLV;
			}
			Grid4->Invalidate();
		}
	}
}
//---------------------------------------------------------------------------
// ロード定義 新規ロードの作成
void __fastcall TMainWnd::Grid4NewLine(int Row)
{
	ant.Edit = ant.Flag = 1;
	if( (Row >= ant.lmax) && (Row < LMAX) ){
		ant.lmax++;
		if( ant.ldef[Row].TYPE != 2 ){
			if( ant.ldef[Row].A[0] == NULLV ) ant.ldef[Row].A[0] = 0;
			if( ant.ldef[Row].B[0] == NULLV ) ant.ldef[Row].B[0] = 0;
			if( ant.ldef[Row].A[1] == NULLV ) ant.ldef[Row].A[1] = 0;
		}
		UpdateCount();
		Grid4->RowCount = ant.lmax + 2;
		Grid4->Invalidate();
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainWnd::Grid4SetEditText(TObject *Sender, int ACol,
	int ARow, const UnicodeString Value)
{
	double	d, dw;

	if( Grid4->EditorMode == TRUE ) return;
	if( ARow ){
		ARow--;
		switch(ACol){
			case 1:		// パルス
				if( !Value.IsEmpty() ){
					ZSCopy(ant.ldef[ARow].PLUS, AnsiString(Value).c_str(), 8);
					Grid4NewLine(ARow);
				}
				break;
			case 2:		// 種類
				break;
			case 3:		// L,R,A0
				if( Calc(d, AnsiString(Value).c_str()) == TRUE ){
					dw = ant.ldef[ARow].A[0] * ant.ldef[ARow].B[0];
					ant.ldef[ARow].A[0] = d;
					if( (ant.ldef[ARow].TYPE == 0) && dw  && d && (ant.ldef[ARow].B[0]!=NULLV) ){
						ant.ldef[ARow].B[0] = dw / d;
					}
					Grid4->Invalidate();
					Grid4NewLine(ARow);
				}
				else if( ant.ldef[ARow].TYPE == 2 ){
					ant.ldef[ARow].A[0] = NULLV;
				}
				break;
			case 4:		// C,jX,B0
				if( Calc(d, AnsiString(Value).c_str()) == TRUE ){
					dw = ant.ldef[ARow].A[0] * ant.ldef[ARow].B[0];
					ant.ldef[ARow].B[0] = d;
					if( (ant.ldef[ARow].TYPE == 0) && dw  && d && (ant.ldef[ARow].A[0]!=NULLV) ){
						ant.ldef[ARow].A[0] = dw / d;
					}
					Grid4->Invalidate();
					Grid4NewLine(ARow);
				}
				else if( ant.ldef[ARow].TYPE == 2 ){
					ant.ldef[ARow].B[0] = NULLV;
				}
				break;
			case 5:		// Q,NULL,A1
				if( ant.ldef[ARow].TYPE != 1 ){
					if( Calc(d, AnsiString(Value).c_str()) == TRUE ){
						ant.ldef[ARow].A[1] = d;
						Grid4NewLine(ARow);
					}
					else if( ant.ldef[ARow].TYPE == 2 ){
						ant.ldef[ARow].A[1] = NULLV;
					}
				}
				break;
			case 6:		// freq,NULL,B1
				if( ARow < ant.lmax ){
					switch(ant.ldef[ARow].TYPE){
						case 0:		// L,C,Q - Freq
							if( ant.ldef[ARow].A[0] || ant.ldef[ARow].B[0] ){
								if( Calc(d, AnsiString(Value).c_str()) == TRUE ){
									dw = (PAI2*d*1.0e6);
									dw = dw * dw;
									if( ant.ldef[ARow].B[0] ){	// Get L
										ant.ldef[ARow].A[0] = 1.0/(dw * ant.ldef[ARow].B[0] * 1.0e-18);
									}
									else {						// Get C
										ant.ldef[ARow].B[0] = 1.0/(dw * ant.ldef[ARow].A[0] * 1.0e-18);
									}
									Grid4->Invalidate();
									Grid4NewLine(ARow);
								}
							}
							break;
						case 1:
							break;
						case 2:		// S-DOMAIN
							if( Calc(d, AnsiString(Value).c_str()) == TRUE ){
								ant.ldef[ARow].B[1] = d;
								Grid4NewLine(ARow);
							}
							else {
								ant.ldef[ARow].B[1] = NULLV;
							}
							break;
					}
				}
				break;
			default:
				if( ant.ldef[ARow].TYPE == 2 ){
					ACol = (ACol - 3);
					if( Calc(d, AnsiString(Value).c_str()) == TRUE ){
						d = NULLV;
					}
					if( ACol & 1 ){
						ant.ldef[ARow].B[ACol/2] = d;
					}
					else {
						ant.ldef[ARow].A[ACol/2] = d;
					}
					Grid4NewLine(ARow);
				}
				break;
		}
	}
}
//---------------------------------------------------------------------------
// ロード定義のテキストを返す
void __fastcall TMainWnd::Grid4GetEditText(TObject *Sender, int ACol,
	int ARow, UnicodeString &Value)
{
	if( ACol == 2 ){
		Grid4->EditorMode = FALSE;
		int X = Left + (Width - ClientWidth)/2 + TabSheet1->Left + Grid4->Left;
		int Y = Top + (Height - ClientHeight)/2 + TabSheet1->Top + Grid4->Top;
		X += Grid4->ColWidths[0] + Grid4->ColWidths[1];
		Y += ((Grid4->Row - Grid4->TopRow) * Grid4->RowHeights[0]);
		PopupLoad->Popup(X, Y);
	}
	else {
		char	bf[64];

		Grid4GetText(bf, ACol, ARow);
		Value = bf;
	}
}
// ポップアップ 「この行を削除」
void __fastcall TMainWnd::K7Click(TObject *Sender)
{
	ant.Edit = ant.Flag = 1;
	int	Row, i;
	if( ActiveControl == Grid2 ){		// ワイヤ
		if( Grid2->Row && (Grid2->Row <= ant.wmax) ){
			Row = Grid2->Row - 1;
			for( i = Row; i < (ant.wmax - 1); i++ ){
				memcpy(&ant.wdef[i], &ant.wdef[i+1], sizeof(WDEF));
			}
			memset(&ant.wdef[i], 0, sizeof(WDEF));
			ant.wmax--;
			UpdateCount();
			Grid2->RowCount = ant.wmax + 2;
			Grid2->Invalidate();
		}
	}
	else if( ActiveControl == Grid3 ){	// 給電
		if( Grid3->Row && (Grid3->Row <= ant.cmax) ){
			Row = Grid3->Row - 1;
			for( i = Row; i < (ant.cmax - 1); i++ ){
				memcpy(&ant.cdef[i], &ant.cdef[i+1], sizeof(CDEF));
			}
			memset(&ant.cdef[i], 0, sizeof(CDEF));
			ant.cmax--;
			UpdateCount();
			SetAutoVol();
			Grid3->RowCount = ant.cmax + 2;
			Grid3->Invalidate();
		}
	}
	else if( ActiveControl == Grid4 ){	// ロード
		if( Grid4->Row && (Grid4->Row <= ant.lmax) ){
			Row = Grid4->Row - 1;
			for( i = Row; i < (ant.lmax - 1); i++ ){
				memcpy(&ant.ldef[i], &ant.ldef[i+1], sizeof(LDEF));
			}
			memset(&ant.ldef[i], 0, sizeof(LDEF));
			for( int j = 0; j < SMAX; j++ ){
				ant.ldef[i].A[j] = NULLV;
				ant.ldef[i].B[j] = NULLV;
			}
			ant.lmax--;
			UpdateCount();
			Grid4->RowCount = ant.lmax + 2;
			Grid4->Invalidate();
		}
	}
}
//---------------------------------------------------------------------------
// ポップアップ 「この行に挿入」
void __fastcall TMainWnd::K9Click(TObject *Sender)
{
	ant.Edit = ant.Flag = 1;
	int	Row, i;
	if( ActiveControl == Grid2 ){		// ワイヤ
		if( Grid2->Row && (Grid2->Row <= ant.wmax) && (ant.wmax < (WMAX-1)) ){
			Row = Grid2->Row - 1;
			for( i = ant.wmax; i > Row; i-- ){
				memcpy(&ant.wdef[i], &ant.wdef[i-1], sizeof(WDEF));
			}
			memset(&ant.wdef[i], 0, sizeof(WDEF));
			if( i ){
				ant.wdef[i].R = ant.wdef[i-1].R;
				ant.wdef[i].SEG = ant.wdef[i-1].SEG;
			}
			else if( ant.wmax ){
				ant.wdef[i].R = ant.wdef[i+1].R;
				ant.wdef[i].SEG = ant.wdef[i+1].SEG;
			}
			else {
				ant.wdef[i].R = 0.001;
				ant.wdef[i].SEG = 0;
			}
			ant.wmax++;
			UpdateCount();
			Grid2->RowCount = ant.wmax + 2;
			Grid2->Invalidate();
		}
	}
	else if( ActiveControl == Grid3 ){	// 給電
		if( Grid3->Row && (Grid3->Row <= ant.cmax) && (ant.cmax < (CMAX-1))){
			Row = Grid3->Row - 1;
			for( i = ant.cmax; i > Row; i-- ){
				memcpy(&ant.cdef[i], &ant.cdef[i-1], sizeof(CDEF));
			}
			memset(&ant.cdef[i], 0, sizeof(CDEF));
			ant.cmax++;
			UpdateCount();
			SetAutoVol();
			Grid3->RowCount = ant.cmax + 2;
			Grid3->Invalidate();
		}
	}
	else if( ActiveControl == Grid4 ){	// ロード
		if( Grid4->Row && (Grid4->Row <= ant.lmax) && (ant.lmax < (LMAX-1))){
			Row = Grid4->Row - 1;
			for( i = ant.lmax; i > Row; i-- ){
				memcpy(&ant.ldef[i], &ant.ldef[i-1], sizeof(LDEF));
			}
			memset(&ant.ldef[i], 0, sizeof(LDEF));
			for( int j = 0; j < SMAX; j++ ){
				ant.ldef[i].A[j] = NULLV;
				ant.ldef[i].B[j] = NULLV;
			}
			ant.ldef[i].A[0] = 0;
			ant.ldef[i].B[0] = 0;
			ant.ldef[i].A[1] = 0;
			ant.lmax++;
			UpdateCount();
			Grid4->RowCount = ant.lmax + 2;
			Grid4->Invalidate();
		}
	}
}
//---------------------------------------------------------------------------
//現在のアンテナ定義を文字列に変換
int __fastcall TMainWnd::SaveAntFile(LPCSTR pName)
{
	AnsiString	out;
	FILE		*fp;

	SaveAntStrings(out);
	if( (fp = fopen(pName, "wb"))!=NULL ){
		fputs(out.c_str(), fp);
		ant.Edit = 0;
		if( fclose(fp) ){
			ErrorMB("'%s'の作成に失敗しました.", pName);
			return FALSE;
		}
		else {
			return TRUE;
		}
	}
	else {
		ErrorMB("'%s'を作成できません.", pName);
		return FALSE;
	}
}
//---------------------------------------------------------------------------
//アンテナ定義ファイルを読込む
int __fastcall TMainWnd::LoadAntFile(LPCSTR pName)
{
	AnsiString	in;
	FILE		*fp;
	char		bf[1024];

	if( (fp = fopen(pName, "rt"))!=NULL ){
		while(!feof(fp)){
			if( fgets(bf, 1024, fp) != NULL ){
				ClipLF(bf);
				in += bf;
				in += "\r\n";
			}
		}
		fclose(fp);

		InitAntDef();
		LoadAntStrings(in);
		SetAntDef();
		exeenv.CalcLog = 1;
		SetStackAnt();
		res.ClearBWC();
		ant.Flag = 0;
		AllViewBtnClick(NULL);
		UpdateAllViews();
		ant.Edit = 0;
		return TRUE;
	}
	else {
		ErrorMB("'%s'が見つかりません.", pName);
		return FALSE;
	}
}

//---------------------------------------------------------------------------
//現在のアンテナ定義を文字列に変換
void __fastcall TMainWnd::SaveAntStrings(AnsiString &out)
{
	char	bf[256];
	int		i, j;
	LPSTR	t;

	GetAntDef();
	out = ant.Name;
	out += "\r\n*\r\n";
	sprintf(bf, "%s\r\n", StrDbl(ant.fq));
	out += bf;
	out += "*** ﾜｲﾔ ***\r\n";
	t = bf;
	if( (ant.StackH > 1)||(ant.StackV > 1) ){
		sprintf(t, "%d,\t%d,\t%d,\t", ant.wmax, ant.StackH, ant.StackV);t += strlen(t);
		sprintf(t, "%s,\t", StrDblE(ant.StackHW));t += strlen(t);
		sprintf(t, "%s,\t", StrDblE(ant.StackVW));t += strlen(t);
		sprintf(t, "%d\r\n", ant.StackVT);
	}
	else {
		sprintf(t, "%d\r\n", ant.wmax);
	}
	out += bf;
	for( i = 0; i < ant.wmax; i++ ){
		t = bf;
		sprintf(t, "%s,\t", StrDblE(ant.wdef[i].X1));t += strlen(t);
		sprintf(t, "%s,\t", StrDblE(ant.wdef[i].Y1));t += strlen(t);
		sprintf(t, "%s,\t", StrDblE(ant.wdef[i].Z1));t += strlen(t);
		sprintf(t, "%s,\t", StrDblE(ant.wdef[i].X2));t += strlen(t);
		sprintf(t, "%s,\t", StrDblE(ant.wdef[i].Y2));t += strlen(t);
		sprintf(t, "%s,\t", StrDblE(ant.wdef[i].Z2));t += strlen(t);
		sprintf(t, "%s,\t", StrDblE(ant.wdef[i].R));t += strlen(t);
		sprintf(t, "%s\r\n", Seg2Str(ant.wdef[i].SEG));
		out += bf;
	}
	out += "*** 給電点 ***\r\n";
	sprintf(bf, "%d,\t%d\r\n", ant.cmax, ant.cauto);
	out += bf;
	for( i = 0; i < ant.cmax; i++ ){
		t = bf;
		sprintf(t, "%s,\t", ant.cdef[i].PLUS);t += strlen(t);
		sprintf(t, "%s,\t", StrDblE(ant.cdef[i].DEG));t += strlen(t);
		sprintf(t, "%s\r\n", StrDblE(ant.cdef[i].EV));
		out += bf;
	}
	out += "*** 集中定数 ***\r\n";
	sprintf(bf, "%d,\t%d\r\n", ant.lmax, ant.lenb);
	out += bf;
	for( i = 0; i < ant.lmax; i++ ){
		t = bf;
		sprintf(t, "%s,\t", ant.ldef[i].PLUS);t += strlen(t);
		sprintf(t, "%d,\t", ant.ldef[i].TYPE);t += strlen(t);
		switch(ant.ldef[i].TYPE){
			case 0:		// L,C,Q
				sprintf(t, "%s,\t", StrDblE(ant.ldef[i].A[0]));t += strlen(t);
				sprintf(t, "%s,\t", StrDblE(ant.ldef[i].B[0]));t += strlen(t);
				sprintf(t, "%s\r\n", StrDblE(ant.ldef[i].A[1]));
				break;
			case 1:		// R+jX
				sprintf(t, "%s,\t", StrDblE(ant.ldef[i].A[0]));t += strlen(t);
				sprintf(t, "%s\r\n", StrDblE(ant.ldef[i].B[0]));
				break;
			case 2:		// S
				for( j = 0; j < ant.ldef[i].SN; j++ ){
					if( j ){
						*t++ = ',';
						*t++ = '\t';
					}
					sprintf(t, "%s,\t", StrDblE(ant.ldef[i].A[j]));t += strlen(t);
					sprintf(t, "%s", StrDblE(ant.ldef[i].B[j]));t += strlen(t);
				}
				strcpy(t, "\r\n");
				break;
		}
		out += bf;
	}
	t = bf;
	out += "*** 自動分割 ***\r\n";
	sprintf(t, "%d,\t%d,\t", ant.DM1, ant.DM2);t += strlen(t);
	sprintf(t, "%s,\t%d\r\n", StrDbl(ant.SC), ant.EC);
	out += bf;
	t = bf;
	out += "*** 計算環境 ***\r\n";
	sprintf(t, "%u,\t%s,\t%u,\t", env.type, StrDblE(env.antheight), env.WireRoss);
	t += strlen(t);
	sprintf(t, "%s,\t%u,\t%u,\t%.lf\r\n", StrDblE(env.RO), env.fbr & 0x0000ffff, (env.fbr >> 16), env.JXO );
	out += bf;
	if( ant.pmax ){
		out += "$$$ ワイヤ組み合わせデータ $$$\r\n";
		sprintf(bf, "%u\r\n", ant.pmax );
		out += bf;
		for( i = 0; i < ant.pmax; i++ ){
			t = bf;
			sprintf(t, "%s,\t", StrDblE(ant.pdef[i].RR)); t += strlen(t);
			sprintf(t, "%u,\t", ant.pdef[i].Type ); t += strlen(t);
			for( j = 0; ant.pdef[i].L[j] && (j < PPMAX); j++ ){
				if( j ){
					*t++ = ',';
					*t++ = '\t';
				}
				sprintf(t, "%s,\t", StrDblE(ant.pdef[i].L[j])); t += strlen(t);
				sprintf(t, "%s", StrDblE(ant.pdef[i].R[j])); t += strlen(t);
			}
			strcpy(t, "\r\n");
			out += bf;
		}
	}
	if( *antRem.c_str() ){
		out += "### コメント ###\r\n";
		out += antRem;
	}
}

//---------------------------------------------------------------------------
//文字列からアンテナ定義を作成
void __fastcall TMainWnd::LoadAntStrings(AnsiString &in)
{
	CTextString	cs(in);
	int		i, j;
	char	bf[1024];
	LPSTR	t, p;

	antRem = "";
	if( cs.LoadTextLoop(bf, 1023) == FALSE ) goto _ex;
	strncpy(ant.Name, bf, sizeof(ant.Name));
	if( cs.LoadTextLoop(bf, 1023) == FALSE ) goto _ex;
	Calc(ant.fq, bf);
	if( cs.LoadTextLoop(bf, 1023) == FALSE ) goto _ex;
	p = StrDlm(t, bf);
	CalcU(ant.wmax, t);
	p = StrDlm(t, p);
	if( *t ){		// スタック設定
		CalcU(ant.StackH, t);
		p = StrDlm(t, p);
		CalcU(ant.StackV, t);
		p = StrDlm(t, p);
		Calc(ant.StackHW, t);
		p = StrDlm(t, p);
		Calc(ant.StackVW, t);
		StrDlm(t, p);
		CalcU(ant.StackVT, t);
		if( (ant.StackH < 1)||(ant.StackV < 1) ){
			ant.StackH = ant.StackV = 1;
		}
	}
	else {
		ant.StackVT = 0;
		ant.StackH = ant.StackV = 1;
	}
	if( (ant.StackH == 1) && (ant.StackV == 1) && ant.fq ){
		ant.StackHW = ant.StackVW = 299.8/ant.fq;
	}
	if( ant.wmax > WMAX ) ant.wmax = WMAX;
	for( i = 0; i < ant.wmax; i++ ){
		if( cs.LoadTextLoop(bf, 1023) == FALSE ) goto _ex;
		p = StrDlm(t, bf);	// X1
		Calc(ant.wdef[i].X1, t);
		p = StrDlm(t, p);	// Y1
		Calc(ant.wdef[i].Y1, t);
		p = StrDlm(t, p);	// Z1
		Calc(ant.wdef[i].Z1, t);
		p = StrDlm(t, p);	// X2
		Calc(ant.wdef[i].X2, t);
		p = StrDlm(t, p);	// Y2
		Calc(ant.wdef[i].Y2, t);
		p = StrDlm(t, p);	// Z2
		Calc(ant.wdef[i].Z2, t);
		p = StrDlm(t, p);	// R
		Calc(ant.wdef[i].R, t);
		p = StrDlm(t, p);	// SEG(DM1)
		CalcI(ant.wdef[i].SEG, t);
		StrDlm(t, p);		// SEG(DM2)
		if( *t ){
			CalcI(j, t);
			if( j ) ant.wdef[i].SEG = 0x80000000 | ABS(ant.wdef[i].SEG) | (ABS(j) << 16);
		}
	}
	if( cs.LoadTextLoop(bf, 1023) == FALSE ) goto _ex;
	p = StrDlm(t, bf);
	CalcU(ant.cmax, t);
	StrDlm(t, p);
	CalcU(ant.cauto, t); if( ant.cauto ) ant.cauto = 1;
	if( ant.cmax > CMAX ) ant.cmax = CMAX;
	for( i = 0; i < ant.cmax; i++ ){
		if( cs.LoadTextLoop(bf, 1023) == FALSE ) goto _ex;
		p = StrDlm(t, bf);	// PLUS
		ZSCopy(ant.cdef[i].PLUS, t, 8);
		p = StrDlm(t, p);	// DEG
		Calc(ant.cdef[i].DEG, t);
		StrDlm(t, p);		// EV
		Calc(ant.cdef[i].EV, t);
	}
	if( cs.LoadTextLoop(bf, 1023) == FALSE ) goto _ex;
	p = StrDlm(t, bf);
	CalcU(ant.lmax, t);
	StrDlm(t, p);
	CalcU(ant.lenb, t); if( ant.lenb ) ant.lenb = 1;
	if( ant.lmax > LMAX ) ant.lmax = LMAX;
	for( i = 0; i < ant.lmax; i++ ){
		if( cs.LoadTextLoop(bf, 1023) == FALSE ) goto _ex;
		p = StrDlm(t, bf);	// PLUS
		ZSCopy(ant.ldef[i].PLUS, t, 8);
		p = StrDlm(t, p);	// TYPE
		CalcU(ant.ldef[i].TYPE, t);
		switch(ant.ldef[i].TYPE){
			case 0:		// L,C,Q
				p = StrDlm(t, p);	// L
				Calc(ant.ldef[i].A[0], t);
				p = StrDlm(t, p);	// C
				Calc(ant.ldef[i].B[0], t);
				StrDlm(t, p);	// Q
				Calc(ant.ldef[i].A[1], t);
				break;
			case 1:		// R+jX
				p = StrDlm(t, p);	// R
				Calc(ant.ldef[i].A[0], t);
				StrDlm(t, p);	// jX
				Calc(ant.ldef[i].B[0], t);
				break;
			case 2:		// S
				for( j = 0; j < SMAX; j++ ){
					p = StrDlm(t, p);	// A[j]
					if( !*t ) break;
					Calc(ant.ldef[i].A[j], t);
					p = StrDlm(t, p);	// B[j]
					Calc(ant.ldef[i].B[j], t);
				}
				ant.ldef[i].SN = j;
				for( ; j < SMAX; j++ ){
					ant.ldef[i].A[j] = NULLV;
					ant.ldef[i].B[j] = NULLV;
				}
				break;
		}
	}

	if( cs.LoadTextLoop(bf, 1023) == TRUE ){
		p = StrDlm(t, bf);		// DM1
		CalcU(ant.DM1, t);
		p = StrDlm(t, p);		// DM2
		CalcU(ant.DM2, t);
		p = StrDlm(t, p);		// SC
		Calc(ant.SC, t);
		if( (ant.SC <= 1.0) || (ant.SC > 3.0) ){
			ant.SC = 2.0;
		}
		StrDlm(t, p);			// EC
		if( *t ){
			CalcU(ant.EC, t);
			if( ant.EC <= 0 ) ant.EC = 1;
		}
		else {
			ant.EC = 1;
		}
	}
	else {
		ant.DM2 = 400;
		ant.DM2 = 40;
		ant.EC = 1;
		ant.SC = 2.0;
	}

	if( cs.LoadTextLoop(bf, 1023) == FALSE ) goto _ex;
	p = StrDlm(t, bf);
	CalcU(env.type, t);
	if( env.type >= 3 ) env.type = 0;
	p = StrDlm(t, p);
	Calc(env.antheight, t);
	p = StrDlm(t, p);
	if( *t ){
		CalcU(env.WireRoss, t);
		if( (env.WireRoss < 0)||(env.WireRoss > 6) ){
			env.WireRoss = 0;
		}
		p = StrDlm(t, p);	// RO
		if( *t && (Calc(env.RO, t)==TRUE) ){
			p = StrDlm(t, p);
			CalcU(i, t);
			p = StrDlm(t, p);
			CalcU(j, t);
			env.fbr = i | (j << 16);
			StrDlm(t, p);
			if( *t ){
				Calc(env.JXO, t);
			}
		}
	}
	else {
		env.WireRoss = 0;
	}
	while(1){
		if( cs.LoadTextLoop(bf, 1023) == FALSE ) goto _ex;
		if( !strncmp(bf, "$$$", 3) ){
			if( cs.LoadTextLoop(bf, 1023) == FALSE ) goto _ex;
			CalcU(ant.pmax, bf);
			for( i = 0; (i < ant.pmax)&&(i < PTMAX); i++ ){
				if( cs.LoadTextLoop(bf, 1023) == FALSE ) goto _ex;
				p = StrDlm(t, bf);	// RR
				Calc(ant.pdef[i].RR, t);
				p = StrDlm(t, p);	// Type
				CalcU(ant.pdef[i].Type, t);
				for( j = 0; j < PPMAX; j++ ){
					p = StrDlm(t, p);
					if( !*t ) break;
					Calc(ant.pdef[i].L[j], t);
					p = StrDlm(t, p);
					Calc(ant.pdef[i].R[j], t);
				}
				for( ; j < PPMAX; j++ ){
					ant.pdef[i].R[j] = 0.007;
				}
			}
			AdjPdef(&ant);
		}
		else if( !strncmp(bf, "###", 3) ){		// コメント
			while(1){
				if( cs.LoadText(bf, 1023) == FALSE ) goto _ex;
				antRem += bf;
				antRem += "\r\n";
			}
		}
	}
_ex:;
	if( ant.fq ){
		sprintf( bf, "%.3lf", ant.fq );
	}
	else {
		bf[0] = 0;
	}
	DoFreq->Text = bf;
	ant.Edit = 0;
	ant.Flag = 1;

	exeenv.AntXc = exeenv.AntYc = 0;
}

//---------------------------------------------------------------------------
// TextEditボタン
// 新規作成 メニュー
void __fastcall TMainWnd::K2Click(TObject *Sender)
{
	if( CheckSaveAntFile() == FALSE ) return;
	antFname = "";
	InitAntDef();
	res.Clear();
	UpdateAllViews();
}
//---------------------------------------------------------------------------
// 開くメニュー
void __fastcall TMainWnd::K3Click(TObject *Sender)
{
	if( CheckSaveAntFile() == FALSE ) return;
//	OpenDialog->Filter = "MMANA AntFile(maa)|*.maa|MMPC AntFile(ant)|*.ANT|";
	OpenDialog->Title = "アンテナファイルを開く";
	OpenDialog->Filter = "MMANA AntFile(*.maa)|*.maa|";
	OpenDialog->FileName = "";
	OpenDialog->DefaultExt = "maa";
	OpenDialog->InitialDir = AntDir;
	if( OpenDialog->Execute() == TRUE ){
		antFname = OpenDialog->FileName;
		if( LoadAntFile(antFname.c_str()) == TRUE ){
			SetDirName(AntDir, antFname.c_str());
			RecentAdd(antFname.c_str(), TRUE);
		}
	}
}
//---------------------------------------------------------------------------
//保存メニュー
void __fastcall TMainWnd::K4Click(TObject *Sender)
{
	if( *antFname.c_str() ){
		SaveAntFile(antFname.c_str());
		RecentAdd(antFname.c_str(), TRUE);
	}
	else {
		K5Click(Sender);
	}
}
//---------------------------------------------------------------------------
// 名前を変更して保存
void __fastcall TMainWnd::K5Click(TObject *Sender)
{
	SaveDialog->Title = "アンテナファイルを保存";
	SaveDialog->Filter = "MMANA AntFile(*.maa)|*.maa|";
	if( *antFname.c_str() ){
		SaveDialog->FileName = GetFileName(antFname.c_str());
	}
	else {
		SaveDialog->FileName = "無題";
	}
	SaveDialog->DefaultExt = "maa";
	SaveDialog->InitialDir = AntDir;
	if( SaveDialog->Execute() == TRUE ){
		antFname = SaveDialog->FileName;
		GetAntDef();
		SetAntDef();
		SaveAntFile(antFname.c_str());
		SetDirName(AntDir, antFname.c_str());
		RecentAdd(antFname.c_str(), TRUE);
	}
}
//---------------------------------------------------------------------------
// 計算ボタン
void __fastcall TMainWnd::CalTrgBtnClick(TObject *Sender)
{
	GetAntDef();
	if( ant.cfq == 0.0 ) ant.cfq = ant.fq;
	SetWaitCursor();
	ant.Flag = 0;
	exeenv.CalcLog = 1;
	if( SetStackAnt() == TRUE ){
		if( (!env.type) || ((pCalAnt->MinZ + env.antheight) >= 0.0) || (YesNoMB( "一部のワイヤが地中に埋まっています. 計算を中止しますか ?") == IDNO) ){
			res.IncResP();
			res.GetResP()->TYPE = env.type;
			res.GetResP()->HEIGHT = env.antheight;
			res.GetResP()->FBR = env.fbr;
			res.ClearBWC();
			Grid1->RowCount = res.GetCnt() + (res.GetCnt() ? 1 : 2);
			BgnTime = ::GetTickCount();
			exeenv.CalcF = 1;
			CalCurrent();
			exeenv.CalcF = 0;
			EndTime = ::GetTickCount();
			LogPrint("%.2lf(s)", double(EndTime - BgnTime)/1000.0);
			res.SetCalc(ant.Name);		// 計算完了フラグ
			res.InitBWC();
			AntName4->Caption = ant.Name;
			DrawPtnH.SetMaxDB(res.MaxG);
			DrawPtnV.SetMaxDB(res.MaxG);
			Grid1->Invalidate();
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainWnd::K10Click(TObject *Sender)
{
	TVerDspDlg *Box = new TVerDspDlg(this);
	Box->ShowModal();
	delete Box;
}
//---------------------------------------------------------------------------
// パルス番号からセグメント番号を得る
int __fastcall TMainWnd::Plus2Seg(int Plus)
{
	int i;
	for( i = 0; i < pCalAnt->wzmax; i++ ){
		if( Plus < pCalAnt->wzdef[i].PNo ) break;
	}
	i--;
	while( !pCalAnt->wzdef[i].PNo ) i--;
	if( i < 0 ) i = 0;
	return pCalAnt->wzdef[i].SNo + (Plus - pCalAnt->wzdef[i].PNo);
}

//---------------------------------------------------------------------------
// アンテナ形状の表示イベント
void __fastcall TMainWnd::PBoxAntPaint(TObject *Sender)
{
	TRect	rc;

	rc.Left = 0;
	rc.Top = 0;
	rc.Right = PBoxAnt->Width;
	rc.Bottom = PBoxAnt->Height;
	PBoxAnt->Canvas->Brush->Color = clWhite;
	PBoxAnt->Canvas->FillRect(rc);
	Clip.SetClip(-100, -100, rc.Right + 100, rc.Bottom + 100);
	double sc = double(TBarSC->Position) / 20.0;
	sc = sc * sc * sc * sc;
	int		Xc = int(PBoxAnt->Width/2 + (exeenv.AntXc * sc));
	int		Yc = int(PBoxAnt->Height/2 + (exeenv.AntYc * sc));

	PBoxAnt->Canvas->Pen->Color = clWhite;
	double x, y;
	int		X,Y, X2, Y2;
	double deg = double(TBarDeg->Position);
	deg *= (PAI / 180.0);
	double zdeg = double(TBarZDeg->Position);
	zdeg *= (PAI / 180.0);

	PBoxAnt->Canvas->Pen->Color = clLtGray;
	Calc3DXY(x, y, deg, zdeg, 0, 0, 0);				// Y
	X = int(x) + Xc;
	Y = Yc - int(y);
	PBoxAnt->Canvas->MoveTo(X, Y);
	Calc3DXY(x, y, deg, zdeg, 0, PBoxAnt->Width/2-16, 0);
	X = int(x) + Xc;
	Y = Yc - int(y);
	PBoxAnt->Canvas->LineTo(X, Y);
	int Sop = ::SetBkMode(PBoxAnt->Canvas->Handle, TRANSPARENT);
	PBoxAnt->Canvas->TextOut(X, Y, "Y");

	Calc3DXY(x, y, deg, zdeg, 0, 0, 0);				// Z
	X = int(x) + Xc;
	Y = Yc - int(y);
	PBoxAnt->Canvas->MoveTo(X, Y);
	Calc3DXY(x, y, deg, zdeg, 0, 0, PBoxAnt->Height);
	X = int(x) + Xc;
	Y = Yc - int(y);
	PBoxAnt->Canvas->LineTo(X, Y);
	::SetBkMode(PBoxAnt->Canvas->Handle, TRANSPARENT);
	PBoxAnt->Canvas->TextOut(X+4, 0, "Z");

	Calc3DXY(x, y, deg, zdeg, 0, 0, 0);				// X
	X = int(x) + Xc;
	Y = Yc - int(y);
	PBoxAnt->Canvas->MoveTo(X, Y);
	Calc3DXY(x, y, deg, zdeg, PBoxAnt->Height/2, 0, 0);
	X = int(x) + Xc;
	Y = Yc - int(y);
	PBoxAnt->Canvas->LineTo(X, Y);
	::SetBkMode(PBoxAnt->Canvas->Handle, TRANSPARENT);
	PBoxAnt->Canvas->TextOut(X, Y, "X");

	// ワイヤの表示
	PBoxAnt->Canvas->Pen->Color = clBlack;
	int sel = Grid2->Row - 1;
	int i;
	WDEF *wp = pCalAnt->wdef;
	for( i = 0; i < pCalAnt->wmax; i++, wp++ ){
		PBoxAnt->Canvas->Pen->Color = wp->R ? clBlack : clGray;
		PBoxAnt->Canvas->Pen->Width = ((i == sel) ? 2 : 1);
		Calc3DXY(x, y, deg, zdeg, wp->X1, wp->Y1, wp->Z1);
		X = int((x * sc)) + Xc;
		Y = Yc - int((y * sc));
		Calc3DXY(x, y, deg, zdeg, wp->X2, wp->Y2, wp->Z2);
		X2 = int((x * sc)) + Xc;
		Y2 = Yc - int((y * sc));
		if( Clip.Clip(X, Y, X2, Y2) == TRUE ){
			PBoxAnt->Canvas->MoveTo(X, Y);
			PBoxAnt->Canvas->LineTo(X2, Y2);
		}
		if( wp->R < 0.0 ){	// 組み合わせワイヤ
			PDEF *pp = FindPP(pCalAnt, wp->R);
			if( pp != NULL ){
				PBoxAnt->Canvas->Pen->Width = 1;
				PBoxAnt->Canvas->Pen->Color = clBlue;
				WDEF	temp[PPMAX*2+2];
				int n = CombWire(temp, pCalAnt, pp, wp) - 1;
				int j;
				WDEF *tp;
				for( tp = temp, j = 0; j < n; j++, tp++ ){
					Calc3DXY(x, y, deg, zdeg, tp->X2, tp->Y2, tp->Z2);
					X = int((x * sc)) + Xc;
					Y = Yc - int((y * sc));
					::SetBkMode(PBoxAnt->Canvas->Handle, TRANSPARENT);
					PBoxAnt->Canvas->MoveTo(X-2, Y-2);
					PBoxAnt->Canvas->LineTo(X+2, Y-2); PBoxAnt->Canvas->LineTo(X+2, Y+2);
					PBoxAnt->Canvas->LineTo(X-2, Y+2); PBoxAnt->Canvas->LineTo(X-2, Y-2);
				}
				PBoxAnt->Canvas->Pen->Color = clBlack;
			}
		}
	}
	PBoxAnt->Canvas->Pen->Width = 1;
	double	cx, cy, cz;
	if( pCalAnt->wmax && DspPlus->Checked ){		// セグメント分割の表示
		PBoxAnt->Canvas->Pen->Color = clGreen;
		for( i = 1; i <= (GetPlusMax()+(pCalAnt->wzmax*2)); i++ ){
			GetSegPos(cx, cy, cz, i);
			Calc3DXY(x, y, deg, zdeg, cx, cy, cz);
			X = int((x * sc)) + Xc;
			Y = Yc - int((y * sc));
			::SetBkMode(PBoxAnt->Canvas->Handle, TRANSPARENT);
			PBoxAnt->Canvas->MoveTo(X-3, Y-3); PBoxAnt->Canvas->LineTo(X+3, Y+3);
			PBoxAnt->Canvas->MoveTo(X+3, Y-3); PBoxAnt->Canvas->LineTo(X-3, Y+3);
		}
	}
	int	YT = 0;
	PBoxAnt->Canvas->Pen->Color = clRed;
	int FH = PBoxAnt->Canvas->TextHeight("給電点");
	PBoxAnt->Canvas->TextOut(12, YT, "給電点");
	PBoxAnt->Canvas->Ellipse(3, YT + (FH/4), 10, YT + (FH/4) + 7);
	YT += FH + 2;
	for( i = 0; i < pCalAnt->cmax; i++ ){	// 給電点
		GetSegPos(cx, cy, cz, Plus2Seg(pCalAnt->cdef[i].PLUSNo));
		Calc3DXY(x, y, deg, zdeg, cx, cy, cz);
		X = int((x * sc)) + Xc;
		Y = Yc - int((y * sc));
		::SetBkMode(PBoxAnt->Canvas->Handle, TRANSPARENT);
		PBoxAnt->Canvas->Ellipse(X-3, Y-3, X+4, Y+4);
	}

	PBoxAnt->Canvas->Pen->Color = clRed;
	Y = YT+(FH/2);
	PBoxAnt->Canvas->MoveTo(3, Y-3); PBoxAnt->Canvas->LineTo(9, Y+3);
	PBoxAnt->Canvas->MoveTo(9, Y-3); PBoxAnt->Canvas->LineTo(3, Y+3);
	PBoxAnt->Canvas->TextOut(12, YT, "集中定数");
	if( EnbLoad->Checked ){
		for( i = 0; i < pCalAnt->lmax; i++ ){	// ロード
			GetSegPos(cx, cy, cz, Plus2Seg(pCalAnt->ldef[i].PLUSNo));
			Calc3DXY(x, y, deg, zdeg, cx, cy, cz);
			X = int((x * sc)) + Xc;
			Y = Yc - int((y * sc));
			::SetBkMode(PBoxAnt->Canvas->Handle, TRANSPARENT);
			PBoxAnt->Canvas->MoveTo(X-3, Y-3); PBoxAnt->Canvas->LineTo(X+3, Y+3);
			PBoxAnt->Canvas->MoveTo(X+3, Y-3); PBoxAnt->Canvas->LineTo(X-3, Y+3);
		}
	}
#if 0
	YT += FH + 2;
	PBoxAnt->Canvas->Pen->Color = clBlue;
	X = 5; Y = YT+(FH/2);
	PBoxAnt->Canvas->MoveTo(X-2, Y-2);
	PBoxAnt->Canvas->LineTo(X+2, Y-2); PBoxAnt->Canvas->LineTo(X+2, Y+2);
	PBoxAnt->Canvas->LineTo(X-2, Y+2); PBoxAnt->Canvas->LineTo(X-2, Y-2);
	PBoxAnt->Canvas->TextOut(12, YT, "組み合わせ点");
#endif

	// 電流分布の表示
	if( DspCur->Checked && res.IsCalc() ){
		int s, p, v;
		int AX=0;
		int AY=0;
		int	wno = -1;
		double Mag = double(TBarCur->Position) / 10.0;
		Mag = Mag * Mag * Mag * Mag;
		double	Vect, Cur;
		for( i = 0; i < pCalAnt->wzmax; i++ ){
			if( !pCalAnt->wzdef[i].PNo ) continue;
			s = pCalAnt->wzdef[i].Wno;
			Vect = ABS(pCalAnt->wdef[s].Z2 - pCalAnt->wdef[s].Z1);
			if( (Vect >= ABS(pCalAnt->wdef[s].Y2 - pCalAnt->wdef[s].Y1))&&
				(Vect >= ABS(pCalAnt->wdef[s].X2 - pCalAnt->wdef[s].X1)) ){
				v = 1;
			}
			else {
				v = 0;
			}
			if( exeenv.CurDir ){
				Vect = (pCalAnt->wdef[s].X2 - pCalAnt->wdef[s].X1) + (pCalAnt->wdef[s].Y2 - pCalAnt->wdef[s].Y1) + (pCalAnt->wdef[s].Z2 - pCalAnt->wdef[s].Z1);
			}
			for( s = pCalAnt->wzdef[i].SNo, p = pCalAnt->wzdef[i].PNo; s <= pCalAnt->wzdef[i].SMax; s++, p++ ){
				GetSegPos(cx, cy, cz, s);
				if( res.Cur[p-1] == NULLF ) continue;
				Cur = res.Cur[p-1] * Mag;
				if( exeenv.CurDir ){
					Cur = (Vect >= 0) ? Cur : -Cur;
				}
				else {
					Cur = ABS(Cur);
				}
				if( v ){
					cy += Cur;
					PBoxAnt->Canvas->Pen->Color = clRed;
				}
				else {
					cz += Cur;
					PBoxAnt->Canvas->Pen->Color = clBlue;
				}
				Calc3DXY(x, y, deg, zdeg, cx, cy, cz);
				X = int((x * sc)) + Xc;
				Y = Yc - int((y * sc));
				ClipXY(X, Y);
				if( wno != pCalAnt->wzdef[i].Wno ){
					wno = pCalAnt->wzdef[i].Wno;
				}
				else {
					::SetBkMode(PBoxAnt->Canvas->Handle, TRANSPARENT);
					PBoxAnt->Canvas->MoveTo(AX, AY); PBoxAnt->Canvas->LineTo(X, Y);
				}
				AX = X; AY = Y;
			}
		}
	}
	DrawWirePara(PBoxAnt, &ant, Grid2->Row - 1);
	if( (ant.StackH > 1)||(ant.StackV > 1) ){
		char bf[32];
		sprintf(bf, "%u×%u スタック", ant.StackH, ant.StackV);
		X = PBoxAnt->Width - PBoxAnt->Canvas->TextWidth(bf);
		PBoxAnt->Canvas->Font->Color = clRed;
		PBoxAnt->Canvas->TextOut(X-2, 0+2, bf);
		PBoxAnt->Canvas->Font->Color = clBlack;
	}
	::SetBkMode(PBoxAnt->Canvas->Handle, Sop);
}
//---------------------------------------------------------------------------
// マウスによるワイヤの選択
int __fastcall TMainWnd::SelectWire(int X, int Y)
{
	int r = FALSE;
	// ワイヤの検索
	double sc = double(TBarSC->Position) / 20.0;
	sc = sc * sc * sc * sc;
	int	Xc = int(PBoxAnt->Width/2 + (exeenv.AntXc * sc));
	int	Yc = int(PBoxAnt->Height/2 + (exeenv.AntYc * sc));
	double x, y;
	double deg = double(TBarDeg->Position);
	deg *= (PAI / 180.0);
	double zdeg = double(TBarZDeg->Position);
	zdeg *= (PAI / 180.0);
	POINT	PP, BP, EP;
	PP.x = X; PP.y = Y;
	int i;
	int mw = 9;
	int w, mi;
	for( i = 0; i < pCalAnt->wmax; i++ ){
		Calc3DXY(x, y, deg, zdeg, pCalAnt->wdef[i].X1, pCalAnt->wdef[i].Y1, pCalAnt->wdef[i].Z1);
		BP.x = int((x * sc)) + Xc;
		BP.y = Yc - int((y * sc));
		Calc3DXY(x, y, deg, zdeg, pCalAnt->wdef[i].X2, pCalAnt->wdef[i].Y2, pCalAnt->wdef[i].Z2);
		EP.x = int((x * sc)) + Xc;
		EP.y = Yc - int((y * sc));
		for( w = 1; w <= mw; w++ ){
			if( IsLine(PP, BP, EP, w) != 0 ){
				mw = w;
				mi = i;
				break;
			}
		}
	}
	if( mw != 9 ){
		if( mi >= ant.wmax ) mi %= ant.wmax;
		Grid2->Row = mi + 1;
		r = TRUE;
	}
	return r;
}
//---------------------------------------------------------------------------
// アンテナ形状表示のマウスイベント
void __fastcall TMainWnd::PBoxAntMouseDown(TObject *Sender, TMouseButton Button,
	TShiftState Shift, int X, int Y)
{
	PBoxAntMX = X;
	PBoxAntMY = Y;
}
//---------------------------------------------------------------------------
// アンテナ形状表示のシングルクリック
void __fastcall TMainWnd::PBoxAntClick(TObject *Sender)
{
	if( SelectWire(PBoxAntMX, PBoxAntMY) == TRUE ){
		PBoxAnt->Invalidate();
	}
}
//---------------------------------------------------------------------------
// アンテナ形状表示のダブルクリック
void __fastcall TMainWnd::PBoxAntDblClick(TObject *Sender)
{
	if( !exeenv.CalcF && (Grid2->Row <= ant.wmax) && (KT1->Enabled == TRUE) ){
		KT1Click(NULL);
	}
}
//---------------------------------------------------------------------------
// ポップアップ－クリック位置を中心にする
void __fastcall TMainWnd::K29Click(TObject *Sender)
{
	double sc = double(TBarSC->Position) / 20.0;
	sc = sc * sc * sc * sc;
	exeenv.AntXc += double((PBoxAnt->Width/2) - PBoxAntMX)/sc;
	exeenv.AntYc += double((PBoxAnt->Height/2) - PBoxAntMY)/sc;
	PBoxAnt->Invalidate();
}
//---------------------------------------------------------------------------
// アンテナ形状表示の原点中心ボタン
void __fastcall TMainWnd::OrgBtnClick(TObject *Sender)
{
	exeenv.AntXc = exeenv.AntYc = 0.0;
	PBoxAnt->Invalidate();
}
//---------------------------------------------------------------------------
// アンテナ形状表示 全景ボタン
void __fastcall TMainWnd::AllViewBtnClick(TObject *Sender)
{
	double	XL, XH, YL, YH, D, x, y;
	int		i, pos;

	if( !ant.wmax ) return;
	double deg = double(TBarDeg->Position);
	deg *= (PAI / 180.0);
	double zdeg = double(TBarZDeg->Position);
	zdeg *= (PAI / 180.0);
	XL = MAXDOUBLE; XH = -MAXDOUBLE;
	YL = MAXDOUBLE; YH = -MAXDOUBLE;
	pos = 0;
	for( i = 0; i < pCalAnt->wmax; i++ ){
		Calc3DXY(x, y, deg, zdeg, pCalAnt->wdef[i].X1, pCalAnt->wdef[i].Y1, pCalAnt->wdef[i].Z1);
		if( XL > x ) XL = x;
		if( XH < x ) XH = x;
		if( YL > y ) YL = y;
		if( YH < y ) YH = y;
		Calc3DXY(x, y, deg, zdeg, pCalAnt->wdef[i].X2, pCalAnt->wdef[i].Y2, pCalAnt->wdef[i].Z2);
		if( XL > x ) XL = x;
		if( XH < x ) XH = x;
		if( YL > y ) YL = y;
		if( YH < y ) YH = y;
		pos = 1;
	}
	if( pos ){
		exeenv.AntXc = -(XL + XH)/2;
		exeenv.AntYc = (YL + YH)/2;
		D = ABS(XH - XL);
		if( D < ABS(YH - YL) ) D = ABS(YH - YL);
		D *= 1.2;
		if( D > 0.0 ){
			D = (PBoxAnt->Height)/D;
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
		exeenv.AntXc = exeenv.AntYc = 0;
		pos = 50;
	}
	TBarSC->Position = pos;
//	AntSC = double(TBarSC->Position) / 20.0;
//	AntSC = AntSC * AntSC * AntSC * AntSC;
//	ScrXc = int(tp->Width/2 + (AntXc * AntSC));
//	ScrYc = int(tp->Height/2 + (AntYc * AntSC));
#if 0
	D = ABS(XL);
	if( D < ABS(XH) ) D = ABS(XH);
	if( D < ABS(YL) ) D = ABS(YL);
	if( D < ABS(YH) ) D = ABS(YH);
	D *= 1.1;
	if( D > 0.0 ){
		D = (PBoxAnt->Height/2) / D;
		pos = int(sqrt(sqrt(D)) * 20.0);
		if( pos > TBarSC->Max ){
			pos = TBarSC->Max;
		}
	}
	else {
		pos = TBarSC->Max;
	}
	TBarSC->Position = pos;
	exeenv.AntXc = exeenv.AntYc = 0.0;
#endif
	PBoxAnt->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TMainWnd::TBarDegChange(TObject *Sender)
{
	PBoxAnt->Invalidate();
}
//---------------------------------------------------------------------------
// パターンの表示イベント
int __fastcall TMainWnd::DrawPtn(void)
{
	DrawPtnH.DrawAngle(res.PtnF);
	DrawPtnV.DrawAngle(res.PtnF);
	if( res.PtnF ){
		if( exeenv.Wave == 3 ){
			TColor c1;
			TColor c2;
			if( res.MaxVP > res.MaxHP ){
				c1 = clBlack;
				c2 = clRed;
			}
			else {
				c1 = clRed;
				c2 = clBlack;
			}
			DrawPtnH.DrawPtn(res.PtnH[0], res.PtnF, c1);
			DrawPtnV.DrawPtn(res.PtnV[0], res.PtnF, c1);
			DrawPtnH.DrawPtn(res.PtnH[1], res.PtnF, c2);
			DrawPtnV.DrawPtn(res.PtnV[1], res.PtnF, c2);
		}
		else {
			DrawPtnH.DrawPtn(res.PtnH[exeenv.Wave], res.PtnF);
			DrawPtnV.DrawPtn(res.PtnV[exeenv.Wave], res.PtnF);
		}
		return TRUE;
	}
	return FALSE;
}
//---------------------------------------------------------------------------
// パターンの表示イベント
void __fastcall TMainWnd::PBoxPtnPaint(TObject *Sender)
{
	if( DrawPtn() == TRUE ){
		int X, Y;
		DrawPtnV.GetBottomPosition(X, Y);
		res.DrawRes(PBoxPtn->Canvas, X, Y+4);
	}
}
//---------------------------------------------------------------------------
// タブシートの変更
void __fastcall TMainWnd::PageChange(TObject *Sender)
{
	if( ant.Flag && !exeenv.CalcF ){
		ant.Flag = 0;
		SetWaitCursor();
		res.Clear();
		GetAntDef();
		exeenv.CalcLog = 1;
		SetStackAnt();
		res.ClearBWC();
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainWnd::DspPlusClick(TObject *Sender)
{
	PBoxAnt->Invalidate();
}
//---------------------------------------------------------------------------
// メディア設定ボタン
void __fastcall TMainWnd::GndSetBtnClick(TObject *Sender)
{
	TMediaDlgBox *Box = new TMediaDlgBox(this);

	if( Box->Execute() == TRUE ){
		// 計算結果無効化
	}
	delete Box;
}
//---------------------------------------------------------------------------
// プログラムの終了（メニュー選択）
void __fastcall TMainWnd::K6Click(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
// 計算結果をセーブする
int __fastcall TMainWnd::SaveResFile(LPCSTR pName)
{
	FILE	*fp;

	if( (fp = fopen(pName, "wb"))!=NULL ){
		fwrite(RESSTR, 1, 16, fp);
		// アンテナ定義を格納
		WriteAntToFp(&ant, antRem, fp);
		// 計算結果を格納
		res.SaveCurData(fp);
		if( fclose(fp) ){
			ErrorMB("'%s'の作成に失敗しました.", pName);
			return FALSE;
		}
		else {
			return TRUE;
		}
	}
	else {
		ErrorMB("'%s'を作成できません.", pName);
	}
	return FALSE;
}
//---------------------------------------------------------------------------
// 計算結果をロードする
int __fastcall TMainWnd::LoadResFile(LPCSTR pName)
{
	FILE		*fp;
	char		bf[256];

	if( (fp = fopen(pName, "rb"))!=NULL ){
		fread(bf, 1, 16, fp);
		int Ver = GetFileVersion(bf, RESSTR);
		if( (Ver != RESVER) && (Ver != 115) ){
			ErrorMB(ILLFMT, pName);
			fclose(fp);
			return FALSE;// フォーマット異常
		}
		InitAntDef();
		ReadAntFromFp(&ant, antRem, fp);
		strcpy(bf, pName);
		SetEXT(bf, ".maa");
		antFname = bf;
		SetAntDef();
		exeenv.CalcLog = 1;
		SetStackAnt();
		ant.Edit = ant.Flag = 0;
		if( Ver == RESVER ){
			res.LoadCurData(fp);
			AntName4->Caption = ant.Name;
			DrawPtnH.SetMaxDB(res.MaxG);
			DrawPtnV.SetMaxDB(res.MaxG);
		}
		else {
			WarningMB("'%s'は古い形式のファイルです.\r\n\r\nアンテナ定義のみ正しく読みこめました.\r\n計算結果は失われていますので再計算して下さい.", pName);
		}
		fclose(fp);
		AllViewBtnClick(NULL);
		UpdateAllViews();
		return TRUE;
	}
	else {
		ErrorMB("'%s'が見つかりません.", pName);
	}
	return FALSE;
}

//---------------------------------------------------------------------------
// 計算結果を開くメニュー
void __fastcall TMainWnd::K12Click(TObject *Sender)
{
	if( CheckSaveAntFile() == FALSE ) return;
	OpenDialog->Title = "計算結果を開く";
	OpenDialog->Filter = "MMANA 計算結果(*.mab)|*.mab|";
	OpenDialog->FileName = "";
	OpenDialog->DefaultExt = "mab";
	OpenDialog->InitialDir = ResDir;
	if( OpenDialog->Execute() == TRUE ){
		if( LoadResFile(AnsiString(OpenDialog->FileName).c_str() ) == TRUE ){
			SetDirName(ResDir, AnsiString(OpenDialog->FileName).c_str());
			if( !exeenv.RecentMAA ) RecentAdd(AnsiString(OpenDialog->FileName).c_str(), TRUE);
		}
	}
}
//---------------------------------------------------------------------------
// 計算結果を保存メニュー
void __fastcall TMainWnd::K11Click(TObject *Sender)
{
	char	bf[256];

	SaveDialog->Title = "計算結果を保存";
	SaveDialog->Filter = "MMANA 計算結果(*.mab)|*.mab|";
	if( *antFname.c_str() ){
		strcpy(bf, antFname.c_str());
		SetEXT(bf, ".mab");
		SaveDialog->FileName = GetFileName(bf);
	}
	else {
		SaveDialog->FileName = "無題";
	}
	SaveDialog->DefaultExt = "mab";
	SaveDialog->InitialDir = ResDir;
	if( SaveDialog->Execute() == TRUE ){
		SaveResFile(AnsiString(SaveDialog->FileName).c_str());
		SetDirName(ResDir, AnsiString(SaveDialog->FileName).c_str());
		if( !exeenv.RecentMAA ) RecentAdd(AnsiString(SaveDialog->FileName).c_str(), TRUE);
	}
}
//---------------------------------------------------------------------------
// 未保存のアンテナファイルを調べて保存する
int __fastcall TMainWnd::CheckSaveAntFile(void)
{
	if( ant.Edit && ant.wmax ){
		int r = ::MessageBox(Handle, "現在のアンテナ定義を保存しますか？", "Mmana", MB_SETFOREGROUND|MB_YESNOCANCEL|MB_ICONQUESTION);
		if( r == IDYES ){
			K4Click(NULL);		// 保存の実行
			return TRUE;
		}
		else if( r == IDCANCEL ){
			return FALSE;
		}
		else {
			ant.Edit = 0;
		}
	}
	return TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TMainWnd::KC1Click(TObject *Sender)
{
	TResCmpDlg *Box = new TResCmpDlg(this);

	Box->Execute(ResColors);
	delete Box;
}
//---------------------------------------------------------------------------
// テキストヘルプの表示
void __fastcall TMainWnd::ShowHelp(LPCSTR pName)
{
	char	bf[2048];
	char	Name[256];
	AnsiString	in;
	FILE	*fp;

	sprintf(Name, "%s%s", BgnDir, pName);
	SetWaitCursor();
	if( (fp = fopen(Name, "rt"))!=NULL ){
		while(!feof(fp)){
			if( fgets(bf, 2048, fp) != NULL ){
				ClipLF(bf);
				in += bf;
				in += "\r\n";
			}
		}
		fclose(fp);
	}
	else {
		ErrorMB("'%s'が見つかりません.", Name);
		return;
	}
	TTextEditDlg *Box = new TTextEditDlg(this);
	Screen->Cursor = crDefault;
	Box->Execute(in, FALSE, pName);
	delete Box;
}
//---------------------------------------------------------------------------
// MMana.Txtの表示メニュー
void __fastcall TMainWnd::K16Click(TObject *Sender)
{
	ShowHelp("Mmana.Txt");
}
//---------------------------------------------------------------------------
void __fastcall TMainWnd::AppendTxt1Click(TObject *Sender)
{
	ShowHelp("Append.Txt");
}
//---------------------------------------------------------------------------
// プログラム終了時の処理
void __fastcall TMainWnd::FormCloseQuery(TObject *Sender, bool &CanClose)
{
	exeenv.CalcAbort = 1;	// 最適化の強制終了
	CanClose = CheckSaveAntFile();
	if( CanClose == TRUE ){
		WriteRegister();
	}
}
//---------------------------------------------------------------------------
// アプリケーションのサイズ変更イベント
void __fastcall TMainWnd::FormResize(TObject *Sender)
{
	AlignList.NewAlign(Page->ActivePage);
	AlignGrid[0].NewAlign(Grid1);
	AlignGrid[1].NewAlign(Grid2);
	AlignGrid[2].NewAlign(Grid3);
	AlignGrid[3].NewAlign(Grid4);

	DrawPtnH.SetRect(PBoxPtn->Canvas, 0, 0, PBoxPtn->Width / 2 - 2, PBoxPtn->Height);
	DrawPtnV.SetRect(PBoxPtn->Canvas, (PBoxPtn->Width/2) + 2, 0, PBoxPtn->Width, PBoxPtn->Height);
	UpdateAllViews();
}
//---------------------------------------------------------------------------
// サイズ変更コントロールの登録
void __fastcall TMainWnd::EntryAlignControl(void)
{
	TControl *BasicControl = TabSheet1;

	AlignList.EntryControl(PBoxAnt, BasicControl, PBoxAnt->Font);
	AlignList.EntryControl(PBoxPtn, BasicControl, PBoxPtn->Font);
	AlignList.EntryControl(CalMemo, BasicControl, CalMemo->Font);
	AlignList.EntryControl(Grid1, BasicControl, Grid1->Font);
	AlignList.EntryControl(CalTrgBtn, BasicControl, CalTrgBtn->Font);
	AlignList.EntryControl(Label1, BasicControl, Label1->Font);
	AlignList.EntryControl(Grid3, BasicControl, Grid3->Font);
	AlignList.EntryControl(WireCnt, BasicControl, WireCnt->Font);
	AlignList.EntryControl(CurCnt, BasicControl, CurCnt->Font);
	AlignList.EntryControl(Grid4, BasicControl, Grid4->Font);
	AlignList.EntryControl(LoadCnt, BasicControl, LoadCnt->Font);
	AlignList.EntryControl(Grid2, BasicControl, Grid2->Font);
	AlignList.EntryControl(AntName, BasicControl, AntName->Font);
	AlignList.EntryControl(Label5, BasicControl, Label5->Font);
	AlignList.EntryControl(AutoVol, BasicControl, AutoVol->Font);
	AlignList.EntryControl(Freq, BasicControl, Freq->Font);
	AlignList.EntryControl(Label6, BasicControl, Label6->Font);
	AlignList.EntryControl(EnbLoad, BasicControl, EnbLoad->Font);
	AlignList.EntryControl(TBarDeg, BasicControl, NULL);
	AlignList.EntryControl(TBarZDeg, BasicControl, NULL);
	AlignList.EntryControl(TBarSC, BasicControl, NULL);
	AlignList.EntryControl(AntName2, BasicControl, AntName2->Font);
	AlignList.EntryControl(Label7, BasicControl, Label7->Font);
	AlignList.EntryControl(Label8, BasicControl, Label8->Font);
	AlignList.EntryControl(DspCur, BasicControl, DspCur->Font);
	AlignList.EntryControl(DoFreq, BasicControl, DoFreq->Font);
	AlignList.EntryControl(Label9, BasicControl, Label9->Font);
	AlignList.EntryControl(Label10, BasicControl, Label10->Font);
	AlignList.EntryControl(CalcSel, BasicControl, CalcSel->Font);
	AlignList.EntryControl(AntName3, BasicControl, AntName3->Font);
	AlignList.EntryControl(GndSetBtn, BasicControl, GndSetBtn->Font);
	AlignList.EntryControl(AntHeight, BasicControl, AntHeight->Font);
	AlignList.EntryControl(Label11, BasicControl, Label11->Font);
	AlignList.EntryControl(Label12, BasicControl, Label12->Font);
	AlignList.EntryControl(DspPlus, BasicControl, DspPlus->Font);
	AlignList.EntryControl(TBarCur, BasicControl, NULL);
	AlignList.EntryControl(Label13, BasicControl, Label13->Font);
	AlignList.EntryControl(AntName4, BasicControl, AntName4->Font);
	AlignList.EntryControl(Label2, BasicControl, Label2->Font);
	AlignList.EntryControl(EditDM1, BasicControl, EditDM1->Font);
	AlignList.EntryControl(Label3, BasicControl, Label3->Font);
	AlignList.EntryControl(Label4, BasicControl, Label4->Font);
	AlignList.EntryControl(EditDM2, BasicControl, EditDM2->Font);
	AlignList.EntryControl(Label14, BasicControl, Label14->Font);
	AlignList.EntryControl(EditEC, BasicControl, EditEC->Font);
	AlignList.EntryControl(Label16, BasicControl, Label16->Font);
	AlignList.EntryControl(EditSC, BasicControl, EditSC->Font);
	AlignList.EntryControl(OrgBtn, BasicControl, OrgBtn->Font);
	AlignList.EntryControl(AllViewBtn, BasicControl, AllViewBtn->Font);
	AlignList.EntryControl(ACalBtn, BasicControl, ACalBtn->Font);
	AlignList.EntryControl(EleEditBtn, BasicControl, EleEditBtn->Font);
	AlignList.EntryControl(WireCadBtn, BasicControl, WireCadBtn->Font);
	AlignList.EntryControl(ACalResBtn, BasicControl, ACalResBtn->Font);
	AlignList.EntryControl(BwBtn, BasicControl, BwBtn->Font);
	AlignList.EntryControl(PrintBtn, BasicControl, PrintBtn->Font);
	AlignList.EntryControl(ChkWith, BasicControl, ChkWith->Font);
	AlignList.EntryControl(ChkRmd, BasicControl, ChkRmd->Font);
	AlignList.EntryControl(WaveSel, BasicControl, WaveSel->Font);
	AlignList.EntryControl(EleBtn, BasicControl, EleBtn->Font);
	AlignList.EntryControl(WireSel, BasicControl, WireSel->Font);
	AlignList.EntryControl(Label15, BasicControl, Label15->Font);
	AlignList.EntryControl(DocBtn, BasicControl, DocBtn->Font);
	AlignList.EntryControl(StackBtn, BasicControl, StackBtn->Font);
	AlignList.EntryControl(Label17, BasicControl, Label17->Font);
	AlignList.EntryControl(Ant3D, BasicControl, Ant3D->Font);

	AlignGrid[0].InitGrid(Grid1);
	AlignGrid[1].InitGrid(Grid2);
	AlignGrid[2].InitGrid(Grid3);
	AlignGrid[3].InitGrid(Grid4);

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
	Page->Align = alClient;
	FormCenter(this, CX, CY);
}
//---------------------------------------------------------------------------
// 計算環境が変更された場合
void __fastcall TMainWnd::DoFreqChange(TObject *Sender)
{
	ant.Flag = 1;
	res.ClearBWC();
}
//---------------------------------------------------------------------------
// 設計周波数の変更
void __fastcall TMainWnd::FreqChange(TObject *Sender)
{
	double	fq;
	if( Calc(fq, AnsiString(Freq->Text).c_str()) == TRUE ){
		if( (fq != ant.fq) && (fq > 0) ){
			ant.fq = fq;
			ant.Edit = ant.Flag = 1;
			DoFreq->Text = Freq->Text;
			exeenv.rmd = 299.8/ant.fq;
			if( (ant.StackH == 1)&&(ant.StackV == 1) ){
				ant.StackHW = ant.StackVW = exeenv.rmd;
			}
			if( exeenv.RmdSel ){
				Grid2->Invalidate();
			}
		}
	}
}
//---------------------------------------------------------------------------
// アンテナ名の変更
void __fastcall TMainWnd::AntNameChange(TObject *Sender)
{
	if( strcmp(ant.Name, AnsiString(AntName->Text).c_str()) ){
		strncpy(ant.Name, AnsiString(AntName->Text).c_str(), sizeof(ant.Name));
		AntName2->Caption = ant.Name;
		AntName3->Caption = ant.Name;
		ant.Edit = 1;
	}
}
//---------------------------------------------------------------------------
// DM1の変更
void __fastcall TMainWnd::EditDM1Change(TObject *Sender)
{
	int		dm1;

	CalcU(dm1, AnsiString(EditDM1->Text).c_str());
	if( dm1 != ant.DM1 ){
		ant.DM1 = dm1;
		if( ant.wmax ) ant.Edit = ant.Flag = 1;
	}
}
//---------------------------------------------------------------------------
// DM2の変更
void __fastcall TMainWnd::EditDM2Change(TObject *Sender)
{
	int		dm2;

	CalcU(dm2, AnsiString(EditDM2->Text).c_str());
	if( dm2 != ant.DM2 ){
		ant.DM2 = dm2;
		if( ant.wmax ) ant.Edit = ant.Flag = 1;
	}

}
//---------------------------------------------------------------------------
// ATの変更
void __fastcall TMainWnd::EditECChange(TObject *Sender)
{
	int		at;

	CalcU(at, AnsiString(EditEC->Text).c_str());
	if( at != ant.EC ){
		if( at <= 0 ) at = 1;
		ant.EC = at;
		if( ant.wmax ) ant.Edit = ant.Flag = 1;
	}
}
//---------------------------------------------------------------------------
// SCの変更
void __fastcall TMainWnd::EditSCChange(TObject *Sender)
{
	double	sc;

	if( Calc(sc, AnsiString(EditSC->Text).c_str()) == TRUE ){
		if( sc != ant.SC ){
			ant.SC = sc;
			if( ant.wmax ) ant.Edit = ant.Flag = 1;
		}
	}
}
//---------------------------------------------------------------------------
// ロードの有効/無効の変更
void __fastcall TMainWnd::EnbLoadClick(TObject *Sender)
{
	if( ant.lenb != EnbLoad->Checked ){
		ant.lenb = EnbLoad->Checked;
		if( ant.wmax && ant.lmax ){
			ant.Edit = ant.Flag = 1;
		}
	}
}
//---------------------------------------------------------------------------
// エレメント編集メニュー
void __fastcall TMainWnd::K18Click(TObject *Sender)
{
	TGrpWireDlg *Box = new TGrpWireDlg(this);

	if( Box->Execute(&ant) == TRUE ){
		UpdateAntData();
	}
	delete Box;
}
//---------------------------------------------------------------------------
// 最適化ボタン
void __fastcall TMainWnd::ACalBtnClick(TObject *Sender)
{
	if( exeenv.CalcF ){		// 計算実行中の時
		exeenv.CalcAbort = 1;
		return;
	}
	else {
		exeenv.CalcAbort = 0;
	}
	GetAntDef();
	if( ant.cfq == 0.0 ) ant.cfq = ant.fq;
	if( pACal == NULL ){
		pACal = new CACal;
	}
	if( pACal != NULL ){
		if( ant.cmax && (pACal->atrg.bmax <= 1) ){
			memset(pACal->atrg.bdef, 0, sizeof(pACal->atrg.bdef));
			pACal->atrg.bmax = 1;
			pACal->atrg.bdef[0].fq = ant.cfq ? ant.cfq : ant.fq;
			for( int i = 0; (i < ant.cmax) && (i < 4); i++ ){
				memcpy(&pACal->atrg.bdef[0].cdef[i], &ant.cdef[i], sizeof(CDEF));
			}
		}
		pACal->Init(&ant, &env);
		TACalDlg *Box = new TACalDlg(this);
		if( Box->Execute(pACal) == TRUE ){
			CalMemo->Lines->Clear();
			res.IncResP();
			res.GetResP()->TYPE = env.type;
			res.GetResP()->HEIGHT = env.antheight;
			res.GetResP()->FBR = env.fbr;
			res.ClearBWC();
			Grid1->RowCount = res.GetCnt() + (res.GetCnt() ? 1 : 2);
			AntName4->Caption = ant.Name;
			SaveAntStrings(antDef);		// 最適化前状態を保存
			pACal->DispStat(CalMemo);
			exeenv.CalcF = 1;
			DisCalcUI();
			Application->ProcessMessages();
			BgnTime = ::GetTickCount();
			int r = pACal->Execute();	// 最適化計算
			EndTime = ::GetTickCount();
			LogPrint("%.2lf(s)", double(EndTime - BgnTime)/1000.0);
			ant.Edit = 1;
			exeenv.CalcF = 0;
			res.SetRes(&pACal->maxres);
			res.InitBWC();
			DrawPtnH.SetMaxDB(res.MaxG);
			DrawPtnV.SetMaxDB(res.MaxG);
			SetStackAnt();
			res.SetCalc(ant.Name);		// 計算完了フラグ
			DoFreq->Text = StrDbl(ant.cfq);
			AntHeight->Text = StrDbl(env.antheight);
			if( (r == TRUE) && (YesNoMB("最適化を終了しました.結果を最適化シートに保存しますか？")== IDYES) ){
				K20Click(NULL);
			}
			DisCalcUI();
			UpdateAllViews();
		}
		delete Box;
	}
}

//---------------------------------------------------------------------------
// 最適化ボタン
void __fastcall TMainWnd::DrawPtnACalInfo(void)
{
	if( pACal == NULL ) return;
	if( Page->ActivePage == TabSheet5 ){	// パターン図表示中
		int SaveSize = PBoxPtn->Canvas->Font->Size;
		PBoxPtn->Canvas->Font->Size = SaveSize*2/3;
		int X = PBoxPtn->Width - PBoxPtn->Canvas->TextWidth(pACal->Info);
		int FH = PBoxPtn->Canvas->TextHeight(pACal->Info);
		int Y = PBoxPtn->Height - FH - FH/2;
		int Sop = ::SetBkMode(PBoxPtn->Canvas->Handle, TRANSPARENT);
		PBoxPtn->Canvas->TextOut(X, Y, pACal->Info);
		Y -= FH;
		::SetBkMode(PBoxPtn->Canvas->Handle, TRANSPARENT);
		PBoxPtn->Canvas->TextOut(X, Y, pACal->InfoTTL);
		::SetBkMode(PBoxPtn->Canvas->Handle, Sop);
		PBoxPtn->Canvas->Font->Size = SaveSize;
	}
	else if( Page->ActivePage == TabSheet2 ){	// アンテナ形状
		int X = 5;
		int FH = PBoxAnt->Canvas->TextHeight(pACal->Info);
		int Y = PBoxAnt->Height - FH - FH/2;
		int Sop = ::SetBkMode(PBoxAnt->Canvas->Handle, TRANSPARENT);
		PBoxAnt->Canvas->TextOut(X, Y, pACal->Info);
		Y -= FH;
		::SetBkMode(PBoxAnt->Canvas->Handle, TRANSPARENT);
		PBoxAnt->Canvas->TextOut(X, Y, pACal->InfoTTL);
		::SetBkMode(PBoxAnt->Canvas->Handle, Sop);
	}
}
//---------------------------------------------------------------------------
// CACalクラスの最適化の実行ループ
int CACal::Exec(void)
{
	int n, pf, f, init, mv, b;
	RESL *rp;
	// 一番最初のパラメータの設定
	SetBand(&MainWnd->ant, 0);		// 最初の給電情報を設定
	for( n = 0; n < amax; n++ ){
		SetPara(n);
	}
	AVAL *ap;
	double	MaxV = -MAXDOUBLE;
	double	MaxD;
	init = 0;
	int	N=1;
	int ContN;
	while(1){
		for( ap = aval, f = n = 0; n < amax; n++, ap++ ){
			if( ap->With == -1 ){		// 独立変数の時
				double	dir = 1.0;
				atrg.AM = MaxV;
				MaxD = ap->DC;
				if( ap->DI > ap->DIO ) ap->DI = ap->DIO;
				pf = 0;
				ContN = 0;
				while(1){
					if( init ){
						ap->DC += (ap->DI * dir);
						if( SetMinMax(ap, ap->DC) ){
							ap->DC = MaxD;				// 最大時の値に設定
							SetMinMax(ap, ap->DC);      // 調整
							if( dir >= 1.0 ){
								dir = -1;
								if( ap->DI > ap->DIO ) ap->DI = ap->DIO;
								ContN = 0;
								continue;
							}
							else {
								SetPara(n);
								break;
							}
						}
					}
					SetPara(n);
					if( IsSameMax() ){						// 組み合わせが最大時と同じ時
						atrg.V = atrg.VM;
						mv = TRUE;
					}
					else if( (mv=IsMV(atrg.V))==FALSE ){	// 記憶リストに存在しない時
						WList.Update();
						for( atrg.V = 0.0, b = 0; b < atrg.bmax; b++ ){
							if( atrg.bmax != 1 ) SetBand(&MainWnd->ant, b);
							if( MainWnd->ACalExecute() != TRUE ) return FALSE;
							rp = res.GetResP();
							if( !b ) SetNowCur(rp);
							BV[b] = CalcV(rp, &res);
							if( b == (atrg.bmax - 1) ){
								atrg.V = BV[0];
								for( int bb = 1; bb < atrg.bmax; bb++ ){
									if( atrg.bweight ){
										atrg.V += BV[b]/atrg.bmax;
									}
									else {
										atrg.V += BV[b];
									}
								}
							}
							if( exeenv.CalcDisp ){
								if( (N==1)||(!(N % 10)) ){
									if( (N != 1) && !(N % 400) ){
										MainWnd->CalMemo->Clear();
									}
									sprintf(InfoTTL, "%s Val  Para       R      jX    SWR   Ga    F/B   %s", (N >= 100) ? "   ":"  ", (atrg.CurCur < 0) ? "El":"Cur");
									LogPrint(InfoTTL);
								}
								sprintf(Info, "%2d %2d %9.4lf %6.1lf %6.1lf %6.2lf %5.2lf %5.2lf %4.1lf %c%s",
									N++, n+1, ap->DC,
									rp->R, rp->JX,
									rp->SWR, rp->GAIN, rp->FB,
									(atrg.CurCur < 0) ? rp->ELV : atrg.CurCur * 1000.0,
									(((b == (atrg.bmax - 1))&&(atrg.V > atrg.VM)) ? '*':' '),
									(atrg.bmax != 1) ? StrDbl(MainWnd->ant.cfq):""
								);
								LogPrint(Info);
								MainWnd->DrawPtnACalInfo();
							}
							if( (b != (atrg.bmax - 1)) && exeenv.CalcAbort ) return FALSE;
						}
						if( atrg.VM < atrg.V ){
							atrg.VM = atrg.V;
							for( int i = 0; i < amax; i++ ){	// 最大時のパラメータを記憶
								atrg.AD[i] = aval[i].DC;
							}
							maxres.CopyLastRes(&res);
							f = 1;
						}
					}
					if( mv == FALSE ) WriteMV();
					if( exeenv.CalcAbort ) return FALSE;
					if( MaxV < atrg.V ){
						MaxV = atrg.V;
						MaxD = ap->DC;
					}
					if( !atrg.TF ){
						init = 1;
						continue;
					}
					if( atrg.AM < atrg.V ){		// 増加傾向の時
						atrg.AM = atrg.V;
						if( init ){
							if( !pf ) pf = 1;
						}
						if( pf == 1 ){
							if( ContN >= 8 ){
								ap->DI = ap->DIO * 32.0;
							}
							else {
								if( ap->DI < (ap->DIO * 16.0) ) ap->DI = ap->DI * 2.0;
							}
						}
						else if( pf == 2 ){
							ap->DC = MaxD;				// 最大時の値に設定
							SetPara(n);
							break;
						}
					}
					else if( (pf == 1) && (ap->DI >= ap->DIO * 16.0) ){
						ap->DC = MaxD;
						ap->DI = ap->DI / 4;
						ContN = 0;
						pf = 2;
					}
					else if( pf ){				// 一旦増加を検出している時は終了
						ap->DC = MaxD;				// 最大時の値に設定
						SetPara(n);
						break;
					}
					else if( dir >= 1.0 ){		// ベクトルがプラスの時はマイナスに設定
						dir = -1;
						ap->DC = MaxD;				// 最初の値に戻す
						if( ap->DI > ap->DIO ) ap->DI = ap->DIO;
						ContN = 0;
					}
					else {
						ap->DC = MaxD;				// 最大時の値に設定
						SetPara(n);
						break;
					}
					init = 1;
					ContN++;
				}
			}
		}
		if( (!f) || (!atrg.TF) ) break;
	}
	return TRUE;
}
//---------------------------------------------------------------------------
// 最適化時の１つの実行処理
int __fastcall TMainWnd::ACalExecute(void)
{
	ant.Flag = 0;
	exeenv.CalcLog = 0;
	if( SetStackAnt() == TRUE ){
		if( (!env.type) || ((pCalAnt->MinZ + env.antheight) >= 0.0) || (YesNoMB( "一部のワイヤが地中に埋まっています. 計算を中止しますか ?") == IDNO) ){
			res.GetResP()->HEIGHT = env.antheight;
			if( pACal->DegUnit ){
				CalCurrentDR();			// ２°分解能
			}
			else {
				CalCurrent();			// １°分解能
			}
			DrawPtnH.SetMaxDB(res.MaxG);
			DrawPtnV.SetMaxDB(res.MaxG);
			res.CalcF = 1;
			if( Page->ActivePage == TabSheet1 ){		// ワイヤ定義
				Grid2->Invalidate();
				Grid4->Invalidate();
			}
			else if( Page->ActivePage == TabSheet2 ){	// ANT形状
				PBoxAnt->Invalidate();
			}
			else if( Page->ActivePage == TabSheet3 ){	// 計算画面
				Grid1->Invalidate();
				DoFreq->Text = StrDbl(ant.cfq);
				AntHeight->Text = StrDbl(env.antheight);
			}
			else if( Page->ActivePage == TabSheet5 ){	// パターン図
				PBoxPtn->Invalidate();
			}
			Application->ProcessMessages();
			return TRUE;
		}
		else {
			return FALSE;
		}
	}
	return FALSE;
}

//---------------------------------------------------------------------------
// 実行時の更新禁止
void __fastcall TMainWnd::DisCalcUI(void)
{
	int f;
	if( exeenv.CalcF ){
		f = FALSE;
		CalTrgBtn->Enabled = f;
		ACalBtn->Caption = "最適化中止";
	}
	else {
		ACalBtn->Caption = "最適化";
		f = TRUE;
	}
	ACalResBtn->Enabled = f;
	TabSheet1->Enabled = f;
	K1->Enabled = f;
	KE1->Enabled = f;
	KV1->Enabled = f;
	KH1->Enabled = f;
	EleEditBtn->Enabled = f;
	WireCadBtn->Enabled = f;
	DoFreq->Enabled = f;
	GndSetBtn->Enabled = f;
	CalcSel->Enabled = f;
	AntHeight->Enabled = f;
	BwBtn->Enabled = f;
	PrintBtn->Enabled = f;
	EleBtn->Enabled = f;
	WireSel->Enabled = f;
}
//---------------------------------------------------------------------------
// 最適化結果一覧
void __fastcall TMainWnd::ACalResBtnClick(TObject *Sender)
{
	if( (pACal == NULL) || (!pACal->GetMV()) ) return;
	pACal->Init(&ant, &env);
	TACalResDlg *Box = new TACalResDlg(this);
	if( Box->Execute(pACal) == TRUE ){
		ant.Edit = 1;
		res.PtnF = 0;
		res.Clear();
		SetAntDef();
		exeenv.CalcLog = 1;
		DoFreq->Text = StrDbl(ant.cfq);
		AntHeight->Text = StrDbl(env.antheight);
		SetStackAnt();
		res.ClearBWC();
		UpdateAllViews();
	}
	delete Box;
}
//---------------------------------------------------------------------------
// 最適化シートをセーブする
int __fastcall TMainWnd::SaveACalFile(LPCSTR pName)
{
	FILE	*fp;

	if( (fp = fopen(pName, "wb"))!=NULL ){
		fwrite(ACALSTR, 1, 16, fp);
		// アンテナ定義を格納
		WriteAntToFp(&ant, antRem, fp);
		// 最適化シートを格納
		pACal->WriteToFp(fp);
		if( fclose(fp) ){
			ErrorMB("'%s'の作成に失敗しました.", pName);
			return FALSE;
		}
		else {
			return TRUE;
		}
	}
	else {
		ErrorMB("'%s'を作成できません.", pName);
	}
	return FALSE;
}
//---------------------------------------------------------------------------
// 最適化シートをセーブする
int __fastcall TMainWnd::LoadACalFile(LPCSTR pName)
{
	AnsiString	in;
	FILE		*fp;
	char		bf[256];

	if( (fp = fopen(pName, "rb"))!=NULL ){
		fread(bf, 1, 16, fp);
		if( strcmp(bf, ACALSTR) ){
			ErrorMB(ILLFMT, pName);
			fclose(fp);
			return FALSE;// フォーマット異常
		}
		InitAntDef();
		ReadAntFromFp(&ant, antRem, fp);
		strcpy(bf, pName);
		SetEXT(bf, ".maa");
		antFname = bf;
		SetAntDef();
		exeenv.CalcLog = 1;
		SetStackAnt();
		res.ClearBWC();
		ant.Edit = ant.Flag = 0;
		if( pACal == NULL ){
			pACal = new CACal;
		}
		pACal->ReadFromFp(fp);
		pACal->Init(&ant, &env);
		AntName4->Caption = ant.Name;
		res.Clear();
		res.PtnF = 0;
		fclose(fp);
		AllViewBtnClick(NULL);
		UpdateAllViews();
		return TRUE;
	}
	else {
		ErrorMB("'%s'が見つかりません.", pName);
	}
	return FALSE;
}
//---------------------------------------------------------------------------
// 最適化シートを開く
void __fastcall TMainWnd::K19Click(TObject *Sender)
{
	if( CheckSaveAntFile() == FALSE ) return;
	OpenDialog->Title = "最適化シートを開く";
	OpenDialog->Filter = "MMANA 最適化シート(*.mao)|*.mao|";
	OpenDialog->FileName = "";
	OpenDialog->DefaultExt = "mao";
	OpenDialog->InitialDir = ResDir;
	if( OpenDialog->Execute() == TRUE ){
		if( LoadACalFile(AnsiString(OpenDialog->FileName).c_str() ) == TRUE ){	// ja7ude 1.0
			SetDirName(ResDir, AnsiString(OpenDialog->FileName).c_str());	// ja7ude 1.0
			if( !exeenv.RecentMAA ) RecentAdd(AnsiString(OpenDialog->FileName).c_str(), TRUE);
		}
	}
}
//---------------------------------------------------------------------------
// 最適化シートを保存
void __fastcall TMainWnd::K20Click(TObject *Sender)
{
	char	bf[256];

	SaveDialog->Title = "最適化シートを保存";
	SaveDialog->Filter = "MMANA 最適化シート(*.mao)|*.mao|";
	if( *antFname.c_str() ){
		strcpy(bf, antFname.c_str());
		SetEXT(bf, ".mao");
		SaveDialog->FileName = GetFileName(bf);
	}
	else {
		SaveDialog->FileName = "無題";
	}
	SaveDialog->DefaultExt = "mao";
	SaveDialog->InitialDir = ResDir;
	if( SaveDialog->Execute() == TRUE ){
		SaveACalFile(AnsiString(SaveDialog->FileName).c_str());	// ja7ude 1.0
		SetDirName(ResDir, AnsiString(SaveDialog->FileName).c_str());
		if( !exeenv.RecentMAA ) RecentAdd(AnsiString(SaveDialog->FileName).c_str(), TRUE);
	}
}
//---------------------------------------------------------------------------
// 電卓の実行
void __fastcall TMainWnd::K17Click(TObject *Sender)
{
	if( DoCalc() == FALSE ){
		K17->Enabled = FALSE;
	}
}
//---------------------------------------------------------------------------
// 共振条件メニュー
void __fastcall TMainWnd::K21Click(TObject *Sender)
{
	TOptDlgBox *Box = new TOptDlgBox(this);

	double fq = ant.cfq;
	double r = 50.0;
	double x = 0;
	double l = 0;
	double c = 0;
	if( !fq ){
		fq = ant.fq;
	}
	if( Grid1->Row ){
		RESL	*rp = res.GetResP(Grid1->Row-1);
		r = rp->R;
		x = rp->JX;
	}
	else {
		res.GetLastRes(r, x);
	}
	if( Page->ActivePage == TabSheet1 ){
		if( ActiveControl == Grid4 ){
			if( Grid4->Row && (Grid4->Row <= ant.lmax) ){
				LDEF *lp = &ant.ldef[Grid4->Row - 1];
				switch(lp->TYPE){
					case 0:		// LC
						l = lp->A[0];
						c = lp->B[0];
						x = 0;
						break;
					case 1:		// R+jx
						r = lp->A[0];
						x = lp->B[0];
						break;
				}
			}
		}
	}

	int FontSize = env.FontSize;
	int rr = Box->Execute(fq, r, x, l, c);
	if( rr == 2 ){
		exeenv.CalcLog = 1;
		SetStackAnt();
		res.ClearBWC();
	}
	if( rr == TRUE ){
		UpdateRecentMenu();
		UpdateAllViews();
	}
	if( FontSize != env.FontSize ){
		FormResize(NULL);
	}
	delete Box;
}
//---------------------------------------------------------------------------
// ＸとＹの交換
void __fastcall TMainWnd::KXYClick(TObject *Sender)
{
	if( YesNoMB("すべてのＸ座標とＹ座標を入れ替えます(上から見て９０°回転します).\r\n\r\nほんまに実行しますか？" ) != IDYES ) return;
	double	s;
	WDEF	*wp = ant.wdef;
	int	i;
	for( i = 0; i < ant.wmax; i++, wp++ ){
		s = wp->X1;
		wp->X1 = wp->Y1;
		wp->Y1 = s;
		s = wp->X2;
		wp->X2 = wp->Y2;
		wp->Y2 = s;
	}
	ant.Edit = 1;
	exeenv.CalcLog = 1;
	SetStackAnt();
	res.ClearBWC();
	ant.Flag = 0;
	UpdateAllViews();
}
//---------------------------------------------------------------------------
// ＸとＺの交換
void __fastcall TMainWnd::KXZ1Click(TObject *Sender)
{
	if( YesNoMB("すべてのＸ座標とＺ座標を入れ替えます(横から見て９０°回転します).\r\n\r\nほんまに実行しますか？" ) != IDYES ) return;
	double	s;
	WDEF	*wp = ant.wdef;
	int	i;
	for( i = 0; i < ant.wmax; i++, wp++ ){
		s = wp->X1;
		wp->X1 = wp->Z1;
		wp->Z1 = s;
		s = wp->X2;
		wp->X2 = wp->Z2;
		wp->Z2 = s;
	}
	ant.Edit = 1;
	exeenv.CalcLog = 1;
	SetStackAnt();
	res.ClearBWC();
	ant.Flag = 0;
	UpdateAllViews();
}
//---------------------------------------------------------------------------
// ＹとＺの交換
void __fastcall TMainWnd::KYZ1Click(TObject *Sender)
{
	if( YesNoMB("すべてのＹ座標とＺ座標を入れ替えます(正面から見て９０°回転します).\r\n\r\nほんまに実行しますか？" ) != IDYES ) return;
	double	s;
	WDEF	*wp = ant.wdef;
	int	i;
	for( i = 0; i < ant.wmax; i++, wp++ ){
		s = wp->Z1;
		wp->Z1 = wp->Y1;
		wp->Y1 = s;
		s = wp->Z2;
		wp->Z2 = wp->Y2;
		wp->Y2 = s;
	}
	ant.Edit = 1;
	exeenv.CalcLog = 1;
	SetStackAnt();
	res.ClearBWC();
	ant.Flag = 0;
	UpdateAllViews();
}
//---------------------------------------------------------------------------
// 拡大・縮小
void __fastcall TMainWnd::KS4Click(TObject *Sender)
{
	TWireScaleDlg *Box = new TWireScaleDlg(this);

	if( Box->Execute(&ant) == TRUE ){
		UpdateAntData();
	}
	delete Box;
}
//---------------------------------------------------------------------------
void __fastcall TMainWnd::AllRoundUp(double sc)
{
	if( YesNoMB("ワイヤ、集中定数の小数点を丸めます. ほんまにやりますか？") != IDYES ) return;
	int i, j;
	WDEF *wp = ant.wdef;
	for( i = 0; i < ant.wmax; i++, wp++ ){
		wp->X1 = SetRmdVal(RoundUp(GetRmdVal(wp->X1), sc));
		wp->Y1 = SetRmdVal(RoundUp(GetRmdVal(wp->Y1), sc));
		wp->Z1 = SetRmdVal(RoundUp(GetRmdVal(wp->Z1), sc));
		wp->X2 = SetRmdVal(RoundUp(GetRmdVal(wp->X2), sc));
		wp->Y2 = SetRmdVal(RoundUp(GetRmdVal(wp->Y2), sc));
		wp->Z2 = SetRmdVal(RoundUp(GetRmdVal(wp->Z2), sc));
		if( exeenv.RmdSel ){
			wp->R = SetRmdVal(RoundUp(GetRmdVal(wp->R), sc));
			if( !wp->R ) wp->R = SetRmdVal(1/sc);
		}
		else {
			wp->R = RoundUp(wp->R, sc * 1000.0);
			if( !wp->R ) wp->R = 1/sc*1000.0;
		}
	}
	for( i = 0; i < ant.lmax; i++ ){
		for( j = 0; j < SMAX; j++ ){
			if( ant.ldef[i].A[j] != NULLV ){
				ant.ldef[i].A[j] = RoundUp(ant.ldef[i].A[j], sc);
			}
			if( ant.ldef[i].B[j] != NULLV ){
				ant.ldef[i].B[j] = RoundUp(ant.ldef[i].B[j], sc);
			}
		}
	}
	ant.Edit = 1;
	exeenv.CalcLog = 1;
	SetStackAnt();
	res.ClearBWC();
	ant.Flag = 0;
	UpdateAllViews();
}
//---------------------------------------------------------------------------
void __fastcall TMainWnd::K23Click(TObject *Sender)
{
	AllRoundUp(100);
}
//---------------------------------------------------------------------------
void __fastcall TMainWnd::K24Click(TObject *Sender)
{
	AllRoundUp(1000);
}
//---------------------------------------------------------------------------
void __fastcall TMainWnd::K25Click(TObject *Sender)
{
	AllRoundUp(10000);
}
//---------------------------------------------------------------------------
void __fastcall TMainWnd::K26Click(TObject *Sender)
{
	AllRoundUp(100000);
}
//---------------------------------------------------------------------------
// 三角形編集
void __fastcall TMainWnd::KT1Click(TObject *Sender)
{
#if 1
	TWireEditDlg *Box = new TWireEditDlg(this);

	if( Box->Execute(ant.wdef, Grid2->Row-1, ant.wmax) == TRUE ){
		UpdateAntData();
	}
	delete Box;
#else
	TTriEditDlg *Box = new TTriEditDlg(this);

	if( Box->Execute(&ant, Grid2->Row-1) == TRUE ){
		UpdateAntData();
	}
	delete Box;
#endif
}
//---------------------------------------------------------------------------
void __fastcall TMainWnd::BwBtnClick(TObject *Sender)
{
	TBwDispDlg *Box = new TBwDispDlg(this);

	if( Box->Execute(&ant, ResColors) == TRUE ){
		DrawPtnH.SetMaxDB(res.MaxG);
		DrawPtnV.SetMaxDB(res.MaxG);
		UpdateAllViews();
	}
	delete Box;
}
//---------------------------------------------------------------------------
// パターン図の印刷
void __fastcall TMainWnd::PrintBtnClick(TObject *Sender)
{
	if( PrintDialog->Execute() == TRUE ){
		SetWaitCursor();
		Printer()->BeginDoc();
		Printer()->Canvas->Font->Name = "ＭＳ ゴシック";
		CDrawPtnH	DrawH;
		CDrawPtnV	DrawV;
		DrawH.gCol = clBlack;
		DrawV.gCol = clBlack;
		TRect rc = Printer()->Canvas->ClipRect;

		int pw = rc.Right - rc.Left;
		int ph = rc.Bottom - rc.Top;
		Printer()->Canvas->Font->Height = sqrt(pw * ph)/45;

		rc.Left = (pw/20);
		rc.Top = (ph/20);
		ph = (ph * 18)/20;
		pw = (pw * 18)/20;
		int Y = rc.Top;
		int FH = Printer()->Canvas->TextHeight("A");
		Printer()->Canvas->TextOut(rc.Left, Y, VERSTR);
		Y += FH + FH;
		Printer()->Canvas->TextOut(rc.Left, Y, AntName4->Caption);
		Y += FH;
		rc.Top = Y;
		ph -= (FH * 3);
		if( pw > ph ){
			DrawH.SetRect(Printer()->Canvas, rc.Left, rc.Top, rc.Left + (pw/2) - FH, rc.Top + ph);
			DrawV.SetRect(Printer()->Canvas, rc.Left + (pw/2) + FH, rc.Top, rc.Left + pw, rc.Top + ph);
		}
		else {
			DrawH.SetRect(Printer()->Canvas, rc.Left, rc.Top, rc.Left + pw, rc.Top + (ph/2) - FH);
			DrawV.SetRect(Printer()->Canvas, rc.Left, rc.Top + (ph/2) + FH, rc.Left + pw, rc.Top + ph);
		}
		DrawH.SetMaxDB(res.MaxG);
		DrawV.SetMaxDB(res.MaxG);
		DrawH.DrawAngle(res.PtnF);
		DrawV.DrawAngle(res.PtnF);
		if( res.PtnF ){
			if( exeenv.Wave == 3 ){
				DrawH.DrawPtn(res.PtnH[0], res.PtnF);
				DrawV.DrawPtn(res.PtnV[0], res.PtnF);
				DrawH.DrawPtn(res.PtnH[1], res.PtnF);
				DrawV.DrawPtn(res.PtnV[1], res.PtnF);
			}
			else {
				DrawH.DrawPtn(res.PtnH[exeenv.Wave], res.PtnF);
				DrawV.DrawPtn(res.PtnV[exeenv.Wave], res.PtnF);
			}
			int X, Y;
			DrawV.GetBottomPosition(X, Y);
			res.DrawRes(Printer()->Canvas, X, Y+4);
		}
		Printer()->EndDoc();
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainWnd::K27Click(TObject *Sender)
{
	TValRepDlg *Box = new TValRepDlg(this);

	if( Box->Execute(&ant, Grid2->Row, Grid2->Col) == TRUE ){
		UpdateAntData();
	}
	delete Box;
}
//---------------------------------------------------------------------------
// 平行移動
void __fastcall TMainWnd::K28Click(TObject *Sender)
{
	TMoveDlgBox *Box = new TMoveDlgBox(this);

	if( Box->Execute(&ant, Grid2->Row, Grid2->Col) == TRUE ){
		UpdateAntData();
	}
	delete Box;
}
//---------------------------------------------------------------------------
// テキストで編集
void __fastcall TMainWnd::K30Click(TObject *Sender)
{
	TTextEditDlg *Box = new TTextEditDlg(this);

	AnsiString	Edit;
	SaveAntStrings(Edit);
	if( Box->Execute(Edit, TRUE) == TRUE ){
		Edit = Box->Memo->Text;
		InitAntDef();
		LoadAntStrings(Edit);
		SetAntDef();
		ant.Edit = ant.Flag = 1;
		UpdateAllViews();
	}
	delete Box;
}
//---------------------------------------------------------------------------
// コメントの編集
void __fastcall TMainWnd::K34Click(TObject *Sender)
{
	TTextEditDlg *Box = new TTextEditDlg(this);

	if( Box->Execute(antRem, TRUE, "文書情報(コメント)") == TRUE ){
		antRem = Box->Memo->Text;
		ant.Edit = 1;
	}
	delete Box;
}
//---------------------------------------------------------------------------
// 波長表記のチェックの変更
void __fastcall TMainWnd::ChkRmdClick(TObject *Sender)
{
	exeenv.RmdSel = ChkRmd->Checked;
	Grid2->Invalidate();
}
//---------------------------------------------------------------------------
// 偏波選択
void __fastcall TMainWnd::WaveSelClick(TObject *Sender)
{
	if( exeenv.Wave != WaveSel->ItemIndex ){
		exeenv.Wave = WaveSel->ItemIndex;
		if( res.PtnF ){
			DrawPtn();
		}
	}
}
//---------------------------------------------------------------------------
// 電流ファイル作成
void __fastcall TMainWnd::KC2Click(TObject *Sender)
{
	char	bf[256];

	SaveDialog->Title = "電流ファイルを作成";
	SaveDialog->Filter = "CSVファイル(*.csv)|*.csv|";
	if( *antFname.c_str() ){
		strcpy(bf, antFname.c_str());
		SetEXT(bf, ".csv");
		SaveDialog->FileName = GetFileName(bf);
	}
	else {
		SaveDialog->FileName = "無題";
	}
	SaveDialog->DefaultExt = "csv";
	SaveDialog->InitialDir = ResDir;
	if( SaveDialog->Execute() == TRUE ){
		SetWaitCursor();
		FILE *fp = fopen(AnsiString(SaveDialog->FileName).c_str(), "wt");
		if( fp != NULL ){
			exeenv.pCurFile = (CURFILE *)new BYTE[sizeof(CURFILE)*env.pmax];
			memset(exeenv.pCurFile, 0, sizeof(CURFILE)*env.pmax);
			exeenv.CalcLog = 0;		// ログ表示ＯＦＦ
			SaveCurrent();
			fprintf(fp, "WIRE,PULSE,X(m),Y(m),Z(m),REAL(A),IMAGINARY(A),MAGNITUDE(A),PHASE(DEG)\n");
			for( int i = 0; i < env.pmax; i++ ){
				if( exeenv.pCurFile[i].Flag ){
					fprintf(fp, "%d,%d,%lf,%lf,%lf,%le,%le,%le,%lf\n",
						exeenv.pCurFile[i].W,
						i+1,
						exeenv.pCurFile[i].X,exeenv.pCurFile[i].Y,exeenv.pCurFile[i].Z,
						exeenv.pCurFile[i].I,
						exeenv.pCurFile[i].IJ,
						exeenv.pCurFile[i].MAG,
						exeenv.pCurFile[i].DEG
					);
				}
			}
			fclose(fp);
			delete exeenv.pCurFile;
			exeenv.pCurFile = NULL;
		}
		else {
			ErrorMB("'%s'が作成できません.", SaveDialog->FileName.c_str());
		}
	}
}
//---------------------------------------------------------------------------
// 近距離電界強度ファイル作成
void __fastcall TMainWnd::K31Click(TObject *Sender)
{
	char bf[256];
	TNearSetDlg *Box = new TNearSetDlg(this);

	exeenv.pNearFile = new NEARFILE;
	memset(exeenv.pNearFile, 0, sizeof(NEARFILE));
	exeenv.pNearFile->POW = 50;
	exeenv.pNearFile->XW = 3;
	exeenv.pNearFile->YW = 3;
	exeenv.pNearFile->ZW = 3;
	exeenv.pNearFile->XN = 5;
	exeenv.pNearFile->YN = 5;
	exeenv.pNearFile->ZN = 5;
	if( *antFname.c_str() ){
		strcpy(bf, antFname.c_str());
		SetEXT(bf, ".csv");
		sprintf(exeenv.pNearFile->Name, "%s%s", ResDir, GetFileName(bf));
	}
	else {
		sprintf(exeenv.pNearFile->Name, "%s無題", ResDir);
	}
	int r = Box->Execute();
	delete Box;
	if( r == FALSE ){
		delete exeenv.pNearFile;
		exeenv.pNearFile = NULL;
		return;
	}
	SetWaitCursor();
	exeenv.pNearFile->fp = fopen(exeenv.pNearFile->Name, "wt");
	if( exeenv.pNearFile->fp == NULL ){
		strcpy(bf, exeenv.pNearFile->Name);
		delete exeenv.pNearFile;
		exeenv.pNearFile = NULL;
		ErrorMB("'%s'が作成できません.", bf);
		return;
	}
	exeenv.CalcLog = 0;		// ログ表示ＯＦＦ
	if( exeenv.pNearFile->TYPE ){
		fprintf(exeenv.pNearFile->fp, "X(m),Y(m),Z(m),VECTOR,REAL(AT/m),IMAGINARY(AT/m),MAGNITUDE(AT/m),PHASE(DEG)\n");
	}
	else {
		fprintf(exeenv.pNearFile->fp, "X(m),Y(m),Z(m),VECTOR,REAL(V/m),IMAGINARY(V/m),MAGNITUDE(V/m),PHASE(DEG)\n");
	}
	SaveNear();
	fclose(exeenv.pNearFile->fp);
	delete exeenv.pNearFile;
	exeenv.pNearFile = NULL;
}
//---------------------------------------------------------------------------
// 遠距離電界強度ファイル作成
void __fastcall TMainWnd::K32Click(TObject *Sender)
{
	char bf[256];
	TFarSetDlg *Box = new TFarSetDlg(this);

	exeenv.pFarFile = new FARFILE;
	memset(exeenv.pFarFile, 0, sizeof(FARFILE));
	exeenv.pFarFile->A = 0;
	exeenv.pFarFile->AW = 10;
	exeenv.pFarFile->AN = 36;
	exeenv.pFarFile->Z = 0;
	exeenv.pFarFile->ZW = 10;
	exeenv.pFarFile->ZN = 10;
	if( *antFname.c_str() ){
		strcpy(bf, antFname.c_str());
		SetEXT(bf, ".csv");
		sprintf(exeenv.pFarFile->Name, "%s%s", ResDir, GetFileName(bf));
	}
	else {
		sprintf(exeenv.pFarFile->Name, "%s無題", ResDir);
	}
	int r = Box->Execute();
	delete Box;
	if( r == FALSE ){
		delete exeenv.pFarFile;
		exeenv.pFarFile = NULL;
		return;
	}
	SetWaitCursor();
	exeenv.pFarFile->fp = fopen(exeenv.pFarFile->Name, "wt");
	if( exeenv.pFarFile->fp == NULL ){
		strcpy(bf, exeenv.pFarFile->Name);
		delete exeenv.pFarFile;
		exeenv.pFarFile = NULL;
		ErrorMB("'%s'が作成できません.", bf);
		return;
	}
	exeenv.CalcLog = 0;		// ログ表示ＯＦＦ
	fprintf(exeenv.pFarFile->fp, "ZENITH(DEG),AZIMUTH(DEG),VERT(dBi),HORI(dBi),TOTAL(dBi)\n");
	SaveFar();
	fclose(exeenv.pFarFile->fp);
	delete exeenv.pFarFile;
	exeenv.pFarFile = NULL;
}
//---------------------------------------------------------------------------
// 周波数特性ファイル
void __fastcall TMainWnd::K49Click(TObject *Sender)
{
	TFreqSetDlg *Box = new TFreqSetDlg(this);
	Box->Execute(pCalAnt);
	delete Box;
}
//---------------------------------------------------------------------------
// 水平パターンの仰角
void __fastcall TMainWnd::EleBtnClick(TObject *Sender)
{
	UnicodeString	DegStr;	// ja7ude 1.0
	double		deg;

	DegStr = StrDbl(RoundUp(res.ElvHPtn, 100));
	if( InputQuery("MMANA", "水平パターンの計算仰角（0～90）を入力", DegStr) == TRUE ){
		if( Calc(deg, AnsiString(DegStr).c_str()) == TRUE ){
			if( (float(deg) != RoundUp(res.ElvHPtn, 100)) && (deg >= 0) && (deg <= 90.0) ){
				res.ElvHPtn = deg;
				SetWaitCursor();
				exeenv.CalcLog = 0;		// ログ表示ＯＦＦ
				CalcNewPtn();
				DrawPtnH.SetMaxDB(res.MaxG);
				DrawPtnV.SetMaxDB(res.MaxG);
				UpdateAllViews();
			}
		}
	}
}
//---------------------------------------------------------------------------
// 全ワイヤの回転
void __fastcall TMainWnd::K33Click(TObject *Sender)
{
	TRotWireDlg *Box = new TRotWireDlg(this);

	if( Box->Execute(&ant) == TRUE ){
		UpdateAntData();
	}
	delete Box;
}
//---------------------------------------------------------------------------
// ワイヤーＣＡＤの起動
void __fastcall TMainWnd::KCADClick(TObject *Sender)
{
	TWireCadDlg *Box = new TWireCadDlg(this);

	int Sel = Grid2->Row - 1;
	if( Box->Execute(&ant, Sel) == TRUE ){
		UpdateAntData();
	}
	delete Box;
	if( Sel < (Grid2->RowCount - 1) ){
		Grid2->Row = Sel + 1;
	}
}
//---------------------------------------------------------------------------
// アンテナ定義の変更
void __fastcall TMainWnd::UpdateAntData(void)
{
	Grid2->EditorMode = FALSE;
	Grid3->EditorMode = FALSE;
	Grid4->EditorMode = FALSE;
	ant.Edit = 1;
	exeenv.CalcLog = 1;
	SetStackAnt();
	res.ClearBWC();
	SetAntDef();
	ant.Flag = 0;
	UpdateAllViews();
}
//---------------------------------------------------------------------------
// 始点と終点の入れ替え
void __fastcall TMainWnd::K35Click(TObject *Sender)
{
	if( ant.wmax && Grid2->Row ){
		int n = Grid2->Row - 1;
		double X = ant.wdef[n].X1;
		double Y = ant.wdef[n].Y1;
		double Z = ant.wdef[n].Z1;
		ant.wdef[n].X1 = ant.wdef[n].X2;
		ant.wdef[n].Y1 = ant.wdef[n].Y2;
		ant.wdef[n].Z1 = ant.wdef[n].Z2;
		ant.wdef[n].X2 = X;
		ant.wdef[n].Y2 = Y;
		ant.wdef[n].Z2 = Z;
		UpdateAntData();
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainWnd::FormPaint(TObject *Sender)
{
	if( FirstInit == FALSE ) return;

	FirstInit = FALSE;
	char	bf[1024];

	for (int i=1;i<=ParamCount();i++){
		strncpy(bf, AnsiString(ParamStr(i)).c_str(), sizeof(bf)-1);
		if( (bf[0] == '-')&&(bf[0] == '/') ){     // オプション
		}
		else {
			LoadDispatch(bf);
		}
	}
}
//---------------------------------------------------------------------------
// アンテナ計算ファイルの保存
/*
	char	bf[256];

	SaveDialog->Title = "アンテナ計算ファイルを保存";
	SaveDialog->Filter = "テキストファイル(*.txt)|*.txt|";
	if( *antFname.c_str() ){
		strcpy(bf, antFname.c_str());
		SetEXT(bf, ".txt");
		SaveDialog->FileName = GetFileName(bf);
	}
	else {
		SaveDialog->FileName = "無題";
	}
	SaveDialog->DefaultExt = "txt";
	SaveDialog->InitialDir = AntDir;
	if( SaveDialog->Execute() == TRUE ){
		SaveAntFile(SaveDialog->FileName.c_str(), 1);
	}
*/
//---------------------------------------------------------------------------
// 最適化前の状態に戻す
void __fastcall TMainWnd::K36Click(TObject *Sender)
{
	if( !antDef.IsEmpty() ){
		if( YesNoMB("最適化前の状態に戻しますか？")== IDYES ){
			LoadAntStrings(antDef);
			antDef = "";
			SetAntDef();
			ant.Edit = ant.Flag = 1;
			UpdateAllViews();
		}
	}
}
//---------------------------------------------------------------------------
// スタックの展開
void __fastcall TMainWnd::K37Click(TObject *Sender)
{
	TStackDlgBox *Box = new TStackDlgBox(this);

	int r = Box->Execute(&ant);
	if( r == 1024 ){
		UpdateAntData();
		if( &ant != pCalAnt ) memcpy(&ant, pCalAnt, sizeof(ANTDEF));
		ant.StackH = ant.StackV = 1;
	}
	if( r != FALSE ){
		UpdateAntData();
		AllViewBtnClick(NULL);
	}
	delete Box;
}
//---------------------------------------------------------------------------
// 現在の状態を記憶
void __fastcall TMainWnd::K38Click(TObject *Sender)
{
	if( ant.wmax ){
		SaveAntStrings(antSave);
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainWnd::K39Click(TObject *Sender)
{
	if( !antSave.IsEmpty() ){
		if( YesNoMB("以前記憶した状態に戻しますか？")== IDYES ){
			LoadAntStrings(antSave);
			antDef = "";
			SetAntDef();
			ant.Edit = ant.Flag = 1;
			UpdateAllViews();
		}
	}
}
//---------------------------------------------------------------------------
int __fastcall TMainWnd::SetStackAnt(void)
{
	if( (ant.StackH == 1)&&(ant.StackV == 1) ){
		if( pCalAnt != &ant ) delete pCalAnt;
		pCalAnt = &ant;
	}
	else {
		if( pCalAnt == &ant ) pCalAnt = new ANTDEF;
		Stack();
	}
	return InitAntData(pCalAnt);
}
//---------------------------------------------------------------------------
void __fastcall TMainWnd::Stack(void)
{
	memcpy(pCalAnt, &ant, sizeof(ANTDEF));
	ANTDEF	*bp = new ANTDEF;
	int i, n;
	WDEF	*wsp, *wtp;

	int		wmax = pCalAnt->wmax;
	memcpy(bp, pCalAnt, sizeof(ANTDEF));
	if( ant.StackH > 1 ){		// 水平方向のスタック
		double	yc = -((ant.StackH-1) * ant.StackHW)/2;
		wtp = pCalAnt->wdef;
		for( i = 0; i < ant.StackH; i++, yc += ant.StackHW ){
			for( wsp = bp->wdef, n = 0; n < bp->wmax; n++, wsp++, wtp++ ){
				memcpy(wtp, wsp, sizeof(WDEF));
				wtp->Y1 += yc;
				wtp->Y2 += yc;
			}
		}
		pCalAnt->wmax *= ant.StackH;
	}
	memcpy(bp, pCalAnt, sizeof(ANTDEF));
	if( ant.StackV > 1 ){		// 垂直方向のスタック
		double	zc;
		switch(ant.StackVT){
			case 1:		// 上向き
				zc = 0;
				break;
			case 2:		// 下向き
				zc = -((ant.StackV-1) * ant.StackVW);
				break;
			default:	// 上下
				zc = -((ant.StackV-1) * ant.StackVW)/2;
				break;
		}
		wtp = pCalAnt->wdef;
		for( i = 0; i < ant.StackV; i++, zc += ant.StackVW ){
			for( wsp = bp->wdef, n = 0; n < bp->wmax; n++, wsp++, wtp++ ){
				memcpy(wtp, wsp, sizeof(WDEF));
				wtp->Z1 += zc;
				wtp->Z2 += zc;
			}
		}
		pCalAnt->wmax *= ant.StackV;
	}
	// 給電点の調整
	CDEF	*csp;
	CDEF	*ctp = pCalAnt->cdef;
	int coff = 0;
	for( i = 0; i < (ant.StackV * ant.StackH); i++, coff += wmax ){
		for( csp = bp->cdef, n = 0; n < bp->cmax; n++, csp++, ctp++ ){
			memcpy(ctp, csp, sizeof(CDEF));
			AddWireNo(ctp->PLUS, coff);
			ctp->EV /= (ant.StackV * ant.StackH);
		}
	}
	pCalAnt->cmax *= (ant.StackV * ant.StackH);
	// ロードの調整
	LDEF	*lsp;
	LDEF	*ltp = pCalAnt->ldef;
	int loff = 0;
	for( i = 0; i < (ant.StackV * ant.StackH); i++, loff += wmax ){
		for( lsp = bp->ldef, n = 0; n < bp->lmax; n++, lsp++, ltp++ ){
			memcpy(ltp, lsp, sizeof(LDEF));
			AddWireNo(ltp->PLUS, loff);
		}
	}
	pCalAnt->lmax *= (ant.StackV * ant.StackH);
	delete bp;
}
//---------------------------------------------------------------------
void __fastcall TMainWnd::AddWireNo(LPSTR t, int off)
{
	if( !off ) return;

	int		d;
	LPSTR	p;

	if( (*t == 'w')||(*t == 'W') ){
		char	bf[32];

		for( p = t+1; *p; p++ ){
			if( !isdigit(*p) ) break;
		}
		strcpy(bf, p);
		*p = 0;
		sscanf(t+1, "%d", &d);
		sprintf(t+1, "%d%s", d + off, bf);
	}
	else {
		sscanf(t, "%d", &d);
		sprintf(t, "%d", d + off);
	}
}

//---------------------------------------------------------------------------


void __fastcall TMainWnd::Ant3DClick(TObject *Sender)
{
	exeenv.Ant3D = Ant3D->Checked;
	AllViewBtnClick(NULL);
}
//---------------------------------------------------------------------------
// 給電点の削除と追加のチェック
int __fastcall TMainWnd::CheckCdef(LPSTR t, int w)
{
	int i, n;
	for( i = 0; i < ant.cmax; i++ ){
		if( toupper(ant.cdef[i].PLUS[0]) == 'W' ){
			if( sscanf(&ant.cdef[i].PLUS[1], "%d", &n) == 1 ){
				n--;
				if( n == w ){
					if( t != NULL ){
						char c = 'C';
						LPCSTR p;
						for( p = &ant.cdef[i].PLUS[1]; *p; p++ ){
							c = char(toupper(*p));
							if( (c == 'B')||(c == 'C')||(c == 'E') ){
								break;
							}
						}
						*t = c;
					}
					return i;
				}
			}
		}
	}
	return -1;
}
//---------------------------------------------------------------------------
// 給電点の削除
void __fastcall TMainWnd::DelCdef(int w)
{
	if( w && (w <= ant.cmax) ){
		w--;
		int i;
		for( i = w; i < (ant.cmax - 1); i++ ){
			memcpy(&ant.cdef[i], &ant.cdef[i+1], sizeof(CDEF));
		}
		memset(&ant.cdef[i], 0, sizeof(CDEF));
		ant.cmax--;
		UpdateCount();
		SetAutoVol();
		Grid3->RowCount = ant.cmax + 2;
		UpdateAntData();
		Grid3->Invalidate();
	}
}
//---------------------------------------------------------------------------
// ポップアップ給電点の追加
void __fastcall TMainWnd::AddCdefClick(char c)
{
	int w = Grid2->Row - 1;
	int n = CheckCdef(NULL, w);
	if( n != -1 ){
		DelCdef(n+1);
	}
	if( ant.cmax < CMAX ){
		sprintf(ant.cdef[ant.cmax].PLUS, "w%d%c", w+1, c);
		ant.cdef[ant.cmax].DEG = 0;
		ant.cdef[ant.cmax].EV = 1.0;
		ant.cmax++;
		SetAutoVol();
		UpdateAntData();
	}
}
//---------------------------------------------------------------------------
// ポップアップ給電点の削除
void __fastcall TMainWnd::K41Click(TObject *Sender)
{
	int w = Grid2->Row - 1;
	int n = CheckCdef(NULL, w);
	if( n != -1 ){
		DelCdef(n+1);
	}
}
//---------------------------------------------------------------------------
// ポップアップ給電点の追加（中心）
void __fastcall TMainWnd::K42Click(TObject *Sender)
{
	AddCdefClick('c');
}
//---------------------------------------------------------------------------
void __fastcall TMainWnd::K43Click(TObject *Sender)
{
	AddCdefClick('b');
}
//---------------------------------------------------------------------------
void __fastcall TMainWnd::K44Click(TObject *Sender)
{
	AddCdefClick('e');
}
//---------------------------------------------------------------------------
void __fastcall TMainWnd::PopupAntPopup(TObject *Sender)
{
	int f1 = FALSE;		// 削除
	int f2 = FALSE;
	if( Grid2->Row ){
		int w = Grid2->Row - 1;
		char c;
		int n = CheckCdef(&c, w);
		K42->Checked = FALSE;
		K43->Checked = FALSE;
		K44->Checked = FALSE;
		if( (ant.cmax < CMAX) && ant.wdef[w].R ) f2 = TRUE;
		if( n != -1 ){
			f1 = TRUE;
			switch(c){
				case 'C':
					K42->Checked = TRUE;
					break;
				case 'B':
					K43->Checked = TRUE;
					break;
				case 'E':
					K44->Checked = TRUE;
					break;
			}
		}
	}
	K41->Enabled = f1;
	K40->Enabled = f2;
}
//---------------------------------------------------------------------------
void __fastcall TMainWnd::K45Click(TObject *Sender)
{
	TWCombDlg *Box = new TWCombDlg(this);

	if( Box->Execute(&ant) == TRUE ){
		AdjPdef(&ant);
		UpdateAntData();
	}
	delete Box;
}
//---------------------------------------------------------------------------
void __fastcall TMainWnd::K47Click(TObject *Sender)
{
	TWCombDspDlg *Box = new TWCombDspDlg(this);

	WDEF *wp = &ant.wdef[Grid2->Row-1];
	PDEF *pp = FindPP(&ant, wp->R);
	Box->Execute(&ant, pp, wp, Grid2->Row);
	delete Box;
}
//---------------------------------------------------------------------------
// Recent Menu Click
void __fastcall TMainWnd::OnRecentClick(TObject *Sender)
{
	LPCSTR pName = RecentMenu.FindItemText((TMenuItem *)Sender);
	if( pName != NULL ){
		char bf[256];
		strcpy(bf, pName);
		if( CheckSaveAntFile() == FALSE ) return;
		LoadDispatch(bf);
	}
}
//---------------------------------------------------------------------------
// Recent Menu Click
void __fastcall TMainWnd::RecentAdd(LPCSTR pNew, int f)
{
	if( RecentMenu.IsAdd(pNew) ){
		RecentMenu.Delete();
		if( f != FALSE ){
			RecentMenu.Add(pNew);
		}
		else {
			RecentMenu.Delete(pNew);
		}
		RecentMenu.Insert(NULL, OnRecentClick);
	}
}
//---------------------------------------------------------------------------
// Recent Menu Update
void __fastcall TMainWnd::UpdateRecentMenu(void)
{
	RecentMenu.Delete();
	RecentMenu.Init(K6, K1, exeenv.RecentMax);
	if( exeenv.RecentMAA ) RecentMenu.ExtFilter("MAA");
	RecentMenu.Insert(NULL, OnRecentClick);
}
//---------------------------------------------------------------------------
// 拡張子によるファイルの読み込み
void __fastcall TMainWnd::LoadDispatch(LPCSTR pName)
{
	int f;

	if( !strcmpi(GetEXT(pName), "MAA") ){
		antFname = pName;
		f = LoadAntFile(pName);
		RecentAdd(pName, f);
	}
	else if( !strcmpi(GetEXT(pName), "MAB") ){
		f = LoadResFile(pName);
		if( !exeenv.RecentMAA ) RecentAdd(pName, f);
	}
	else if( !strcmpi(GetEXT(pName), "MAO") ){
		f = LoadACalFile(pName);
		if( !exeenv.RecentMAA ) RecentAdd(pName, f);
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainWnd::KMMANAWebW1Click(TObject *Sender)
{
	WebRef.ShowHTML("http://plaza27.mbn.or.jp/~je3hht/mmana/index.html");	
}
//---------------------------------------------------------------------------
