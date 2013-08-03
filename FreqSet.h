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
#ifndef FreqSetH
#define FreqSetH
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

#include "ComLib.h"
//----------------------------------------------------------------------------
class TFreqSetDlg : public TForm
{
__published:
	TButton *OKBtn;
	TButton *CancelBtn;
	TGroupBox *GroupBox1;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TEdit *FreqIntval;
	TLabel *Label4;
	TGroupBox *GroupBox2;
	TRadioGroup *MatchSel;
	TLabel *Label6;
	TEdit *MatchFreq;
	TLabel *Label7;
	TLabel *LCount;
	TLabel *Label5;
	TEdit *EndFreq;
	TLabel *Label8;
	TLabel *Label9;
	TEdit *BgnFreq;
	void __fastcall BgnFreqChange(TObject *Sender);
	void __fastcall MatchSelClick(TObject *Sender);
	void __fastcall OKBtnClick(TObject *Sender);
private:
	CRes	SaveRes;	// 結果のセーブ領域
	double	SaveFQ;		// 周波数のセーブ
	ANTDEF	*pAnt;

	int		CanCalc;
	void __fastcall DispCount(void);
    void __fastcall UpdateUIAll(void);
	int __fastcall MakeFile(void);
	int __fastcall MakeFile(LPCSTR pName);
	double __fastcall CalcSWR(BWS *np, BWS *mp);
	int __fastcall CalcOne(BWS *rp);
public:
	virtual __fastcall TFreqSetDlg(TComponent* AOwner);
	int __fastcall Execute(ANTDEF *ap);
};
//----------------------------------------------------------------------------
//extern TFreqSetDlg *FreqSetDlg;
//----------------------------------------------------------------------------
#endif
