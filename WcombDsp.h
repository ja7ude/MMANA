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
#ifndef WcombDspH
#define WcombDspH
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
#include "ComLib.h"
//----------------------------------------------------------------------------
class TWCombDspDlg : public TForm
{
__published:
	TButton *OKBtn;
	TStringGrid *Grid;
	TLabel *LStat;
	void __fastcall GridDrawCell(TObject *Sender, int Col, int Row, TRect &Rect,
	TGridDrawState State);
private:
	int	wmax;
	WDEF wdef[PPMAX*2+2];
    double	Len[PPMAX*2+2];
public:
	virtual __fastcall TWCombDspDlg(TComponent* AOwner);

	void __fastcall Execute(ANTDEF *ap, PDEF *pp, WDEF *wp, int n);
};
//----------------------------------------------------------------------------
//extern TWCombDspDlg *WCombDspDlg;
//----------------------------------------------------------------------------
#endif    
