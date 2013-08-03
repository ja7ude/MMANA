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
#ifndef ACalResH
#define ACalResH
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
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.StdCtrls.hpp>
//----------------------------------------------------------------------------
class TACalResDlg : public TForm
{
__published:
	TButton *OKBtn;
	TButton *CancelBtn;
	TLabel *CalCond;
	TStringGrid *Grid;
	TComboBox *SortSel;
	TLabel *Label1;
	TPaintBox *PBox;
	void __fastcall GridDrawCell(TObject *Sender, int Col, int Row, TRect &Rect,
	TGridDrawState State);
	void __fastcall SortSelChange(TObject *Sender);
	void __fastcall PBoxPaint(TObject *Sender);
	void __fastcall FormResize(TObject *Sender);
	void __fastcall GridSelectCell(TObject *Sender, int Col, int Row,
	bool &CanSelect);
	void __fastcall OKBtnClick(TObject *Sender);
	
private:
	CACal	*pACal;
	int		CurRow;
	int		mmax;
	MVAL	mval[MVMAX];
	double	MaxG;
	double	MaxV;
    double	MaxH;
    double	MaxD;
	float	PtnH[180];
    float	PtnV[91];

	CDrawPtnH	DrawPtnH;
	CDrawPtnV	DrawPtnV;

	void __fastcall Sort(int n);

	CAlignList	AlignList;
	CAlignGrid	AlignGrid;
	void __fastcall EntryAlignControl(void);
	void __fastcall SetupPtn(int Row);

	int __fastcall GetCurIndex(void);
	void __fastcall ScrollIndex(int n);
public:
	virtual __fastcall TACalResDlg(TComponent* AOwner);

    int __fastcall Execute(CACal *p);
};
//----------------------------------------------------------------------------
//extern TACalResDlg *ACalResDlg;
//----------------------------------------------------------------------------
#endif
