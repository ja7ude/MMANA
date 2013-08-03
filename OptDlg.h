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

//----------------------------------------------------------------------------
#ifndef OptDlgH
#define OptDlgH
//----------------------------------------------------------------------------
#include <System.hpp>
#include <Windows.hpp>
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Graphics.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Controls.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
//----------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Graphics.hpp>
#include <Vcl.StdCtrls.hpp>

#include "ComLib.h"

class TOptDlgBox : public TForm
{
__published:        
	TButton *OKBtn;
	TButton *CancelBtn;
	TPageControl *Page;
	TTabSheet *TabTank;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label4;
	TComboBox *TFreq;
	TEdit *TEditX;
	TEdit *TEditL;
	TEdit *TEditC;
	TLabel *Label5;
	TLabel *Label6;
	TLabel *Label7;
	TLabel *Label8;
	TLabel *TStat;
	TCheckBox *TFixFreq;
	TTabSheet *TabCoil;
	TEdit *AEditL;
	TLabel *Label9;
	TLabel *Label10;
	TLabel *Label11;
	TLabel *Label12;
	TLabel *Label13;
	TEdit *AEditN;
	TEdit *AEditD;
	TEdit *AEditW;
	TEdit *AEditS;
	TLabel *Label14;
	TLabel *Label15;
	TLabel *Label16;
	TLabel *Label17;
	TLabel *Label18;
	TRadioGroup *ACalSel;
	TLabel *Label20;
	TLabel *ACoilLen;
	TLabel *Label22;
	TTabSheet *TabMatch;
	TLabel *Label19;
	TEdit *MEditR;
	TLabel *Label21;
	TLabel *Label23;
	TEdit *MEditX;
	TLabel *Label24;
	TLabel *Label25;
	TEdit *MEditZ;
	TLabel *Label26;
	TComboBox *MFreq;
	TLabel *Label27;
	TLabel *Label28;
	TPaintBox *PBoxM;
	TTabSheet *TabCore;
	TLabel *Label29;
	TLabel *Label30;
	TLabel *Label31;
	TComboBox *CType;
	TEdit *CEditX;
	TEdit *CEditL;
	TLabel *Label32;
	TLabel *Label33;
	TTabSheet *TabEnv;
	TGroupBox *GroupBox1;
	TLabel *Label34;
	TLabel *Label35;
	TEdit *EEditFBR;
	TLabel *Label36;
	TEdit *EEditFBRV;
	TLabel *Label37;
	TRadioGroup *MXSel;
	TLabel *Label38;
	TLabel *Label39;
	TLabel *Label40;
	TComboBox *CFreq;
	TLabel *Label41;
	TRadioGroup *CStub;
	TLabel *CLCType;
	TEdit *CEditLC;
	TLabel *CLCUnit;
	TLabel *Label42;
	TLabel *Label43;
	TEdit *CEditP;
	TLabel *Label44;
	TEdit *CEditZ;
	TLabel *CFo;
	TTabSheet *TabQM;
	TGroupBox *GroupBox2;
	TEdit *QEdRi1;
	TLabel *Label45;
	TLabel *Label46;
	TEdit *QEdXi1;
	TLabel *Label47;
	TLabel *Label48;
	TEdit *QEdRL1;
	TEdit *QEdXL1;
	TGroupBox *GroupBox3;
	TLabel *Label51;
	TLabel *Label52;
	TEdit *QEdRL2;
	TEdit *QEdXL2;
	TLabel *Label53;
	TEdit *QEdRi2;
	TGroupBox *GroupBox4;
	TLabel *Label55;
	TLabel *Label56;
	TEdit *QEdZo1;
	TEdit *QEdL1;
	TGroupBox *GroupBox5;
	TLabel *Label57;
	TLabel *Label58;
	TEdit *QEdZo2;
	TEdit *QEdL2;
	TGroupBox *GroupBox6;
	TLabel *Label49;
	TEdit *QEdZo;
	TLabel *Label50;
	TEdit *QEdL;
	TLabel *Label54;
	TLabel *Label59;
	TLabel *Label60;
	TLabel *Label61;
	TLabel *Label62;
	TLabel *Label63;
	TLabel *Label64;
	TLabel *Label65;
	TLabel *Label66;
	TLabel *Label67;
	TLabel *Label68;
	TLabel *Label71;
	TLabel *Label72;
	TLabel *QLSWR2;
	TButton *QMBtn;
	TLabel *QLSWR;
	TLabel *Label69;
	TEdit *QEdA;
	TLabel *Label70;
	TCheckBox *QMSel;
	TLabel *Label73;
	TLabel *Label74;
	TLabel *QLSWRL;
	TTabSheet *TabStub;
	TGroupBox *GroupBox7;
	TLabel *Label75;
	TLabel *Label76;
	TEdit *SMEditR;
	TEdit *SMEditX;
	TLabel *Label77;
	TLabel *Label78;
	TGroupBox *GroupBox8;
	TLabel *Label79;
	TEdit *SMEditZo;
	TLabel *Label80;
	TGroupBox *GroupBox9;
	TGroupBox *GroupBox10;
	TLabel *SMStat2;
	TLabel *SMStat1;
	TButton *SMCalcBtn;
	TImage *Image1;
	TLabel *Label81;
	TEdit *SMEditCm;
	TComboBox *SMEditFreq;
	TEdit *SMEditZi;
	TLabel *Label82;
	TLabel *Label83;
	TLabel *Label84;
	TLabel *Label85;
	TComboBox *EEditRO;
	TGroupBox *GroupBox11;
	TCheckBox *CBCurDir;
	TGroupBox *GroupBox12;
	TRadioGroup *RGFixAngle;
	TLabel *Label86;
	TEdit *EditJXO;
	TLabel *Label87;
	TGroupBox *GroupBox13;
	TComboBox *EditPls;
	TGroupBox *GroupBox14;
	TComboBox *EditRecentMax;
	TLabel *Label88;
	TCheckBox *RecentMAA;
	TButton *AutojX;
	TGroupBox *GroupBox15;
	TComboBox *EditFont;
	void __fastcall TEditXChange(TObject *Sender);
	void __fastcall TEditLChange(TObject *Sender);
	void __fastcall TEditCChange(TObject *Sender);
	void __fastcall TFreqChange(TObject *Sender);

