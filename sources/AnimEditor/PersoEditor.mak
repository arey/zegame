# ---------------------------------------------------------------------------
VERSION = BCB.03
# ---------------------------------------------------------------------------
!ifndef BCB
BCB = $(MAKEDIR)\..
!endif
# ---------------------------------------------------------------------------
PROJECT = PersoEditor.exe
OBJFILES = PersoEditor.obj DDLibEditor.obj fPrincipale.obj fMenuBank.obj \
  fMenuBankGrille.obj fMenuBankGrav.obj consts.obj IDMap.obj fMenuIDName.obj \
  fMenuIDPath.obj fAnimView.obj fBarOutils.obj fWorkStation.obj fMenuScript.obj \
  Animall.obj PersoLib.obj DIlib.obj ImDecors2.obj ..\..\DDraw\Include\ddutil.obj \
  DDlib.obj D3DLib.obj GestFile.obj WorldSimulation.obj MapLib.obj PNJLib.obj \
  uAnimsInfos.obj GameDebug.obj SpritesLib.obj
RESFILES = PersoEditor.res
RESDEPEN = $(RESFILES) fPrincipale.dfm fMenuBank.dfm fMenuBankGrille.dfm fMenuBankGrav.dfm \
  fMenuIDName.dfm fMenuIDPath.dfm fAnimView.dfm fBarOutils.dfm fWorkStation.dfm \
  fMenuScript.dfm uAnimsInfos.dfm
LIBFILES = "..\..\..\..\PROG\DirectX 6  SDK\lib\Borland\ddraw.lib" \
  "..\..\..\..\PROG\DirectX 6  SDK\lib\Borland\d3dim.lib" "..\..\..\..\PROG\DirectX 6  SDK\lib\Borland\d3dframe.lib"
LIBRARIES = vcldbx35.lib vcldb35.lib vclx35.lib vcl35.lib
SPARELIBS = vcl35.lib vclx35.lib vcldb35.lib vcldbx35.lib
PACKAGES = VCLX35.bpi VCL35.bpi VCLDB35.bpi VCLDBX35.bpi NMFAST35.bpi INETDB35.bpi \
  INET35.bpi QRPT35.bpi TEEUI35.bpi TEEDB35.bpi TEE35.bpi
PATHASM = .;
PATHCPP = .;..\..\DDraw\Include
PATHPAS = .;
PATHRC = .;
DEBUGLIBPATH = $(BCB)\lib\debug
RELEASELIBPATH = $(BCB)\lib\release
DEFFILE =
# ---------------------------------------------------------------------------
CFLAG1 = -Od -Hc -w- -Ve -r- -k -y -v -vi- -c -b- -w-par -w-inl -Vx
CFLAG2 = -DCBUILDER \
  -I..\..\..\..\utilitaires\borland\cbuilder3\bin;..\..\ddraw\include;$(BCB)\images\buttons;$(BCB)\bin;$(BCB)\include;$(BCB)\include\vcl;F:\Prog\dxsdk\sdk\Lib;F:\CRPG\Prog\DDraw\Include \
  -H=$(BCB)\lib\vcld.csm
CFLAG3 = -ff -5
PFLAGS = -AWinTypes=Windows;WinProcs=Windows;DbiTypes=BDE;DbiProcs=BDE;DbiErrs=BDE \
  -DCBUILDER \
  -U..\..\..\..\utilitaires\borland\cbuilder3\bin;..\..\ddraw\include;$(BCB)\images\buttons;F:\crpg\prog\ddraw\include;$(BCB)\bin;$(BCB)\lib\obj;$(BCB)\lib;"C:\Prog\SDK\DirectX 6.1 SDK\Lib\Borland";$(RELEASELIBPATH) \
  -I..\..\..\..\utilitaires\borland\cbuilder3\bin;..\..\ddraw\include;$(BCB)\images\buttons;$(BCB)\bin;$(BCB)\include;$(BCB)\include\vcl;F:\Prog\dxsdk\sdk\Lib;F:\CRPG\Prog\DDraw\Include \
  -$Y -$W -$O- -v -JPHNV -M
