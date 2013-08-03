//Copyright+LGPL

//-----------------------------------------------------------------------------------------------------------------------------------------------
// Copyright 1999-9013 Makoto Mori
//-----------------------------------------------------------------------------------------------------------------------------------------------
// This file is part of MMANA.

// MMANA is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// MMANA is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License along with MMANA.  If not, see
// <http://www.gnu.org/licenses/>.
//-----------------------------------------------------------------------------------------------------------------------------------------------

/*********************************************************************
	MININEC3 Convert by JE3HHT (Makoto.Mori) 1999.
*********************************************************************/
#include <vcl.h>
#pragma hdrstop

#include <except.h>
#include "MiniNec3.h"
#include "Main.h"

#define	INT(c)	(((c)<0)?((int)((c)-1)):((int)(c)))

#define	IVSQ2	0.707106781186547524400844362104849
static	UINT	pmax;   // 計算時のパルスの数
static	UINT	IX1;
static	UINT	IX2;
#define	GETREF(a,b)			((pmax*(a))+(b))
#define	REFDIM(base,index)	base[index]
#define REFACI(a,b) aCi[(a)*3+(b)]

static	double	HeightOff;
static	ANTDEF	*pAp;

static	char	Ac;
static	char	Ic;
static	char	Pc;
static	char	Sc;
static	char	Nc;
static	char	Lc;


static	int		I,I1,I2,I3,I4,I5,I6;
static	int		IX,IY,IZ;
static	int		J,J1,J2,J3,J8,J9;
static	int		K;
static	int		L;
static	int		L1,L2,L3,L4;
static	int		N,N1,N2;
static	int		NA,NL,NM,NS,NR,NW,NX,NY,NZ;
static	int		Ei;
static	int		F5i;
static	int		MA,MP,MW,MM,ML;
static	int		G;
static	int		Gi;
static	int		TB;
static	int		P1i,P2i,P3i,P4i;

static	double	A1,A3,A4;
static	double	AA,AC;
static	double	B,B1,B2,B9;
static	double	C,C0,C1,C2,C3,C4,C5,C6,C7,C8,C9;
static	double	CRd,CI,CO,CT;
static	double	D,D0,D1,D2;
static	double	E;
static	double	F;
static	double	FLG;
static	double	FVS;
static	double	F1,F2,F3,F4,F5,F6,F7,F8;
static	double	G0;
static	double	H1,H2,H8,H9;
static	double	LI,LR;
static	double	M;
static	double	O2;
static	double	P,P0,P1,P2,P3;
static	double	PK;
static	double	Q1,Q2;
static	double	PCT,PCTN,PWR;
static	double	R,R1,R2,R3;
static	double	RD,RR;
static	double	S,S0,S1,S2,S3,S4,S8,S9;
static	double	X,X0,X1,X2,X3,X4,X5;
static	double	XC,XI,XX;
static	double	T,T1,T2,T3,T4,T5,T6,T7,T8,T9;
static	double	U1,U2,U3,U4,U5,U6,U7,U8;
static	double	V0,V1,V2,V3,V8,V9;
static	double	VM,VP;
static	double	W,W0,W1,W2,W6,W7;
static	double	Y0,Y1,Y2,Y3,Y5;
static	double	YI,YC,YY;
static	double	Z0,Z1,Z2,Z3,Z4,Z5,Z6,Z7,Z8;
static	double	ZA,ZC,ZI,ZZ;
static	double	D3;
static	double	SRM;
static	double	A2;

static	double	I1f,I2f,I3f,I6f;
static	double	J1f,J2f,J3f;
static	double	K9f;

/*Array*/
static	double	aKf[7][3];
static	double	aQ[15];

//static	double	aX[(WZMAX*2)+PMAX+1], aY[(WZMAX*2)+PMAX+1], aZ[(WZMAX*2)+PMAX+1];
static	double	*aX, *aY, *aZ;
/*	DIM A(50),CA(50),CB(50),CG(50),J1(50),J2(50,2),N(50,2),S(50)*/
static	double	aA[WZMAX+1];
static	double	aCA[WZMAX+1];
static	double	aCB[WZMAX+1];
static	double	aCG[WZMAX+1];
static	int	aJ1[WZMAX+1];
static	int	aJ2[WZMAX+1][3];
static	int	aN[WZMAX+1][3];
static	double	aS[WZMAX+1];
/*	DIM LA(2,11,8),LP(11),LS(11)*/
static	double	aLA[3][LMAX+WMAX+1][SMAX+1];
static	int		aLP[LMAX+WMAX+1];
static	int		aLS[LMAX+WMAX+1];
/*	DIM H(6),T(6),U(6),V(6),Z1(6),Z2(6)*/
static	double	aH[EMAX+1];
static	double	aT[EMAX+1];
static	double	aU[EMAX+1];
static	double	aV[EMAX+1];
static	double	aZ1[EMAX+1];
static	double	aZ2[EMAX+1];

// パルス数によるメモリ動的確保
int		*aCi;
double	*aCI;
double	*aCR;
int		*aPi;
int		*aWi;
float	*aZR;
float	*aZI;
int		*aEi;
double	*aEf;
double	*aL;
double	*aM;

#if 0
/*	DIM C%(50,2),CI(50),CR(50),P(50),W%(50)*/
static	int		aCi[PMAX+1][3];
static	double	aCI[PMAX+1];
static	double	aCR[PMAX+1];
static	int		aPi[PMAX+1];
static	int		aWi[PMAX+1];
/*	DIM ZR(50,50),ZI(50,50)*/
static	float	aZR[PMAX+1][PMAX+1];
static	float	aZI[PMAX+1][PMAX+1];
/*	DIM E(100),L(100),M(100)*/
static	int		aEi[WZMAX+PMAX+1];
static	double	aEf[WZMAX+PMAX+1];
static	double	aL[WZMAX+PMAX+1];
static	double	aM[WZMAX+PMAX+1];
#endif

void S28(void);
void S56(void);
void S66(void);
void S75(void);
void S87(void);
void S102(void);
void L113(void);
void L135(void);
void S184(void);
void S196(void);
void S497(void);
void S572(void);
void S607(void);
void S621(void);
void S875(void);
//void S1133(void);
//void S1152(void);
//void S1153(void);
void S1299(void);
//void S1364(void);
//void S1369(void);
//void S1430(void);
//void S1455(void);
void S1550(void);
void S1580(void);
void S1589(void);

#if 0
static __fastcall float GetAdd(float a, float b)
{
	return a * a + b * b;
}
#endif

#if 0
void DebugOut(void)
{
	FILE	*fp;

	fp = fopen("E:TEST.TXT", "wt");
	for( I = 1; I <= N; I++ ){
		for( J = 1; J <= N; J++ ){
			fprintf( fp, "%14.7lf  %14.7lf\n", aZR[I][J], aZI[I][J] );
		}
	}
	fclose(fp);
}
#endif

static void Print(char *ct, ...)
{
	if( !exeenv.CalcLog ) return;
	va_list	pp;
	char	bf[256];

	va_start(pp, ct);
	vsprintf( bf, ct, pp );
	va_end(pp);
	MainWnd->CalMemo->Lines->Add(bf);
}

char inputYN(char *p, ...)
{
  return ' '; //ja7ude 1.0
}

int input(char *p, char *ct, ...)
{
#if 0
	va_list  argptr;
	char	bf[256];
	int cnt;

	printf(p);
	gets(bf);
/*	fflush(stdin);*/

	va_start(argptr, ct);
//	cnt = vsscanf(bf, ct, argptr);
	va_end(argptr);
	return(cnt);
#else
	return 0;
#endif
}

double POW(double d, int n)
{
	double	dd;

	if( n == 0 ){
		dd = 1.0;
	}
	else if( n > 0 ){
		for( dd = d, n--; n; n-- ){
			dd *= d;
		}
	}
	else if( n < 0 ){
		n = -n;
		for( dd = d, n--; n; n-- ){
			dd *= d;
		}
		dd = 1/dd;
	}
	return dd;
}

/*------------------------------------------------------------------
	********** KERNEL EVALUATION OF INTEGRALS I2 & I3 **********
-------------------------------------------------------------------*/
void S28(void)
{
/*
S28  :S171,S173
*/
	if( K < 0 ){
		X3=V1+T*(X2-V1);
		Y3=V2+T*(Y2-V2);
		Z3=V3+T*(Z2-V3);
	}
	else {
		X3=X2+T*(V1-X2);
		Y3=Y2+T*(V2-Y2);
		Z3=Z2+T*(V3-Z2);
	}
	D3=X3*X3+Y3*Y3+Z3*Z3;
	/*	REM ----- MOD FOR SMALL RADIUS TO WAVELENGTH RATIO*/
	if( aA[P4i]<=SRM ){
		D=sqrt(D3);
	}
	else {
		D=D3+A2;
		if( D > 0.0 ) D=sqrt(D);
		/*	REM ----- CRITERIA FOR USING REDUCED KERNEL*/
		if( I6f != 0 ){
			/*	REM ----- EXACT KERNEL CALCULATION WITH ELLIPTIC INTEGRAL*/
			B=D3/(D3+4*A2);
			W0=C0+B*(C1+B*(C2+B*(C3+B*C4)));
			W1=C5+B*(C6+B*(C7+B*(C8+B*C9)));
			V0=(W0-W1*log(B))*sqrt(1-B);
			T3=T3+(V0+log(D3/(64*A2))/2)/P/aA[P4i]-1/D;
		}
	}
	B1=D*W;
	/*	REM ----- EXP(-J*K*R)/R*/
	T3=T3+cos(B1)/D;
	T4=T4-sin(B1)/D;
}

void S56(void)
{
/*
	REM ***** PSI(P1,P2,P3) = T1 + J * T2 **********
	REM ----- ENTRIES REQUIRED FOR NEAR FIELD CALCULATION
*/
	X1=X0+P1*T5/2;
	Y1=Y0+P1*T6/2;
	Z1=Z0+P1*T7/2;
	P2i = (int)P2;
	X2=X1-aX[P2i];
	Y2=Y1-aY[P2i];
	Z2=Z1-K*aZ[P2i];
	P3i = (int)P3;
	V1=X1-aX[P3i];
	V2=Y1-aY[P3i];
	V3=Z1-K*aZ[P3i];
	L135();
}
void S66(void)
{
/*
S66  :S998
*/
	I4=INT(P2);
	I5=I4+1;
	X2=X0-(aX[I4]+aX[I5])/2;
	Y2=Y0-(aY[I4]+aY[I5])/2;
	Z2=Z0-K*(aZ[I4]+aZ[I5])/2;
	P3i = (int)P3;
	V1=X0-aX[P3i];
	V2=Y0-aY[P3i];
	V3=Z0-K*aZ[P3i];
	L135();
}

void S75(void)
{
/*
S75  :S991
*/
	P2i = (int)P2;
	X2=X0-aX[P2i];
	Y2=Y0-aY[P2i];
	Z2=Z0-K*aZ[P2i];
	I4=INT(P3);
	I5=I4+1;
	V1=X0-(aX[I4]+aX[I5])/2;
	V2=Y0-(aY[I4]+aY[I5])/2;
	V3=Z0-K*(aZ[I4]+aZ[I5])/2;
	L135();
}

void S87(void)
{
/*
	REM ----- ENTRIES REQUIRED FOR IMPEDANCE MATRIX CALCULATION
	REM ----- S(M) GOES IN (X1,Y1,Z1) FOR SCALAR POTENTIAL
	REM ----- MOD FOR SMALL RADIUS TO WAVE LENGTH RATIO
S87  :S283,S292,S300,S309
*/
	FVS=1;
	if( K >= 1 ){
		if( aA[P4i]<=SRM ){
			if( ((P3==(P2+1)) && (P1==((P2+P3)/2))) ){
				T1=2*log(aS[P4i]/aA[P4i]);
				T2=-W*aS[P4i];
				return;
			}
		}
	}
	I4=INT(P1);
	I5=I4+1;
	X1=(aX[I4]+aX[I5])/2;
	Y1=(aY[I4]+aY[I5])/2;
	Z1=(aZ[I4]+aZ[I5])/2;
	L113();
}

void S102(void)
{
/*
	REM ----- S(M) GOES IN (X1,Y1,Z1) FOR VECTOR POTENTIAL
	REM ----- MOD FOR SMALL RADIUS TO WAVE LENGTH RATIO
S102 :S252,S259
*/
	FVS=0;
	if( K >= 1 ){
		if( aA[P4i] < SRM ){
			if( (I==J) && (P3==(P2+.5)) ){
				T1=log(aS[P4i]/aA[P4i]);
				T2=-W*aS[P4i]/2;
				return;
			}
		}
	}
	P1i = (int)P1;
	X1=aX[P1i];
	Y1=aY[P1i];
	Z1=aZ[P1i];
	L113();
}

