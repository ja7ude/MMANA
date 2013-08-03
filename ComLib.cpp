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
#include <vcl.h>
#pragma hdrstop

#include "ComLib.h"
#include "MiniNec3.h"


char	BgnDir[128];
char	AntDir[128];
char	ResDir[128];
CRes	res;
ENVDEF	env;
EXEENV	exeenv;
AnsiString	antDef;			// 最適化前のアンテナ定義
AnsiString	antSave;		// 記憶したアンテナ定義

const char	ILLFMT[]="'%s'は予期しない形式のファイルです.";

int GetFileVersion(LPCSTR pbf, LPCSTR pVer)
{
	int Ver = -1;

	if( !memcmp(pbf, pVer, 9) ){
        char	bf[16];
		bf[0] = pbf[9];
        bf[1] = pbf[11];
        bf[2] = pbf[12];
        bf[3] = 0;
		if( sscanf(bf, "%d", &Ver) != 1 ){
			Ver = 0;
        }
    }
	return Ver;
}

LPCSTR	FreqTbl[]={
	"1.825","1.910","3.510","3.550","3.800","7.010","7.050",
	"10.110","10.130","14.050","14.150","14.200","18.080","18.120",
	"21.050","21.200","24.900","24.940","28.050","28.500","29.000",
	"50.100","50.200","50.500","51.000","144.100","145.000",
    "430.100","435.000","1294.100","1295.000",
	NULL,
	"2400",
};

char LastC(LPCSTR p)
{
	char c = 0;

	if( *p ){
		c = *(p + strlen(p) - 1);
	}
	return c;
}

void SetEXT(LPSTR pName, LPSTR pExt)
{
	if( *pName ){
		LPSTR	p;

		for( p = &pName[strlen(pName)-1]; p >= pName; p-- ){
			if( *p == '.' ){
				strcpy(p, pExt);
				return;
			}
		}
		strcat(pName, pExt);
	}
}

char ZenToHan(LPCSTR s)
{
	const char tt1[]="．，＋－＊／（）＃、。＝ー";
	const char tt2[]=".,+-*/()#,.=-";

	UINT k;
	int i;

	if( _mbsbtype((const unsigned char *)s, 0) == _MBC_LEAD ){
		k = UINT(UCHAR(*s));
		k = k << 8;
		s++;
		k |= UINT(UCHAR(*s));
		if( (k >= 0x824f)&&(k <= 0x8258) ){
			k -= 0x824f;
			k += '0';
		}
		else if( (k>=0x8260)&&(k <= 0x8279) ){
			k -= 0x8260;
			k += 'A';
		}
		else if( (k >= 0x8281) && (k <= 0x829a) ){
			k -= 0x8281;
			k += 'a';
		}
		else {
			s--;
			k = ' ';
			for( i = 0; tt2[i]; i++ ){
				if( !memcmp(s, &tt1[i*2], 2) ){
					k = tt2[i];
					break;
				}
			}
		}
		return char(k);
	}
	else {
		return 0;
	}
}

/*#$%
========================================================
	文字列のコピー（半角変換付き）
--------------------------------------------------------
	t : 格納先のポインタ
	p : 文字列のポインタ
	n : コピーするサイズ
--------------------------------------------------------
	文字コード
--------------------------------------------------------
========================================================
*/
void ZSCopy(LPSTR t, LPCSTR p, int n)
{
	char c;

	for( ; *p && n; p++, t++, n-- ){
		c = ZenToHan(p);
		if( c ){
			p++;
		}
		else {
			c = *p;
		}
		*t = c;
	}
	*t = 0;
}

/*#$%
========================================================
	拡張子を得る
--------------------------------------------------------
	p : 文字列のポインタ
--------------------------------------------------------
	文字コード
--------------------------------------------------------
========================================================
*/
LPCSTR GetEXT(LPCSTR Fname)
{
	if( *Fname ){
		LPCSTR p = Fname + (strlen(Fname) - 1);
		for( ; p > Fname; p-- ){
			if( *p == '.' ) return p+1;
		}
	}
	return "";
}

void SetFileName(LPSTR pName)
{
	if( *pName ){
		int i;
		for( i = strlen(pName)-1; i > 0; i-- ){
			if( ((pName[i]== '\\')&&(_mbsbtype((unsigned const char *)pName, i-1)!=_MBC_LEAD))||(pName[i] == ':') ){
				strcpy(pName, &pName[i+1]);
				break;
			}
		}
		SetEXT(pName, "");
	}
}

LPCSTR GetFileName(LPCSTR pName)
{
	if( *pName ){
		int i;
		for( i = strlen(pName)-1; i > 0; i-- ){
			if( ((pName[i]== '\\')&&(_mbsbtype((unsigned const char *)pName, i-1)!=_MBC_LEAD))||(pName[i] == ':') ){
				return &pName[i+1];
			}
		}
	}
	return pName;
}

LPCSTR StrCLX(double r, double x)
{
	static	char	bf[64];

	sprintf( bf, "%.2lf%cj%.2lf", r, (x >= 0) ? '+' : '-', ABS(x));
	return bf;
}

LPCSTR StrCLXZ(double r, double x)
{
	static	char	bf[64];

	if( x ){
		sprintf( bf, "%.1lf%cj%.1lf", r, (x >= 0) ? '+' : '-', ABS(x));
	}
	else {
		sprintf( bf, "%.1lf", r );
	}
	return bf;
}

LPCSTR StrCLXF(double r, double x)
{
	static	char	bf[64];

	sprintf( bf, "%lf%cj%lf", r, (x >= 0) ? '+' : '-', ABS(x));
	return bf;
}

LPCSTR StrDbl(double d)
{
	static	char	bf[64];
	LPSTR	p;

	if( d == NULLV ){
		bf[0] = 0;
	}
	else {
		sprintf(bf, "%lf", d);
		for( p = &bf[strlen(bf)-1]; p >= bf; p-- ){
			if( (*p == '0')&&(*(p-1)!='.') ){
				*p = 0;
			}
			else {
				break;
			}
		}
	}
	return bf;
}

LPCSTR StrDbl3(double d)
{
	static	char	bf[32];
	int		n, l;

	if( d == NULLV ){
		bf[0] = 0;
	}
	else {
		sprintf(bf, "%.3lf", d);
		n = (d < 0.0) ? 7 : 6;
		for( l = strlen(bf) - 1; l > n; l-- ){
			if( bf[l] == '.' ) break;
			bf[l] = 0;
		}
		if( bf[l] == '.' ) bf[l] = 0;
	}
	return bf;
}

LPCSTR StrDbl2(double d)
{
	static	char	bf[32];
	int		n, l;

	if( d == NULLV ){
		bf[0] = 0;
	}
	else {
		sprintf(bf, "%.2lf", d);
		n = (d < 0.0) ? 7 : 6;
		for( l = strlen(bf) - 1; l > n; l-- ){
			if( bf[l] == '.' ) break;
			bf[l] = 0;
		}
		if( bf[l] == '.' ) bf[l] = 0;
	}
	return bf;
}

LPCSTR StrDbl1(double d)
{
	static	char	bf[32];

	if( d == NULLV ){
		bf[0] = 0;
	}
	else {
		sprintf(bf, "%.1lf", d);
	}
	return bf;
}

LPCSTR StrDblE(double d)
{
	static	char	bf[16];

	if( d == NULLV ){
		bf[0] = 0;
	}
	else if( (d == 0) || (ABS(d) >= 1e-3) ){
		return StrDbl(d);
	}
	else {
		sprintf(bf, "%.3le", d);
	}
	return bf;
}

LPCSTR StrInt(int d)
{
	static	char	bf[16];

	sprintf(bf, "%d", d);
	return bf;
}

double RoundUp(double d)
{
	char	bf[64];

	sprintf(bf, "%.3lf", d);
	sscanf(bf, "%lf", &d);
	return d;
}

double RoundUp(double d, double s)
{
	if( d >= 0.0 ){
		d *= s;
		d += 0.5;
		d = int(d);
		d = d / s;
	}
	else {
		d = -d;
		d *= s;
		d += 0.5;
		d = int(d);
		d = -(d / s);
	}
	return d;
}

double RoundUpStr(double d)
{
	char	bf[64];

	sprintf(bf, "%lf", d);
	sscanf(bf, "%lf", &d);
	return d;
}


double GetNormal(double d)
{
	if( d == 0 ) return d;
	int n;
	for( n = 0; ABS(d) < 1.0; d *= 10.0, n++ );
	d = int(d);
	if( d > 5 ) d = 10.0;
	for( ; n; n--, d /= 10.0);
	return d;
}

double GetSCNormal(double d)
{
	if( d == 0 ) return d;
	int n;
	double	sgn = SGN(d);
	d = ABS(d);
	if( d < 1.0 ){
		for( n = 0; d < 1.0; d *= 10.0, n++ );
	}
	else {
		for( n = 0; d > 10.0; d /= 10.0, n-- );
	}
	if( d > 8.0 ){
		d = 10.0;
	}
	else if( d > 4.0 ){
		d = 8.0;
	}
	else if( d > 2.0 ){
		d = 4.0;
	}
	else if( d > 1.0 ){
		d = 2.0;
	}
	else {
		d = 1.0;
	}
	if( n >= 0 ){
		for( ; n; d /= 10.0, n-- );
	}
	else {
		for( ; n < 0; d *= 10.0, n++ );
	}
	return d * sgn;
}


void SetCurDir(LPSTR t, int size)
{
	if( !::GetCurrentDirectory(size-1, t) ){
		*t = 0;
	}
	else {
		if( LastC(t) != '\\' ){
			strcat(t, "\\");
		}
	}
}

void SetDirName(LPSTR t, LPCSTR pName)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char name[_MAX_FNAME];
	char ext[_MAX_EXT];
	AnsiString	Dir;

	::_splitpath( pName, drive, dir, name, ext );
	Dir = drive;
	Dir += dir;
	strncpy(t, Dir.c_str(), 128);
}
//-----------------------------------------------------------------------
//SWRを得る
double CalcSWR(double Rl, double Xl, double Ro, double Xo)
{
	if( (Rl==NULLV)||(Xl==NULLV) ) return NULLV;
	Xl -= Xo;
	double	L = (Ro+Rl)*(Ro+Rl) + Xl*Xl;

	double	A = (Rl*Rl + Xl*Xl - Ro*Ro)/L;
	double	B = (2 * Xl * Ro)/L;
	L = sqrt(A * A + B * B);
	if( L >= 1.0 ){
		return 99999.9;
	}
	else {
		return (1 + L)/(1 - L);
	}
}

//-----------------------------------------------------------------------
//並列共振回路のインピーダンスを得る
// f:MHz, L:uH, C:pF
void CalcParaZ(double &R, double &X, double f, double L, double C, double Q)
{
	double	wl = PAI2 * f * L;
	double	wc = PAI2 * f * C * 1.0e-6;
	double	r = (Q!=0.0) ? wl/Q : 0.0;

	double	A = r / (r*r + wl*wl);
	double	B = wc - wl/(r*r + wl*wl);
	double	Y = (A*A) + (B*B);

	if( Y == 0.0 ){
		R = MAXDOUBLE;
		X = -MAXDOUBLE;
	}
	else {
		R = A / Y;
		X = -B / Y;
	}
}
//---------------------------------------------------------------------------
// 角度を計算する
double GetDeg(double x, double y)
{
	double	deg;

	if( x > 0.0 ){
		deg = atan(y/x)*180.0/PAI;
	}
	else if( x < 0.0 ){
		x = -x;
		deg = atan(y/x)*180.0/PAI;
		deg = 180.0 - deg;
	}
	else if( y > 0.0 ){
		deg = 90.0;
	}
	else if( y < 0.0 ){
		deg = -90.0;
	}
	else {
		deg = 0.0;
	}
	if( deg > 180.0 ) deg -= 360.0;
	if( deg < -180.0 ) deg += 360.0;
	return deg;
}
///----------------------------------------------------------------
///  コメントを削除（スペースおよびＴＡＢも削除）
///
void DeleteComment(LPSTR bf)
{
	LPSTR sp, tp;

	for( sp = tp = bf; *sp; sp++ ){
		if( (*sp == ';')||(*sp == CR)||(*sp == LF) ){
			break;
		}
		else if( (*sp!=' ')||(*sp!=TAB) ){
			*tp++ = *sp;
		}
	}
	*tp = 0;
}

///----------------------------------------------------------------
///  コメントを削除（スペースおよびＴＡＢも削除）
///
void ClipLF(LPSTR sp)
{
	for( ; *sp; sp++ ){
		if( (*sp == LF)||(*sp == CR) ){
			*sp = 0;
			break;
		}
	}
}

///----------------------------------------------------------------
///  ホワイトスペースのスキップ
///
LPSTR SkipSpace(LPSTR sp)
{
	for( ; *sp; sp++ ){
		if( (*sp != ' ')&&(*sp != TAB) ) break;
	}
	return sp;
}

LPCSTR SkipSpace(LPCSTR sp)
{
	for( ; *sp; sp++ ){
		if( (*sp != ' ')&&(*sp != TAB) ) break;
	}
	return sp;
}

///----------------------------------------------------------------
///  デリミッタ分解を行う
///
LPSTR StrDlm(LPSTR &tp, LPSTR sp)
{
	tp = sp;
	for( ; *sp; sp++ ){
		if( *sp == ',' ){
			*sp = 0;
			sp++;
			break;
		}
	}
	tp = SkipSpace(tp);
	return sp;
}

///----------------------------------------------------------------
///  デリミッタ分解を行う
///
LPSTR StrDlm(LPSTR &tp, LPSTR sp, char c)
{
	tp = sp;
	for( ; *sp; sp++ ){
		if( *sp == c ){
			*sp = 0;
			sp++;
			break;
		}
	}
	tp = SkipSpace(tp);
	return sp;
}

///----------------------------------------------------------------
///  エラーメッセージの表示
///
void ErrorMB(LPCSTR fmt, ...)
{
	va_list	pp;
	char	bf[1024];

	va_start(pp, fmt);
	vsprintf( bf, fmt, pp );
	va_end(pp);

	HWND hd = Screen->ActiveForm->Handle;
	if( hd == NULL ) hd = Application->Handle;
	int flag = MB_SETFOREGROUND|MB_ICONSTOP|MB_OK;
	flag |= (hd == NULL) ? MB_TASKMODAL : MB_APPLMODAL;
	::MessageBox(hd, bf, "ｴﾗｰ", flag);
}

///----------------------------------------------------------------
///  警告メッセージの表示
///
void WarningMB(LPCSTR fmt, ...)
{
	va_list	pp;
	char	bf[1024];

	va_start(pp, fmt);
	vsprintf( bf, fmt, pp );
	va_end(pp);

	HWND hd = Screen->ActiveForm->Handle;
	if( hd == NULL ) hd = Application->Handle;
	int flag = MB_SETFOREGROUND|MB_ICONEXCLAMATION|MB_OK;
	flag |= (hd == NULL) ? MB_TASKMODAL : MB_APPLMODAL;
	::MessageBox(hd, bf, "警告", flag);
}

///----------------------------------------------------------------
///  実行選択メッセージの表示
///
int YesNoMB(LPCSTR fmt, ...)
{
	va_list	pp;
	char	bf[1024];

	va_start(pp, fmt);
	vsprintf( bf, fmt, pp );
	va_end(pp);

	HWND hd = Screen->ActiveForm->Handle;
	if( hd == NULL ) hd = Application->Handle;
	int flag = MB_SETFOREGROUND|MB_ICONQUESTION|MB_YESNO;
	flag |= (hd == NULL) ? MB_TASKMODAL : MB_APPLMODAL;
	return ::MessageBox(hd, bf, "MMANA", flag);
}

void Calc3DXY(double &x, double &y, double deg, double zdeg, double X, double Y, double Z)
{
//	Xg = Y*COSθ + X*SINθ
//	Yg = Z*COSδ - ( -Y*COSθ + X*COSθ )*SINδ

	if( exeenv.Ant3D ){
		x = Y*cos(deg) + X*sin(deg);
		y = Z*cos(zdeg) - (-Y*sin(deg)+X*cos(deg))*sin(zdeg);
	}
	else {
		x = X * sin(deg) + Y;
		y = X * cos(deg) + Z;
	}
}

double ReadDoubleIniFile(TIniFile *p, LPCSTR c1, LPCSTR c2, double d)
{
	AnsiString cs = StrDbl(d);
	AnsiString rs = p->ReadString(c1, c2, cs);
	if( Calc(d, rs.c_str()) != TRUE ){
		d = NULLV;
	}
	return d;
}

void WriteDoubleIniFile(TIniFile *p, LPCSTR c1, LPCSTR c2, double d)
{
	AnsiString cs = StrDbl(d);
	p->WriteString(c1, c2, cs);
}

void AddWZ(ANTDEF *ap, double x1, double y1, double z1, double x2, double y2, double z2, double R, int SEG, int wno)
{
	WZDEF	*wp;

	if( ap->wzmax < WZMAX ){
		wp = &ap->wzdef[ap->wzmax];
		ap->wzmax++;
		wp->X1 = x1;
		wp->Y1 = y1;
		wp->Z1 = z1;
		wp->X2 = x2;
		wp->Y2 = y2;
		wp->Z2 = z2;
		wp->R = R;
		wp->SEG = SEG;
		wp->Wno = wno;
		wp->SNo = 0;
		wp->PNo = 0;
	}
}
#define	DM2DEF	0.85	// DM2分割の拡大許容範囲
///----------------------------------------------------------------
///  自動均等分割の処理を行う
void TeiAvg(ANTDEF *ap, double x1, double y1, double z1, double x2, double y2, double z2, double R, int DM, int wno)
{
	double	xx, yy, zz, d;
	int		N;

	R = ABS(R);
	xx = x2 - x1;
	yy = y2 - y1;
	zz = z2 - z1;
	d = sqrt(xx*xx + yy*yy + zz*zz);

	N = int((d / (ap->RMD/double(DM))));
	N &= 0x0000fffe;
	if( !N ) N+=2;
	if( (N <= 4) && (((d/N)*DM2DEF) > (ap->RMD/DM)) ) N+=2;
	AddWZ(ap, x1, y1, z1, x2, y2, z2, R, N, wno);
}
///----------------------------------------------------------------
///  自動テーパリングの処理を行う
void Tei(ANTDEF *ap, double x1, double y1, double z1, double x2, double y2, double z2, double R, int DM1, int wno)
{
	double	xx, yy, zz, d, x;
	double	dd, xc, yc, zc, xxc, yyc, zzc;
	double	dm1 = double((DM1 == 1)?ap->DM1:DM1);
	double	dm2 = double(ap->DM2);
	double	dm;
	int		N, NT, NL;

	R = ABS(R);
	xx = x2 - x1;
	yy = y2 - y1;
	zz = z2 - z1;
	d = sqrt(xx*xx + yy*yy + zz*zz);

	while(dm2 < dm1 ){
		for( NT = 0, dm = dm1, x = 0.0; dm > dm2; dm /= ap->SC, NT++ ){
			if( NT == 0 ){
				x += ((ap->RMD*ap->EC)/dm);
			}
			else {
				x += (ap->RMD/dm);
			}
		}
		dd = d - (x * 2);		/* 中央部分のエレメントの長さ	*/
		N = int((dd / (ap->RMD/dm2)));
		N &= 0x0000fffe;
		if( (dd > 0) && (N >= 2) ){		/* 分割可能	*/
	//		printf( "dm:%7.5f  d:%7.5f  dd:%7.5f  x:%7.5f\n", dm, d, dd, x);
			dd = dd / N;
	/*		printf( "N:%u, dd:%7.5f\n", N, dd);	*/

//			NN = 1;
	/*		printf( "%d\t%7.3f\t%7.3f\t%7.3f\n", NN++, x1, y1, z1);*/
			xc = x1, yc = y1, zc = z1;
			for( NL = 0, dm = dm1; dm > dm2; dm /= ap->SC, NL++ ){
				xxc = xc; yyc = yc; zzc = zc;
				x = ap->RMD * (NL?1:ap->EC) / dm;
				xc += (xx*x/d);
				yc += (yy*x/d);
				zc += (zz*x/d);
				AddWZ(ap, xxc, yyc, zzc, xc, yc, zc, R, NL ? 1 : ap->EC, wno);
			}
			if( N ){
				xxc = xc; yyc = yc; zzc = zc;
	//			printf( "%d\t%7.3f\t%7.3f\t%7.3f\t", NN++, xc, yc, zc);
				xc += (xx * dd * N)/d;
				yc += (yy * dd * N)/d;
				zc += (zz * dd * N)/d;
	//			printf( "%7.3f\t%7.3f\t%7.3f\t%d\n", xc, yc, zc, N);
				if( (N <= 4) && ((dd/N)*DM2DEF > (ap->RMD/dm2)) ) N+=2;
				AddWZ(ap, xxc, yyc, zzc, xc, yc, zc, R, N, wno);
			}
			dm *= ap->SC;
			for( ; NL; dm *= ap->SC, NL-- ){
				xxc = xc; yyc = yc; zzc = zc;
				x = ap->RMD / dm;
				xc += (xx*x/d);
				yc += (yy*x/d);
				zc += (zz*x/d);
				if( NL == 1 ){
					AddWZ(ap, xxc, yyc, zzc, x2, y2, z2, R, ap->EC, wno);
				}
				else {
					AddWZ(ap, xxc, yyc, zzc, xc, yc, zc, R, 1, wno);
				}
			}
			return;
		}
		dm2 *= 2;
	}
	TeiAvg(ap, x1, y1, z1, x2, y2, z2, R, dm1, wno);
}
///----------------------------------------------------------------
///  始点のみテーパリングの処理を行う
void TeiB(ANTDEF *ap, double x1, double y1, double z1, double x2, double y2, double z2, double R, int DM1, int wno)
{
	double	xx, yy, zz, d, x;
	double	dd, xc, yc, zc, xxc, yyc, zzc;
	double	dm1 = double((DM1 == 1)?ap->DM1:DM1);
	double	dm2 = double(ap->DM2);
	double	dm;
	int		N, NT;

	R = ABS(R);
	xx = x2 - x1;
	yy = y2 - y1;
	zz = z2 - z1;
	d = sqrt(xx*xx + yy*yy + zz*zz);

	while(dm2 < dm1 ){
		for( NT = 0, dm = dm1, x = 0.0; dm > dm2; dm /= ap->SC, NT++ ){
			if( NT == 0 ){
				x += ((ap->RMD*ap->EC)/dm);
			}
			else {
				x += (ap->RMD/dm);
			}
		}
		dd = d - x;			/* 均等部分のエレメントの長さ	*/
		N = int((dd / (ap->RMD/dm2)));
//		if( (dd > 0) && N && (dd >= (ap->RMD/dm)) ){	/* 分割可能	*/
		if( (dd > 0) && N ){	/* 分割可能	*/
			xc = x1, yc = y1, zc = z1;
			for( dm = dm1; dm > dm2; dm /= ap->SC ){
				xxc = xc; yyc = yc; zzc = zc;
				x = ap->RMD * ((dm==dm1)?ap->EC:1) / dm;
				xc += (xx*x/d);
				yc += (yy*x/d);
				zc += (zz*x/d);
				AddWZ(ap, xxc, yyc, zzc, xc, yc, zc, R, (dm==dm1) ? ap->EC : 1, wno);
			}
			if( ((dd/N)*DM2DEF) > (ap->RMD/ap->DM2) ) N++;
			AddWZ(ap, xc, yc, zc, x2, y2, z2, R, N, wno);
			return;
		}
		dm2 *= 1.25;
	}
	TeiAvg(ap, x1, y1, z1, x2, y2, z2, R, dm1, wno);
}
///----------------------------------------------------------------
///  始点のみテーパリングの処理を行う
void TeiE(ANTDEF *ap, double x1, double y1, double z1, double x2, double y2, double z2, double R, int DM1, int wno)
{
	double	xx, yy, zz, d, x;
	double	dd, xc, yc, zc, xxc, yyc, zzc;
	double	dm1 = double((DM1 == 1)?ap->DM1:DM1);
	double	dm2 = double(ap->DM2);
	double	dm;
	int		N, NT;

	R = ABS(R);
	xx = x2 - x1;
	yy = y2 - y1;
	zz = z2 - z1;
	d = sqrt(xx*xx + yy*yy + zz*zz);

	while(dm2 < dm1 ){
		for( NT = 0, dm = dm1, x = 0.0; dm > dm2; dm /= ap->SC, NT++ ){
			if( NT == 0 ){
				x += ((ap->RMD*ap->EC)/dm);
			}
			else {
				x += (ap->RMD/dm);
			}
		}
		dd = d - x;			/* 均等部分のエレメントの長さ	*/
		N = int((dd / (ap->RMD/dm2)));
//		if( (dd > 0) && N && (dd >= (ap->RMD/dm)) ){	/* 分割可能	*/
		if( (dd > 0) && N ){	/* 分割可能	*/
			xc = x1 + (xx*dd/d);
			yc = y1 + (yy*dd/d);
			zc = z1 + (zz*dd/d);
			if( ((dd/N)*DM2DEF) > (ap->RMD/ap->DM2) ) N++;
			AddWZ(ap, x1, y1, z1, xc, yc, zc, R, N, wno);
			dm *= ap->SC;
			for( ; NT; dm *= ap->SC, NT-- ){
				xxc = xc; yyc = yc; zzc = zc;
				x = ap->RMD / dm;
				xc += (xx*x/d);
				yc += (yy*x/d);
				zc += (zz*x/d);
				if( NT == 1 ){
					AddWZ(ap, xxc, yyc, zzc, x2, y2, z2, R, ap->EC, wno);
				}
				else {
					AddWZ(ap, xxc, yyc, zzc, xc, yc, zc, R, 1, wno);
				}
			}
			return;
		}
		dm2 *= 1.25;
	}
	TeiAvg(ap, x1, y1, z1, x2, y2, z2, R, dm1, wno);
}

