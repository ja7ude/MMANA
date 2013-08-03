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
#ifndef ACalBoxH
#define ACalBoxH
//----------------------------------------------------------------------------
/* ja7ude 1.0
#include <vcl\System.hpp>
#include <vcl\Windows.hpp>
#include <vcl\SysUtils.hpp>
#include <vcl\Classes.hpp>
#include <vcl\Graphics.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Controls.hpp>
#include <vcl\Buttons.hpp>
#include <vcl\ExtCtrls.hpp>
#include <vcl\ComCtrls.hpp>
#include <vcl\Grids.hpp>
#include <vcl\Menus.hpp>
*/
#include "Comlib.h"
#include <System.Classes.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.StdCtrls.hpp>
//----------------------------------------------------------------------------
class TACalDlg : public TForm
{
__published:
	TButton *OKBtn;
	TButton *CancelBtn;
	TGroupBox *GroupBox1;
	TTrackBar *TBGain;
	TTrackBar *TBFB;
	TTrackBar *TBJX;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TStringGrid *Grid;
	TLabel *Label4;
	TButton *RefEle;
	TPopupMenu *PopupType;
	TMenuItem *K1;
	TMenuItem *K2;
	TMenuItem *K3;
	TMenuItem *K4;
	TPopupMenu *PopupWire;
	TMenuItem *Y11;
	TMenuItem *Z11;
	TMenuItem *Y21;
	TMenuItem *Z21;
	TPopupMenu *PopupEle;
	TMenuItem *K5;
	TMenuItem *K6;
	TMenuItem *K7;
	TMenuItem *Kor1;
	TPopupMenu *PopupLoad;
	TMenuItem *L1;
	TMenuItem *C1;
	TMenuItem *K8;
	TButton *DelBtn;
	TCheckBox *DispRes;
	TTrackBar *TBSWR;
	TLabel *Label5;
	TMenuItem *K9;
	TMenuItem *R1;
	TTrackBar *TBELV;
	TLabel *Label6;
	TButton *AllEle;
	TCheckBox *PitchAbs;
	TCheckBox *DegUnit;
	TTrackBar *TBMCH;
	TLabel *Label7;
	TButton *BandBtn;
	TCheckBox *TerDis;
	TPopupMenu *Popup;
	TMenuItem *K10;
	TMenuItem *K11;
	TMenuItem *N1;
	TMenuItem *K12;
	TPopupMenu *PopupWireP;
	TMenuItem *K13;
	TMenuItem *K14;
	TMenuItem *K15;
	TMenuItem *K16;
	TMenuItem *K17;
	TMenuItem *K18;
	TMenuItem *K19;
	TMenuItem *K20;
	TMenuItem *K21;
	TMenuItem *N2;
	TMenuItem *N3;
	TMenuItem *K22;
	TMenuItem *X21;
	TButton *TerBtn;
	TMenuItem *K23;
	TMenuItem *K24;
	TPopupMenu *PopupFeed;
	TMenuItem *K25;
	TMenuItem *K26;
	TMenuItem *K27;
	TMenuItem *K28;
	TPopupMenu *PopupStack;
	TMenuItem *K29;
	TMenuItem *K30;
	TMenuItem *K31;
	TMenuItem *K32;
	TMenuItem *K33;
	TTrackBar *TBCUR;
	TLabel *Label8;
	TMenuItem *K34;
	TMenuItem *K61;
	TMenuItem *K71;
	TMenuItem *Kor11;
	void __fastcall GridDrawCell(TObject *Sender, int Col, int Row, TRect &Rect,
	TGridDrawState State);
	void __fastcall RefEleClick(TObject *Sender);
	void __fastcall GridGetEditText(TObject *Sender, int ACol, int ARow,
	UnicodeString &Value);
	void __fastcall GridSetEditText(TObject *Sender, int ACol, int ARow,
	const UnicodeString Value);
	void __fastcall K1Click(TObject *Sender);
	void __fastcall K2Click(TObject *Sender);
	void __fastcall K3Click(TObject *Sender);
	void __fastcall K4Click(TObject *Sender);
	void __fastcall Y11Click(TObject *Sender);
	void __fastcall Z11Click(TObject *Sender);
	void __fastcall Y21Click(TObject *Sender);
	void __fastcall Z21Click(TObject *Sender);
	void __fastcall K8Click(TObject *Sender);
	void __fastcall DelBtnClick(TObject *Sender);
	
	void __fastcall K9Click(TObject *Sender);
	
	void __fastcall AllEleClick(TObject *Sender);
	void __fastcall PitchAbsClick(TObject *Sender);
	void __fastcall OKBtnClick(TObject *Sender);
	void __fastcall FormResize(TObject *Sender);
	void __fastcall BandBtnClick(TObject *Sender);
	void __fastcall TerDisClick(TObject *Sender);
	void __fastcall K11Click(TObject *Sender);
	void __fastcall PopupPopup(TObject *Sender);
	void __fastcall K12Click(TObject *Sender);
	void __fastcall K18Click(TObject *Sender);
	void __fastcall K19Click(TObject *Sender);
	void __fastcall K20Click(TObject *Sender);
	void __fastcall K21Click(TObject *Sender);
	
	void __fastcall TerBtnClick(TObject *Sender);
	void __fastcall K23Click(TObject *Sender);
	void __fastcall K24Click(TObject *Sender);
	
	
	void __fastcall K27Click(TObject *Sender);
	void __fastcall K28Click(TObject *Sender);
	
	void __fastcall K32Click(TObject *Sender);
	
	void __fastcall K34Click(TObject *Sender);
	
	
	
private:
	CACal *pACal;

	CAlignList	AlignList;
	CAlignGrid	AlignGrid;
	void __fastcall EntryAlignControl(void);

    int		WHFlag;	// â°ïùÅEècïùòAågÉtÉâÉO
	int		WHHW, WHVW, WHXW;
    int		WHHC, WHVC, WHXC;
	void __fastcall SetEleWith(int n, int w, double a, double b);

private:
	void __fastcall UpdateView(void);
	void __fastcall GridGetText(LPSTR t, long Col, long Row);
	void __fastcall GridNewLine(int Row);
	void __fastcall SetType(int n);
	void __fastcall SetSubPos(int n);
	int __fastcall CheckSameWireChen(double ox, double oy, double oz);
	void __fastcall AddSameWireChen(int n);
	void __fastcall AddSameLoadChen(int n);
	void __fastcall AddEle(int n);
	void __fastcall AdjWithEle(void);


	void __fastcall GetWithStr(LPSTR t, AVAL *ap);
	int __fastcall SetWithValue(int ARow, LPSTR s, AVAL *ap);

public:
	virtual __fastcall TACalDlg(TComponent* AOwner);

	int __fastcall Execute(CACal *p);
};
//----------------------------------------------------------------------------
//extern TACalDlg *ACalDlg;
//----------------------------------------------------------------------------
#endif    
