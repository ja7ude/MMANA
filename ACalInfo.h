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
#ifndef ACalInfoH
#define ACalInfoH
//----------------------------------------------------------------------------	// ja7ude 1.0
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

#include "ComLib.h"
//----------------------------------------------------------------------------
class TACalInfoBox : public TForm
{
__published:
	TButton *OKBtn;
	TButton *CancelBtn;
	TGroupBox *GroupBox1;
	TEdit *EditGain;
	TLabel *Label1;
	TEdit *EditFB;
	TLabel *Label2;
	TRadioGroup *MatchSel;
	TLabel *Label3;
	TLabel *Label4;
	TUpDown *UDGain;
	TUpDown *UDFB;
	TEdit *EditJX;
	TLabel *Label5;
	TEdit *EditSWR;
	TLabel *Label6;
	TUpDown *UDJX;
	TLabel *Label7;
	TUpDown *UDSWR;
	TPageControl *PageControl1;
	TTabSheet *TabTer;
	TTabSheet *TabEnv;
	TGroupBox *GroupBox2;
	TLabel *Label8;
	TLabel *Label9;
	TEdit *EditFBH;
	TEdit *EditFBV;
	TUpDown *UDFBH;
	TUpDown *UDFBV;
	TLabel *Label10;
	TLabel *Label11;
	TGroupBox *GroupBox3;
	TLabel *Label12;
	TGroupBox *GroupBox4;
	TEdit *EditTergR;
	TEdit *EditTergJX;
	TLabel *Label13;
	TLabel *Label14;
	TComboBox *EditSWRZ;
	TGroupBox *GroupBox5;
	TEdit *EditCur;
	TLabel *Label15;
	TRadioButton *RBIMAX;
	TRadioButton *RBIMIN;
	TLabel *Label16;
	TLabel *Label17;
	TEdit *EditSWRJX;
	TLabel *Label18;
	TButton *AutoJX;
	void __fastcall UDSWRClick(TObject *Sender, TUDBtnType Button);
	
	void __fastcall UDSWRChanging(TObject *Sender, bool &AllowChange);
	void __fastcall MatchSelClick(TObject *Sender);
	void __fastcall AutoJXClick(TObject *Sender);
private:
	void __fastcall TACalInfoBox::UpdateUIEditTerg(void);

public:
	virtual __fastcall TACalInfoBox(TComponent* AOwner);
	int __fastcall TACalInfoBox::Execute(CACal *p);
};
//----------------------------------------------------------------------------
//extern TACalInfoBox *ACalInfoBox;
//----------------------------------------------------------------------------
#endif
