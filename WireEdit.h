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
#ifndef WireEditH
#define WireEditH
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

#include "ComLib.H"
//----------------------------------------------------------------------------
class TWireEditDlg : public TForm
{
__published:        
	TButton *OKBtn;
	TButton *CancelBtn;
	TGroupBox *GroupBox1;
	TLabel *Label1;
	TEdit *EditX1;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *Label6;
	TEdit *EditX2;
	TEdit *EditY1;
	TEdit *EditZ1;
	TEdit *EditY2;
	TEdit *EditZ2;
	TLabel *Label7;
	TLabel *Label8;
	TLabel *Label9;
	TLabel *Label10;
	TLabel *Label11;
	TLabel *Label12;
	TLabel *Label13;
	TLabel *Label14;
	TEdit *EditSeg;
	TEdit *EditR;
	TLabel *Label15;
	TGroupBox *GroupBox2;
	TLabel *Label16;
	TLabel *Label17;
	TRadioGroup *BaseSel;
	TLabel *Label18;
	TEdit *EditLen;
	TEdit *EditHDeg;
	TEdit *EditVDeg;
	TCheckBox *ChkChen;
	TLabel *Label19;
	TLabel *Label20;
	TLabel *Label21;
	TButton *InitBtn;
	TCheckBox *ChkRmd;
	void __fastcall EditX1Change(TObject *Sender);
	void __fastcall EditY1Change(TObject *Sender);
	void __fastcall EditZ1Change(TObject *Sender);
	void __fastcall EditX2Change(TObject *Sender);
	void __fastcall EditY2Change(TObject *Sender);
	void __fastcall EditZ2Change(TObject *Sender);
	void __fastcall EditRChange(TObject *Sender);
	void __fastcall EditSegChange(TObject *Sender);
	void __fastcall EditLenChange(TObject *Sender);
	void __fastcall EditHDegChange(TObject *Sender);
	void __fastcall EditVDegChange(TObject *Sender);
	void __fastcall BaseSelClick(TObject *Sender);
	void __fastcall InitBtnClick(TObject *Sender);
	void __fastcall ChkRmdClick(TObject *Sender);
private:
	int		EnbCalc;
    WDEF	NowW;
    WDEF	NewW;
    double	WL;
    double	XYD;
    double	YZD;

	void __fastcall UpdateView(void);
	void __fastcall DispItem(int n);
	void __fastcall CalcDegLen(void);
	void __fastcall CalcXYZ(void);

public:
	virtual __fastcall TWireEditDlg(TComponent* AOwner);

	int __fastcall Execute(WDEF *wp, int n, int max);
};
//----------------------------------------------------------------------------
//extern TWireEditDlg *WireEditDlg;
//----------------------------------------------------------------------------
#endif
