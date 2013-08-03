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
#ifndef WireCadH
#define WireCadH
//----------------------------------------------------------------------------
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
#include <Menus.hpp>

#include "ComLib.h"
//----------------------------------------------------------------------------
class CSaveWire {
public:
	int		Flag;
	int		wmax;
    int		Sel;
    WDEF	wdef[WMAX];
public:
	CSaveWire(){
		Flag = 0;
    };
	inline int IsSwap(void){
    	return Flag;
    };
    inline void Align(CSaveWire &sw){
		SetData(sw.Sel, sw.wmax, sw.wdef);
    };
	inline void SetData(int sel, int max, WDEF *wp){
		Sel = sel;
		wmax = max;
        memcpy(wdef, wp, sizeof(wdef));
        Flag = 1;
    };
    inline void SwapData(int &sel, int &max, WDEF *wp){
		if( !Flag ) return;
		int ms = max;
        max = wmax;
        wmax = ms;
        ms = sel;
        sel = Sel;
        Sel = ms;
        WDEF *sp = new WDEF[WMAX];
        memcpy(sp, wp, sizeof(wdef));
        memcpy(wp, wdef, sizeof(wdef));
        memcpy(wdef, sp, sizeof(wdef));
        delete sp;
    };
};
//----------------------------------------------------------------------------
class TWireCadDlg : public TForm
{
__published:
	TButton *OKBtn;
	TButton *CancelBtn;
	TPageControl *Page;
	TTabSheet *TabAll;
	TTabSheet *TabXY;
	TTabSheet *TabXZ;
	TTabSheet *TabYZ;
	TPaintBox *PBoxALL;
	TPaintBox *PBoxXY;
	TPaintBox *PBoxXZ;
	TPaintBox *PBoxYZ;
	TTrackBar *TBSC;
	TLabel *Label1;
	TSpeedButton *SBSel;
	TSpeedButton *SBLine;
	TSpeedButton *SBLoop;
	TPopupMenu *PopupMenu;
	TMenuItem *K1;
	TMenuItem *K2;
	TGroupBox *GroupBox1;
	TCheckBox *ChkGrid;
	TComboBox *GridWidth;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *LCursor;
	TLabel *LWire;
	TMenuItem *K4;
	TMenuItem *N1;
	TMenuItem *K5;
	TMenuItem *N2;
	TMenuItem *N3;
	TMenuItem *K6;
	TMenuItem *K7;
	TMenuItem *KX11;
	TMenuItem *K8;
	TMenuItem *K10;
	TTimer *Timer;
	TLabel *Label4;
	TComboBox *SelFilter;
	TSpeedButton *SBView;
	TCheckBox *ChkAutoGrid;
	TMenuItem *N4;
	TMenuItem *K11;
	TMenuItem *K3;
	TMenuItem *K12;
	TMenuItem *K13;
	TMenuItem *K14;
	TMenuItem *K15;
	TMenuItem *K16;
	TMenuItem *K17;
	TMenuItem *K18;
	TMenuItem *K19;
	TMenuItem *N5;
	TScrollBar *HSBXY;
	TScrollBar *VSBXY;
	TScrollBar *HSBAll;
	TScrollBar *VSBAll;
	TScrollBar *HSBXZ;
	TScrollBar *VSBXZ;
	TScrollBar *HSBYZ;
	TScrollBar *VSBYZ;
	TMenuItem *KU1;
	TMenuItem *KS1;
	TMenuItem *KP2;
	TMenuItem *KP3;
	TMenuItem *KP4;
	TMenuItem *KP5;
	TMenuItem *KP6;
	TMenuItem *KP7;
	TMenuItem *KP8;
	TMenuItem *KP9;
	TMenuItem *KP10;
	TTrackBar *TBDEG;
	TLabel *Label5;
	TTrackBar *TBZDEG;
	TMenuItem *K9;
	TSpeedButton *SBOrg;
	void __fastcall FormResize(TObject *Sender);
	
