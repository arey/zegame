# Microsoft Developer Studio Project File - Name="ZeGame" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ZeGame - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ZeGame.mak".
!MESSAGE 
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

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/ZeGame", BAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ZeGame - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GR /GX /O2 /I "Include/" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "VISUALC" /D "MICROSOFT" /D "NEW_FILE_MANAGER" /D "WIN2000" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x40c /i "Include\\" /d "NDEBUG" /d "_AFXDLL" /d "VISUALC"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386 /out:"ZeGame/ZeGame.exe" /libpath:"Lib"

!ELSEIF  "$(CFG)" == "ZeGame - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /Gi /GR /GX /ZI /Od /I "Sources" /I "Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "MICROSOFT" /D "VISUALC" /D "DEBUG" /D "_AFXDLL" /D "NEW_FILE_MANAGER" /D "WIN2000" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x40c /i "Lib" /i "Include" /i "Sources" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 dxerr8.lib /nologo /subsystem:windows /debug /machine:I386 /out:"ZeGame/ZeGame.exe" /pdbtype:sept /libpath:"Lib"
# SUBTRACT LINK32 /profile /incremental:no

!ENDIF 

# Begin Target

# Name "ZeGame - Win32 Release"
# Name "ZeGame - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "ScnActions"

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=.\Sources\ScnActions_PersosMove.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\ScnActions_Sound.cpp
# End Source File
# End Group
# Begin Group "Mpglib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Sources\Mpglib\Common.c
# End Source File
# Begin Source File

SOURCE=.\Sources\Mpglib\Dct64.c
# End Source File
# Begin Source File

SOURCE=.\Sources\Mpglib\Decode.c
# End Source File
# Begin Source File

SOURCE=.\Sources\Mpglib\Huffman.h
# End Source File
# Begin Source File

SOURCE=.\Sources\Mpglib\interface.c
# End Source File
# Begin Source File

SOURCE=.\Sources\Mpglib\Layer3.c
# End Source File
# Begin Source File

SOURCE=.\Sources\Mpglib\Mpg123.h
# End Source File
# Begin Source File

SOURCE=.\Sources\Mpglib\Mpglib.h
# End Source File
# Begin Source File

SOURCE=.\Sources\Mpglib\Tabinit.c
# End Source File
# End Group
# Begin Group "PhysicObject"

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=.\Sources\PhysicInorganicObject.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\PhysicObject.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\PhysicObjectManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\PhysicProjectileObject.cpp
# End Source File
# End Group
# Begin Group "Input"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Sources\DIlib.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\ForceFeedbackEffect.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\ForceFeedbackEffect_BumpIntoWall.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\ForceFeedbackEffectManager.cpp
# End Source File
# End Group
# Begin Group "Network"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Sources\httpsocket.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\httpsocket.h
# End Source File
# Begin Source File

SOURCE=.\Sources\MPClient.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\MPClient.h
# End Source File
# Begin Source File

SOURCE=.\Sources\MPMonitor.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\MPMonitor.h
# End Source File
# Begin Source File

SOURCE=.\Sources\MPServer.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\MPServer.h
# End Source File
# Begin Source File

SOURCE=.\Sources\MPSession.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\MPSession.h
# End Source File
# Begin Source File

SOURCE=.\Sources\zghttpsocket.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\zghttpsocket.h
# End Source File
# Begin Source File

SOURCE=.\Sources\ZGMultiplayer.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\ZGMultiplayer.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Sources\Animall.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\Consts.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\D3DLib.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\DDlib.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\DDLibAsm.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\DSLib.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\DSMp3.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\DSWave.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\fBeginDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\FileManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\FileVirtual.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\FPath.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\GameDebug.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\GameObject.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\GameObjectList.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\GestFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\IDMap.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\ImageDDFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\ImageFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\ImageUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\ImDecors2.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\INILib.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\MapLib.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\MapZone.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\MemLib.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\PakFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\PersoLib.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\PNJLib.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\ScenarioManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\ScenarioObject.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\ScnActions.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\ScnCauses.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\ScnConsts.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\ScnEvents.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\ScnGame.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\ScnScript.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\SoundEffect.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\SoundLib.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\SoundUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\SpritesLib.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\TimeClock.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\WorldSimulation.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\ZeGame.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\ZeGame.rc
# End Source File
# Begin Source File

