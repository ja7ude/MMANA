//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "ComLib2.h"
//---------------------------------------------------------------------------
///----------------------------------------------------------------
///  CAidSpinƒNƒ‰ƒX
void CAidSpin::SetControl(TUpDown *pS, TControl *pC, double val, double min, double max, double dw)
{
	pSpin->Associate = pCtrl;
	pSpin = pS;
	pCtrl = pC;
	Val = val;
	Min = min;
	Max = max;
	DW = dw;
	pSpin->Associate = NULL;
}

void CAidSpin::Init(void)
{
	pSpin->Position = 50;
	pCtrl->Text = StrDbl(Val);
}

void CAidSpin::OnClick(TUDBtnType Button)
{
	double	d = Min;

	if( Calc(d, pCtrl->Text.c_str()) != TRUE ){
		d = Min;
	}
	if( d < Min ) d = Min;
	if( d > Max ) d = Max;
	if( Button == btNext ){
		d += DW;
	}
	else if( Button == btPrev ){
		d -= DW;
	}
	if( d < Min ) d = Min;
	if( d > Max ) d = Max;
	pCtrl->Text = StrDbl(d);
}

void CAidSpin::OnChanging(void)
{
	pSpin->Position = 50;
}
//---------------------------------------------------------------------------

