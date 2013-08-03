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

//----------------------------------------------------------------------------
#ifndef BwDispH
#define BwDispH
//----------------------------------------------------------------------------
// ja7ude 1.0
#include <System.hpp>
#include <Windows.hpp>
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Graphics.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Controls.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
#include <Menus.hpp>
#include <Grids.hpp>

#include "ComLib.h"
#include <System.Classes.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.StdCtrls.hpp>
//----------------------------------------------------------------------------
typedef struct {
	double	SWR;
	double	FQ;
}BWSLST;
class TBwDispDlg : public TForm
{
__published:        
	TButton *OKBtn;
	TPageControl *Page;
	TTabSheet *TabZ;
	TTabSheet *TabSWR;
	TTabSheet *TabGA;
	TPaintBox *PBoxG;
	TPaintBox *PBoxS;
	TPaintBox *PBoxZ;
	TButton *TryBtn;
	TComboBox *BwSel;
	TLabel *Label1;
	TLabel *Label2;
	TButton *PrintBtn;
	TPopupMenu *PopupMenu;
	TMenuItem *KM;
	TMenuItem *KMOFF;
	TMenuItem *KMSWR;
	TMenuItem *KMZSWR;
	TTabSheet *TabSet;
	TRadioGroup *SelMatch;
	TGroupBox *GroupBox1;
	TComboBox *EditCenter;
	TLabel *Label3;
	TUpDown *UDFreq;
	TButton *GetFoBtn;
	TTabSheet *TabPtn;
	TPaintBox *PBPtn;
	TStringGrid *Grid;
	TRadioGroup *RGPtn;
	TColorDialog *ColorDialog;
	TButton *ColorBtn;
	TButton *MeasBtn;
	TButton *HalfBtn;
	TGroupBox *GroupBox2;
	TGroupBox *GroupBox3;
	TComboBox *MatchFreq;
	TLabel *Label4;
	TCheckBox *MatchCenter;
	TUpDown *UDMatch;
	TGroupBox *GroupBox4;
	TComboBox *LimitSWR;
	TGroupBox *GroupBox5;
	TComboBox *HalfAddCnt;
	void __fastcall TryBtnClick(TObject *Sender);
	void __fastcall PBoxZPaint(TObject *Sender);
	void __fastcall PBoxSPaint(TObject *Sender);
	void __fastcall PBoxGPaint(TObject *Sender);
	void __fastcall MeasBtnClick(TObject *Sender);
	void __fastcall BwSelChange(TObject *Sender);
	void __fastcall PrintBtnClick(TObject *Sender);
	void __fastcall KMOFFClick(TObject *Sender);
	void __fastcall KMSWRClick(TObject *Sender);
	void __fastcall KMZSWRClick(TObject *Sender);
	void __fastcall PopupMenuPopup(TObject *Sender);
	void __fastcall SelMatchClick(TObject *Sender);
	void __fastcall PageChange(TObject *Sender);
	void __fastcall UDFreqClick(TObject *Sender, TUDBtnType Button);
	void __fastcall UDFreqChanging(TObject *Sender, bool &AllowChange);
	void __fastcall GetFoBtnClick(TObject *Sender);
	void __fastcall GridDrawCell(TObject *Sender, int Col, int Row, TRect &Rect,
	TGridDrawState State);
	void __fastcall PBPtnPaint(TObject *Sender);
	void __fastcall RGPtnClick(TObject *Sender);
	void __fastcall GridClick(TObject *Sender);
	
	void __fastcall ColorBtnClick(TObject *Sender);
	void __fastcall HalfBtnClick(TObject *Sender);
	void __fastcall UDMatchClick(TObject *Sender, TUDBtnType Button);
	void __fastcall UDMatchChanging(TObject *Sender, bool &AllowChange);
	void __fastcall MatchCenterClick(TObject *Sender);
	void __fastcall LimitSWRChange(TObject *Sender);
	
	void __fastcall HalfAddCntChange(TObject *Sender);
private:
	TColor		*pColorTable;

	int		EnbSel;
	int		CalcF;
	BWC		bwct;		// 推測データ
	int		slstpos;
	BWSLST	slst[BWMAX];// ＳＷＲリスト
	CRes	SaveRes;	// 結果のセーブ領域
	double	SaveFQ;		// 周波数のセーブ
	int		FirstUD;
	int		FirstUDM;
	double	SWRLimit;

	double	AntL;
	double	AntC;
	double	AntQ;
	double	AntW;
	double	AntF;

	CGraph	GZ;
	CGraph	GS;
	CGraph	GG;

	ANTDEF	*ap;

	float		PtnH[180];
    float		PtnV[91];
	CDrawPtnH	DrawPtnH;
	CDrawPtnV	DrawPtnV;

	int			HCnt;
    BWS			MatchData;
private:
	void __fastcall UpdateMatch(void);
	int __fastcall CalcSim(void);
	void __fastcall UpdateView(int bwflag);
	double __fastcall CalcSWR(double f, double R, double jX);
	void __fastcall CalcZ(double f, double &R, double &jX);
	void __fastcall AdjX(double f, double fo, double &X);

	int __fastcall CheckSWRDisp(void);
	double __fastcall GetSWRFreq(double sw, double f1, double f2, double s1, double s2);
	double __fastcall GetSWRWidth(double sw);

public:
	virtual __fastcall TBwDispDlg(TComponent* AOwner);

	int __fastcall Execute(ANTDEF *p, TColor *pc);
};
//----------------------------------------------------------------------------
//extern TBwDispDlg *BwDispDlg;
//----------------------------------------------------------------------------
#endif

