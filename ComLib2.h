//---------------------------------------------------------------------------
#ifndef ComLib2H
#define ComLib2H
//---------------------------------------------------------------------------
//#include <vcl\comctrls.hpp>
//---------------------------------------------------------------------------
class CAidSpin
{
private:
	double		Val;
	double		DW;
	double		Min;
    double		Max;

	TUpDown		*pSpin;
    TControl	*pCtrl;
public:
	CAidSpin(){
		pSpin = NULL;
        pCtrl = NULL;
        Val = 0;
        DW = 0.1;
        Min = 0;
        Max = 9999;
    };
    double GetValue(void){
		return Val;
    };
	void SetControl(TUpDown *pS, TControl *pC, double val, double min, double max, double dw);
	void Init(void);
	void OnClick(TUDBtnType Button);
	void OnChanging(void);
}
#endif
 