void L113(void)
{
/*
	REM ----- S(U)-S(M) GOES IN (X2,Y2,Z2)
L113 :L99
*/
	I4=INT(P2);
	if( ((double)I4)==P2 ){
		X2=aX[I4]-X1;		/* P2 -> I4 by Mako	*/
		Y2=aY[I4]-Y1;		/* P2 -> I4 by Mako	*/
		Z2=K*aZ[I4]-Z1;		/* P2 -> I4 by Mako	*/
	}
	else {
		I5=I4+1;
		X2=(aX[I4]+aX[I5])/2-X1;
		Y2=(aY[I4]+aY[I5])/2-Y1;
		Z2=K*(aZ[I4]+aZ[I5])/2-Z1;
	}
	/*	REM ----- S(V)-S(M) GOES IN (V1,V2,V3)*/
	I4=INT(P3);
	if( ((double)I4)==P3 ){
		V1=aX[I4]-X1;		/* P3 -> I4 by Mako	*/
		V2=aY[I4]-Y1;		/* P3 -> I4 by Mako	*/
		V3=K*aZ[I4]-Z1;		/* P3 -> I4 by Mako	*/
	}
	else {
		I5=I4+1;
		V1=(aX[I4]+aX[I5])/2-X1;
		V2=(aY[I4]+aY[I5])/2-Y1;
		V3=K*(aZ[I4]+aZ[I5])/2-Z1;
	}
	L135();
}

void L135(void)
{
	int	i,j;

	D0=X2*X2+Y2*Y2+Z2*Z2;
	/*	REM ----- MAGNITUDE OF S(V) - S(M)*/
	if( D0 > 0 ) D0=sqrt(D0);
	D3=V1*V1+V2*V2+V3*V3;
	if( D3 > 0 ) D3=sqrt(D3);
	/* REM ----- SQUARE OF WIRE RADIUS*/
	A2=aA[P4i]*aA[P4i];
	/* REM ----- MAGNITUDE OF S(V) - S(U)*/
	S4=(P3-P2)*aS[P4i];
	/* REM ----- ORDER OF INTEGRATION*/
	/* REM ----- LTH ORDER GAUSSIAN QUADRATURE*/
	T1=0;
	T2=0;
	I6f=0;
	F2=1;
	L=7;
	T=(D0+D3)/aS[P4i];
	/* REM ----- CRITERIA FOR EXACT KERNEL*/
	if( T > 1.1 ) goto L165;
	if( FVS == -1 ) goto L165;
	i = aWi[I];
	j = aWi[J];
	if( (aJ2[i][1]==aJ2[j][1]) ||
		(aJ2[i][1]==aJ2[j][2]) ||
		(aJ2[i][2]==aJ2[j][1]) ||
		(aJ2[i][2]==aJ2[j][2]) ){
		if( aA[P4i] > SRM ){
			F2=2*(P3-P2);
			I6f=(1-log(S4/F2/8/aA[P4i]))/P/aA[P4i];
			goto L167;
		}
		if( FVS == 1 ){
			T1=2*log(aS[P4i]/aA[P4i]);
			T2=-W*aS[P4i];
		}
		else {
			T1=log(aS[P4i]/aA[P4i]);
			T2=-W*aS[P4i]/2;
		}
		return;
	}
L165:;
	if( T > 6 ) L=3;
	if( T > 10 ) L=1;
L167:;
	I5=L+L;
L168:;
	T3=0;
	T4=0;
	T=(aQ[L]+.5)/F2;
	S28();
	T=(.5-aQ[L])/F2;
	S28();
	L=L+1;
	T1=T1+aQ[L]*T3;
	T2=T2+aQ[L]*T4;
	L=L+1;
	if( L < I5) goto L168;
	T1=S4*(T1+I6f);
	T2=S4*T2;
}

void S184(void)
{
/*
	REM ********** COMPLEX SQUARE ROOT **********
	REM ----- W6+I*W7=sqrt(Z6+I*Z7)
S184 :S629,S778
	T6=sqrt((ABS(Z6)+sqrt(Z6*Z6+Z7*Z7))/2)
	T7=ABS(Z7)/2/T6
	IF Z6<0 THEN L191
	W6=T6
	W7=T7
	IF Z7<0 THEN W7=-T7
	RETURN
L191 :L186
	W6=T7
	W7=T6
	IF Z7<0 THEN W7=-T6
	RETURN
*/
	T6=sqrt((ABS(Z6)+sqrt(Z6*Z6+Z7*Z7))/2);
	T7=ABS(Z7)/2/T6;
	if( Z6 < 0 ){
		W6=T7;
		W7=T6;
		if( Z7 < 0 ) W7=-T6;
	}
	else {
		W6=T6;
		W7=T7;
		if( Z7 < 0 ) W7=-T7;
	}
}

