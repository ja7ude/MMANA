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
#ifndef StackDlgH
#define StackDlgH
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

#include "ComLib.h"
//----------------------------------------------------------------------------
class TStackDlgBox : public TForm
{
__published:        
	TButton *OKBtn;
	TButton *CancelBtn;
	TBevel *Bevel1;
	TLabel *Label1;
	TComboBox *HC;
	TLabel *Label2;
	TComboBox *VC;
	TLabel *Label3;
	TLabel *Label4;
	TEdit *HW;
	TEdit *VW;
	TLabel *LHW;
	TLabel *LVW;
	TCheckBox *CBRmdSel;
	TCheckBox *CBSame;
	TRadioGroup *VT;
	TButton *AntNewBtn;
	void __fastcall CBRmdSelClick(TObject *Sender);
	void __fastcall CBSameClick(TObject *Sender);
	void __fastcall HWChange(TObject *Sender);
	void __fastcall VWChange(TObject *Sender);
	void __fastcall HCChange(TObject *Sender);
	void __fastcall VCChange(TObject *Sender);
private:
	ANTDEF		*pAnt;
	int			RmdSel;
    double		RMD;
    int			ChgEnb;
	double		StackHW;
    double		StackVW;

	void __fastcall UpdateView(void);
	void __fastcall UpdateAntBtn(void);
	void __fastcall SetWireW(double &vw, double &hw);

public:
	virtual __fastcall TStackDlgBox(TComponent* AOwner);

	int __fastcall Execute(ANTDEF *ap);
};
//----------------------------------------------------------------------------
//extern TStackDlgBox *StackDlgBox;
//----------------------------------------------------------------------------
#endif    
