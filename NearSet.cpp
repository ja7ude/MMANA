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

#include "NearSet.h"
#include "Main.h"
//--------------------------------------------------------------------- 
#pragma resource "*.dfm"
//TNearSetDlg *NearSetDlg;
//---------------------------------------------------------------------
__fastcall TNearSetDlg::TNearSetDlg(TComponent* AOwner)
	: TForm(AOwner)
{
}
//---------------------------------------------------------------------------
int __fastcall TNearSetDlg::Execute(void)
{
	if( exeenv.pNearFile == NULL ) return FALSE;
	char bf[128];
	CalcSel->ItemIndex = exeenv.pNearFile->TYPE;
	EditName->Text = exeenv.pNearFile->Name;
	EditPow->Text = StrDbl(exeenv.pNearFile->POW);
	EditX->Text = StrDbl(exeenv.pNearFile->X);
	EditXW->Text = StrDbl(exeenv.pNearFile->XW);
	EditY->Text = StrDbl(exeenv.pNearFile->Y);
	EditYW->Text = StrDbl(exeenv.pNearFile->YW);
	EditZ->Text = StrDbl(exeenv.pNearFile->Z);
	EditZW->Text = StrDbl(exeenv.pNearFile->ZW);
	sprintf(bf, "%d", exeenv.pNearFile->XN);
	EditXN->Text = bf;
	sprintf(bf, "%d", exeenv.pNearFile->YN);
	EditYN->Text = bf;
	sprintf(bf, "%d", exeenv.pNearFile->ZN);
	EditZN->Text = bf;
	if( ShowModal() == IDOK ){
		exeenv.pNearFile->TYPE = CalcSel->ItemIndex;
		strncpy(exeenv.pNearFile->Name, AnsiString(EditName->Text).c_str(), 256);	//ja7ude 1.0
		if( Calc(exeenv.pNearFile->POW, AnsiString(EditPow->Text).c_str())==FALSE ) return FALSE;
		if( Calc(exeenv.pNearFile->X, AnsiString(EditX->Text).c_str())==FALSE ) return FALSE;
		if( Calc(exeenv.pNearFile->Y, AnsiString(EditY->Text).c_str())==FALSE ) return FALSE;
		if( Calc(exeenv.pNearFile->Z, AnsiString(EditZ->Text).c_str())==FALSE ) return FALSE;
		if( Calc(exeenv.pNearFile->XW, AnsiString(EditXW->Text).c_str())==FALSE ) return FALSE;
		if( Calc(exeenv.pNearFile->YW, AnsiString(EditYW->Text).c_str())==FALSE ) return FALSE;
		if( Calc(exeenv.pNearFile->ZW, AnsiString(EditZW->Text).c_str())==FALSE ) return FALSE;
		if( CalcU(exeenv.pNearFile->XN, AnsiString(EditXN->Text).c_str())==FALSE ) return FALSE;
		if( CalcU(exeenv.pNearFile->YN, AnsiString(EditYN->Text).c_str())==FALSE ) return FALSE;
		if( CalcU(exeenv.pNearFile->ZN, AnsiString(EditZN->Text).c_str())==FALSE ) return FALSE;
		return TRUE;
	}
	return FALSE;
}
//---------------------------------------------------------------------
void __fastcall TNearSetDlg::FileBtnClick(TObject *Sender)
{
	MainWnd->SaveDialog->Title = "近距離データファイル";
	MainWnd->SaveDialog->Filter = "CSVファイル(*.csv)|*.csv|";
	MainWnd->SaveDialog->FileName = exeenv.pNearFile->Name;
	MainWnd->SaveDialog->DefaultExt = "csv";
	MainWnd->SaveDialog->InitialDir = ResDir;
	if( MainWnd->SaveDialog->Execute() == TRUE ){
		strncpy(exeenv.pNearFile->Name, AnsiString(MainWnd->SaveDialog->FileName).c_str(), 256);
		EditName->Text = exeenv.pNearFile->Name;
	}
}
//---------------------------------------------------------------------------