void S196(void)
{
/*
	REM ********** IMPEDANCE MATRIX CALCULATION **********
*/

	if( FLG == 1 ) goto L428;
	if( FLG == 2 ) goto L477;
	/*	REM ----- BEGIN MATRIX FILL TIME CALCULATION*/
//	Print("BEGIN MATRIX FILL");
	Print("FILL MATRIX...");
	/*	REM ----- ZERO IMPEDANCE MATRIX*/
#if 1
	for( I = 1; I <= N; I++ ){
		IX1 = GETREF(I,1);
		memset(&REFDIM(aZR,IX1), 0, sizeof(double) * N);
		memset(&REFDIM(aZI,IX1), 0, sizeof(double) * N);
	}
#else
	memset(aZR, 0, sizeof(double)*(N+1)*(N+1));
	memset(aZI, 0, sizeof(double)*(N+1)*(N+1));
#endif

	/*	REM ----- COMPUTE ROW I OF MATRIX (OBSERVATION LOOP)*/
	for( I = 1; I <= N; I++ ){
		I1=ABS(REFACI(I,1));
		I2=ABS(REFACI(I,2));
		F4=SGN(REFACI(I,1))*aS[I1];
		F5=SGN(REFACI(I,2))*aS[I2];
		/* 	REM ----- R(M + 1/2) - R(M - 1/2) HAS COMPONENTS (T5,T6,T7)*/
		T5=F4*aCA[I1]+F5*aCA[I2];
		T6=F4*aCB[I1]+F5*aCB[I2];
		T7=F4*aCG[I1]+F5*aCG[I2];
		if( (REFACI(I,1)) == -(REFACI(I,2)) ){
			T7=aS[I1]*(aCG[I1]+aCG[I2]);
		}
		/*REM ----- COMPUTE COLUMN J OF ROW I (SOURCE LOOP)*/
		for( J = 1; J <= N; J++ ){
			J1=ABS(REFACI(J,1));
			J2=ABS(REFACI(J,2));
			F4=SGN(REFACI(J,1));
			F5=SGN(REFACI(J,2));
			F6=1;
			F7=1;
			/*REM ----- IMAGE LOOP*/
			for( K = 1; K >= G; K -= 2 ){
				if( (REFACI(J,1)) != -(REFACI(J,2)) ) goto L235;
				if( K < 0 ) goto L332;
				F6=F4;
				F7=F5;
			L235:;
				F8=0;
				if( K < 0 ) goto L248;
				/* REM ----- SET FLAG TO AVOID REDUNANT CALCULATIONS*/
				if( I1 != I2 ) goto L246;
				if( (aCA[I1]+aCB[I1])==0 ) goto L241;
				if( (REFACI(I,1)) != (REFACI(I,2)) ) goto L246;
			L241:;
				if( J1 != J2 ) goto L246;
				if( (aCA[J1]+aCB[J1])==0 ) goto L244;
				if( REFACI(J,1) != REFACI(J,2) ) goto L246;
			L244:;
				if( I1==J1 ) F8 = 1;
				if( I==J ) F8=2;
			L246:;/* :L238,L240,L241,L243*/
				if( REFDIM(aZR,GETREF(I,J)) != 0 ) goto L317;
				/*				REM ----- COMPUTE PSI(M,N,N+1/2)*/
			L248:;	/* :L236*/
				P1=2*aWi[I]+I-1;
				P2=2*aWi[J]+J-1;
				P3=P2+.5;
				P4i=J2;
				S102();
				U1=F5*T1;
				U2=F5*T2;
				/*				REM ----- COMPUTE PSI(M,N-1/2,N)*/
				P3=P2;
				P2=P2-.5;
				P4i=J1;
				if( F8<2 ) S102();
				V1=F4*T1;
				V2=F4*T2;
				/*				REM ----- S(N+1/2)*PSI(M,N,N+1/2) + S(N-1/2)*PSI(M,N-1/2,N)*/
				X3=U1*aCA[J2]+V1*aCA[J1];
				Y3=U1*aCB[J2]+V1*aCB[J1];
				Z3=(F7*U1*aCG[J2]+F6*V1*aCG[J1])*K;
				/*				REM ----- REAL PART OF VECTOR POTENTIAL CONTRIBUTION*/
				D1=W2*(X3*T5+Y3*T6+Z3*T7);
				X3=U2*aCA[J2]+V2*aCA[J1];
				Y3=U2*aCB[J2]+V2*aCB[J1];
				Z3=(F7*U2*aCG[J2]+F6*V2*aCG[J1])*K;
				/*				REM ----- IMAGINARY PART OF VECTOR POTENTIAL CONTRIBUTION*/
				D2=W2*(X3*T5+Y3*T6+Z3*T7);
				/*				REM ----- COMPUTE PSI(M+1/2,N,N+1)*/
				P1=P1+.5;
				if( F8==2 ) P1=P1-1;
				P2=P3;
				P3=P3+1;
				P4i=J2;
				if( F8 != 1 ) goto L283;
				U5=F5*U1+T1;
				U6=F5*U2+T2;
				goto L291;
			L283:;/* :L279*/
				S87();
				if( F8 < 2 ) goto L288;
				U1=(2*T1-4*U1*F5)/aS[J1];
				U2=(2*T2-4*U2*F5)/aS[J1];
				goto L314;
			L288:; /* :L284*/
				U5=T1;
				U6=T2;
				/*				REM ----- COMPUTE PSI(M-1/2,N,N+1)*/
			L291:;	/* :L282*/
				P1=P1-1;
				S87();
				U1=(T1-U5)/aS[J2];
				U2=(T2-U6)/aS[J2];
				/* REM ----- COMPUTE PSI(M+1/2,N-1,N)	*/
				P1=P1+1;
				P3=P2;
				P2=P2-1;
				P4i=J1;
				S87();
				U3=T1;
				U4=T2;
				/* REM ----- COMPUTE PSI(M-1/2,N-1,N)	*/
				if( F8 < 1 ) goto L308;
				T1=U5;
				T2=U6;
				goto L311;
			L308:;	/* :L304	*/
				P1=P1-1;
				S87();
				/*REM ----- GRADIENT OF SCALAR POTENTIAL CONTRIBUTION	*/
			L311:;	/* :L307	*/
				U1=U1+(U3-T1)/aS[J1];
				U2=U2+(U4-T2)/aS[J1];
				/* REM ----- SUM INTO IMPEDANCE MATRIX	*/
			L314:; /*:L287*/
				IX1 = GETREF(I,J);
				REFDIM(aZR,IX1) += K*(D1+U1);
				REFDIM(aZI,IX1) += K*(D2+U2);
				/* REM ----- AVOID REDUNANT CALCULATIONS	*/
			L317:;	/* :L246*/
				if( J < I ) goto L332;
				if( F8==0 ) goto L332;
				IX1 = GETREF(J,I);
				IX2 = GETREF(I,J);
				REFDIM(aZR,IX1)=REFDIM(aZR,IX2);
				REFDIM(aZI,IX1)=REFDIM(aZI,IX2);
				/* REM ----- SEGMENTS ON SAME WIRE SAME DISTANCE APART HAVE SAME Z*/
				P1=J+1;
				if( P1 > N ) goto L332;
				P1i = (int)P1;
				if( REFACI(P1i,1) != REFACI(P1i,2) ) goto L332;
				if( REFACI(P1i,2) == REFACI(J,2) ) goto L328;
				if( REFACI(P1i,2) != -REFACI(J,2) ) goto L332;
				if( (aCA[J2]+aCB[J2]) != 0 ) goto L332;
			L328:;	/* :L325	*/
				P2=I+1;
				if( P2 > N ) goto L332;
				P1i = (int)P1;
				P2i = (int)P2;
				IX1 = GETREF(P1i,P2i);
				IX2 = GETREF(I,J);
				REFDIM(aZR,IX1)=REFDIM(aZR,IX2);
				REFDIM(aZI,IX1)=REFDIM(aZI,IX2);
			L332:;	/* :L232,L317,L318,L323,L324,L326,L327,L329	*/
			}
		}
		PCT=I/N;
	}

//    DebugOut();
	/*REM ----- END MATRIX FILL TIME CALCULATION*/
//	Print("FILL MATRIX  : ");
	/*REM ********** ADDITION OF LOADS ***********/
	if( NL==0 ) goto L377;
//	Print( "*** Calc %d Load(s)", NL);
	F5=2*P*F*1000000;
	for( I = 1; I <= NL; I++ ){
		if( aLS[I] < 1 ) goto L366;
		/* REM ----- S-DOMAIN LOADS	*/
		U1=0;
		U2=0;
		D1=0;
		D2=0;
		S=-1;
		for( J = 0; J < aLS[I]; J+=2 ){
			S=-S;
			U1=U1+aLA[1][I][J]*S*POW(F5,J);
			D1=D1+aLA[2][I][J]*S*POW(F5,J);
			L=J+1;
			U2=U2+aLA[1][I][L]*S*POW(F5,J+1);
			D2=D2+aLA[2][I][L]*S*POW(F5,J+1);
		}
		J=aLP[I];
		D=D1*D1+D2*D2;
		LI=(U2*D1-D2*U1)/D;
		LR=(U1*D1+U2*D2)/D;
		goto L369;
	L366:;	/* :L346	*/
		LR=aLA[1][I][1];
		LI=aLA[2][I][1];
		J=aLP[I];
	L369:;	/* :L365	*/
		F2=1/M;
		if( REFACI(J,1) != -REFACI(J,2) ) goto L372;
		if( K < 0 ) F2=2/M;
	L372:;	/* :L370	*/
		IX1 = GETREF(J,J);
		REFDIM(aZR,IX1) += F2*LI;
		REFDIM(aZI,IX1) -= F2*LR;
	}
	/* REM ********** IMPEDANCE MATRIX FACTORIZATION **********	*/
	/* REM ----- BEGIN MATRIX FACTOR TIME CALCULATION			*/
L377:;	/* :L343	*/
//	Print("BEGIN FACTOR MATRIX");
	Print("FACTOR MATRIX...");
	X=N;
	PCTN=X*(X-1)*(X+X-1);
	for( K = 1; K < N; K++ ){
		/* REM ----- SEARCH FOR PIVOT	*/
		IX1 = GETREF(K,K);
		T=REFDIM(aZR,IX1)*REFDIM(aZR,IX1)+REFDIM(aZI,IX1)*REFDIM(aZI,IX1);
//		T=GetAdd(REFDIM(aZR,IX1), REFDIM(aZI,IX1));
		I1=K;
		for( I = K+1; I <= N; I++ ){
			IX1 = GETREF(I,K);
			T1=REFDIM(aZR,IX1)*REFDIM(aZR,IX1)+REFDIM(aZI,IX1)*REFDIM(aZI,IX1);
//			T1=GetAdd(REFDIM(aZR,IX1), REFDIM(aZI,IX1));
			if( T1 >= T ){
				I1=I;
				T=T1;
			}
		}
		/* REM ----- EXCHANGE ROWS K AND I1	*/
		if( I1 != K ){
			IX1 = GETREF(K,1);
			IX2 = GETREF(I1,1);
			for( J = 1; J <= N; J++ ){
				T1=REFDIM(aZR,IX1);
				T2=REFDIM(aZI,IX1);
				REFDIM(aZR,IX1)=REFDIM(aZR,IX2);
				REFDIM(aZI,IX1)=REFDIM(aZI,IX2);
				REFDIM(aZR,IX2)=T1;
				REFDIM(aZI,IX2)=T2;
				IX1++; IX2++;
			}
		}
		aPi[K]=I1;
		/* REM ----- SUBTRACT ROW K FROM ROWS K+1 TO N	*/
		for( I=K+1; I <= N; I++ ){
			/* REM ----- COMPUTE MULTIPLIER L(I,K)	*/
			IX1 = GETREF(I,K);
			IX2 = GETREF(K,K);
			T1=(REFDIM(aZR,IX1)*REFDIM(aZR,IX2)+REFDIM(aZI,IX1)*REFDIM(aZI,IX2))/T;
			T2=(REFDIM(aZI,IX1)*REFDIM(aZR,IX2)-REFDIM(aZR,IX1)*REFDIM(aZI,IX2))/T;
			REFDIM(aZR,IX1)=T1;
			REFDIM(aZI,IX1)=T2;
			/* REM ----- SUBTRACT ROW K FROM ROW I	*/
			IX1 = GETREF(I,K+1);
			IX2 = GETREF(K,K+1);
			for( J = K+1; J <= N; J++ ){
				REFDIM(aZR,IX1) -= (REFDIM(aZR,IX2)*T1-REFDIM(aZI,IX2)*T2);
				REFDIM(aZI,IX1) -= (REFDIM(aZR,IX2)*T2+REFDIM(aZI,IX2)*T1);
				IX1++; IX2++;
			}
		}
		X=N-K;
		PCT=1-X*(X-1)*(X+X-1)/PCTN;
	}
	/* REM ----- END MATRIX FACTOR TIME CALCULATION	*/
//	Print("FACTOR MATRIX: ");
	/* REM ********** SOLVE **********	*/
	/* REM ----- COMPUTE RIGHT HAND SIDE	*/
L428:; /* :L196	*/
	for( I = 1; I <= N; I++ ){
		aCR[I]=0;
		aCI[I]=0;
	}
	for( J = 1; J <= NS; J++ ){
		F2=1/M;
		if( REFACI(aEi[J],1) == -REFACI(aEi[J],2) ) F2=2/M;
		aCR[aEi[J]]=F2*aM[J];
		aCI[aEi[J]]=-F2*aL[J];
	}
	/* REM ----- PERMUTE EXCITATION	*/
	for( K=1; K < N; K++ ){
		I1=aPi[K];
		if( I1 != K ){
			T1=aCR[K];
			T2=aCI[K];
			aCR[K]=aCR[I1];
			aCI[K]=aCI[I1];
			aCR[I1]=T1;
			aCI[I1]=T2;
		}
	}
	/* REM ----- FORWARD ELIMINATION	*/
	for( I=2; I <= N; I++ ){
		T1=0;
		T2=0;
		IX1 = GETREF(I,1);
		for( J=1; J < I; J++ ){
			T1=T1+REFDIM(aZR,IX1)*aCR[J]-REFDIM(aZI,IX1)*aCI[J];
			T2=T2+REFDIM(aZR,IX1)*aCI[J]+REFDIM(aZI,IX1)*aCR[J];
			IX1++;
		}
		aCR[I]=aCR[I]-T1;
		aCI[I]=aCI[I]-T2;
	}
	/* REM ----- BACK SUBSTITUTION		*/
	for( I=N; I >= 1; I-- ){
		T1=0;
		T2=0;
		if( I != N ){
			IX1 = GETREF(I,I+1);
			for( J=I+1; J <= N; J++ ){
				T1=T1+REFDIM(aZR,IX1)*aCR[J]-REFDIM(aZI,IX1)*aCI[J];
				T2=T2+REFDIM(aZR,IX1)*aCI[J]+REFDIM(aZI,IX1)*aCR[J];
				IX1++;
			}
		}
		IX1 = GETREF(I,I);
		T=REFDIM(aZR,IX1)*REFDIM(aZR,IX1)+REFDIM(aZI,IX1)*REFDIM(aZI,IX1);
//		T=GetAdd(REFDIM(aZR,IX1), REFDIM(aZI,IX1));
		T1=aCR[I]-T1;
		T2=aCI[I]-T2;
		IX1 = GETREF(I,I);
		aCR[I]=(T1*REFDIM(aZR,IX1)+T2*REFDIM(aZI,IX1))/T;
		aCI[I]=(T2*REFDIM(aZR,IX1)-T1*REFDIM(aZI,IX1))/T;
	}
	FLG=2;
	/* REM ********** SOURCE DATA **********	*/
L477:;	/* :L197	*/
//Print("SOURCE DATA");
	Print("PULSE VOLTAGE[V]    CURRENT[mA]      IMPEDANCE(Ω)  SWR");
	PWR=0;
	for( I=1; I <= NS; I++ ){
		CRd=aCR[aEi[I]];
		CI=aCI[aEi[I]];
		T=CRd*CRd+CI*CI;
		T1=(aL[I]*CRd+aM[I]*CI)/T;
		T2=(aM[I]*CRd-aL[I]*CI)/T;
		O2=(aL[I]*CRd+aM[I]*CI)/2;
		PWR=PWR+O2;
#if 0
		Print("PULSE %d,VOLTAGE = (%lf,%lfJ)", aEi[I], aL[I], aM[I] );
		Print("CURRENT = (%lf,%lfJ)", CRd, CI);
		Print("IMPEDANCE = (%lf,%lfJ)", T1, T2);
		Print("POWER = %lf[W]", O2);
#else
		float swr = CalcSWR(T1,T2,env.RO,env.JXO);
		if( exeenv.CalcLog ){
			StrText st(512);
			st.Printf("%-5s ", pAp->cdef[I-1].PLUS);
			st.Printf("%-13s ", StrCLX(aL[I],aM[I]));
			st.Printf("%-16s ", StrCLX(CRd*1000.0, CI*1000.0));
			st.Printf("%-14s ", StrCLX(T1, T2));
			st.Printf("%.2f", swr);
			Print(st.GetText());
		}
#endif
		if( (I == 1) && (exeenv.pCurFile == NULL) ){
			RESL *rp = res.GetResP();
			rp->FQ = F;
			rp->R = T1;
			rp->JX = T2;
			rp->SWR = swr;
		}
	}
//	if( NS > 1 ) Print("TOTAL POWER = %lf[W]", PWR);
//	if( NS > 1 ) Print("TOTAL POWER                                      %lf", PWR);
//	getch();
}

void S497(void)
{
/*
	REM ********** PRINT CURRENTS **********
*/
	if( exeenv.pCurFile == NULL ){
		res.ClearCur();
	}
	S196();
	Print("CURRENT DATA...");
L501:;	/* :L568	*/
	for( K=1; K <= NW; K++ ){
//		Print("WIRE NO. %d:\n", K);
//		Print("PULSE\tREAL\tIMAGINARY\tMAGNITUDE\tPHASE\n");
//		Print(" NO.\t(AMPS)\t(AMPS)\t(AMPS)\t(DEGREES)\n");
	L507:;	/* :L502	*/
		N1=aN[K][1];
		N2=aN[K][2];
		I=N1;
		C=REFACI(I,1);
		if( (N1==0) && (N2==0) ) C=K;
		if( G != 1 ){
			if( (aJ1[K]==-1) && (N1>N2) ) N2=N1;
			if( aJ1[K]==-1 ) goto L525;
		}
		Ei=1;
		S572();
		I2f=I1f;
		J2f=J1f;
		S607();
/*		IF S$="N" THEN PRINT #3, I$,I1f;TAB(29);J1f;TAB(43);S1;TAB(57);S2*/
//		Print("b%d\t\t%8.5f %8.5f %8.5f %8.5f\n", K, I1f, J1f, S1, S2 );
		if( exeenv.pCurFile == NULL ){
			res.SetCur(pAp, K-1, I1f, J1f, S1);
		}
		else {
			WriteCur(pAp, K, -1, I1f, J1f, S1, S2);
		}
		if( N1==0 ) goto L532;
		if( C==K ) goto L525;
		if( Ic=='J' ) N1=N1+1;
	L525:;	/* :L514,L523	*/
		for( I=N1; I < N2; I++ ){
			I2f=aCR[I];
			J2f=aCI[I];
			S607();
//			Print( "A%d\t%d\t%8.5f %8.5f %8.5f %8.5f\n", K, I, aCR[I], aCI[I], S1, S2);
			if( exeenv.pCurFile == NULL ){
				res.SetCur(I-1, aCR[I], aCI[I], S1);
			}
			else {
				WriteCur(pAp, K, I, aCR[I], aCI[I], S1, S2);
			}
		}
	L532:;	/* :L522	*/
		I=N2;
		C=REFACI(I,2);
		if( (N1==0) && (N2==0) ) C=K;
		if( G==1 ) goto L537;
		if( aJ1[K]==1 ) goto L543;
	L537:;	/* :L535	*/
		Ei=2;
		S572();
		if( (N1==0) && (N2==0) ) goto L549;
		if( N1>N2 ) goto L549;
		if( C==K ) goto L543;
		if( Ic=='J' ) goto L549;
	L543:;	/* :L536,L541	*/
		I2f=aCR[N2];
		J2f=aCI[N2];
		S607();
//		Print( "B%d\t%d\t%8.5f %8.5f %8.5f %8.5f\n", K, N2, aCR[N2], aCI[N2], S1, S2);
		if( exeenv.pCurFile == NULL ){
			res.SetCur(N2-1, aCR[N2], aCI[N2],  S1);
		}
		else {
			WriteCur(pAp, K, N2, aCR[N2], aCI[N2], S1, S2);
		}
		if( aJ1[K]==1 ) goto L554;
	L549:;	/* :L539,L540,L542	*/
		I2f=I1f;
		J2f=J1f;
		S607();
		/*IF S$="N" THEN PRINT #3,I$,I1f;TAB(29);J1f;TAB(43);S1;TAB(57);S2*/
//		Print("e%d\t\t%8.5f %8.5f %8.5f %8.5f\n", K, I1f, J1f, S1, S2 );
		/*IF S$="Y" THEN PRINT #1,I1f;",";J1f;",";S1;",";S2*/
	L554:;	/* :L548	*/
		/*IF S$="Y" THEN PRINT #1," 1 , 1 , 1 , 1"*/
	}
L570:;	/* :L559*/
	res.FinishCur();
}

