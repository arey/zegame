//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "fMenuBankGrille.h"
#include "fMenuBankGrav.h"
//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TFormImageGrille *FormImageGrille;
//---------------------------------------------------------------------------
__fastcall TFormImageGrille::TFormImageGrille(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------//---------------------------------------------------------------------------?
void __fastcall TFormImageGrille::BitBtn1Click(TObject *Sender)
{
	// Ouvre la boîte de défilement des anims pour attribuer le centre de gravité
    Hide();
    FormGrilleGrav->ShowModal();
}
//---------------------------------------------------------------------------
