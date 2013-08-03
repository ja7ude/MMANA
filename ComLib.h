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

#ifndef ComLibH
#define ComLibH
#include <stdio.h>    //ja7ude 1.0
#include <stdlib.h>   //ja7ude 1.0
#include <ExtCtrls.hpp>
#include <inifiles.hpp>
#include <Grids.hpp>
//#include <vcl\comctrls.hpp>
#include <values.h>
#include <math.h>
#include <mbstring.h>
//---------------------------------------------------------------------------
#define	WMAX	512		// ワイヤの数
#define	WZMAX	4096	// 展開ワイヤの数
#define	CMAX	64		// 給電点の数
#define	LMAX	100		// ロードの数
#define	EMAX	15		// 環境の数
#define	PMAX	8192	// 扱えるパルスの数
#define	SMAX	12		// S-DOMAINの階数

#define	PTMAX	10
#define	PPMAX	10

#define	RESMAX	10		// 結果比較の最大の数

#define	VERSTR	"MMANA Ver1.77 (C) JE3HHT 1999-2000"
#define RESSTR	"MMANA Res1.16\x1a"	// RESVERと同じ値
#define ACALSTR	"MMANA Opt1.13\x1a"	// OPTVERと同じ値
#define	RESVER	116		// RES Verを変更した場合 TMainWnd::LoadResFile()をチェック
#define	OPTVER	113

extern	LPCSTR	FreqTbl[];
extern	const char	ILLFMT[];

enum TFontPitch { fpDefault, fpVariable, fpFixed };	//JA7UDE 0427

typedef struct {		// ワイヤーの定義
	double	X1;
	double	Y1;
	double	Z1;
	double	X2;
	double	Y2;
	double	Z2;
	double	R;
	int		SEG;			// セグメント数

	int		PNo;			// ワイヤの開始パルス番号
	int		PMax;			// パルスの数
}WDEF;
typedef struct {		// 給電点の定義
	char	PLUS[8+1];		// パルス位置
	int		PLUSNo;			// パルス番号
	double	EV;				// 電圧
	double	DEG;			// 位相
}CDEF;
typedef struct {		// ロードの定義
	int     TYPE;			// 0-L,C,Q, 1-R+jX, 2-S-Domain
	char	PLUS[8+1];		// パルス位置
	int		PLUSNo;			// パルス番号
	int		SN;				// 階数
	double	A[SMAX];
	double	B[SMAX];
}LDEF;
typedef struct {		// 分割定義
	double	RR;				// 参照Ｒ
	double	L[PPMAX];
	double	R[PPMAX];
	int		Type;			// 0-中央分割, 1-始点分割
}PDEF;
typedef struct {		// ワイヤーの定義
	double	X1;
	double	Y1;
	double	Z1;
	double	X2;
	double	Y2;
	double	Z2;
	double	R;
	int		SEG;			// セグメント数

	int		Wno;			// 参照元のワイヤ番号（０～）
	int		PNo;			// ワイヤの開始パルス番号
	int		SNo;			// ワイヤの開始セグメント番号 aX[SNo]
	int		SMax;			// セグメント番号の最大値
}WZDEF;
typedef struct {		// アンテナの定義
	int		Edit;			// 編集フラグ
	int		Flag;			// 設定フラグ
	char	Name[129];		// 名称
	double	fq;				// 設計周波数
	int		wmax;			// ワイヤーの数
	WDEF	wdef[WMAX];		// ワイヤー定義
	double	cfq;			// 給電周波数
	int		cmax;			// 給電点の数
	int		cauto;			// 給電電圧自動設定
	CDEF	cdef[CMAX];		// 給電点定義
	int		lenb;			// ロード計算を許可
	int		lmax;
	LDEF	ldef[LMAX];

	int		pmax;
	PDEF	pdef[PTMAX];	// ワイヤ組み合わせデータ

	int		wzmax;			// 展開ワイヤーの数
	WZDEF	wzdef[WZMAX];	// 展開ワイヤー定義

	double	MinZ;			// 定義ワイヤの最下部のＺ座標

	double	RMD;			// 波長（λ）
	int		DM1;			// 最小分割幅 1/DM1λ
	int		DM2;			// 最大分割幅 1/DM2λ
	int		EC;				// 端点のセグメント数
	double	SC;				// 分割係数 (1-2)

	int		StackVT;		// 展開方向 0-上下，１－上向き，２－下向き
	int		StackH;
	int		StackV;
	double	StackHW;
	double	StackVW;
}ANTDEF;

typedef struct {		// 計算環境
	int		type;			// 0-自由空間, 1-完全大地, 2-リアルグランド
	double	antheight;		// 地上高
	int		mmax;			// メディアの数
	int		mtype;			// メディアの種類 0-リニア, 1-グランドスクリーン
	int		gscrcnt;		// ラディアルの本数
	double	gscrr;			// ラジアルの半径
	double	rel[EMAX];		//
	double	cond[EMAX];		//
	double	intval[EMAX];	// メディアの距離
	double	height[EMAX];	// メディアの高さ

	int		fbr;			// F/B比のバック側比較範囲
	double	RO;				// SWRの計算インピーダンス
	double	JXO;			// SWRの計算インピーダンス（複素数）
	int		WireRoss;
	int		pmax;			// 扱う最大のパルス数
	int		FontSize;		// フォントのサイズ
}ENVDEF;
extern	ENVDEF	env;