void S572(void)
{
/*
	REM ----- SORT JUNCTION CURRENTS
*/
	Ic='E';
	I1f=0;
	J1f=0;
	if( (C==K) || (C==0) ) goto L580;
	Ic='J';
	I1f=aCR[I];
	J1f=aCI[I];
	/* REM ----- CHECK FOR OTHER OVERLAPPING WIRES */
L580:;	/*:L575*/
	for( J=1; J <= NW; J++ ){
		if( J==K ) goto L604;
		L1=aN[J][1];
		L2=aN[J][2];
		if( Ei==2 ) goto L590;
		CO=REFACI(L1,1);
		CT=REFACI(L2,2);
		L3=L1;
		L4=L2;
		goto L594;
	L590:;	/*:L584*/
		CO=REFACI(L2,2);
		CT=REFACI(L1,1);
		L3=L2;
		L4=L1;
	L594:;	/*:L589*/
		if( CO==-K ) goto L596;
		goto L599;
	L596:;	/*:L594*/
		I1f=I1f-aCR[L3];
		J1f=J1f-aCI[L3];
		Ic='J';
	L599:;	/*:L595*/
		if( CT==K ){
			I1f=I1f+aCR[L4];
			J1f=J1f+aCI[L4];
			Ic='J';
		}
	L604:;	/*:L581,L600*/
	}
}

void S607(void)
{
/*
	REM ----- CALCULATE S1 AND S2
*/
	I3f=I2f*I2f;
	J3f=J2f*J2f;
	if( (I3f>0) || (J3f>0) ) goto L612;
	S1=0.0;
	goto L613;
L612:;	/*:L609*/
	S1=sqrt(I3f+J3f);
L613:;	/*:L611*/
	if( I2f!=0 ){
		S2=atan(J2f/I2f)/P0;
		if( I2f > 0 ) return;
		S2=S2+SGN(J2f)*180;
	}
	else {
		S2=0.0;
	}
}

void S621(void)
{
/*
	REM ********** FAR FIELD CALCULATION **********
*/
	FILE	*fp;
	fp = NULL;

	if( FLG < 2 ) S196();
	O2=PWR;
	/* REM ----- TABULATE IMPEDANCE	*/
	if( NM==0 ) goto L634;
	for( I=1; I <= NM; I++ ){
		Z6=aT[I];
		Z7=-aV[I]/(2*P*F*8.85E-06);
		/* REM ----- FORM IMPEDANCE=1/SQR(DIELECTRIC CONSTANT)	*/
		S184();
		D=W6*W6+W7*W7;
		aZ1[I]=W6/D;
		aZ2[I]=-W7/D;
	}
L634:;	/* :L624*/
//	Print( "FAR FIELD");
	/* REM ----- INPUT VARIABLES FOR FAR FIELD CALCULATION	*/
L638:;	/* :L640*/
//	Print( "\nCALCULATE PATTERN IN DBI OR VOLTS/METER (D/V)" );
//	Pc = getch();
//	Pc = toupper(Pc);
//	Print( "%c\n", Pc);
	Pc = 'D';
	if( Pc=='D' ) goto L655;
	if( Pc!='V' ) goto L638;
	F1=1;
//	Print( "\n" );
//	Print( "PRESENT POWER LEVEL =  %f[W]\n", PWR);
L644:;	/* :L646,L649*/
//	while(1){
//		Ac = inputYN("CHANGE POWER LEVEL (Y/N) ");
//		if( Ac=='N' ) goto L650;
//		if( Ac!='Y' ) goto L644;
//		input( "NEW POWER LEVEL (WATTS)  ", "%lf", &O2);
//	}
L650:;	/* :L645*/
	if( (O2<0) || (O2==0) ) O2=PWR;
	F1=sqrt(O2/PWR);
//	Print( "\n" );
//	input("RADIAL DISTANCE (METERS) ", "%lf", &RD);
	if( RD<0 ) RD=0;
L655:;	/* :L639*/
//	input("ZENITH ANGLE : INITIAL,INCREMENT,NUMBER", "%lf,%lf,%d", &ZA,&ZC,&NZ );
	if( NZ==0 ) NZ=1;
//	input("AZIMUTH ANGLE: INITIAL,INCREMENT,NUMBER", "%lf,%lf,%d", &AA,&AC,&NA );
	if( NA==0 ) NA=1;
	/* REM ********** FILE FAR FIELD DATA ***********/
L667:;	/* :L669	*/
//	Sc = inputYN("FILE PATTERN (Y/N)");
//	if( Sc=='N' ) goto L676;
//	if( Sc!='Y' ) goto L667;
//	fp = fopen("OUTPUT.PTN", "wt");
/*	INPUT "FILENAME (NAME.OUT)";F$*/
/*	IF LEFT$(RIGHT$(F$,4),1)="." THEN L673 ELSE F$=F$+".OUT"*/
L673:;	/* :L672	*/
/*	OPEN F$ FOR OUTPUT AS #1*/
/*	PRINT #1,NA*NZ;",";O2;",";P$*/
//	fprintf(fp, "%d,%f\n", NA*NZ,O2);
L676:;	/* :L668	*/
/*	PRINT #3, " "*/
	K9f=.016678/PWR;
	/* REM ----- PATTERN HEADER	*/
//	Print("PATTERN DATA");
#if 0
	if( Pc=='V' ){
		if( RD>0 ) Print("RADIAL DISTANCE = %f[m]\n", RD);
		Print("POWER LEVEL = %f[W]\n", PWR*F1*F1);
		Print("ZENITH\tAZIMUTH\tE(THETA)\tE(PHI)\n");
		Print(" ANGLE\tANGLE\tMAG(V/M)\tPHASE(DEG)\tMAG(V/M)\tPHASE(DEG)\n");
		if( fp != NULL ){
			fprintf(fp, "POWER LEVEL = %f[W]\n", PWR*F1*F1);
			fprintf(fp, "ZENITH\tAZIMUTH\tE(THETA)\tE(PHI)\n");
			fprintf(fp, " ANGLE\tANGLE\tMAG(V/M)\tPHASE(DEG)\tMAG(V/M)\tPHASE(DEG)\n");
		}
	}
	else {
		Print("ZENITH\tAZIMUTH\tVERT\tHORI\tTOTAL\n");
		Print(" ANGLE\tANGLE\tPAT(DB)\tPAT(DB)\tPAT(DB)\n");
		if( fp != NULL ){
			fprintf(fp, "ZENITH\tAZIMUTH\tVERT\tHORI\tTOTAL\n");
			fprintf(fp," ANGLE\tANGLE\tPAT(DB)\tPAT(DB)\tPAT(DB)\n");
		}
	}
#endif
	/* REM ----- LOOP OVER AZIMUTH ANGLE	*/
L692:;	/* :L684	*/
	Q1=AA;
	for( I1=1; I1 <= NA; I1++ ){
		U3=Q1*P0;
		V1=-sin(U3);
		V2=cos(U3);
		/* REM ----- LOOP OVER ZENITH ANGLE	*/
		Q2=ZA;
		for( I2=1; I2<=NZ; I2++ ){
			U4=Q2*P0;
			R3=cos(U4);
			T3=-sin(U4);
			T1=R3*V2;
			T2=-R3*V1;
			R1=-T3*V2;
			R2=T3*V1;
			X1=0;
			Y1=0;
			Z1=0;
			X2=0;
			Y2=0;
			Z2=0;
			/* REM ----- IMAGE LOOP	*/
			for( K=1; K >= G; K-=2 ){
				for( I=1; I <= N; I++ ){
					if( K>0 ) goto L718;
					if( REFACI(I,1) == -REFACI(I,2) ) goto L813;
				L718:;	/* :L716	*/
					J=2*aWi[I]-1+I;
					/* REM ----- FOR EACH END OF PULSE COMPUTE A CONTRIBUTION TO E-FIELD*/
					for( F5i=1; F5i <= 2; F5i++ ){
						L=ABS(REFACI(I,F5i));
						F3=SGN(REFACI(I,F5i))*W*aS[L]/2;
						if( REFACI(I,1) != -REFACI(I,2) ) goto L725;
						if( F3<0 ) goto L812;
					L725:;	/* :L723*/
						if( K==1 ) goto L728;
						if( NM != 0 ) goto L747;
						/* REM ----- STANDARD CASE*/
					L728:;	/* :L725*/
						S2=W*(aX[J]*R1+aY[J]*R2+aZ[J]*K*R3);
						S1=cos(S2);
						S2=sin(S2);
						B1=F3*(S1*aCR[I]-S2*aCI[I]);
						B2=F3*(S1*aCI[I]+S2*aCR[I]);
						if( REFACI(I,1) == -REFACI(I,2) ) goto L742;
						X1=X1+K*B1*aCA[L];
						X2=X2+K*B2*aCA[L];
						Y1=Y1+K*B1*aCB[L];
						Y2=Y2+K*B2*aCB[L];
						Z1=Z1+B1*aCG[L];
						Z2=Z2+B2*aCG[L];
						goto L812;
						/* REM ----- GROUNDED ENDS*/
					L742:;	/* :L733	*/
						Z1=Z1+2*B1*aCG[L];
						Z2=Z2+2*B2*aCG[L];
						goto L812;
						/*REM ----- REAL GROUND CASE*/
						/*REM ----- BEGIN BY FINDING SPECULAR DISTANCE*/
					L747:;	/* :L726	*/
						T4=100000;
						if( R3==0 ) goto L750;
						T4=-aZ[J]*T3/R3;
					L750:;	/* :L748	*/
						B9=T4*V2+aX[J];
						if( TB==1 ) goto L755;
						B9=B9*B9+POW((aY[J]-T4*V1),2);
						if( B9>0 ){
							B9=sqrt(B9);
						}
						else {
							goto L755;
						}
						/* REM ----- SEARCH FOR THE CORRESPONDING MEDIUM*/
					L755:;	/* :L751	*/
						J2=NM;
						for( J1=NM; J1 >= 1; J1-- ){
							if( B9 <= aU[J1] ){
								J2=J1;
							}
						}
						/*REM ----- OBTAIN IMPEDANCE AT SPECULAR POINT*/
						Z4=aZ1[J2];
						Z5=aZ2[J2];
						/*REM ----- IF PRESENT INCLUDE GROUND SCREEN IMPEDANCE IN PARALLEL*/
						if( NR==0 ) goto L776;
						if( B9>aU[1] ) goto L776;
						R=B9+NR*RR;
						Z8=W*R*log(R/(NR*RR))/NR;
						S8=-Z5*Z8;
						S9=Z4*Z8;
						T8=Z4;
						T9=Z5+Z8;
						D=T8*T8+T9*T9;
						Z4=(S8*T8+S9*T9)/D;
						Z5=(S9*T8-S8*T9)/D;
						/* REM ----- FORM SQR(1-Z^2*SIN^2)*/
					L776:;	/* :L764,L765*/
						Z6=1-(Z4*Z4-Z5*Z5)*T3*T3;
						Z7=-(2*Z4*Z5)*T3*T3;
						S184();
						/*REM ----- VERTICAL REFLECTION COEFFICIENT*/
						S8=R3-(W6*Z4-W7*Z5);
						S9=-(W6*Z5+W7*Z4);
						T8=R3+(W6*Z4-W7*Z5);
						T9=W6*Z5+W7*Z4;
						D=T8*T8+T9*T9;
						V8=(S8*T8+S9*T9)/D;
						V9=(S9*T8-S8*T9)/D;
						/*REM ----- HORIZONTAL REFLECTION COEFFICIENT*/
						S8=W6-R3*Z4;
						S9=W7-R3*Z5;
						T8=W6+R3*Z4;
						T9=W7+R3*Z5;
						D=T8*T8+T9*T9;
						H8=(S8*T8+S9*T9)/D-V8;
						H9=(S9*T8-S8*T9)/D-V9;
						/*REM ----- COMPUTE CONTRIBUTION TO SUM*/
						S2=W*(aX[J]*R1+aY[J]*R2-(aZ[J]-2*aH[J2])*R3);
						S1=cos(S2);
						S2=sin(S2);
						B1=F3*(S1*aCR[I]-S2*aCI[I]);
						B2=F3*(S1*aCI[I]+S2*aCR[I]);
						W6=B1*V8-B2*V9;
						W7=B1*V9+B2*V8;
						D=aCA[L]*V1+aCB[L]*V2;
						Z6=D*(B1*H8-B2*H9);
						Z7=D*(B1*H9+B2*H8);
						X1=X1-(aCA[L]*W6+V1*Z6);
						X2=X2-(aCA[L]*W7+V1*Z7);
						Y1=Y1-(aCB[L]*W6+V2*Z6);
						Y2=Y2-(aCB[L]*W7+V2*Z7);
						Z1=Z1+aCG[L]*W6;
						Z2=Z2+aCG[L]*W7;
					L812:;	/* :L724,L740,L744*/
					}
				L813:;	/* :L717*/
				}
			}
			H2=-(X1*T1+Y1*T2+Z1*T3)*G0;
			H1=(X2*T1+Y2*T2+Z2*T3)*G0;
			X4=-(X1*V1+Y1*V2)*G0;
			X3=(X2*V1+Y2*V2)*G0;
			if( Pc=='D' ) goto L827;
			if( RD==0 ) goto L842;
			H1=H1/RD;
			H2=H2/RD;
			X3=X3/RD;
			X4=X4/RD;
			goto L842;
			/*REM ----- PATTERN IN DB*/
		L827:;	/* :L819*/
			P1=-999;
			P2=P1;
			P3=P1;
			T1=K9f*(H1*H1+H2*H2);
			T2=K9f*(X3*X3+X4*X4);
			T3=T1+T2;
			/*REM ----- CALCULATE VALUES IN DB*/
			if( T1>1E-30 ) P1=4.343*log(T1);
			if( T2>1E-30 ) P2=4.343*log(T2);
			if( T3>1E-30 ) P3=4.343*log(T3);
//			Print("%7.3f\t%7.3f\t%7.3f\t%7.3f\t%7.3f\n", Q2,Q1,P1,P2,P3 );
			if( exeenv.pFarFile == NULL ){
				res.SetPtn(P1, P2, P3);
			}
			else {
				fprintf(exeenv.pFarFile->fp, "%.3lf,%.3lf,%.3lf,%.3lf,%.3lf\n", Q2, Q1, P1, P2, P3);
			}
			goto L866;
			/*REM ----- PATTERN IN VOLTS/METER*/
			/*REM ----- MAGNITUDE AND PHASE OF E(THETA)*/
		L842:;	/* :L820,L825*/
			S1=0;
			if( (H1==0) && (H2==0) ) goto L845;
			S1=sqrt(H1*H1+H2*H2);
		L845:;	/* :L843	*/
			if( H1!=0 ) goto L848;
			S2=0;
			goto L851;
		L848:;	/* :L845	*/
			S2=atan(H2/H1)/P0;
			if( H1<0 ) S2=S2+SGN(H2)*180;
			/* REM ----- MAGNITUDE AND PHASE OF E(PHI)*/
		L851:;	/* :L847	*/
			S3=0;
			if( (X3==0) && (X4==0) ) goto L854;
			S3=sqrt(X3*X3+X4*X4);
		L854:;	/* :L852	*/
			if( X3 != 0 ) goto L857;
			S4=0;
			goto L859;
		L857:;	/* :L854	*/
			S4=atan(X4/X3)/P0;
			if( X3<0 ) S4=S4+SGN(X4)*180;
		L859:;	/* :L856	*/
#if 0
			Print( "%6.2f\t%6.2f", Q2,Q1);
			Print( "\t%6.2f", S1*F1);
			Print( "\t%6.2f", S2);
			Print( "\t%6.2f", S3*F1);
			Print( "\t%6.2f\n", S4);
			if( fp != NULL ){
				fprintf( fp, "%6.2f\t%6.2f", Q2,Q1);
				fprintf( fp, "\t%6.2f", S1*F1);
				fprintf( fp, "\t%6.2f", S2);
				fprintf( fp, "\t%6.2f", S3*F1);
				fprintf( fp, "\t%6.2f\n", S4);
			}
#endif
			/* REM ----- INCREMENT ZENITH ANGLE	*/
		L866:;	/* :L839	*/
			Q2=Q2+ZC;
		}
		/* REM ----- INCREMENT AZIMUTH ANGLE	*/
		Q1=Q1+AC;
	}
//	if( fp != NULL ) fclose(fp);
}

