////////////////////////////////////////////////////////////////////////////////
// D�termine le chemin d'acc�s � utiliser par l'appelant					  //
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
    // Cr�ation d'un nouveau r�pertoire
	int stat;
    AnsiString chem;
    chem = DirectoryListBox->Directory + '\\' + EditNewRep->Text;
    stat = mkdir(chem.c_str());
    if (stat)
       MessageBoxA(NULL,"Impossible de cr�er le r�pertoire sp�cifi�","Warning !",1);
    else
    {  // Ca s'est bien pass�
       DirectoryListBox->Directory = chem;
    }
}
//---------------------------------------------------------------------------