void SetWZ(ANTDEF *ap, int i, WDEF *wp)
{
	if( wp->SEG > 0 ){			// 手動均等分割
		AddWZ(ap, wp->X1, wp->Y1, wp->Z1, wp->X2, wp->Y2, wp->Z2, wp->R, wp->SEG, i);
	}
	else if( wp->SEG < 0 ){		// 自動テーパリング
		switch(wp->SEG){
			case -2:
				TeiB(ap, wp->X1, wp->Y1, wp->Z1, wp->X2, wp->Y2, wp->Z2, wp->R, ap->DM1, i);
				break;
			case -3:
				TeiE(ap, wp->X1, wp->Y1, wp->Z1, wp->X2, wp->Y2, wp->Z2, wp->R, ap->DM1, i);
				break;
			default:
				if( wp->SEG < -10000 ){
					int SaveDM2 = ap->DM2;
					ap->DM2 = (wp->SEG >> 16) & 0x00007fff;
					Tei(ap, wp->X1, wp->Y1, wp->Z1, wp->X2, wp->Y2, wp->Z2, wp->R, wp->SEG & 0x00007fff, i);
					ap->DM2 = SaveDM2;
				}
				else {
					Tei(ap, wp->X1, wp->Y1, wp->Z1, wp->X2, wp->Y2, wp->Z2, wp->R, -wp->SEG, i);
				}
				break;
		}
	}
	else {						// 自動均等分割
		TeiAvg(ap, wp->X1, wp->Y1, wp->Z1, wp->X2, wp->Y2, wp->Z2, wp->R, ap->DM2, i);
	}
}

PDEF *FindPP(ANTDEF *ap, double r)
{
	int i;
	for(i = 0; i < ap->pmax; i++ ){
		if( r == ap->pdef[i].RR ) return &ap->pdef[i];
	}
	return NULL;
}

static double GetLen(PDEF *pp, int n)
{
	double d = pp->L[0];
	int i;
	for( i = 1; i <= n; i++ ){
		if( pp->L[i] ){
			d += pp->L[i] * 2;
		}
		else {
			d = 1.0e20;
			break;
		}
	}
	return d;
}

double GetWireLen(WDEF *wp)
{
	double xx = wp->X2 - wp->X1;
	double yy = wp->Y2 - wp->Y1;
	double zz = wp->Z2 - wp->Z1;
	return sqrt(xx*xx + yy*yy + zz*zz);
}

//-----------------------------------------------------------------
// 組み合わせワイヤの分割を行う
int CombWire(WDEF *tbl, ANTDEF *ap, PDEF *pp, WDEF *wp)
{
	double xx = wp->X2 - wp->X1;
	double yy = wp->Y2 - wp->Y1;
	double zz = wp->Z2 - wp->Z1;
	double d = sqrt(xx*xx + yy*yy + zz*zz);
	double dd = d;
	double	l;
	int i, j, n;

	tbl[0].X1 = wp->X1;
	tbl[0].Y1 = wp->Y1;
	tbl[0].Z1 = wp->Z1;
	n = 0;
	switch(pp->Type&1){
		case 0:		// 中央展開
			for( i = 0; (i < PPMAX); i++ ){
				if( d <= GetLen(pp, i) ) break;
			}
			if( !i ) break; 	// 第一ワイヤが丁度の時
			l = (dd - GetLen(pp, i - 1))/2;
			for( j = i - 1; j >= 0; j-- ){
				tbl[n+1].X1 = tbl[n].X2 = tbl[n].X1 + (xx * l) / dd;
				tbl[n+1].Y1 = tbl[n].Y2 = tbl[n].Y1 + (yy * l) / dd;
				tbl[n+1].Z1 = tbl[n].Z2 = tbl[n].Z1 + (zz * l) / dd;
				tbl[n].R = pp->L[j+1] ? pp->R[j+1] : pp->R[j];
				if( pp->Type & 2 ){
					tbl[n].SEG = (wp->SEG >= 0) ? wp->SEG : 0;
				}
				else {
					tbl[n].SEG = (wp->SEG == -2)||(wp->SEG == -3) ? 0 : wp->SEG;
				}
				d -= l;
				l = pp->L[j];
				n++;
			}
			for( j = 0; (j <= i) && d; j++ ){
				l = pp->L[j];
				if( (!l) || (l > d) ) l = d;
				tbl[n+1].X1 = tbl[n].X2 = tbl[n].X1 + (xx * l) / dd;
				tbl[n+1].Y1 = tbl[n].Y2 = tbl[n].Y1 + (yy * l) / dd;
				tbl[n+1].Z1 = tbl[n].Z2 = tbl[n].Z1 + (zz * l) / dd;
				tbl[n].R = pp->L[j] ? pp->R[j] : (j ? pp->R[j-1] : 0.008);
				if( pp->Type & 2 ){
					tbl[n].SEG = (wp->SEG >= 0) ? wp->SEG : 0;
				}
				else {
					tbl[n].SEG = (wp->SEG == -2)||(wp->SEG == -3) ? 0 : wp->SEG;
				}
				d -= l;
				n++;
			}
			break;
		case 1:		// 始点展開
			for( i = 0; (i < PPMAX) && d; i++ ){
				l = pp->L[i];
				if( (!l) || (l > d) ) l = d;
				tbl[n+1].X1 = tbl[n].X2 = tbl[n].X1 + (xx * l) / dd;
				tbl[n+1].Y1 = tbl[n].Y2 = tbl[n].Y1 + (yy * l) / dd;
				tbl[n+1].Z1 = tbl[n].Z2 = tbl[n].Z1 + (zz * l) / dd;
				tbl[n].R = pp->L[i] ? pp->R[i] : (i ? pp->R[i-1] : 0.008);
				if( pp->Type & 2 ){
					tbl[n].SEG = (wp->SEG >= 0) ? wp->SEG : 0;
				}
				else {
					tbl[n].SEG = (wp->SEG == -2)||(wp->SEG == -3) ? 0 : wp->SEG;
				}
				d -= l;
				n++;
			}
			break;
	}
	if( n ){			// セグメント分割の調整
		i = n - 1;
		tbl[i].X2 = wp->X2;
		tbl[i].Y2 = wp->Y2;
		tbl[i].Z2 = wp->Z2;
		double DMLen = (ap->RMD/ap->DM2);
		if( wp->SEG < 0 ){
			switch(wp->SEG){
				case -2:
					if( GetWireLen(&tbl[0]) < DMLen ){
						tbl[0].SEG = -1;
						tbl[1].SEG = -2;
					}
					else {
						tbl[0].SEG = -2;
					}
					break;
				case -3:
					if( i && (GetWireLen(&tbl[0]) < DMLen) ){
						tbl[i-1].SEG = -3;
						tbl[i].SEG = -1;
					}
					else {
						tbl[i].SEG = -3;
					}
					break;
				default:
					if( !i ){
						tbl[0].SEG = wp->SEG;
					}
					else if( pp->Type & 2 ){
						if( GetWireLen(&tbl[0]) < DMLen ){
							tbl[0].SEG = -1;
							tbl[1].SEG = (n > 3) ? -2 : -1;
						}
						else {
							tbl[0].SEG = -2;
						}
						if( i && (GetWireLen(&tbl[0]) < DMLen) ){
							tbl[i-1].SEG = (n > 3) ? -3 : -1;
							tbl[i].SEG = -1;
						}
						else {
							tbl[i].SEG = -3;
						}
					}
					break;
			}
		}
		if( !(pp->Type & 1) && (n >= 3) ){
			for( i = 0; i < n; i++ ){
				if( (!tbl[i].SEG) && (i != (n/2)) ){
					d = GetWireLen(&tbl[i]);
					int N = d/DMLen;
					if( !N ) N++;
					if( (N <= 4) && (((d/N)*DM2DEF) > DMLen) ) N++;
					tbl[i].SEG = N;
				}
			}
		}
	}
	else {
		memcpy(tbl, wp, sizeof(WDEF));
		tbl[0].R = pp->R[0];
		n = 1;
	}
	return n;
}

void SetWPZ(ANTDEF *ap, PDEF *pp, int ref, WDEF *wp)
{
	WDEF	temp[PPMAX*2+2];

	int n = CombWire(temp, ap, pp, wp);
	for( int i = 0; i < n; i++ ){
		SetWZ(ap, ref, &temp[i]);
	}
}
///----------------------------------------------------------------
///  自動ワイヤ分割の処理を行う
int SetWZ(ANTDEF *ap)
{
	WDEF	*wp;
	int		i;

	ap->MinZ = MAXDOUBLE;
	ap->wzmax = 0;
	for( i = 0; i < ap->wmax; i++ ){
		wp = &ap->wdef[i];
		wp->PNo = 0;
		wp->PMax = 0;
		if( !wp->R ) continue;
		if( ap->wzmax >= WZMAX ){
			WarningMB("内部のワイヤ展開領域を使いきりました.");
			return FALSE;
		}
		if( ap->MinZ > wp->Z1 ) ap->MinZ = wp->Z1;
		if( ap->MinZ > wp->Z2 ) ap->MinZ = wp->Z2;
		PDEF *pp;
		if( wp->R < 0.0 ){
			if( (pp = FindPP(ap, wp->R))!=NULL ){
				SetWPZ(ap, pp, i, wp);
			}
			else {
				WarningMB("R = %s の「組み合わせワイヤ」が見つかりません.", StrDbl(wp->R*1000.0));
				SetWZ(ap, i, wp);
				return FALSE;
			}
		}
		else {
			SetWZ(ap, i, wp);
		}


#if 0
		if( wp->SEG > 0 ){			// 手動均等分割
			AddWZ(ap, wp->X1, wp->Y1, wp->Z1, wp->X2, wp->Y2, wp->Z2, wp->R, wp->SEG, i);
		}
		else if( wp->SEG < 0 ){		// 自動テーパリング
			switch(wp->SEG){
				case -2:
					TeiB(ap, wp->X1, wp->Y1, wp->Z1, wp->X2, wp->Y2, wp->Z2, wp->R, ap->DM1, i);
					break;
				case -3:
					TeiE(ap, wp->X1, wp->Y1, wp->Z1, wp->X2, wp->Y2, wp->Z2, wp->R, ap->DM1, i);
					break;
				default:
					if( wp->SEG < -10000 ){
						int SaveDM2 = ap->DM2;
						ap->DM2 = (wp->SEG >> 16) & 0x00007fff;
						Tei(ap, wp->X1, wp->Y1, wp->Z1, wp->X2, wp->Y2, wp->Z2, wp->R, wp->SEG & 0x00007fff, i);
						ap->DM2 = SaveDM2;
					}
					else {
						Tei(ap, wp->X1, wp->Y1, wp->Z1, wp->X2, wp->Y2, wp->Z2, wp->R, -wp->SEG, i);
					}
					break;
			}
		}
		else {						// 自動均等分割
			TeiAvg(ap, wp->X1, wp->Y1, wp->Z1, wp->X2, wp->Y2, wp->Z2, wp->R, ap->DM2, i);
		}
#endif
	}
	return TRUE;
}
///----------------------------------------------------------------
/// セグメントの文字列を得る
LPCSTR Seg2Str(int Seg)
{
	static char	bf[32];

	if( Seg < -10000 ){
		sprintf(bf, "%d,%d", Seg & 0x00007fff, (Seg >> 16) & 0x00007fff );
	}
	else {
		sprintf(bf, "%d", Seg );
	}
	return bf;
}
///----------------------------------------------------------------
/// セグメントの値を返す
int Str2Seg(int &Seg, LPCSTR p)
{
	int	dm1, dm2;

	if( (sscanf(p, "%d,%d", &dm1, &dm2) == 2) && dm1 && dm2 && (dm1 > dm2) ){
		dm1 = ABS(dm1); dm2 = ABS(dm2);
		Seg = 0x80000000 | (dm1 | (dm2 << 16));
	}
	else if( sscanf(p, "%d", &dm1) == 1){
		Seg = dm1;
	}
	else {
		return FALSE;
	}
	return TRUE;
}

///----------------------------------------------------------------
///  パルス名からパルス番号を返す
int Str2PlusNo(ANTDEF *ap, LPCSTR s)
{
	char	bf[32];
	char	c;
	LPSTR	p;
	int		n, w, pno;

	strcpy(bf, s);
	strupr(bf);
	if( isdigit(bf[0]) ){		// 数値の時
		sscanf(bf, "%u", &pno);
	}
	else if( bf[0] == 'W' ){
		c = 'C';
		for( p = &bf[1]; *p; p++ ){
			if( !isdigit(*p) ){
				c = *p;
				*p = 0;
				p++;
				break;
			}
		}
		if( *p ){
			sscanf(p, "%d", &n);
		}
		else {
			n = 0;
		}
		if( bf[1] ){
			sscanf(&bf[1], "%u", &w);
			if( w ) w--;
		}
		else {
			// 定義エラー
			w = 0;
			pno = 0;
		}
		switch(c){
			case 'C':		// 中心
				pno = (ap->wdef[w].PMax / 2) + ap->wdef[w].PNo;
				pno += n;
				break;
			case 'B':		// 先頭
				pno = ap->wdef[w].PNo;
				pno += n;
				break;
			case 'E':		// 末尾
				pno = ap->wdef[w].PNo + ap->wdef[w].PMax;
				pno -= n;
				break;
		}
	}
	else {
		// 定義エラー
		pno = 0;
	}
	return pno;
}

///----------------------------------------------------------------
///  パルス名からパルス番号を設定する
void AdjPlusNo(ANTDEF *ap)
{
	int		i;

	// 定義ワイヤ毎のパルス数を計算する
	WZDEF	*zp;
	WDEF	*wp;
	for( zp = ap->wzdef, i = 0; i < ap->wzmax; i++, zp++ ){
		if( zp->PNo ){
			wp = &ap->wdef[zp->Wno];
			if( !wp->PNo ) wp->PNo = zp->PNo;
			wp->PMax += (zp->SMax - zp->SNo) + 1;
		}
	}

	for( i = 0; i < ap->cmax; i++ ){
		ap->cdef[i].PLUSNo = Str2PlusNo(ap, ap->cdef[i].PLUS);
	}
	for( i = 0; i < ap->lmax; i++ ){
		ap->ldef[i].PLUSNo = Str2PlusNo(ap, ap->ldef[i].PLUS);
	}
}

//---------------------------------------------------------------------------
//アンテナ定義をバイナリイメージで保存
int WriteAntToFp(ANTDEF *ap, AnsiString &rem, FILE *fp)
{
	fwrite(ap->Name, 1, sizeof(ap->Name), fp);
	fwrite(&ap->fq, 1, sizeof(ap->fq), fp);
	fwrite(&ap->wmax, 1, sizeof(ap->wmax), fp);
	if( ap->wmax ){
		fwrite(ap->wdef, 1, sizeof(WDEF) * ap->wmax, fp);
	}
	fwrite(&ap->cfq, 1, sizeof(ap->cfq), fp);
	fwrite(&ap->cmax, 1, sizeof(ap->cmax), fp);
	fwrite(&ap->cauto, 1, sizeof(ap->cauto), fp);
	if( ap->cmax ){
		fwrite(ap->cdef, 1, sizeof(CDEF) * ap->cmax, fp);
	}
	fwrite(&ap->lenb, 1, sizeof(ap->lenb), fp);
	fwrite(&ap->lmax, 1, sizeof(ap->lmax), fp);
	if( ap->lmax ){
		fwrite(ap->ldef, 1, sizeof(LDEF) * ap->lmax, fp);
	}
	fwrite(&ap->DM1, 1, sizeof(ap->DM1), fp);
	fwrite(&ap->DM2, 1, sizeof(ap->DM2), fp);
	fwrite(&ap->SC, 1, sizeof(ap->SC), fp);
	fwrite(&ap->EC, 1, sizeof(ap->EC), fp);

	fwrite(&ap->StackVT, 1, sizeof(ap->StackVT), fp);
	fwrite(&ap->StackV, 1, sizeof(ap->StackV), fp);
	fwrite(&ap->StackH, 1, sizeof(ap->StackH), fp);
	fwrite(&ap->StackVW, 1, sizeof(ap->StackVW), fp);
	fwrite(&ap->StackHW, 1, sizeof(ap->StackHW), fp);

	fwrite(&env.type, 1, sizeof(env.type), fp);
	fwrite(&env.antheight, 1, sizeof(env.antheight), fp);
	fwrite(&env.WireRoss, 1, sizeof(env.WireRoss), fp);
	fwrite(&env.RO, 1, sizeof(env.RO), fp);
	fwrite(&env.JXO, 1, sizeof(env.JXO), fp);
	fwrite(&env.fbr, 1, sizeof(env.fbr), fp);

	fwrite(&ap->pmax, 1, sizeof(ap->pmax), fp);
	fwrite(&ap->pdef, 1, sizeof(ap->pdef), fp);

	int len = strlen(rem.c_str());
	fwrite(&len, 1, sizeof(len), fp);
	if( len ){
		fwrite(rem.c_str(), 1, len, fp);
	}
	return ferror(fp) ? FALSE : TRUE;
}
//---------------------------------------------------------------------------
//アンテナ定義をバイナリイメージで保存
int ReadAntFromFp(ANTDEF *ap, AnsiString &rem, FILE *fp)
{
	fread(ap->Name, 1, sizeof(ap->Name), fp);
	fread(&ap->fq, 1, sizeof(ap->fq), fp);
	fread(&ap->wmax, 1, sizeof(ap->wmax), fp);
	if( ap->wmax ){
		fread(ap->wdef, 1, sizeof(WDEF) * ap->wmax, fp);
	}
	fread(&ap->cfq, 1, sizeof(ap->cfq), fp);
	fread(&ap->cmax, 1, sizeof(ap->cmax), fp);
	fread(&ap->cauto, 1, sizeof(ap->cauto), fp);
	if( ap->cmax ){
		fread(ap->cdef, 1, sizeof(CDEF) * ap->cmax, fp);
	}
	fread(&ap->lenb, 1, sizeof(ap->lenb), fp);
	fread(&ap->lmax, 1, sizeof(ap->lmax), fp);
	if( ap->lmax ){
		fread(ap->ldef, 1, sizeof(LDEF) * ap->lmax, fp);
	}

	fread(&ap->DM1, 1, sizeof(ap->DM1), fp);
	fread(&ap->DM2, 1, sizeof(ap->DM2), fp);
	fread(&ap->SC, 1, sizeof(ap->SC), fp);
	fread(&ap->EC, 1, sizeof(ap->EC), fp);

	fread(&ap->StackVT, 1, sizeof(ap->StackVT), fp);
	fread(&ap->StackV, 1, sizeof(ap->StackV), fp);
	fread(&ap->StackH, 1, sizeof(ap->StackH), fp);
	fread(&ap->StackVW, 1, sizeof(ap->StackVW), fp);
	fread(&ap->StackHW, 1, sizeof(ap->StackHW), fp);

	fread(&env.type, 1, sizeof(env.type), fp);
	fread(&env.antheight, 1, sizeof(env.antheight), fp);
	fread(&env.WireRoss, 1, sizeof(env.WireRoss), fp);
	fread(&env.RO, 1, sizeof(env.RO), fp);
	fread(&env.JXO, 1, sizeof(env.JXO), fp);
	fread(&env.fbr, 1, sizeof(env.fbr), fp);

	fread(&ap->pmax, 1, sizeof(ap->pmax), fp);
	fread(&ap->pdef, 1, sizeof(ap->pdef), fp);

	int len;
	fread(&len, 1, sizeof(len), fp);
	if( len ){
		LPSTR bp = new char[len+1];
		fread(bp, 1, len, fp);
		bp[len] = 0;
		rem = bp;
		delete bp;
	}
	return ferror(fp) ? FALSE : TRUE;
}

// 電卓を実行する
int DoCalc(void)
{
	if( ::WinExec("Calc.Exe", SW_SHOW) < 32 ){
		char bf[256];
		::GetWindowsDirectory(bf, 222);
		if( LastC(bf) != '\\' ) strcat(bf, "\\");
		strcat(bf, "Calc.Exe");
		if( ::WinExec(bf, SW_SHOW) < 32 ){
			ErrorMB("電卓(Calc.Exe)は実行できません.");
			return FALSE;
		}
	}
	return TRUE;
}
///----------------------------------------------------------------
// 他のワイヤの接続を自動連携させる
void AdjWireChen(WDEF *wp, double ox, double oy, double oz, double nx, double ny, double nz)
{
	if( (ox==wp->X1)&&(oy==wp->Y1)&&(oz==wp->Z1) ){
		wp->X1 = nx;
		wp->Y1 = ny;
		wp->Z1 = nz;
	}
	else if( (ox==wp->X2)&&(oy==wp->Y2)&&(oz==wp->Z2) ){
		wp->X2 = nx;
		wp->Y2 = ny;
		wp->Z2 = nz;
	}
}
///----------------------------------------------------------------
// 他のワイヤの接続を自動連携させる
void AdjWireChen(WDEF *wp, int wmax, double ox, double oy, double oz, double nx, double ny, double nz)
{
	for( int i = 0; i < wmax; i++, wp++ ){
		if( (ox==wp->X1)&&(oy==wp->Y1)&&(oz==wp->Z1) ){
			wp->X1 = nx;
			wp->Y1 = ny;
			wp->Z1 = nz;
		}
		else if( (ox==wp->X2)&&(oy==wp->Y2)&&(oz==wp->Z2) ){
			wp->X2 = nx;
			wp->Y2 = ny;
			wp->Z2 = nz;
		}
	}
}
void AdjWireChen(WDEF *wp, int wmax, WDEF *np, WDEF *op)
{
	if( (np->X1 != op->X1) || (np->Y1 != op->Y1) || (np->Z1 != op->Z1) ){
		AdjWireChen(wp, wmax, op->X1, op->Y1, op->Z1, np->X1, np->Y1, np->Z1);
	}
	if( (np->X2 != op->X2) || (np->Y2 != op->Y2) || (np->Z2 != op->Z2) ){
		AdjWireChen(wp, wmax, op->X2, op->Y2, op->Z2, np->X2, np->Y2, np->Z2);
	}
}
//---------------------------------------------------------------
//極座標データを得る
void GetWirePoll(double &WL, double &XYD, double &YZD, WDEF *wp)
{
	double XX = wp->X2 - wp->X1;
	double YY = wp->Y2 - wp->Y1;
	double ZZ = wp->Z2 - wp->Z1;
	WL = sqrt(XX*XX + YY*YY + ZZ*ZZ);
	XYD = GetDeg(XX, YY);
	YZD = GetDeg(ZZ, sqrt(XX*XX + YY*YY));
	while( XYD > 180.0 ) XYD -= 360.0;
	while( XYD < -180.0 ) XYD += 360.0;
	while( YZD > 180.0 ) YZD -= 360.0;
	while( YZD < -180.0 ) YZD += 360.0;
}