typedef struct {		// 電流ファイル用メモリ
	int		Flag;
	int		W;
	float	X;
	float	Y;
	float	Z;
	float	I;
	float	IJ;
	float	MAG;
	float	DEG;
}CURFILE;
typedef struct {		// 近距離電界強度ファイル
	FILE	*fp;
	char	Name[256];		// ファイル名
	int		TYPE;
	double	POW;
	double	X;
	double	XW;
	int		XN;
	double	Y;
	double	YW;
	int		YN;
	double	Z;
	double	ZW;
	int		ZN;
}NEARFILE;
typedef struct {		// 遠距離電界強度ファイル
	FILE	*fp;
	char	Name[256];
	double	Z;
	double	ZW;
	int		ZN;
	double	A;
	double	AW;
	int		AN;
}FARFILE;
typedef struct {		// 実行環境の定義
	double	AntXc;			// アンテナ形状表示の中心
	double	AntYc;			// アンテナ形状表示の中心

	int		CalcDisp;		// 最適化中の画面更新
	int		CalcAbort;		// 最適化中止フラグ
	int		CalcF;			// 計算実行フラグ
	int		CalcLog;		// ログウインドウへの表示フラグ

	int		RmdSel;			// エレメント編集の波長表記
	double	rmd;			// 設計周波数の波長

	int		IntPos;			// 0-間隔表記，1-位置表記
	int		Wave;			// 表示偏波
	int		BwMatch;		// 周波数特性の帯域表示マッチング

	int		Ant3D;			// ３Ｄ表示方法
	int		CurDir;			// 電流方向の表現
	int		FixFreeAngle;	// 自由空間の時は計算仰角を０度に固定
	int		RecentMax;		// Recent File の表示する数
	int		RecentMAA;		// Recent File はMAAファイルのみ

	int		WindowState;	// 画面の状態

    int		EleWidthAll;	// エレメント編集の際の横幅、縦幅の扱い

	CURFILE	*pCurFile;		// 電流ファイル
	NEARFILE *pNearFile;	// 近距離電界強度フィアル
	FARFILE	*pFarFile;		// 遠距離電界強度ファイル
}EXEENV;
extern	EXEENV	exeenv;

int GetFileVersion(LPCSTR pbf, LPCSTR pVer);
char LastC(LPCSTR p);
void SetEXT(LPSTR pName, LPSTR pExt);
LPCSTR GetEXT(LPCSTR Fname);
void ZSCopy(LPSTR t, LPCSTR p, int n);
void SetFileName(LPSTR pName);
LPCSTR GetFileName(LPCSTR pName);
LPCSTR StrCLX(double r, double x);
LPCSTR StrCLXZ(double r, double x);
LPCSTR StrCLXF(double r, double x);
LPCSTR StrDbl(double d);
LPCSTR StrDbl3(double d);
LPCSTR StrDbl2(double d);
LPCSTR StrDbl1(double d);
LPCSTR StrDblE(double d);
LPCSTR StrInt(int d);
double RoundUp(double d);
double RoundUp(double d, double s);
double RoundUpStr(double d);
double GetNormal(double d);
void SetCurDir(LPSTR t, int size);
void SetDirName(LPSTR t, LPCSTR pName);
double CalcSWR(double Rl, double Xl, double Ro, double Xo);
void CalcParaZ(double &R, double &X, double f, double L, double C, double Q);
double GetDeg(double x, double y);
void DeleteComment(LPSTR bf);
void ClipLF(LPSTR sp);
LPSTR SkipSpace(LPSTR sp);
LPCSTR SkipSpace(LPCSTR sp);
LPSTR StrDlm(LPSTR &tp, LPSTR sp);
LPSTR StrDlm(LPSTR &tp, LPSTR sp, char c);
void ErrorMB(LPCSTR fmt, ...);
void WarningMB(LPCSTR fmt, ...);
int YesNoMB(LPCSTR fmt, ...);
void Calc3DXY(double &x, double &y, double deg, double zdeg, double X, double Y, double Z);
double ReadDoubleIniFile(TIniFile *p, LPCSTR c1, LPCSTR c2, double d);
void WriteDoubleIniFile(TIniFile *p, LPCSTR c1, LPCSTR c2, double d);

PDEF *FindPP(ANTDEF *ap, double r);
int CombWire(WDEF *tbl, ANTDEF *ap, PDEF *pp, WDEF *wp);
void AddWZ(ANTDEF *ap, double x1, double y1, double z1, double x2, double y2, double z2, double R, int SEG, int wno);
void TeiAvg(ANTDEF *ap, double x1, double y1, double z1, double x2, double y2, double z2, double R, int DM, int wno);
void Tei(ANTDEF *ap, double x1, double y1, double z1, double x2, double y2, double z2, double R, int DM1, int wno);
int SetWZ(ANTDEF *ap);

LPCSTR Seg2Str(int Seg);
int Str2Seg(int &Seg, LPCSTR p);

int Str2PlusNo(ANTDEF *ap, LPCSTR s);
void AdjPlusNo(ANTDEF *ap);

int WriteAntToFp(ANTDEF *ap, AnsiString &rem, FILE *fp);
int ReadAntFromFp(ANTDEF *ap, AnsiString &rem, FILE *fp);
void AdjWireChen(WDEF *wp, double ox, double oy, double oz, double nx, double ny, double nz);
void AdjWireChen(WDEF *wp, int wmax, double ox, double oy, double oz, double nx, double ny, double nz);
void AdjWireChen(WDEF *wp, int wmax, WDEF *np, WDEF *op);
void GetWirePoll(double &WL, double &XYD, double &YZD, WDEF *wp);
int IsLine(POINT &pnt, POINT &spnt, POINT &tpnt, int Width);
int IsLine(POINT &pnt, POINT &spnt, POINT &tpnt);
int DoCalc(void);

