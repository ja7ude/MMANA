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
#ifndef WireSclH
#define WireSclH
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
#include "ComLIb.h"
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.StdCtrls.hpp>
//----------------------------------------------------------------------------
class TWireScaleDlg : public TForm
{
__published:        
	TButton *OKBtn;
	TButton *CancelBtn;
	TBevel *Bevel1;
	TLabel *Label1;
	TLabel *DFreq;
	TLabel *Label2;
	TComboBox *TFreq;
	TLabel *Label3;
	TLabel *Label4;
	TEdit *TSC;
	TGroupBox *GroupBox;
	TCheckBox *WireRSW;
	TCheckBox *WireXSW;
	TCheckBox *WireYSW;
	TCheckBox *WireZSW;
	void __fastcall TFreqChange(TObject *Sender);
	void __fastcall TSCChange(TObject *Sender);
private:
	int		EnbCalc;
	ANTDEF	*ap;

    double	tfq;
    double	tsc;

	void __fastcall DispItem(int n);
public:
	virtual __fastcall TWireScaleDlg(TComponent* AOwner);

	int __fastcall Execute(ANTDEF *p);
};
//----------------------------------------------------------------------------
//extern TWireScaleDlg *WireScaleDlg;
//----------------------------------------------------------------------------
#endif    