//---------------------------------------------------------------
//ポイントが近くかどうか判定する
int IsInside(POINT &pnt, RECT &rc)
{
	if( rc.left < rc.right ){
		if( pnt.x < rc.left ) return FALSE;
		if( pnt.x > rc.right ) return FALSE;
	}
	else {
		if( pnt.x > rc.left ) return FALSE;
		if( pnt.x < rc.right ) return FALSE;
	}
	if( rc.top > rc.bottom ){
		if( pnt.y > rc.top ) return FALSE;
		if( pnt.y < rc.bottom ) return FALSE;
	}
	else {
		if( pnt.y < rc.top ) return FALSE;
		if( pnt.y > rc.bottom ) return FALSE;
	}
	return TRUE;
}

int IsNear(POINT &pnt, POINT &tpnt, int Width)
{
	RECT	rc;
	rc.left = tpnt.x - Width;
	rc.right = tpnt.x + Width;
	rc.top = tpnt.y + Width;
	rc.bottom = tpnt.y - Width;
	return IsInside(pnt, rc);
}

int IsLine(POINT &pnt, POINT &spnt, POINT &tpnt, int Width)
{
	int		Xw = tpnt.x - spnt.x;
	int		Yw = tpnt.y - spnt.y;
	double	A, B;
	int		C;

	if( IsNear(pnt, spnt, Width)==TRUE ){
		return 1;		/* 始点と一致	*/
	}
	else if( IsNear(pnt, tpnt, Width)==TRUE ){
		return 2;		/* 終点と一致	*/
	}
	if( spnt.x > tpnt.x ){		/* X方向の入れ替え	*/
		POINT	bpnt = spnt;
		spnt = tpnt;
		tpnt = bpnt;
	}
	if( (pnt.x >= (spnt.x-Width)) && (pnt.x <= (tpnt.x+Width)) ){
		if( Xw ){
			A = double(Yw)/double(Xw);
			B = double(spnt.y) - (A * double(spnt.x));
			C = int((A * double(pnt.x)) + B);
			A = A < 0.0 ? -A : A;
			if( A >= 1.0 ){
				Width = int(double(Width) * A);
			}
			if( (pnt.y < (C+Width)) && (pnt.y >= (C-Width)) ){
				return 3;
			}
		}
		else if( spnt.y > tpnt.y ){
			if( (pnt.y < spnt.y) && (pnt.y > tpnt.y) ){
				return 3;
			}
		}
		else if( (pnt.y > spnt.y) && (pnt.y < tpnt.y) ){
			return 3;
		}
	}
	return 0;
}

int IsLine(POINT &pnt, POINT &spnt, POINT &tpnt)
{
	int r = IsLine(pnt, spnt, tpnt, 4);
	if( (r == 1)||(r == 2) ) return r;
	return IsLine(pnt, spnt, tpnt, 10) ? 3 : 0;
}
//---------------------------------------------------------------------------
// 波長換算の値を得る
double GetRmdVal(double d)
{
	if( exeenv.RmdSel ){
		d = d / exeenv.rmd;
	}
	return d;
}
//---------------------------------------------------------------------------
// 波長換算の値を得る
double SetRmdVal(double d)
{
	if( exeenv.RmdSel ){
		d = d * exeenv.rmd;
	}
	return d;
}
//---------------------------------------------------------------------------
// 波長換算の値を得る(値はmm)
double GetRmdValD(double d)
{
	if( exeenv.RmdSel ){
		d = (d * 0.001)/exeenv.rmd;
	}
	return d;
}
//---------------------------------------------------------------------------
// 波長換算の値を得る（返す値はmm）
double SetRmdValD(double d)
{
	if( exeenv.RmdSel ){
		d = d * 1000.0 * exeenv.rmd;
	}
	return d;
}

///----------------------------------------------------------------
///  式の計算
typedef struct {
	int		pr;			/* 優先順位	*/
	char	cd;			/* 演算ｺｰﾄﾞ	*/
	double	d;
}VSS;
static int	_err;

/*#$%
========================================================
	演算子の優先順位を返す
--------------------------------------------------------
	c  : 演算子
--------------------------------------------------------
--------------------------------------------------------
========================================================
*/
static int _cpr(char c)
{
	switch(c){
		case 0:
			return(0);
		case '+':
			return(1);
		case '-':
			return(1);
		case '*':
			return(2);
		case '/':
			return(2);
		default:
			return(-1);
	}
}

/*#$%
========================================================
	演算子かどうかを返す
--------------------------------------------------------
	c  : 演算子
--------------------------------------------------------
--------------------------------------------------------
========================================================
*/
static int _iscd(char c)
{
	if( c == 0 ) return(0);
	return( (_cpr(c) == -1) ? 0 : 1 );
}


/*#$%
========================================================
	演算子と値を式管理データに格納する
--------------------------------------------------------
	vs : 式管理データのポインタ
	p  : 文字列のポインタ
--------------------------------------------------------
	文字列のポインタ
--------------------------------------------------------
========================================================
*/
static LPCSTR _val(VSS *vs, LPCSTR p)
{
	char	*t, bf[80];

	vs->cd = 0;
	vs->pr = 0;
	for( t = bf; *p; p++ ){
		if( _iscd(*p) && (t!=bf) && ( (t == bf)||((*(p-1)!='E')&&(*(p-1)!='e')) ) ){
			vs->cd = *p;
			vs->pr = _cpr(*p);
			p++;
			break;
		}
		else if( *p == ')' ){
			p++;
			break;
		}
		else if( (*p != ' ')&&(*p != TAB) ){
			*t = *p;
			t++;
		}
	}
	*t = 0;
	if( bf[0] == 0 ){
		vs->d = 0.0;
	}
	else if( !strcmpi(bf, "R") ){
		vs->d = exeenv.rmd;
	}
	else if( !strcmpi(bf, "I") ){
		vs->d = 0.0254;
	}
	else if( !strcmpi(bf, "F") ){
		vs->d = 0.0254 * 12;
	}
	else if( sscanf(bf, "%lf", &vs->d ) != 1 ){
		_err = FALSE;
	}
	return(p);
}

/*#$%
========================================================
	演算子に従って計算を行う
--------------------------------------------------------
	vp : 式管理データのポインタ
	vw : 式管理データのポインタ
--------------------------------------------------------
--------------------------------------------------------
========================================================
*/
static void _cop(VSS *vs, VSS *vw)
{
	switch(vs->cd){
		case '+':
			vs->d += vw->d;
			break;
		case '-':
			vs->d -= vw->d;
			break;
		case '*':
			vs->d *= vw->d;
			break;
		case '/':
			if( vw->d ){
				vs->d /= vw->d;
			}
			else {
				vs->d = MAXDOUBLE;
			}
			break;
	}
}

/*#$%
========================================================
	式計算処理
--------------------------------------------------------
	vp : 式管理データのポインタ
	p  : 文字列のポインタ
--------------------------------------------------------
	文字列のポインタ
--------------------------------------------------------
========================================================
*/
static LPCSTR _calc(VSS *vs, LPCSTR p)
{
	VSS		vw;

	p = SkipSpace(p);
	if( *p ){
		if( *p == '(' ){
			p++;
			vw.d = 0;
			vw.pr = 0;
			vw.cd = '+';
			p = _calc(&vw, p);
			if( *p && _iscd(*p) ){
				vw.cd = *p;
				vw.pr = _cpr(*p);
				p++;
			}
		}
		else {
			p = _val(&vw, p);
		}
		if( vw.pr == 0 ){
			_cop(vs, &vw);
			p = SkipSpace(p);
			if( _iscd(*p) ){
				vs->cd = *p;
				vs->pr = _cpr(*p);
				p++;
			}
		}
		else if( vs->pr >= vw.pr ){
			_cop(vs, &vw);
			vs->pr = vw.pr;
			vs->cd = vw.cd;
			p = _calc(vs, p);
		}
		else {
			p = _calc(&vw, p);
			_cop(vs, &vw);
			vs->pr = vw.pr;
			vs->cd = vw.cd;
		}
	}
	return(p);
}

/*#$%
========================================================
	式計算処理
--------------------------------------------------------
	d  : 計算結果格納位置のポインタ
	p  : 文字列のポインタ
--------------------------------------------------------
	ERR
--------------------------------------------------------
========================================================
*/
int Calc(double &d, LPCSTR p)
{
	VSS		vs;
	char	bf[256];

	d = 0;
	ZSCopy(bf, p, sizeof(bf)-1);
	_err = TRUE;
	vs.d = 0;
	vs.pr = 0;
	vs.cd = '+';
	_calc(&vs, bf);
	d = vs.d;
	return(_err);
}
/*#$%
========================================================
	式計算処理
--------------------------------------------------------
	d  : 計算結果格納位置のポインタ
	p  : 文字列のポインタ
--------------------------------------------------------
	ERR
--------------------------------------------------------
========================================================
*/
int CalcI(int &d, LPCSTR p)
{
	double	dd;
	int r = Calc(dd, p);
	if( dd >= 0 ){
		d = int(dd + 0.5);
    }
    else {
		d = int(dd - 0.5);
    }
	return r;
}
/*#$%
========================================================
	式計算処理
--------------------------------------------------------
	d  : 計算結果格納位置のポインタ
	p  : 文字列のポインタ
--------------------------------------------------------
	ERR
--------------------------------------------------------
========================================================
*/
int CalcU(int &d, LPCSTR p)
{
	double	dd;
	int r = Calc(dd, p);
	d = int(ABS(dd)+0.5);
	return r;
}
///----------------------------------------------------------------
///  電流ファイルデータの作成
void WriteCur(ANTDEF *ap, int W, int P, float I, float IJ, float Mag, float Deg)
{
	if( exeenv.pCurFile == NULL ) return;
	W--;
	if( P < 0 ){
		if( W < ap->wzmax ){
			if( ap->wzdef[W].PNo ){	// パルスが存在する時
				P = ap->wzdef[W].PNo;
				if( (P < 1) || (P > env.pmax) || exeenv.pCurFile[P-1].Flag ){
					P = -1;
				}
			}
		}
	}
	if( P < 0 ) return;
	int i;
	for( i = 0; i < ap->wzmax; i++ ){
		if( P < ap->wzdef[i].PNo ) break;
	}
	i--;
	while( !ap->wzdef[i].PNo ) i--;
	if( i < 0 ) i = 0;
	int S = ap->wzdef[i].SNo + (P - ap->wzdef[i].PNo);
	P--;
	if( (P >= 0) && (P < env.pmax) ){
		double	x, y, z;
		GetSegPosReal(x, y, z, S);
		W = ap->wzdef[W].Wno + 1;
		exeenv.pCurFile[P].Flag = 1;
		exeenv.pCurFile[P].W = W;
		exeenv.pCurFile[P].X = x;
		exeenv.pCurFile[P].Y = y;
		exeenv.pCurFile[P].Z = z;
		exeenv.pCurFile[P].I = I;
		exeenv.pCurFile[P].IJ = IJ;
		exeenv.pCurFile[P].MAG = Mag;
		exeenv.pCurFile[P].DEG = Deg;
	}
}
///----------------------------------------------------------------
///  クリッピング
void ClipXY(int &x, int &y)
{
	if( x > 32000 ) x = 32000;
	if( x < -32000 ) x = -32000;
	if( y > 32000 ) y = 32000;
	if( y < -32000 ) y = -32000;
}
///----------------------------------------------------------------
///  近似計算を行う
double Teira(double x, const double *p, int n)
{
	double y = *p++;
	int i;
	for( i = 1; i < n; i++, p++ ){
		y += pow(x, i)*(*p);
	}
	return y;
}
///----------------------------------------------------------------
///  ワイヤのロスを計算する
double GetWireRoss(ANTDEF *ap, WDEF *wp)
{
	if( wp->R < 0.0 ){
		PDEF *pp = FindPP(ap, wp->R);
		if( pp != NULL ){
			WDEF	temp[PPMAX*2+2];
			int n = CombWire(temp, ap, pp, wp);
			double d = 0.0;
			int i;
			for( wp = temp, i = 0; i < n; i++, wp++ ){
				d += GetWireRoss(ap->cfq, wp);
			}
			return d;
		}
	}
	return GetWireRoss(ap->cfq, wp);
}

double GetWireRoss(double f, WDEF *wp)
{
	double xx = wp->X2 - wp->X1;
	double yy = wp->Y2 - wp->Y1;
	double zz = wp->Z2 - wp->Z1;
	double l = sqrt(xx*xx + yy*yy + zz*zz);
	double s = wp->R * wp->R * PAI;
	double LO;		// ρ
	double US;		// μS
	//	s = 1e-3 * sqrt(f) * s * 0.83 / (wp->R * 100.0);
	switch(env.WireRoss){
		case 1:			// 銅線
		case 2:			// 銅パイプ
			LO = 1.8e-8;
			US = 1-0.96e-5;
			break;
		case 3:			// アルミ線
		case 4:			// アルミパイプ
			LO = 2.8e-8;
			US = 1+2.2e-5;
			break;
		case 5:			// 鉄線
		case 6:			// 鉄パイプ
			LO = 9.4e-8;
			US = 150;
			break;
	}
	double	r, x, t;
	if( (env.WireRoss - 1) & 1 ){		// パイプ
		t = wp->R * 0.9;
		s -= (t * t * PAI);
		r = l * LO/s;
		x = 0.1985 * wp->R * 10 * sqrt(US*f*1e6/(LO*1e8));
		if( x >= 4.0 ){
			r *= x;
		}
		else if( x >= 1.0 ){
			const double _tt[]={	/*1.0 - 4.0*/
				-4.772531e-01,
				5.851077e+00,
				-9.276207e+00,
				7.476381e+00,
				-3.116668e+00,
				7.079749e-01,
				-8.326961e-02,
				3.965883e-03,
			};
			r *= Teira(x, _tt, 8);
		}
		else if( x >= 0.3 ){
			const double _tt[]={		/* 0.3 - 1.0*/
				3.760000e-01,
				8.423095e+00,
				-4.696111e+01,
				1.402500e+02,
				-2.425694e+02,
				2.437500e+02,
				-1.319444e+02,
				2.976190e+01,
			};
			r *= Teira(x, _tt, 8);
		}
	}
	else {								// 線
		r = l * LO/s;
		x = 28.1 * wp->R * sqrt(US*f*1e6/(LO*1e8));
		if( x >= 100.0 ){
			r *= x * 0.3561;
		}
		else if( x >= 10.0 ){
			const double _tt[]={	/* 10.0 - 100.0	*/
				-2.120000e+00,
				1.004767e+00,
				-7.089785e-02,
				4.119507e-03,
				-1.430243e-04,
				3.112024e-06,
				-4.280139e-08,
				3.612087e-10,
				-1.706349e-12,
				3.452932e-15,
			};
			r *= Teira(x, _tt, 10);
		}
		else if( x >= 1.0 ){
			const double _tt[]={	/* 1.0 - 10.0	*/
				1.384010e+00,
				-1.388426e+00,
				2.087645e+00,
				-1.712435e+00,
				8.488221e-01,
				-2.589923e-01,
				5.010682e-02,
				-6.182203e-03,
				4.721516e-04,
				-2.036364e-05,
				3.794671e-07,
			};
			r *= Teira(x, _tt, 11);
		}
	}
	return r;
}
///----------------------------------------------------------------
///	ウィンドウをクライアントセンターにする
void FormCenter(TForm *tp, int XW, int YW)
{
	int top = (YW - tp->Height)/2;
	int left = (XW - tp->Width)/2;
	if( top < 0 ) top = 0;
	if( left < 0 ) left = 0;
	tp->Top = top;
	tp->Left = left;
}

void DrawWirePara(TPaintBox *tp, WDEF *wp, int w)
{
	char	bf[64], bf2[32];

	double	WL, XYD, YZD;
	GetWirePoll(WL, XYD, YZD, wp);
	int RW = tp->Canvas->TextWidth("W")*22;
	int FH = tp->Canvas->TextHeight("W");
	TRect rc;
	rc.Top = tp->Height - (FH * 12);
	rc.Bottom = tp->Height - 3;
	rc.Left = tp->Width - RW - 9;
	rc.Right = rc.Left + RW + 6;
	int Y = rc.Top + (FH/2);
	int X = rc.Left + 3;
	tp->Canvas->Pen->Width = 1;
	tp->Canvas->Pen->Color = clBlack;
	tp->Canvas->Rectangle(rc.Left, rc.Top, rc.Right, rc.Bottom);

	sprintf(bf, "Wire No.%d", w + 1);
	tp->Canvas->TextOut(X, Y, bf); Y+=FH;
	sprintf(bf, "X1   : %s m", StrDbl(wp->X1));
	tp->Canvas->TextOut(X, Y, bf); Y+=FH;
	sprintf(bf, "Y1   : %s m", StrDbl(wp->Y1));
	tp->Canvas->TextOut(X, Y, bf); Y+=FH;
	sprintf(bf, "Z1   : %s m", StrDbl(wp->Z1));
	tp->Canvas->TextOut(X, Y, bf); Y+=FH;
	sprintf(bf, "X2   : %s m", StrDbl(wp->X2));
	tp->Canvas->TextOut(X, Y, bf); Y+=FH;
	sprintf(bf, "Y2   : %s m", StrDbl(wp->Y2));
	tp->Canvas->TextOut(X, Y, bf); Y+=FH;
	sprintf(bf, "Z2   : %s m", StrDbl(wp->Z2));
	tp->Canvas->TextOut(X, Y, bf); Y+=FH;
	strcpy(bf2, wp->R < 0 ? "(組合せ)" : wp->R ? "mm" : "(絶縁)");
	sprintf(bf, "R    : %s %s", StrDbl(wp->R*1000.0), bf2);
	tp->Canvas->TextOut(X, Y, bf); Y+=FH;

	sprintf(bf, "長さ : %s m", StrDbl(WL));
	tp->Canvas->TextOut(X, Y, bf); Y+=FH;
	sprintf(bf, "動径 : %s °", StrDbl(XYD));
	tp->Canvas->TextOut(X, Y, bf); Y+=FH;
	sprintf(bf, "天頂 : %s °", StrDbl(YZD));
	tp->Canvas->TextOut(X, Y, bf);
}

void DrawWirePara(TPaintBox *tp, ANTDEF *ap, int w)
{
	if( w < ap->wmax ){
		DrawWirePara(tp, &ap->wdef[w], w);
    }
}

void DrawWirePara(TPaintBox *tp, CWGroupList *lp, int w)
{

	if( w < lp->wmax ){
		int i, j;
		WDEF	wd;
	    WREF	*rp;

		for( i = 0; i < lp->gmax; i++ ){
			for( j = 0, rp = lp->List[i].pB; j < lp->List[i].bcnt; j++, rp++ ){
				if( rp->ref == w ){
					memcpy(&wd, &lp->wdef[w], sizeof(wd));
                    double xf = lp->GetXPosition(i) + lp->List[0].GetX();
                    wd.X1 += xf;
                    wd.X2 += xf;
                    DrawWirePara(tp, &wd, w);
                    return;
                }
            }
		}
    }
}

///----------------------------------------------------------------
///	ワイヤ組み合わせデータの調整
void AdjPdef(ANTDEF *ap)
{
	int i, j;

	for( i = 0; i < ap->pmax; i++ ){
		PDEF *pp = &ap->pdef[i];
		if( !pp->RR ){
			ap->pmax = i;
			return;
		}
		for( j = 0; j < PPMAX; j++ ){
			if( !pp->L[j] ){
				if( j ) j--;
				pp->L[j] = 99999.9;
				if( !pp->R[j] ) pp->R[j] = j ? pp->R[j-1] : 0.0008;
				break;
			}
			else if( j == (PPMAX - 1) ){
				pp->L[j] = 99999.9;
			}
			if( !pp->R[j] ) pp->R[j] = j ? pp->R[j-1] : 0.0008;
		}
	}
}
///----------------------------------------------------------------
///	複素数演算クラス
void CLX::Mul(const CLX &ref)
{
	double R, J;
	R = r * ref.r - j * ref.j;
	J = r * ref.j + j * ref.r;
	r = R;
	j = J;
}

void CLX::Div(const CLX &ref)
{
	double R, J, D;
	R = r * ref.r + j * ref.j;
	J = j * ref.r - r * ref.j;
	D = ref.r * ref.r + ref.j * ref.j;
	r = R/D;
	j = J/D;
}

CLX operator+(const double R, const CLX &ref)
{
	CLX a(R);
	a+=ref;
	return a;
}
CLX operator-(const double R, const CLX &ref)
{
	CLX a(R);
	a-=ref;
	return a;
}
CLX operator*(const double R, const CLX &ref)
{
	CLX a(R);
	a*=ref;
	return a;
}
CLX operator/(const double R, const CLX &ref)
{
	CLX a(R);
	a/=ref;
	return a;
}
///----------------------------------------------------------------
///  CResクラス
void CRes::IncResP(void)
{
	RWp++;
	if( RWp >= RMAX ) RWp = 0;
	RCnt++;
	if( RCnt > RMAX ) RCnt = RMAX;
	Res[RWp].HEIGHT = NULLF;
	Res[RWp].FQ = NULLF;
	Res[RWp].R = NULLF;
	Res[RWp].JX = NULLF;
	Res[RWp].SWR = NULLF;
	Res[RWp].GAIN = NULLF;
	Res[RWp].FB = NULLF;
	Res[RWp].ELV = NULLF;
	Res[RWp].FBR = env.fbr;
	Res[RWp].RO = env.RO;
	Res[RWp].JXO = env.JXO;
	Res[RWp].MAXVP = NULLF;
	Res[RWp].MAXHP = NULLF;
}

RESL* CRes::GetResP(int Index)
{
	Index = RWp - Index;
	if( Index < 0 ) Index += RMAX;
	return &Res[Index];
}

void CRes::SetPtn(float s1, float s2, float s3)
{
	switch(DegF){
		case 0:		// 水平パターン
			PtnH[0][Voff] = s1; 	// 垂直偏波
			PtnH[1][Voff] = s2;		// 水平偏波
			PtnH[2][Voff] = s3;		// 合算値
			break;
		case 1:		// 垂直パターン
			PtnV[0][Voff] = s1;
			PtnV[1][Voff] = s2;
			PtnV[2][Voff] = s3;
			break;
		case 2:		// 垂直詳細パターン
			if( MaxEV < s3 ){
				PtnV[0][DegV] = s1;
				PtnV[1][DegV] = s2;
				PtnV[2][DegV] = s3;
				MaxEV = s3;
				Elv = double(Voff)/10.0;
			}
			break;
	}
	Voff++;
}

int CRes::GetHMAX(void)
{
	double F = -999999;
	DegH = 0;
	int N = 360/PtnF;
	for( int i = 0; i < N; i++ ){
		if( F < PtnH[2][i] ){
			F = PtnH[2][i];
			DegH = i;
		}
	}
	return DegH * PtnF;
}

int CRes::GetVMAX(void)
{
	MaxV = -999999;
	DegV = 0;
	int N = 180/PtnF;
	for( int i = 0; i <= N; i++ ){
		if( (MaxV < PtnV[2][i]) || ((i>(N/2))&&(MaxV==PtnV[2][i])) ){
			MaxV = PtnV[2][i];
			DegV = i;
		}
	}
	return DegV * PtnF;
}