double GetRmdVal(double d);
double SetRmdVal(double d);
double GetRmdValD(double d);
double SetRmdValD(double d);

int Calc(double &d, LPCSTR p);
int CalcI(int &d, LPCSTR p);
int CalcU(int &d, LPCSTR p);
void WriteCur(ANTDEF *ap, int W, int P, float I, float IJ, float Mag, float Deg);
void ClipXY(int &x, int &y);
double GetWireRoss(ANTDEF *ap, WDEF *wp);
double GetWireRoss(double f, WDEF *wp);
void FormCenter(TForm *tp, int XW, int YW);
void DrawWirePara(TPaintBox *tp, WDEF *wp, int w);
void DrawWirePara(TPaintBox *tp, ANTDEF *ap, int w);
void AdjPdef(ANTDEF *ap);

extern	char	BgnDir[128];
extern	char	AntDir[128];
extern	char	ResDir[128];
extern	AnsiString	antDef;			// 最適化前のアンテナ定義
extern	AnsiString	antSave;		// 記憶したアンテナ定義

//---------------------------------------------------------------------------
#define	PAI		3.1415926535897932384626433832795
#define	PAI2	(PAI*2.0)
#define	COS30	0.866025403784438646763723170752936
#define	TAN15	0.267949192431122706472553658494128
#define	SIN15	0.258819045102520762348898837624048
#define	COS15	0.965925826289068286749743199728897
#define	TAN22R5	0.414213562373095048801688724209698
#define	SIN22R5	0.382683432365089771728459984030399
#define	COS22R5	0.923879532511286756128183189396788
#define	RUTE2	1.4142135623730950488016887242097
#define	NULLV	(1.0e-99)
#define	NULLF	float(1.2345e-37)

#define	CR		'\r'
#define	LF		'\n'
#define	TAB		'\t'

#define	ABS(c)	(((c)<0)?(-(c)):(c))
#define	SGN(c)	(((c)<0)?-1:(((c)>0)?1:0))
//---------------------------------------------------------------------------
///---------------------------------------------------------
///  テキストバッファストリーマー
class StrText{
public:
	char	*Bp;
	char	*Wp;
	inline StrText(int max){
		Bp = new char[max];
		Wp = Bp;
	};
	inline ~StrText(){
		delete Bp;
	};
	inline char *Printf(char *ct, ...){
		va_list	pp;

		va_start(pp, ct);
		vsprintf(Wp, ct, pp );
		va_end(pp);
		ct = Wp;
		Wp += strlen(Wp);
		return(ct);
	};
	inline char *GetText(void){
		return Bp;
	};
	inline void Clear(void){
		Wp = Bp;
		*Wp = 0;
	};
};

///---------------------------------------------------------
///  テキストファイルストリーマー
class CTextFile
{
private:
	char	*Bf;
	int		Len;
	int		Max;
	int		N;
	char	*rp;
	TFileStream *FP;

	inline void LoadBuf(void){
		Len = FP->Read(Bf, Max);
		rp = Bf;
		N = 0;
	}
	inline char GetChar(void){
		char c = *rp++;
		N++;
		if( N >= Len ) LoadBuf();
		return c;
	};

public:
	inline CTextFile(TFileStream *fp, int max = 16384){
		Max = max;
		Bf = new char[Max];
		Len = 0;
		FP = fp;
		LoadBuf();
	};
	inline ~CTextFile(){
		delete Bf;
	}
	inline int LoadText(LPSTR tp, int len){
		char c;
		int n = 0;
		if( !Len ) return FALSE;
		while(Len){
			c = GetChar();
			if( c == LF ){
				*tp = 0;
				return TRUE;
			}
			else if( (c != CR)&&(c != 0x1a) ){
				if( n < (len-1) ){
					*tp++ = c;
					n++;
				}
			}
		}
		*tp = 0;
		return TRUE;
	};
};

///---------------------------------------------------------
///  テキスト文字列ストリーマー
class CTextString
{
private:
	LPCSTR	rp;
public:
	inline CTextString(LPCSTR p){
		rp = p;
	};
	inline CTextString(AnsiString &As){
		rp = As.c_str();
	};
	inline int LoadText(LPSTR tp, int len){
		char c;
		int n = 0;
		if( !(*rp) ) return FALSE;
		while(*rp){
			c = *rp++;
			if( c == LF ){
				*tp = 0;
				return TRUE;
			}
			else if( (c != CR)&&(c != 0x1a) ){
				if( n < (len-1) ){
					*tp++ = c;
					n++;
				}
			}
		}
		*tp = 0;
		return TRUE;
	};
	inline int LoadTextLoop(LPSTR tp, int len){
		while(1){
			if( LoadText(tp, len) == FALSE ) break;
			if( *tp != '*' ) return TRUE;
		}
		return FALSE;
	}
};

///---------------------------------------------------------
///  テキスト文字列ＦＩＦＯ
class CTextFifo
{
private:
	LPSTR	bp, wp, rp;
	int		Cnt;
	int		Max;
	int 	Line;
public:
	inline CTextFifo(int max = 65536){
		Max = max;
		bp = new char[Max];
		wp = rp = bp;
		Cnt = 0;
		Line = 0;
	};
	inline ~CTextFifo(){
		delete bp;
	};
	inline void WriteChar(char c){
		if( Cnt < Max ){
			if( c != LF ){
				*wp = c;
				wp++;
				if( wp >= (bp+Max) ){
					wp = bp;
				}
				Cnt++;
			}
			if( c == CR ){
				Line++;
			}
		}
	}
	inline void WriteString(LPCSTR sp){
		for( ; *sp; sp++ ){
			WriteChar(*sp);
		}
	};
	inline int GetLine(void){
		return Line;
	};
	inline int LoadText(LPSTR tp, int len){
		if( !Line ) return FALSE;
		while(1){
			char c = *rp;
			rp++;
			if( rp >= (bp+Max) ) rp = bp;
			Cnt--;

			if( c == CR ){
				*tp = 0;
				Line--;
				break;
			}
			else if( (len - 1) > 0 ){
				*tp++ = c;
				len--;
			}
		}
		return TRUE;
	};
};

