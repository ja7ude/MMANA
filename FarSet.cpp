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

#include "FarSet.h"
#include "Main.h"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
//TFarSetDlg *FarSetDlg;
//---------------------------------------------------------------------
__fastcall TFarSetDlg::TFarSetDlg(TComponent* AOwner)
	: TForm(AOwner)
{
}
//---------------------------------------------------------------------------
int __fastcall TFarSetDlg::Execute(void)
{
	if( exeenv.pFarFile == NULL ) return FALSE;
	char bf[128];
	EditName->Text = exeenv.pFarFile->Name;
	EditA->Text = StrDbl(exeenv.pFarFile->A);
	EditAW->Text = StrDbl(exeenv.pFarFile->AW);
	EditZ->Text = StrDbl(exeenv.pFarFile->Z);
	EditZW->Text = StrDbl(exeenv.pFarFile->ZW);
	sprintf(bf, "%d", exeenv.pFarFile->AN);
	EditAN->Text = bf;
	sprintf(bf, "%d", exeenv.pFarFile->ZN);
	EditZN->Text = bf;
	if( ShowModal() == IDOK ){
		strncpy(exeenv.pFarFile->Name, AnsiString(EditName->Text).c_str(), 256);
		if( Calc(exeenv.pFarFile->A, AnsiString(EditA->Text).c_str())==FALSE ) return FALSE;
		if( Calc(exeenv.pFarFile->Z, AnsiString(EditZ->Text).c_str())==FALSE ) return FALSE;
		if( Calc(exeenv.pFarFile->AW, AnsiString(EditAW->Text).c_str())==FALSE ) return FALSE;
		if( Calc(exeenv.pFarFile->ZW, AnsiString(EditZW->Text).c_str())==FALSE ) return FALSE;
		if( CalcU(exeenv.pFarFile->AN, AnsiString(EditAN->Text).c_str())==FALSE ) return FALSE;
		if( CalcU(exeenv.pFarFile->ZN, AnsiString(EditZN->Text).c_str())==FALSE ) return FALSE;
		return TRUE;
	}
	return FALSE;
}
//---------------------------------------------------------------------
void __fastcall TFarSetDlg::FileBtnClick(TObject *Sender)
{
	MainWnd->SaveDialog->Title = "遠距離データファイル";
	MainWnd->SaveDialog->Filter = "CSVファイル(*.csv)|*.csv|";
	MainWnd->SaveDialog->FileName = exeenv.pFarFile->Name;
	MainWnd->SaveDialog->DefaultExt = "csv";
	MainWnd->SaveDialog->InitialDir = ResDir;
	if( MainWnd->SaveDialog->Execute() == TRUE ){
		strncpy(exeenv.pFarFile->Name, AnsiString(MainWnd->SaveDialog->FileName).c_str(), 256);
		EditName->Text = exeenv.pFarFile->Name;
	}
}
//---------------------------------------------------------------------------
