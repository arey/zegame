////////////////////////////////////////////////////////////////////////////////
// Fenêtre permettant de rentrer le nom d'un identificateur 				  //
////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop
#include "fMenuIDName.h"

//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TFormIdentifier *FormIdentifier;
//---------------------------------------------------------------------------
__fastcall TFormIdentifier::TFormIdentifier(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormIdentifier::BitBtnOKClick(TObject *Sender)
{  sortieOK = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormIdentifier::BitBtnAnnulerClick(TObject *Sender)
{  sortieOK = false;
}
//---------------------------------------------------------------------------