///----------------------------------------------------------------
///	複素数演算クラス
class CLX {
public:
	double	r;
	double	j;
	inline CLX(void){
		r = 0;
		j = 0;
	};
	inline CLX(const double R){
		r = R;
		j = 0;
	};
	inline CLX(const double R, const double J){
		r = R;
		j = J;
	};
	inline CLX(CLX &ref){
		r = ref.r;
		j = ref.j;
	};
	inline void Add(const CLX &ref){
		r += ref.r;
		j += ref.j;
	};
	inline void Sub(const CLX &ref){
		r -= ref.r;
		j -= ref.j;
	};
	void Mul(const CLX &ref);
	void Div(const CLX &ref);
	inline CLX& operator=(const CLX &ref){
		if( this != &ref ){
			r = ref.r;
			j = ref.j;
		}
		return *this;
	};
	inline CLX& operator+=(const CLX &ref){
		Add(ref);
		return *this;
	};
	inline CLX& operator+=(const double R){
		CLX ref(R, 0);
		Add(ref);
		return *this;
	};
	inline CLX& operator-=(const CLX &ref){
		Sub(ref);
		return *this;
	};
	inline CLX& operator-=(const double R){
		CLX ref(R, 0);
		Sub(ref);
		return *this;
	};
	inline CLX& operator*=(const CLX &ref){
		Mul(ref);
		return *this;
	};
	inline CLX& operator*=(const double R){
		CLX ref(R, 0);
		Mul(ref);
		return *this;
	};
	inline CLX& operator/=(const CLX &ref){
		Div(ref);
		return *this;
	};
	inline CLX& operator/=(const double R){
		CLX ref(R, 0);
		Div(ref);
		return *this;
	};
	inline CLX operator+(const CLX &ref) const {
		CLX a(r, j);
		a+=ref;
		return a;
	};
	inline CLX operator+(const double R) const {
		CLX a(r, j);
		a+=R;
		return a;
	};
	inline CLX operator-(const CLX &ref) const {
		CLX a(r, j);
		a-=ref;
		return a;
	};
	inline CLX operator-(const double R) const {
		CLX a(r, j);
		a-=R;
		return a;
	};
	inline CLX operator*(const CLX &ref) const {
		CLX a(r, j);
		a*=ref;
		return a;
	};
	inline CLX operator*(const double R) const {
		CLX a(r, j);
		a*=R;
		return a;
	};
	inline CLX operator/(const CLX &ref) const {
		CLX a(r, j);
		a/=ref;
		return a;
	};
	inline CLX operator/(const double R) const {
		CLX a(r, j);
		a/=R;
		return a;
	};
	friend CLX operator+(const double R, const CLX &ref);
	friend CLX operator-(const double R, const CLX &ref);
	friend CLX operator*(const double R, const CLX &ref);
	friend CLX operator/(const double R, const CLX &ref);
};

#define	BWMAX	5
#define	BWSMAX	(BWMAX-1)
#define	BWSDIMMAX	4
typedef struct {
	int		Flag;	// 有効フラグ
	double	FQ;		// 周波数
	double	R;		// Ｒ
	double	JX;		// ｊＸ
	double	GA;
	double	FB;
}BWS;
typedef struct {
	int		Flag;	// 有効フラグ
	double	FQ;		// 周波数
	double	R;		// Ｒ
	double	JX;		// ｊＸ
	double	GA;
	double	FB;
	int		PFlag;
	short	PtnH[3][180];
	short	PtnV[3][91];
}BW;
typedef struct {
	int		bcnt;			// 測定した数
	int		bo;				// 設計データのインデックス
	double	fo;				// 設計周波数
	double	fw;				// 周波数幅
	double	bi;				// 周波数間隔
	BW		bw[BWMAX];
	int		bwsdim;			// 1, 2, 3, 4
	int		bwsfull;		// 0 or 1
    BWS		bws[BWSMAX][BWSDIMMAX];
	int		MatchCenter;	// マッチング周波数
    BWS		MatchData;		// マッチングデータ
}BWC;
typedef struct {		// 結果
	int		TYPE;			// 計算条件
	float	HEIGHT;			// 地上高
	float	FQ;
	float	R;
	float	JX;
	float	SWR;
	float	GAIN;
	float	FB;
	float	ELV;
	int		FBR;
	float	RO;				// SWR計算Ｚ
	float	JXO;			// SWR計算Z
	float	MAXHP;			// 水平偏波の最大値
	float	MAXVP;			// 垂直偏波の最大値
	int		Wave;			// 偏波
}RESL;
///---------------------------------------------------------
///  結果管理クラス
#define	RMAX	128			// 結果のヒストリ
class CRes
{
public:
	char	Name[128];		// アンテナの名称

	int		RWp;			// 次の書き込み位置
	int		RCnt;			// 現在の結果の数
	RESL	Res[RMAX];		// 結果のヒストリ

	int		CalcF;			// 計算フラグ