RFLAGS = -DCBUILDER \
  -i..\..\..\..\utilitaires\borland\cbuilder3\bin;..\..\ddraw\include;$(BCB)\images\buttons;$(BCB)\bin;$(BCB)\include;$(BCB)\include\vcl;F:\Prog\dxsdk\sdk\Lib;F:\CRPG\Prog\DDraw\Include
AFLAGS = /i..\..\..\..\utilitaires\borland\cbuilder3\bin /i..\..\ddraw\include \
  /i$(BCB)\images\buttons /i$(BCB)\bin /i$(BCB)\include /i$(BCB)\include\vcl \
  /iF:\Prog\dxsdk\sdk\Lib /iF:\CRPG\Prog\DDraw\Include /dCBUILDER /mx /kh32768 \
  /w2 /zi
LFLAGS = -L..\..\..\..\utilitaires\borland\cbuilder3\bin;..\..\ddraw\include;$(BCB)\images\buttons;F:\crpg\prog\ddraw\include;$(BCB)\bin;$(BCB)\lib\obj;$(BCB)\lib;"C:\Prog\SDK\DirectX 6.1 SDK\Lib\Borland";$(RELEASELIBPATH) \
  -aa -Tpe -x -v
IFLAGS =
LINKER = ilink32
# ---------------------------------------------------------------------------
ALLOBJ = c0w32.obj sysinit.obj $(OBJFILES)
ALLRES = $(RESFILES)
ALLLIB = $(LIBFILES) $(LIBRARIES) import32.lib cp32mt.lib
# ---------------------------------------------------------------------------
.autodepend

!ifdef IDEOPTIONS

[Version Info]
IncludeVerInfo=1
AutoIncBuild=0
MajorVer=0
MinorVer=3
Release=0
Build=1
Debug=0
PreRelease=0
Special=0
Private=0
DLL=0
Locale=1036
CodePage=1252

[Version Info Keys]
CompanyName=
FileDescription=
FileVersion=0.3.0.1
InternalName=
LegalCopyright=
LegalTrademarks=
OriginalFilename=
ProductName=
ProductVersion=1.0.0.0
Comments=

[HistoryLists\hlIncludePath]
Count=2
Item0=..\..\..\..\utilitaires\borland\cbuilder3\bin;..\..\ddraw\include;$(BCB)\images\buttons;$(BCB)\bin;$(BCB)\include;$(BCB)\include\vcl;F:\Prog\dxsdk\sdk\Lib;F:\CRPG\Prog\DDraw\Include
Item1=..\..\ddraw\include;$(BCB)\images\buttons;$(BCB)\bin;$(BCB)\include;$(BCB)\include\vcl;C:\Prog\dxsdk\sdk\Lib;C:\CRPG\Prog\DDraw\Include;

[HistoryLists\hlLibraryPath]
Count=3
Item0=..\..\..\..\utilitaires\borland\cbuilder3\bin;..\..\ddraw\include;$(BCB)\images\buttons;F:\crpg\prog\ddraw\include;$(BCB)\bin;$(BCB)\lib\obj;$(BCB)\lib;C:\Prog\SDK\DirectX 6.1 SDK\Lib\Borland
Item1=..\..\..\..\utilitaires\borland\cbuilder3\bin;..\..\ddraw\include;$(BCB)\images\buttons;F:\crpg\prog\ddraw\include;$(BCB)\bin;$(BCB)\lib\obj;$(BCB)\lib;F:\Prog\dxsdk\sdk\Lib
Item2=..\..\ddraw\include;$(BCB)\images\buttons;c:\crpg\prog\ddraw\include;$(BCB)\bin;$(BCB)\lib\obj;$(BCB)\lib;C:\Prog\dxsdk\sdk\Lib

[HistoryLists\hlDebugSourcePath]
Count=2
Item0=F:\Utilitaires\Borland\Borland Partage\Debugger
Item1=C:\Utilitaires\Borland\Borland Partage\Debugger

[HistoryLists\hlConditionals]
Count=1
Item0=CBUILDER

[HistoryLists\hlUnitAliases]
Count=1
Item0=WinTypes=Windows;WinProcs=Windows;DbiTypes=BDE;DbiProcs=BDE;DbiErrs=BDE

[Debugging]
DebugSourceDirs=F:\Utilitaires\Borland\Borland Partage\Debugger

[Parameters]
RunParams=
HostApplication=

!endif

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
