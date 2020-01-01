////////////////////////////////////////////////////////////////////////////////
//  Unit� "fIDUnits.cpp"   impl�mentation de la classe TIDOutils 			  //
//                                                                            //
//	Description  :  fen�tre permettant � l'utilisateur de choisir, cr�er ou	  //
//					supprimer un num�ro d'ID parmi une liste.				  //
//                                                                            //
//  Copyright (C) 1999 Antoine Rey    arey@insa-lyon.fr                       //
//                                    antrey@club-internet.fr                 //
//  Version      : beta 0.01                                                  //
//  Creation     : 24/08/1999 		                                          //
//  Last upgrade : <none>	                                                  //
////////////////////////////////////////////////////////////////////////////////

// --------------------------------------------------------------------- include
#include <vcl.h>
#include "Editor.h"			//v Fichier principal du projet
#pragma hdrstop
#include "fIDOutils.h"		// Header de la classe

// ---------------------------------------------------------------------- pragma
#pragma package(smart_init)
#pragma resource "*.dfm"

// ------------------------------------------------------------------- variables
TIDOutils *IDOutils = NULL;

////////////////////////////////////////////////////////////////////////////////
// Classe TIDOUTILS                                                           //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur ===========================================================//
__fastcall TIDOutils::TIDOutils(TComponent* Owner)
	: TForm(Owner)
{   lpIDMap = NULL;
}
//----------------------------------------------------------------------------//

//=== Lance la fen�tre =======================================================//
void TIDOutils::LetsGo(std::IDMap* map)
{
	lpIDMap = map;
	// Si le dernier n�ID s�lectionn� est valide, on le conserve
	if ((map==NULL) || (iIDSelect==-1) || (!lpIDMap->IsElement(iIDSelect))) iIDSelect = -1;

    // MaJ la liste des nom d'ID
    RefreshIDList();

    // MaJ de l'apparence de la fiche
    IDListBox->ItemIndex = -1;
    NameIDEdit->Text = "";
    NameIDEdit->ReadOnly = true;
	NumIDEdit->Text = "";
    IDSuppr->Enabled = false;
    IDRename->Enabled = false;
    IDOk->Enabled = false;
    IDNew->Enabled = true;
    IDMode = IDMODE_NONE;

    // Lance la fiche
    ShowModal();
}
//----------------------------------------------------------------------------//

//=== MaJ de la liste des nom d'ID ===========================================//
void TIDOutils::RefreshIDList()
{	IDListBox->Items->Clear();
    for (std::IDMap::Iterat iter = lpIDMap->MapID.begin(); iter!= lpIDMap->MapID.end() ; iter++)
    {   
		AnsiString name = (*iter).first;
        IDListBox->Items->Add(name);
    }
    IDSuppr->Enabled = true;
    IDRename->Enabled = true;
}
//----------------------------------------------------------------------------//

//=== S�lection d'un nom d'ID ================================================//
void __fastcall TIDOutils::IDListBoxClick(TObject *Sender)
{
    // On r�cup�re son nom et son n�ID
	AnsiString nameID = IDListBox->Items->Strings[IDListBox->ItemIndex];
    int numID = lpIDMap->NumID(nameID);
    NumIDEdit->Text = numID;
    NameIDEdit->Text = lpIDMap->CleID(numID);
}
//----------------------------------------------------------------------------//

//=== Validation de l'Ajout/Renommage d'un ID ================================//
void __fastcall TIDOutils::IDOkClick(TObject *Sender)
{   AnsiString nameID = NameIDEdit->Text;
	int numID;

    // 1) V�rifie si ce nom n'existe d�j� pas
    if (lpIDMap->IsCle(nameID))
    {	MessageBoxA(NULL,"Impossible d'ajouter/Renomm� cet ID","Nom d�j� existant !",0);
        return;
    }

    // Ajout d'un ID � la liste
    if (IDMode == IDMODE_NEW)
    {   // 2) Attribution d'un n� � ce nom
        lpIDMap->AjoutClef(nameID);
        numID = lpIDMap->NumID(nameID);
        NumIDEdit->Text = numID;
    }
    // Renommage de l'ancien n�ID
    else if (IDMode == IDMODE_RENAME)
    {	// 2) Renommage de l'ID
    	lpIDMap->ChangeClef(IDRenamed, nameID);
    }

    // 3) Sauvegarde la librairie
    lpIDMap->SauveFileID(CheminEditor + lpIDMap->IDFilename);

    // 4) Rafra�chit la liste
    RefreshIDList();
    NameIDEdit->ReadOnly = true;
}
//----------------------------------------------------------------------------//

//=== Ajout d'un ID � la liste ===============================================//
void __fastcall TIDOutils::IDNewClick(TObject *Sender)
{	IDMode = IDMODE_NEW;
	IDListBox->ItemIndex = -1;
    NameIDEdit->Text = "";
    NameIDEdit->ReadOnly = false;
	NumIDEdit->Text = "";
	IDOk->Enabled = true;
    IDSuppr->Enabled = false;
    IDNew->Enabled = false;
}
//----------------------------------------------------------------------------//

//=== Renommage d'un ID de la liste ==========================================//
void __fastcall TIDOutils::IDRenameClick(TObject *Sender)
{	IDMode = IDMODE_RENAME;
    NameIDEdit->ReadOnly = true;
	IDRenamed = NameIDEdit->Text;
  	IDOk->Enabled = true;
    IDSuppr->Enabled = false;
    IDRename->Enabled = false;
    IDNew->Enabled = false;
}
//----------------------------------------------------------------------------//

//=== R�cup�re le n�ID =======================================================//
void __fastcall TIDOutils::BitBtn1Click(TObject *Sender)
{	AnsiString nameID = NameIDEdit->Text;
	iIDSelect = lpIDMap->NumID(nameID);
}
//----------------------------------------------------------------------------//

//=== Aucun n� choisi ========================================================//
void __fastcall TIDOutils::BitBtn2Click(TObject *Sender)
{	iIDSelect = -1;
}
//----------------------------------------------------------------------------//

// Db-Clic sur la s�lection d'un ID ==========================================//
void __fastcall TIDOutils::IDListBoxDblClick(TObject *Sender)
{   // Revient � un Clic
	IDListBoxClick(Sender);
    // Puis � un clic sur OK
    BitBtn1Click(Sender);
    // Puis on referme la fiche
    Close();
}
//----------------------------------------------------------------------------//

////////////////////////// Fin de la classe TIDOUTILS //////////////////////////