void S875(void)
{
/*
	REM ********** NEAR FIELD CALCULATION **********
	REM ----- ENSURE CURRENTS HAVE BEEN CALCULATED
*/
	const char _vt1[]="XXYZ";
	const char _vt2[]="XXXYYZZ";

	if( FLG < 2 ) S196();
#if 0
	FVS=-1;O2=PWR;
	Print( "    NEAR FIELDS     \n");
	while(1){
		Print( "\nELECTRIC OR MAGNETIC NEAR FIELDS (E/H):" );
//		Nc = getch();
		Print( "%c", Nc );
		if( (Nc!='H') && (Nc!='E')) break;
	}
	Print( "\n" );
	/*	REM ----- INPUT VARIABLES FOR NEAR FIELD CALCULATION	*/
	Print( "FIELD LOCATION(S):" );
	input( "X-COORDINATE (M): INITIAL,INCREMENT,NUMBER :", "%lf,%lf,%d", &XI,&XC,&NX );
	if( NX==0 ) NX=1;
	input( "Y-COORDINATE (M): INITIAL,INCREMENT,NUMBER :", "%lf,%lf,%d", &YI,&YC,&NY );
	if( NY==0 ) NY=1;
	input( "Z-COORDINATE (M): INITIAL,INCREMENT,NUMBER :", "%lf,%lf,%d", &ZI,&ZC,&NZ );
	if( NZ==0 ) NZ=1;
	F1=1;
	Print( "\nPRESENT POWER LEVEL IS %f[W]\n", PWR);
	while(1){
		Ac = inputYN("CHANGE POWER LEVEL (Y/N) ");
		if( Ac == 'N' ) break;
		if( Ac == 'Y' ){
			input( "NEW POWER LEVEL (WATTS) : ", "%lf", &O2 );
		}
	}
	if( (O2<0) || (O2==0) ) O2=PWR;
#else
	FVS = -1;
	O2 = exeenv.pNearFile->POW;
	Nc = exeenv.pNearFile->TYPE ? 'H' : 'E';
	XI = exeenv.pNearFile->X;
	XC = exeenv.pNearFile->XW;
	NX = exeenv.pNearFile->XN;
	YI = exeenv.pNearFile->Y;
	YC = exeenv.pNearFile->YW;
	NY = exeenv.pNearFile->YN;
	ZI = exeenv.pNearFile->Z;
	ZC = exeenv.pNearFile->ZW;
	NZ = exeenv.pNearFile->ZN;
	if( !XC ) NX = 1;
	if( !YC ) NY = 1;
	if( !ZC ) NZ = 1;
	if( NX==0 ) NX=1;
	if( NY==0 ) NY=1;
	if( NZ==0 ) NZ=1;
#endif
	/* REM ----- RATIO OF POWER LEVELS*/
	F1=sqrt(O2/PWR);
	if( Nc=='H' ) F1=F1/S0/4/P;
//	Print( "\n" );
	/* REM ----- DESIGNATION OF OUTPUT FILE FOR NEAR FIELD DATA*/
L914:;	/* :L916	*/
/*	INPUT "SAVE TO A FILE (Y/N) ";S$	*/
/*	IF S$="N" THEN L923	*/
/*	IF S$<>"Y" THEN L914	*/
/*	INPUT "FILENAME (NAME.OUT)  ";F$	*/
/*	IF LEFT$(RIGHT$(F$,4),1)="." THEN L919 ELSE F$=F$+".OUT"	*/
L919:;	/* :L918	*/
/*	IF O$>"C" THEN PRINT #3," ":PRINT #3,"FILENAME (NAME.OUT) ";F$	*/
/*	OPEN F$ FOR OUTPUT AS #2	*/
/*	PRINT #2,NX*NY*NZ;",";O2;",";N$	*/
	/* REM ----- LOOP OVER Z DIMENSION	*/
L923:;	/* :L915	*/
	for( IZ=1; IZ <= NZ; IZ++ ){
		ZZ=ZI+(IZ-1)*ZC;
		/* REM ----- LOOP OVER Y DIMENSION	*/
		for( IY=1; IY <= NY; IY++ ){
			YY=YI+(IY-1)*YC;
			/* REM ----- LOOP OVER X DIMENSION	*/
			for( IX=1; IX <= NX; IX++ ){
				XX=XI+(IX-1)*XC;
				/* REM ----- NEAR FIELD HEADER	*/
#if 0
				Print( "\n" );
				if( Nc == 'E' ){
					Print( "*NEAR ELECTRIC FIELDS*\n");
				}
				else {
					Print( "*NEAR MAGNETIC FIELDS*\n");
				}
				Print( "FIELD POINT: X = %f\tY = %f\tZ = %f\n", XX, YY, ZZ);
				Print( "VECTOR\tREAL\tIMAGINARY\tMAGNITUDE\tPHASE\n");
				if( Nc == 'E' ){
					Print( "COMPONENT\t V/M \t V/M \t V/M \tDEG\n" );
				}
				else {
					Print( "COMPONENT\t AMPS/M \t AMPS/M \t AMPS/M \t DEG\n");
				}
#endif
				A1=0;
				A3=0;
				A4=0;
				/* REM ----- LOOP OVER THREE VECTOR COMPONENTS*/
				for( I=1; I<=3; I++ ){
					X0=XX;
					Y0=YY;
					Z0=ZZ;
					if( Nc=='H' ) goto L958;
					T5=0;
					T6=0;
					T7=0;
					switch(I){
						case 1:
							T5=2*S0;
							break;
						case 2:
							T6=2*S0;
							break;
						case 3:
							T7=2*S0;
							break;
					}
					U7=0;
					U8=0;
					goto L968;
				L958:;	/* :L948	*/
					for( J8=1; J8 <= 6; J8++ ){
						aKf[J8][1]=0;
						aKf[J8][2]=0;
					}
					J9=1;
					J8=-1;
				L964:;	/* :L1057	*/
					switch(I){
						case 1:
							X0=XX+J8*S0/2;
							break;
						case 2:
							Y0=YY+J8*S0/2;
							break;
						case 3:
							Z0=ZZ+J8*S0/2;
							break;
					}
					/* REM ----- LOOP OVER SOURCE SEGMENTS	*/
				L968:;	/* :L957	*/
					for( J=1; J <= N; J++ ){
						J1=ABS(REFACI(J,1));
						J2=ABS(REFACI(J,2));
						J3=J2;
						if( J1>J2 ) J3=J1;
						F4=SGN(REFACI(J,1));
						F5=SGN(REFACI(J,2));
						F6=1;
						F7=1;
						U5=0;
						U6=0;
						/* REM ----- IMAGE LOOP	*/
						for( K=1; K >= G; K -= 2){
							if( REFACI(J,1) != -REFACI(J,2) ) goto L987;
							if( K<0 ) goto L1048;
							/* REM ----- COMPUTE VECTOR POTENTIAL A	*/
							F6=F4;
							F7=F5;
							/* REM ----- COMPUTE PSI(0,J,J+.5)	*/
						L987:;	/* :L981	*/
							P1=0;
							P2=2*J3+J-1;
							P3=P2+.5;
							P4i=J2;
							S75();
							U1=T1*F5;
							U2=T2*F5;
							/* REM ----- COMPUTE PSI(0,J-.5,J)	*/
							P3=P2;
							P2=P2-.5;
							P4i=J1;
							S66();
							V1=F4*T1;
							V2=F4*T2;
							/* REM ----- REAL PART OF VECTOR POTENTIAL CONTRIBUTION	*/
							X3=U1*aCA[J2]+V1*aCA[J1];
							Y3=U1*aCB[J2]+V1*aCB[J1];
							Z3=(F7*U1*aCG[J2]+F6*V1*aCG[J1])*K;
							/* REM ----- IMAGINARY PART OF VECTOR POTENTIAL CONTRIBUTION	*/
							X5=U2*aCA[J2]+V2*aCA[J1];
							Y5=U2*aCB[J2]+V2*aCB[J1];
							Z5=(F7*U2*aCG[J2]+F6*V2*aCG[J1])*K;
							/* REM ----- MAGNETIC FIELD CALCULATION COMPLETED	*/
							if( Nc=='H' ) goto L1042;
							D1=(X3*T5+Y3*T6+Z3*T7)*W2;
							D2=(X5*T5+Y5*T6+Z5*T7)*W2;
							/* REM ----- COMPUTE PSI(.5,J,J+1)	*/
							P1=.5;
							P2=P3;
							P3=P3+1;
							P4i=J2;
							S56();
							U1=T1;
							U2=T2;
							/* REM ----- COMPUTE PSI(-.5,J,J+1)	*/
							P1=-P1;
							S56();
							U1=(T1-U1)/aS[J2];
							U2=(T2-U2)/aS[J2];
							/* REM ----- COMPUTE PSI(.5,J-1,J)	*/
							P1=-P1;
							P3=P2;
							P2=P2-1;
							P4i=J1;
							S56();
							U3=T1;
							U4=T2;
							/* REM ----- COMPUTE PSI(-.5,J-1,J)	*/
							P1=-P1;
							S56();
							/* REM ----- GRADIENT OF SCALAR POTENTIAL	*/
							U5=(U1+(U3-T1)/aS[J1]+D1)*K+U5;
							U6=(U2+(U4-T2)/aS[J1]+D2)*K+U6;
							goto L1048;
							/* REM ----- COMPONENTS OF VECTOR POTENTIAL A	*/
						L1042:;	/* :L1010*/
							aKf[1][J9]=aKf[1][J9]+(X3*aCR[J]-X5*aCI[J])*K;
							aKf[2][J9]=aKf[2][J9]+(X5*aCR[J]+X3*aCI[J])*K;
							aKf[3][J9]=aKf[3][J9]+(Y3*aCR[J]-Y5*aCI[J])*K;
							aKf[4][J9]=aKf[4][J9]+(Y5*aCR[J]+Y3*aCI[J])*K;
							aKf[5][J9]=aKf[5][J9]+(Z3*aCR[J]-Z5*aCI[J])*K;
							aKf[6][J9]=aKf[6][J9]+(Z5*aCR[J]+Z3*aCI[J])*K;
						L1048:;	/*:L982,L1040*/
						}
						if( Nc=='H' ) goto L1052;
						U7=U5*aCR[J]-U6*aCI[J]+U7;
						U8=U6*aCR[J]+U5*aCI[J]+U8;
					L1052:;	/* :L1049*/
					}
					if( Nc=='E' ) goto L1075;
					/* REM ----- DIFFERENCES OF VECTOR POTENTIAL A	*/
					J8=1;
					J9=J9+1;
					if( J9==2 ) goto L964;
					/*ON I GOTO L1059,1064,1069*/
					switch(I){
						case 1:
							aH[3]=aKf[5][1]-aKf[5][2];
							aH[4]=aKf[6][1]-aKf[6][2];
							aH[5]=aKf[3][2]-aKf[3][1];
							aH[6]=aKf[4][2]-aKf[4][1];
							break;
						case 2:
							aH[1]=aKf[5][2]-aKf[5][1];
							aH[2]=aKf[6][2]-aKf[6][1];
							aH[5]=aH[5]-aKf[1][2]+aKf[1][1];
							aH[6]=aH[6]-aKf[2][2]+aKf[2][1];
							break;
						case 3:
							aH[1]=aH[1]-aKf[3][2]+aKf[3][1];
							aH[2]=aH[2]-aKf[4][2]+aKf[4][1];
							aH[3]=aH[3]+aKf[1][2]-aKf[1][1];
							aH[4]=aH[4]+aKf[2][2]-aKf[2][1];
							break;
					}
					goto L1097;
					/* REM ----- IMAGINARY PART OF ELECTRIC FIELD*/
				L1075:;	/* :L1053*/
					U7=-M*U7/S0;
					/* REM ----- REAL PART OF ELECTRIC FIELD	*/
					U8=M*U8/S0;
					/* REM ----- MAGNITUDE AND PHASE CALCULATION	*/
					S1=0;
					if( U7 || U8 ){
						S1=sqrt(U7*U7+U8*U8);
					}
					S2=0;
					if( U8 != 0 ) S2=atan(U7/U8)/P0;
					if( U8 > 0 ) goto L1086;
					S2=S2+SGN(U7)*180;
				L1086:;	/* :L1084	*/
#if 0
					switch(I){
						case 1:
							Print( "   X  \n");
							break;
						case 2:
							Print( "   Y  \n");
							break;
						case 3:
							Print( "   Z  \n");
							break;
					}
					Print( "%f\t%f\t%f\t%f\n", F1*U8, F1*U7, F1*S1, S2);
#endif
					fprintf(exeenv.pNearFile->fp, "%lf,%lf,%lf,%c,%le,%le,%le,%lf\n",
						XX, YY, ZZ,
						_vt1[I],
						F1*U8*IVSQ2, F1*U7*IVSQ2, F1*S1*IVSQ2, S2
					);
					/* REM ----- CALCULATION FOR PEAK ELECTRIC FIELD	*/
					S1=S1*S1;
					S2=S2*P0;
					A1=A1+S1*cos(2*S2);
					A3=A3+S1*sin(2*S2);
					A4=A4+S1;
				L1097:;	/*:L1063,L1068,L1073	*/
				}
				if( Nc=='E' ) goto L1121;
				/* REM ----- MAGNETIC FIELD MAGNITUDE AND PHASE CALCULATION*/
				for( I=1; I <= 5; I+=2){
					S1=0;
					if( aH[I] || aH[I+1] ){
						S1=sqrt(aH[I]*aH[I]+aH[I+1]*aH[I+1]);
					}
					S2=0;
					if( aH[I] != 0 ) S2=atan(aH[I+1]/aH[I])/P0;
					if( aH[I] <= 0 ){
						S2=S2+SGN(aH[I+1])*180;
					}
#if 0
					switch(I){
						case 1:
							Print( "   X  \n");
							break;
						case 3:
							Print( "   Y  \n");
							break;
						case 5:
							Print( "   Z  \n");
							break;
					}
					Print( "%f\n%f\t%f\t%f\n", F1*aH[I], F1*aH[I+1], F1*S1, S2);
#endif
					fprintf(exeenv.pNearFile->fp, "%lf,%lf,%lf,%c,%le,%le,%le,%lf\n",
						XX, YY, ZZ,
						_vt2[I],
						F1*aH[I]*IVSQ2, F1*aH[I+1]*IVSQ2, F1*S1*IVSQ2, S2
					);
					/* REM ----- CALCULATION FOR PEAK MAGNETIC FIELD	*/
					S1=S1*S1;
					S2=S2*P0;
					A1=A1+S1*cos(2*S2);
					A3=A3+S1*sin(2*S2);
					A4=A4+S1;
				}
				/* REM ----- PEAK FIELD CALCULATION	*/
			L1121:;	/* :L1098*/
				PK=sqrt(A4/2+sqrt(A1*A1+A3*A3)/2);
//				Print( "   MAXIMUM OR PEAK FIELD = %f\n", F1*PK);
			}
		}
	}
}