	int		PtnF;			// パターン計算分解能
	int		Voff;
	int		DegF;
	int		DegV;
	int		DegH;
	float	PtnV[3][181];
	float	PtnH[3][360];
	float	MaxH;
	float	MaxV;
	float	MaxG;
	float	MaxEV;
	float	MaxVP;
	float	MaxHP;
	float	Elv;
	float	ElvHPtn;		// 水平パターンの仰角

	float	Cur[PMAX];		// 電流分布絶対値データ

	int		CurN;
	BYTE	CurDir[PMAX];	// 電流方向データ
	double	RealMax;
	double	ImgMax;

	BWC		Bwc;
public:
	inline CRes(void){
		PtnF = CalcF = 0;
		RWp = 0;
		RCnt = 0;
		RealMax = ImgMax = 0.0;
		memset(&Bwc, 0, sizeof(Bwc));
	};
	inline ~CRes(){
	};
	inline int GetLastRes(double &R, double &X){
		if( RCnt ){
			R = Res[RWp].R;
			X = Res[RWp].JX;
			return TRUE;
		}
		return FALSE;
	};
	inline void Clear(void){
		CalcF = 0;
	};
	inline void SetCalc(LPCSTR pName){
		CalcF = 1;
		strncpy(Name, pName, sizeof(Name));
	}
	inline int IsCalc(void){
		return CalcF;
	};
	inline RESL *GetResP(void){
		return &Res[RWp];
	};
	void IncResP(void);
	RESL *GetResP(int Index);
	inline int GetCnt(void){
		return RCnt;
	};
	inline void SetVert(int off){
		DegF = 1;
		Voff = off;
	};
	inline void SetHori(void){
		Voff = 0;
		DegF = 0;
	};
	inline void SetElv(int off){
		MaxEV = MaxV;
		Voff = off * 10;
		DegF = 2;
		double dv = DegV;
		if( dv >= (90/PtnF) ) dv = (180/PtnF) - dv;
		Elv = dv*PtnF;		// 暫定のピーク
	}
	inline double GetElv(void){
		if( Elv >= 90 ) Elv = 180 - Elv;
		return Elv;
	};
	double GetAdjElv(void);
	void SetPtn(float s1, float s2, float s3);
	int GetHMAX(void);
	int GetVMAX(void);
	void CalcPtn(void);
	void ReCalcGain(void);
	void CalcFB(void);
	void ClearCur(void);
	inline void SetCurDir(int n, double r, double j){
		if( r < 0 ){
			CurDir[n] |= 0x01;
			r = -r;
		}
		if( j < 0 ){
			CurDir[n] |= 0x02;
			j = -j;
		}
		if( r > RealMax ) RealMax = r;
		if( j > ImgMax ) ImgMax = j;
		CurN = n;
	}
	inline void SetCur(int n, double r, double j, double d){
		SetCurDir(n, r, j);
		Cur[n] = d;
	};
	void SetCur(ANTDEF *ap, int n, double r, double j, double d);
	inline void FinishCur(void){
		BYTE	CurMask = BYTE(( RealMax > ImgMax ) ? 0x01 : 0x02);
		for( int i = 0; i <= CurN; i++ ){
			if( CurDir[i] & CurMask ) Cur[i] = -Cur[i];
		}
	};
	void DrawRes(TCanvas *tp, int X, int Y);
	void SaveCurData(FILE *fp);
	void LoadCurData(FILE *fp);
	int CopyLastRes(CRes *rp);
	int SetRes(CRes *rp);

	void InitBWC(void);
	void AddBWC(int n);
	void AddBWS(int n, int j);
	void SetBWSDim(int n);
	void UpdateCountBW(void);
	BWS	 *SearchBW(BWC *bp, double fq);
	void ClearBWC(void);
};
extern CRes	res;

///---------------------------------------------------------
///  パターン描画クラス
class CDrawPtnH
{
public:
	int		xl,yt,xr,yb;
	RECT	rc;				// 円の正方形
	int		xc, yc;
	int		rr;
	int		ri;
	TCanvas	*tp;			// キャンバスのポインタ

	double	Max;			// 0db == max
	TColor	gCol;
	void GetDBR(int &X, int &Y, double deg, double db);
public:
	inline CDrawPtnH(void){
		Max = 0.0;
		tp = NULL;
		gCol = clGray;
	};
	inline ~CDrawPtnH(){
	};
	void SetRect(TCanvas *pc, int x1, int y1, int x2, int y2);
	inline void SetMaxDB(double m){
		Max = m;
	};
	void DrawAngle(int Add = 1);
	void DrawGage(double deg, int i);
	void DrawPtn(float *pt, int Add, TColor Col = clBlack);
};


///---------------------------------------------------------
///  パターン描画クラス
class CDrawPtnV
{
public:
	int		xl,yt,xr,yb;
	RECT	rc;				// 円の正方形
	int		xc, yc;
	int		rr;
	int		ri;
	TCanvas	*tp;			// キャンバスのポインタ

	double	Max;			// 0db == max
	TColor	gCol;
	void GetDBR(int &X, int &Y, double deg, double db);
	void DrawGage(double deg, int i);
public:
	inline CDrawPtnV(void){
		Max = 0.0;
		tp = NULL;
		gCol = clGray;
	};
	inline ~CDrawPtnV(){
	};
	void SetRect(TCanvas *pc, int x1, int y1, int x2, int y2);
	inline void GetBottomPosition(int &x, int &y){
		x = xl;
		y = yc;
	};
	inline void SetMaxDB(double m){
		Max = m;
	};
	void DrawAngle(int Add = 1);
	void DrawPtn(float *pt, int Add, TColor Col = clBlack);
};

