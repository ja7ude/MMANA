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
#ifndef ACalMultH
#define ACalMultH
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
#include <Grids.hpp>

#include "ComLib.h"
//----------------------------------------------------------------------------
class TACalMultDlg : public TForm
{
__published:        
	TButton *OKBtn;
	TStringGrid *Grid;
	TButton *DelBtn;
	TCheckBox *Weight;
	void __fastcall GridDrawCell(TObject *Sender, int Col, int Row, TRect &Rect,
	TGridDrawState State);
	void __fastcall GridGetEditText(TObject *Sender, int ACol, int ARow,
	UnicodeString &Value);
	void __fastcall GridSetEditText(TObject *Sender, int ACol, int ARow,
	const UnicodeString Value);
	void __fastcall DelBtnClick(TObject *Sender);
private:
	CACal *pACal;
	void __fastcall GridGetText(LPSTR t, long Col, long Row);
	void __fastcall GridNewLine(int Row);
	void __fastcall UpdateView(void);
public:
	virtual __fastcall TACalMultDlg(TComponent* AOwner);
	int __fastcall Execute(CACal *p);
};
//----------------------------------------------------------------------------
//extern TACalMultDlg *ACalMultDlg;
//----------------------------------------------------------------------------
#endif    