void S1299(void)
{
/*
	REM ********** CONNECTIONS **********
*/

	aEf[I]=X1;
	aL[I]=Y1;
	aM[I]=Z1;
	aEf[I+NW]=X2;
	aL[I+NW]=Y2;
	aM[I+NW]=Z2;
	Gi=0;
	I1=0;
	I2=0;
	aJ1[I]=0;
	aJ2[I][1]=-I;
	aJ2[I][2]=-I;
	if( G==1 ) goto L1323;
	/*REM ----- CHECK FOR GROUND CONNECTION*/
	if( Z1==0 ) goto L1315;
	goto L1318;
L1315:;	/*:L1313*/
	I1=-I;
	aJ1[I]=-1;
	goto L1340;
L1318:;	/*:L1314*/
	if( Z2==0 ) goto L1320;
	goto L1323;
L1320:;	/*:L1318*/
	I2=-I;
	aJ1[I]=1;
	Gi=1;
L1323:;	/*:L1311,L1319*/
	if( I==1 ) goto L1358;
	for( J=1; J < I; J++ ){
		/*REM ----- CHECK FOR END1 TO END1*/
		if( (X1==aEf[J]) && (Y1==aL[J]) && (Z1==aM[J]) ) goto L1328;
		goto L1333;
	L1328:;	/*:L1326*/
		I1=-J;
		aJ2[I][1]=J;
		if( aJ2[J][1]==-J ) aJ2[J][1]=J;
		goto L1340;
		/* REM ----- CHECK FOR END1 TO END2*/
	L1333:;	/*:L1327*/
		if( (X1==aEf[J+NW]) && (Y1==aL[J+NW]) && (Z1==aM[J+NW]) ) goto L1335;
		goto L1339;
	L1335:;	/*:L1333*/
		I1=J;
		aJ2[I][1]=J;
		if( aJ2[J][2] == -J ) aJ2[J][2]=J;
		goto L1340;
	L1339:;	/*:L1334*/
	}
L1340:;	/*:L1317,L1331,L1338*/
	if( Gi==1 ) goto L1358;
	if( I==1 ) goto L1358;
	for( J=1; J < I; J++ ){
		/*REM ----- CHECK END2 TO END2*/
		if( (X2==aEf[J+NW]) && (Y2==aL[J+NW]) && (Z2==aM[J+NW]) ) goto L1346;
		goto L1351;
	L1346:;	/*:L1344*/
		I2=-J;
		aJ2[I][2]=J;
		if( aJ2[J][2]==-J ) aJ2[J][2]=J;
		goto L1358;
		/*REM ----- CHECK FOR END2 TO END1*/
	L1351:;	/*:L1345*/
		if( (X2==aEf[J]) && (Y2==aL[J]) && (Z2==aM[J]) ) goto L1353;
		goto L1357;
	L1353:;	/*:L1351*/
		I2=J;
		aJ2[I][2]=J;
		if( aJ2[J][1]==-J ) aJ2[J][1]=J;
		goto L1358;
	L1357:;	/*:L1352*/
	}
L1358:;	/*:L1323,L1340,L1341,L1349,L1356*/
//	Print( "\tCOORDINATES\t \t \tEND\tNO. OF");
//	Print( "   X\t   Y\t   Z\tRADIUS\tCONNECTION\tSEGMENTS");
//	Print( "%7.3f\t%7.3f\t%7.3f\t\t%d", X1, Y1, Z1, I1);
//	Print( "%7.3f\t%7.3f\t%7.3f\t%7.3f\t%d\t%d", X2, Y2, Z2, aA[I], I2, (int)S1 );
}

#if 0
void S1364(void)
{
/*
	REM ********** ENVIROMENT INPUT **********
*/
	Print("\n");
	Print("				**** WARNING ****\n");
	Print("REDO GEOMETRY TO ENSURE PROPER GROUND CONNECTION/DISCONNECTION\n");
	Print("\n");
	S1369();
}