double CRes::GetAdjElv(void)
{
	double vv = GetElv();
	if( !env.type && exeenv.FixFreeAngle ){
		vv = 0.0;
	}
	else {
		if( vv >= 87.0 ) vv = 45.0;
	}
	ElvHPtn = vv;
	return vv;
}

void CRes::CalcPtn(void)
{
	int i;
	MaxVP = PtnH[0][0];
	MaxHP = PtnH[1][0];
	MaxH = PtnH[2][0];
	DegH = 0;
	int N = 360/PtnF;
	for( i = 1; i < N; i++ ){
		if( MaxVP < PtnH[0][i] ){
			MaxVP = PtnH[0][i];
		}
		if( MaxHP < PtnH[1][i] ){
			MaxHP = PtnH[1][i];
		}
		if( MaxH < PtnH[2][i] ){
			MaxH = PtnH[2][i];
			DegH = i;
		}
	}
	MaxG = MaxV;
	if( MaxG < MaxEV ) MaxG = MaxEV;
	if( MaxG < MaxH ) MaxG = MaxH;
	Res[RWp].GAIN = MaxG;
	CalcFB();
	if( Elv >= 90 ) Elv = 180 - Elv;
#if 0
	ElvHPtn = Elv;
	if( !env.type && env.FixFreeAngle ){
		ElvHPtn = 0.0;
	}
	else {
		if( ElvHPtn >= 87.0 ) ElvHPtn = 45.0;
	}
#endif
	Res[RWp].ELV = Elv;
	Res[RWp].MAXVP = MaxVP;
	Res[RWp].MAXHP = MaxHP;
}

// ゲインの再計算
void CRes::ReCalcGain(void)
{
	int i;
	MaxVP = PtnH[0][0];
	MaxHP = PtnH[1][0];
	MaxH = PtnH[2][0];
	DegH = 0;
	int N = 360/PtnF;
	for( i = 1; i < N; i++ ){
		if( MaxVP < PtnH[0][i] ){
			MaxVP = PtnH[0][i];
		}
		if( MaxHP < PtnH[1][i] ){
			MaxHP = PtnH[1][i];
		}
		if( MaxH < PtnH[2][i] ){
			MaxH = PtnH[2][i];
			DegH = i;
		}
	}
	MaxG = MaxV;
	if( MaxG < MaxEV ) MaxG = MaxEV;
	if( MaxG < MaxH ) MaxG = MaxH;
	Res[RWp].GAIN = MaxG;
	CalcFB();
	Res[RWp].MAXVP = MaxVP;
	Res[RWp].MAXHP = MaxHP;
}

void CRes::CalcFB(void)
{
	int i;

	float BD = PtnH[2][180/PtnF];
	if( env.fbr ){					// バック側の範囲の最大を得る
		int fbrh = env.fbr & 0x0000ffff;
		int fbrv = (env.fbr >> 16) & 0x0000ffff;
		// 水平のチェック
		int n = (180 - (fbrh/2))/PtnF;
		for( i = 0; i <= fbrh; i+=PtnF, n++ ){
			if( BD < PtnH[2][n] ) BD = PtnH[2][n];
		}
		// 垂直のチェック
		for( i = n = 0; i <= fbrv; i+=PtnF, n++ ){
			if( BD < PtnV[2][n] ) BD = PtnV[2][n];
		}
	}
	Res[RWp].FB = PtnH[2][0] - BD;
}

void CRes::InitBWC(void)
{
	memset(&Bwc.bw, 0, sizeof(Bwc.bw));
	Bwc.bcnt = 0;
	Bwc.fo = RoundUp(Res[RWp].FQ, 1000);
	Bwc.bi = GetNormal(Bwc.fo * 0.005);
	Bwc.fw = Bwc.bi * 4;
	int i;
	double f = Bwc.fo - (Bwc.bi*2);
	for( i = 0; i < BWMAX; i++, f += Bwc.bi ){
		Bwc.bw[i].FQ = RoundUp(f, 1000);
	}
	Bwc.bo = BWMAX/2;
	AddBWC(Bwc.bo);
	SetBWSDim(1);
    Bwc.MatchCenter = 1;
    Bwc.MatchData.Flag = 0;
    Bwc.MatchData.FQ = Bwc.fo;
}

void CRes::SetBWSDim(int n)
{
	if( (n < 1) || (n > BWSDIMMAX) ) return;

	Bwc.bwsdim = n;

	int i, j;
	double f = Bwc.fo - (Bwc.bi*2);
    double fi = Bwc.bi/(n + 1);
	double ff;
	for( i = 0; i < BWSMAX; i++, f += Bwc.bi ){
		ff = f + fi;
		for( j = 0; j < n; j++, ff += fi ){
			Bwc.bws[i][j].FQ = RoundUp(ff, 1000);
			Bwc.bws[i][j].Flag = 0;
        }
        for( ; j < BWSDIMMAX; j++ ){
			Bwc.bws[i][j].Flag = 0;
        }
    }
}

//--------------------------------------------------------
// 固定小数点に変換
short ToFix(float d)
{
	d *= 256;
	if( d <= -32767.0 ){
		return -32767;
	}
	else {
		return short(d);
	}
}

void CRes::UpdateCountBW(void)
{
	int i, j;

	Bwc.bwsfull = 1;
	for( Bwc.bcnt = 0, i = 0; i < BWMAX; i++ ){
		if( Bwc.bw[i].Flag == 1 ) Bwc.bcnt++;
        if( i < BWSMAX ){
			for( j = 0; j < Bwc.bwsdim; j++ ){
				if( !Bwc.bws[i][j].Flag ) Bwc.bwsfull = 0;
            }
        }
	}
}

BWS *CRes::SearchBW(BWC *bp, double fq)
{
	if( bp->bcnt ){
		int i, j;
	    for( i = 0; i < BWMAX; i++ ){
			if( bp->bw[i].Flag && (fq == bp->bw[i].FQ) ){
				return (BWS *)&bp->bw[i];
	        }
			if( i < BWSMAX ){
				for( j = 0; j < bp->bwsdim; j++ ){
					if( bp->bws[i][j].Flag && (fq == bp->bws[i][j].FQ) ){
						return &bp->bws[i][j];
	                }
	            }
	        }
	    }
        if( bp->MatchData.Flag && (fq == bp->MatchData.FQ) ){
			return &bp->MatchData;
        }
    }
    return NULL;
}

void CRes::ClearBWC(void)
{
	Bwc.bcnt = 0;
	memset(&Bwc.bws, 0, sizeof(Bwc.bws));
    Bwc.MatchData.Flag = 0;
}

void CRes::AddBWC(int n)
{
	if( n >= BWMAX ) return;

	Bwc.bw[n].Flag = 1;
	Bwc.bw[n].R = Res[RWp].R;		// Ｒ
	Bwc.bw[n].JX = Res[RWp].JX;		// ｊＸ
	Bwc.bw[n].GA = Res[RWp].GAIN;
	Bwc.bw[n].FB = Res[RWp].FB;
	Bwc.bw[n].PFlag = 1;
	int N = 2/PtnF;
	int i, j;
	for( i = j = 0; i < 180; i++, j += N ){			// 水平パターンをコピー
		Bwc.bw[n].PtnH[0][i] = ToFix(PtnH[0][j]);
		Bwc.bw[n].PtnH[1][i] = ToFix(PtnH[1][j]);
		Bwc.bw[n].PtnH[2][i] = ToFix(PtnH[2][j]);
	}
	for( i = j = 0; i <= 90; i++, j += N ){			// 垂直パターンをコピー
		Bwc.bw[n].PtnV[0][i] = ToFix(PtnV[0][j]);
		Bwc.bw[n].PtnV[1][i] = ToFix(PtnV[1][j]);
		Bwc.bw[n].PtnV[2][i] = ToFix(PtnV[2][j]);
	}
	Bwc.bcnt++;
}

void CRes::AddBWS(int n, int j)
{
	if( n >= BWSMAX ) return;
	if( j >= BWSDIMMAX ) return;
	Bwc.bws[n][j].Flag = 1;
	Bwc.bws[n][j].R = Res[RWp].R;		// Ｒ
	Bwc.bws[n][j].JX = Res[RWp].JX;		// ｊＸ
	Bwc.bws[n][j].GA = Res[RWp].GAIN;
	Bwc.bws[n][j].FB = Res[RWp].FB;
}

void CRes::ClearCur(void)
{
	for( int i = 0; i < env.pmax; i++ ){
		Cur[i] = NULLF;
		CurDir[i] = 0;
	}
	RealMax = ImgMax = 0.0;
}

void CRes::SetCur(ANTDEF *ap, int n, double r, double j, double d)
{
	if( n < ap->wzmax ){
		if( ap->wzdef[n].PNo ){					// パルスが存在する時
			n = ap->wzdef[n].PNo - 1;
			if( Cur[n] == NULLF ){
				SetCurDir(n, r, j);
				Cur[n] = d;
			}
		}
	}
}

void CRes::DrawRes(TCanvas *tp, int X, int Y)
{
	if( !RCnt ) return;
	LPCSTR	_ty[]={"自由空間","完全導体GND","リアルGND"};

	char	bf[64];
	char	bf2[64];
	RESL	*rp = GetResP();
	double	g = rp->GAIN;
	if( g < -999 ) g = -999;
	sprintf( bf, "Ga  :%s(dBi) = 0dB  (%s偏波)", StrDbl2(g), (rp->MAXVP > rp->MAXHP) ? "垂直":"水平");
	int FH = tp->TextHeight(bf) + 1;
	int Sop = ::SetBkMode(tp->Handle, TRANSPARENT);
	tp->TextOut(X, Y, bf); Y+=FH;
	if( !rp->TYPE ){
		g -= 2.15;
		if( g < -999 ) g = -999;
		sprintf( bf, "Gh  :%s(dBd)", StrDbl2(g));
		::SetBkMode(tp->Handle, TRANSPARENT);
		tp->TextOut(X, Y, bf); Y+=FH;
	}
	if( rp->FBR & 0xffff0000 ){
		sprintf(bf2, "水平%u°垂直%u°", (rp->FBR & 0x0000ffff), (rp->FBR >> 16) & 0x0000ffff);
	}
	else {
		sprintf(bf2, "水平%u°", rp->FBR & 0x0000ffff );
	}
	sprintf( bf, "F/B :%s(dB) 後方:%s", StrDbl2(rp->FB), bf2);
	::SetBkMode(tp->Handle, TRANSPARENT);
	tp->TextOut(X, Y, bf); Y+=FH;
	sprintf( bf, "Freq:%s(MHz)", StrDbl3(rp->FQ));
	::SetBkMode(tp->Handle, TRANSPARENT);
	tp->TextOut(X, Y, bf); Y+=FH;
	strcpy(bf2, StrDbl3(ABS(rp->JX)));
	sprintf( bf, "Z   :%s%cj%s", StrDbl3(rp->R), (rp->JX < 0.0)?'-':'+', bf2 );
	::SetBkMode(tp->Handle, TRANSPARENT);
	tp->TextOut(X, Y, bf); Y+=FH;
	sprintf(bf2, "%s(600Ω)", StrDbl2(CalcSWR(rp->R, rp->JX, 600.0, 0)));
	sprintf( bf, "SWR :%s(%sΩ) %s", StrDbl2(rp->SWR), StrCLXZ(rp->RO, rp->JXO), bf2);
	::SetBkMode(tp->Handle, TRANSPARENT);
	tp->TextOut(X, Y, bf); Y+=FH;
	sprintf(bf2, ":%smH", StrDbl(rp->HEIGHT));
	if( !rp->TYPE ) bf2[0] = 0;
	sprintf( bf, "仰角:%s°(%s%s)", StrDbl1(rp->ELV), _ty[rp->TYPE], bf2);
	::SetBkMode(tp->Handle, TRANSPARENT);
	tp->TextOut(X, Y, bf);
	if( rp->ELV != res.ElvHPtn ){
		Y+=FH;
		g = (rp->MAXHP > rp->MAXVP) ? rp->MAXHP : rp->MAXVP;
		if( g < -999 ) g = -999;
		sprintf(bf2, "%s", StrDbl2(g));
		sprintf( bf, "(水平ﾊﾟﾀｰﾝの仰角:%s°Peak:%sdBi)", StrDbl1(res.ElvHPtn), bf2);
		::SetBkMode(tp->Handle, TRANSPARENT);
		tp->TextOut(X, Y, bf);
	}
	::SetBkMode(tp->Handle, Sop);
}

void CRes::SaveCurData(FILE *fp)
{
	fwrite(&CalcF, 1, sizeof(CalcF), fp);
	if( !CalcF ) return;
	fwrite(Name, 1, sizeof(Name), fp);
	fwrite(GetResP(), 1, sizeof(RESL), fp);		// 最新の結果

	fwrite(&PtnF, 1, sizeof(PtnF), fp);
	fwrite(&Voff, 1, sizeof(Voff), fp);
	fwrite(&DegF, 1, sizeof(DegF), fp);
	fwrite(&DegV, 1, sizeof(DegV), fp);
	fwrite(&DegH, 1, sizeof(DegH), fp);
	fwrite(&MaxH, 1, sizeof(MaxH), fp);
	fwrite(&MaxV, 1, sizeof(MaxV), fp);
	fwrite(&MaxG, 1, sizeof(MaxG), fp);
	fwrite(PtnV, 1, sizeof(PtnV), fp);
	fwrite(PtnH, 1, sizeof(PtnH), fp);
	fwrite(Cur, 1, sizeof(Cur), fp);
	fwrite(&Bwc, 1, sizeof(Bwc), fp);
	fwrite(&ElvHPtn, 1, sizeof(ElvHPtn), fp);
}

void CRes::LoadCurData(FILE *fp)
{
	int d;
	fread(&d, 1, sizeof(d), fp);
	if( !d ) return;
	CalcF = d;
	fread(Name, 1, sizeof(Name), fp);
	fread(&Res[0], 1, sizeof(RESL), fp);		// 最新の結果
	RWp = 0;
	RCnt = 1;

	fread(&PtnF, 1, sizeof(PtnF), fp);
	fread(&Voff, 1, sizeof(Voff), fp);
	fread(&DegF, 1, sizeof(DegF), fp);
	fread(&DegV, 1, sizeof(DegV), fp);
	fread(&DegH, 1, sizeof(DegH), fp);
	fread(&MaxH, 1, sizeof(MaxH), fp);
	fread(&MaxV, 1, sizeof(MaxV), fp);
	fread(&MaxG, 1, sizeof(MaxG), fp);
	fread(PtnV, 1, sizeof(PtnV), fp);
	fread(PtnH, 1, sizeof(PtnH), fp);
	fread(Cur, 1, sizeof(Cur), fp);
	fread(&Bwc, 1, sizeof(Bwc), fp);
	fread(&ElvHPtn, 1, sizeof(ElvHPtn), fp);
	MaxVP = Res[0].MAXVP;
	MaxHP = Res[0].MAXHP;
}

int CRes::CopyLastRes(CRes *rp)
{
	if( rp->CalcF ){
		strcpy(Name, rp->Name);
		RWp = 0;
		RCnt = 1;
		memcpy(Res, rp->GetResP(), sizeof(RESL));
		CalcF = rp->CalcF;
		PtnF = rp->PtnF;

		Voff = rp->Voff;
		DegF = rp->DegF;
		DegV = rp->DegV;
		DegH = rp->DegH;
		memcpy(PtnV, rp->PtnV, sizeof(PtnV));
		memcpy(PtnH, rp->PtnH, sizeof(PtnH));
		MaxH = rp->MaxH;
		MaxV = rp->MaxV;
		MaxG = rp->MaxG;
		MaxVP = rp->MaxVP;
		MaxHP = rp->MaxHP;
		ElvHPtn = rp->ElvHPtn;
		memcpy(Cur, rp->Cur, sizeof(Cur));
	}
	return CalcF;
}

int CRes::SetRes(CRes *rp)
{
	if( rp->CalcF ){
		memcpy(GetResP(), rp->GetResP(), sizeof(RESL));
		CalcF = rp->CalcF;
		PtnF = rp->PtnF;

		Voff = rp->Voff;
		DegF = rp->DegF;
		DegV = rp->DegV;
		DegH = rp->DegH;
		memcpy(PtnV, rp->PtnV, sizeof(PtnV));
		memcpy(PtnH, rp->PtnH, sizeof(PtnH));
		MaxH = rp->MaxH;
		MaxV = rp->MaxV;
		MaxG = rp->MaxG;
		memcpy(Cur, rp->Cur, sizeof(Cur));
		MaxVP = rp->MaxVP;
		MaxHP = rp->MaxHP;
		ElvHPtn = rp->ElvHPtn;
	}
	return CalcF;
}

///----------------------------------------------------------------
///  CDrawPtnHクラス
void CDrawPtnH::GetDBR(int &X, int &Y, double deg, double db)
{
	db = db / 20.0;
	db = pow(10.0, db);
	db = sqrt(db) * double(rr);
	X = xc + int(db * sin(deg));
	Y = yc + int(db * -cos(deg));
}

void CDrawPtnH::DrawGage(double deg, int i)
{
	char	bf[10];
	int SFS = tp->Font->Height;
	int fs = SFS / 2;
	if( !fs ) fs++;
	tp->Font->Height = fs;

	int n, X, Y, AX, AY;
	if( !(i & 1) ){
		for( n = 0; n <= 50; n++ ){
			if( !(n % 10) ){
				if( (n < 30) || (!(i % 2)) ){
					GetDBR(X, Y, deg, double(-n));
					if( gCol == clBlack ){
						if( n ){
							GetDBR(AX, AY, deg+(PAI/180.0), double(-n));
						}
						else {
							GetDBR(AX, AY, deg, double(-n)-0.5);
						}
						tp->Pen->Color = gCol;
						tp->MoveTo(AX, AY);tp->LineTo(X, Y);
					}
					else if( n ){
						tp->Pixels[X][Y] = gCol;
					}
				}
				if( (i == 0) && (n <= 40) ){
					sprintf(bf, "%d", -n);
					X -= tp->TextWidth(bf)/2;
					int Sop = ::SetBkMode(tp->Handle, TRANSPARENT);
					tp->TextOut(X, Y, bf);
					::SetBkMode(tp->Handle, Sop);
				}
			}
			else if( n < 30 ){
				if( !(i % 10) ){
					GetDBR(X, Y, deg, double(-n));
					if( gCol == clBlack ){
						GetDBR(AX, AY, deg, double(-n)-0.5);
						tp->Pen->Color = gCol;
						tp->MoveTo(AX, AY);tp->LineTo(X, Y);
					}
					else {
						tp->Pixels[X][Y] = gCol;
					}
				}
			}
		}
	}
	tp->Font->Height = SFS;
}

void CDrawPtnH::SetRect(TCanvas *pc, int x1, int y1, int x2, int y2)
{
	tp = pc;
	int r1 = (x2 - x1)/2;
	int r2 = (y2 - y1)/2;
	xc = x1 + r1;
	yc = y1 + r2;
	rr = ((r1 > r2) ? r2 : r1);
	ri = rr / 4;
	xl = xc - rr, yt = yc - rr;
	xr = xc + rr + 1, yb = yc + rr + 1;
}

void CDrawPtnH::DrawAngle(int Add /* = 1*/)
{
	if( !Add ) Add++;
	tp->Pen->Style = psSolid;
	tp->Pen->Color = (gCol != clBlack) ? clLtGray : gCol;
	tp->Brush->Color = clWhite;
	tp->Ellipse(xl, yt, xr, yb);		// 外枠

	int i;
	double	deg;
	for( i = 0; i <= 360; i+=Add ){
		deg = double(i) * PAI / 180.0;
		DrawGage(deg, i);
	}
}

void CDrawPtnH::DrawPtn(float *pt, int Add, TColor Col /*= clBlack*/)
{
	if( !Add ) Add++;
	int i, X, Y, AX, AY;
	double	deg;
	tp->Pen->Style = psSolid;
	tp->Pen->Color = Col;
	for( i = AX = AY = 0; i <= 360; i+=Add, pt++ ){
		if( i == 360 ) pt -= (360/Add);
		deg = double(i) * PAI / 180.0;
		GetDBR(X, Y, deg, *pt - Max);
		if( i ){
			tp->MoveTo(AX, AY);
			tp->LineTo(X, Y);
		}
		AX = X; AY = Y;
	}
}

///----------------------------------------------------------------
///  CDrawPtnVクラス
void CDrawPtnV::GetDBR(int &X, int &Y, double deg, double db)
{
	db = db / 20.0;
	db = pow(10.0, db);
	db = sqrt(db) * double(rr);
	X = xc + int(db * -sin(deg));
	Y = yc + int(db * cos(deg));
}

void CDrawPtnV::DrawGage(double deg, int i)
{
	char	bf[10];
	int SFS = tp->Font->Height;
	int fs = SFS / 2;
	if( !fs ) fs++;
	tp->Font->Height = fs;

	int n, X, Y, AX, AY;
	if( !(i & 1) ){
		for( n = 0; n <= 50; n++ ){
			if( !(n % 10) ){
				if( (n < 30) || (!(i % 2)) ){
					GetDBR(X, Y, deg, double(-n));
					if( gCol == clBlack ){
						if( n ){
							GetDBR(AX, AY, deg+(PAI/180.0), double(-n));
						}
						else {
							GetDBR(AX, AY, deg, double(-n)-0.5);
						}
						tp->Pen->Color = gCol;
						tp->MoveTo(AX, AY);tp->LineTo(X, Y);
					}
					else if( n ){
						tp->Pixels[X][Y] = gCol;
					}
				}
				if( (i == 90) && (n <= 40) ){
					sprintf(bf, "%d", -n);
					X -= tp->TextWidth(bf)/2;
					int Sop = ::SetBkMode(tp->Handle, TRANSPARENT);
					tp->TextOut(X, Y, bf);
					::SetBkMode(tp->Handle, Sop);
				}
			}
			else if( n < 30 ){
				if( !(i % 10) ){
					GetDBR(X, Y, deg, double(-n));
					if( gCol == clBlack ){
						GetDBR(AX, AY, deg, double(-n)-0.5);
						tp->Pen->Color = gCol;
						tp->MoveTo(AX, AY);tp->LineTo(X, Y);
					}
					else {
						tp->Pixels[X][Y] = gCol;
					}
				}
			}
		}
	}
	tp->Font->Height = SFS;
}

void CDrawPtnV::SetRect(TCanvas *pc, int x1, int y1, int x2, int y2)
{
	tp = pc;
	int r1 = (x2 - x1)/2;
	int r2 = (y2 - y1)/2;
	xc = x1 + r1;
	yc = y1 + r2;
	rr = ((r1 > r2) ? r2 : r1);
	ri = rr / 4;
	xl = xc - rr, yt = yc - rr;
	xr = xc + rr + 1, yb = yc + rr + 1;
}

void CDrawPtnV::DrawAngle(int Add /* = 1*/)
{
	if( !Add ) Add++;
	tp->Pen->Style = psSolid;
	tp->Pen->Color = (gCol != clBlack) ? clLtGray : gCol;
	tp->Brush->Color = clWhite;
	tp->Pie(xl, yt, xr, yb, xr, yc, xl, yc);

	int i;
	double	deg;
	for( i = 0; i < 180; i+=Add ){
		deg = double(i+90) * PAI / 180.0;
		DrawGage(deg, i);
	}
}

