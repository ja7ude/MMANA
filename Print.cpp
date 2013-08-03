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

//---------------------------------------------------------------------------
#include <vcl.h>	// ja7ude 1.0
#include <Printers.hpp>
#pragma hdrstop

#include "Print.h"
//---------------------------------------------------------------------------
#pragma resource "*.dfm"
//TPrintDlgBox *PrintDlgBox;
//---------------------------------------------------------------------------
__fastcall TPrintDlgBox::TPrintDlgBox(TComponent* Owner)
	: TForm(Owner)
{
    mAbort = 0;
	mMaxRow = 0;
	mRowCount = 0;
    mColCount = 0;
    mbp = NULL;
}
//---------------------------------------------------------------------------
__fastcall TPrintDlgBox::~TPrintDlgBox()
{
	Delete();
}
//---------------------------------------------------------------------------
// 文字列の確保
LPSTR __fastcall TPrintDlgBox::StrDup(LPCSTR p)
{
	LPSTR t = new char[strlen(p)+1];
	if( t != NULL ) strcpy(t, p);
    return t;
}
//---------------------------------------------------------------------------
// すべての記憶アイテムを削除する
void __fastcall TPrintDlgBox::Delete(void)
{
	int i;

	for( i = 0; i < mColCount; i++ ){
		delete mTitle[i];
    }
    mColCount = 0;
	if( mbp != NULL ){
		for( i = 0; i < mRowCount*mColCount; i++ ){
			delete mbp[i];
        }
        mRowCount = mColCount = 0;
	    delete mbp;
	    mbp = NULL;
    }
}
//---------------------------------------------------------------------------
// １つの行を確保する
void __fastcall TPrintDlgBox::AllocRow(int row)
{
	if( (mbp==NULL)||(row >= mMaxRow) ){
		mMaxRow = mMaxRow ? mMaxRow * 2 : 32;
		LPCSTR	*np = new LPCSTR[mMaxRow*mColCount];
        memset(np, 0, mMaxRow*mColCount*sizeof(LPCSTR));
        if( mbp != NULL ){
			memcpy(np, mbp, mRowCount*mColCount*sizeof(LPCSTR));
            delete mbp;
        }
        mbp = np;
    }
	if( row <= mRowCount ) mRowCount = row + 1;
}
//---------------------------------------------------------------------------
// 列比率の設定
void __fastcall TPrintDlgBox::SetMargin(int left, int top, int right, int bottom)
{
	mMG.Left = left;
    mMG.Top = top;
    mMG.Right = right;
    mMG.Bottom = bottom;
}
//---------------------------------------------------------------------------
// 列比率の設定
void __fastcall TPrintDlgBox::SetWidth(int col, int w)
{
	if( col >= COLMAX ) return;
	if( col >= mColCount ) mColCount = col + 1;
	mWidthP[col] = w;
}
//---------------------------------------------------------------------------
// タイトルの設定
void __fastcall TPrintDlgBox::SetTitle(int col, LPCSTR p)
{
	if( col >= COLMAX ) return;
	if( col >= mColCount ) mColCount = col + 1;
	mTitle[col] = StrDup(p);
    if( mWidthP[col] < int(strlen(p)) ) mWidthP[col] = strlen(p);
}
//---------------------------------------------------------------------------
// アイテムの設定
void __fastcall TPrintDlgBox::SetString(int row, int col, LPCSTR p)
{
	if( col >= mColCount ) return;
	AllocRow(row);
	mbp[row * mColCount + col] = StrDup(p);
}
//---------------------------------------------------------------------------
// 文字列が中心になるＸ位置を得る
int __fastcall TPrintDlgBox::GetCenter(int w, LPCSTR p)
{
	int ww = cp->TextWidth(p);
    if( ww >= w ) return 0;
    return (w - ww)/2;
}
//---------------------------------------------------------------------------
// 印刷ページの表示
void __fastcall TPrintDlgBox::DispPageStat(void)
{
	char	bf[32];

	wsprintf(bf, "印刷中ページ : %u / %u", mPage + 1, mPageMax);
	Stat->Caption = bf;
}
//---------------------------------------------------------------------------
// 各種パラメータの調整
int __fastcall TPrintDlgBox::InitSetup(TCanvas *p)
{
	cp = p;
	mPW = mRC.Right - mRC.Left;
	mPH = mRC.Bottom - mRC.Top;
    mRC.Left = int(double(mPW * mMG.Left)/100.0);
    mRC.Top = int(double(mPH * mMG.Top)/100.0);
	mPW = int(mPW * double(100.0 - (mMG.Left + mMG.Right))/100.0);
	mPH = int(mPH * double(100.0 - (mMG.Top + mMG.Bottom))/100.0);

    // 横の印字位置と各アイテムの幅を計算する
	int	w;
    int i;
    for( w = i = 0; i < mColCount; i++ ){
		w += mWidthP[i];
    }
    w += mColCount + 1;
	mXW = double(mPW)/w;
	double x = mXW + mRC.Left;
	mBC.Left = int(x) - (mXW/2);
    for( i = 0; i < mColCount; i++ ){
		mLeft[i] = int(x);
		mWidth[i] = int(double(mWidthP[i])*mXW);
        x += double((mWidthP[i] + 1)*mXW);
    }
    mBC.Right = int(x) - (mXW/2);
	// 縦の情報を計算する
	mYW = cp->TextHeight("A")*mXW/cp->TextWidth("A");
	w = int(mPH / mYW);
	mYW = double(mPH/w);
    w -= 4;
    w /= 2;
    if( w <= 0 ) return FALSE;		// 印刷縦幅が小さすぎる
	mPageLine = w;
    mPageMax = (mRowCount + (mPageLine - 1))/mPageLine;
    mPage = 0;
    mBC.Top = (mYW + mYW/2) + mRC.Top;
	mBC.Bottom = mBC.Top + (mPageLine * 2 + 2) * mYW;
	::GetLocalTime(&mTime);
    return TRUE;
}