void S1369(void)
{
/*
	REM ----- INITIALIZE NUMBER OF RADIAL WIRES TO ZERO
*/

	NR=0;
	/*REM ----- SET ENVIRONMENT*/
	Print(" ");
L1373:;	/*:L1377*/
	input( "ENVIRONMENT (+1 FOR FREE SPACE, -1 FOR GROUND PLANE)", "%d", &G);
	if( G==1 ) goto L1428;
	if( G!=-1) goto L1373;
	/* REM ----- NUMBER OF MEDIA*/
L1380:;/*:L1384*/
	input( " NUMBER OF MEDIA (0 FOR PERFECTLY CONDUCTING GROUND)", "%d", &NM);
	if( NM<=MM ) goto L1385;
	Print( "NUMBER OF MEDIA EXCEEDS DIMENSION..." );
	goto L1380;
L1385:;	/*:L1382*/
	/*REM ----- INITIALIZE BOUNDARY TYPE*/
	TB=1;
	if( NM==0 ) goto L1428;
	if( NM==1 ) goto L1396;
	/*REM ----- TYPE OF BOUNDARY*/
	input(" TYPE OF BOUNDARY (1-LINEAR, 2-CIRCULAR)", "%d", &TB);
	/*REM ----- BOUNDARY CONDITIONS*/
L1396:;	/*:L1389*/
	for( I=1; I <= NM; I++ ){
		Print( "MEDIA %d\n", I );
		input( " RELATIVE DIELECTRIC CONSTANT, CONDUCTIVITY", "%lf,%lf", &aT[I], &aV[I]);
		if( I>1 ) goto L1414;
		if( TB==1 ) goto L1414;
		input( " NUMBER OF RADIAL WIRES IN GROUND SCREEN", "%d", &NR);
		if( NR==0 ) goto L1414;
		input(" RADIUS OF RADIAL WIRES", "%lf", &RR);
		/*REM ----- INITIALIZE COORDINATE OF MEDIA INTERFACE*/
	L1414:;	/*:L1402,L1403,L1408*/
		aU[I]=1000000;
		/*REM ----- INITIALIZE HEIGHT OF MEDIA*/
		aH[I]=0;
		if( I==NM ) goto L1422;
		input(" X OR R COORDINATE OF NEXT MEDIA INTERFACE", "%lf", &aU[I]);
	L1422:;	/*:L1417*/
		if( I==1 ) goto L1427;
		input( " HEIGHT OF MEDIA", "%lf", &aH[I] );
	L1427:;	/*:L1422*/
	}
L1428:;	/*:L1376,L1388*/
}

void S1430(void)
{
/*
	REM ********** EXCITATION INPUT **********
*/
	Print( "\n");
L1432:;	/*:L1437*/
	input( "NO. OF SOURCES :", "%d", &NS);
	if( NS<1 ) NS=1;
	if( NS<=MP ) goto L1438;
	Print( "NO. OF SOURCES EXCEEDS DIMENSION ..." );
	goto L1432;
L1438:;	/*:L1435*/
	for( I = 1; I <= NS; I++ ){
		Print( "\n");
		Print( "SOURCE NO. %d:\n", I);
	L1443:;	/*:L1447*/
		input("PULSE NO., VOLTAGE MAGNITUDE, PHASE (DEGREES) :", "%d,%lf,%lf", &aEi[I],&VM,&VP );
		if( aEi[I]<=N ) goto L1448;
		Print( "PULSE NUMBER EXCEEDS NUMBER OF PULSES..." );
		goto L1443;
	L1448:;	/*:L1445*/
		aL[I]=VM*cos(VP*P0);
		aM[I]=VM*sin(VP*P0);
	}
	if( FLG==2 ) FLG=1;
}

void S1455(void)
{
/*
	REM ********** LOADS INPUT **********
*/
	Print( "\n" );
L1456:;	/*:L1459*/
	input("NUMBER OF LOADS	 :", "%d", &NL );
	if( NL<=ML ) goto L1460;
	Print( "NUMBER OF LOADS EXCEEDS DIMENSION..." );
	goto L1456;
L1460:;	/*:L1457*/
	if( NL<1 ) goto L1492;
L1462:;	/*:L1463*/
	Lc = inputYN("S-DOMAIN (S=jw) IMPEDANCE LOAD (Y/N)");
	for( I=1; I <= NL; I++ ){
		Print( "\n");
		Print( "LOAD NO. %d:\n", I);
		if( Lc == 'Y' ) goto L1476;
	L1470:;	/*:L1472*/
		aLS[I] = 0;	/* add by Mako */
		input( "PULSE NO.,RESISTANCE,REACTANCE", "%d,%lf,%lf", &aLP[I],&aLA[1][I][1],&aLA[2][I][1] );
		if( aLP[I]>N ){
			Print( "PULSE NUMBER EXCEEDS NUMBER OF PULSES..." );
			goto L1470;
		}
		goto L1491;
		/*REM ----- S-DOMAIN LOADS*/
	L1476:;	/*:L1469,L1478*/
	L1477:;/*:L1479*/
		input( "PULSE NO., ORDER OF S-DOMAIN FUNCTION", "%d,%d", aLP[I],aLS[I] );
		if( aLP[I]>N ){
			Print( "PULSE NUMBER EXCEEDS NUMBER OF PULSES..." );
			goto L1476;
		}
		if( aLS[I]>MA ){
			Print( "MAXIMUM DIMENSION IS 10\n" );
			goto L1477;
		}
		for( J=0; J <= aLS[I]; J++ ){
			Print( "NUMERATOR, DENOMINATOR COEFFICIENTS OF S^%d :", J );
			input("", "%lf,%lf", &aLA[1][I][J],&aLA[2][I][J] );
		}
		if( aLS[I]>0 ) goto L1491;
		aLS[I]=1;
		aLA[1][I][1]=0;
		aLA[2][I][1]=0;
	L1491:;/*:L1474,L1487*/
	}
L1492:;	/*:L1461*/
	FLG=0;
}
#endif

/************************************************************************

************************************************************************/
int InitAntData(ANTDEF *ap)
{
	pAp = ap;
	if( exeenv.CalcLog ) MainWnd->CalMemo->Lines->Clear();
	MW=WZMAX;
	/*REM ----- MAXIMUM NUMBER OF LOADS = 11*/
	ML=LMAX;
	/*REM ----- MAXIMUM ORDER OF S-DOMAIN LOADS = 8*/
	MA=SMAX;
	/*REM ----- MAXIMUM NUMBER OF MEDIA = 6*/
	MM=EMAX;
	/*REM ----- H MUST BE DIMENSIONED AT LEAST 6*/
	/*REM ----- MAXIMUM NUMBER OF PULSES = 50 */
	MP=env.pmax;

	static const double _CQ[]={
		0,
		.288675135,.5,.430568156,.173927423,.169990522,.326072577,
		.480144928,.050614268,.398333239,.111190517,
		.262766205,.156853323,.091717321,.181341892
	};
	int	i;

	P=4*atan(1);
	/*REM ----- CHANGES DEGREES TO RADIANS*/
	P0=P/180;
	/*REM ----- INTRINSIC IMPEDANCE OF FREE SPACE DIVIDED BY 2 PI*/
	G0=29.979221;
	/*REM ---------- Q-VECTOR FOR GAUSSIAN QUADRATURE*/
	for( i = 1; i <= 14; i++ ){
		aQ[i] = _CQ[i];
	}
	/*REM ---------- E-VECTOR FOR COEFFICIENTS OF ELLIPTIC INTEGRAL*/
	C0 = 1.38629436112;
	C1 = 0.09666344259;
	C2 = 0.03590092383;
	C3 = 0.03742563713;
	C4 = 0.01451196212;
	C5 = 0.5;
	C6 = 0.12498593397;
	C7 = 0.06880248576;
	C8 = 0.0332835346;
	C9 = 0.00441787012;

	// 周波数の設定
	if( ap->cfq == 0.0 ) ap->cfq = ap->fq;

	F = ap->cfq;
	if( F==0 ) F=299.8;
	W=299.8/F;
	/* REM -----VIRTUAL DIPOLE LENGTH FOR NEAR FIELD CALCULATION	*/
	S0=0.001*W;
	/* REM ----- 1 / (4 * PI * OMEGA * EPSILON)	*/
	M=4.77783352*W;
	/* REM ----- SET SMALL RADIUS MODIFICATION CONDITION	*/
	SRM=0.0001*W;
	Print( "WAVE LENGTH = %f[m]", W);
	ap->RMD = W;
	/* REM ----- 2 PI / WAVELENGTH	*/
	W=2*P/W;
	W2=W*W/2;
	FLG=0;

	SetWZ(ap);
	// グランド等の設定
	NR=0;
	if( env.type ){		// 完全グラウンド
		G = -1;
		NM = 0;
		TB = 1;
		if( env.type == 2 ){	// リアルグランド
			NM = env.mmax;
			for( i = 0; i < env.mmax; i++ ){
				aT[i+1] = env.rel[i];
				aV[i+1] = env.cond[i];
				aU[i+1] = (env.intval[i] == NULLV) ? 1000000.0 : env.intval[i];
				aH[i+1] = (env.height[i] == NULLV) ? 0 : env.height[i];
			}
			if( env.mtype ){	// グランドスクリーン
				NR = env.gscrcnt;
				RR = env.gscrr;
				TB = 2;
			}
			else {
				NR = 0;
			}
			if( i == env.mmax ){	// 最後の時
				if( aU[i] < 1000000.0 ){
					aU[i] = 1000000.0;
				}
			}
		}
	}
	else {
		G = 1;
		NM = 0;
	}


	// ワイヤーの設定
	NW = ap->wzmax;		// ワイヤの数
	N=0;
	HeightOff = (env.type ? env.antheight : 0.0);
	for( I = 1; I <= NW; I++ ){
		S1 = ap->wzdef[I-1].SEG;
		X1 = ap->wzdef[I-1].X1;
		Y1 = ap->wzdef[I-1].Y1;
		Z1 = ap->wzdef[I-1].Z1 + HeightOff;
		X2 = ap->wzdef[I-1].X2;
		Y2 = ap->wzdef[I-1].Y2;
		Z2 = ap->wzdef[I-1].Z2 + HeightOff;
		aA[I] = ap->wzdef[I-1].R;
		if( (X1==X2) && (Y1==Y2) && (Z1==Z2) ){
			Print("ZERO LENGTH WIRE. (No.%d)", I);
			return FALSE;
		}
		S1299();		// Connection
		X3=X2-X1;
		Y3=Y2-Y1;
		Z3=Z2-Z1;
		D=sqrt(X3*X3+Y3*Y3+Z3*Z3);
		aCA[I]=X3/D;
		aCB[I]=Y3/D;
		aCG[I]=Z3/D;
		aS[I]=D/S1;
		/*REM ----- COMPUTE CONNECTIVITY DATA (PULSES N1 TO N)	*/
		N1=N+1;
		aN[I][1]=N1;
		if( (S1==1) && (I1==0) ) aN[I][1]=0;
		N=N1+S1;
		if( I1==0 ) N=N-1;
		if( I2==0 ) N=N-1;
		if( N > MP ){
			Print("PULSE NUMBER EXCEEDS DIMENSION (MAX=%d)", env.pmax );
			return FALSE;
		}
		aN[I][2]=N;
		if( (S1==1) && (I2==0) ) aN[I][2]=0;
		if( N < N1 ) goto L1247;
		for( J=N1; J <= N; J++ ){
			REFACI(J,1)=I;
			REFACI(J,2)=I;
			aWi[J]=I;
		}
		REFACI(N1,1)=I1;
		REFACI(N,2)=I2;
		/* REM ----- COMPUTE COORDINATES OF BREAK POINTS	*/
		I1=N1+2*(I-1);
		I3=I1;
		aX[I1]=X1;
		aY[I1]=Y1;
		aZ[I1]=Z1;
		if( REFACI(N1,1)==0 ) goto L1230;
		I2=ABS(REFACI(N1,1));
		F3=SGN(REFACI(N1,1))*aS[I2];
		aX[I1]=aX[I1]-F3*aCA[I2];
		aY[I1]=aY[I1]-F3*aCB[I2];
		if( REFACI(N1,1)==-I ) F3=-F3;
		aZ[I1]=aZ[I1]-F3*aCG[I2];
		I3=I3+1;
	L1230:;	/*:L1222	*/
		I6=N+2*I;
		for( I4=I1+1; I4 <= I6; I4++ ){
//			Print("%d,%d,%d,%d,%d", I1, I2, I3, I4, I6);
			J=I4-I3;
			aX[I4]=X1+J*X3/S1;
			aY[I4]=Y1+J*Y3/S1;
			aZ[I4]=Z1+J*Z3/S1;
		}
		if( REFACI(N,2)==0 ) goto L1245;
		I2=ABS(REFACI(N,2));
		F3=SGN(REFACI(N,2))*aS[I2];
		I3=I6-1;
		aX[I6]=aX[I3]+F3*aCA[I2];
		aY[I6]=aY[I3]+F3*aCB[I2];
		if( I==-REFACI(N,2) ) F3=-F3;
		aZ[I6]=aZ[I3]+F3*aCG[I2];
	L1245:;	/*:L1237	*/
		goto L1255;
		/* REM ---- SINGLE SEGMEN 0 PULSE CASE	*/
	L1247:;	/*:L1208	*/
		I1=N1+2*(I-1);
		aX[I1]=X1;
		aY[I1]=Y1;
		aZ[I1]=Z1;
		I1=I1+1;
		aX[I1]=X2;
		aY[I1]=Y2;
		aZ[I1]=Z2;
	L1255:;	/*:L1245	*/
	}
	if( N <= 0 ){
		Print( "NUMBER OF PULSES IS ZERO...." );
		return FALSE;
	}
L1264:;	/*:L1259*/
	K=1;
	J=0;
	for( I=1; I <= N; I++ ){
		I1=2*aWi[I]-1+I;
		if( K>NW ) goto L1279;
		if( K==J ) goto L1279;
	L1270:;	/*:L1278,L1284,L1285	*/
		J=K;
//		Print( "\n" );
//		Print( "WIRE NO. %d", K);
//		Print( "  K I1\tX\tY\tZ\tRADIUS\tEND1\tEND2\tNO.");
		if( (aN[K][1]!=0) || (aN[K][2]!=0) ) goto L1279;
/*		PRINT #3,"-","-","-","    -"," -    -    0"	*/
		K=K+1;
		if( K>NW ) goto L1286;
		goto L1270;
	L1279:;	/*:L1268,L1269,L1274*/
//		Print( "%3d %3d\t%7.3f\t%7.3f\t%7.3f\t%7.3f\t%3d\t%3d\t%2d", K, I1, aX[I1], aY[I1], aZ[I1], aA[aWi[I]], aCi[I][1],aCi[I][2],I);
		i = K-1;
		if( !ap->wzdef[i].PNo ) ap->wzdef[i].PNo = I;
		if( !ap->wzdef[i].SNo ) ap->wzdef[i].SNo = I1;
		ap->wzdef[i].SMax = I1;
		if( (I==aN[K][2]) || (aN[K][1]==aN[K][2]) || (REFACI(I,2)==0) ) K=K+1;
		if( REFACI(I,1)==0 ) REFACI(I,1)=aWi[I];
		if( REFACI(I,2)==0 ) REFACI(I,2)=aWi[I];
		if( (K==NW) && (aN[K][1]==0) && (aN[K][2]==0) ) goto L1270;
		if( (I==N) && (K<NW) ) goto L1270;
	L1286:;	/*:L1277*/
	}
	pmax = N;
	Print( "TOTAL PULSE = %d", N );
L1289:;	/*:L1291*/
	/*REM ----- EXCITATION INPUT*/
L1293:;	/*:L1288*/
	/*REM ----- LOADS/NETWORKS INPUT*/
	FLG=0;

	AdjPlusNo(ap);		// パルス番号の調整

// ロードの設定

	NL = ap->lenb ? ap->lmax : 0;
	for( I=1; I <= NL; I++ ){
		aLS[I] = 0;
		aLP[I] = ap->ldef[I-1].PLUSNo;
		if( aLP[I]>N ){
			Print( "PULSE NUMBER EXCEEDS NUMBER OF PULSES..." );
			return FALSE;
		}
		switch(ap->ldef[I-1].TYPE){
			case 0:		// LC
				if( ap->ldef[I-1].A[0] && ap->ldef[I-1].B[0] ){		// 並列共振回路
					CalcParaZ(aLA[1][I][1], aLA[2][I][1], ap->cfq, ap->ldef[I-1].A[0], ap->ldef[I-1].B[0], ap->ldef[I-1].A[1]);
				}
				else if( ap->ldef[I-1].A[0] ){		// L
					aLA[2][I][1] = ap->cfq*PAI2*ap->ldef[I-1].A[0];					// jwl
					if( ap->ldef[I-1].A[1] ){
						aLA[1][I][1] = aLA[2][I][1]/ap->ldef[I-1].A[1];
					}
					else {	// Q=0
						aLA[1][I][1] = 0;
					}
				}
				else {								// C
					aLA[2][I][1] = -1.0/(ap->cfq*PAI2*ap->ldef[I-1].B[0]*1.0e-6);	// 1/jwc
					if( ap->ldef[I-1].A[1] ){
						aLA[1][I][1] = -(aLA[2][I][1]/ap->ldef[I-1].A[1]);
					}
					else {	// Q=0
						aLA[1][I][1] = 0;
					}
				}
				break;
			case 1:   	// R+jX
				aLA[1][I][1] = ap->ldef[I-1].A[0];
				aLA[2][I][1] = ap->ldef[I-1].B[0];
				break;
			case 2:		// S-DOMAIN
				aLS[I] = ap->ldef[I-1].SN;		// 係数を設定
				for( J = 0; J < aLS[I]; J++ ){
					aLA[1][I][J] = ap->ldef[I-1].A[J];
					aLA[2][I][J] = ap->ldef[I-1].B[J];
				}
				if( aLS[I] <= 0 ){
					aLS[I] = 1;
					aLA[1][I][1]=0;
					aLA[2][I][1]=0;
				}
				break;
		}
	}
	if( env.WireRoss ){
		for( i = 0; i < ap->wmax; i++ ){
			if( ap->wdef[i].R ){
				aLS[I] = 0;
				aLP[I] = ap->wdef[i].PNo + (ap->wdef[i].PMax/2);
				aLA[1][I][1] = GetWireRoss(ap, &ap->wdef[i]);
				aLA[2][I][1] = 0.0;
				I++;
				NL++;
			}
		}
	}

//	給電点の設定

	NS = ap->cmax;
	if( NS<1 ) NS=1;
	if( NS > MP ){
		Print( "NO. OF SOURCES EXCEEDS DIMENSION ..." );
		return FALSE;
	}
	for( I = 1; I <= NS; I++ ){
		aEi[I] = ap->cdef[I-1].PLUSNo;
		VM = ap->cdef[I-1].EV;
		VP = ap->cdef[I-1].DEG;
		if( aEi[I] > N ){
			Print( "PULSE NUMBER EXCEEDS NUMBER OF PULSES..." );
			return FALSE;
		}
		aL[I]=VM*cos(VP*P0);
		aM[I]=VM*sin(VP*P0);
	}

	FLG = 0;
	res.Clear();
	return TRUE;
}

