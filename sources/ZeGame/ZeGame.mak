# Microsoft Developer Studio Generated NMAKE File, Based on ZeGame.dsp
!IF "$(CFG)" == ""
CFG=ZeGame - Win32 Debug
!MESSAGE No configuration specified. Defaulting to ZeGame - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ZeGame - Win32 Release" && "$(CFG)" != "ZeGame - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ZeGame.mak" CFG="ZeGame - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ZeGame - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ZeGame - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "ZeGame - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : ".\ZeGame\ZeGame.exe"


CLEAN :
	-@erase "$(INTDIR)\Animall.obj"
	-@erase "$(INTDIR)\Common.obj"
	-@erase "$(INTDIR)\Consts.obj"
	-@erase "$(INTDIR)\D3DLib.obj"
	-@erase "$(INTDIR)\Dct64.obj"
	-@erase "$(INTDIR)\DDlib.obj"
	-@erase "$(INTDIR)\DDLibAsm.obj"
	-@erase "$(INTDIR)\ddutil.obj"
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\Decode.obj"
	-@erase "$(INTDIR)\DIlib.obj"
	-@erase "$(INTDIR)\DSLib.obj"
	-@erase "$(INTDIR)\DSMp3.obj"
	-@erase "$(INTDIR)\DSWave.obj"
	-@erase "$(INTDIR)\fBeginDlg.obj"
	-@erase "$(INTDIR)\FileManager.obj"
	-@erase "$(INTDIR)\FileVirtual.obj"
	-@erase "$(INTDIR)\FPath.obj"
	-@erase "$(INTDIR)\GameDebug.obj"
	-@erase "$(INTDIR)\GameObject.obj"
	-@erase "$(INTDIR)\GameObjectList.obj"
	-@erase "$(INTDIR)\GestFile.obj"
	-@erase "$(INTDIR)\IDMap.obj"
	-@erase "$(INTDIR)\ImageDDFile.obj"
	-@erase "$(INTDIR)\ImageFile.obj"
	-@erase "$(INTDIR)\ImageUtil.obj"
	-@erase "$(INTDIR)\ImDecors2.obj"
	-@erase "$(INTDIR)\INILib.obj"
	-@erase "$(INTDIR)\interface.obj"
	-@erase "$(INTDIR)\Layer3.obj"
	-@erase "$(INTDIR)\MapLib.obj"
	-@erase "$(INTDIR)\MapZone.obj"
	-@erase "$(INTDIR)\MemLib.obj"
	-@erase "$(INTDIR)\PakFile.obj"
	-@erase "$(INTDIR)\PersoLib.obj"
	-@erase "$(INTDIR)\PhysicObject.obj"
	-@erase "$(INTDIR)\PNJLib.obj"
	-@erase "$(INTDIR)\ScenarioObject.obj"
	-@erase "$(INTDIR)\ScnActions.obj"
	-@erase "$(INTDIR)\ScnActions_PersosMove.obj"
	-@erase "$(INTDIR)\ScnActions_Sound.obj"
	-@erase "$(INTDIR)\ScnCauses.obj"
	-@erase "$(INTDIR)\ScnConsts.obj"
	-@erase "$(INTDIR)\ScnEvents.obj"
	-@erase "$(INTDIR)\ScnGame.obj"
	-@erase "$(INTDIR)\ScnScript.obj"
	-@erase "$(INTDIR)\SoundEffect.obj"
	-@erase "$(INTDIR)\SoundLib.obj"
	-@erase "$(INTDIR)\SoundUtil.obj"
	-@erase "$(INTDIR)\SpritesLib.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\Tabinit.obj"
	-@erase "$(INTDIR)\TimeClock.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\wave.obj"
	-@erase "$(INTDIR)\WorldSimulation.obj"
	-@erase "$(INTDIR)\ZeGame.obj"
	-@erase "$(INTDIR)\ZeGame.res"
	-@erase "$(INTDIR)\ZeGameMain.obj"
	-@erase "$(INTDIR)\ZeGameWnd.obj"
	-@erase "$(INTDIR)\ZoneManager.obj"
	-@erase ".\ZeGame\ZeGame.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G5 /MD /W3 /GR /GX /O2 /I "Include/" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "VISUALC" /D "MICROSOFT" /D "NEW_FILE_MANAGER" /Fp"$(INTDIR)\ZeGame.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x40c /fo"$(INTDIR)\ZeGame.res" /i "Include\\" /d "NDEBUG" /d "_AFXDLL" /d "VISUALC" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ZeGame.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\ZeGame.pdb" /machine:I386 /out:"ZeGame/ZeGame.exe" /libpath:"Lib" 
