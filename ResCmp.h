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
#ifndef ResCmpH
#define ResCmpH
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
#include <Dialogs.hpp>
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.StdCtrls.hpp>
//----------------------------------------------------------------------------
class TResCmpDlg : public TForm
{
__published:
	TButton *OKBtn;
	TPaintBox *PBox;
	TStringGrid *Grid;
	TButton *LoadBtn;
	TOpenDialog *OpenDialog;
	TButton *DelBtn;
	TButton *ColBtn;
	TColorDialog *ColorDialog;
	TRadioGroup *WaveSel;
	void __fastcall PBoxPaint(TObject *Sender);
	void __fastcall GridDrawCell(TObject *Sender, int Col, int Row, TRect &Rect,
	TGridDrawState State);
	void __fastcall LoadBtnClick(TObject *Sender);
	void __fastcall DelBtnClick(TObject *Sender);
	void __fastcall ColBtnClick(TObject *Sender);
	void __fastcall FormResize(TObject *Sender);
	void __fastcall WaveSelClick(TObject *Sender);
private:
	TColor		*pColorTable;
	CDrawPtnH	DrawPtnH;
	CDrawPtnV	DrawPtnV;

	int		Cnt;			// 現在の比較データの数
	CRes	Res[RESMAX];	// 比較データの配列
	AnsiString	Fname[RESMAX];
	double	MaxG, MaxH, MaxV;		// 最大のゲイン

	void __fastcall CalcMaxDB(void);
	int __fastcall LoadResFile(LPCSTR pName);
	void __fastcall UpdateUIAll(void);

	CAlignList	AlignList;
	CAlignGrid	AlignGrid;
	void __fastcall EntryAlignControl(void);
public:
	virtual __fastcall TResCmpDlg(TComponent* AOwner);

	int __fastcall Execute(TColor *pct);
};
//----------------------------------------------------------------------------
//extern TResCmpDlg *ResCmpDlg;
//----------------------------------------------------------------------------
#endif
