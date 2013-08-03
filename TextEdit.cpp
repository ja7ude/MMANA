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

#include "ComLib.h"
#include "TextEdit.h"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
TTextEditDlg *TextEditDlg;
//---------------------------------------------------------------------
__fastcall TTextEditDlg::TTextEditDlg(TComponent* AOwner)
	: TForm(AOwner)
{
	ap = NULL;
	Memo->Font->Pitch = fpFixed;       //ja7ude 1.0
	EntryAlignControl();
}
//---------------------------------------------------------------------
int __fastcall TTextEditDlg::Execute(AnsiString &as, int flag, LPCSTR pTitle /* = NULL*/)
{
	CWaitCursor tw;
	ap = as.c_str();
	if( flag != TRUE ){
		OKBtn->Visible = FALSE;
		CancelBtn->Caption = "閉じる";
		Memo->ReadOnly = TRUE;
	}
	if( pTitle != NULL ){
		Caption = pTitle;
	}
	tw.Delete();
	int r = ShowModal();
	if( r == IDOK ){
		if( Memo->Modified == FALSE ) return FALSE;
		if( flag == TRUE ){
			as = Memo->Text;
		}
		return TRUE;
	}
	return FALSE;
}

void __fastcall TTextEditDlg::FontBtnClick(TObject *Sender)
{
	FontDialog->Font->Assign(Memo->Font);
	if( FontDialog->Execute() == TRUE ){
		CWaitCursor tw;
		Memo->Font->Assign(FontDialog->Font);
		Memo->Update();
	}
}
//---------------------------------------------------------------------------
// サイズ変更コントロールの登録
void __fastcall TTextEditDlg::EntryAlignControl(void)
{
	RECT	rc;
	rc.left = 0;
	rc.top = 0;
	rc.right = 799;
	rc.bottom = 599;

	AlignList.EntryControl(OKBtn, &rc, OKBtn->Font);
	AlignList.EntryControl(CancelBtn, &rc, CancelBtn->Font);
	AlignList.EntryControl(FontBtn, &rc, FontBtn->Font);
	AlignList.EntryControl(Memo, &rc, Memo->Font);

	int CX = ::GetSystemMetrics(SM_CXFULLSCREEN);
	int CY = ::GetSystemMetrics(SM_CYFULLSCREEN);
//	int CX = ::GetSystemMetrics(SM_CXSCREEN);
//	int CY = ::GetSystemMetrics(SM_CYSCREEN);
	if( (CX < Width)||(CY < Height) ){
		Top = 0;
		Left = 0;
		Width = CX;
		Height = CY;
	}
	FormCenter(this, CX, CY);
	if( Owner != NULL ){
		WindowState = ((TForm *)Owner)->WindowState;
	}
}
//---------------------------------------------------------------------------
void __fastcall TTextEditDlg::FormResize(TObject *Sender)
{
	CWaitCursor tw;
	AlignList.NewAlign(this);
}
//---------------------------------------------------------------------------
void __fastcall TTextEditDlg::FormPaint(TObject *Sender)
{
	if( ap == NULL ) return;
	CWaitCursor tw;
	Memo->Text = ap;
	ap = NULL;
	Memo->Update();
}
//---------------------------------------------------------------------------
