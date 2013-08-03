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
#ifndef MainH
#define MainH
//---------------------------------------------------------------------------
/*
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\ComCtrls.hpp>
#include <vcl\ExtCtrls.hpp>
#include <vcl\Menus.hpp>
#include <vcl\Grids.hpp>
*/
#include <Printers.hpp>     //ja7ude 1.0

#include "ComLib.h"
#include <Dialogs.hpp>
#include <System.Classes.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.StdCtrls.hpp>
//---------------------------------------------------------------------------
class TMainWnd : public TForm
{
__published:	// IDE 管理のコンポーネント
	TOpenDialog *OpenDialog;
	TSaveDialog *SaveDialog;
	TPageControl *Page;
	TTabSheet *TabSheet1;
	TTabSheet *TabSheet2;
	TPaintBox *PBoxAnt;
	TTabSheet *TabSheet3;
	TTabSheet *TabSheet5;
	TPaintBox *PBoxPtn;
	TMainMenu *MainMenu;
	TMenuItem *K1;
	TMenuItem *K2;
	TMenuItem *K3;
	TMenuItem *K4;
	TMenuItem *K5;
	TMenuItem *K6;
	TMemo *CalMemo;
	TStringGrid *Grid1;
	TButton *CalTrgBtn;
	TLabel *Label1;
	TStringGrid *Grid3;
	TLabel *WireCnt;
	TLabel *CurCnt;
	TStringGrid *Grid4;
	TLabel *LoadCnt;
	TStringGrid *Grid2;
	TEdit *AntName;
	TLabel *Label5;
	TCheckBox *AutoVol;
	TComboBox *Freq;
	TLabel *Label6;
	TPopupMenu *EditPop;
	TMenuItem *K7;
	TMenuItem *K9;
	TCheckBox *EnbLoad;
	TMenuItem *KH1;
	TMenuItem *K10;
	TTrackBar *TBarDeg;
	TTrackBar *TBarSC;
	TLabel *AntName2;
	TLabel *Label7;
	TLabel *Label8;
	TCheckBox *DspCur;
	TComboBox *DoFreq;
	TLabel *Label9;
	TLabel *Label10;
	TRadioGroup *CalcSel;
	TLabel *AntName3;
	TButton *GndSetBtn;
	TLabel *Label11;
	TLabel *Label12;
	TCheckBox *DspPlus;
	TTrackBar *TBarCur;
	TLabel *Label13;
	TLabel *AntName4;
	TMenuItem *K11;
	TMenuItem *K12;
	TMenuItem *N1;
	TMenuItem *N2;
	TMenuItem *KE1;
	TMenuItem *K13;
	TMenuItem *K14;
	TMenuItem *N4;
	TMenuItem *KV1;
	TMenuItem *KC1;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label4;
	TLabel *Label14;
	TEdit *EditSC;
	TButton *OrgBtn;
	TMenuItem *N5;
	TMenuItem *K16;
	TButton *AllViewBtn;
	TComboBox *AntHeight;
	TMenuItem *K18;
	TButton *ACalBtn;
	TButton *EleEditBtn;
	TButton *ACalResBtn;
	TMenuItem *K8;
	TMenuItem *N6;
	TMenuItem *K15;
	TMenuItem *K19;
	TMenuItem *K20;
	TMenuItem *N7;
	TPopupMenu *PopupLoad;
	TMenuItem *KS1;
	TMenuItem *KS2;
	TMenuItem *KS3;
	TMenuItem *K17;
	TMenuItem *N3;
	TMenuItem *N8;
	TMenuItem *K21;
	TMenuItem *N9;
	TMenuItem *O1;
	TMenuItem *KXY;
	TMenuItem *KXY1;
	TMenuItem *KXZ1;
	TMenuItem *KYZ1;
	TMenuItem *N10;
	TMenuItem *KS4;
	TMenuItem *K22;
	TMenuItem *K23;
	TMenuItem *K24;
	TMenuItem *K25;
	TMenuItem *K26;
	TMenuItem *KT1;
	TMenuItem *KT2;
	TButton *BwBtn;
	TButton *PrintBtn;
	TPrintDialog *PrintDialog;
	TCheckBox *ChkWith;
	TMenuItem *K27;
	TMenuItem *KR1;
	TMenuItem *K28;
	TMenuItem *N12;
	TMenuItem *KM1;
	TPopupMenu *PopupAnt;
	TMenuItem *K29;
	TMenuItem *N11;
	TMenuItem *KM2;
	TMenuItem *KT3;
	TMenuItem *N13;
	TMenuItem *K30;
	TCheckBox *ChkRmd;
	TRadioGroup *WaveSel;
	TMenuItem *KC2;
	TMenuItem *N14;
	TMenuItem *K31;
	TMenuItem *K32;
	TButton *EleBtn;
	TMenuItem *K33;
	TMenuItem *N15;
	TMenuItem *K34;
	TLabel *Label15;
	TComboBox *WireSel;
	TMenuItem *KCAD;
	TButton *WireCadBtn;
	TMenuItem *K35;
	TMenuItem *KS5;
	TComboBox *EditDM1;
	TComboBox *EditDM2;
	TButton *DocBtn;
	TLabel *Label16;
	TComboBox *EditEC;
	TMenuItem *K36;
	TMenuItem *N16;
	TMenuItem *K37;
	TMenuItem *K38;
	TMenuItem *K39;
	TButton *StackBtn;
	TTrackBar *TBarZDeg;
	TLabel *Label17;
	TCheckBox *Ant3D;
	TMenuItem *N17;
	TMenuItem *K40;
	TMenuItem *K41;
	TMenuItem *K42;
	TMenuItem *K43;
	TMenuItem *K44;
	TMenuItem *AppendTxt1;
	TMenuItem *K45;
	TMenuItem *N18;
	TMenuItem *K46;
	TMenuItem *K47;
	TMenuItem *N19;
	TMenuItem *KC3;
	TMenuItem *K48;
	TMenuItem *N20;
	TMenuItem *KMMANAWebW1;
	TMenuItem *K49;
	void __fastcall Grid2DrawCell(TObject *Sender, int  Col, int Row,
	TRect &Rect, TGridDrawState State);
	void __fastcall Grid3DrawCell(TObject *Sender, int Col, int Row, TRect &Rect,
	TGridDrawState State);
	void __fastcall Grid4DrawCell(TObject *Sender, int Col, int Row, TRect &Rect,
	TGridDrawState State);
	void __fastcall Grid4SelectCell(TObject *Sender, int Col, int Row,
	bool &CanSelect);
	void __fastcall CalTrgBtnClick(TObject *Sender);

