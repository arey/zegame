//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USERES("MapEditor.res");
USEFORM("Editor.cpp", Editeur);
USEUNIT("consts.cpp");
USEUNIT("DDlib.cpp");
USEUNIT("DDLibEditor.cpp");
USEFORM("fDesignOutils.cpp", FormDesignOutils);
USEFORM("fPersosOutils.cpp", FormPersosOutils);
USEFORM("uMapView.cpp", MapView);
USEUNIT("MapLib.cpp");
USEFORM("fNewMap.cpp", NewCarte);
USEFORM("fMapOutils.cpp", MapOutils);
USEFORM("fEditLibDecors.cpp", EditLibDecors);
USEUNIT("ImDecors2.cpp");
USEFORM("fLibOutils.cpp", LibOutils);
USEFORM("fObjOutils.cpp", ObjOutils);
USEFORM("fPrefer.cpp", Prefer);
USEFORM("fEditLibLevel.cpp", LevelLibEditor);
USEFORM("fPathFinding.cpp", PathFinding);
USEUNIT("FPath.cpp");
USEUNIT("Animall.cpp");
USEUNIT("ddutil.cpp");
USEUNIT("IDMap.cpp");
USEUNIT("GestFile.cpp");
USEUNIT("ScnConsts.cpp");
USEUNIT("ScnActions.cpp");
USEUNIT("ScnEvents.cpp");
USEUNIT("GameObject.cpp");
USEUNIT("ScnCauses.cpp");
USEUNIT("ScnScript.cpp");
USEUNIT("GameObjectList.cpp");
USEFORM("fIDOutils.cpp", IDOutils);
USEUNIT("INILib.cpp");
USEUNIT("MapZone.cpp");
USEUNIT("FileVirtual.cpp");
USEUNIT("FileManager.cpp");
USEUNIT("PakFile.cpp");
USEUNIT("WorldSimulation.cpp");
USEUNIT("PersoLib.cpp");
USEUNIT("TimeClock.cpp");
USEUNIT("D3DLib.cpp");
USEUNIT("PNJLib.cpp");
USEUNIT("MemLib.cpp");
USEUNIT("SpritesLib.cpp");
USEUNIT("ImageFile.cpp");
USEUNIT("ImageDDFile.cpp");
USEUNIT("ImageUtil.cpp");
USEUNIT("DIlib.cpp");
USEUNIT("GameDebug.cpp");
USELIB("C:\Prog\SDK - DirectX 6.1\lib\Borland\dxguid.lib");
USELIB("C:\Prog\SDK - DirectX 6.1\lib\Borland\d3dim.lib");
USELIB("C:\Prog\SDK - DirectX 6.1\lib\Borland\ddraw.lib");
USELIB("C:\Prog\SDK - DirectX 6.1\lib\Borland\dinput.lib");
USELIB("C:\Prog\SDK - DirectX 6.1\lib\Borland\dsound.lib");
USELIB("C:\Prog\SDK - DirectX 6.1\lib\Borland\d3dframe.lib");
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		Application->Initialize();
        Application->Title = "Editeur de niveau -- Nain --- v 2.43 beta ";
		Application->CreateForm(__classid(TEditeur), &Editeur);
		Application->CreateForm(__classid(TMapView), &MapView);
		Application->CreateForm(__classid(TFormDesignOutils), &FormDesignOutils);
		Application->CreateForm(__classid(TFormPersosOutils), &FormPersosOutils);
		Application->CreateForm(__classid(TNewCarte), &NewCarte);
		Application->CreateForm(__classid(TMapOutils), &MapOutils);
		Application->CreateForm(__classid(TEditLibDecors), &EditLibDecors);
		Application->CreateForm(__classid(TLibOutils), &LibOutils);
		Application->CreateForm(__classid(TObjOutils), &ObjOutils);
		Application->CreateForm(__classid(TPrefer), &Prefer);
		Application->CreateForm(__classid(TLevelLibEditor), &LevelLibEditor);
		Application->CreateForm(__classid(TPathFinding), &PathFinding);
		Application->CreateForm(__classid(TIDOutils), &IDOutils);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	return 0;
}
//---------------------------------------------------------------------------


