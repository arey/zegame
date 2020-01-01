# ---------------------------------------------------------------------------
VERSION = BCB.03
# ---------------------------------------------------------------------------
!ifndef BCB
BCB = $(MAKEDIR)\..
!endif
# ---------------------------------------------------------------------------
PROJECT = MapEditor.exe
OBJFILES = MapEditor.obj Editor.obj consts.obj DDlib.obj DDLibEditor.obj fDesignOutils.obj \
  fPersosOutils.obj uMapView.obj MapLib.obj fNewMap.obj fMapOutils.obj \
  fEditLibDecors.obj ImDecors2.obj fLibOutils.obj fObjOutils.obj fPrefer.obj \
  fEditLibLevel.obj fPathFinding.obj FPath.obj Animall.obj \
  ..\..\DDraw\Include\ddutil.obj IDMap.obj GestFile.obj ScnConsts.obj \
  ScnActions.obj ScnEvents.obj GameObject.obj ScnCauses.obj ScnScript.obj \
  GameObjectList.obj fIDOutils.obj INILib.obj MapZone.obj FileVirtual.obj \
  FileManager.obj PakFile.obj WorldSimulation.obj PersoLib.obj TimeClock.obj \
  D3DLib.obj PNJLib.obj MemLib.obj SpritesLib.obj ImageFile.obj ImageDDFile.obj \
  ImageUtil.obj DIlib.obj GameDebug.obj
RESFILES = MapEditor.res
RESDEPEN = $(RESFILES) Editor.dfm fDesignOutils.dfm fPersosOutils.dfm uMapView.dfm \
  fNewMap.dfm fMapOutils.dfm fEditLibDecors.dfm fLibOutils.dfm fObjOutils.dfm \
  fPrefer.dfm fEditLibLevel.dfm fPathFinding.dfm fIDOutils.dfm
LIBFILES = d3dim.lib ddraw.lib d3dframe.lib dxguid.lib
LIBRARIES = vcldbx35.lib vcldb35.lib vclx35.lib vcl35.lib
SPARELIBS = vcl35.lib vclx35.lib vcldb35.lib vcldbx35.lib
PACKAGES = VCLX35.bpi VCL35.bpi
PATHASM = .;
PATHCPP = .;..\..\DDraw\Include
PATHPAS = .;
PATHRC = .;
DEBUGLIBPATH = $(BCB)\lib\debug
RELEASELIBPATH = $(BCB)\lib\release
DEFFILE =
# ---------------------------------------------------------------------------
CFLAG1 = -Od -Hc -w -Ve -r- -k -y -v -vi- -c -b- -w-par -w-inl -Vx
CFLAG2 = -DCBUILDER;MAPEDITOR;_RTLDLL \
  -I"..\..\..\..\logiciels\borland\borland partage\images\buttons";..\..\..\..\logiciels\borland\cbuilder3\bin;..\..\ddraw\include;..\..\..\..\utilitaires\borland\cbuilder3\bin;$(BCB)\bin;$(BCB)\include;$(BCB)\include\vcl;C:\Prog\dxsdk\sdk\Lib;C:\CRPG\Prog\DDraw\Include \
  -H=$(BCB)\lib\vcl.csm
CFLAG3 =
PFLAGS = -AWinTypes=Windows;WinProcs=Windows;DbiTypes=BDE;DbiProcs=BDE;DbiErrs=BDE \
  -DCBUILDER;MAPEDITOR;_RTLDLL \
  -U..\..\ddraw\include;"C:\Prog\SDK\DirectX 6.1 SDK\lib\Borland\";"..\..\..\..\logiciels\borland\borland partage\images\buttons";..\..\..\..\logiciels\borland\cbuilder3\bin;..\..\..\..\utilitaires\borland\cbuilder3\bin;$(BCB)\bin;$(BCB)\lib\obj;$(BCB)\lib;$(DEBUGLIBPATH) \
  -I"..\..\..\..\logiciels\borland\borland partage\images\buttons";..\..\..\..\logiciels\borland\cbuilder3\bin;..\..\ddraw\include;..\..\..\..\utilitaires\borland\cbuilder3\bin;$(BCB)\bin;$(BCB)\include;$(BCB)\include\vcl;C:\Prog\dxsdk\sdk\Lib;C:\CRPG\Prog\DDraw\Include \
  -$Y -$W -$O- -JPHNV -M
RFLAGS = -DCBUILDER;MAPEDITOR;_RTLDLL \
  -i"..\..\..\..\logiciels\borland\borland partage\images\buttons";..\..\..\..\logiciels\borland\cbuilder3\bin;..\..\ddraw\include;..\..\..\..\utilitaires\borland\cbuilder3\bin;$(BCB)\bin;$(BCB)\include;$(BCB)\include\vcl;C:\Prog\dxsdk\sdk\Lib;C:\CRPG\Prog\DDraw\Include
AFLAGS = /i"..\..\..\..\logiciels\borland\borland partage\images\buttons" \
  /i..\..\..\..\logiciels\borland\cbuilder3\bin /i..\..\ddraw\include \
  /i..\..\..\..\utilitaires\borland\cbuilder3\bin /i$(BCB)\bin /i$(BCB)\include \
  /i$(BCB)\include\vcl /iC:\Prog\dxsdk\sdk\Lib /iC:\CRPG\Prog\DDraw\Include \
  /dCBUILDER /dMAPEDITOR /d_RTLDLL /mx /w2 /zi
