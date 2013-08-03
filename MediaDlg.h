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
#ifndef MediaDlgH
#define MediaDlgH
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
#include <vcl\Grids.hpp>
*/

#include "ComLib.h"
#include <Menus.hpp>
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.StdCtrls.hpp>
//----------------------------------------------------------------------------
class TMediaDlgBox : public TForm
{
__published:
	TButton *OKBtn;
	TButton *CancelBtn;
	TStringGrid *Grid;
	TGroupBox *GroupBox1;
	TCheckBox *GScrFlag;
	TEdit *RadCnt;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TEdit *RadR;
	TLabel *Label4;
	TPopupMenu *PopupMenu;
	TMenuItem *K1;
	TMenuItem *K2;
	void __fastcall GridDrawCell(TObject *Sender, int Col, int Row, TRect &Rect,
	TGridDrawState State);

	void __fastcall GScrFlagClick(TObject *Sender);
	void __fastcall GridSetEditText(TObject *Sender, int ACol, int ARow,
	const UnicodeString Value);
	void __fastcall GridGetEditText(TObject *Sender, int ACol, int ARow,
	UnicodeString &Value);
	void __fastcall K1Click(TObject *Sender);
	void __fastcall K2Click(TObject *Sender);
	void __fastcall GridClick(TObject *Sender);
private:
	ENVDEF	lenv;

	void __fastcall UpdateUIAll(void);
	void __fastcall GridGetText(LPSTR t, long Col, long Row);
	void __fastcall GridNewLine(int Row);

public:
	virtual __fastcall TMediaDlgBox(TComponent* AOwner);

	int __fastcall Execute(void);
};
//----------------------------------------------------------------------------
//extern TMediaDlgBox *MediaDlgBox;
//----------------------------------------------------------------------------
#endif    
