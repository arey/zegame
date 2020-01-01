//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "PakBuilder.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TPakBuilderForm *PakBuilderForm;
AnsiString CheminApplication;

//---------------------------------------------------------------------------
__fastcall TPakBuilderForm::TPakBuilderForm(TComponent* Owner)
    : TForm(Owner)
{	bIniChanged = false;
	iniFileName = "";
}
//----------------------------------------------------------------------------//

//=== Ouvre un fichier TryPak.ini ============================================//
void __fastcall TPakBuilderForm::Ouvrir1Click(TObject *Sender)
{   // Récupère le nom du .ini à ouvrir
//	PakFileOpen->InitialDir = "ZeGame\\";
    PakFileOpen->Execute();
	iniFileName = PakFileOpen->FileName;
    bool cdru = IniFile.LoadFile(iniFileName.c_str());
    if (cdru==false)
    {	Application->MessageBox("Erreur lors de l'ouverture du fichier", "Warning !", 0);
    	return;
    }

    // Affiche le .ini dans le memo
    PakFileMemo->Clear();

//    PakFileMemo->Lines->LoadFromFile(iniFileName);
    PakFileMemo->ReadOnly = false;
    SauverIni->Enabled = true;

    bIniChanged = false;
}
//----------------------------------------------------------------------------//

//=== Sauve le fichier TryPak.ini ============================================//
void __fastcall TPakBuilderForm::SauverIniClick(TObject *Sender)
{	// Sauve le contenu du mémo dans le .ini
    PakFileMemo->Lines->SaveToFile(iniFileName);
    bIniChanged = false;
}
//----------------------------------------------------------------------------//

//=== .ini modifié ===========================================================//
void __fastcall TPakBuilderForm::PakFileMemoChange(TObject *Sender)
{   bIniChanged = true;
}
//----------------------------------------------------------------------------//

//=== Construit le .zgs correspondant ========================================//
void __fastcall TPakBuilderForm::Build1Click(TObject *Sender)
{   // Vérifie si un fichier a déjà été ouvert
	if (iniFileName=="")
	{	Application->MessageBox("Aucun fichier d'ouvert", "Erreur ...", 0);
    	return;
    }
	// Vérifie si le fichier n'a pas été modifié depuis son ouverture
	if (bIniChanged)
    {	int cdru = Application->MessageBox("Voulez-vous le sauver avant de créer le .zgs", "Ce fichier a été modifié !", 3);
    	if (cdru == IDYES)
        {	PakFileMemo->Lines->SaveToFile(iniFileName);
		    bIniChanged = false;
        } else
        if (cdru == IDCANCEL) return;
    }

    PakStatusBar->SimpleText = "Fichier pak en cours de création ...";
    PakFile.CreateZgp(iniFileName.c_str());
    AnsiString pakFileName = iniFileName;
    pakFileName.SetLength(pakFileName.Length()-4);
    pakFileName += ".zgp";
    PakFile.PakWrite(pakFileName.c_str());
    PakStatusBar->SimpleText = "Le fichier " + pakFileName + " a été créé";
}
//----------------------------------------------------------------------------//