LINK32_OBJS= \
	"$(INTDIR)\ScnActions_PersosMove.obj" \
	"$(INTDIR)\ScnActions_Sound.obj" \
	"$(INTDIR)\Common.obj" \
	"$(INTDIR)\Dct64.obj" \
	"$(INTDIR)\Decode.obj" \
	"$(INTDIR)\interface.obj" \
	"$(INTDIR)\Layer3.obj" \
	"$(INTDIR)\Tabinit.obj" \
	"$(INTDIR)\Animall.obj" \
	"$(INTDIR)\Consts.obj" \
	"$(INTDIR)\D3DLib.obj" \
	"$(INTDIR)\DDlib.obj" \
	"$(INTDIR)\DDLibAsm.obj" \
	"$(INTDIR)\DIlib.obj" \
	"$(INTDIR)\DSLib.obj" \
	"$(INTDIR)\DSMp3.obj" \
	"$(INTDIR)\DSWave.obj" \
	"$(INTDIR)\fBeginDlg.obj" \
	"$(INTDIR)\FileManager.obj" \
	"$(INTDIR)\FileVirtual.obj" \
	"$(INTDIR)\FPath.obj" \
	"$(INTDIR)\GameDebug.obj" \
	"$(INTDIR)\GameObject.obj" \
	"$(INTDIR)\GameObjectList.obj" \
	"$(INTDIR)\GestFile.obj" \
	"$(INTDIR)\IDMap.obj" \
	"$(INTDIR)\ImageDDFile.obj" \
	"$(INTDIR)\ImageFile.obj" \
	"$(INTDIR)\ImageUtil.obj" \
	"$(INTDIR)\ImDecors2.obj" \
	"$(INTDIR)\INILib.obj" \
	"$(INTDIR)\MapLib.obj" \
	"$(INTDIR)\MapZone.obj" \
	"$(INTDIR)\MemLib.obj" \
	"$(INTDIR)\PakFile.obj" \
	"$(INTDIR)\PersoLib.obj" \
	"$(INTDIR)\PhysicObject.obj" \
	"$(INTDIR)\PNJLib.obj" \
	"$(INTDIR)\ScnActions.obj" \
	"$(INTDIR)\ScnCauses.obj" \
	"$(INTDIR)\ScnConsts.obj" \
	"$(INTDIR)\ScnEvents.obj" \
	"$(INTDIR)\ScnGame.obj" \
	"$(INTDIR)\ScnScript.obj" \
	"$(INTDIR)\SoundEffect.obj" \
	"$(INTDIR)\SoundLib.obj" \
	"$(INTDIR)\SoundUtil.obj" \
	"$(INTDIR)\SpritesLib.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TimeClock.obj" \
	"$(INTDIR)\WorldSimulation.obj" \
	"$(INTDIR)\ZeGame.obj" \
	"$(INTDIR)\ZeGameMain.obj" \
	"$(INTDIR)\ZeGameWnd.obj" \
	"$(INTDIR)\ZoneManager.obj" \
	"$(INTDIR)\ddutil.obj" \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\wave.obj" \
	"$(INTDIR)\ZeGame.res" \
	".\Lib\Mapi32.lib" \
	".\Lib\d3dim.lib" \
	".\Lib\ddraw.lib" \
	".\Lib\dinput.lib" \
	".\Lib\dsound.lib" \
	".\Lib\dxguid.lib" \
	".\Lib\DXTrans.lib" \
	".\Lib\Guids.lib" \
	".\Lib\d3dframe.lib" \
	".\Lib\Winmm.lib" \
	"$(INTDIR)\ScenarioObject.obj"