SOURCE=.\Sources\ZeGameMain.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\ZeGameWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Sources\ZoneManager.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "ScnActionsH"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\Sources\ScnActions_PersosMove.h
# End Source File
# Begin Source File

SOURCE=.\Sources\ScnActions_Sound.h
# End Source File
# End Group
# Begin Group "PhysicObjectH"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Sources\PhysicInorganicObject.h
# End Source File
# Begin Source File

SOURCE=.\Sources\PhysicObject.h
# End Source File
# Begin Source File

SOURCE=.\Sources\PhysicObjectManager.h
# End Source File
# Begin Source File

SOURCE=.\Sources\PhysicProjectileObject.h
# End Source File
# End Group
# Begin Group "InputH"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\Sources\DIlib.h
# End Source File
# Begin Source File

SOURCE=.\Sources\ForceFeedbackEffect.h
# End Source File
# Begin Source File

SOURCE=.\Sources\ForceFeedbackEffect_BumpIntoWall.h
# End Source File
# Begin Source File

SOURCE=.\Sources\ForceFeedbackEffectID.h
# End Source File
# Begin Source File

SOURCE=.\Sources\ForceFeedbackEffectManager.h
# End Source File
# End Group
# Begin Group "NetworkH"

# PROP Default_Filter ""
# End Group
# Begin Source File

SOURCE=.\Sources\Animall.h
# End Source File
# Begin Source File

SOURCE=.\Sources\ColorFormat.h
# End Source File
# Begin Source File

SOURCE=.\Sources\Consts.h
# End Source File
# Begin Source File

SOURCE=.\Sources\D3DLib.h
# End Source File
# Begin Source File

SOURCE=.\Sources\DDlib.h
# End Source File
# Begin Source File

SOURCE=.\Sources\DDLibAsm.h
# End Source File
# Begin Source File

SOURCE=.\Sources\DSLib.h
# End Source File
# Begin Source File

SOURCE=.\Sources\DSMp3.h
# End Source File
# Begin Source File

SOURCE=.\Sources\DSWave.h
# End Source File
# Begin Source File

SOURCE=.\Sources\fBeginDlg.h
# End Source File
# Begin Source File

SOURCE=.\Sources\FileFormat.h
# End Source File
# Begin Source File

SOURCE=.\Sources\FileManager.h
# End Source File
# Begin Source File

SOURCE=.\Sources\FileVirtual.h
# End Source File
# Begin Source File

SOURCE=.\Sources\FPath.h
# End Source File
# Begin Source File

SOURCE=.\Sources\GameDebug.h
# End Source File
# Begin Source File

SOURCE=.\Sources\GameObject.h
# End Source File
# Begin Source File

SOURCE=.\Sources\GameObjectList.h
# End Source File
# Begin Source File

SOURCE=.\Sources\GestFile.h
# End Source File
# Begin Source File

SOURCE=.\Sources\IDMap.h
# End Source File
# Begin Source File

SOURCE=.\Sources\ImageDDFile.h
# End Source File
# Begin Source File

SOURCE=.\Sources\ImageFile.h
# End Source File
# Begin Source File

SOURCE=.\Sources\ImageUtil.h
# End Source File
# Begin Source File

SOURCE=.\Sources\ImDecors2.h
# End Source File
# Begin Source File

SOURCE=.\Sources\INILib.h
# End Source File
# Begin Source File

SOURCE=.\Sources\MapLib.h
# End Source File
# Begin Source File

SOURCE=.\Sources\MapZone.h
# End Source File
# Begin Source File

SOURCE=.\Sources\MemLib.h
# End Source File
# Begin Source File

SOURCE=.\Sources\PakFile.h
# End Source File
# Begin Source File

SOURCE=.\Sources\PersoLib.h
# End Source File
# Begin Source File

SOURCE=.\Sources\PixelFormat.h
# End Source File
# Begin Source File

SOURCE=.\Sources\PNJLib.h
# End Source File
# Begin Source File