	void __fastcall AEditLChange(TObject *Sender);
	void __fastcall AEditNChange(TObject *Sender);
	void __fastcall AEditDChange(TObject *Sender);
	void __fastcall AEditWChange(TObject *Sender);
	void __fastcall AEditSChange(TObject *Sender);
	void __fastcall PageChange(TObject *Sender);
	void __fastcall PBoxMPaint(TObject *Sender);
	void __fastcall MFreqChange(TObject *Sender);
	void __fastcall MEditRChange(TObject *Sender);
	void __fastcall MEditXChange(TObject *Sender);
	void __fastcall MEditZChange(TObject *Sender);
	
	void __fastcall CTypeChange(TObject *Sender);
	void __fastcall CEditXChange(TObject *Sender);
	void __fastcall CEditLChange(TObject *Sender);
	void __fastcall MXSelClick(TObject *Sender);
	void __fastcall CEditLCChange(TObject *Sender);
	void __fastcall CStubClick(TObject *Sender);
	void __fastcall CFreqChange(TObject *Sender);
	void __fastcall CEditPChange(TObject *Sender);
	void __fastcall CEditZChange(TObject *Sender);
	void __fastcall QEdRi1Change(TObject *Sender);
	void __fastcall QEdXi1Change(TObject *Sender);
	void __fastcall QEdRL1Change(TObject *Sender);
	void __fastcall QEdXL1Change(TObject *Sender);
	void __fastcall QEdZoChange(TObject *Sender);
	void __fastcall QEdLChange(TObject *Sender);
	void __fastcall QEdRi2Change(TObject *Sender);
	
