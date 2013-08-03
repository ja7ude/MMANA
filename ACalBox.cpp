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

#include "ACalBox.h"
#include "GrpWire.h"
#include "ACalMult.h"
#include "ACalInfo.h"
#include "ACalEle.h"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
//TACalDlg *ACalDlg;
//---------------------------------------------------------------------
__fastcall TACalDlg::TACalDlg(TComponent* AOwner)
	: TForm(AOwner)
{
	EntryAlignControl();
}
//---------------------------------------------------------------------------
int __fastcall TACalDlg::Execute(CACal *p)
{
	pACal = p;
	Grid->RowCount = pACal->amax + 2;
	TBGain->Position = int(pACal->atrg.TG);
	TBFB->Position = int(pACal->atrg.TFB);
	TBJX->Position = int(pACal->atrg.TJX);
	TBSWR->Position = int(pACal->atrg.TSWR);
	TBELV->Position = int(pACal->atrg.TELV);
	TBMCH->Position = int(pACal->atrg.TM);
	TBCUR->Position = int(pACal->atrg.TI);
	DispRes->Checked = exeenv.CalcDisp;
	PitchAbs->Checked = pACal->PitchUnit;
	DegUnit->Checked = pACal->DegUnit;
	TerDis->Checked = !pACal->atrg.TF;
	UpdateView();
	if( p->AP->cmax > 4 ) p->atrg.bmax = 1;
	int r = ShowModal();
	pACal->PitchUnit = PitchAbs->Checked;
	exeenv.CalcDisp = DispRes->Checked;
	pACal->DegUnit = DegUnit->Checked;
	pACal->atrg.TF = !TerDis->Checked;
	pACal->SetTradeOff(TBGain->Position, TBFB->Position, TBJX->Position, TBSWR->Position, TBELV->Position, TBMCH->Position, TBCUR->Position);
	if( r == IDOK ){
		return TRUE;
	}
	else {
		return FALSE;
	}
}
//---------------------------------------------------------------------------
void __fastcall TACalDlg::UpdateView(void)
{
	int f = pACal->amax ? TRUE : FALSE;
	OKBtn->Enabled = f;
	K23->Enabled = f;
	if(Grid->Row > pACal->amax) f = FALSE;
	DelBtn->Enabled = f;
	BandBtn->Enabled = (!TerDis->Checked) && (pACal->AP->cmax <= 4);
	K32->Enabled = (pACal->AP->StackH != 1)||(pACal->AP->StackV != 1);
	Grid->RowCount = pACal->amax + 2;
	Grid->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TACalDlg::PopupPopup(TObject *Sender)
{
	int f = pACal->amax ? TRUE : FALSE;
	K27->Enabled = f;	// 全削除
	K34->Enabled = f;	// エレメントの連携
	if(Grid->Row > (pACal->amax)) f = FALSE;
	K10->Enabled = f;	// 削除
	if( pACal->amax >= AVMAX ) f = FALSE;
	K28->Enabled = f;	// 挿入

	f = FALSE;
	if( (Grid->Row) && (Grid->Row <= pACal->amax) ){
		int n = Grid->Row - 1;
		if( (pACal->aval[n].Type == avWIRE) || (pACal->aval[n].Type == avLOAD) ){
			f = TRUE;
		}
	}
	K11->Enabled = f;		// ポップアップのワイヤ連携一括
}
//---------------------------------------------------------------------
void __fastcall TACalDlg::GridDrawCell(TObject *Sender, int Col, int Row,
	TRect &Rect, TGridDrawState State)
{
	char	bf[64];

	Grid->Canvas->FillRect(Rect);
	int X = Rect.Left + 1;
	int Y = Rect.Top + 1;

	if( Row ){
		GridGetText(bf, Col, Row);
		Grid->Canvas->TextOut(X, Y, bf);
	}
	else {		// タイトル
		LPCSTR	_tt[]={
			"No.","種類","位置","箇所","連携","ﾋﾟｯﾁ(%)","Min","Max","現在値"
		};
		X += 8;
		_tt[5] = (PitchAbs->Checked) ? "ﾋﾟｯﾁ":"ﾋﾟｯﾁ(%)";
		Grid->Canvas->TextOut(X, Y, _tt[Col]);
	}
}
//---------------------------------------------------------------------------
// セル位置のテキストを返す
void __fastcall TACalDlg::GridGetText(LPSTR t, long Col, long Row)
{
	LPCSTR	_tt1[]={
		"ワイヤ","ｴﾚﾒﾝﾄ","集中定数","地上高","周波数","極ﾜｲﾔ","給電点","スタック",
	};
	LPCSTR	_tt2[]={
		"Ｙ１","Ｚ１","Ｙ２","Ｚ２","Ｒ","Ｘ１","Ｘ２"
	};
	LPCSTR	_tt3[]={
		"間隔","横幅","縦幅","全周","Ｒ", "位置","横幅(全)","縦幅(全)","全周(全)"
	};
	LPCSTR	_tt4[]={
		"長さ(始)","長さ(終)","長さ(中)",
		"径度(始)","径度(終)","径度(中)",
		"天頂(始)","天頂(終)","天頂(中)",
	};
	LPCSTR	_tt5[]={
		"間隔","水平間隔","垂直間隔",
	};
	*t = 0;
	if( Row ){
		Row--;
		AVAL	*ap = &pACal->aval[Row];
		switch(Col){
			case 0:
				if( Row < pACal->amax ){
					sprintf( t, "%2d", Row+1);
				}
				else {
					strcpy(t, "新規");
				}
				break;
			case 1:		// 種類
				if( Row < pACal->amax ){
					strcpy(t, _tt1[ap->Type]);
				}
				break;
			case 2:		// 位置
				if( Row < pACal->amax ){
					if( (ap->Type != avHEIGHT) && (ap->Type != avFREQ) && (ap->Type != avSTACK) ){
						sprintf(t, "%u", ap->Pos + 1);
					}
				}
				break;
			case 3:		// サブ位置
				if( Row < pACal->amax ){
					switch(ap->Type){
						case avWIRE:
							strcpy(t, _tt2[ap->SubPos] );
							break;
						case avELE:
							strcpy(t, _tt3[ap->SubPos] );
							if( (ap->SubPos==3)||(ap->SubPos==8) ){
								if( pACal->WList.List[ap->Pos].EnbXw() ){
									strcpy(t, ap->SubPos==3 ? "Ｘ幅":"Ｘ幅(全)" );
								}
							}
							break;
						case avLOAD:
							switch(pACal->AP->ldef[ap->Pos].TYPE){
								case 0:
									strcpy(t, ap->SubPos ? "C" : "L");
									break;
								case 1:
									strcpy(t, ap->SubPos ? "jX" : "R");
									break;
								case 2:
									strcpy(t, "Error");
									break;
							}
							break;
						case avWIREP:
							strcpy(t, _tt4[ap->SubPos] );
							break;
						case avFEED:
							strcpy(t, ap->SubPos ? "電圧":"位相");
							break;
						case avSTACK:
							strcpy(t, _tt5[ap->SubPos] );
							break;
						default:
							break;
					}
				}
				break;
			case 4:		// 連携
				if( Row < pACal->amax ){
					GetWithStr(t, ap);
				}
				break;
			case 5:		// 可変ピッチ
				if( Row < pACal->amax ){
					if( ap->With == -1 ){
						strcpy(t, StrDbl(ap->DW));
					}
				}
				break;
			case 6:		// Min
				if( Row < pACal->amax ){
					if( ap->With == -1 ){
						if( ap->Min & 0x8000 ){
							sprintf(t, "#%u", (ap->Min & 0x7fff) + 1);
						}
						else {
							strcpy(t, StrDbl(ap->DMIN));
						}
					}
				}
				break;
			case 7:		// Max
				if( Row < pACal->amax ){
					if( ap->With == -1 ){
						if( ap->Max & 0x8000 ){
							sprintf(t, "#%u", (ap->Max & 0x7fff) + 1);
						}
						else {
							strcpy(t, StrDbl(ap->DMAX));
						}
					}
				}
				break;
			case 8:		// 現在値
				if( Row < pACal->amax ){
					strcpy(t, StrDbl(pACal->GetNowValue(Row)));
					if( ap->With != -1 ){
						int n = pACal->GetLastChen(0, ap->With);
						if( n != -1 ){
							double d = pACal->GetNowValue(n, ap->DW);
							if( RoundUpStr(d) != RoundUpStr(pACal->GetNowValue(Row)) ){
								strcat(t, "/");
								strcat(t, StrDbl(d));
							}
						}
					}
				}
				break;
		}
	}
}
//---------------------------------------------------------------------------
// エレメント参照
void __fastcall TACalDlg::RefEleClick(TObject *Sender)
{
	if( Grid->Row ){
		int Row = Grid->Row - 1;
		if( Row >= AVMAX ) return;
		AVAL *ap = &pACal->aval[Row];
		TGrpWireDlg *Box = new TGrpWireDlg(this);

		int Pos, SubPos;
		Pos = -1;
		if( Row < pACal->amax ){
			Pos = ap->Pos;
			SubPos = ap->SubPos;
			if( (ap->Type == avWIRE) || (ap->Type == avWIREP) ){
				SubPos = -1;
			}
		}
		if( Box->Select(Pos, SubPos, pACal->AP, pACal) == TRUE ){
			if( (SubPos >= 0) || (Row >= pACal->amax) || (ap->Pos != Pos) || ((ap->Type!=avWIRE)&&(ap->Type!=avWIREP)) ){
				GridNewLine(Row);
				if( SubPos < 0 ){
					ap->Pos = Pos;
					if( (ap->Type != avWIRE)&&(ap->Type != avWIREP) ){
						ap->Type = avWIRE;
						ap->SubPos = 0;
					}
				}
				else {
					ap->Type = avELE;
					ap->Pos = Pos;
					ap->SubPos = SubPos;
				}
				pACal->AdjSetup(Row);
				Grid->Invalidate();
				Grid->Row = Grid->Row + 1;
			}
		}
		delete Box;
	}
}
//---------------------------------------------------------------------------
// 新規バリアブルの作成
void __fastcall TACalDlg::GridNewLine(int Row)
{
	if( (Row >= pACal->amax) && (Row < AVMAX) ){
		AVAL *ap = &pACal->aval[pACal->amax];
		pACal->amax++;
		ap->With = -1;
		UpdateView();
	}
}
//---------------------------------------------------------------------------
void __fastcall TACalDlg::GridGetEditText(TObject *Sender, int ACol, int ARow,
	UnicodeString &Value)
{
	if( (ARow-1) >= AVMAX ){
		Grid->EditorMode = FALSE;
		return;
	}
	int X = Left + (Width - ClientWidth)/2 + Grid->Left;
	int Y = Top + (Height - ClientHeight)/2 + Grid->Top;
	X += Grid->ColWidths[0] + Grid->ColWidths[1];
	Y += ((Grid->Row - Grid->TopRow) * Grid->RowHeights[0]);
	if( ACol == 1 ){
		Grid->EditorMode = FALSE;
		PopupType->Popup(X, Y);
	}
	else if( ACol == 3 ){
		switch(pACal->aval[ARow-1].Type){
			case avWIRE:
				Grid->EditorMode = FALSE;
				PopupWire->Popup(X, Y);
				break;
			case avELE:
				Grid->EditorMode = FALSE;
				PopupEle->Popup(X, Y);
				break;
			case avLOAD:
				Grid->EditorMode = FALSE;
				PopupLoad->Popup(X, Y);
				break;
			case avWIREP:
				Grid->EditorMode = FALSE;
				PopupWireP->Popup(X, Y);
				break;
			case avFEED:
				Grid->EditorMode = FALSE;
				PopupFeed->Popup(X, Y);
				break;
			case avSTACK:
				Grid->EditorMode = FALSE;
				PopupStack->Popup(X, Y);
				break;
		}
	}
	else {
		char	bf[64];

		GridGetText(bf, ACol, ARow);
		Value = bf;
	}
}
//---------------------------------------------------------------------------
// 連携式の作成
void __fastcall TACalDlg::GetWithStr(LPSTR t, AVAL *ap)
{
	if( ap->With == -1 ){
		strcpy(t, "0");
	}
	else if( ap->With < 0 ){
		sprintf(t, "%d", ap->With + 1);
	}
	else if( ap->With & 0x00010000 ){
		sprintf(t, "%d%c%s", (ap->With&0x0000ffff) + 1, char((ap->DW >= 0) ? '+':'-'), StrDbl(ABS(ap->DW)));
	}
	else if( ap->With & 0x00020000 ){
		sprintf(t, "-%d%c%s", (ap->With&0x0000ffff) + 1, char((ap->DW >= 0) ? '+':'-'), StrDbl(ABS(ap->DW)));
	}
	else if( ap->With & 0x00040000 ){
		sprintf(t, "%d*%s", (ap->With&0x0000ffff) + 1, StrDbl(ap->DW));
	}
	else if( ap->With & 0x00080000 ){
		sprintf(t, "%d/%s", (ap->With&0x0000ffff) + 1, StrDbl(ap->DW));
	}
	else {
		sprintf(t, "%d", ap->With + 1);
	}
}
//---------------------------------------------------------------------------
// 連携式の解読
int __fastcall TACalDlg::SetWithValue(int ARow, LPSTR s, AVAL *ap)
{
	LPSTR	p;
	char	c = 0;
	for( p = s+1; *p; p++ ){
		if( (*p=='+')||(*p=='-')||(*p=='*')||(*p=='/') ){
			if( *(p+1) ){
				c = *p;
				*p = 0;
				p++;
			}
			else {
				*p = 0;
			}
			break;
		}
	}
	int	di;
	double dd;
	if( CalcI(di, s) != TRUE) return FALSE;
	if( di == 0 ){		// 連携なし
		GridNewLine(ARow);
		ap->With = -1;
	}
	else if( c ){		// 式が存在する場合
		if( Calc(dd, p) == FALSE ) dd = 0.0;
		switch(c){
			case '-':
				dd = -dd;
			case '+':
				if( di < 0 ){
					di = -di;
					if( di > AVMAX ) return FALSE;
					GridNewLine(ARow);
					ap->With = (di - 1) | 0x00020000;
				}
				else {
					if( di > AVMAX ) return FALSE;
					GridNewLine(ARow);
					ap->With = (di - 1) | 0x00010000;
				}
				if( *p == '=' ){
					dd = pACal->GetNowValue(ARow) - pACal->GetNowValue(ap->With, 0);
				}
				break;
			case '*':
				if( di < 0 ){
					dd = -dd;
					di = -di;
				}
				if( di > AVMAX ) return FALSE;
				GridNewLine(ARow);
				ap->With = (di - 1) | 0x00040000;
				if( (*p == '=') && pACal->GetNowValue(ap->With, 1.0) ){
					dd = pACal->GetNowValue(ARow)/pACal->GetNowValue(ap->With, 1.0);
				}
				if( !dd ) dd = 1.0;
				break;
			case '/':
				if( di < 0 ){
					dd = -dd;
					di = -di;
				}
				if( di > AVMAX ) return FALSE;
				GridNewLine(ARow);
				ap->With = (di - 1) | 0x00080000;
				if( (*p == '=') && pACal->GetNowValue(ARow) ){
					dd = pACal->GetNowValue(ap->With, 1.0)/pACal->GetNowValue(ARow);
				}
				if( !dd ) dd = 1.0;
				break;
		}
		ap->DW = dd;
		if( (c=='+')||(c=='-') ){
			if( ap->DW == 0.0 ){
				ap->With &= 0x0000ffff;
			}
		}
		else if( (c=='*')||(c=='/') ){
			if( ap->DW == 1.0 ){
				ap->With &= 0x0000ffff;
			}
			else if( ap->DW == -1.0 ){
				ap->With &= 0x0000ffff;
				ap->With = -ap->With - 2;
			}
		}
	}
	else {				// 単純連携
		GridNewLine(ARow);
		ap->With = di - 1;
	}
	if( ap->With == -1 ){
		pACal->AdjSetup(ARow);
	}
	return TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TACalDlg::GridSetEditText(TObject *Sender, int ACol, int ARow,
	const UnicodeString Value)
{
	char	bf[128];
	double	d;
	int		di, max;

	if( Grid->EditorMode == TRUE ) return;
	if( ARow ){
		ARow--;
		if( ARow >= AVMAX ) return;
		AVAL *ap = &pACal->aval[ARow];
		switch(ACol){
			case 1:		// 種類
				break;
			case 2:		// 位置
				if( CalcU(di, AnsiString(Value).c_str()) == TRUE){
					max = 0;
					switch(ap->Type){
						case avWIRE:
						case avWIREP:
							max = pACal->WList.wmax;
							break;
						case avELE:
							max = pACal->WList.gmax;
							break;
						case avLOAD:
							max = pACal->AP->lmax;
							break;
						case avFEED:
							max = pACal->AP->cmax;
							break;
					}
					if( (di >= 1) && (di <= max) ){
						GridNewLine(ARow);
						ap->Pos = di - 1;
						pACal->AdjSetup(ARow);
					}
				}
				break;
			case 3:		// サブ位置
				break;
			case 4:		// 連携
				ZSCopy(bf, AnsiString(Value).c_str(), sizeof(bf)-1);
				SetWithValue(ARow, bf, ap);
				break;
			case 5:		// 可変幅ピッチ
				if( Calc(d, AnsiString(Value).c_str()) == TRUE){
					if( ap->With == -1 ){
						GridNewLine(ARow);
						ap->DW = d;
					}
				}
				break;
			case 6:		// Min
				if( ap->With == -1 ){
					ZSCopy(bf, AnsiString(Value).c_str(), sizeof(bf)-1);
					if( bf[0] == '#' ){
						CalcI(di, &bf[1]);
						if( di ) di--;
						GridNewLine(ARow);
						ap->Min = USHORT(di | 0x8000);
					}
					else if( Calc(d, bf) == TRUE){
						GridNewLine(ARow);
						ap->Min = 0;
						ap->DMIN = d;
					}
				}
				break;
			case 7:		// Max
				if( ap->With == -1 ){
					ZSCopy(bf, AnsiString(Value).c_str(), sizeof(bf)-1);
					if( bf[0] == '#' ){
						CalcI(di, &bf[1]);
						if( di ) di--;
						GridNewLine(ARow);
						ap->Max = USHORT(di | 0x8000);
					}
					else if( Calc(d, bf) == TRUE){
						GridNewLine(ARow);
						ap->Max = 0;
						ap->DMAX = d;
					}
				}
				break;
			case 8:
				if( Calc(d, AnsiString(Value).c_str()) == TRUE){
					if( ap->With == -1 ){
						if( ARow < pACal->amax ){
							if( YesNoMB("ほんまに変更しますか？") == IDYES ){
								pACal->SetPara(ARow, d);
							}
						}
					}
				}
				break;
			default:
				break;
		}
	}
	Grid->Invalidate();
}
//---------------------------------------------------------------------------
// 箇所の設定
void __fastcall TACalDlg::SetType(int n)
{
	if( Grid->Row ){
		int Row = Grid->Row - 1;
		GridNewLine(Row);
		pACal->aval[Row].Type = n;
		pACal->AdjSetup(Row);
		Grid->Invalidate();
	}
}
//---------------------------------------------------------------------------
// ワイヤを選択
void __fastcall TACalDlg::K1Click(TObject *Sender)
{
	SetType(avWIRE);
}
//---------------------------------------------------------------------------
// 極座標ワイヤを選択
void __fastcall TACalDlg::K12Click(TObject *Sender)
{
	SetType(avWIREP);
}
//---------------------------------------------------------------------------
// エレメントを選択
void __fastcall TACalDlg::K2Click(TObject *Sender)
{
	SetType(avELE);
}
//---------------------------------------------------------------------------
// 集中定数を選択
void __fastcall TACalDlg::K3Click(TObject *Sender)
{
	SetType(avLOAD);
}
//---------------------------------------------------------------------------
// 地上高さを選択
void __fastcall TACalDlg::K4Click(TObject *Sender)
{
	SetType(avHEIGHT);
}
//---------------------------------------------------------------------------
// 周波数を選択
void __fastcall TACalDlg::K9Click(TObject *Sender)
{
	SetType(avFREQ);
}
//---------------------------------------------------------------------------
void __fastcall TACalDlg::K24Click(TObject *Sender)
{
	SetType(avFEED);
}
//---------------------------------------------------------------------------
void __fastcall TACalDlg::K32Click(TObject *Sender)
{
	SetType(avSTACK);
}
//---------------------------------------------------------------------------
// 箇所の設定
void __fastcall TACalDlg::SetSubPos(int n)
{
	if( Grid->Row ){
		int Row = Grid->Row - 1;
		GridNewLine(Row);
		pACal->aval[Row].SubPos = n;
		pACal->AdjSetup(Row);
		Grid->EditorMode = FALSE;
		Grid->Invalidate();
	}
}
//---------------------------------------------------------------------------
void __fastcall TACalDlg::Y11Click(TObject *Sender)
{
	SetSubPos(0);
}
//---------------------------------------------------------------------------
void __fastcall TACalDlg::Z11Click(TObject *Sender)
{
	SetSubPos(1);
}
//---------------------------------------------------------------------------
void __fastcall TACalDlg::Y21Click(TObject *Sender)
{
	SetSubPos(2);
}
//---------------------------------------------------------------------------
void __fastcall TACalDlg::Z21Click(TObject *Sender)
{
	SetSubPos(3);
}
//---------------------------------------------------------------------------
void __fastcall TACalDlg::K8Click(TObject *Sender)
{
	SetSubPos(4);
}
//---------------------------------------------------------------------------
void __fastcall TACalDlg::K18Click(TObject *Sender)
{
	SetSubPos(5);
}
//---------------------------------------------------------------------------
void __fastcall TACalDlg::K19Click(TObject *Sender)
{
	SetSubPos(6);
}
//---------------------------------------------------------------------------
void __fastcall TACalDlg::K20Click(TObject *Sender)
{
	SetSubPos(7);
}
//---------------------------------------------------------------------------
void __fastcall TACalDlg::K21Click(TObject *Sender)
{
	SetSubPos(8);
}
//---------------------------------------------------------------------------
// この行に挿入
void __fastcall TACalDlg::K28Click(TObject *Sender)
{
	if( pACal->amax >= AVMAX ) return;
	int	Row, i;
	if( Grid->Row && (Grid->Row <= pACal->amax) ){
		Row = Grid->Row - 1;
		AVAL *ap;
		for( ap = pACal->aval, i = 0; i < pACal->amax; i++, ap++ ){
			if( ap->With != -1 ){
				if( pACal->GetChenNo(ap->With) >= Row ){
					if( ap->With < 0 ){
						ap->With--;
					}
					else {
						ap->With++;
					}
				}
			}
		}
		for( i = pACal->amax; i > Row; i-- ){
			memcpy(&pACal->aval[i], &pACal->aval[i-1], sizeof(AVAL));
		}
		pACal->amax++;
		UpdateView();
	}
}
//---------------------------------------------------------------------------
// 削除ボタン
void __fastcall TACalDlg::DelBtnClick(TObject *Sender)
{
	int	Row, i;
	if( Grid->Row && (Grid->Row <= pACal->amax) ){
		Row = Grid->Row - 1;
		AVAL *ap;
		for( ap = pACal->aval, i = 0; i < pACal->amax; i++, ap++ ){
			if( ap->With != -1 ){
				if( pACal->GetChenNo(ap->With) == Row ){
					ap->With = -1;
				}
				else if( pACal->GetChenNo(ap->With) > Row ){
					if( ap->With < 0 ){
						ap->With++;
					}
					else {
						ap->With--;
					}
				}
			}
		}
		for( i = Row; i < (pACal->amax - 1); i++ ){
			memcpy(&pACal->aval[i], &pACal->aval[i+1], sizeof(AVAL));
		}
		memset(&pACal->aval[i], 0, sizeof(AVAL));
		pACal->amax--;
		UpdateView();
	}
}
//---------------------------------------------------------------------------
// 全エレメント参照
static void AddAVAL(CACal *cp, int pos, int sub, double d, double dm)
{
	if( cp->amax >= AVMAX ) return;
	AVAL *ap = &cp->aval[cp->amax];
	ap->Type = avELE;
	ap->With = -1;
	ap->Pos = pos;
	ap->SubPos = sub;
	if( cp->PitchUnit ){
		ap->DW = GetNormal(d / dm);
	}
	else {
		ap->DW = (100.0 / dm);
	}
	if( !ap->DW ) ap->DW = 0.001;
	ap->DMAX = 2000.0;
	ap->DMIN = 0.0;
	cp->amax++;
}

void __fastcall TACalDlg::SetEleWith(int n, int w, double a, double b)
{
	if( n < 0 ) return;
	if( w < 0 ) return;
	AVAL *ap = &pACal->aval[n];
	if( RoundUpStr(a) == RoundUpStr(b) ){
		ap->With = w;
	}
	else {
		ap->With = w | 0x00040000;
		ap->DW = a / b;
	}
}

void __fastcall TACalDlg::AddEle(int n)
{
	if( pACal->WList.List[n].EnbXw() ){
		double h, v, x;
		h = pACal->WList.List[n].GetWidth();
		v = pACal->WList.List[n].GetHeight();
		x = pACal->WList.List[n].GetRound();
		if( WHFlag == 1 ){
			WHHW = h ? 1 : 0;
			WHVW = v ? 1 : 0;
			WHXW = x ? 1 : 0;
			if( (WHHW && WHVW)||(WHHW && WHXW)||(WHVW && WHXW) ){
				TACalEleBox *Box = new TACalEleBox(this);
				Box->Execute(WHHW, WHVW, WHXW, WHHC, WHVC, WHXC);
				delete Box;
				WHFlag = 0;
			}
		}
		int hp = -1;
		int vp = -1;
		int xp = -1;
		if( h && WHHW ){
			hp = pACal->amax;
			AddAVAL(pACal, n, exeenv.EleWidthAll ? 6 : 1, h, 500);
		}
		if( v && WHVW ){
			vp = pACal->amax;
			AddAVAL(pACal, n, exeenv.EleWidthAll ? 7 : 2, v, 500);
		}
		if( x && WHXW ){
			xp = pACal->amax;
			AddAVAL(pACal, n, exeenv.EleWidthAll ? 8 : 3, x, 500);
		}
		if( (WHHC==1)&&(vp!=-1) ){	// 横幅を縦幅に連携
			SetEleWith(hp, vp, h, v);
		}
		if( (WHHC==2)&&(xp!=-1) ){	// 横幅をＸ幅に連携
			SetEleWith(hp, xp, h, x);
		}
		if( (WHVC==1)&&(hp!=-1) ){	// 縦幅を横幅に連携
			SetEleWith(vp, hp, v, h);
		}
		if( (WHVC==2)&&(xp!=-1) ){	// 縦幅をＸ幅に連携
			SetEleWith(vp, xp, v, x);
		}
		if( (WHXC==1)&&(hp!=-1) ){	// Ｘ幅を横幅に連携
			SetEleWith(xp, hp, x, h);
		}
		if( (WHXC==2)&&(vp!=-1) ){	// Ｘ幅を縦幅に連携
			SetEleWith(xp, vp, x, v);
		}
	}
	else if( pACal->WList.List[n].GetRound() ){
		AddAVAL(pACal, n, exeenv.EleWidthAll ? 8 : 3, pACal->WList.List[n].GetRound(), 500);
	}
}
//--------------------------------------------------------------------
void __fastcall TACalDlg::AdjWithEle(void)
{
	int i, j;
	double x, d;
	AVAL *ap = pACal->aval;
	for( i = 0; i < pACal->amax; i++, ap++ ){
		if( (ap->Type == avELE)&&(ap->With == -1) ){
			x = RoundUpStr(pACal->WList.GetXPosition(ap->Pos));
			d = RoundUpStr(pACal->GetNowValue(i));
			if( ((ap->SubPos>=1)&&(ap->SubPos<=3)) ||
            	((ap->SubPos>=6)&&(ap->SubPos<=8)) ||
            	(ap->SubPos == 5) ){		// 位置・幅・全周の連携
				AVAL *rp = ap + 1;
				for( j = i + 1; j < pACal->amax; j++, rp++ ){
					if( (rp->Type == avELE) && (rp->SubPos == ap->SubPos) ){
						if( x == RoundUpStr(pACal->WList.GetXPosition(rp->Pos)) ){
							if( d == RoundUpStr(pACal->GetNowValue(j)) ){
								rp->With = i;
							}
						}
					}
				}
			}
		}
	}
}

void __fastcall TACalDlg::AllEleClick(TObject *Sender)
{
	int i, BaseW;

	WHFlag = 1;
	WHHW = WHVW = WHXW = 1;
	WHHC = WHVC = WHXC = 0;
	if( pACal->amax && (YesNoMB("現在の設定を失います. よろしいですか？") != IDYES) ) return;

	for( BaseW = pACal->WList.BaseW; (BaseW < pACal->WList.gmax) && (!pACal->WList.Intval[BaseW]); BaseW++ );
	if( pACal->WList.Intval[BaseW] || (BaseW == pACal->WList.gmax) ) BaseW--;
	pACal->amax = 0;
	for( i = BaseW; i >= pACal->WList.BaseW; i-- ){		// Raのエレメント
		AddEle(i);
	}
	for( ; i >= 0; i-- ){								// Rfのエレメントと間隔
		AddEle(i);
		if( exeenv.IntPos ){
			if( pACal->WList.Intval[i] ){
				AddAVAL(pACal, i, 0, pACal->WList.Intval[i], 100);
			}
		}
		else {
			if( i && pACal->WList.GetXPosition(i) ){
				AddAVAL(pACal, i, 5, pACal->WList.GetXPosition(i), 500);
			}
		}
	}
	if( !exeenv.IntPos && BaseW < (pACal->WList.gmax - 1) ){
		for( i = BaseW; i >= pACal->WList.BaseW; i-- ){		// Raのエレメント
			if( pACal->WList.GetXPosition(i) ){
				AddAVAL(pACal, i, 5, pACal->WList.GetXPosition(i), 500);
			}
		}
	}
	double LastX = pACal->WList.GetXPosition(pACal->WList.gmax-1);
	for( i = BaseW + 1; i < pACal->WList.gmax; i++ ){  // 先端へのエレメントと間隔
		AddEle(i);
		if( exeenv.IntPos ){
			if( pACal->WList.Intval[i] ){
				AddAVAL(pACal, i, 0, pACal->WList.Intval[i], 100);
			}
		}
		else {
			if( i < (pACal->WList.gmax - 1) ){
				if( pACal->WList.GetXPosition(i) != LastX ){
					AddAVAL(pACal, i, 5, pACal->WList.GetXPosition(i), 500);
				}
			}
		}
	}
	AdjWithEle();
	UpdateView();
}
//---------------------------------------------------------------------------
void __fastcall TACalDlg::PitchAbsClick(TObject *Sender)
{
	pACal->PitchUnit = PitchAbs->Checked;
}
//---------------------------------------------------------------------------
// 実行ボタン
void __fastcall TACalDlg::OKBtnClick(TObject *Sender)
{
	if( pACal->Check() == FALSE ) return;
	if( pACal->InitPara() == FALSE ) return;
	ModalResult = IDOK;
}
//---------------------------------------------------------------------------
// サイズ変更コントロールの登録
void __fastcall TACalDlg::EntryAlignControl(void)
{
	RECT	rc;
	rc.left = 0;
	rc.top = 0;
	rc.right = 719;
	rc.bottom = 519;

	AlignList.EntryControl(OKBtn, &rc, OKBtn->Font);
	AlignList.EntryControl(CancelBtn, &rc, CancelBtn->Font);
	AlignList.EntryControl(GroupBox1, &rc, GroupBox1->Font);
	AlignList.EntryControl(TBGain, &rc, NULL);
	AlignList.EntryControl(TBFB, &rc, NULL);
	AlignList.EntryControl(TBJX, &rc, NULL);
	AlignList.EntryControl(Label1, &rc, Label1->Font);
	AlignList.EntryControl(Label2, &rc, Label2->Font);
	AlignList.EntryControl(Label3, &rc, Label3->Font);
	AlignList.EntryControl(Grid, &rc, Grid->Font);
	AlignList.EntryControl(Label4, &rc, Label4->Font);
	AlignList.EntryControl(RefEle, &rc, RefEle->Font);
	AlignList.EntryControl(DelBtn, &rc, DelBtn->Font);
	AlignList.EntryControl(DispRes, &rc, DispRes->Font);
	AlignList.EntryControl(TBSWR, &rc, NULL);
	AlignList.EntryControl(Label5, &rc, Label5->Font);
	AlignList.EntryControl(TBELV, &rc, NULL);
	AlignList.EntryControl(Label6, &rc, Label6->Font);
	AlignList.EntryControl(AllEle, &rc, AllEle->Font);
	AlignList.EntryControl(PitchAbs, &rc, PitchAbs->Font);
	AlignList.EntryControl(DegUnit, &rc, DegUnit->Font);
	AlignList.EntryControl(TBMCH, &rc, NULL);
	AlignList.EntryControl(Label7, &rc, Label7->Font);
	AlignList.EntryControl(BandBtn, &rc, BandBtn->Font);
	AlignList.EntryControl(TerBtn, &rc, TerBtn->Font);
	AlignList.EntryControl(Label8, &rc, Label8->Font);
	AlignList.EntryControl(TBCUR, &rc, NULL);
	AlignGrid.InitGrid(Grid);

	int CX = ::GetSystemMetrics(SM_CXFULLSCREEN);
	int CY = ::GetSystemMetrics(SM_CYFULLSCREEN);
//	int CX = ::GetSystemMetrics(SM_CXSCREEN);
//	int CY = ::GetSystemMetrics(SM_CYSCREEN);
	if( CX < Width ){
		Left = 0;
		Width = CX;
	}
	if( CY < Height ){
		Top = 0;
		Height = CY;
	}
	FormCenter(this, CX, CY);
	if( Owner != NULL ){
		WindowState = ((TForm *)Owner)->WindowState;
	}
}
//---------------------------------------------------------------------------
void __fastcall TACalDlg::FormResize(TObject *Sender)
{
	AlignList.NewAlign(this);
	AlignGrid.NewAlign(Grid);
}
//---------------------------------------------------------------------------
void __fastcall TACalDlg::BandBtnClick(TObject *Sender)
{
	TACalMultDlg *Box = new TACalMultDlg(this);

	Box->Execute(pACal);
	delete Box;
}
//---------------------------------------------------------------------------
void __fastcall TACalDlg::TerDisClick(TObject *Sender)
{
	UpdateView();
}
//---------------------------------------------------------------------------
// 連携の設定
static void AddAVAL(CACal *cp, int type, int pos, int sub, int with, double d, double dm)
{
	if( cp->amax >= AVMAX ) return;
	AVAL *ap = &cp->aval[cp->amax];
	ap->Type = type;
	ap->With = with;
	ap->Pos = pos;
	ap->SubPos = sub;
	if( cp->PitchUnit ){
		ap->DW = GetNormal(d / dm);
	}
	else {
		ap->DW = 100.0 / dm;
	}
	if( !ap->DW ) ap->DW = 0.001;
	ap->DMAX = 2000.0;
	if( (type == avWIRE) && (sub != 4) ){
		ap->DMIN = -2000.0;
	}
	else {
		ap->DMIN = 0.0;
	}
	cp->amax++;
}
//---------------------------------------------------------------------------
// 既登録のチェック
int __fastcall TACalDlg::CheckSameWireChen(double ox, double oy, double oz)
{
	AVAL	*ap = pACal->aval;
	WDEF	*wp;
	for( int i = 0; i < pACal->amax; i++, ap++ ){
		if( ap->Type == avWIRE ){
			wp = &pACal->AP->wdef[ap->Pos];
			if( (ap->SubPos < 2) || (ap->SubPos == 5) ){
				if( (ox == wp->X1) && (oy == wp->Y1) && (oz == wp->Z1) ) return TRUE;
			}
			else {
				if( (ox == wp->X2) && (oy == wp->Y2) && (oz == wp->Z2) ) return TRUE;
			}
		}
	}
	return FALSE;
}
//---------------------------------------------------------------------------
// 連携ワイヤの一括設定
void __fastcall TACalDlg::AddSameWireChen(int n)
{
	AVAL	*ap = &pACal->aval[n];
	WDEF	*wp;
	double	d, dd;
	int i;

	if( ap->SubPos == 5 ){
		d = pACal->AP->wdef[ap->Pos].X1;
	}
	else if( ap->SubPos == 6 ){
		d = pACal->AP->wdef[ap->Pos].X2;
	}
	else {
		d = pACal->GetNowValue(n);
	}
	d = RoundUp(d, 100000);		// 現在の値を得る
#if 0
	if( (ap->SubPos == 5)||(ap->SubPos == 6) ){		// Ｘ軸
		WREF *rp;
		int max = 0;
		for( i = 0; i < pACal->WList.gmax; i++ ){
			rp = pACal->WList.List[i].pB;
			for( int j = 0; j < pACal->WList.List[i].bcnt; j++, rp++ ){
				if( ap->Pos == rp->ref ){
					rp = pACal->WList.List[i].pB;
					max = pACal->WList.List[i].bcnt;
					break;
				}
			}
			if( max ) break;
		}
		for( i = 0; i < max; i++, rp++ ){
			if( CheckSameWireChen(rp->wp->X1, rp->wp->Y1, rp->wp->Z1)==FALSE ){
				dd = RoundUp(rp->wp->X1, 100000);
				if( dd == d ){
					AddAVAL(pACal, avWIRE, rp->ref, 5, n, d, 100);
				}
				else if( dd == -d ){
					AddAVAL(pACal, avWIRE, rp->ref, 5, -n-2, d, 100);
				}
			}
			if( CheckSameWireChen(rp->wp->X2, rp->wp->Y2, rp->wp->Z2)==FALSE ){
				dd = RoundUp(rp->wp->X2, 100000);
				if( dd == d ){
					AddAVAL(pACal, avWIRE, rp->ref, 6, n, d, 100);
				}
				else if( dd == -d ){
					AddAVAL(pACal, avWIRE, rp->ref, 6, -n-2, d, 100);
				}
			}
		}
		return;
	}
#endif
	for( wp = pACal->AP->wdef, i = 0; i < pACal->WList.wmax; i++, wp++ ){
		if( ap->SubPos == 4 ){		// 半径の時
			if( i != ap->Pos ){
				if( RoundUp(wp->R, 100000) == d ){
					AddAVAL(pACal, avWIRE, i, ap->SubPos, n, d, 100);
				}
			}
		}
		else if( (ap->SubPos == 5)||(ap->SubPos == 6) ){		// Ｘ軸
			if( (i!=ap->Pos) || (ap->SubPos!=5) ){			// X1
				if( CheckSameWireChen(wp->X1, wp->Y1, wp->Z1)==FALSE ){
					dd = RoundUp(wp->X1, 100000);
					if( dd == d ){
						AddAVAL(pACal, avWIRE, i, 5, n, d, 100);
					}
					else if( dd == -d ){
						AddAVAL(pACal, avWIRE, i, 5, -n-2, d, 100);
					}
				}
			}
			if( (i!=ap->Pos) || (ap->SubPos!=6) ){			// X2
				if( CheckSameWireChen(wp->X2, wp->Y2, wp->Z2)==FALSE ){
					dd = RoundUp(wp->X2, 100000);
					if( dd == d ){
						AddAVAL(pACal, avWIRE, i, 6, n, d, 100);
					}
					else if( dd == -d ){
						AddAVAL(pACal, avWIRE, i, 6, -n-2, d, 100);
					}
				}
			}
		}
		else {						// 座標の時
			if( (i!=ap->Pos) || (ap->SubPos!=0) ){			// Y1
				if( CheckSameWireChen(wp->X1, wp->Y1, wp->Z1)==FALSE ){
					dd = RoundUp(wp->Y1, 100000);
					if( dd == d ){
						AddAVAL(pACal, avWIRE, i, 0, n, d, 100);
					}
					else if( dd == -d ){
						AddAVAL(pACal, avWIRE, i, 0, -n-2, d, 100);
					}
				}
			}
			if( (i!=ap->Pos) || (ap->SubPos!=1) ){			// Z1
				if( CheckSameWireChen(wp->X1, wp->Y1, wp->Z1)==FALSE ){
					dd = RoundUp(wp->Z1, 100000);
					if( dd == d ){
						AddAVAL(pACal, avWIRE, i, 1, n, d, 100);
					}
					else if( dd == -d ){
						AddAVAL(pACal, avWIRE, i, 1, -n-2, d, 100);
					}
				}
			}
			if( (i!=ap->Pos) || (ap->SubPos!=2) ){			// Y2
				if( CheckSameWireChen(wp->X2, wp->Y2, wp->Z2)==FALSE ){
					dd = RoundUp(wp->Y2, 100000);
					if( dd == d ){
						AddAVAL(pACal, avWIRE, i, 2, n, d, 100);
					}
					else if( dd == -d ){
						AddAVAL(pACal, avWIRE, i, 2, -n-2, d, 100);
					}
				}
			}
			if( (i!=ap->Pos) || (ap->SubPos!=3) ){			// Z2
				if( CheckSameWireChen(wp->X2, wp->Y2, wp->Z2)==FALSE ){
					dd = RoundUp(wp->Z2, 100000);
					if( dd == d ){
						AddAVAL(pACal, avWIRE, i, 3, n, d, 100);
					}
					else if( dd == -d ){
						AddAVAL(pACal, avWIRE, i, 3, -n-2, d, 100);
					}
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
// 連携ロードの一括設定
void __fastcall TACalDlg::AddSameLoadChen(int n)
{
	AVAL	*ap = &pACal->aval[n];
	LDEF	*lp;
	double	d;
	int		type;

	type = pACal->AP->ldef[ap->Pos].TYPE;
	d = RoundUp(pACal->GetNowValue(n), 100000);		// 現在の値を得る
	int i;
	for( lp = pACal->AP->ldef, i = 0; i < pACal->AP->lmax; i++, lp++ ){
		if( (i != ap->Pos) && (lp->TYPE == type) ){
			if( ap->SubPos ){
				if( RoundUp(lp->B[0], 100000) == d ){
					AddAVAL(pACal, avLOAD, i, ap->SubPos, n, d, 100);
				}
			}
			else {
				if( RoundUp(lp->A[0], 100000) == d ){
					AddAVAL(pACal, avLOAD, i, ap->SubPos, n, d, 100);
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
// 連携の一括設定
void __fastcall TACalDlg::K11Click(TObject *Sender)
{
	if( Grid->Row ){
		int n = Grid->Row - 1;
		switch(pACal->aval[n].Type){
			case avWIRE:		// ワイヤ
				AddSameWireChen(n);
				UpdateView();
				break;
			case avLOAD:		// ロード
				AddSameLoadChen(n);
				UpdateView();
				break;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TACalDlg::TerBtnClick(TObject *Sender)
{
	TACalInfoBox *Box = new TACalInfoBox(this);

	Box->Execute(pACal);
	delete Box;
}
//---------------------------------------------------------------------------
void __fastcall TACalDlg::K23Click(TObject *Sender)
{
	if( !pACal->amax ) return;
	int N = Grid->Row - 1;
	if( N < 0 ) N = 0;
	while( N >= pACal->amax ) N--;
	UnicodeString st = StrDbl(pACal->aval[N].DW);	// ja7ude 1.0
	if( InputQuery("MMANA-最適化", "ピッチを入力", st) == TRUE ){
		double d;
		if( Calc(d, AnsiString(st).c_str()) == TRUE ){
			AVAL *ap = pACal->aval;
			for( int i = 0; i < pACal->amax; i++, ap++ ){
				if( ap->With == -1 ){
					ap->DW = d;
				}
			}
			Grid->Invalidate();
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TACalDlg::K27Click(TObject *Sender)
{
	pACal->amax = 0;
	memset(&pACal->aval, 0, sizeof(pACal->aval));
	UpdateView();
}
//---------------------------------------------------------------------------
// エレメントの連携設定
void __fastcall TACalDlg::K34Click(TObject *Sender)
{
	AVAL	*ap = pACal->aval;
	int i, j;
	for( i = 0; i < pACal->amax; i++, ap++ ){
		if( (ap->Type == avELE)&&(ap->With == -1) ){
			AVAL *rp = ap + 1;
			double d = RoundUpStr(pACal->GetNowValue(i));
			for( j = i + 1; j < pACal->amax; j++, rp++ ){
				if( (rp->Type == avELE) && (rp->With == -1) ){
					switch(ap->SubPos){
						case 1:
						case 2:
							if( (rp->SubPos!=1)&&(rp->SubPos!=2) ) continue;
							break;
						default:
							if( ap->SubPos != rp->SubPos ) continue;
							break;
					}
					if( d == RoundUpStr(pACal->GetNowValue(j)) ){
						rp->With = i;
					}
				}
			}
		}
	}
	UpdateView();
}
//---------------------------------------------------------------------------
