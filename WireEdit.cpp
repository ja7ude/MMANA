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

//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "WireEdit.h"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
//TWireEditDlg *WireEditDlg;
//---------------------------------------------------------------------
__fastcall TWireEditDlg::TWireEditDlg(TComponent* AOwner)
	: TForm(AOwner)
{
	EnbCalc = 0;
	memset(&NewW, 0, sizeof(NewW));
	WL = XYD = YZD = 0.0;
}
//---------------------------------------------------------------------
void __fastcall TWireEditDlg::DispItem(int n)
{
	EnbCalc = 0;
	if( n != 0 ) EditX1->Text = StrDbl(GetRmdVal(NewW.X1));
	if( n != 1 ) EditY1->Text = StrDbl(GetRmdVal(NewW.Y1));
	if( n != 2 ) EditZ1->Text = StrDbl(GetRmdVal(NewW.Z1));
	if( n != 3 ) EditX2->Text = StrDbl(GetRmdVal(NewW.X2));
	if( n != 4 ) EditY2->Text = StrDbl(GetRmdVal(NewW.Y2));
	if( n != 5 ) EditZ2->Text = StrDbl(GetRmdVal(NewW.Z2));
	if( n != 6 ) EditR->Text = StrDbl(GetRmdValD(NewW.R * 1000.0));
	if( n != 7 ){
		EditSeg->Text = Seg2Str(NewW.SEG);
	}
	if( n != 10 ) EditLen->Text = StrDbl(GetRmdVal(WL));
	if( n != 11 ) EditHDeg->Text = StrDbl(XYD);
	if( n != 12 ) EditVDeg->Text = StrDbl(YZD);
	EnbCalc = 1;
	InitBtn->Enabled = memcmp(&NewW, &NowW, sizeof(NewW)) ? TRUE : FALSE;
}
//---------------------------------------------------------------------
void __fastcall TWireEditDlg::UpdateView(void)
{
	LPCSTR lp = exeenv.RmdSel ? "λ":"ｍ";
	Label7->Caption = lp;
	Label8->Caption = lp;
	Label9->Caption = lp;
	Label10->Caption = lp;
	Label11->Caption = lp;
	Label12->Caption = lp;
	Label19->Caption = lp;
	Label15->Caption = exeenv.RmdSel ? lp : "mm";
}
//---------------------------------------------------------------------
int __fastcall TWireEditDlg::Execute(WDEF *p, int n, int max)
{
	char	bf[80];

	sprintf(bf, "ワイヤ(極座標)編集 - Wire No.%d", n + 1);
	Caption = bf;
	int RmdSel = exeenv.RmdSel;
	ChkRmd->Checked = exeenv.RmdSel;
	memcpy(&NowW, &p[n], sizeof(NowW));
	memcpy(&NewW, &p[n], sizeof(NewW));
	if( (SGN(NowW.Y1)*SGN(NowW.Y2) == -1) || (SGN(NowW.Z1)*SGN(NowW.Z2) == -1) || (SGN(NowW.X1)*SGN(NowW.X2) == -1) ){
		BaseSel->ItemIndex = 2;
	}
	else if( ((NowW.Y1==0)&&(NowW.Y2)) || ((NowW.Z1==0)&&(NowW.Z2)) || ((NowW.X1==0)&&(NowW.X2)) ){
		BaseSel->ItemIndex = 0;
	}
	else if( ((NowW.Y2==0)&&(NowW.Y1)) || ((NowW.Z2==0)&&(NowW.Z1)) || ((NowW.X2==0)&&(NowW.X1)) ){
		BaseSel->ItemIndex = 1;
	}
	CalcDegLen();
	DispItem(-1);
	if( ShowModal() == IDOK ){
		int f = 0;
		NewW.X1 = RoundUpStr(NewW.X1);
		NewW.Y1 = RoundUpStr(NewW.Y1);
		NewW.Z1 = RoundUpStr(NewW.Z1);
		NewW.X2 = RoundUpStr(NewW.X2);
		NewW.Y2 = RoundUpStr(NewW.Y2);
		NewW.Z2 = RoundUpStr(NewW.Z2);
		NewW.R = RoundUpStr(NewW.R);
		if( (NowW.X1 != NewW.X1)||(NowW.Y1 != NewW.Y1)||(NowW.Z1 != NewW.Z1) ){
			f = 1;
			if( ChkChen->Checked == TRUE ){
				AdjWireChen(p, max, NowW.X1, NowW.Y1, NowW.Z1, NewW.X1, NewW.Y1, NewW.Z1);
			}
		}
		if( (NowW.X2 != NewW.X2)||(NowW.Y2 != NewW.Y2)||(NowW.Z2 != NewW.Z2) ){
			f = 1;
			if( ChkChen->Checked == TRUE ){
				AdjWireChen(p, max, NowW.X2, NowW.Y2, NowW.Z2, NewW.X2, NewW.Y2, NewW.Z2);
			}
		}
		if( (NowW.R != NewW.R) || (NowW.SEG != NewW.SEG) ) f = 1;
		if( f == 0 ) return FALSE;		// 変更なし
		memcpy(&p[n], &NewW, sizeof(WDEF));
		exeenv.RmdSel = RmdSel;
		return TRUE;
	}
	else {
		exeenv.RmdSel = RmdSel;
		return FALSE;
	}
}
//---------------------------------------------------------------------------
// 現状の座標から長さと角度を得る
void __fastcall TWireEditDlg::CalcDegLen(void)
{
	GetWirePoll(WL, XYD, YZD, &NewW);
}
//---------------------------------------------------------------------------
// 現状の長さと角度から新しい座標を得る
void __fastcall TWireEditDlg::CalcXYZ(void)
{
	double XX = WL * sin(YZD*PAI/180.0) * cos(XYD*PAI/180.0);
	double YY = WL * sin(YZD*PAI/180.0) * sin(XYD*PAI/180.0);
	double ZZ = WL * cos(YZD*PAI/180.0);
	double XC = (NowW.X1 + NowW.X2)/2.0;
	double YC = (NowW.Y1 + NowW.Y2)/2.0;
	double ZC = (NowW.Z1 + NowW.Z2)/2.0;
	switch(BaseSel->ItemIndex){
		case 0:		// 始点
			NewW.X1 = NowW.X1;
			NewW.Y1 = NowW.Y1;
			NewW.Z1 = NowW.Z1;
			NewW.X2 = NewW.X1 + XX;
			NewW.Y2 = NewW.Y1 + YY;
			NewW.Z2 = NewW.Z1 + ZZ;
			break;
		case 1:		// 終点
			NewW.X2 = NowW.X2;
			NewW.Y2 = NowW.Y2;
			NewW.Z2 = NowW.Z2;
			NewW.X1 = NewW.X2 - XX;
			NewW.Y1 = NewW.Y2 - YY;
			NewW.Z1 = NewW.Z2 - ZZ;
			break;
		case 2:		// 中心
			XX /= 2.0;
			YY /= 2.0;
			ZZ /= 2.0;
			NewW.X1 = XC - XX;
			NewW.Y1 = YC - YY;
			NewW.Z1 = ZC - ZZ;
			NewW.X2 = XC + XX;
			NewW.Y2 = YC + YY;
			NewW.Z2 = ZC + ZZ;
			break;
	}
}
//---------------------------------------------------------------------
void __fastcall TWireEditDlg::EditX1Change(TObject *Sender)
{
	if( !EnbCalc ) return;
	double	d;
	if( Calc(d, AnsiString(EditX1->Text).c_str()) == TRUE ){	// ja7ude 1.0
		d = SetRmdVal(d);
		if( d != NewW.X1 ){
			NewW.X1 = d;
			CalcDegLen();
			DispItem(0);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TWireEditDlg::EditY1Change(TObject *Sender)
{
	if( !EnbCalc ) return;
	double	d;
	if( Calc(d, AnsiString(EditY1->Text).c_str()) == TRUE ){	// ja7ude 1.0
		d = SetRmdVal(d);
		if( d != NewW.Y1 ){
			NewW.Y1 = d;
			CalcDegLen();
			DispItem(1);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TWireEditDlg::EditZ1Change(TObject *Sender)
{
	if( !EnbCalc ) return;
	double	d;
	if( Calc(d, AnsiString(EditZ1->Text).c_str()) == TRUE ){	// ja7ude 1.0
		d = SetRmdVal(d);
		if( d != NewW.Z1 ){
			NewW.Z1 = d;
			CalcDegLen();
			DispItem(2);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TWireEditDlg::EditX2Change(TObject *Sender)
{
	if( !EnbCalc ) return;
	double	d;
	if( Calc(d, AnsiString(EditX2->Text).c_str()) == TRUE ){	// ja7ude 1.0
		d = SetRmdVal(d);
		if( d != NewW.X2 ){
			NewW.X2 = d;
			CalcDegLen();
			DispItem(3);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TWireEditDlg::EditY2Change(TObject *Sender)
{
	if( !EnbCalc ) return;
	double	d;
	if( Calc(d, AnsiString(EditY2->Text).c_str()) == TRUE ){	// ja7ude 1.0
		d = SetRmdVal(d);
		if( d != NewW.Y2 ){
			NewW.Y2 = d;
			CalcDegLen();
			DispItem(4);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TWireEditDlg::EditZ2Change(TObject *Sender)
{
	if( !EnbCalc ) return;
	double	d;
	if( Calc(d, AnsiString(EditZ2->Text).c_str()) == TRUE ){	// ja7ude 1.0
		d = SetRmdVal(d);
		if( d != NewW.Z2 ){
			NewW.Z2 = d;
			CalcDegLen();
			DispItem(5);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TWireEditDlg::EditRChange(TObject *Sender)
{
	if( !EnbCalc ) return;
	double	d;
	if( Calc(d, AnsiString(EditR->Text).c_str()) == TRUE ){
		d = SetRmdVal(d) / 1000.0;
		if( d != NewW.R ){
			NewW.R = d;
			DispItem(6);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TWireEditDlg::EditSegChange(TObject *Sender)
{
	if( !EnbCalc ) return;
	int d;
	if( Str2Seg(d, AnsiString(EditSeg->Text).c_str()) == TRUE ){
		if( d != NewW.SEG ){
			NewW.SEG = d;
			DispItem(7);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TWireEditDlg::EditLenChange(TObject *Sender)
{
	if( !EnbCalc ) return;
	double	d;
	if( Calc(d, AnsiString(EditLen->Text).c_str()) == TRUE ){
		d = SetRmdVal(d);
		if( d != WL ){
			WL = d;
			CalcXYZ();
			DispItem(10);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TWireEditDlg::EditHDegChange(TObject *Sender)
{
	if( !EnbCalc ) return;
	double	d;
	if( Calc(d, AnsiString(EditHDeg->Text).c_str()) == TRUE ){
		if( d != XYD ){
			XYD = d;
			CalcXYZ();
			DispItem(11);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TWireEditDlg::EditVDegChange(TObject *Sender)
{
	if( !EnbCalc ) return;
	double	d;
	if( Calc(d, AnsiString(EditVDeg->Text).c_str()) == TRUE ){
		if( d != YZD ){
			YZD = d;
			CalcXYZ();
			DispItem(12);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TWireEditDlg::BaseSelClick(TObject *Sender)
{
	if( !EnbCalc ) return;
	CalcXYZ();
	DispItem(-1);
}
//---------------------------------------------------------------------------
void __fastcall TWireEditDlg::InitBtnClick(TObject *Sender)
{
	if( !EnbCalc ) return;
	memcpy(&NewW, &NowW, sizeof(NewW));
	CalcDegLen();
	DispItem(-1);
}
//---------------------------------------------------------------------------
void __fastcall TWireEditDlg::ChkRmdClick(TObject *Sender)
{
	if( !EnbCalc ) return;
	exeenv.RmdSel = ChkRmd->Checked;
	UpdateView();
	DispItem(-1);
}
//---------------------------------------------------------------------------
