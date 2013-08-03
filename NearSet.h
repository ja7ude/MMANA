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
#ifndef NearSetH
#define NearSetH
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
class TNearSetDlg : public TForm
{
__published:
	TButton *OKBtn;
	TButton *CancelBtn;
	TRadioGroup *CalcSel;
	TGroupBox *GroupBox1;
	TLabel *Label1;
	TEdit *EditX;
	TEdit *EditXW;
	TEdit *EditXN;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *Label6;
	TEdit *EditY;
	TEdit *EditYW;
	TEdit *EditYN;
	TEdit *EditZ;
	TEdit *EditZW;
	TEdit *EditZN;
	TLabel *Label7;
	TEdit *EditName;
	TButton *FileBtn;
	TGroupBox *GroupBox2;
	TEdit *EditPow;
	TLabel *Label8;
	void __fastcall FileBtnClick(TObject *Sender);
private:
public:
	virtual __fastcall TNearSetDlg(TComponent* AOwner);

	int __fastcall Execute(void);
};
//----------------------------------------------------------------------------
//extern TNearSetDlg *NearSetDlg;
//----------------------------------------------------------------------------
#endif