	void __fastcall QEdRL2Change(TObject *Sender);
	void __fastcall QEdXL2Change(TObject *Sender);
	void __fastcall QEdZo1Change(TObject *Sender);
	void __fastcall QEdL1Change(TObject *Sender);
	void __fastcall QEdZo2Change(TObject *Sender);
	void __fastcall QEdL2Change(TObject *Sender);
	void __fastcall QMBtnClick(TObject *Sender);
	void __fastcall QEdAChange(TObject *Sender);
	void __fastcall SMEditRChange(TObject *Sender);
	void __fastcall SMEditXChange(TObject *Sender);
	void __fastcall SMEditZoChange(TObject *Sender);
	
	void __fastcall SMCalcBtnClick(TObject *Sender);
	
	void __fastcall SMEditCmChange(TObject *Sender);
	void __fastcall SMEditZiChange(TObject *Sender);
	void __fastcall SMEditFreqChange(TObject *Sender);
	
	void __fastcall AutojXClick(TObject *Sender);
private:
	int		EnbCalc;
	double	Freq;
    double	X;
    double	L;
    double	C;

	double	CoilL;
    double	CoilW;		// ワイヤの直径
    double	CoilN;		// 巻数
    double	CoilS;		// スペース
    double	CoilD;		// コイルの直径

	double	MF;
    double	MZ;
    double	MR;
    double	MX;

	int		MT;
    char	MM1[2];
    char	MM2[2];
	double	MX1;
    double	MX2;
	double	MD1;
    double	MD2;

	double	CoreFreq;
    int		CoreType;
    double	CoreP;
    double	CoreZ;
	double	CoreX;
    double	CoreL;
    double	CoreLC;
    double	CoreFo;

    double	QMRi1;
    double	QMXi1;
    double	QMRL1;
    double	QMXL1;
    double	QMZo;
    double	QML;
    double	QMA;

    double	QMRi2;
    double	QMRL2;
    double	QMXL2;
    double	QMZo1;
    double	QML1;
    double	QMZo2;
    double	QML2;
	double	QMSWR;
	double	QMSWRL;
	double	QMSWR2;

    int		SMF;
    double	SMR;
    double	SMX;
    double	SMZo;
    double	SMCm;
    double	SMFreq;
    double	SMZi;

    double	SML1;
    double	SMR1;
    double	SMX1;
    double	SMLS1;
    double	SMLO1;
    double	SMXC1;
    double	SMSWR1;
    double	SMRi1;
    double	SMXi1;
    double	SML2;
    double	SMR2;
    double	SMX2;
    double	SMLS2;
    double	SMLO2;
    double	SMXC2;
    double	SMSWR2;
    double	SMRi2;
    double	SMXi2;


	void __fastcall SetValueItem(int n);
	void __fastcall CalcTank(int sw);
	void __fastcall CalcCoil(int sw);
	void __fastcall CalcMatch(void);
	void __fastcall CalcCore(int n);

	void __fastcall CalcZi(double &Ri, double &Xi, double Zo, double L, double RL, double XL);
	void __fastcall CalcZL(double &RL, double &XL, double Zo, double L, double Ri, double Xi);
	void __fastcall CalcZi(double &Ri, double &Xi, double Zo, double L, double RL, double XL, double a);
	void __fastcall CalcZL(double &RL, double &XL, double Zo, double L, double Ri, double Xi, double a);

	void __fastcall CalcQML(void);
	void __fastcall CalcQMSWR(void);
	void __fastcall CopyQM(void);

	double __fastcall CalcSMStub(double L, int sw);
	void __fastcall CalcSM(void);

public:
	virtual __fastcall TOptDlgBox(TComponent* AOwner);
	int __fastcall Execute(double freq, double r, double x, double l, double c);
};
//----------------------------------------------------------------------------
//extern TFreqDlgBox *FreqDlgBox;
//----------------------------------------------------------------------------
#endif    