	void __fastcall Grid2GetEditText(TObject *Sender, int ACol, int ARow,
	UnicodeString &Value);
	void __fastcall Grid2SetEditText(TObject *Sender, int ACol, int ARow,
	const UnicodeString Value);
	void __fastcall Grid3GetEditText(TObject *Sender, int ACol, int ARow,
	UnicodeString &Value);
	void __fastcall Grid3SetEditText(TObject *Sender, int ACol, int ARow,
	const UnicodeString Value);
	void __fastcall Grid4GetEditText(TObject *Sender, int ACol, int ARow,
	UnicodeString &Value);
	void __fastcall Grid4SetEditText(TObject *Sender, int ACol, int ARow,
	const UnicodeString Value);
	void __fastcall K7Click(TObject *Sender);
	void __fastcall K9Click(TObject *Sender);
	void __fastcall K4Click(TObject *Sender);
	
	void __fastcall K3Click(TObject *Sender);

	void __fastcall K5Click(TObject *Sender);
	void __fastcall K2Click(TObject *Sender);
	
	void __fastcall K10Click(TObject *Sender);

	void __fastcall PBoxAntPaint(TObject *Sender);

	
	void __fastcall TBarDegChange(TObject *Sender);

	
	
	
	void __fastcall PBoxPtnPaint(TObject *Sender);
	void __fastcall Grid1DrawCell(TObject *Sender, int Col, int Row, TRect &Rect,
	TGridDrawState State);
	