void CDrawPtnV::DrawPtn(float *pt, int Add, TColor Col /*= clBlack*/)
{
	if( !Add ) Add++;
	int i, X, Y, AX, AY;
	double	deg;
	tp->Pen->Style = psSolid;
	tp->Pen->Color = Col;
	for( i = AX = AY = 0; i <= 180; i+=Add, pt++ ){
		deg = double(i+90) * PAI / 180.0;
		GetDBR(X, Y, deg, *pt - Max);
		if( i ){
			tp->MoveTo(AX, AY);
			tp->LineTo(X, Y);
		}
		AX = X; AY = Y;
	}
}
///----------------------------------------------------------------
///  CAlignクラス
void CAlign::InitControl(TControl *p, TControl *pB, TFont *pF /*= NULL*/)
{
	tp = p;
	OTop = p->Top;
	OLeft = p->Left;
	OWidth = p->Width;
	OHeight = p->Height;
	if( pF != NULL ){
		fp = pF;
		OFontHeight = pF->Height;
	}

	BTop = pB->Top;
	BLeft = pB->Left;
	BWidth = pB->Width;
	BHeight = pB->Height;
}

void CAlign::InitControl(TControl *p, RECT *rp, TFont *pF /*= NULL*/)
{
	tp = p;
	OTop = p->Top;
	OLeft = p->Left;
	OWidth = p->Width;
	OHeight = p->Height;
	if( pF != NULL ){
		fp = pF;
		OFontHeight = pF->Height;
	}

	BTop = rp->top;
	BLeft = rp->left;
	BWidth = rp->right - rp->left + 1;
	BHeight = rp->bottom - rp->top + 1;
}

void CAlign::NewAlign(TControl *pB)
{
	double Sx = double(pB->Width)/double(BWidth);
	double Sy = double(pB->Height)/double(BHeight);
	tp->SetBounds(int(OLeft * Sx), int(OTop * Sy), int(OWidth * Sx), int(OHeight * Sy));
	if( fp != NULL ){
		if( Sx > Sy ) Sx = Sy;
		fp->Height = int(OFontHeight * Sx) - env.FontSize;
	}
}
///----------------------------------------------------------------
///  CAlignListクラス
CAlignList::CAlignList(void)
{
	Max = 0;
	Cnt = 0;
	AlignList = NULL;
}

CAlignList::~CAlignList(){
	if( AlignList != NULL ){
		for( int i = 0; i < Cnt; i++ ){
			delete AlignList[i];
		}
		delete AlignList;
		AlignList = NULL;
	}
}

void CAlignList::Alloc(void)
{
	if( Cnt >= Max ){
		Max = Max ? Max * 2 : 16;
		CAlign **np = (CAlign **)new BYTE[sizeof(CAlign*) * Max];
		if( AlignList != NULL ){
			for( int i = 0; i < Cnt; i++ ){
				np[i] = AlignList[i];
			}
			delete AlignList;
		}
		AlignList = np;
	}
	AlignList[Cnt] = new CAlign;
}
void CAlignList::EntryControl(TControl *tp, TControl *pB, TFont *pF /*= NULL*/)
{
	Alloc();
	AlignList[Cnt]->InitControl(tp, pB, pF);
	Cnt++;
}

void CAlignList::EntryControl(TControl *tp, RECT *rp, TFont *pF /*= NULL*/){
	Alloc();
	AlignList[Cnt]->InitControl(tp, rp, pF);
	Cnt++;
}

void CAlignList::NewAlign(TControl *pB)
{
	if( AlignList == NULL ) return;
	for( int i = 0; i < Cnt; i++ ){
		AlignList[i]->NewAlign(pB);
	}
}

///----------------------------------------------------------------
///  CAlignGridクラス
void CAlignGrid::InitGrid(TStringGrid *p)
{
	BWidth = p->Width;
	BHeight = p->Height;
	BRowHeight = p->DefaultRowHeight;
	Max = p->ColCount;
	bp = new int[Max];
	for( int i = 0; i < Max; i++ ){
		bp[i] = p->ColWidths[i];
	}
}

void CAlignGrid::NewAlign(TStringGrid *p)
{
	double Sx = double(p->Width)/double(BWidth);
	double Sy = double(p->Height)/double(BHeight);
	p->DefaultRowHeight = int(BRowHeight * Sy);
	for( int i = 0; (i < Max) && (i < p->ColCount); i++ ){
		p->ColWidths[i] = int(bp[i] * Sx);
	}
}

///----------------------------------------------------------------
///  CWGroupクラス
CWGroup::CWGroup(void)
{
	Clear();
}

void CWGroup::Init(WDEF *p)
{
	YL = 0;
	YH = 0;
	ZL = 0;
	ZH = 0;
	XL = 0;
	XH = 0;

	BX = 0;
	BY = 0;
	BZ = 0;

	Delete();
	pW = p;
}

void CWGroup::Clear(void)
{
	pB = NULL;
	Delete();
}

void CWGroup::Delete(void)
{
	if( pB != NULL ){
		delete pB;
		pB = NULL;
	}
	bmax = bcnt = 0;
	Type = 0;
	EnbR = EnbSeg = 1;
	XwF = 0;
}

void CWGroup::Alloc(void)
{
	if( bmax <= bcnt ){
		bmax = bmax ? (bmax * 2) : 4;
		WREF *np = new WREF[bmax];
		if( pB != NULL ){
			memcpy(np, pB, sizeof(WREF)*bcnt);
			delete pB;
		}
		pB = np;
	}
}

void CWGroup::AddWire(int Index)
{
	Alloc();
	pB[bcnt].ref = Index;
	pB[bcnt].wp = &pW[Index];
	bcnt++;
}

int CWGroup::AnaLoop(int N, double OX, double OY, double OZ, double X, double Y, double Z)
{
	int i;
	WREF	*rp = pB;
	for( i = 0; i < bcnt; i++, rp++ ){
		if( !rp->LCF ){
			if( (X == rp->wp->X1)&&(Y == rp->wp->Y1)&&(Z == rp->wp->Z1) ){
				rp->LCF = N + 1;
				if( (OX == rp->wp->X2)&&(OY == rp->wp->Y2)&&(OZ == rp->wp->Z2) ){
					return 1;
				}
				else if( AnaLoop(N+1, OX, OY, OZ, rp->wp->X2, rp->wp->Y2, rp->wp->Z2) ){
					return 1;
				}
				else {
					rp->LCF = -1;
				}
				return 0;
			}
			else if( (X == rp->wp->X2)&&(Y == rp->wp->Y2)&&(Z == rp->wp->Z2) ){
				rp->LCF = N + 1;
				if( (OX == rp->wp->X1)&&(OY == rp->wp->Y1)&&(OZ == rp->wp->Z1) ){
					return 1;
				}
				else if( AnaLoop(N+1, OX, OY, OZ, rp->wp->X1, rp->wp->Y1, rp->wp->Z1) ){
					return 1;
				}
				else {
					rp->LCF = -1;
				}
				return 0;
			}
		}
	}
	rp->LCF = -1;
	return 0;
}

// 形式を調べる
void CWGroup::AnaType(void)
{
	// ワイヤリストを作成する
	int i, j;
	WREF	*rp = pB;
	double	DM = 0.0;
	int		NM = 0;		// 最大長のワイヤ番号
	for( i = 0; i < bcnt; i++, rp++ ){
		rp->LCF = 0;
		rp->XX = rp->wp->X2 - rp->wp->X1;
		rp->YY = rp->wp->Y2 - rp->wp->Y1;
		rp->ZZ = rp->wp->Z2 - rp->wp->Z1;
		rp->D = sqrt(rp->XX * rp->XX + rp->YY * rp->YY + rp->ZZ * rp->ZZ);
		if( rp->YY ){
			if( rp->XX ){
				rp->VXY = RoundUp(rp->YY / rp->XX, 10.0);
			}
			else {
				rp->VXY = 1.0;
			}
		}
		else {
			rp->VXY = 0;
		}
		if( rp->ZZ ){
			if( rp->YY ){
				rp->VYZ = RoundUp(rp->ZZ / rp->YY, 10.0);
			}
			else {
				rp->VYZ = 1.0;
			}
		}
		else {
			rp->VYZ = 0;
		}
		if( rp->D < DM ){
			DM = rp->D;
			NM = i;
		}
	}
	// ループ構造を調べる
	pB[NM].LCF = 1;
	XwF = 0;
	if( !AnaLoop(1, pB[NM].wp->X1, pB[NM].wp->Y1, pB[NM].wp->Z1, pB[NM].wp->X2, pB[NM].wp->Y2, pB[NM].wp->Z2) ){
		XwF = 1;
		if( ZH != ZL ){
			if( (XL==XH)&&(YL==YH) ){
				Type = cwVDP;
			}
			else if( (ZH - ZL) >= (GetWidth()/4.0) ){
				Type = cwVANT;
			}
			else {
				Type = cwHANT;
			}
		}
		else if( XL == XH ){
			Type = cwHDP;
		}
		else {
			Type = cwHANT;
		}
		return;
	}
	// ループの巡回テーブルを作成する
	int *ip = new int[bcnt];
	for( NM = i = 0; i < bcnt; i++ ){
		for( rp = pB, j = 0; j < bcnt; j++, rp++ ){
			if( i == (rp->LCF - 1) ){
				ip[i] = j;
				NM++;
				break;
			}
		}
	}
	// 頂点の数を計算する
	LOOPS	*lp = new LOOPS[NM];
	int NT = 0;
	WREF *rrp;
	double	ZZ = 0.0;
	XL = MAXDOUBLE; YL = MAXDOUBLE; ZL = MAXDOUBLE;
	XH = -MAXDOUBLE; YH = -MAXDOUBLE; ZH = -MAXDOUBLE;
	for( i = 0; i < NM; i++ ){
		int j = i + 1;
		if( j >= NM ) j = 0;
		rp = &pB[ip[i]];
		ZZ += ABS(rp->ZZ);
		rrp = &pB[ip[j]];
		if( (rp->VXY == rrp->VXY) && (rp->VYZ == rrp->VYZ) ){
			// 等しいベクトル
		}
		else {
			if( ( (rp->wp->X1 == rrp->wp->X1) && (rp->wp->Y1 == rrp->wp->Y1) && (rp->wp->Z1 == rrp->wp->Z1) ) ||
				( (rp->wp->X1 == rrp->wp->X2) && (rp->wp->Y1 == rrp->wp->Y2) && (rp->wp->Z1 == rrp->wp->Z2) ) ){
				lp[NT].X = rp->wp->X1;
				lp[NT].Y = rp->wp->Y1;
				lp[NT].Z = rp->wp->Z1;
			}
			else {
				lp[NT].X = rp->wp->X2;
				lp[NT].Y = rp->wp->Y2;
				lp[NT].Z = rp->wp->Z2;
			}
			lp[NT].VXY = rp->VXY;
			lp[NT].VYZ = rp->VYZ;
			if( XL > lp[NT].X ) XL = lp[NT].X;
			if( YL > lp[NT].Y ) YL = lp[NT].Y;
			if( ZL > lp[NT].Z ) ZL = lp[NT].Z;
			if( XH < lp[NT].X ) XH = lp[NT].X;
			if( YH < lp[NT].Y ) YH = lp[NT].Y;
			if( ZH < lp[NT].Z ) ZH = lp[NT].Z;
			NT++;
		}
	}
	if( NT == 3 ){		// ▽、△ループ
		if( ZZ ){			// 垂直
			if( lp[0].Z == lp[1].Z ){
				if( lp[2].Z > lp[0].Z ){
					Type = cwVTA;
				}
				else {
					Type = cwVITA;
				}
			}
			else if( lp[0].Z == lp[2].Z ){
				if( lp[1].Z > lp[0].Z ){
					Type = cwVTA;
				}
				else {
					Type = cwVITA;
				}
			}
			else {
				if( lp[0].Z > lp[1].Z ){
					Type = cwVTA;
				}
				else {
					Type = cwVITA;
				}
			}
		}
		else {				// 水平
			if( lp[0].X == lp[1].X ){
				if( lp[2].X > lp[0].X ){
					Type = cwHTA;
				}
				else {
					Type = cwHITA;
				}
			}
			else if( lp[0].X == lp[2].X ){
				if( lp[1].X > lp[0].X ){
					Type = cwHTA;
				}
				else {
					Type = cwHITA;
				}
			}
			else {
				if( lp[0].X > lp[1].X ){
					Type = cwHTA;
				}
				else {
					Type = cwHITA;
				}
			}
		}
	}
	else if( NT == 4 ){	// □、◇ループ
		if( ZZ ){		// 垂直
			if( lp[0].VXY && lp[0].VYZ ){
				Type = cwVDM;
			}
			else {
				Type = cwVSQ;
			}
		}
		else {			// 水平
			if( (!lp[0].VXY) || (lp[0].VXY == 1.0) ){
				Type = cwHSQ;
			}
			else {
				Type = cwHDM;
			}
		}
	}
	else if( NT >= 3 ){	// 不明なループ
		XwF = 1;
		if( ZZ ){
			if( NT == 8 ){			// ８角ループ
				Type = cwVPIE;
				XwF = 0;
			}
			else {
				Type = cwVLP;
			}
		}
		else {
			Type = cwHLP;
		}
	}
	else {
		XwF = 1;
		Type = cwNULL;
	}
	delete lp;
	delete ip;
}

// ワイヤ形式を調べる
void CWGroup::AnaWire(void)
{
	int i;
	double	R = 0.0;
	int		SEG = 0;

	EnbR = EnbSeg = 1;
	WREF *rp = pB;
	XL = MAXDOUBLE; YL = MAXDOUBLE; ZL = MAXDOUBLE;
	XH = -MAXDOUBLE; YH = -MAXDOUBLE; ZH = -MAXDOUBLE;
	for(i = 0; i < bcnt; i++, rp++ ){
		if( XL > rp->wp->X1 ) XL = rp->wp->X1;
		if( YL > rp->wp->Y1 ) YL = rp->wp->Y1;
		if( ZL > rp->wp->Z1 ) ZL = rp->wp->Z1;
		if( XH < rp->wp->X1 ) XH = rp->wp->X1;
		if( YH < rp->wp->Y1 ) YH = rp->wp->Y1;
		if( ZH < rp->wp->Z1 ) ZH = rp->wp->Z1;

		if( XL > rp->wp->X2 ) XL = rp->wp->X2;
		if( YL > rp->wp->Y2 ) YL = rp->wp->Y2;
		if( ZL > rp->wp->Z2 ) ZL = rp->wp->Z2;
		if( XH < rp->wp->X2 ) XH = rp->wp->X2;
		if( YH < rp->wp->Y2 ) YH = rp->wp->Y2;
		if( ZH < rp->wp->Z2 ) ZH = rp->wp->Z2;
		if( i ){
			if( rp->wp->R != R ) EnbR = 0;
			if( rp->wp->SEG != SEG ) EnbSeg = 0;
		}
		else {
			R = rp->wp->R;
			SEG = rp->wp->SEG;
		}
	}
	AnaType();
	BX = (XH + XL)/2;
	BY = (YH + YL)/2;
	BZ = (ZH + ZL)/2;
	rp = pB;
	for(i = 0; i < bcnt; i++, rp++ ){
		rp->wp->X1 -= BX;
		rp->wp->X2 -= BX;
	}
	XL -= BX;
	XH -= BX;
}

double CWGroup::GetRound(void)
{
	double	h, w;

	h = GetHeight();
	w = GetWidth();
	switch(Type){
		case cwVSQ:		// 垂直□
		case cwHSQ:		// 水平□
			return (h * 2) + (w * 2);
		case cwVDM:		// 垂直◇
		case cwHDM:		// 水平◇
			return 2 * sqrt((h * h) + (w * w));
		case cwVTA:		// 垂直△
		case cwVITA:		// 垂直▽
		case cwHTA:		// 水平△
		case cwHITA:		// 水平▽
			return w + 2 * sqrt((h * h) + ((w * w)/4));
		case cwVPIE:
			return ((h * TAN22R5) * 4) + ((w * TAN22R5) * 4);
		case cwHLP:		// 水平ループ
		case cwVLP:		// 垂直ループ
		case cwNULL:	// 不明
		case cwVDP:		// 垂直DP
		case cwHDP:		// 水平DP
		default:
			return XH - XL;
	}
}

void CWGroup::SetRound(double d)
{
	double	h, w, b;

	h = GetHeight();
	w = GetWidth();
	switch(Type){
		case cwVSQ:		// 垂直□
		case cwHSQ:		// 水平□
			w = (d / ((h/w) + 1.0));
			SetWidth(w/2.0);
			SetHeight((d - w)/2.0);
			break;
		case cwVDM:		// 垂直◇
		case cwHDM:		// 水平◇
			b = (h/w);
			w = d / sqrt(4.0 * ((b * b) + 1.0));
//			h = sqrt((d * d) - (4 * w * w))/2.0;
			h = b * w;
			SetWidth(w);
			SetHeight(h);
			break;
		case cwVTA:		// 垂直△
		case cwVITA:		// 垂直▽
		case cwHTA:		// 水平△
		case cwHITA:		// 水平▽
			b = (h/w);
#if 1
			w = d / (2.0 * sqrt(b * b + 0.25) + 1.0);
#else
			w = d / (1.0+(2.0*b/COS30));
#endif
			h = b * w;
			SetWidth(w);
			SetHeight(h);
			break;
		case cwVPIE:
			SetPieRound(d);
			break;
		case cwHLP:		// 水平ループ
		case cwVLP:		// 垂直ループ
		case cwNULL:	// 不明
		case cwVDP:		// 垂直DP
		case cwHDP:		// 水平DP
		default:
			SetXWidth(d);
			break;
	}
}

void CWGroup::SetRoundAll(double d)
{
	double	h, w, b;

	h = GetHeight();
	w = GetWidth();
	switch(Type){
		case cwVSQ:		// 垂直□
		case cwHSQ:		// 水平□
			w = (d / ((h/w) + 1.0));
			SetWidthAll(w/2.0);
			SetHeightAll((d - w)/2.0);
			break;
		case cwVDM:		// 垂直◇
		case cwHDM:		// 水平◇
			b = (h/w);
			w = d / sqrt(4.0 * ((b * b) + 1.0));
//			h = sqrt((d * d) - (4 * w * w))/2.0;
			h = b * w;
			SetWidthAll(w);
			SetHeightAll(h);
			break;
		case cwVTA:		// 垂直△
		case cwVITA:		// 垂直▽
		case cwHTA:		// 水平△
		case cwHITA:		// 水平▽
			b = (h/w);
#if 1
			w = d / (2.0 * sqrt(b * b + 0.25) + 1.0);
#else
			w = d / (1.0+(2.0*b/COS30));
#endif
			h = b * w;
			SetWidthAll(w);
			SetHeightAll(h);
			break;
		case cwVPIE:
			SetPieRound(d);
			break;
		case cwHLP:		// 水平ループ
		case cwVLP:		// 垂直ループ
		case cwNULL:	// 不明
		case cwVDP:		// 垂直DP
		case cwHDP:		// 水平DP
		default:
			SetXWidthAll(d);
			break;
	}
}

void CWGroup::SetPieRound(double d)
{
	WREF *rp = pB;
	double l = d / 8.0;
	double h = l / TAN22R5;
	SetWidth(h);
	SetHeight(h);
	double	rsin = (l/2);
	int i;
	for( i = 0; i < bcnt; i++, rp++ ){
		if( (rp->wp->Z1 == ZH)||(rp->wp->Z1 == ZL) ){
			if( rp->wp->Y1 > BY ){
				rp->wp->Y1 = BY + rsin;
			}
			else {
				rp->wp->Y1 = BY - rsin;
			}
		}
		if( (rp->wp->Z2 == ZH)||(rp->wp->Z2 == ZL) ){
			if( rp->wp->Y2 > BY ){
				rp->wp->Y2 = BY + rsin;
			}
			else {
				rp->wp->Y2 = BY - rsin;
			}
		}
		if( (rp->wp->Y1 == YH)||(rp->wp->Y1 == YL) ){
			if( rp->wp->Z1 > BZ ){
				rp->wp->Z1 = BZ + rsin;
			}
			else {
				rp->wp->Z1 = BZ - rsin;
			}
		}
		if( (rp->wp->Y2 == YH)||(rp->wp->Y2 == YL) ){
			if( rp->wp->Z2 > BZ ){
				rp->wp->Z2 = BZ + rsin;
			}
			else {
				rp->wp->Z2 = BZ - rsin;
			}
		}
	}
}

void CWGroup::SetXWidth(double W)
{
	int i;
	WREF *rp = pB;

	if( XH != XL ){
		double NXL = -(W/2);
		double NXH = (W/2);
		for( i = 0; i < bcnt; i++, rp++ ){
			if( rp->wp->X1 > XH ) rp->wp->X1 += NXH - XH;
			if( rp->wp->X2 > XH ) rp->wp->X2 += NXH - XH;
			if( rp->wp->X1 < XL ) rp->wp->X1 -= XL - NXL;
			if( rp->wp->X2 < XL ) rp->wp->X2 -= XL - NXL;
			if( rp->wp->X1 == XL ) rp->wp->X1 = NXL;
			if( rp->wp->X2 == XL ) rp->wp->X2 = NXL;
			if( rp->wp->X1 == XH ) rp->wp->X1 = NXH;
			if( rp->wp->X2 == XH ) rp->wp->X2 = NXH;
		}
		XL = NXL;
		XH = NXH;
	}
}

void CWGroup::SetWidth(double W)
{
	int i;
	WREF *rp = pB;

	if( YH != YL ){
		double	NYL;
		double	NYH;
		if( YL == 0 ){
			NYL = 0;
			NYH = W;
		}
		else if( YH == 0 ){
			NYL = -W;
			NYH = 0.0;
		}
		else if( (YL < 0) && (YH > 0) && (YH - YL) ){
			NYL = YL * (W/(YH-YL));
			NYH = YH * (W/(YH-YL));
		}
		else {
			NYL = BY - (W/2);
			NYH = BY + (W/2);
		}
		for( i = 0; i < bcnt; i++, rp++ ){
			if( rp->wp->Y1 > YH ) rp->wp->Y1 += NYH - YH;
			if( rp->wp->Y2 > YH ) rp->wp->Y2 += NYH - YH;
			if( rp->wp->Y1 < YL ) rp->wp->Y1 -= YL - NYL;
			if( rp->wp->Y2 < YL ) rp->wp->Y2 -= YL - NYL;
			if( rp->wp->Y1 == YL ) rp->wp->Y1 = NYL;
			if( rp->wp->Y2 == YL ) rp->wp->Y2 = NYL;
			if( rp->wp->Y1 == YH ) rp->wp->Y1 = NYH;
			if( rp->wp->Y2 == YH ) rp->wp->Y2 = NYH;
		}
		YL = NYL;
		YH = NYH;
	}
	else {
		SetXWidth(W);
	}
}

void CWGroup::SetHeight(double W)
{
	int i;
	WREF *rp = pB;

	if( ZH != ZL ){
		double	NZL;
		double	NZH;
		if( ZL == 0 ){
			NZL = 0;
			NZH = W;
		}
		else if( ZH == 0 ){
			NZL = -W;
			NZH = 0.0;
		}
		else if( (ZL < 0) && (ZH > 0) && (ZH - ZL) ){
			NZL = ZL * (W/(ZH-ZL));
			NZH = ZH * (W/(ZH-ZL));
		}
		else {
			NZL = BZ - (W/2);
			NZH = BZ + (W/2);
		}
		for( i = 0; i < bcnt; i++, rp++ ){
			if( rp->wp->Z1 > ZH ) rp->wp->Z1 += NZH - ZH;
			if( rp->wp->Z2 > ZH ) rp->wp->Z2 += NZH - ZH;
			if( rp->wp->Z1 < ZL ) rp->wp->Z1 -= ZL - NZL;
			if( rp->wp->Z2 < ZL ) rp->wp->Z2 -= ZL - NZL;
			if( rp->wp->Z1 == ZL ) rp->wp->Z1 = NZL;
			if( rp->wp->Z2 == ZL ) rp->wp->Z2 = NZL;
			if( rp->wp->Z1 == ZH ) rp->wp->Z1 = NZH;
			if( rp->wp->Z2 == ZH ) rp->wp->Z2 = NZH;
		}
		ZL = NZL;
		ZH = NZH;
	}
	else {
		SetXWidth(W);
	}
}