".\ZeGame\ZeGame.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : ".\ZeGame\ZeGame.exe" "$(OUTDIR)\ZeGame.bsc"


CLEAN :
	-@erase "$(INTDIR)\Animall.obj"
	-@erase "$(INTDIR)\Animall.sbr"
	-@erase "$(INTDIR)\Common.obj"
	-@erase "$(INTDIR)\Common.sbr"
	-@erase "$(INTDIR)\Consts.obj"
	-@erase "$(INTDIR)\Consts.sbr"
	-@erase "$(INTDIR)\D3DLib.obj"
	-@erase "$(INTDIR)\D3DLib.sbr"
	-@erase "$(INTDIR)\Dct64.obj"
	-@erase "$(INTDIR)\Dct64.sbr"
	-@erase "$(INTDIR)\DDlib.obj"
	-@erase "$(INTDIR)\DDlib.sbr"
	-@erase "$(INTDIR)\DDLibAsm.obj"
	-@erase "$(INTDIR)\DDLibAsm.sbr"
	-@erase "$(INTDIR)\ddutil.obj"
	-@erase "$(INTDIR)\ddutil.sbr"
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\debug.sbr"
	-@erase "$(INTDIR)\Decode.obj"
	-@erase "$(INTDIR)\Decode.sbr"
	-@erase "$(INTDIR)\DIlib.obj"
	-@erase "$(INTDIR)\DIlib.sbr"
	-@erase "$(INTDIR)\DSLib.obj"
	-@erase "$(INTDIR)\DSLib.sbr"
	-@erase "$(INTDIR)\DSMp3.obj"
	-@erase "$(INTDIR)\DSMp3.sbr"
	-@erase "$(INTDIR)\DSWave.obj"
	-@erase "$(INTDIR)\DSWave.sbr"
	-@erase "$(INTDIR)\fBeginDlg.obj"
	-@erase "$(INTDIR)\fBeginDlg.sbr"
	-@erase "$(INTDIR)\FileManager.obj"
	-@erase "$(INTDIR)\FileManager.sbr"
	-@erase "$(INTDIR)\FileVirtual.obj"
	-@erase "$(INTDIR)\FileVirtual.sbr"
	-@erase "$(INTDIR)\FPath.obj"
	-@erase "$(INTDIR)\FPath.sbr"
	-@erase "$(INTDIR)\GameDebug.obj"
	-@erase "$(INTDIR)\GameDebug.sbr"
	-@erase "$(INTDIR)\GameObject.obj"
	-@erase "$(INTDIR)\GameObject.sbr"
	-@erase "$(INTDIR)\GameObjectList.obj"
	-@erase "$(INTDIR)\GameObjectList.sbr"
	-@erase "$(INTDIR)\GestFile.obj"
	-@erase "$(INTDIR)\GestFile.sbr"
	-@erase "$(INTDIR)\IDMap.obj"
	-@erase "$(INTDIR)\IDMap.sbr"
	-@erase "$(INTDIR)\ImageDDFile.obj"
	-@erase "$(INTDIR)\ImageDDFile.sbr"
	-@erase "$(INTDIR)\ImageFile.obj"
	-@erase "$(INTDIR)\ImageFile.sbr"
	-@erase "$(INTDIR)\ImageUtil.obj"
	-@erase "$(INTDIR)\ImageUtil.sbr"
	-@erase "$(INTDIR)\ImDecors2.obj"
	-@erase "$(INTDIR)\ImDecors2.sbr"
	-@erase "$(INTDIR)\INILib.obj"
	-@erase "$(INTDIR)\INILib.sbr"
	-@erase "$(INTDIR)\interface.obj"
	-@erase "$(INTDIR)\interface.sbr"
	-@erase "$(INTDIR)\Layer3.obj"
	-@erase "$(INTDIR)\Layer3.sbr"
	-@erase "$(INTDIR)\MapLib.obj"
	-@erase "$(INTDIR)\MapLib.sbr"
	-@erase "$(INTDIR)\MapZone.obj"
	-@erase "$(INTDIR)\MapZone.sbr"
	-@erase "$(INTDIR)\MemLib.obj"
	-@erase "$(INTDIR)\MemLib.sbr"
	-@erase "$(INTDIR)\PakFile.obj"
	-@erase "$(INTDIR)\PakFile.sbr"
	-@erase "$(INTDIR)\PersoLib.obj"
	-@erase "$(INTDIR)\PersoLib.sbr"
	-@erase "$(INTDIR)\PhysicObject.obj"
	-@erase "$(INTDIR)\PhysicObject.sbr"
	-@erase "$(INTDIR)\PNJLib.obj"
	-@erase "$(INTDIR)\PNJLib.sbr"
	-@erase "$(INTDIR)\ScenarioObject.obj"
	-@erase "$(INTDIR)\ScenarioObject.sbr"
	-@erase "$(INTDIR)\ScnActions.obj"
	-@erase "$(INTDIR)\ScnActions.sbr"
	-@erase "$(INTDIR)\ScnActions_PersosMove.obj"
	-@erase "$(INTDIR)\ScnActions_PersosMove.sbr"
	-@erase "$(INTDIR)\ScnActions_Sound.obj"
	-@erase "$(INTDIR)\ScnActions_Sound.sbr"
	-@erase "$(INTDIR)\ScnCauses.obj"
	-@erase "$(INTDIR)\ScnCauses.sbr"
	-@erase "$(INTDIR)\ScnConsts.obj"
	-@erase "$(INTDIR)\ScnConsts.sbr"
	-@erase "$(INTDIR)\ScnEvents.obj"
	-@erase "$(INTDIR)\ScnEvents.sbr"
	-@erase "$(INTDIR)\ScnGame.obj"
	-@erase "$(INTDIR)\ScnGame.sbr"
	-@erase "$(INTDIR)\ScnScript.obj"
	-@erase "$(INTDIR)\ScnScript.sbr"
	-@erase "$(INTDIR)\SoundEffect.obj"
	-@erase "$(INTDIR)\SoundEffect.sbr"
	-@erase "$(INTDIR)\SoundLib.obj"
	-@erase "$(INTDIR)\SoundLib.sbr"
	-@erase "$(INTDIR)\SoundUtil.obj"
	-@erase "$(INTDIR)\SoundUtil.sbr"
	-@erase "$(INTDIR)\SpritesLib.obj"
	-@erase "$(INTDIR)\SpritesLib.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\Tabinit.obj"
	-@erase "$(INTDIR)\Tabinit.sbr"
	-@erase "$(INTDIR)\TimeClock.obj"
	-@erase "$(INTDIR)\TimeClock.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\wave.obj"
	-@erase "$(INTDIR)\wave.sbr"
	-@erase "$(INTDIR)\WorldSimulation.obj"
	-@erase "$(INTDIR)\WorldSimulation.sbr"
	-@erase "$(INTDIR)\ZeGame.obj"
	-@erase "$(INTDIR)\ZeGame.res"
	-@erase "$(INTDIR)\ZeGame.sbr"
	-@erase "$(INTDIR)\ZeGameMain.obj"
	-@erase "$(INTDIR)\ZeGameMain.sbr"
	-@erase "$(INTDIR)\ZeGameWnd.obj"
	-@erase "$(INTDIR)\ZeGameWnd.sbr"
	-@erase "$(INTDIR)\ZoneManager.obj"
	-@erase "$(INTDIR)\ZoneManager.sbr"
	-@erase "$(OUTDIR)\ZeGame.bsc"
	-@erase "$(OUTDIR)\ZeGame.pdb"
	-@erase ".\ZeGame\ZeGame.exe"
	-@erase ".\ZeGame\ZeGame.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W4 /Gm /Gi /GR /GX /ZI /Od /I "Sources" /I "Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "MICROSOFT" /D "VISUALC" /D "DEBUG" /D "_AFXDLL" /D "NEW_FILE_MANAGER" /D "WIN2000" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\ZeGame.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x40c /fo"$(INTDIR)\ZeGame.res" /i "Lib" /i "Include" /i "Sources" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ZeGame.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ScnActions_PersosMove.sbr" \
	"$(INTDIR)\ScnActions_Sound.sbr" \
	"$(INTDIR)\Common.sbr" \
	"$(INTDIR)\Dct64.sbr" \
	"$(INTDIR)\Decode.sbr" \
	"$(INTDIR)\interface.sbr" \
	"$(INTDIR)\Layer3.sbr" \
	"$(INTDIR)\Tabinit.sbr" \
	"$(INTDIR)\Animall.sbr" \
	"$(INTDIR)\Consts.sbr" \
	"$(INTDIR)\D3DLib.sbr" \
	"$(INTDIR)\DDlib.sbr" \
	"$(INTDIR)\DDLibAsm.sbr" \
	"$(INTDIR)\DIlib.sbr" \
	"$(INTDIR)\DSLib.sbr" \
	"$(INTDIR)\DSMp3.sbr" \
	"$(INTDIR)\DSWave.sbr" \
	"$(INTDIR)\fBeginDlg.sbr" \
	"$(INTDIR)\FileManager.sbr" \
	"$(INTDIR)\FileVirtual.sbr" \
	"$(INTDIR)\FPath.sbr" \
	"$(INTDIR)\GameDebug.sbr" \
	"$(INTDIR)\GameObject.sbr" \
	"$(INTDIR)\GameObjectList.sbr" \
	"$(INTDIR)\GestFile.sbr" \
	"$(INTDIR)\IDMap.sbr" \
	"$(INTDIR)\ImageDDFile.sbr" \
	"$(INTDIR)\ImageFile.sbr" \
	"$(INTDIR)\ImageUtil.sbr" \
	"$(INTDIR)\ImDecors2.sbr" \
	"$(INTDIR)\INILib.sbr" \
	"$(INTDIR)\MapLib.sbr" \
	"$(INTDIR)\MapZone.sbr" \
	"$(INTDIR)\MemLib.sbr" \
	"$(INTDIR)\PakFile.sbr" \
	"$(INTDIR)\PersoLib.sbr" \
	"$(INTDIR)\PhysicObject.sbr" \
	"$(INTDIR)\PNJLib.sbr" \
	"$(INTDIR)\ScnActions.sbr" \
	"$(INTDIR)\ScnCauses.sbr" \
	"$(INTDIR)\ScnConsts.sbr" \
	"$(INTDIR)\ScnEvents.sbr" \
	"$(INTDIR)\ScnGame.sbr" \
	"$(INTDIR)\ScnScript.sbr" \
	"$(INTDIR)\SoundEffect.sbr" \
	"$(INTDIR)\SoundLib.sbr" \
	"$(INTDIR)\SoundUtil.sbr" \
	"$(INTDIR)\SpritesLib.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\TimeClock.sbr" \
	"$(INTDIR)\WorldSimulation.sbr" \
	"$(INTDIR)\ZeGame.sbr" \
	"$(INTDIR)\ZeGameMain.sbr" \
	"$(INTDIR)\ZeGameWnd.sbr" \
	"$(INTDIR)\ZoneManager.sbr" \
	"$(INTDIR)\ddutil.sbr" \
	"$(INTDIR)\debug.sbr" \
	"$(INTDIR)\wave.sbr" \
	"$(INTDIR)\ScenarioObject.sbr"