///---------------------------------------------------------
///  コントロールのアラインの管理クラス
class CAlign
{
private:
	int		BTop, BLeft;
	int		BWidth, BHeight;
	int		OTop, OLeft;
	int		OWidth, OHeight;
	int		OFontHeight;

	TControl	*tp;
	TFont		*fp;
public:
	inline CAlign(void){
		tp = NULL;
		fp = NULL;
	};
	inline ~CAlign(){
	};
	void InitControl(TControl *p, TControl *pB, TFont *pF = NULL);
	void InitControl(TControl *p, RECT *rp, TFont *pF = NULL);
	void NewAlign(TControl *pB);
};

///---------------------------------------------------------
///  コントロールのアラインの管理クラス
class CAlignList
{
private:
	int		Max;
	int		Cnt;
	CAlign	**AlignList;
	void Alloc(void);
public:
	CAlignList(void);
	~CAlignList();
	void EntryControl(TControl *tp, TControl *pB, TFont *pF = NULL);
	void EntryControl(TControl *tp, RECT *rp, TFont *pF = NULL);
	void NewAlign(TControl *pB);
};

class CAlignGrid
{
private:
	int		BWidth;
	int		BHeight;
	int		BRowHeight;

	int		Max;
	int		*bp;
public:
	inline CAlignGrid(void){
		Max = 0;
		bp = NULL;
	};
	inline ~CAlignGrid(){
		if( bp != NULL ){
			delete bp;
		}
	};
	void InitGrid(TStringGrid *p);
	void NewAlign(TStringGrid *p);
};


enum CWTYPE {
	cwNULL,		// 不明
	cwVDP,		// 垂直DP
	cwVSQ,		// 垂直□
	cwVDM,		// 垂直◇
	cwVTA,		// 垂直△
	cwVITA,		// 垂直▽
	cwVPIE,		// 垂直○
	cwVLP,		// 垂直ループ
	cwHDP,		// 水平DP
	cwHSQ,		// 水平□
	cwHDM,		// 水平◇
	cwHTA,		// 水平△
	cwHITA,		// 水平▽
	cwHLP,		// 水平ループ
	cwHANT,		// 水平系
	cwVANT,		// 垂直系
	cwVSQ2,		// 垂直双□
	cwVTA2,		// 垂直双▽
};
typedef struct {
	int		ref;
	WDEF	*wp;
	double	D;				// ワイヤの距離
	double	XX;
	double	YY;
	double	ZZ;
	double	VXY;
	double	VYZ;
	int		LCF;
}WREF;
typedef struct {
	double	X, Y, Z;		// 頂点の座標
	double	VXY;
	double	VYZ;
}LOOPS;

class CWGroup
{
private:
	int		bmax;		// ワイヤ配列の数
	WDEF	*pW;		// ワイヤ定義配列のポインタ
	double	YL;
	double	YH;
	double	ZL;
	double	ZH;
	double	XL;
	double	XH;
	int		XwF;		// ループアンテナフラグ
public:
	int		Type;		// 形式
	int		bcnt;		// ワイヤの数
	int		EnbR;		// Ｒ変更可能フラグ
	int		EnbSeg;		// SEG変更可能フラグ
	WREF	*pB;		// ワイヤリストの配列

	double	BX;				// 中心位置の座標
	double	BY;
	double	BZ;
private:
	int AnaLoop(int N, double OX, double OY, double OZ, double X, double Y, double Z);
	void AnaType(void);
	void SetPieRound(double d);
public:
	CWGroup(void);
	inline ~CWGroup(){
		Delete();
	};
	void Clear(void);
	void Init(WDEF *p);
	void Delete(void);
	void Alloc(void);
	void AddWire(int Index);
	void AnaWire(void);
	inline double GetX(void){			// Ｘ軸ポジションを得る
		return BX;
	};
	inline double GetHeight(void){		// 高さを得る
		return (ZH != ZL) ? ZH - ZL : (XwF == 0) ? XH - XL : 0.0;
	};
	inline double GetWidth(void){		// 横幅を得る
		return (YH != YL) ? YH - YL : (XwF == 0) ? XH - XL : 0.0;
	};
	double GetRound(void);		// 全周を得る
	void SetRound(double d);	// 全周を設定する
	inline double GetR(void){
		if( bcnt ){
			return pB->wp->R;
		}
		else {
			return 0.005;
		}
	}
	inline int GetSEG(void){
		if( bcnt ){
			return pB->wp->SEG;
		}
		else {
			return 0;
		}
	}
	void SetXWidth(double W);
	void SetWidth(double W);
	void SetHeight(double W);
	void SetRoundAll(double d);	// 全周を設定する
	void SetXWidthAll(double W);
	void SetWidthAll(double W);
	void SetHeightAll(double W);
	void SetR(double r);
	void SetSeg(int s);
	inline int EnbXw(void){
		return XwF;
	};

	void SetXWidth(int all, double W);
	void SetWidth(int all, double W);
	void SetHeight(int all, double W);
	void SetRound(int all, double d);	// 全周を設定する
};

#define	CWMAX	256
class CWGroupList
{
private:
	BYTE	Con[WMAX];
	int		Edit;
	double	OffX;
public:
	int		gmax;
	CWGroup	List[CWMAX];
	int		BaseW;			// 基準ワイヤ位置
	double	Intval[CWMAX];