static void SetLineXY(double &t, double oh, double ol, double nh, double nl)
{
	double	ow = oh - ol;
    double	nw = nh - nl;

    t = t * (nw/ow);
}

void CWGroup::SetXWidthAll(double W)
{
	int i;
	WREF *rp = pB;

	if( XH != XL ){
		double NXL = -(W/2);
		double NXH = (W/2);
		for( i = 0; i < bcnt; i++, rp++ ){
			SetLineXY(rp->wp->X1, XH, XL, NXH, NXL);
			SetLineXY(rp->wp->X2, XH, XL, NXH, NXL);
		}
		XL = NXL;
		XH = NXH;
	}
}

void CWGroup::SetWidthAll(double W)
{
	int i;
	WREF *rp = pB;

	if( YH != YL ){
		double	NYL;
		double	NYH;
		if( YL == 0 ){
			NYL = 0;
			NYH = W;
		}
		else if( YH == 0 ){
			NYL = -W;
			NYH = 0.0;
		}
		else if( (YL < 0) && (YH > 0) && (YH - YL) ){
			NYL = YL * (W/(YH-YL));
			NYH = YH * (W/(YH-YL));
		}
		else {
			NYL = BY - (W/2);
			NYH = BY + (W/2);
		}
		for( i = 0; i < bcnt; i++, rp++ ){
			SetLineXY(rp->wp->Y1, YH, YL, NYH, NYL);
			SetLineXY(rp->wp->Y2, YH, YL, NYH, NYL);
		}
		YL = NYL;
		YH = NYH;
	}
	else {
		SetXWidthAll(W);
	}
}

void CWGroup::SetHeightAll(double W)
{
	int i;
	WREF *rp = pB;

	if( ZH != ZL ){
		double	NZL;
		double	NZH;
		if( ZL == 0 ){
			NZL = 0;
			NZH = W;
		}
		else if( ZH == 0 ){
			NZL = -W;
			NZH = 0.0;
		}
		else if( (ZL < 0) && (ZH > 0) && (ZH - ZL) ){
			NZL = ZL * (W/(ZH-ZL));
			NZH = ZH * (W/(ZH-ZL));
		}
		else {
			NZL = BZ - (W/2);
			NZH = BZ + (W/2);
		}
		for( i = 0; i < bcnt; i++, rp++ ){
			SetLineXY(rp->wp->Z1, ZH, ZL, NZH, NZL);
			SetLineXY(rp->wp->Z2, ZH, ZL, NZH, NZL);
		}
		ZL = NZL;
		ZH = NZH;
	}
	else {
		SetXWidthAll(W);
	}
}

void CWGroup::SetR(double r)
{
	if( EnbR ){
		WREF *rp = pB;
		for( int i = 0; i < bcnt; i++, rp++ ){
			rp->wp->R = r;
		}
	}
}

void CWGroup::SetSeg(int s)
{
	if( EnbSeg ){
		WREF *rp = pB;
		for( int i = 0; i < bcnt; i++, rp++ ){
			rp->wp->SEG = s;
		}
	}
}

void CWGroup::SetXWidth(int all, double W)
{
	if( all ){
		SetXWidthAll(W);
    }
    else {
		SetXWidth(W);
    }
}
void CWGroup::SetWidth(int all, double W)
{
	if( all ){
		SetWidthAll(W);
    }
    else {
		SetWidth(W);
    }
}
void CWGroup::SetHeight(int all, double W)
{
	if( all ){
		SetHeightAll(W);
    }
    else {
		SetHeight(W);
    }
}
void CWGroup::SetRound(int all, double d)
{
	if( all ){
    	SetRoundAll(d);
    }
    else {
		SetRound(d);
    }
}
///----------------------------------------------------------------
///  CWGroupListクラス
int CWGroupList::GetNewWire(void)
{
	int i;
	for( i = 0; i < wmax; i++ ){
		if( !wdef[i].PNo ) return i;
	}
	wmax++;
	return i;
}

int CWGroupList::AddWire(double x1, double y1, double z1, double x2, double y2, double z2, double r, int seg)
{
	int wm = GetNewWire();
	wdef[wm].X1 = x1;
	wdef[wm].Y1 = y1;
	wdef[wm].Z1 = z1;
	wdef[wm].X2 = x2;
	wdef[wm].Y2 = y2;
	wdef[wm].Z2 = z2;
	wdef[wm].R = r;
	wdef[wm].SEG = seg;
	wdef[wm].PNo = 1;
	return wm;
}

void CWGroupList::Connect(int w, double X, double Y, double Z)
{
	for( int i = 0; i < wmax; i++ ){
		if( Con[i]==0xff ){
			if( (wdef[i].X1 == X) && (wdef[i].Y1 == Y) && (wdef[i].Z1 == Z) ){
				Con[i] = BYTE(w);
				Connect(w, wdef[i].X2, wdef[i].Y2, wdef[i].Z2);
			}
			else if( (wdef[i].X2 == X) && (wdef[i].Y2 == Y) && (wdef[i].Z2 == Z) ){
				Con[i] = BYTE(w);
				Connect(w, wdef[i].X1, wdef[i].Y1, wdef[i].Z1);
			}
		}
	}
}

void CWGroupList::Sort(void)
{
	int	gap, i, j;
	CWGroup	cw;

	// X座標でソートする
	for( gap = gmax/2; gap > 0; gap /= 2 ){
		for( i = gap; i < gmax; i++ ){
			for( j = i - gap; j >= 0; j -= gap ){
				if( List[j].GetX() <= List[j+gap].GetX() ) break;
				memcpy(&cw, &List[j], sizeof(CWGroup));
				memcpy(&List[j], &List[j+gap], sizeof(CWGroup));
				memcpy(&List[j+gap], &cw, sizeof(CWGroup));
			}
		}
		cw.Clear();
	}
	// ベース位置を探す（Ｘが一番小さな値をベース位置にする）
	double minx = MAXDOUBLE;
	for( BaseW = i = 0; i < gmax; i++ ){
		if( minx > ABS(List[i].GetX()) ){
			minx = ABS(List[i].GetX());
			BaseW = i;
		}
	}
	double	d;
	for( d = List[BaseW].GetX(), i = BaseW; i >= 0; i-- ){
		Intval[i] = d - List[i].GetX();
		d = List[i].GetX();
	}
	for( d = List[BaseW].GetX(), i = BaseW + 1; i < gmax; i++ ){
		Intval[i] = List[i].GetX() - d;
		d = List[i].GetX();
	}
}

void CWGroupList::Delete(void)
{
	for( int i = 0; i < gmax; i++ ){
		List[i].Delete();
	}
	memset(Con, 0xff, sizeof(Con));
	gmax = 0;
}

double CWGroupList::GetXPosition(int n)
{
	double	d = 0.0;
	int i;
	for( i = 0; (i < n) && (i < gmax); i++ ){
		if( i == BaseW ){
			i++, n++;
		}
		d += Intval[i];
	}
	return d;
}

#if 0
int CWGroupList::SetXPosition(int n, double d)
{
	d -= GetXPosition(n);
	if( n == BaseW ){			// 基準位置
		if( ((n + 1) == gmax) || (Intval[n+1] >= d) ){
			if( n ) Intval[n-1] += d;
			if( (n + 1) < gmax ) Intval[n+1] -= d;
			return TRUE;
		}
	}
	else if( n < BaseW ){
		if( Intval[n] >= d ){
			if( n ) Intval[n-1] += d;
			Intval[n] -= d;
			return TRUE;
		}
	}
	else {
		if( ((n + 1) == gmax) || (Intval[n+1] >= d) ){
			Intval[n] += d;
			if( (n + 1) < gmax ) Intval[n+1] -= d;
			return TRUE;
		}
	}
	return FALSE;
}
#else
int CWGroupList::SetXPosition(int n, double d)
{
	d -= GetXPosition(n);
	if( n == BaseW ){			// 基準位置
		if( n ) Intval[n-1] += d;
		if( (n + 1) < gmax ) Intval[n+1] -= d;
	}
	else if( n < BaseW ){
		if( n ) Intval[n-1] += d;
		Intval[n] -= d;
	}
	else {
		Intval[n] += d;
		if( (n + 1) < gmax ) Intval[n+1] -= d;
	}
	return TRUE;
}
#endif

void CWGroupList::SetWire(ANTDEF *p)
{
	int i, j;

	Delete();
	ap = p;
	wmax = p->wmax;
	memcpy(wdef, p->wdef, wmax * sizeof(WDEF));
	for( i = 0; i < wmax; i++ ){
		wdef[i].PNo = 1;
		if( Con[i]==0xff ){
			Con[i] = BYTE(gmax);
			Connect(gmax, wdef[i].X1, wdef[i].Y1, wdef[i].Z1);
			Connect(gmax, wdef[i].X2, wdef[i].Y2, wdef[i].Z2);
			gmax++;
		}
	}
	for( i = 0; i < gmax; i++ ){
		List[i].Init(wdef);
		for( j = 0; j < wmax; j++ ){
			if( Con[j] == BYTE(i) ){
				List[i].AddWire(j);
			}
		}
		List[i].AnaWire();
	}
	Sort();
}

void CWGroupList::Update(void)
{
	int		i, j;
	int		n;

	int DelFlag = 0;
	for( i = 0; i < wmax; i++ ){		// 歯抜けをサーチ
		if( !wdef[i].PNo ){
			DelFlag = 1;
			break;
		}
	}
	WREF	*rp;
	double	B = List[BaseW].GetX();
	double	bx = 0.0;
	for( i = BaseW, n = 0; i >= 0; i-- ){
		bx -= Intval[i];
		for( rp = List[i].pB, j = 0; j < List[i].bcnt; j++, rp++ ){
			if( rp->wp->PNo ){
				if( !DelFlag ) n = rp->ref;
				memcpy(&ap->wdef[n], rp->wp, sizeof(WDEF));
				ap->wdef[n].X1 += bx + B;
				ap->wdef[n].X2 += bx + B;
				n++;
			}
		}
	}
	bx = 0.0;
	for( i = BaseW + 1; i < gmax; i++ ){
		bx += Intval[i];
		for( rp = List[i].pB, j = 0; j < List[i].bcnt; j++, rp++ ){
			if( rp->wp->PNo ){
				if( !DelFlag ) n = rp->ref;
				memcpy(&ap->wdef[n], rp->wp, sizeof(WDEF));
				ap->wdef[n].X1 += bx + B;
				ap->wdef[n].X2 += bx + B;
				n++;
			}
		}
	}
	ap->wmax = DelFlag ? n : wmax;
}

void CWGroupList::CreateWire(int w, int type)
{
	int wm;
	if( w ){
		List[w].Init(wdef);
		for( int i = 0; i < List[w-1].bcnt; i++ ){
			wm = GetNewWire();
			memcpy(&wdef[wm], List[w-1].pB[i].wp, sizeof(WDEF));
			List[w].AddWire(wm);
		}
		List[w].AnaWire();
		if( Intval[w-1] ){
			Intval[w] = Intval[w-1];
		}
		else if( (w >= 2) && Intval[w-2] ){
			Intval[w] = Intval[w-2];
		}
		else {
			Intval[w] = exeenv.rmd/6;
		}
		gmax++;
	}
	else {
		SetNewType(w, type);
	}
}

void CWGroupList::InsertItem(int w)
{
	if( w < gmax ){
		int i;
		for( i = gmax;i > w; i-- ){
			memcpy(&List[i], &List[i-1], sizeof(CWGroup));
			memcpy(&Intval[i], &Intval[i-1], sizeof(double));
		}
		List[w].Clear();
		int wm;
		List[w].Init(wdef);
		for( i = 0; i < List[w+1].bcnt; i++ ){
			wm = GetNewWire();
			memcpy(&wdef[wm], List[w+1].pB[i].wp, sizeof(WDEF));
			List[w].AddWire(wm);
		}
		List[w].AnaWire();
		if( Intval[w+1] ){
			Intval[w] = Intval[w+1];
		}
		else if( w && Intval[w-1] ){
			Intval[w] = Intval[w-1];
		}
		else {
			Intval[w] = exeenv.rmd/6;
		}
		gmax++;
		if( w <= BaseW ) BaseW++;
	}
	else {
		CreateWire(w, cwHDP);
	}
}

void CWGroupList::DeleteItem(int n)
{
	if( n >= gmax ) return;
	DeleteWire(n);
	// テーブルから削除
	int i;
	for( i = n; i < (gmax - 1); i++ ){
		memcpy(&List[i], &List[i+1], sizeof(CWGroup));
		memcpy(&Intval[i], &Intval[i+1], sizeof(double));
	}
	List[i].Clear();
	gmax--;
	if( n <= BaseW ) BaseW--;
}

void CWGroupList::DeleteWire(int n)
{
	if( n >= gmax ) return;
	// 関連ワイヤを削除
	int i;
	for( i = 0; i < List[n].bcnt; i++ ){
		List[n].pB[i].wp->PNo = 0;
	}
	List[n].Delete();
}

