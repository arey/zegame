//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("TileDecors.cpp", Form1);
USERES("CreateDecors.res");
USEUNIT("ImDecors.cpp");
USEFORM("ImageZoom.cpp", Zooming);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		Application->Initialize();
		Application->Title = "CreateDecors";
		Application->CreateForm(__classid(TForm1), &Form1);
                Application->CreateForm(__classid(TZooming), &Zooming);
                Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	return 0;
}
//---------------------------------------------------------------------------
