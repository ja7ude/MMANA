//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "TriEdit.h"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
//TTriEditDlg *TriEditDlg;
//---------------------------------------------------------------------
__fastcall TTriEditDlg::TTriEditDlg(TComponent* AOwner)
	: TForm(AOwner)
{
	EnbCalc = 0;
}
//---------------------------------------------------------------------------
int __fastcall TTriEditDlg::Execute(ANTDEF *p, int n)
{
	if( exeenv.RmdSel ){
		Label3->Caption = "λ";
	}
	ap = p;
	Index = n;
	memcpy(&NowW, &ap->wdef[n], sizeof(WDEF));
	memcpy(&NewW, &ap->wdef[n], sizeof(WDEF));
	if( (SGN(NowW.Y1)*SGN(NowW.Y2) == -1) || (SGN(NowW.Z1)*SGN(NowW.Z2) == -1) || (SGN(NowW.X1)*SGN(NowW.X2) == -1) ){
		BasicSel->ItemIndex = 2;
	}
	else if( ((NowW.Y1==0)&&(NowW.Y2)) || ((NowW.Z1==0)&&(NowW.Z2)) || ((NowW.X1==0)&&(NowW.X2)) ){
		BasicSel->ItemIndex = 0;
	}
	else if( ((NowW.Y2==0)&&(NowW.Y1)) || ((NowW.Z2==0)&&(NowW.Z1)) || ((NowW.X2==0)&&(NowW.X1)) ){
		BasicSel->ItemIndex = 1;
	}
	CalcNow();
	DispItem(-1);
	if( ShowModal() == IDOK ){
		int f = 0;
		if( (NowW.X1 != NewW.X1)||(NowW.Y1 != NewW.Y1)||(NowW.Z1 != NewW.Z1) ){
			f = 1;
			if( CheckSamePos(n, NowW.X1, NowW.Y1, NowW.Z1)==TRUE ) f = 2;
		}
		if( (NowW.X2 != NewW.X2)||(NowW.Y2 != NewW.Y2)||(NowW.Z2 != NewW.Z2) ){
			if( f != 2 ) f = 1;
			if( CheckSamePos(n, NowW.X2, NowW.Y2, NowW.Z2)==TRUE ) f = 2;
		}
		if( f == 0 ) return FALSE;		// 変更なし
		if( f == 2 ){
			if( YesNoMB("他のワイヤが連携しています.その座標も変更しますか？") != IDYES ) f = 1;
		}
		if( f == 1 ){
			memcpy(&ap->wdef[n], &NewW, sizeof(WDEF));
		}
		else {
			NewSamePos(NowW.X1, NowW.Y1, NowW.Z1, NewW.X1, NewW.Y1, NewW.Z1);
			NewSamePos(NowW.X2, NowW.Y2, NowW.Z2, NewW.X2, NewW.Y2, NewW.Z2);
		}
		return TRUE;
	}
	return FALSE;
}
//---------------------------------------------------------------------------
int __fastcall TTriEditDlg::CheckSamePos(int n, double x, double y, double z)
{
	int i;
	WDEF *wp = ap->wdef;
	for( i = 0; i < ap->wmax; i++, wp++ ){
		if( i != n ){
			if( (wp->X1==x) && (wp->Y1==y) && (wp->Z1==z) ) return TRUE;
			if( (wp->X2==x) && (wp->Y2==y) && (wp->Z2==z) ) return TRUE;
		}
	}
	return FALSE;
}
void __fastcall TTriEditDlg::NewSamePos(double bx, double by, double bz, double x, double y, double z)
{
	if( (bx == x) && (by == y) && (bz == z) ) return;
	int i;
	WDEF *wp = ap->wdef;
	for( i = 0; i < ap->wmax; i++, wp++ ){
		if( (wp->X1==bx) && (wp->Y1==by) && (wp->Z1==bz) ){
			wp->X1 = x;
			wp->Y1 = y;
			wp->Z1 = z;
		}
		if( (wp->X2==bx) && (wp->Y2==by) && (wp->Z2==bz) ){
			wp->X2 = x;
			wp->Y2 = y;
			wp->Z2 = z;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TTriEditDlg::DispItem(int n)
{
	EnbCalc = 0;
	char bf[128];
	sprintf(bf, "%10.5lf,%10.5lf,%10.5lf", GetRmdVal(NowW.X1), GetRmdVal(NowW.Y1), GetRmdVal(NowW.Z1));
	NowBgn->Caption = bf;
	sprintf(bf, "%10.5lf,%10.5lf,%10.5lf", GetRmdVal(NowW.X2), GetRmdVal(NowW.Y2), GetRmdVal(NowW.Z2));
	NowEnd->Caption = bf;
	sprintf(bf, "%10.5lf,%10.5lf,%10.5lf", GetRmdVal(NewW.X1), GetRmdVal(NewW.Y1), GetRmdVal(NewW.Z1));
	NewBgn->Caption = bf;
	sprintf(bf, "%10.5lf,%10.5lf,%10.5lf", GetRmdVal(NewW.X2), GetRmdVal(NewW.Y2), GetRmdVal(NewW.Z2));
	NewEnd->Caption = bf;

	if( n != 0 ) EditLen->Text = StrDbl(GetRmdVal(WL));
	if( n != 1 ) EditXY->Text = StrDbl(XYD);
	if( n != 2 ) EditYZ->Text = StrDbl(YZD);
	EnbCalc = 1;
}
//---------------------------------------------------------------------------
// 現状の座標から長さと角度を得る
void __fastcall TTriEditDlg::CalcNow(void)
{
	GetWirePoll(WL, XYD, YZD, &NewW);
}
//---------------------------------------------------------------------------
// 現状の長さと角度から新しい座標を得る
void __fastcall TTriEditDlg::CalcNew(void)
{
	double XX = WL * sin(YZD*PAI/180.0) * cos(XYD*PAI/180.0);
	double YY = WL * sin(YZD*PAI/180.0) * sin(XYD*PAI/180.0);
	double ZZ = WL * cos(YZD*PAI/180.0);
	double XC = (NowW.X1 + NowW.X2)/2.0;
	double YC = (NowW.Y1 + NowW.Y2)/2.0;
	double ZC = (NowW.Z1 + NowW.Z2)/2.0;
	switch(BasicSel->ItemIndex){
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
// ワイヤ長さの変更
void __fastcall TTriEditDlg::EditLenChange(TObject *Sender)
{
	if( !EnbCalc ) return;
	double	d;

	if( Calc(d, EditLen->Text.c_str()) == TRUE ){
		if( (d != WL) && (d > 0) ){
			WL = SetRmdVal(d);
			CalcNew();
			DispItem(0);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TTriEditDlg::EditXYChange(TObject *Sender)
{
	if( !EnbCalc ) return;
	double	d;

	if( Calc(d, EditXY->Text.c_str()) == TRUE ){
		if( (d != XYD) && (d >= -360.0) && (d <= 360.0) ){
			XYD = d;
			CalcNew();
			DispItem(1);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TTriEditDlg::EditYZChange(TObject *Sender)
{
	if( !EnbCalc ) return;
	double	d;

	if( Calc(d, EditYZ->Text.c_str()) == TRUE ){
		if( (d != YZD) && (d >= -360.0) && (d <= 360.0) ){
			YZD = d;
			CalcNew();
			DispItem(2);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TTriEditDlg::BasicSelClick(TObject *Sender)
{
	if( !EnbCalc ) return;

	CalcNew();
	DispItem(-1);
}
//---------------------------------------------------------------------------
