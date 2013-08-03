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

//---------------------------------------------------------------------------
USEFORM("Main.cpp", MainWnd);
USEUNIT("mininec3.cpp");
USERES("mmana.res");
USEUNIT("ComLib.cpp");
USEFORM("TextEdit.cpp", TextEditDlg);
USEFORM("VerDsp.cpp", VerDspDlg);
USEFORM("MediaDlg.cpp", MediaDlgBox);
USEFORM("ResCmp.cpp", ResCmpDlg);
USEFORM("GrpWire.cpp", GrpWireDlg);
USEFORM("ACalBox.cpp", ACalDlg);
USEFORM("ACalRes.cpp", ACalResDlg);
USEFORM("OptDlg.cpp", OptDlgBox);
USEFORM("ACalMult.cpp", ACalMultDlg);
USEFORM("WireScl.cpp", WireScaleDlg);
USEFORM("BwDisp.cpp", BwDispDlg);
USEFORM("ValRep.cpp", ValRepDlg);
USEFORM("MoveDlg.cpp", MoveDlgBox);
USEFORM("NearSet.cpp", NearSetDlg);
USEFORM("FarSet.cpp", FarSetDlg);
USEFORM("RotWire.cpp", RotWireDlg);
USEFORM("WireCad.cpp", WireCadDlg);
USEFORM("WireEdit.cpp", WireEditDlg);
USEFORM("ACalInfo.cpp", ACalInfoBox);
USERC("Mmanam.rc");
USEFORM("StackDlg.cpp", StackDlgBox);
USEFORM("WComb.cpp", WCombDlg);
USEFORM("WcombDsp.cpp", WCombDspDlg);
USEFORM("ACalEle.cpp", ACalEleBox);
USEFORM("FreqSet.cpp", FreqSetDlg);
USEFORM("Print.cpp", PrintDlgBox);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		Application->Initialize();
		Application->Title = "MMANA";
		Application->CreateForm(__classid(TMainWnd), &MainWnd);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	return 0;
}
//---------------------------------------------------------------------------