	void __fastcall PBoxALLPaint(TObject *Sender);
	void __fastcall TBSCChange(TObject *Sender);
	void __fastcall PBoxXYPaint(TObject *Sender);
	
	
	void __fastcall PBoxXZPaint(TObject *Sender);
	void __fastcall PBoxYZPaint(TObject *Sender);
	
	
	void __fastcall PBoxALLMouseDown(TObject *Sender, TMouseButton Button,
	TShiftState Shift, int X, int Y);
	void __fastcall PBoxALLMouseUp(TObject *Sender, TMouseButton Button,
	TShiftState Shift, int X, int Y);
	void __fastcall PBoxALLClick(TObject *Sender);
	void __fastcall PBoxXYMouseDown(TObject *Sender, TMouseButton Button,
	TShiftState Shift, int X, int Y);
	void __fastcall PBoxXYMouseUp(TObject *Sender, TMouseButton Button,
	TShiftState Shift, int X, int Y);
	void __fastcall PBoxXYMouseMove(TObject *Sender, TShiftState Shift, int X,
	int Y);
	void __fastcall PBoxXYClick(TObject *Sender);
	void __fastcall SBSelClick(TObject *Sender);
	void __fastcall SBLineClick(TObject *Sender);
	void __fastcall SBLoopClick(TObject *Sender);
	void __fastcall PopupMenuPopup(TObject *Sender);
	
	void __fastcall K3Click(TObject *Sender);
	void __fastcall GridWidthChange(TObject *Sender);
	void __fastcall ChkGridClick(TObject *Sender);
	void __fastcall K1Click(TObject *Sender);
	void __fastcall PageChange(TObject *Sender);
	void __fastcall K4Click(TObject *Sender);
	void __fastcall K5Click(TObject *Sender);
	void __fastcall K7Click(TObject *Sender);
	void __fastcall KX11Click(TObject *Sender);
	void __fastcall K8Click(TObject *Sender);
	
	
	

	
	
	void __fastcall K10Click(TObject *Sender);
	void __fastcall TimerTimer(TObject *Sender);
	
	
	
	
	
	
	
	void __fastcall PBoxXZMouseDown(TObject *Sender, TMouseButton Button,
	TShiftState Shift, int X, int Y);
	void __fastcall PBoxXZMouseUp(TObject *Sender, TMouseButton Button,
	TShiftState Shift, int X, int Y);
	void __fastcall PBoxXZMouseMove(TObject *Sender, TShiftState Shift, int X,
	int Y);
	void __fastcall PBoxXZClick(TObject *Sender);
	
	
	
	
	void __fastcall PBoxYZMouseDown(TObject *Sender, TMouseButton Button,
	TShiftState Shift, int X, int Y);
	void __fastcall PBoxYZMouseUp(TObject *Sender, TMouseButton Button,
	TShiftState Shift, int X, int Y);
	void __fastcall PBoxYZMouseMove(TObject *Sender, TShiftState Shift, int X,
	int Y);
	void __fastcall PBoxYZClick(TObject *Sender);
	void __fastcall SelFilterChange(TObject *Sender);
	void __fastcall SBViewClick(TObject *Sender);
	void __fastcall K11Click(TObject *Sender);
	