	ANTDEF	*ap;
	WDEF	wdef[WMAX];
	int		wmax;
private:
	int GetNewWire(void);
	int AddWire(double x1, double y1, double z1, double x2, double y2, double z2, double r, int seg);
	void Connect(int w, double X, double Y, double Z);
	void Sort(void);
public:
	inline CWGroupList(void){
		gmax = 0;
		wmax = 0;
		memset(Intval, 0, sizeof(Intval));
	};
	inline ~CWGroupList(){
	};
	void Delete(void);
	double GetXPosition(int n);
	int SetXPosition(int n, double d);

	void SetWire(ANTDEF *p);
	void Update(void);
	void CreateWire(int w, int type);
	void InsertItem(int w);
	void DeleteItem(int n);
	void DeleteWire(int n);
	void SetNewType(int n, int type);
	int GetEleNo(int w);
	CWGroup* GetCW(int w);
	double GetXAbs(int w);
};

void DrawWirePara(TPaintBox *tp, CWGroupList *lp, int w);

enum AVTYPE {
	avWIRE,
	avELE,
	avLOAD,
	avHEIGHT,
	avFREQ,
	avWIREP,
	avFEED,
	avSTACK,
};
#define	AVMAX	128		// 処理できる最適化バリアブルの最大数
// 最適化バリアブルの配列構造
typedef struct {
	int		Type;		// バリアブルの形式  0-ﾜｲﾔ, 1-ｴﾚﾒﾝﾄ, 2-ﾛｰﾄﾞ, 3-地上高
	int		With;			// 関連番号(-1:関連なし, 0～ 関連付けの番号)
	int		Pos;			// 変数の位置(ワイヤ番号,ｴﾚﾒﾝﾄ番号,ﾛｰﾄﾞ番号等)
	int		SubPos;			// 変数の箇所
	USHORT	Min;
	USHORT	Max;
	int		Dummy1;
	int		Dummy2;

	double	DW;				// 可変幅
	double	DMAX;			// 最適化最大制限の値
	double	DMIN;			// 最適化最小制限の値
	double	DIO;			// 最適化の初期間隔
	double	DI;				// 最適化の実行間隔
	double	DC;				// 現在の値
}AVAL;

#define BMAX	10
typedef struct {			// 給電点の定義
	double	fq;
	CDEF	cdef[4];
}BDEF;
// 目標の構造
typedef struct {
	int		TF;				// 目標あり／なし
	double	TG;				// Ｇのトレードオフ
	double	TFB;			// ＦＢのトレードオフ
	double	TJX;			// ＪＸのトレードオフ
	double	TSWR;			// ＳＷＲのトレードオフ
	double	TELV;			// エレベーション
	double	TM;				// シンプルマッチング
	double	TI;				// 電流

	double	CG;
	double	CFB;
	double	CRR;
	double	CJX;
	double	CSWR;
	double	CELV;
	double	CM;

	double	V;				// 評価値
	double	AM;				// 前回の評価値
	double	VM;				// 最大の評価値
	double	AD[AVMAX];		// 最大時の各変数の値

	int		bmax;			// バンドの数
	int		bweight;
	BDEF	bdef[BMAX];		// バンドの定義

	double	MaxGain;		// 目標最大ゲイン
	double	MaxFB;			// 目標最大ＦＢ比
	double	MinJX;
	double	MinSWR;
	int		Match;
	double	TergR;
	double	TergJX;

	int		IType;			// 0-電流最大, 1-電流最小
	char	IPLUS[8+1];		// 電流位置
	double	CurCur;			// 現在の電流値
}ATRG;
#define	MVMAX	128			// 記録リストの数
typedef struct {			// 記憶リストの構造
	double	AD[AVMAX];			// パラメータの状態
	double	V;					// 評価値

	float	CG;					// 表示用
	float	CFB;
	float	CRR;
	float	CJX;
	float	CSWR;
	float	CELV;
	short	PtnH[180];			// 2度分解能の水平パターン
	short	PtnV[91];			// 2度分解能の垂直パターン
}MVAL;
// 最適化実行クラス
class CACal
{
public:
	AVAL	aval[AVMAX];	// 変数の配列
	int		amax;			// 登録した変数の数
	CWGroupList	WList;		// エレメントデータ
	ATRG	atrg;			// 目標のデータ
	int		PitchUnit;		// ピッチの単位
	int		DegUnit;		// パターンの分解能

	double	BV[BMAX];
	double	BVM[BMAX];

	ENVDEF	*EP;
	ANTDEF	*AP;
	CRes	maxres;			// 最大時の結果のデータ
	char	InfoTTL[192];
	char	Info[192];
private:
	MVAL	mval[MVMAX];	// 記憶リストの配列
	int		mvwp;			// 次の書き込み位置
	int		mvmax;			// 現在のデータ数
private:
	void WriteMV(void);
	int IsMV(double &max);

	void AdjWireChen(int w, WDEF *op, WDEF *np);
	void SetParaWire(AVAL *ap, double d);
	void SetParaEle(AVAL *ap, double d);
	void SetParaLoad(AVAL *ap, double d);
	void SetParaHeight(AVAL *ap);
	void SetParaWireP(AVAL *ap, double d);
	void SetParaFeed(AVAL *ap, double d);
	void SetParaStack(AVAL *ap, double d);
	double GetParaWire(AVAL *ap);
	double GetParaEle(AVAL *ap);
	double GetParaLoad(AVAL *ap);
	double GetParaWireP(AVAL *ap);
	double GetParaFeed(AVAL *ap);
	double GetParaStack(AVAL *ap);
	int SetMinMax(AVAL *ap, double &d);
	void SetPara(int n);
	int Exec(void);
//	int SetMax(RESL *rp);
	int IsSameMax(void);
	int _GetLastChen(int c, int n);
	void SetNowCur(RESL *rp);
	double CalcV(RESL *rp, CRes *cp);
public:
	CACal(void);
	inline ~CACal(){
	};

