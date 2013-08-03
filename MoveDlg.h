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
#ifndef MoveDlgH
#define MoveDlgH
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
*/
#include "ComLib.h"
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.StdCtrls.hpp>
//----------------------------------------------------------------------------
class TMoveDlgBox : public TForm
{
__published:
	TButton *OKBtn;
	TButton *CancelBtn;
	TRadioGroup *VectSel;
	TEdit *EditLen;
	TLabel *Label1;
	TLabel *Label2;
	TCheckBox *ChkWith;
	TCheckBox *ChkAll;
	void __fastcall ChkAllClick(TObject *Sender);
private:
	void __fastcall UpdateView(void);
public:
	virtual __fastcall TMoveDlgBox(TComponent* AOwner);

	int __fastcall Execute(ANTDEF *ap, int Row, int Col);
};
//----------------------------------------------------------------------------
//extern TMoveDlgBox *MoveDlgBox;
//----------------------------------------------------------------------------
#endif
