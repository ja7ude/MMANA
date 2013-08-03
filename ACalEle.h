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
#ifndef ACalEleH
#define ACalEleH
//----------------------------------------------------------------------------	// ja7ude 1.0
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <StdCtrls.hpp>
#include <Controls.hpp>
#include <Forms.hpp>
#include <Graphics.hpp>
#include <Classes.hpp>
#include <SysUtils.hpp>
#include <Windows.hpp>
#include <System.hpp>
//----------------------------------------------------------------------------
class TACalEleBox : public TForm
{
__published:
	TButton *OKBtn;
	TLabel *Label1;
	TGroupBox *GroupBox1;
	TCheckBox *CBHW;
	TRadioGroup *RGHW;
	TGroupBox *GroupBox2;
	TCheckBox *CBVW;
	TRadioGroup *RGVW;
	TGroupBox *GroupBox3;
	TCheckBox *CBXW;
	TRadioGroup *RGXW;
	void __fastcall CBHWClick(TObject *Sender);
private:
	void __fastcall UpdateItemIndex(void);
public:
	virtual __fastcall TACalEleBox(TComponent* AOwner);

	int __fastcall TACalEleBox::Execute(int &HW, int &VW, int &XW, int &HC, int &VC, int &XC);
};
//----------------------------------------------------------------------------
//extern TACalEleBox *ACalEleBox;
//----------------------------------------------------------------------------
#endif    
