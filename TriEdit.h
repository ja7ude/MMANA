//----------------------------------------------------------------------------
#ifndef TriEditH
#define TriEditH
//----------------------------------------------------------------------------
#include <vcl\System.hpp>
#include <vcl\Windows.hpp>
#include <vcl\SysUtils.hpp>
#include <vcl\Classes.hpp>
#include <vcl\Graphics.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Controls.hpp>
#include <vcl\Buttons.hpp>
#include <vcl\ExtCtrls.hpp>

#include "ComLib.h"
//----------------------------------------------------------------------------
class TTriEditDlg : public TForm
{
__published:
	TButton *OKBtn;
	TButton *CancelBtn;
	TBevel *Bevel1;
	TRadioGroup *BasicSel;
	TLabel *Label1;
	TLabel *Label2;
	TEdit *EditLen;
	TEdit *EditXY;
	TLabel *Label3;
	TLabel *Label4;
	TLabel *Label5;
	TEdit *EditYZ;
	TLabel *Label6;
	TGroupBox *GroupBox1;
	TLabel *NowBgn;
	TLabel *NowEnd;
	TGroupBox *GroupBox2;
	TLabel *NewBgn;
	TLabel *NewEnd;
	void __fastcall EditLenChange(TObject *Sender);
	void __fastcall EditXYChange(TObject *Sender);
	void __fastcall EditYZChange(TObject *Sender);
	void __fastcall BasicSelClick(TObject *Sender);
private:
	int		Index;
	ANTDEF	*ap;
    WDEF	NowW;
    WDEF	NewW;

    double	WL;
    double	XYD;
    double	YZD;

    int		EnbCalc;
private:
	void __fastcall DispItem(int n);
	void __fastcall CalcNow(void);
	void __fastcall CalcNew(void);
	int __fastcall CheckSamePos(int n, double x, double y, double z);
	void __fastcall NewSamePos(double bx, double by, double bz, double x, double y, double z);

public:
	virtual __fastcall TTriEditDlg(TComponent* AOwner);

	int __fastcall Execute(ANTDEF *p, int n);
};
//----------------------------------------------------------------------------
//extern TTriEditDlg *TriEditDlg;
//----------------------------------------------------------------------------
#endif    
