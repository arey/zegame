//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "fPersosOutils.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormPersosOutils *FormPersosOutils;
//---------------------------------------------------------------------------
__fastcall TFormPersosOutils::TFormPersosOutils(TComponent* Owner)
    : TForm(Owner)
{   Top = 211; Left = 664;
    Width = 350; Height = 480;
    WindowState = wsMinimized; 		// par défaut
}
//---------------------------------------------------------------------------