"$(OUTDIR)\ZeGame.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\ZeGame.pdb" /debug /machine:I386 /out:"ZeGame/ZeGame.exe" /pdbtype:sept /libpath:"Lib" 
LINK32_OBJS= \
	"$(INTDIR)\ScnActions_PersosMove.obj" \
	"$(INTDIR)\ScnActions_Sound.obj" \
	"$(INTDIR)\Common.obj" \
	"$(INTDIR)\Dct64.obj" \
	"$(INTDIR)\Decode.obj" \
	"$(INTDIR)\interface.obj" \
	"$(INTDIR)\Layer3.obj" \
	"$(INTDIR)\Tabinit.obj" \
	"$(INTDIR)\Animall.obj" \
	"$(INTDIR)\Consts.obj" \
	"$(INTDIR)\D3DLib.obj" \
	"$(INTDIR)\DDlib.obj" \
	"$(INTDIR)\DDLibAsm.obj" \
	"$(INTDIR)\DIlib.obj" \
	"$(INTDIR)\DSLib.obj" \
	"$(INTDIR)\DSMp3.obj" \
	"$(INTDIR)\DSWave.obj" \
	"$(INTDIR)\fBeginDlg.obj" \
	"$(INTDIR)\FileManager.obj" \
	"$(INTDIR)\FileVirtual.obj" \
	"$(INTDIR)\FPath.obj" \
	"$(INTDIR)\GameDebug.obj" \
	"$(INTDIR)\GameObject.obj" \
	"$(INTDIR)\GameObjectList.obj" \
	"$(INTDIR)\GestFile.obj" \
	"$(INTDIR)\IDMap.obj" \
	"$(INTDIR)\ImageDDFile.obj" \
	"$(INTDIR)\ImageFile.obj" \
	"$(INTDIR)\ImageUtil.obj" \
	"$(INTDIR)\ImDecors2.obj" \
	"$(INTDIR)\INILib.obj" \
	"$(INTDIR)\MapLib.obj" \
	"$(INTDIR)\MapZone.obj" \
	"$(INTDIR)\MemLib.obj" \
	"$(INTDIR)\PakFile.obj" \
	"$(INTDIR)\PersoLib.obj" \
	"$(INTDIR)\PhysicObject.obj" \
	"$(INTDIR)\PNJLib.obj" \
	"$(INTDIR)\ScnActions.obj" \
	"$(INTDIR)\ScnCauses.obj" \
	"$(INTDIR)\ScnConsts.obj" \
	"$(INTDIR)\ScnEvents.obj" \
	"$(INTDIR)\ScnGame.obj" \
	"$(INTDIR)\ScnScript.obj" \
	"$(INTDIR)\SoundEffect.obj" \
	"$(INTDIR)\SoundLib.obj" \
	"$(INTDIR)\SoundUtil.obj" \
	"$(INTDIR)\SpritesLib.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TimeClock.obj" \
	"$(INTDIR)\WorldSimulation.obj" \
	"$(INTDIR)\ZeGame.obj" \
	"$(INTDIR)\ZeGameMain.obj" \
	"$(INTDIR)\ZeGameWnd.obj" \
	"$(INTDIR)\ZoneManager.obj" \
	"$(INTDIR)\ddutil.obj" \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\wave.obj" \
	"$(INTDIR)\ZeGame.res" \
	".\Lib\Mapi32.lib" \
	".\Lib\d3dim.lib" \
	".\Lib\ddraw.lib" \
	".\Lib\dinput.lib" \
	".\Lib\dsound.lib" \
	".\Lib\dxguid.lib" \
	".\Lib\DXTrans.lib" \
	".\Lib\Guids.lib" \
	".\Lib\d3dframe.lib" \
	".\Lib\Winmm.lib" \
	"$(INTDIR)\ScenarioObject.obj"