	void __fastcall PageChange(TObject *Sender);
	void __fastcall DspPlusClick(TObject *Sender);
	
	void __fastcall GndSetBtnClick(TObject *Sender);
	void __fastcall K6Click(TObject *Sender);
	void __fastcall FreqChange(TObject *Sender);
	void __fastcall K12Click(TObject *Sender);
	void __fastcall K11Click(TObject *Sender);

	void __fastcall KC1Click(TObject *Sender);
	void __fastcall EditDM1Change(TObject *Sender);


	void __fastcall PBoxAntMouseDown(TObject *Sender, TMouseButton Button,
	TShiftState Shift, int X, int Y);
	void __fastcall OrgBtnClick(TObject *Sender);
	void __fastcall K16Click(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall FormResize(TObject *Sender);
	void __fastcall AllViewBtnClick(TObject *Sender);
	void __fastcall KS1Click(TObject *Sender);
	void __fastcall KS2Click(TObject *Sender);
	void __fastcall KS3Click(TObject *Sender);
	
	
	void __fastcall DoFreqChange(TObject *Sender);
	void __fastcall AntNameChange(TObject *Sender);
	void __fastcall EditDM2Change(TObject *Sender);
	void __fastcall EditSCChange(TObject *Sender);
	void __fastcall EnbLoadClick(TObject *Sender);
	
	void __fastcall K18Click(TObject *Sender);
	void __fastcall ACalBtnClick(TObject *Sender);
	
	void __fastcall ACalResBtnClick(TObject *Sender);
	
	
	void __fastcall K19Click(TObject *Sender);
	void __fastcall K20Click(TObject *Sender);
	void __fastcall K17Click(TObject *Sender);
	void __fastcall K21Click(TObject *Sender);
	
	void __fastcall KXYClick(TObject *Sender);
	void __fastcall KXZ1Click(TObject *Sender);
	void __fastcall KYZ1Click(TObject *Sender);
	
	void __fastcall KS4Click(TObject *Sender);
	
	void __fastcall K24Click(TObject *Sender);
	void __fastcall K25Click(TObject *Sender);
	void __fastcall K26Click(TObject *Sender);
	void __fastcall K23Click(TObject *Sender);
	void __fastcall KT1Click(TObject *Sender);
	void __fastcall BwBtnClick(TObject *Sender);
	void __fastcall PrintBtnClick(TObject *Sender);
	void __fastcall K27Click(TObject *Sender);
	void __fastcall PBoxAntDblClick(TObject *Sender);
	
	void __fastcall PBoxAntClick(TObject *Sender);
	void __fastcall K28Click(TObject *Sender);
	void __fastcall K29Click(TObject *Sender);
	void __fastcall K30Click(TObject *Sender);
	void __fastcall ChkRmdClick(TObject *Sender);
	void __fastcall WaveSelClick(TObject *Sender);
	void __fastcall KC2Click(TObject *Sender);
	void __fastcall K31Click(TObject *Sender);
	void __fastcall K32Click(TObject *Sender);
	void __fastcall EleBtnClick(TObject *Sender);
	void __fastcall K33Click(TObject *Sender);
	void __fastcall K34Click(TObject *Sender);



	void __fastcall KCADClick(TObject *Sender);
	void __fastcall K35Click(TObject *Sender);

	void __fastcall FormPaint(TObject *Sender);
	
	void __fastcall EditECChange(TObject *Sender);
	
	void __fastcall K36Click(TObject *Sender);
	void __fastcall K37Click(TObject *Sender);
	void __fastcall K38Click(TObject *Sender);
	void __fastcall K39Click(TObject *Sender);

	void __fastcall Ant3DClick(TObject *Sender);
	void __fastcall K41Click(TObject *Sender);
	void __fastcall K42Click(TObject *Sender);
	void __fastcall K43Click(TObject *Sender);
	void __fastcall K44Click(TObject *Sender);
	void __fastcall PopupAntPopup(TObject *Sender);
	void __fastcall AppendTxt1Click(TObject *Sender);
	void __fastcall K45Click(TObject *Sender);
	void __fastcall K47Click(TObject *Sender);
	
	
	void __fastcall KMMANAWebW1Click(TObject *Sender);
	void __fastcall K49Click(TObject *Sender);
private:	// ユーザー宣言
	void __fastcall OnIdle(TObject *Sender, bool &Done);

    int			FirstInit;
	CDrawPtnH	DrawPtnH;
	CDrawPtnV	DrawPtnV;
	TColor		ResColors[RESMAX];			// 結果比較の色情報
    CClip		Clip;

	CACal		*pACal;						// 最適化計算のポインタ

	void __fastcall RegCopy(void);
	void __fastcall ReadRegister(void);
	void __fastcall WriteRegister(void);

	void __fastcall UpdateAllViews(void);
	void __fastcall UpdateCount(void);

	void __fastcall InitAntDef(void);
	void __fastcall SetAntDef(void);
	void __fastcall GetAntDef(void);
	void __fastcall SetAutoVol(void);

	void __fastcall Grid2GetText(LPSTR t, long Col, long Row);
	int __fastcall Grid2NewLine(int Row);
	void __fastcall Grid3GetText(LPSTR t, long Col, long Row);
	void __fastcall Grid3NewLine(int Row);
	void __fastcall Grid4GetText(LPSTR t, long Col, long Row);
	void __fastcall Grid4NewLine(int Row);

	void __fastcall SaveAntStrings(AnsiString &out);
	void __fastcall LoadAntStrings(AnsiString &in);
	int __fastcall SaveAntFile(LPCSTR pName);
	int __fastcall LoadAntFile(LPCSTR pName);

	int __fastcall Plus2Seg(int Plus);

	int __fastcall SaveResFile(LPCSTR pName);
	int __fastcall LoadResFile(LPCSTR pName);
	int __fastcall SaveACalFile(LPCSTR pName);
	int __fastcall LoadACalFile(LPCSTR pName);

	int __fastcall CheckSaveAntFile(void);

	CAlignList	AlignList;
	CAlignGrid	AlignGrid[4];

	void __fastcall EntryAlignControl(void);

	void __fastcall DisCalcUI(void);
	DWORD	BgnTime;
    DWORD	EndTime;
	void __fastcall AllRoundUp(double sc);
	int __fastcall SelectWire(int X, int Y);
	void __fastcall UpdateAntData(void);

    int		PBoxAntMX;
    int		PBoxAntMY;

	int __fastcall DrawPtn(void);

	void __fastcall Stack(void);
	void __fastcall AddWireNo(LPSTR t, int off);

	int __fastcall CheckCdef(LPSTR t, int w);
	void __fastcall DelCdef(int w);
	void __fastcall AddCdefClick(char c);

	void __fastcall ShowHelp(LPCSTR pName);

	void __fastcall OnRecentClick(TObject *Sender);
	void __fastcall RecentAdd(LPCSTR pNew, int f);
	void __fastcall UpdateRecentMenu(void);
	void __fastcall LoadDispatch(LPCSTR pName);

    CRecentMenu	RecentMenu;
    CWebRef		WebRef;

public:		// ユーザー宣言
	__fastcall TMainWnd(TComponent* Owner);
	__fastcall ~TMainWnd();

	void __fastcall SetWaitCursor(void);

	void __fastcall TMainWnd::DrawPtnACalInfo(void);
	int __fastcall ACalExecute(void);
	int __fastcall SetStackAnt(void);

	AnsiString	antFname;
    AnsiString	antRem;			// アンテナのコメント
	ANTDEF		ant;
	ANTDEF		*pCalAnt;		// 計算時のアンテナ定義
};
//---------------------------------------------------------------------------
extern TMainWnd *MainWnd;
//---------------------------------------------------------------------------
#endif
