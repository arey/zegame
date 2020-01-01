//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("BuildPakFile.res");
USEFORM("PakBuilder.cpp", PakBuilderForm);
USEUNIT("FileVirtual.cpp");
USEUNIT("FileManager.cpp");
USEUNIT("PakFile.cpp");
USEUNIT("INILib.cpp");
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    try
    {
        Application->Initialize();
        Application->Title = "ZGP Pak Builder";
		Application->CreateForm(__classid(TPakBuilderForm), &PakBuilderForm);
		Application->Run();
    }
    catch (Exception &exception)
    {
        Application->ShowException(&exception);
    }
    return 0;
}
//---------------------------------------------------------------------------