	void __fastcall K17Click(TObject *Sender);
	void __fastcall K18Click(TObject *Sender);
	void __fastcall K19Click(TObject *Sender);
	void __fastcall K13Click(TObject *Sender);
	void __fastcall K14Click(TObject *Sender);
	void __fastcall K15Click(TObject *Sender);
	void __fastcall HSBAllChange(TObject *Sender);
	void __fastcall VSBAllChange(TObject *Sender);
	void __fastcall HSBXYChange(TObject *Sender);
	void __fastcall VSBXYChange(TObject *Sender);
	void __fastcall HSBXZChange(TObject *Sender);
	void __fastcall VSBXZChange(TObject *Sender);
	void __fastcall HSBYZChange(TObject *Sender);
	void __fastcall VSBYZChange(TObject *Sender);
	void __fastcall HSBAllScroll(TObject *Sender, TScrollCode ScrollCode,
	int &ScrollPos);
	void __fastcall KU1Click(TObject *Sender);
	void __fastcall KS1Click(TObject *Sender);
	void __fastcall PBoxALLMouseMove(TObject *Sender, TShiftState Shift, int X,
	int Y);
	void __fastcall KP2Click(TObject *Sender);
	void __fastcall KP3Click(TObject *Sender);
	void __fastcall KP4Click(TObject *Sender);
	void __fastcall KP5Click(TObject *Sender);
	void __fastcall KP6Click(TObject *Sender);
	void __fastcall KP7Click(TObject *Sender);
	void __fastcall KP8Click(TObject *Sender);
	void __fastcall KP9Click(TObject *Sender);
	void __fastcall KP10Click(TObject *Sender);
	void __fastcall TBDEGChange(TObject *Sender);
	void __fastcall K9Click(TObject *Sender);
	
private:
	void __fastcall UpdateView(void);
	int __fastcall IsWire(double x1, double y1, double z1, double x2, double y2, double z2);
	int __fastcall IsWire(WDEF &wd);
	int __fastcall IsCon(int wno, double x, double y, double z);
	int __fastcall IsConnect(int wno, double x, double y, double z);
	void __fastcall Connect(int w, double X, double Y, double Z);
	void __fastcall EntryGroup(void);
	int __fastcall IsGroup(int wno);
	int __fastcall IsFilter(int wno);
	TPaintBox* __fastcall GetPB(void);
	void __fastcall DrawDot(TPaintBox *tp, int X, int Y, int SW);
	void __fastcall DrawNot(TPaintBox *tp, int x1, int y1, int x2, int y2);
	void __fastcall DrawNot(TPaintBox *tp, double x1, double y1, double x2, double y2);
	void __fastcall DrawNotLoop(TPaintBox *tp, double x1, double y1, double x2, double y2);
	void __fastcall DrawGage(TPaintBox *tp);
	void __fastcall DrawGrid(TPaintBox *tp);
	void __fastcall NormalX(double &X);
	void __fastcall NormalY(double &Y);
	void __fastcall NormalXY(double &X, double &Y);
	void __fastcall NormalXXYY(double X1, double Y1, double &X2, double &Y2);
	void __fastcall DispCursor(void);
	void __fastcall DispSelWire(void);
	void __fastcall DispWire(void);
	double __fastcall GetXYZ(int sw);
	void __fastcall AdjAngle(double &X1, double &Y1, double &X2, double &Y2);
	int __fastcall CheckFree(int N);
	int __fastcall GetNewWire(void);
	void __fastcall AdjLoop(double &X1, double &Y1, double &X2, double &Y2);
	void __fastcall PartWire(int n);

	void __fastcall EntryAlignControl(void);
	int __fastcall IsWireHitAll(int w, int X, int Y);
	int __fastcall SelectWireAllSub(int X, int Y);
	int __fastcall SelectWireAll(int X, int Y);
	int __fastcall SelectWireXY(int X, int Y);
	int __fastcall SelectWireXZ(int X, int Y);
	int __fastcall SelectWireYZ(int X, int Y);
	void __fastcall AdjGrid(int sw);
	void __fastcall AdjSeg(int wno);
	void __fastcall MemoryView(int n);
	void __fastcall RestView(int n);
	void __fastcall SetShiftStat(TShiftState Shift);
	void __fastcall AdjPie(double X1, double Y1, double X2, double Y2, double &X, double &Y);
	void __fastcall SetupSB(void);
	void __fastcall Scroll(void);
	void __fastcall ScrollEdge(TScrollBar *sp);


	CAlignList	AlignList;
	ANTDEF	*pAnt;

    CSaveWire	SaveWire;
    int		EnbSetup;
	int		TimerCmd;

    int		Filter;
	int		wmax;
    WDEF	wdef[WMAX];
    WDEF	wsave;
    WDEF	wdisp;

	int		gmax;
	BYTE	glist[WMAX];

	int		Mode;
	double	Grid;
    double	GridX;
    double	GridY;
    int		SelWire;
	int		SelCmd;
	double	AntDeg;
	double	AntZDeg;
    double	AntXc;
    double	AntYc;
    double	AntSC;
	int		ScrXc;
    int		ScrYc;
	double	HSBXc;
    double	VSBYc;
    double	SBSC;
	int		AntMX;
    int		AntMY;
    int		SaveMX;
    int		SaveMY;
    int		MoveF;
    int		DownF;
    int		ShiftF;
    int		ScrollF;

	CClip	Clip;

public:
	virtual __fastcall TWireCadDlg(TComponent* AOwner);
	int __fastcall Execute(ANTDEF *p, int &Sel);
};
//----------------------------------------------------------------------------
//extern TWireCadDlg *WireCadDlg;

class CViewPos {
public:
	int		Flag;
	int		Page;
    int		Pos;
    int		Sel;
	int		Mode;
	int		Filter;
	int		Gf;
    int		Deg;
    int		ZDeg;
	double	Xc;
	double	Yc;
    double	Gw;
	double	Gx;
    double	Gy;
public:
	CViewPos(){
		Flag = 0;
    };
};
//----------------------------------------------------------------------------
#endif