void CWGroupList::SetNewType(int n, int type)
{
	if( n > gmax ) return;
	int		wm;
	double	rmd = exeenv.rmd;
	if( !rmd ) rmd = 23.0;
	double	Z = 0.0;
	double	Round = rmd;
	double	Width = rmd/2;
	double	Height = rmd/2;
	double	Rad = 0.005;
	if( n < gmax ){
		if( type == List[n].Type ) return;	// 同一タイプ
		Z = List[n].BZ;
		Round = List[n].GetRound();
		Width = List[n].GetWidth();
		Height = List[n].GetHeight();
		Rad = List[n].GetR();
	}
	if( n < gmax ) DeleteWire(n);
	switch(type){
		case cwHDP: // 水平DP
			List[n].Init(wdef);
			wm = AddWire(0, -1.0, 0, 0, 1.0, 0, Rad, 0);
			List[n].AddWire(wm);
			List[n].AnaWire();
			List[n].SetWidth(Round ? (Round/2.0) : Width ? Width : Height ? Height : rmd/2);
			break;
		case cwVDP:	// 垂直DP
			List[n].Init(wdef);
			wm = AddWire(0, 0, -1.0, 0, 0, 1.0, Rad, 0);
			List[n].AddWire(wm);
			List[n].AnaWire();
			List[n].SetHeight(Round ? (Round/2.0) : Width ? Width : Height ? Height : rmd/2);
			break;
		case cwVSQ:	// 垂直□
			List[n].Init(wdef);
			wm = AddWire(0, -1, -1, 0, 1, -1, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(0, 1, -1, 0, 1, 1, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(0, 1, 1, 0, -1, 1, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(0, -1, 1, 0, -1, -1, Rad, -1);
			List[n].AddWire(wm);
			List[n].AnaWire();
			List[n].SetRound(Round ? Round : Width ? Width * 2.0 : Height ? Height * 2.0 : rmd);
			break;
		case cwVDM:	// 垂直◇
			List[n].Init(wdef);
			wm = AddWire(0, 1, 0, 0, 0, -1, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(0, 0, -1, 0, -1, 0, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(0, -1, 0, 0, 0, 1, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(0, 0, 1, 0, 1, 0, Rad, -1);
			List[n].AddWire(wm);
			List[n].AnaWire();
			List[n].SetRound(Round ? Round : Width ? Width * 2.0 : Height ? Height * 2.0 : rmd);
			break;
		case cwVTA:	// 垂直△
			List[n].Init(wdef);
			wm = AddWire(0, COS30, -0.5, 0, -COS30, -0.5, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(0, -COS30, -0.5, 0, 0, 1.0, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(0, 0, 1.0, 0, COS30, -0.5, Rad, -1);
			List[n].AddWire(wm);
			List[n].AnaWire();
			List[n].SetRound(Round ? Round : Width ? Width * 2.0 : Height ? Height * 2.0 : rmd);
			break;
		case cwVITA:// 垂直▽
			List[n].Init(wdef);
			wm = AddWire(0, COS30, 0.5, 0, -COS30, 0.5, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(0, -COS30, 0.5, 0, 0, -1.0, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(0, 0, -1.0, 0, COS30, 0.5, Rad, -1);
			List[n].AddWire(wm);
			List[n].AnaWire();
			List[n].SetRound(Round ? Round : Width ? Width * 2.0 : Height ? Height * 2.0 : rmd);
			break;
		case cwVPIE:	// 垂直○
			List[n].Init(wdef);
			wm = AddWire(0, SIN22R5, -COS22R5, 0, -SIN22R5, -COS22R5, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(0, -SIN22R5, -COS22R5, 0, -COS22R5, -SIN22R5, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(0, -COS22R5, -SIN22R5, 0, -COS22R5, SIN22R5, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(0, -COS22R5, SIN22R5, 0, -SIN22R5, COS22R5, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(0, -SIN22R5, COS22R5, 0, SIN22R5, COS22R5, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(0, SIN22R5, COS22R5, 0, COS22R5, SIN22R5, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(0, COS22R5, SIN22R5, 0, COS22R5, -SIN22R5, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(0, COS22R5, -SIN22R5, 0, SIN22R5, -COS22R5, Rad, -1);
			List[n].AddWire(wm);
			List[n].AnaWire();
			List[n].SetRound(Round ? Round : Width ? Width * 2.0 : Height ? Height * 2.0 : rmd);
			break;
		case cwVSQ2:	// 垂直双□
			List[n].Init(wdef);
			wm = AddWire(0, -2, 1, 0, 0, 1, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(0, 0, 1, 0, 2, 1, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(0, 2, 1, 0, 2, -1, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(0, 2, -1, 0, 0, -1, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(0, 0, -1, 0, -2, -1, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(0, -2, -1, 0, -2, 1, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(0, 0, 1, 0, 0, -1, Rad, -1);
			List[n].AddWire(wm);
			List[n].AnaWire();
#define	_KVSQ2	1.5
			List[n].SetRound(Round ? (Round*_KVSQ2) : Width ? (Width * _KVSQ2*2) : Height ? (Height * _KVSQ2*2) : (rmd * _KVSQ2));
			break;
		case cwVTA2:	// 垂直双▽
			List[n].Init(wdef);
			wm = AddWire(0, -2, 1, 0, 0, 0.01, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(0, 0, 0.01, 0, 2, 1, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(0, 2, 1, 0, 2, -1, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(0, 2, -1, 0, 0, -0.01, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(0, 0, -0.01, 0, -2, -1, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(0, -2, -1, 0, -2, 1, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(0, 0, 0.01, 0, 0, -0.01, Rad, -1);
			List[n].AddWire(wm);
			List[n].AnaWire();
#define	_KVTA2	0.71
			Round = Round ? (Round*_KVTA2) : Width ? (Width*_KVTA2*2.0) : Height ? (Height*_KVTA2*2.0) : (rmd*_KVTA2*2.0);
			List[n].SetWidth(Round);
			List[n].SetHeight(Round/2);
			break;
		case cwHSQ:	// 水平□
			List[n].Init(wdef);
			wm = AddWire(-1, -1, Z, 1, -1, Z, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(1, -1, Z, 1, 1, Z, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(1, 1, Z, -1, 1, Z, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(-1, 1, Z, -1, -1, Z, Rad, -1);
			List[n].AddWire(wm);
			List[n].AnaWire();
			List[n].SetRound(Round ? Round : Width ? Width * 2.0 : Height ? Height * 2.0 : rmd);
			break;
		case cwHDM:	// 水平◇
			List[n].Init(wdef);
			wm = AddWire(0, -1, Z, -1, 0, Z, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(-1, 0, Z, 0, 1, Z, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(0, 1, Z, 1, 0, Z, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(1, 0, Z, 0, -1, Z, Rad, -1);
			List[n].AddWire(wm);
			List[n].AnaWire();
			List[n].SetRound(Round ? Round : Width ? Width * 2.0 : Height ? Height * 2.0 : rmd);
			break;
		case cwHTA:	// 水平△
			List[n].Init(wdef);
			wm = AddWire(0.5, COS30, Z, 0.5, -COS30, Z, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(0.5, -COS30, Z, -1.0, 0, Z, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(-1.0, 0, Z, 0.5, COS30, Z, Rad, -1);
			List[n].AddWire(wm);
			List[n].AnaWire();
			List[n].SetRound(Round ? Round : Width ? Width * 2.0 : Height ? Height * 2.0 : rmd);
			break;
		case cwHITA:// 水平▽
			List[n].Init(wdef);
			wm = AddWire(-0.5, COS30, Z, -0.5, -COS30, Z, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(-0.5, -COS30, Z, 1.0, 0, Z, Rad, -1);
			List[n].AddWire(wm);
			wm = AddWire(1.0, 0, Z, -0.5, COS30, Z, Rad, -1);
			List[n].AddWire(wm);
			List[n].AnaWire();
			List[n].SetRound(Round ? Round : Width ? Width * 2.0 : Height ? Height * 2.0 : rmd);
			break;
	}
	if( n >= gmax ) gmax++;
}
///----------------------------------------------------------------
/// ワイヤ番号からエレメント番号を得る
int CWGroupList::GetEleNo(int w)
{
	WREF *rp;
	int i, n;

	for( i = 0; i < gmax; i++ ){
		for(rp = List[i].pB, n = List[i].bcnt; n; n--, rp++ ){
			if( rp->ref == w ) return i;
		}
	}
	return -1;
}
///----------------------------------------------------------------
/// ワイヤからグループポインタを得る
CWGroup* CWGroupList::GetCW(int w)
{
	int e;

	if( (e = GetEleNo(w))!=-1 ){
		return &List[e];
	}
	else {
		return NULL;
	}
}
///----------------------------------------------------------------
/// ワイヤ番号からX軸絶対位置を得る
double CWGroupList::GetXAbs(int w)
{
	int e;

	if( (e = GetEleNo(w))!=-1 ){
		return GetXPosition(e) + List[0].GetX();
	}
	else {
		return 0.0;
	}
}
///----------------------------------------------------------------
///  CACal クラス
CACal::CACal(void)
{
	amax = 0;
	memset(aval, 0, sizeof(aval));
	memset(&atrg, 0, sizeof(atrg));
	atrg.TF = 1;
	atrg.TG = 50.0;
	atrg.TFB = 50.0;
	atrg.TJX = 50.0;
	strcpy(atrg.IPLUS, "w1c");
	atrg.MaxGain = 100.0;
	atrg.MaxFB = 25.0;
	atrg.MinJX = 0;
	atrg.MinSWR = 1.0;
	atrg.Match = 0;
	atrg.TergR = env.RO;
	atrg.TergJX = 0;
	PitchUnit = 1;
	DegUnit = 1;
	atrg.bweight = 1;
	AP = NULL;
}
// ワイヤの調整
void CACal::AdjWireChen(int w, WDEF *op, WDEF *np)
{
	CWGroup *cp = WList.GetCW(w);		// エレメントの参照を得る
	if( cp == NULL ) return;

	WREF *rp = cp->pB;
	int i;
	for( i = 0; i < cp->bcnt; i++, rp++ ){
		if( w != rp->ref ){
			if( (np->X1 != op->X1) || (np->Y1 != op->Y1) || (np->Z1 != op->Z1) ){
				::AdjWireChen(rp->wp, op->X1, op->Y1, op->Z1, np->X1, np->Y1, np->Z1);
			}
			if( (np->X2 != op->X2) || (np->Y2 != op->Y2) || (np->Z2 != op->Z2) ){
				::AdjWireChen(rp->wp, op->X2, op->Y2, op->Z2, np->X2, np->Y2, np->Z2);
			}
		}
	}
}
// 単純ワイヤ
void CACal::SetParaWire(AVAL *ap, double d)
{
	WDEF *wp = &WList.wdef[ap->Pos];
	WDEF OldW;
	memcpy(&OldW, wp, sizeof(WDEF));
	switch(ap->SubPos){
		case 0:
			wp->Y1 = d;
			break;
		case 1:
			wp->Z1 = d;
			break;
		case 2:
			wp->Y2 = d;
			break;
		case 3:
			wp->Z2 = d;
			break;
		case 4:
			wp->R = d;
			break;
		case 5:
			wp->X1 = d - WList.GetXAbs(ap->Pos);
			break;
		case 6:
			wp->X2 = d - WList.GetXAbs(ap->Pos);
			break;
	}
	AdjWireChen(ap->Pos, &OldW, wp);
}
// 極座標ワイヤ
void CACal::SetParaWireP(AVAL *ap, double d)
{
	WDEF *wp = &WList.wdef[ap->Pos];
	WDEF OldW;
	memcpy(&OldW, wp, sizeof(WDEF));
	double WL;
	double XYD;
	double YZD;
	GetWirePoll(WL, XYD, YZD, wp);
	int	sw = 0;
	switch(ap->SubPos){
		case 0:		// ワイヤ長（始点基準）
			WL = d;
			sw = 0;
			break;
		case 1:		// ワイヤ長（終点基準）
			WL = d;
			sw = 1;
			break;
		case 2:		// ワイヤ長（中心基準）
			WL = d;
			sw = 2;
			break;
		case 3:		// 径度（始点基準）
			XYD = d;
			sw = 0;
			break;
		case 4:		// 径度（終点基準）
			XYD = d;
			sw = 1;
			break;
		case 5:		// 径度（中心基準）
			XYD = d;
			sw = 2;
			break;
		case 6:		// 天頂角（始点基準）
			YZD = d;
			sw = 0;
			break;
		case 7:		// 天頂角（終点基準）
			YZD = d;
			sw = 1;
			break;
		case 8:		// 天頂角（中心基準）
			YZD = d;
			sw = 2;
			break;
	}
	double XX = WL * sin(YZD*PAI/180.0) * cos(XYD*PAI/180.0);
	double YY = WL * sin(YZD*PAI/180.0) * sin(XYD*PAI/180.0);
	double ZZ = WL * cos(YZD*PAI/180.0);
	double XC = (wp->X1 + wp->X2)/2.0;
	double YC = (wp->Y1 + wp->Y2)/2.0;
	double ZC = (wp->Z1 + wp->Z2)/2.0;
	switch(sw){
		case 0:		// 始点
			wp->X2 = wp->X1 + XX;
			wp->Y2 = wp->Y1 + YY;
			wp->Z2 = wp->Z1 + ZZ;
			break;
		case 1:		// 終点
			wp->X1 = wp->X2 - XX;
			wp->Y1 = wp->Y2 - YY;
			wp->Z1 = wp->Z2 - ZZ;
			break;
		case 2:		// 中心
			XX /= 2.0;
			YY /= 2.0;
			ZZ /= 2.0;
			wp->X1 = XC - XX;
			wp->Y1 = YC - YY;
			wp->Z1 = ZC - ZZ;
			wp->X2 = XC + XX;
			wp->Y2 = YC + YY;
			wp->Z2 = ZC + ZZ;
			break;
	}
	AdjWireChen(ap->Pos, &OldW, wp);
}

void CACal::SetParaEle(AVAL *ap, double d)
{
	CWGroup	*cp = &WList.List[ap->Pos];
	switch(ap->SubPos){
		case 0:		// 間隔
			WList.Intval[ap->Pos] = d;
			break;
		case 1:		// 横幅
			cp->SetWidth(d);
			break;
		case 2:		// 縦幅
			cp->SetHeight(d);
			break;
		case 3:		// 全周 or Ｘ幅
			cp->SetRound(d);
			break;
		case 4:		// Ｒ
			cp->SetR(d);
			break;
		case 5:		// 位置
			WList.SetXPosition(ap->Pos, d);
			break;
		case 6:		// 横幅
			cp->SetWidthAll(d);
			break;
		case 7:		// 縦幅
			cp->SetHeightAll(d);
			break;
		case 8:		// 全周 or Ｘ幅
			cp->SetRoundAll(d);
			break;
	}
}
void CACal::SetParaLoad(AVAL *ap, double d)
{
	LDEF *lp = &AP->ldef[ap->Pos];
	if( (lp->TYPE == 0) && lp->A[0] && lp->B[0] ){		// 共振周波数を一定にする
		double w2 = 1/(lp->A[0] * lp->B[0]);
		if( ap->SubPos ){	// C
			lp->B[0] = d;
			lp->A[0] = 1/(w2 * d);
		}
		else {				// L
			lp->A[0] = d;
			lp->B[0] = 1/(w2 * d);
		}
	}
	else if( ap->SubPos ){	// C
		lp->B[0] = d;
	}
	else {					// L
		lp->A[0] = d;
	}
}
void CACal::SetParaFeed(AVAL *ap, double d)
{
	if( ap->SubPos ){
		AP->cdef[ap->Pos].EV = d;
	}
	else {
		AP->cdef[ap->Pos].DEG = d;
	}
}
void CACal::SetParaStack(AVAL *ap, double d){
	switch(ap->SubPos){
		case 0:
			AP->StackHW = AP->StackVW = d;
			break;
		case 1:
			AP->StackHW = d;
			break;
		case 2:
			AP->StackVW = d;
			break;
	}
}
double CACal::GetParaWire(AVAL *ap)
{
	WDEF *wp = &WList.wdef[ap->Pos];
	switch(ap->SubPos){
		case 0:
			return wp->Y1;
		case 1:
			return wp->Z1;
		case 2:
			return wp->Y2;
		case 3:
			return wp->Z2;
		case 4:
			return wp->R;
		case 5:
			return wp->X1 + WList.GetXAbs(ap->Pos);
		case 6:
			return wp->X2 + WList.GetXAbs(ap->Pos);
	}
	return 0.0;
}
// 極座標ワイヤ
double CACal::GetParaWireP(AVAL *ap)
{
	WDEF *wp = &WList.wdef[ap->Pos];
	double WL, XYD, YZD;
	GetWirePoll(WL, XYD, YZD, wp);
	switch(ap->SubPos){
		case 0:		// ワイヤ長（始点基準）
		case 1:		// ワイヤ長（終点基準）
		case 2:		// ワイヤ長（中心基準）
			return WL;
		case 3:		// 径度（始点基準）
		case 4:		// 径度（終点基準）
		case 5:		// 径度（中心基準）
			return XYD;
		case 6:		// 天頂角（始点基準）
		case 7:		// 天頂角（終点基準）
		case 8:		// 天頂角（中心基準）
			return YZD;
	}
	return 0.0;
}
double CACal::GetParaEle(AVAL *ap)
{
	CWGroup	*cp = &WList.List[ap->Pos];
	switch(ap->SubPos){
		case 0:		// 間隔
			return WList.Intval[ap->Pos];
		case 1:		// 横幅
		case 6:
			return cp->GetWidth();
		case 2:		// 縦幅
        case 7:
			return cp->GetHeight();
		case 3:		// 全周 or Ｘ幅
        case 8:
			return cp->GetRound();
		case 4:		// Ｒ
			return cp->GetR();
		case 5:		// 位置
			return WList.GetXPosition(ap->Pos);
	}
	return 0.0;
}
double CACal::GetParaLoad(AVAL *ap){
	if( ap->SubPos ){		// C
		return AP->ldef[ap->Pos].B[0];
	}
	else {					// L
		return AP->ldef[ap->Pos].A[0];
	}
}

double CACal::GetParaFeed(AVAL *ap){
	if( ap->SubPos ){
		return AP->cdef[ap->Pos].EV;
	}
	else {
		return AP->cdef[ap->Pos].DEG;
	}
}

double CACal::GetParaStack(AVAL *ap){
	if( ap->SubPos == 2 ){
		return AP->StackVW;
	}
	else {
		return AP->StackHW;
	}
}

void CACal::SetPara(int n, double d)
{
	aval[n].DC = d;
	switch(aval[n].Type){
		case avWIRE:
			SetParaWire(&aval[n], d);
			break;
		case avELE:
			SetParaEle(&aval[n], d);
			break;
		case avLOAD:
			SetParaLoad(&aval[n], d);
			break;
		case avHEIGHT:
			EP->antheight = d;
			break;
		case avFREQ:
			AP->cfq = d;
			atrg.bdef[0].fq = d;
			break;
		case avWIREP:
			SetParaWireP(&aval[n], d);
			break;
		case avFEED:
			SetParaFeed(&aval[n], d);
			break;
		case avSTACK:
			SetParaStack(&aval[n], d);
			break;
	}
}

void CACal::SetPara(int n)
{
	if( aval[n].With == -1 ){			// 独立変数の時
		SetPara(n, aval[n].DC);
		AVAL *ap = aval;
		int i;
		for( i = 0; i < amax; i++, ap++ ){
			if( ap->With == n ){
				SetPara(i, aval[n].DC);
			}
			else if( (-ap->With-2) == n ){
				SetPara(i, -aval[n].DC);
			}
			else if( (ap->With & 0x0000ffff) == n ){
				if( ap->With & 0x00010000 ){
					SetPara(i, aval[n].DC + ap->DW);
				}
				else if( ap->With & 0x00020000 ){
					SetPara(i, -aval[n].DC + ap->DW);
				}
				else if( ap->With & 0x00040000 ){
					SetPara(i, aval[n].DC * ap->DW);
				}
				else if( ap->With & 0x00080000 ){
					SetPara(i, aval[n].DC / ap->DW);
				}
			}
		}
	}
}
// 給電点の設定を行う
void CACal::SetBand(ANTDEF *ap, int n)
{
	if( ap->cmax > 4 ) return;

	CDEF *cp = atrg.bdef[n].cdef;
	ap->cfq = atrg.bdef[n].fq;
	for( ap->cmax = 0; cp->PLUS[0] && (ap->cmax < 4); cp++, ap->cmax++ ){
		memcpy(&ap->cdef[ap->cmax], cp, sizeof(CDEF));
	}
}

void CACal::SetNowCur(RESL *rp)
{
	atrg.CG = rp->GAIN;
	atrg.CFB = rp->FB;
	atrg.CRR = rp->R;
	atrg.CJX = rp->JX;
	atrg.CSWR = rp->SWR;
	atrg.CELV = rp->ELV;
}

// 評価値を得る
double CACal::CalcV(RESL *rp, CRes *cp)
{
	double V;

	if( atrg.TF ){
		double fb = rp->FB;
		if( fb > atrg.MaxFB ) fb = atrg.MaxFB + log(fb/atrg.MaxFB);
		fb *= atrg.TFB;
		double gain = rp->GAIN;
		if( gain > atrg.MaxGain ) gain = atrg.MaxGain + log(gain/atrg.MaxGain);
		gain *= atrg.TG;
		double jx = ABS(rp->JX);
		if( jx < atrg.MinJX ){
			jx = (atrg.MinJX - ((atrg.MinJX - jx)/20.0)) * atrg.TJX;
		}
		else {
			jx *= atrg.TJX;
		}
		jx = -jx;
		double swr = rp->SWR;
		if( swr < atrg.MinSWR ){
			swr = (atrg.MinSWR - ((atrg.MinSWR - swr)/20.0)) * atrg.TSWR;
		}
		else {
			swr = (swr - 1.0) * atrg.TSWR;
		}
		swr = -swr;
//		double swr = 1.0 - (rp->SWR * atrg.TSWR);
		double elv = -(rp->ELV * atrg.TELV);
		double mch = 0.0;
		if( atrg.TM ){
			double s;
			if( atrg.Match == 2 ){
				s = (atrg.TergR == NULLV) ? 1.0 : (rp->R - atrg.TergR);
				mch = (atrg.TergJX == NULLV) ? 1.0 : (rp->JX - atrg.TergJX);
				mch = -sqrt(4 * s * s + mch * mch);
			}
			else if( rp->R <= env.RO ){
				double s = sqrt((env.RO/rp->R)-1);
				mch = (rp->R * s);
				if( atrg.Match ){		// ヘアピン
					mch += rp->JX;
				}
				else {					// コンデンサ
					mch -= rp->JX;
				}
				mch = -ABS(mch);
			}
			else {
				s = 4 * env.RO * env.RO * rp->JX * rp->JX - 4 * (env.RO - rp->R) * env.RO * (rp->R*rp->R + rp->JX*rp->JX);
				if( s >= 0 ){
					s = sqrt(s);
					double b = -2 * env.RO * rp->JX;
					double v = 2 * (env.RO - rp->R);
					double MX1 = (b + s)/v;
					double MX2 = -(MX1 * rp->R * rp->R + (rp->JX + MX1)*rp->JX*MX1)/(rp->R*rp->R+(rp->JX+MX1)*(rp->JX+MX1));
					mch = -ABS(MX2);
				}
				else {
					mch = -rp->R;
				}
				mch -= 100.0;
			}
			mch *= atrg.TM;
		}
		double cur = 0.0;
		if( atrg.TI && atrg.IPLUS[0] ){
			int n = Str2PlusNo(AP, atrg.IPLUS);
			if( n ){
				atrg.CurCur = ABS(cp->Cur[n-1]);
				cur = atrg.IType ? -atrg.CurCur : atrg.CurCur;
				cur *= atrg.TI * 100;
			}
		}
		fb /= 10.0;
		jx /= 50.0;
		mch /= 50.0;
		V = gain + fb + jx + swr + elv + mch + cur;
	}
	else {
		V = atrg.V - 0.1;
		if( atrg.TI && atrg.IPLUS[0] ){
			int n = Str2PlusNo(AP, atrg.IPLUS);
			if( n ){
				atrg.CurCur = ABS(cp->Cur[n-1]);
			}
		}
	}
	return V;
}

void CACal::Init(ANTDEF *ap, ENVDEF *ep)
{
	AP = ap;
	EP = ep;
	WList.SetWire(ap);
}

int CACal::IsSameMax(void)
{
	AVAL *ap;
	int i;
	double *dp = atrg.AD;
	for( i = 0, ap = aval; i < amax; i++, ap++, dp++ ){
		if( ap->DC != *dp ) return FALSE;
	}
	return TRUE;
}

int CACal::WriteToFp(FILE *fp)		// セットアップのセーブ
{
	fwrite(&amax, 1, sizeof(amax), fp);
	if( amax ){
		fwrite(aval, 1, sizeof(aval), fp);
	}
	fwrite(&atrg, 1, sizeof(atrg), fp);
	fwrite(&PitchUnit, 1, sizeof(PitchUnit), fp);
	fwrite(&DegUnit, 1, sizeof(DegUnit), fp);

	fwrite(&mvmax, 1, sizeof(mvmax), fp);
	if( mvmax ){
		fwrite(&mvwp, 1, sizeof(mvwp), fp);
		fwrite(mval, 1, sizeof(mval), fp);
	}
	return ferror(fp) ? FALSE : TRUE;
}

int CACal::ReadFromFp(FILE *fp)		// セットアップのロード
{
	fread(&amax, 1, sizeof(amax), fp);
	if( amax ){
		fread(aval, 1, sizeof(aval), fp);
	}
	fread(&atrg, 1, sizeof(atrg), fp);
	fread(&PitchUnit, 1, sizeof(PitchUnit), fp);
	fread(&DegUnit, 1, sizeof(DegUnit), fp);

	fread(&mvmax, 1, sizeof(mvmax), fp);
	if( mvmax ){
		fread(&mvwp, 1, sizeof(mvwp), fp);
		fread(mval, 1, sizeof(mval), fp);
	}
	return ferror(fp) ? FALSE : TRUE;
}

void CACal::WriteMV(void)
{
	MVAL	*mp = &mval[mvwp];

	mp->V = atrg.V;
	double *dp = mp->AD;
	AVAL *ap = aval;
	int i;
	for( int i = 0; i < amax; i++, dp++, ap++ ){
		*dp = ap->DC;
	}
	mp->CG = atrg.CG;
	mp->CFB = atrg.CFB;
	mp->CRR = atrg.CRR;
	mp->CJX = atrg.CJX;
	mp->CSWR = atrg.CSWR;
	mp->CELV = atrg.CELV;
	float *pp;
	int N = 2/res.PtnF;
	for( pp = res.PtnH[2], i = 0; i < 180; i++, pp += N ){			// 水平パターンをコピー
		mp->PtnH[i] = ToFix(*pp);
	}
	for( pp = res.PtnV[2], i = 0; i <= 90; i++, pp += N ){			// 垂直パターンをコピー
		mp->PtnV[i] = ToFix(*pp);
	}
	mvwp++;
	if( mvwp >= MVMAX ) mvwp = 0;
	mvmax++;
	if( mvmax > MVMAX ) mvmax--;
}

int CACal::IsMV(double &max)
{
	AVAL	*ap;
	double	*dp;
	int i, j;
	MVAL	*mp = &mval[mvwp - 1];
	for( i = 0; i < mvmax; i++, mp-- ){
		if( mp < mval ) mp = &mval[MVMAX-1];
		dp = mp->AD;
		ap = aval;
		for( j = 0; j < amax; j++, dp++, ap++ ){
			if( *dp != ap->DC ){
				goto _nx;
			}
		}
		max = mp->V;
		return TRUE;
		_nx:;
	}
	return FALSE;
}

void CACal::ReadMV(MVAL *wp, int n)
{
	MVAL	*mp = &mval[mvwp - n - 1];
	if( mp < mval ) mp += MVMAX;
	memcpy(wp, mp, sizeof(MVAL));
}

int CACal::_GetLastChen(int c, int n)
{
	if( aval[n].With == -1 ){
		return n;
	}
	else {
		if( c >= amax ){
			return -1;
		}
		else {
			return _GetLastChen(c + 1, GetChenNo(aval[n].With));
		}
	}
}

int CACal::GetLastChen(int c, int n)
{
	int s = n;
	n = _GetLastChen(c, GetChenNo(n));
	if( n == -1 ) return -1;
	if( s < 0 ){
		n = - n - 2;
	}
	else if( s & 0x000f0000 ){
		n |= (s & 0x000f0000);
	}
	return n;
}

int CACal::InitPara(void)
{
	AVAL	*ap;
	int		i;

	mvwp = mvmax = 0;
	for( i = 0, ap = aval; i < amax; i++, ap++ ){
		if( ap->With != -1 ){
			if( (ap->With = GetLastChen(0, ap->With)) == -1 ){
				ErrorMB("変数(%d): 連携先の独立変数が見つかりません.", i+1);
				return FALSE;
			}
		}
		else {
			ap->DC = GetNowValue(i);
			if( !ap->DC && (((ap->Type==avELE)&&(ap->SubPos<5)) || (ap->Type==avLOAD)) ){
				ErrorMB("変数(%d): この変数は可変できません.", i+1);
				return FALSE;
			}
			atrg.AD[i] = -ap->DC;
			if( PitchUnit ){
				ap->DI = ap->DW;
			}
			else {
				ap->DI = (ap->DC * ap->DW) / 100.0;
			}
			ap->DI = ABS(ap->DI);
			ap->DIO = ap->DI;
		}
	}
	atrg.V = 1.0;
	atrg.VM = -MAXDOUBLE;
	atrg.AM = -MAXDOUBLE;
	atrg.CurCur = -1;
	if( !atrg.TF ) atrg.bmax = 1;
	for( i = 0; i < atrg.bmax; i++ ){
		BVM[i] = -MAXDOUBLE;
	}
	return TRUE;
}

void CACal::SetTradeOff(int gain, int fb, int jx, int swr, int elv, int mch, int cur)
{
	int dm = gain + fb + jx + swr + elv + mch + cur;
	if( !dm ) gain = fb = jx = swr = elv = mch = cur = 1;
	atrg.TG = double(gain);
	atrg.TFB = double(fb);
	atrg.TJX = double(jx);
	atrg.TSWR = double(swr);
	atrg.TELV = double(elv);
	atrg.TM = double(mch);
	atrg.TI = double(cur);
}

int CACal::Execute(void)
{
	int r = FALSE;
	if( amax ){
		r = Exec();
		for( int i = 0; i < amax; i++ ){
			SetPara(i, atrg.AD[i]);		// 最終値を設定する
		}
		WList.Update();
	}
	return r;
}

double CACal::GetNowValue(int n)
{
	switch(aval[n].Type){
		case avWIRE:
			return GetParaWire(&aval[n]);
		case avELE:
			return GetParaEle(&aval[n]);
		case avLOAD:
			return GetParaLoad(&aval[n]);
		case avHEIGHT:
			return EP->antheight;
		case avFREQ:
			return AP->cfq;
		case avWIREP:
			return GetParaWireP(&aval[n]);
		case avFEED:
			return GetParaFeed(&aval[n]);
		case avSTACK:
			return GetParaStack(&aval[n]);
	}
	return 0.0;
}

double CACal::GetNowValue(int n, double sc)
{
	if( n < 0 ){
		n = -n - 2;
		return -GetNowValue(n);
	}
	else if( n & 0x00010000 ){
		n &= 0x0000ffff;
		return (GetNowValue(n) + sc);
	}
	else if( n & 0x00020000 ){
		n &= 0x0000ffff;
		return (-GetNowValue(n) + sc);
	}
	else if( n & 0x00040000 ){
		n &= 0x0000ffff;
		return (GetNowValue(n) * sc);
	}
	else if( n & 0x00080000 ){
		n &= 0x0000ffff;
		return (GetNowValue(n) / sc);
	}
	else {
		return GetNowValue(n);
	}
}

void CACal::AdjSetup(int n)
{
	if( n >= AVMAX ) return;
	AVAL *ap = &aval[n];
	switch(ap->Type){
		case avWIRE:
			ap->DMAX = 2000.0;
			ap->DMIN = -2000.0;
			if( ap->Pos >= WList.wmax ){
				ap->Pos = WList.wmax - 1;
			}
			if( ap->SubPos >= 7 ){
				ap->SubPos = 0;
			}
			if( ap->SubPos == 4 ){
				ap->DMIN = 0.0;
				ap->DMAX = 0.5;
			}
			ap->DW = PitchUnit ? GetNormal(GetNowValue(n)/200.0) : 0.5;
			if( !ap->DW ) ap->DW = 0.1;
			break;
		case avELE:
			ap->DMAX = 2000.0;
			ap->DMIN = 0.0;
			if( ap->Pos >= WList.gmax ){
				ap->Pos = WList.gmax - 1;
			}
			if( ap->SubPos >= 9 ){
				ap->SubPos = 0;
			}
			if( ap->SubPos == 4 ){
				ap->DMIN = 0.0;
				ap->DMAX = 0.5;
			}
			ap->DW = PitchUnit ? GetNormal(GetNowValue(n)/200.0) : 0.5;
			if( (!ap->DW) && (ap->SubPos != 5) ) ap->DW = 0.1;
			break;
		case avLOAD:
			ap->DMAX = 10000.0;
			ap->DMIN = 0.0;
			if( ap->Pos >= AP->lmax ){
				ap->Pos = AP->lmax - 1;
			}
			if( ap->SubPos >= 2 ){
				ap->SubPos = 0;
			}
			ap->DW = PitchUnit ? GetNormal(GetNowValue(n)/200.0) : 0.5;
			if( !ap->DW ) ap->DW = 0.1;
			break;
		case avHEIGHT:
			ap->DMAX = 200.0;
			ap->DMIN = 0.0;
			ap->Pos = 0;
			ap->SubPos = 0;
			ap->DW = PitchUnit ? 1.0 : 10.0;
			break;
		case avFREQ:
			ap->DMAX = 1300.0;
			ap->DMIN = 1.5;
			ap->Pos = 0;
			ap->SubPos = 0;
			ap->DW = PitchUnit ? GetNormal(GetNowValue(n)/100.0) : 1.0;
			if( !ap->DW ) ap->DW = 0.1;
			break;
		case avWIREP:
			if( ap->Pos >= WList.wmax ){
				ap->Pos = WList.wmax - 1;
			}
			if( ap->SubPos >= 9 ){
				ap->SubPos = 0;
			}
			if( ap->SubPos <= 2 ){	// 長さ
				ap->DMAX = 2000.0;
				ap->DMIN = 0;
			}
			else {					// 角度
				ap->DMAX = 360.0;
				ap->DMIN = -360.0;
			}
			ap->DW = PitchUnit ? GetNormal(GetNowValue(n)/200.0) : 1.0;
			if( !ap->DW ) ap->DW = 0.1;
			break;
		case avFEED:
			if( ap->SubPos ){
				ap->DMIN = 0;
				ap->DMAX = 1.0;
				ap->DW = 0.01;
			}
			else {
				ap->DMAX = 360.1;
				ap->DMIN = -360.1;
				ap->DW = 1.0;
			}
			if( ap->Pos >= AP->cmax ){
				ap->Pos = AP->cmax - 1;
			}
			if( ap->SubPos > 1 ) ap->SubPos = 1;
			break;
		case avSTACK:
			ap->Pos = 0;
			ap->DMIN = 0.0;
			ap->DMAX = 2000.0;
			ap->DW = PitchUnit ? 0.01 : 1.0;
			if( ap->SubPos > 1 ) ap->SubPos = 1;
			break;
	}
}

int CACal::Check(void)
{
	double	d;
	int i, j;
	AVAL *ap = aval;
	int r = TRUE;
	for( i = 0; i < amax; i++, ap++ ){
		d = GetNowValue(i);
		switch(ap->Type){
			case avWIRE:
				if( ap->Pos >= WList.wmax ){
					r = FALSE;
				}
				if( ap->SubPos >= 7 ){
					r = FALSE;
				}
				break;
			case avELE:
				if( ap->Pos >= WList.gmax ){
					r = FALSE;
				}
				if( ap->SubPos >= 9 ){
					r = FALSE;
				}
				if( !d && (ap->SubPos != 5)){
					ErrorMB("変数(%d): このパラメータは変更できません.", i + 1);
					return FALSE;
				}
				break;
			case avLOAD:
				if( ap->Pos >= AP->lmax ){
					r = FALSE;
				}
				if( ap->SubPos >= 2 ){
					r = FALSE;
				}
				if( AP->ldef[ap->Pos].TYPE == 2 ){
					ErrorMB("変数(%d): この集中定数は可変できません.", i + 1);
				}
				break;
			case avHEIGHT:
				if( !env.type ){
					ErrorMB("変数(%d): 自由空間なので地上高を変化させても意味はありません.", i + 1);
					return FALSE;
				}
				break;
		}
		if( r == FALSE ){
			ErrorMB("変数(%d): この変数は存在しません.", i + 1);
			return r;
		}
		for( j = 0; j < amax; j++ ){
			if( i != j ){
				if( (ap->Type == aval[j].Type)&&(ap->Pos == aval[j].Pos)&&(ap->SubPos==aval[j].SubPos) ){
					ErrorMB("変数(%d): 変数(%d)と重複しています.", i + 1, j+1);
					return FALSE;
				}
			}
		}
		if( ap->With != -1 ){
			if( RoundUpStr(d) != RoundUpStr(GetNowValue(ap->With, ap->DW)) ){
				if( YesNoMB("変数(%d): 連携先の変数(%d)と初期値が異なります.\r\n\r\n連携先の変数の値に強制設定されます。よろしいですか？", i+1, GetChenNo(ap->With)+1) != IDYES ) return FALSE;
			}
		}
		else {
			if( SetMinMax(ap, d) ) SetPara(i, d);
		}
	}
	return TRUE;
}

int CACal::SetMinMax(AVAL *ap, double &d)
{
	if( ap->With != -1 ) return 0;

	double	dd;
	if( ap->Max & 0x8000 ){
		dd = GetNowValue(ap->Max & 0x7fff);
		if( d > dd ){
			d = dd;
			return 1;
		}
	}
	else if( d >= ap->DMAX ){
		d = ap->DMAX - ap->DI;
		return 1;
	}
	if( ap->Min & 0x8000 ){
		dd = GetNowValue(ap->Min & 0x7fff);
		if( d < dd ){
			d = dd;
			return 1;
		}
	}
	else if( d <= ap->DMIN ){
		d = ap->DMIN + ap->DI;
		return 1;
	}
	return 0;
}

void CACal::SetTradeStr(LPSTR t)
{
	double	gain, fb, jx, swr, elv, mch, cur;
	gain = atrg.TG;
	fb = atrg.TFB;
	jx = atrg.TJX;
	swr = atrg.TSWR;
	elv = atrg.TELV;
	mch = atrg.TM;
	cur = atrg.TI;
	double dm = gain + fb + jx + swr + elv + mch + cur;
	if( dm && atrg.TF ){
		gain = (gain * 100.0)/dm;
		fb = (fb * 100.0)/dm;
		jx = (jx * 100.0)/dm;
		swr = (swr * 100.0)/dm;
		elv = (elv * 100.0)/dm;
		mch = (mch * 100.0)/dm;
		cur = (cur * 100.0)/dm;
		strcpy(t, "最適化 "); t += strlen(t);
		if( gain ){
			sprintf(t, " Gain:%s%%", StrDbl1(gain)); t += strlen(t);
		}
		if( fb ){
			sprintf(t, " F/B:%s%%", StrDbl1(fb)); t += strlen(t);
		}
		if( jx ){
			sprintf(t, " jX:%s%%", StrDbl1(jx)); t += strlen(t);
		}
		if( swr ){
			sprintf(t, " SWR:%s%%", StrDbl1(swr)); t += strlen(t);
		}
		if( elv ){
			sprintf(t, " Elv:%s%%", StrDbl1(elv)); t += strlen(t);
		}
		if( mch ){
			sprintf(t, " M:%s%%", StrDbl1(mch)); t += strlen(t);
		}
		if( cur ){
			sprintf(t, " C:%s%%", StrDbl1(cur));
		}
	}
	else {
		sprintf(t, "最適化 目標なし...(単純スイープ動作)");
	}

}

void CACal::DispStat(TMemo *tp)
{
	char	bf[128];

	SetTradeStr(bf);
	tp->Lines->Add(bf);
}

///----------------------------------------------------------------
///  CGraphクラス
CGraph::CGraph(void)
{
	gCol = clBtnFace;
	DotLen = 3;
	YLIMMAX = -1.0;
	Clear();
}

void CGraph::Init(TPaintBox *pp)
{
	cp = pp->Canvas;
	xl = pp->Left + 60;
	xr = pp->Width - 60;
	yt = 10;
	yb = pp->Height - 30;
	xw = xr - xl;
	yw = yb - yt;
	clip.SetClip(xl, yt, xr, yb);
}

void CGraph::Init(TCanvas *p, int x1, int y1, int x2, int y2)
{
	cp = p;
	int fw = cp->TextWidth("A");
	int fh = cp->TextHeight("A");
	xl = x1 + fw*6;
	xr = x2 - fw*6;
	yt = y1 + fh;
	yb = y2 - (fh*2);
	xw = xr - xl;
	yw = yb - yt;
	gCol = clBlack;
	DotLen = fw/3;
	clip.SetClip(xl, yt, xr, yb);
}

void CGraph::ClearView(TPaintBox *pp)
{
	TRect	rc;

	rc.Left = 0;
	rc.Top = 0;
	rc.Right = pp->Width;
	rc.Bottom = pp->Height;
	cp->Brush->Color = clWhite;
	cp->FillRect(rc);
}

void CGraph::DrawAngle(void)
{
	cp->Pen->Style = psSolid;
	cp->Pen->Color = clBlack;
	cp->MoveTo(xl, yt); cp->LineTo(xr, yt);
	cp->MoveTo(xr, yt); cp->LineTo(xr, yb);
	cp->MoveTo(xl, yb); cp->LineTo(xr, yb);
	cp->MoveTo(xl, yt); cp->LineTo(xl, yb);

	cp->Pen->Color = gCol;
	cp->Pen->Style = psDot;
	int i, X, Y;
	for( i = 1; i < 4; i++ ){
		X = int(xl + (i * double(xw)/4.0));
		cp->MoveTo(X, yt); cp->LineTo(X, yb);
	}
	for( i = 1; i < 4; i++ ){
		Y = int(yt + (i * double(yw)/4.0));
		cp->MoveTo(xl, Y); cp->LineTo(xr, Y);
	}
	cp->Pen->Style = psSolid;
}

void CGraph::Clear(void)
{
	pcnt = 0;
	memset(flag, 0, sizeof(flag));
	memset(pos, 0, sizeof(pos));
	memset(data, 0, sizeof(data));
}

void CGraph::AddPoint(double x, double y, int fg)
{
	if( pcnt < (BWMAX + (BWSMAX*BWSDIMMAX)) ){
		flag[pcnt] = fg;
		pos[pcnt] = x;
		data[pcnt] = y;
		pcnt++;
	}
}

void CGraph::AutoRenge(double lim)
{
	int i;
	ydl = xdl = MAXDOUBLE;
	ydh = xdh = -MAXDOUBLE;
	for( i = 0; i < pcnt; i++ ){
		if( xdl > pos[i] ) xdl = pos[i];
		if( xdh < pos[i] ) xdh = pos[i];
		if( ydl > data[i] ) ydl = data[i];
		if( ydh < data[i] ) ydh = data[i];
	}
	rydl = ydl;
	rydh = ydh;
	xdw = xdh - xdl;
	rydw = ydw = ydh - ydl;
	ydw = GetSCNormal(ydw*2);
	if( ydw < 0.1 ) ydw = 0.1;
	double d;
	if( ydl < 0.0 ){
		ydl = GetSCNormal(ydl);
		d = ydl + ydw;
		while(d < ydh){
			ydw *= 2;
			d = ydl + ydw;
		}
		ydh = d;
	}
	else {
		ydh = GetSCNormal(ydh);
		d = ydh - ydw;
		while(d > ydl){
			ydw *= 2;
			d = ydh - ydw;
		}
		ydl = d;
	}
	// 幅が小さい場合の補正
	if( (ydw < 0.4) ){
		ydl = ydl - 0.4;
		ydh = ydl + 0.8;
		ydw = 0.8;
	}
	else if( ydw >= 0.8 ){
		if( ydw > rydw*3 ){	// 幅が大きすぎる場合の補正
			d = (ydl + ydh)/2;
			while( (ydw >= 0.8) && (ydw > rydw*3) ){	// 幅が大きすぎる場合の補正
				ydw /= 2;
				ydl = d - ydw/2;
				ydh = d + ydw/2;
			}
		}
		while((ydh - rydh) > (ydw/4)){		// 偏りの修正(上側)
			ydh -= ydw/4;
			ydl -= ydw/4;
		}
		while((rydl - ydl) > (ydw/4)){		// 偏りの修正(下側)
			ydh += ydw/4;
			ydl += ydw/4;
		}
		if( ((rydl - ydl) < ydw*0.25) && ((ydh - rydh) > (ydw*0.4)) ){		// 偏りの修正(上側)
			ydh -= ydw/4;
			ydl -= ydw/4;
		}
	}
	if( ydl < lim ){
		lim = lim - ydl;
		ydl += lim;
		ydh += lim;
		// 幅が大きすぎる場合の補正
		while( (ydw >= 0.8) && ((ydh - rydh) > (ydw/2)) ){
			ydw /= 2;
			ydh = ydl + ydw;
		}
	}
	if( (YLIMMAX > 0) && (ydh > YLIMMAX) ){
    	ydh = YLIMMAX;
        ydw = ydh - ydl;
    }
}

void CGraph::AdjRengeZero(void)
{
	if( (ydl < 0.0) && (ydh > 0.0) ){		// 0を含んでいる場合
		ydl = -ydw/2;
		ydh = ydw/2;
		while( (ydl > rydl) || (ydh < rydh) ){
			ydw *= 2.0;
			ydl = -ydw/2;
			ydh = ydw/2;
		}
	}
}

void CGraph::DrawTitle(int sw, LPCSTR p, TColor col)
{
	cp->Font->Color = col;
	int FW = cp->TextWidth(p);
	int FH = cp->TextHeight(p);
	int y = yt + (yw*3/8) + FH/2;
	if( sw ){		// 右側
		cp->TextOut(xr + FH/2, y, p);
	}
	else {			// 左側
		cp->TextOut(xl - FW - (FH/2), y, p);
	}
}

void CGraph::DrawGage(int sw, TColor col)
{
	LPCSTR	sp;
	int i, FW, FH, Y;
	double	dh = ydh;
	double	di = ydw/4;
	cp->Font->Color = col;
	cp->Pen->Color = clBlack;
	for( i = 0; i < 5; i++, dh -= di ){
		Y = int(yt + (i * double(yw)/4.0));
		if( int(ABS(dh) * 100.0 + 0.5) % 10 ){
			sp = StrDbl2(dh);
        }
        else {
			sp = StrDbl1(dh);
        }
		FW = cp->TextWidth(sp);
		FH = cp->TextHeight(sp);
		if( sw ){		// 右側
			cp->TextOut(xr+FW/4, Y-FH/2, sp);
		}
		else {			// 左側
			cp->TextOut(xl-FW-FW/4, Y-FH/2, sp);
		}
	}
}

void CGraph::DrawXGage(void)
{
	LPCSTR	sp;
	int i, FW, FH, X;
	cp->Font->Color = clBlack;
	cp->Pen->Color = clBlack;
	double d = xdl;
	for( i = 0; i < BWMAX; i++, d += (xdw/4) ){
		X = int(xl + (i * double(xw)/4.0));
		sp = StrDbl3(RoundUp(d,1000));
		FW = cp->TextWidth(sp);
		FH = cp->TextHeight(sp);
		cp->TextOut(X-FW/2, yb+FH/2, sp);
	}
}

void CGraph::Plot(TColor col, TPenStyle style /* = psSolid*/)
{
	if( !pcnt ) return;
	cp->Pen->Color = col;
	int i;
	int	X, Y, AX, AY;
	for( AX = AY = i = 0; i < pcnt; i++ ){
		X = xl + int(((pos[i] - xdl) * xw)/xdw);
		Y = yb - int(((data[i] - ydl) * yw)/ydw);
		if( i ){
			cp->Pen->Style = style;
			clip.Line(cp, AX, AY, X, Y);
		}
		AX = X; AY = Y;
		if( flag[i] ){
			cp->Pen->Style = psSolid;
			clip.Line(cp, X-DotLen, Y-DotLen, X+DotLen, Y+DotLen);
			clip.Line(cp, X-DotLen, Y+DotLen, X+DotLen, Y-DotLen);
		}
	}
	cp->Pen->Style = psSolid;
}

void CGraph::DrawConst(double l, double c, double q, double w, double f)
{
	char	bf[64];
	int FW = cp->TextWidth("L");
	int FH = cp->TextHeight("L");
	int x = xl + FW/2;
	int y = yt + FH/2;
	cp->Font->Color = clBlack;
	sprintf(bf, "L: %suH", StrDbl1(l));
	cp->TextOut(x, y, bf);
	y += FH;
	sprintf(bf, "C: %spF", StrDbl1(c));
	cp->TextOut(x, y, bf);
	y += FH;
	sprintf(bf, "Q: %s", StrDbl1(q));
	cp->TextOut(x, y, bf);
	y += FH;
	sprintf(bf, "B: %sKHz", StrDbl1(w));
	cp->TextOut(x, y, bf);
	y += FH;
	sprintf(bf, "fo: %sMHz", StrDbl3(f));
	cp->TextOut(x, y, bf);
}

void CGraph::DrawInfo(LPCSTR p)
{
	int FW = cp->TextWidth(p);
	int FH = cp->TextHeight(p);
	int x = xr - FW - cp->TextWidth("L")/2;
	int y = yt + FH/2;
	cp->Font->Color = clBlack;
	cp->TextOut(x, y, p);
}

void CGraph::DrawInfoL(int n, LPCSTR fmt, ...)
{
	va_list	pp;
	char	bf[1024];

	va_start(pp, fmt);
	vsprintf( bf, fmt, pp );
	va_end(pp);

	int FW = cp->TextWidth("L");
	int FH = cp->TextHeight(bf);
	int x = xl + FW/2;
	int y = yt + FH/2 + (n * FH);
	cp->Font->Color = clBlack;
	cp->TextOut(x, y, bf);
}

///----------------------------------------------------------------
///  CClipクラス
int CClip::Clip(int &x1, int &y1, int &x2, int &y2)
{
	if( (x1 < Xl) && (x2 < Xl) ) return FALSE;
	if( (x1 > Xr) && (x2 > Xr) ) return FALSE;
	if( (y1 < Yt) && (y2 < Yt) ) return FALSE;
	if( (y1 > Yb) && (y2 > Yb) ) return FALSE;
	double	bx = double(x1);
	double	by = double(y1);
	double	ex = double(x2);
	double	ey = double(y2);
	if( bx < Xl ){
		by = ClipSubV(bx, by, ex, ey, double(Xl));
		bx = double(Xl);
	}
	if( ex < Xl ){
		ey = ClipSubV(bx, by, ex, ey, double(Xl));
		ex = double(Xl);
	}
	if( bx > Xr ){
		by = ClipSubV(bx, by, ex, ey, double(Xr));
		bx = double(Xr);
	}
	if( ex > Xr ){
		ey = ClipSubV(bx, by, ex, ey, double(Xr));
		ex = double(Xr);
	}
	if( by < Yt ){
		bx = ClipSubH(bx, by, ex, ey, double(Yt));
		by = double(Yt);
	}
	if( ey < Yt ){
		ex = ClipSubH(bx, by, ex, ey, double(Yt));
		ey = double(Yt);
	}
	if( by > Yb ){
		bx = ClipSubH(bx, by, ex, ey, double(Yb));
		by = double(Yb);
	}
	if( ey > Yb ){
		ex = ClipSubH(bx, by, ex, ey, double(Yb));
		ey = double(Yb);
	}
	x1 = int(bx);
	y1 = int(by);
	x2 = int(ex);
	y2 = int(ey);
	return TRUE;
}

///-------------------------------------------------------
/// CRecentMenuクラス

CRecentMenu::CRecentMenu()
{
	InsPos = 0;
	Max = RECMENUMAX;
	int i;
	for( i = 0; i < RECMENUMAX; i++ ){
		Caption[i] = "";
		Items[i] = NULL;
	}
	Items[i] = NULL;
}
void CRecentMenu::Init(int pos, TMenuItem *pmenu, int max)
{
	InsPos = pos;
	pMenu = pmenu;
	Max = max;
}
void CRecentMenu::Init(TMenuItem *pi, TMenuItem *pmenu, int max)
{
	Init(pmenu->IndexOf(pi), pmenu, max);
}
int CRecentMenu::GetCount(void)
{
	int i, n;
	for( i = n = 0; i < RECMENUMAX; i++, n++ ){
		if( Caption[i].IsEmpty() ) break;
	}
	return n;
}
LPCSTR CRecentMenu::GetItemText(int n)
{
	if( (n >= 0) && (n < RECMENUMAX) ){
		return Caption[n].c_str();
	}
	else {
		return NULL;
	}
}
void CRecentMenu::SetItemText(int n, LPCSTR p)
{
	if( (n >= 0) && (n < RECMENUMAX) ){
		Caption[n] = p;
	}
}
int CRecentMenu::IsAdd(LPCSTR pNew)
{
	return strcmp(Caption[0].c_str(), pNew);
}
void CRecentMenu::Add(LPCSTR pNew)
{
	if( !IsAdd(pNew) ) return;
	int i;
	for( i = 1; i < RECMENUMAX; i++ ){
		if( !strcmp(pNew, Caption[i].c_str()) ){
			for( ; i < (RECMENUMAX - 1); i++ ){
				Caption[i] = Caption[i+1];
			}
			Caption[i] = "";
			break;
		}
	}
	for( i = RECMENUMAX - 1; i > 0; i-- ){
		Caption[i] = Caption[i-1];
	}
	Caption[0] = pNew;
}
void CRecentMenu::Delete(LPCSTR pName)
{
	int i;
	for( i = 0; i < RECMENUMAX; i++ ){
		if( !strcmp(pName, Caption[i].c_str()) ){
			for( ; i < (RECMENUMAX - 1); i++ ){
				Caption[i] = Caption[i+1];
			}
			Caption[i] = "";
			break;
		}
	}
}
void CRecentMenu::ExtFilter(LPCSTR pExt)
{
	int i, j;

	for( i = 0; i < RECMENUMAX; i++ ){
		if( (!Caption[i].IsEmpty()) && strcmpi(GetEXT(Caption[i].c_str()), pExt) ){
			for( j = i; j < (RECMENUMAX - 1); j++ ){
				Caption[j] = Caption[j+1];
			}
			Caption[j] = "";
			i--;
		}
	}
}
void CRecentMenu::Clear(void)
{
	Delete();
	for( int i = 0; i < RECMENUMAX; i++ ){
		Caption[i] = "";
	}
}
void CRecentMenu::Insert(TObject *op, Classes::TNotifyEvent pEvent)
{
	int n = GetCount();
	int i;
	int pos = InsPos;
	if( n > Max ) n = Max;
	char bf[256];
	for( i = 0; i < n; i++, pos++ ){
		Items[i] = new TMenuItem((TComponent *)op);
		sprintf(bf, "&%d %s", (i + 1) % 10, Caption[i].c_str());
		Items[i]->Caption = bf;
		Items[i]->OnClick = pEvent;
		pMenu->Insert(pos, Items[i]);
	}
	if( n ){
		Items[i] = new TMenuItem((TComponent *)op);
		Items[i]->Caption = "-";
		Items[i]->OnClick = NULL;
		pMenu->Insert(pos, Items[n]);
	}
}
void CRecentMenu::Delete(void)
{
	for( int i = 0; i < RECMENUMAX+1; i++ ){
		if( Items[i] != NULL ){
			pMenu->Delete(InsPos);
			Items[i] = NULL;
		}
	}
}
int CRecentMenu::FindItem(TMenuItem *mp)
{
	for( int i = 0; i < RECMENUMAX; i++ ){
		if( Items[i] == mp ){
			return i;
		}
	}
	return -1;
}
LPCSTR CRecentMenu::FindItemText(TMenuItem *mp)
{
	return GetItemText(FindItem(mp));
}

///------------------------------------------------------
///
///CWebRef クラス
///
void MakeCommand(LPSTR t, LPCSTR s, LPCSTR p)
{
	for( ;*s; s++ ){
		if( *s == '%' ){
			s++;
			if( *s == '%' ){
				*t++ = '%';
			}
			else if( (p != NULL) && (*s == '1') ){
				strcpy(t, p);
				t += strlen(t);
				p = NULL;
			}
		}
		else {
			*t++ = *s;
		}
	}
	*t = 0;
	if( p != NULL ){
		*t++ = ' ';
		strcpy(t, p);
	}
}

CWebRef::CWebRef()
{
	HTML = "";

	HKEY hkey=NULL;	/* オープン キーのハンドル	*/

	char    bf[512], name[512];
	ULONG   namelen, len;
	if( !RegOpenKeyEx(HKEY_CLASSES_ROOT, "http", 0, KEY_READ, &hkey) ){
		if( !RegOpenKeyEx(hkey, "shell", 0, KEY_READ, &hkey) ){
			if( !RegOpenKeyEx(hkey, "open", 0, KEY_READ, &hkey) ){
				if( !RegOpenKeyEx(hkey, "command", 0, KEY_READ, &hkey) ){
					namelen = sizeof(name);
					len = sizeof(bf);
					if( !::RegEnumValue(hkey, 0, name, &namelen, NULL,
					NULL, (LPBYTE)bf, &len) ){
						HTML = bf;
					}
				}
			}
		}
	}
	::RegCloseKey(hkey);
}

void CWebRef::ShowHTML(LPCSTR url)
{
	char    cmd[1024];
	MakeCommand(cmd, HTML.c_str(), url);
	::WinExec(cmd, SW_SHOW);
}


///------------------------------------------------------
///
///CWaitCursor クラス
///
CWaitCursor::CWaitCursor()
{
	sv = Screen->Cursor;
    Screen->Cursor = crHourGlass;
}

CWaitCursor::~CWaitCursor()
{
	Screen->Cursor = sv;
}

void CWaitCursor::Delete(void)
{
	Screen->Cursor = sv;
}

void CWaitCursor::Wait(void)
{
    Screen->Cursor = crHourGlass;
}

