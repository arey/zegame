# ---------------------------------------------------------------------------
VERSION = BCB.03
# ---------------------------------------------------------------------------
!ifndef BCB
BCB = $(MAKEDIR)\..
!endif
# ---------------------------------------------------------------------------
PROJECT = CreateDecors.exe
OBJFILES = CreateDecors.obj TileDecors.obj ImDecors.obj ImageZoom.obj
RESFILES = CreateDecors.res
RESDEPEN = $(RESFILES) TileDecors.dfm ImageZoom.dfm
LIBFILES = 
LIBRARIES = vcldbx35.lib vcldb35.lib vclx35.lib vcl35.lib
SPARELIBS = vcl35.lib vclx35.lib vcldb35.lib vcldbx35.lib
PACKAGES = VCLX35.bpi VCL35.bpi
PATHASM = .;
PATHCPP = .;
PATHPAS = .;
PATHRC = .;
DEBUGLIBPATH = $(BCB)\lib\debug
RELEASELIBPATH = $(BCB)\lib\release
DEFFILE =
# ---------------------------------------------------------------------------
CFLAG1 = -Od -Hc -w -Ve -k- -y -v -vi- -c -b- -w-par -w-inl -Vx
CFLAG2 = -I$(BCB)\bin;$(BCB)\include;$(BCB)\include\vcl;F:\Prog\dxsdk\sdk\Lib;F:\CRPG\Prog\DDraw\Include \
  -H=$(BCB)\lib\vcld.csm
CFLAG3 =
PFLAGS = -AWinTypes=Windows;WinProcs=Windows;DbiTypes=BDE;DbiProcs=BDE;DbiErrs=BDE \
  -U$(BCB)\bin;$(BCB)\lib\obj;$(BCB)\lib;F:\Prog\dxsdk\sdk\Lib;$(RELEASELIBPATH) \
  -I$(BCB)\bin;$(BCB)\include;$(BCB)\include\vcl;F:\Prog\dxsdk\sdk\Lib;F:\CRPG\Prog\DDraw\Include \
  -$L- -$D- -v -JPHNV -M
RFLAGS = -i$(BCB)\bin;$(BCB)\include;$(BCB)\include\vcl;F:\Prog\dxsdk\sdk\Lib;F:\CRPG\Prog\DDraw\Include
AFLAGS = /i$(BCB)\bin /i$(BCB)\include /i$(BCB)\include\vcl /iF:\Prog\dxsdk\sdk\Lib \
  /iF:\CRPG\Prog\DDraw\Include /mx /w2 /zn
LFLAGS = -L$(BCB)\bin;$(BCB)\lib\obj;$(BCB)\lib;F:\Prog\dxsdk\sdk\Lib;$(RELEASELIBPATH) \
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
IncludeVerInfo=0
AutoIncBuild=0
MajorVer=1
MinorVer=0
Release=0
Build=0
Debug=0
PreRelease=0
Special=0
Private=0
DLL=0
Locale=1036
CodePage=1252

[Version Info Keys]
FileVersion=1.0.0.0

[HistoryLists\hlIncludePath]
Count=1
Item0=$(BCB)\bin;$(BCB)\include;$(BCB)\include\vcl;F:\Prog\dxsdk\sdk\Lib;F:\CRPG\Prog\DDraw\Include

[HistoryLists\hlLibraryPath]
Count=1
Item0=$(BCB)\bin;$(BCB)\lib\obj;$(BCB)\lib;F:\Prog\dxsdk\sdk\Lib

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
