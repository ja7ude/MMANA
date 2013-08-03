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
#ifndef TextEditH
#define TextEditH
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.StdCtrls.hpp>
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
#include <vcl\Dialogs.hpp>
*/
//----------------------------------------------------------------------------
class TTextEditDlg : public TForm
{
__published:        
	TButton *OKBtn;
	TButton *CancelBtn;
	TMemo *Memo;
	TButton *FontBtn;
	TFontDialog *FontDialog;
	void __fastcall FontBtnClick(TObject *Sender);
	void __fastcall FormResize(TObject *Sender);
	
	
	
	
	void __fastcall FormPaint(TObject *Sender);
private:
	int		InitFlag;
	LPCSTR	ap;

	CAlignList	AlignList;
	void __fastcall EntryAlignControl(void);
public:
	virtual __fastcall TTextEditDlg(TComponent* AOwner);

	int __fastcall Execute(AnsiString &as, int flag, LPCSTR pTitle = NULL);
};
//----------------------------------------------------------------------------
extern TTextEditDlg *TextEditDlg;
//----------------------------------------------------------------------------
#endif    
