//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEUNIT("DDLibEditor.cpp");
USEFORM("fPrincipale.cpp", FormPrinc);
USERES("PersoEditor.res");
USEFORM("fMenuBank.cpp", MenuNewBank);
USEFORM("fMenuBankGrille.cpp", FormImageGrille);
USEFORM("fMenuBankGrav.cpp", FormGrilleGrav);
USEUNIT("consts.cpp");
USEUNIT("IDMap.cpp");
USEFORM("fMenuIDName.cpp", FormIdentifier);
USEFORM("fMenuIDPath.cpp", FormPath);
USEFORM("fAnimView.cpp", AnimView);
USEFORM("fBarOutils.cpp", BarOutils);
USEFORM("fWorkStation.cpp", WokStation);
USEFORM("fMenuScript.cpp", FormScriptEditor);
USEUNIT("Animall.cpp");
USEUNIT("PersoLib.cpp");
USEUNIT("DIlib.cpp");
USEUNIT("ImDecors2.cpp");
USEUNIT("DDlib.cpp");
USEUNIT("D3DLib.cpp");
USEUNIT("GestFile.cpp");
USEUNIT("WorldSimulation.cpp");
USEUNIT("MapLib.cpp");
USEUNIT("PNJLib.cpp");
USEFORM("uAnimsInfos.cpp", AnimsInfos);
USEUNIT("GameDebug.cpp");
USEUNIT("SpritesLib.cpp");
USEUNIT("Ddutil.cpp");
USELIB("C:\Prog\SDK - DirectX 6.1\lib\Borland\dxguid.lib");
USELIB("C:\Prog\SDK - DirectX 6.1\lib\Borland\d3dim.lib");
USELIB("C:\Prog\SDK - DirectX 6.1\lib\Borland\ddraw.lib");
USELIB("C:\Prog\SDK - DirectX 6.1\lib\Borland\d3dframe.lib");
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		Application->Initialize();
		Application->Title = "AnimEditor";
		Application->CreateForm(__classid(TFormPrinc), &FormPrinc);
		Application->CreateForm(__classid(TMenuNewBank), &MenuNewBank);
		Application->CreateForm(__classid(TFormImageGrille), &FormImageGrille);
		Application->CreateForm(__classid(TFormGrilleGrav), &FormGrilleGrav);
		Application->CreateForm(__classid(TFormIdentifier), &FormIdentifier);
		Application->CreateForm(__classid(TFormPath), &FormPath);
		Application->CreateForm(__classid(TAnimView), &AnimView);
		Application->CreateForm(__classid(TBarOutils), &BarOutils);
		Application->CreateForm(__classid(TWokStation), &WokStation);
		Application->CreateForm(__classid(TFormScriptEditor), &FormScriptEditor);
		Application->CreateForm(__classid(TAnimsInfos), &AnimsInfos);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	return 0;
}
//---------------------------------------------------------------------------