//---------------------------------------------------------------------------
// 枠の描画
void __fastcall TPrintDlgBox::DrawRect(void)
{
	// 横線の描画
	int i, y;
	for( y = 0; y <= mPageLine; y++ ){
		cp->MoveTo(mBC.Left, mBC.Top + (y * 2 * mYW));
		cp->LineTo(mBC.Right, mBC.Top + (y * 2 * mYW));
    }
	cp->MoveTo(mBC.Left, mBC.Bottom);
	cp->LineTo(mBC.Right, mBC.Bottom);
	// 縦線の描画
	for( i = 0; i < mColCount; i++ ){
		cp->MoveTo(mLeft[i] - mXW/2, mBC.Top);
		cp->LineTo(mLeft[i] - mXW/2, mBC.Bottom);
    }
	cp->MoveTo(mBC.Right, mBC.Top);
	cp->LineTo(mBC.Right, mBC.Bottom);
}

//---------------------------------------------------------------------------
// ページの描画
void __fastcall TPrintDlgBox::DrawPage(void)
{
	int i, j, w;
	char	bf[128];

	int TYW = int(mYW * 1.5);
	cp->Font->Height = int(TYW);
	// 日付の描画
	wsprintf(bf, "%4u年%2u月%02u日 %s", mTime.wYear, mTime.wMonth, mTime.wDay, mDocTitle.c_str());
    cp->TextOut(mBC.Left, mBC.Top - TYW, bf);
    // ページ番号の描画
    wsprintf(bf, "%d/%d", mPage+1, mPageMax);
	w = cp->TextWidth(bf);
    cp->TextOut(mBC.Right - w, mBC.Top - TYW, bf);
	// タイトルの描画
	cp->Font->Height = int(mYW);
	for( i = 0; i < mColCount; i++ ){
		cp->TextOut(mLeft[i]+GetCenter(mWidth[i], mTitle[i]), mBC.Top + (mYW/2), mTitle[i]);
    }
	// 各アイテムの描画
	int n = mPage * mPageLine;
    LPCSTR	sp;
	for( i = 0; i < mPageLine; i++, n++ ){
		if( n >= mRowCount ) break;
		for( j = 0; j < mColCount; j++ ){
			sp = mbp[(n * mColCount) + j];
			cp->Font->Height = int(mYW);
			w = cp->TextWidth(sp);
            if( w > mWidth[j] ){
				cp->Font->Height = int(mYW*mWidth[j]/w);
            }
			cp->TextOut(mLeft[j], mBC.Top + (i*2+3)*mYW-(mYW/2), sp);
        }
    }
}

//---------------------------------------------------------------------------
// 印刷の実行
int __fastcall TPrintDlgBox::Execute(void)
{
	if( (!mColCount) || (!mRowCount) ) return FALSE;
	int r = FALSE;
	if( PrintDialog->Execute() == TRUE ){
		TForm *pActive = Screen->ActiveForm;
        if( pActive != NULL ) pActive->Enabled = FALSE;
		mAbort = 0;
		CWaitCursor	tw;
        Printer()->Title = "MMANA";
		Printer()->BeginDoc();
		Printer()->Canvas->Font->Name = "ＭＳ ゴシック";
		mRC = Printer()->Canvas->ClipRect;
		if( InitSetup(Printer()->Canvas) == FALSE ){
			mAbort = 1;
        }
		tw.Delete();
		if( !mAbort ){
			Show();
			for( mPage = 0; mPage < mPageMax; mPage++ ){	// 印刷ループ
				if( mPage ) Printer()->NewPage();
				DispPageStat();
				Application->ProcessMessages();
				if( mAbort ) break;
				DrawRect();
				Application->ProcessMessages();
				if( mAbort ) break;
				DrawPage();
				Application->ProcessMessages();
				if( mAbort ) break;
	        }
		}
		tw.Wait();
        if( mAbort ){
			Printer()->Abort();
        }
        else {
			Printer()->EndDoc();
            r = TRUE;
        }
        if( pActive != NULL ) pActive->Enabled = TRUE;
		Visible = FALSE;
    }
    return r;
}
//---------------------------------------------------------------------------
// 中止ボタン
void __fastcall TPrintDlgBox::AbortBtnClick(TObject *Sender)
{
	mAbort = 1;
}
//---------------------------------------------------------------------------