	inline int GetChenNo(int n){
		if( n >= 0 ){
			return n & 0x0000ffff;
		}
		else {
			return -n-2;
		}
	};
	int GetLastChen(int c, int n);
	int WriteToFp(FILE *fp);		// セットアップのセーブ
	int ReadFromFp(FILE *fp);		// セットアップのロード
	inline int GetMV(void){
		return mvmax;
	};
	void ReadMV(MVAL *wp, int n);

	void Init(ANTDEF *ap, ENVDEF *ep);
	int InitPara(void);
	void SetPara(int n, double d);
	void SetBand(ANTDEF *ap, int n);
	void SetTradeOff(int gain, int fb, int jx, int swr, int elv, int mch, int cur);
	int Execute(void);
	double GetNowValue(int n);
	double GetNowValue(int n, double sc);
	void AdjSetup(int n);
	int Check(void);
	void SetTradeStr(LPSTR t);
	void DispStat(TMemo *tp);
};

///-------------------------------------------------------
/// クリッピングクラス
class CClip {
private:
	int		Xl;
	int		Yt;
	int		Xr;
	int		Yb;
	inline double ClipSubV(double bx, double by, double ex, double ey, double XX){
		if( bx != ex ){
			return ey + (XX-ex)*(by-ey)/(bx-ex);
		}
		else {
			return ey;
		}
	};
	inline double ClipSubH(double bx, double by, double ex, double ey, double YY){
		if( ey != by ){
			return bx + (YY-by)*(ex-bx)/(ey-by);
		}
		else {
			return bx;
		}
	};
public:
	inline CClip(){
	};
	inline void SetClip(int x1, int y1, int x2, int y2){
		Xl = x1;
		Yt = y1;
		Xr = x2;
		Yb = y2;
	};
	int Clip(int &x1, int &y1, int &x2, int &y2);
	inline int ClipLong(long &x1, long &y1, long &x2, long &y2){
		int	xx1, xx2, yy1, yy2;
		xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
		int r = Clip(xx1, yy1, xx2, yy2);
		x1 = xx1; y1 = yy1; x2 = xx2; y2 = yy2;
		return r;
	}
	inline void Line(TCanvas *tp, int X1, int Y1, int X2, int Y2){
		if( Clip(X1, Y1, X2, Y2) == TRUE ){
			tp->MoveTo(X1, Y1);
			tp->LineTo(X2, Y2);
		}
	}
};

class CGraph
{
private:
	int		pcnt;
	int		flag[BWMAX+BWSMAX*BWSDIMMAX];
	double	pos[BWMAX+BWSMAX*BWSDIMMAX];
	double	data[BWMAX+BWSMAX*BWSDIMMAX];
    CClip	clip;
public:
	int	xl, xr, yt, yb;
	int	xw, yw;
	int	DotLen;
	double	xdl, xdh, xdw;
	double	ydl, ydh, ydw;
	double	rydl, rydh, rydw;
	double	YLIMMAX;
	TColor	gCol;
	TCanvas	*cp;
public:
	CGraph(void);
	inline ~CGraph(){
	};
	void Init(TPaintBox *pp);
	void Init(TCanvas *p, int x1, int y1, int x2, int y2);
	void ClearView(TPaintBox *pp);
	void DrawAngle(void);
	void Clear(void);
	void AddPoint(double x, double y, int fg);
	void AutoRenge(double lim);
    inline void SetYLimit(double lim){
		YLIMMAX = lim;
    };
	void AdjRengeZero(void);
	void DrawTitle(int sw, LPCSTR p, TColor col);
	void DrawGage(int sw, TColor col);
	void DrawXGage(void);
	void Plot(TColor col, TPenStyle style = psSolid);
	void DrawConst(double l, double c, double q, double w, double f);
	void DrawInfo(LPCSTR p);
	void DrawInfoL(int n, LPCSTR fmt, ...);
};


///-------------------------------------------------------
/// CRecentMenuクラス
#define	RECMENUMAX	10	// 処理できる最大の数
class CRecentMenu
{
private:
public:
	int		InsPos;		// メニュー挿入位置の番号
	int		Max;		// 処理する数
	AnsiString	Caption[RECMENUMAX];
	TMenuItem	*pMenu;
	TMenuItem	*Items[RECMENUMAX+1];

public:
	CRecentMenu();
	void Init(int pos, TMenuItem *pmenu, int max);
	void Init(TMenuItem *pi, TMenuItem *pmenu, int max);
	int GetCount(void);
	LPCSTR GetItemText(int n);
	void SetItemText(int n, LPCSTR p);
	int IsAdd(LPCSTR pNew);
	void Add(LPCSTR pNew);
	void ExtFilter(LPCSTR pExt);
	void Clear(void);
	void Insert(TObject *op, Classes::TNotifyEvent pEvent);
	void Delete(void);
	void Delete(LPCSTR pName);
	int FindItem(TMenuItem *mp);
	LPCSTR FindItemText(TMenuItem *mp);
};

class CWebRef
{
private:
	AnsiString	HTML;
public:
	CWebRef();
	inline bool IsHTML(void){
		return !HTML.IsEmpty();
	};
	void ShowHTML(LPCSTR url);
};

class CWaitCursor
{
private:
	TCursor sv;
public:
	CWaitCursor();
	~CWaitCursor();
	void Delete(void);
	void Wait(void);
};

#endif