void CalCurrent(void)
{
	S497();		// 電流計算
//	Application->ProcessMessages();

	Print( "FAR FIELD...");
	res.PtnF = 1;
	ZA = 0; ZC = 1; NZ = 91;		// 垂直
	AA = 0; AC = 0; NA = 0;	// 水平
	res.SetVert(90);		// 水平ピークの垂直パターンを得る
	S621();				// パターン計算

	ZA = 270; ZC = 1; NZ = 90;		// 垂直
	AA = 0; AC = 0; NA = 0;	// 水平
	res.SetVert(0);		// 水平ピークの垂直パターンを得る
	S621();				// パターン計算

	int v = res.GetVMAX();		// 垂直ピークを得る(0～90～180)
	v = (v >= 90) ? v - 90 : 90 - v;
	// 打ち上げ各の詳細を調べる
	v++;
	if( v >= 90 ) v = 90;
	ZA = v; ZC = -0.1; NZ = 21;	// 垂直
	AA = 0; AC = 0; NA = 0;			// 水平
	res.SetElv(90 - v);		// 垂直詳細パターンを得る
	S621();						// パターン計算

	double vv = res.GetAdjElv();	// 計算仰角を得る
	vv = (vv >= 90) ? vv - 90 : 90 - vv;
	ZA = double(vv); ZC = 0; NZ = 0;	// 垂直
	AA = 0; AC = 1; NA = 360;		// 水平
	res.SetHori();		// 垂直ピークの水平パターンを得る
	S621();				// パターン計算

	Print("No Fatal Error(s)");
	res.CalcPtn();
}

void CalCurrentDR(void)
{
	S497();		// 電流計算
//	Application->ProcessMessages();

	res.PtnF = 2;
	ZA = 0; ZC = 2; NZ = 91;		// 垂直
	AA = 0; AC = 0; NA = 0;	// 水平
	res.SetVert(45);		// 水平ピークの垂直パターンを得る
	S621();				// パターン計算

	ZA = 270; ZC = 2; NZ = 90;		// 垂直
	AA = 0; AC = 0; NA = 0;	// 水平
	res.SetVert(0);		// 水平ピークの垂直パターンを得る
	S621();				// パターン計算

	int v = res.GetVMAX();			// 垂直ピークを得る
	v = (v >= 90) ? v - 90 : 90 - v;
	// 打ち上げ各の詳細を調べる
	v++;
	if( v >= 90 ) v = 90;
	ZA = v; ZC = -0.1; NZ = 21;	// 垂直
	AA = 0; AC = 0; NA = 0;		// 水平
	res.SetElv(90 - v);			// 垂直詳細パターンを得る
	S621();						// パターン計算

	double vv = res.GetAdjElv();	// 計算仰角を得る
	vv = (vv >= 90) ? vv - 90 : 90 - vv;
	ZA = double(vv); ZC = 0; NZ = 0;	// 垂直
	AA = 0; AC = 2; NA = 360;		// 水平
	res.SetHori();		// 垂直ピークの水平パターンを得る
	S621();				// パターン計算

	res.CalcPtn();
}

int GetPlusMax(void)
{
	return N;
}

void GetSegPos(double &x, double &y, double &z, int Seg)
{
	x = aX[Seg];
	y = aY[Seg];
	z = aZ[Seg] - HeightOff;
}

void GetSegPosReal(double &x, double &y, double &z, int Seg)
{
	x = aX[Seg];
	y = aY[Seg];
	z = aZ[Seg];
}

void SaveCurrent(void)
{
	S497();		// 電流計算
}

void SaveNear(void)
{
	S875();
}

void SaveFar(void)
{
	ZA = exeenv.pFarFile->Z;
	ZC = exeenv.pFarFile->ZW;
	NZ = exeenv.pFarFile->ZN;
	if( !ZC ) NZ = 1;
	AA = exeenv.pFarFile->A;
	AC = exeenv.pFarFile->AW;
	NA = exeenv.pFarFile->AN;
	if( !AC ) NA = 1;
	S621();				// パターン計算
}

void CalcNewPtn(void)
{
	double vv = res.ElvHPtn;
	vv = (vv >= 90) ? vv - 90 : 90 - vv;
	ZA = double(vv); ZC = 0; NZ = 0;	// 垂直
	AA = 0; AC = res.PtnF; NA = 360;	// 水平
	res.SetHori();		// 垂直ピークの水平パターンを得る
	S621();				// パターン計算
	res.ReCalcGain();
}

void ClearNEC(void)
{
	aX = NULL;
	aY = NULL;
	aZ = NULL;

	aCi = NULL;
	aCI = NULL;
	aCR = NULL;
	aPi = NULL;
	aWi = NULL;
	aZR = NULL;
	aZI = NULL;

	aEi = NULL;
	aEf = NULL;
	aL = NULL;
	aM = NULL;
}

void DeleteNEC(void)
{
	delete aX;
	delete aY;
	delete aZ;

	delete aCi;
	delete aCI;
	delete aCR;
	delete aPi;
	delete aWi;
	delete aZR;
	delete aZI;

	delete aEi;
	delete aEf;
	delete aL;
	delete aM;
	ClearNEC();
}

int InitNEC(int max)
{
	ClearNEC();

	while(max > 0){
		int amax = max + 1;
		try {
			aX = new double[(WZMAX*2)+amax];
			aY = new double[(WZMAX*2)+amax];
			aZ = new double[(WZMAX*2)+amax];

			aCi = new int[amax*3];
			aCI = new double[amax];
			aCR = new double[amax];
			aPi = new int[amax];
			aWi = new int[amax];
			aZR = new float[amax*amax];
			aZI = new float[amax*amax];

			aEi = new int[WZMAX+amax];
			aEf = new double[WZMAX+amax];
			aL = new double[WZMAX+amax];
			aM = new double[WZMAX+amax];
			break;
		}
		catch(xalloc){ // xalloc が送出された場合にのみこのブロックに入る
			max -= 256;
			DeleteNEC();
		}
	}
	if( max < 0 ) max = 0;
	return max;
}

