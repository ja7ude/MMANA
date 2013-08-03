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
#ifndef GrpWireH
#define GrpWireH
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
#include <vcl\ComCtrls.hpp>
#include <vcl\Menus.hpp>
#include <vcl\Dialogs.hpp>
*/
#include "ComLib.h"
#include <System.Classes.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.StdCtrls.hpp>
//----------------------------------------------------------------------------
#define	SAMECHECK	1	// 同一エレメントのチェック
class TGrpWireDlg : public TForm
{
__published:
	TButton *OKBtn;
	TButton *CancelBtn;
	TPageControl *Page;
	TTabSheet *TabSheet1;
	TTabSheet *TabSheet2;
	TStringGrid *Grid;
	TPopupMenu *PopupMenu;
	TMenuItem *K1;
	TMenuItem *KS1;
	TMenuItem *KS2;
	TMenuItem *N1;
	TMenuItem *KS3;
	TMenuItem *KS4;
	TMenuItem *KS5;
	TMenuItem *KS6;
	TMenuItem *N2;
	TMenuItem *KS7;
	TMenuItem *KS8;
	TMenuItem *KS9;
	TMenuItem *KS10;
	TMenuItem *K2;
	TMenuItem *K3;
	TMenuItem *N3;
	TLabel *HelpStat;
	TPaintBox *PBox;
	TTrackBar *TBarDeg;
	TTrackBar *TBarSC;
	TButton *AllViewBtn;
	TButton *OrgBtn;
	TLabel *Label1;
	TLabel *Label2;
	TMenuItem *K4;
	TMenuItem *N4;
	TCheckBox *ChkRmd;
	TCheckBox *ChkIntval;
	TMenuItem *K5;
	TTrackBar *TBarZDeg;
	TLabel *Label3;
	TCheckBox *Ant3D;
	TMenuItem *K6;
	TMenuItem *N5;
	TMenuItem *K7;
	TMenuItem *K8;
	TMenuItem *K9;
	TMenuItem *K10;
	TMenuItem *K11;
	TMenuItem *K12;
	TMenuItem *K13;
	TMenuItem *N6;
	TMenuItem *KCSV1;
	TMenuItem *K14;
	TRadioButton *EleWidthEdge;
	TRadioButton *EleWidthAll;
	void __fastcall GridDrawCell(TObject *Sender, int Col, int Row, TRect &Rect,
	TGridDrawState State);
	void __fastcall GridGetEditText(TObject *Sender, int ACol, int ARow,
	UnicodeString &Value);
	void __fastcall GridSetEditText(TObject *Sender, int ACol, int ARow,
	const UnicodeString Value);






	
	
	
	

	void __fastcall KS3Click(TObject *Sender);
	void __fastcall KS1Click(TObject *Sender);
	void __fastcall KS2Click(TObject *Sender);
	void __fastcall KS4Click(TObject *Sender);
	void __fastcall KS5Click(TObject *Sender);
	void __fastcall KS6Click(TObject *Sender);
	void __fastcall KS7Click(TObject *Sender);
	void __fastcall KS8Click(TObject *Sender);
	void __fastcall KS9Click(TObject *Sender);
	void __fastcall KS10Click(TObject *Sender);
	void __fastcall PopupMenuPopup(TObject *Sender);
	
	void __fastcall K2Click(TObject *Sender);
	void __fastcall K3Click(TObject *Sender);
	void __fastcall PBoxPaint(TObject *Sender);
	void __fastcall TBarDegChange(TObject *Sender);
	
	void __fastcall PBoxMouseDown(TObject *Sender, TMouseButton Button,
	TShiftState Shift, int X, int Y);
	void __fastcall OrgBtnClick(TObject *Sender);
	void __fastcall AllViewBtnClick(TObject *Sender);
	void __fastcall PageChange(TObject *Sender);
	void __fastcall FormResize(TObject *Sender);
	
	void __fastcall GridSelectCell(TObject *Sender, int Col, int Row,
	bool &CanSelect);
	
	
	
	
	

	
	
	void __fastcall K4Click(TObject *Sender);
	void __fastcall OKBtnClick(TObject *Sender);
	void __fastcall ChkRmdClick(TObject *Sender);
	void __fastcall ChkIntvalClick(TObject *Sender);
	void __fastcall K5Click(TObject *Sender);
	void __fastcall Ant3DClick(TObject *Sender);
	
	void __fastcall K6Click(TObject *Sender);
	void __fastcall K7Click(TObject *Sender);
	void __fastcall K8Click(TObject *Sender);
	void __fastcall K10Click(TObject *Sender);
	void __fastcall K11Click(TObject *Sender);
	void __fastcall K12Click(TObject *Sender);
	void __fastcall K13Click(TObject *Sender);
	void __fastcall KCSV1Click(TObject *Sender);
	void __fastcall K14Click(TObject *Sender);
private:
	int		Edit;
	int		SelOnly;
	int		SelWire;
	CACal	*pACal;

	CWGroupList	WList;
	ANTDEF	*pAnt;
	double	AntXc;
	double	AntYc;

	CClip	Clip;
	void __fastcall UpdateUIAll(void);
	void __fastcall GridGetText(LPSTR t, long Col, long Row);
	void __fastcall GridTitleText(LPSTR t, long Col, long Row);

	void __fastcall GridNewLine(int Row, int type = cwHDP);
	void __fastcall SetModify(void);
	int __fastcall IsEntry(int Pos, int SubPos);
	int __fastcall IsEntry(int Wno);
	int __fastcall IsSelect(int &Pos, int &SubPos, int Row, int Col);
	int __fastcall SelectWire(int X, int Y);

#if SAMECHECK
	int __fastcall IsSameElement(int i, int j);
	int __fastcall IsSameElement(void);
#endif

	void __fastcall AllRoundUp(double sc);

	CAlignList	AlignList;
	CAlignGrid	AlignGrid;
	void __fastcall EntryAlignControl(void);
public:
	virtual __fastcall TGrpWireDlg(TComponent* AOwner);
	int __fastcall Execute(ANTDEF *ap);
	int __fastcall Select(int &Pos, int &SubPos, ANTDEF *ap, CACal *pA);
};
//----------------------------------------------------------------------------
//extern TGrpWireDlg *GrpWireDlg;
//----------------------------------------------------------------------------
#endif    