".\ZeGame\ZeGame.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("ZeGame.dep")
!INCLUDE "ZeGame.dep"
!ELSE 
!MESSAGE Warning: cannot find "ZeGame.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ZeGame - Win32 Release" || "$(CFG)" == "ZeGame - Win32 Debug"
SOURCE=.\Sources\ScnActions_PersosMove.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\ScnActions_PersosMove.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\ScnActions_PersosMove.obj"	"$(INTDIR)\ScnActions_PersosMove.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\ScnActions_Sound.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\ScnActions_Sound.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\ScnActions_Sound.obj"	"$(INTDIR)\ScnActions_Sound.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\Mpglib\Common.c

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\Common.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\Common.obj"	"$(INTDIR)\Common.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\Mpglib\Dct64.c

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\Dct64.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\Dct64.obj"	"$(INTDIR)\Dct64.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\Mpglib\Decode.c

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\Decode.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\Decode.obj"	"$(INTDIR)\Decode.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\Mpglib\interface.c

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\interface.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\interface.obj"	"$(INTDIR)\interface.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\Mpglib\Layer3.c

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\Layer3.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\Layer3.obj"	"$(INTDIR)\Layer3.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\Mpglib\Tabinit.c

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\Tabinit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\Tabinit.obj"	"$(INTDIR)\Tabinit.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\Animall.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\Animall.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\Animall.obj"	"$(INTDIR)\Animall.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\Consts.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\Consts.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\Consts.obj"	"$(INTDIR)\Consts.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\D3DLib.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\D3DLib.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\D3DLib.obj"	"$(INTDIR)\D3DLib.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\DDlib.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\DDlib.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\DDlib.obj"	"$(INTDIR)\DDlib.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\DDLibAsm.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\DDLibAsm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\DDLibAsm.obj"	"$(INTDIR)\DDLibAsm.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\DIlib.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\DIlib.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\DIlib.obj"	"$(INTDIR)\DIlib.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\DSLib.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\DSLib.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\DSLib.obj"	"$(INTDIR)\DSLib.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\DSMp3.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\DSMp3.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\DSMp3.obj"	"$(INTDIR)\DSMp3.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\DSWave.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\DSWave.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\DSWave.obj"	"$(INTDIR)\DSWave.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\fBeginDlg.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\fBeginDlg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\fBeginDlg.obj"	"$(INTDIR)\fBeginDlg.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\FileManager.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\FileManager.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\FileManager.obj"	"$(INTDIR)\FileManager.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\FileVirtual.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\FileVirtual.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\FileVirtual.obj"	"$(INTDIR)\FileVirtual.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\FPath.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\FPath.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\FPath.obj"	"$(INTDIR)\FPath.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\GameDebug.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\GameDebug.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\GameDebug.obj"	"$(INTDIR)\GameDebug.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\GameObject.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\GameObject.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\GameObject.obj"	"$(INTDIR)\GameObject.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\GameObjectList.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\GameObjectList.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\GameObjectList.obj"	"$(INTDIR)\GameObjectList.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\GestFile.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\GestFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\GestFile.obj"	"$(INTDIR)\GestFile.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\IDMap.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\IDMap.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\IDMap.obj"	"$(INTDIR)\IDMap.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\ImageDDFile.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\ImageDDFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\ImageDDFile.obj"	"$(INTDIR)\ImageDDFile.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\ImageFile.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\ImageFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\ImageFile.obj"	"$(INTDIR)\ImageFile.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\ImageUtil.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\ImageUtil.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\ImageUtil.obj"	"$(INTDIR)\ImageUtil.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\ImDecors2.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\ImDecors2.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\ImDecors2.obj"	"$(INTDIR)\ImDecors2.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\INILib.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\INILib.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\INILib.obj"	"$(INTDIR)\INILib.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\MapLib.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\MapLib.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\MapLib.obj"	"$(INTDIR)\MapLib.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\MapZone.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\MapZone.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\MapZone.obj"	"$(INTDIR)\MapZone.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\MemLib.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\MemLib.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\MemLib.obj"	"$(INTDIR)\MemLib.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\PakFile.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\PakFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\PakFile.obj"	"$(INTDIR)\PakFile.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\PersoLib.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\PersoLib.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\PersoLib.obj"	"$(INTDIR)\PersoLib.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\PhysicObject.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\PhysicObject.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\PhysicObject.obj"	"$(INTDIR)\PhysicObject.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\PNJLib.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\PNJLib.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\PNJLib.obj"	"$(INTDIR)\PNJLib.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\ScenarioObject.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\ScenarioObject.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\ScenarioObject.obj"	"$(INTDIR)\ScenarioObject.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\ScnActions.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\ScnActions.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\ScnActions.obj"	"$(INTDIR)\ScnActions.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\ScnCauses.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\ScnCauses.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\ScnCauses.obj"	"$(INTDIR)\ScnCauses.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\ScnConsts.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\ScnConsts.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\ScnConsts.obj"	"$(INTDIR)\ScnConsts.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\ScnEvents.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\ScnEvents.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\ScnEvents.obj"	"$(INTDIR)\ScnEvents.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\ScnGame.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\ScnGame.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\ScnGame.obj"	"$(INTDIR)\ScnGame.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\ScnScript.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\ScnScript.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\ScnScript.obj"	"$(INTDIR)\ScnScript.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\SoundEffect.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\SoundEffect.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\SoundEffect.obj"	"$(INTDIR)\SoundEffect.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\SoundLib.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\SoundLib.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\SoundLib.obj"	"$(INTDIR)\SoundLib.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\SoundUtil.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\SoundUtil.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\SoundUtil.obj"	"$(INTDIR)\SoundUtil.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\SpritesLib.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\SpritesLib.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\SpritesLib.obj"	"$(INTDIR)\SpritesLib.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\StdAfx.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\StdAfx.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\TimeClock.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\TimeClock.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\TimeClock.obj"	"$(INTDIR)\TimeClock.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\WorldSimulation.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\WorldSimulation.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\WorldSimulation.obj"	"$(INTDIR)\WorldSimulation.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\ZeGame.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\ZeGame.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\ZeGame.obj"	"$(INTDIR)\ZeGame.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\ZeGame.rc

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\ZeGame.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x40c /fo"$(INTDIR)\ZeGame.res" /i "Include\\" /i "Sources" /d "NDEBUG" /d "_AFXDLL" /d "VISUALC" $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\ZeGame.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x40c /fo"$(INTDIR)\ZeGame.res" /i "Lib" /i "Include" /i "Sources" /d "_DEBUG" /d "_AFXDLL" $(SOURCE)


!ENDIF 

SOURCE=.\Sources\ZeGameMain.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\ZeGameMain.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\ZeGameMain.obj"	"$(INTDIR)\ZeGameMain.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\ZeGameWnd.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\ZeGameWnd.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\ZeGameWnd.obj"	"$(INTDIR)\ZeGameWnd.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Sources\ZoneManager.cpp

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\ZoneManager.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\ZoneManager.obj"	"$(INTDIR)\ZoneManager.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE="..\zegame 29 02 2000\ddutil.cpp"

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\ddutil.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\ddutil.obj"	"$(INTDIR)\ddutil.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE="..\zegame 29 02 2000\debug.c"

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\debug.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\debug.obj"	"$(INTDIR)\debug.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE="..\zegame 29 02 2000\wave.c"

!IF  "$(CFG)" == "ZeGame - Win32 Release"


"$(INTDIR)\wave.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"


"$(INTDIR)\wave.obj"	"$(INTDIR)\wave.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

