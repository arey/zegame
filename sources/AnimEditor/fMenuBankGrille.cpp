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
	// Ouvre la bo�te de d�filement des anims pour attribuer le centre de gravit�
    Hide();
    FormGrilleGrav->ShowModal();
}
//---------------------------------------------------------------------------
