////////////////////////////////////////////////////////////////////////////////
// Détermine le chemin d'accés à utiliser par l'appelant					  //
////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "fMenuIDPath.h"
#include <dir.h>			// Pour utiliser la commande "mkdir"

//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TFormPath *FormPath;
//---------------------------------------------------------------------------
__fastcall TFormPath::TFormPath(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormPath::BitBtnNewRepClick(TObject *Sender)
{
    // Création d'un nouveau répertoire
	int stat;
    AnsiString chem;
    chem = DirectoryListBox->Directory + '\\' + EditNewRep->Text;
    stat = mkdir(chem.c_str());
    if (stat)
       MessageBoxA(NULL,"Impossible de créer le répertoire spécifié","Warning !",1);
    else
    {  // Ca s'est bien passé
       DirectoryListBox->Directory = chem;
    }
}
//---------------------------------------------------------------------------