LFLAGS = -L..\..\ddraw\include;"C:\Prog\SDK\DirectX 6.1 SDK\lib\Borland\";"..\..\..\..\logiciels\borland\borland partage\images\buttons";..\..\..\..\logiciels\borland\cbuilder3\bin;..\..\..\..\utilitaires\borland\cbuilder3\bin;$(BCB)\bin;$(BCB)\lib\obj;$(BCB)\lib;$(DEBUGLIBPATH) \
  -aa -Tpe -x -v
IFLAGS =
LINKER = ilink32
# ---------------------------------------------------------------------------
ALLOBJ = c0w32.obj sysinit.obj $(OBJFILES)
ALLRES = $(RESFILES)
ALLLIB = $(LIBFILES) $(LIBRARIES) import32.lib cp32mti.lib
# ---------------------------------------------------------------------------
.autodepend

!ifdef IDEOPTIONS

[Version Info]
IncludeVerInfo=1
AutoIncBuild=0
MajorVer=4
MinorVer=5
Release=1
Build=201
Debug=0
PreRelease=1
Special=0
Private=0
DLL=0
Locale=1036
CodePage=1252

[Version Info Keys]
CompanyName=Nains
FileDescription=Editeur de map pour ZeGame
FileVersion=4.5.1.201
InternalName=MapEditor
LegalCopyright=(C) 1998 - Nains - ZeGame
LegalTrademarks=
OriginalFilename=
ProductName=
ProductVersion=1.0.0.0
Comments=

[Excluded Packages]
D:\WIN2000\System32\ibsmp35.bpl=Composant  Alerter InterBase Borland C++

[HistoryLists\hlIncludePath]
Count=4
Item0=..\..\..\..\logiciels\borland\borland partage\images\buttons;..\..\..\..\logiciels\borland\cbuilder3\bin;..\..\ddraw\include;..\..\..\..\utilitaires\borland\cbuilder3\bin;$(BCB)\bin;$(BCB)\include;$(BCB)\include\vcl;C:\Prog\dxsdk\sdk\Lib;C:\CRPG\Prog\DDraw\Include
Item1=..\..\ddraw\include;..\..\..\..\utilitaires\borland\cbuilder3\bin;$(BCB)\bin;$(BCB)\include;$(BCB)\include\vcl;C:\Prog\dxsdk\sdk\Lib;C:\CRPG\Prog\DDraw\Include
Item2=..\..\..\..\utilitaires\borland\cbuilder3\bin;$(BCB)\bin;$(BCB)\include;$(BCB)\include\vcl;C:\Prog\dxsdk\sdk\Lib;C:\CRPG\Prog\DDraw\Include
Item3=..\..\..\..\utilitaires\borland\cbuilder3\bin;$(BCB)\bin;$(BCB)\include;$(BCB)\include\vcl;F:\Prog\dxsdk\sdk\Lib;F:\CRPG\Prog\DDraw\Include

[HistoryLists\hlLibraryPath]
Count=6
Item0=..\..\ddraw\include;C:\Prog\SDK\DirectX 6.1 SDK\lib\Borland\;..\..\..\..\logiciels\borland\borland partage\images\buttons;..\..\..\..\logiciels\borland\cbuilder3\bin;..\..\..\..\utilitaires\borland\cbuilder3\bin;$(BCB)\bin;$(BCB)\lib\obj;$(BCB)\lib
Item1=C:\Prog\SDK\DirectX 6.1 SDK\lib\Borland\;..\..\..\..\logiciels\borland\borland partage\images\buttons;..\..\..\..\logiciels\borland\cbuilder3\bin;..\..\..\..\utilitaires\borland\cbuilder3\bin;$(BCB)\bin;$(BCB)\lib\obj;$(BCB)\lib
Item2=..\..\..\..\logiciels\borland\borland partage\images\buttons;..\..\..\..\logiciels\borland\cbuilder3\bin;..\..\ddraw\include;..\..\..\..\utilitaires\borland\cbuilder3\bin;$(BCB)\bin;$(BCB)\lib\obj;$(BCB)\lib;C:\Prog\dxsdk\sdk\Lib
Item3=..\..\ddraw\include;..\..\..\..\utilitaires\borland\cbuilder3\bin;$(BCB)\bin;$(BCB)\lib\obj;$(BCB)\lib;C:\Prog\dxsdk\sdk\Lib
Item4=..\..\..\..\utilitaires\borland\cbuilder3\bin;$(BCB)\bin;$(BCB)\lib\obj;$(BCB)\lib;C:\Prog\dxsdk\sdk\Lib
Item5=..\..\..\..\utilitaires\borland\cbuilder3\bin;$(BCB)\bin;$(BCB)\lib\obj;$(BCB)\lib;F:\Prog\dxsdk\sdk\Lib

[HistoryLists\hlConditionals]
Count=3
Item0=CBUILDER;MAPEDITOR;_RTLDLL
Item1=CBUILDER;MAPEDITOR
Item2=CBUILDER

[HistoryLists\hlUnitAliases]
Count=1
Item0=WinTypes=Windows;WinProcs=Windows;DbiTypes=BDE;DbiProcs=BDE;DbiErrs=BDE

[Debugging]
DebugSourceDirs=

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
