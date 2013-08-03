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
#ifndef PrintH
#define PrintH
//---------------------------------------------------------------------------	// ja7ude 1.0
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
#include "ComLib.h"
#define	COLMAX	16				// 処理できる列の数
class TPrintDlgBox : public TForm
{
__published:	// IDE 管理のコンポーネント
	TPrintDialog *PrintDialog;
	TButton *AbortBtn;
	TLabel *Stat;
	
	void __fastcall AbortBtnClick(TObject *Sender);
private:	// ユーザー宣言
	int		mAbort;				// 印刷の中止

    int		mColCount;			// 印刷列の数
    int		mRowCount;			// 印刷行の数
	int		mMaxRow;			// 現在確保している行の数

    TRect	mMG;				// マージン情報（パーセント）
	TRect	mRC;				// 印刷範囲の座標情報（ピクセル）
	int		mPW;				// 印刷範囲の横幅（ピクセル）
    int		mPH;				// 印刷範囲の縦幅（ピクセル）
	double	mXW;
    double	mYW;
	TRect	mBC;				// 枠の描画範囲
	int		mPage;				// 現在のページ
    int		mPageMax;			// 全ページ数
    int		mPageLine;			// １ページあたりの行数
	int		mWidthP[COLMAX];	// 各列の横幅（比率係数）
    int		mWidth[COLMAX];		// 各列の横幅（ピクセル）
	int		mLeft[COLMAX];		// 各列の印刷位置
    LPCSTR	mTitle[COLMAX];		// タイトルのリスト
	LPCSTR	*mbp;				// 各項目リストのベースポインタ
   	SYSTEMTIME	mTime;			// 印刷開始時刻
	AnsiString	mDocTitle;		// ドキュメントタイトル

    TCanvas	*cp;				// 出力デバイス
private:
	LPSTR __fastcall StrDup(LPCSTR p);
	void __fastcall AllocRow(int row);
	void __fastcall Delete(void);
	int __fastcall GetCenter(int w, LPCSTR p);

	void __fastcall DispPageStat(void);
	int __fastcall InitSetup(TCanvas *p);
	void __fastcall DrawRect(void);
	void __fastcall DrawPage(void);

public:		// ユーザー宣言
	__fastcall TPrintDlgBox(TComponent* Owner);
	__fastcall ~TPrintDlgBox();

    int __fastcall Execute(void);
	inline __fastcall SetDocTitle(LPCSTR pTitle){
		mDocTitle = pTitle;
    };
	void __fastcall SetMargin(int left, int top, int right, int bottom);
	void __fastcall SetWidth(int col, int w);
	void __fastcall SetTitle(int col, LPCSTR p);
	void __fastcall SetString(int row, int col, LPCSTR p);
};
//---------------------------------------------------------------------------
//extern TPrintDlgBox *PrintDlgBox;
//---------------------------------------------------------------------------
#endif