SOURCE=.\Sources\RandomType.h
# End Source File
# Begin Source File

SOURCE=.\Sources\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Sources\ScenarioManager.h
# End Source File
# Begin Source File

SOURCE=.\Sources\ScenarioObject.h
# End Source File
# Begin Source File

SOURCE=.\Sources\ScnActions.h
# End Source File
# Begin Source File

SOURCE=.\Sources\ScnCauses.h
# End Source File
# Begin Source File

SOURCE=.\Sources\ScnConsts.h
# End Source File
# Begin Source File

SOURCE=.\Sources\ScnEvents.h
# End Source File
# Begin Source File

SOURCE=.\Sources\ScnGame.h
# End Source File
# Begin Source File

SOURCE=.\Sources\ScnScript.h
# End Source File
# Begin Source File

SOURCE=.\Sources\Singleton.h
# End Source File
# Begin Source File

SOURCE=.\Sources\SoundEffect.h
# End Source File
# Begin Source File

SOURCE=.\Sources\SoundEffectID.h
# End Source File
# Begin Source File

SOURCE=.\Sources\SoundLib.h
# End Source File
# Begin Source File

SOURCE=.\Sources\SoundUtil.h
# End Source File
# Begin Source File

SOURCE=.\Sources\SpritesLib.h
# End Source File
# Begin Source File

SOURCE=.\Sources\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Sources\TimeClock.h
# End Source File
# Begin Source File

SOURCE=.\Sources\WorldSimulation.h
# End Source File
# Begin Source File

SOURCE=.\Sources\ZeGame.h
# End Source File
# Begin Source File

SOURCE=.\Sources\ZeGameMain.h
# End Source File
# Begin Source File

SOURCE=.\Sources\ZeGameWnd.h
# End Source File
# Begin Source File

SOURCE=.\Sources\ZeNain.h
# End Source File
# Begin Source File

SOURCE=.\Sources\ZoneManager.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Nains.ico
# End Source File
# Begin Source File

SOURCE=.\RES\ZeGame.ico
# End Source File
# Begin Source File

SOURCE=.\res\ZeGame.rc2
# End Source File
# End Group
# Begin Group "Lib"

# PROP Default_Filter "lib"
# Begin Source File

SOURCE=.\Lib\Mapi32.lib
# End Source File
# Begin Source File

SOURCE=.\Lib\d3dim.lib
# End Source File
# Begin Source File

SOURCE=.\Lib\ddraw.lib
# End Source File
# Begin Source File

SOURCE=.\Lib\dinput.lib
# End Source File
# Begin Source File

SOURCE=.\Lib\dsound.lib
# End Source File
# Begin Source File

SOURCE=.\Lib\dxguid.lib
# End Source File
# Begin Source File

SOURCE=.\Lib\DXTrans.lib
# End Source File
# Begin Source File

SOURCE=.\Lib\Guids.lib
# End Source File
# Begin Source File

SOURCE=.\Lib\d3dframe.lib
# End Source File
# Begin Source File

SOURCE=.\Lib\Winmm.lib
# End Source File
# Begin Source File

SOURCE=.\Lib\dinput8.lib
# End Source File
# Begin Source File

SOURCE=.\Lib\WSOCK32.LIB
# End Source File
# End Group
# Begin Group "Include"

# PROP Default_Filter "h;cpp;c;"
# Begin Source File

SOURCE=".\Include\ddutil.cpp"
# End Source File
# Begin Source File

SOURCE="..\zegame 29 02 2000\ddutil.h"
# End Source File
# Begin Source File

SOURCE="..\zegame 29 02 2000\Include\debug.c"
# End Source File
# Begin Source File

SOURCE="..\zegame 29 02 2000\debug.h"
# End Source File
# Begin Source File

SOURCE=.\Include\dxutil.cpp
# End Source File
# Begin Source File

SOURCE=.\Include\dxutil.h
# End Source File
# Begin Source File

SOURCE="..\zegame 29 02 2000\Include\wave.c"
# End Source File
# Begin Source File

SOURCE="..\zegame 29 02 2000\wave.h"
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
