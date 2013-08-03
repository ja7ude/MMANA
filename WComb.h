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
#ifndef WCombH
#define WCombH
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
#include <Grids.hpp>
//----------------------------------------------------------------------------
#include "Comlib.h"
#include <Menus.hpp>
class TWCombDlg : public TForm
{
__published:
	TButton *OKBtn;
	TStringGrid *Grid;
	TPopupMenu *PopupType;
	TMenuItem *K1;
	TMenuItem *K2;
	TButton *DelBtn;
	TMenuItem *K3;
	TMenuItem *K4;
	void __fastcall GridDrawCell(TObject *Sender, int Col, int Row,
	TRect &Rect, TGridDrawState State);
	void __fastcall GridGetEditText(TObject *Sender, int ACol, int ARow,
	UnicodeString &Value);
	void __fastcall GridSetEditText(TObject *Sender, int ACol, int ARow,
	const UnicodeString Value);
	void __fastcall K1Click(TObject *Sender);
	void __fastcall K2Click(TObject *Sender);
	void __fastcall DelBtnClick(TObject *Sender);
	void __fastcall K3Click(TObject *Sender);
	void __fastcall K4Click(TObject *Sender);
private:
	ANTDEF	*pAnt;
	int		pmax;
    PDEF	pdef[PTMAX];

	void __fastcall GridGetText(LPSTR t, long Col, long Row);
	void __fastcall NewLine(int Row);
	void __fastcall SetType(int type);
public:
	virtual __fastcall TWCombDlg(TComponent* AOwner);

	int __fastcall Execute(ANTDEF *ap);
};
//----------------------------------------------------------------------------
//extern TWCombDlg *WCombDlg;
//----------------------------------------------------------------------------
#endif    
