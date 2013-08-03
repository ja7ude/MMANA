# ---------------------------------------------------------------------------
VERSION = BCB.01
# ---------------------------------------------------------------------------
!ifndef BCB
BCB = $(MAKEDIR)\..
!endif
# ---------------------------------------------------------------------------
PROJECT = Mmana.exe
OBJFILES = Mmana.obj Main.obj mininec3.obj ComLib.obj TextEdit.obj VerDsp.obj \
   MediaDlg.obj ResCmp.obj GrpWire.obj ACalBox.obj ACalRes.obj OptDlg.obj \
   ACalMult.obj WireScl.obj BwDisp.obj ValRep.obj MoveDlg.obj NearSet.obj \
   FarSet.obj RotWire.obj WireCad.obj WireEdit.obj ACalInfo.obj StackDlg.obj \
   WComb.obj WcombDsp.obj ACalEle.obj FreqSet.obj Print.obj
RESFILES = mmana.res Mmanam.res
RESDEPEN = $(RESFILES) Main.dfm TextEdit.dfm VerDsp.dfm MediaDlg.dfm \
   ResCmp.dfm GrpWire.dfm ACalBox.dfm ACalRes.dfm OptDlg.dfm ACalMult.dfm \
   WireScl.dfm BwDisp.dfm ValRep.dfm MoveDlg.dfm NearSet.dfm FarSet.dfm \
   RotWire.dfm WireCad.dfm WireEdit.dfm ACalInfo.dfm StackDlg.dfm WComb.dfm \
   WcombDsp.dfm ACalEle.dfm FreqSet.dfm Print.dfm
LIBFILES = 
DEFFILE = 
# ---------------------------------------------------------------------------
CFLAG1 = -O2 -Hc -w -d -k- -r -vi -c -a4 -b- -w-par -w-inl -Vx -Ve -x
CFLAG2 = -I$(BCB)\include;$(BCB)\include\vcl -H=$(BCB)\lib\vcl.csm 
PFLAGS = -AWinTypes=Windows;WinProcs=Windows;DbiTypes=BDE;DbiProcs=BDE;DbiErrs=BDE \
   -U$(BCB)\lib\obj;$(BCB)\lib -I$(BCB)\include;$(BCB)\include\vcl -$L- -$D- \
   -JPHNV -M     
RFLAGS = -i$(BCB)\include;$(BCB)\include\vcl 
LFLAGS = -L$(BCB)\lib\obj;$(BCB)\lib -S:0x200000 -aa -Tpe -x -V4.0 
IFLAGS = 
LINKER = ilink32
# ---------------------------------------------------------------------------
ALLOBJ = c0w32.obj $(OBJFILES)
ALLRES = $(RESFILES)
ALLLIB = $(LIBFILES) vcl.lib import32.lib cp32mt.lib 
# ---------------------------------------------------------------------------
.autodepend

$(PROJECT): $(OBJFILES) $(RESDEPEN) $(DEFFILE)
    $(BCB)\BIN\$(LINKER) @&&!
    $(LFLAGS) +
    $(ALLOBJ), +
    $(PROJECT),, +
    $(ALLLIB), +
    $(DEFFILE), +
    $(ALLRES) 
!

.pas.hpp:
    $(BCB)\BIN\dcc32 $(PFLAGS) { $** }

.pas.obj:
    $(BCB)\BIN\dcc32 $(PFLAGS) { $** }

.cpp.obj:
    $(BCB)\BIN\bcc32 $(CFLAG1) $(CFLAG2) -o$* $* 

.c.obj:
    $(BCB)\BIN\bcc32 $(CFLAG1) $(CFLAG2) -o$* $**

.rc.res:
    $(BCB)\BIN\brcc32 $(RFLAGS) $<
#-----------------------------------------------------------------------------
