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
{   // R�cup�re le nom du .ini � ouvrir
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
{	// Sauve le contenu du m�mo dans le .ini
    PakFileMemo->Lines->SaveToFile(iniFileName);
    bIniChanged = false;
}
//----------------------------------------------------------------------------//

//=== .ini modifi� ===========================================================//
void __fastcall TPakBuilderForm::PakFileMemoChange(TObject *Sender)
{   bIniChanged = true;
}
//----------------------------------------------------------------------------//

//=== Construit le .zgs correspondant ========================================//
void __fastcall TPakBuilderForm::Build1Click(TObject *Sender)
{   // V�rifie si un fichier a d�j� �t� ouvert
	if (iniFileName=="")
	{	Application->MessageBox("Aucun fichier d'ouvert", "Erreur ...", 0);
    	return;
    }
	// V�rifie si le fichier n'a pas �t� modifi� depuis son ouverture
	if (bIniChanged)
    {	int cdru = Application->MessageBox("Voulez-vous le sauver avant de cr�er le .zgs", "Ce fichier a �t� modifi� !", 3);
    	if (cdru == IDYES)
        {	PakFileMemo->Lines->SaveToFile(iniFileName);
		    bIniChanged = false;
        } else
        if (cdru == IDCANCEL) return;
    }

    PakStatusBar->SimpleText = "Fichier pak en cours de cr�ation ...";
    PakFile.CreateZgp(iniFileName.c_str());
    AnsiString pakFileName = iniFileName;
    pakFileName.SetLength(pakFileName.Length()-4);
    pakFileName += ".zgp";
    PakFile.PakWrite(pakFileName.c_str());
    PakStatusBar->SimpleText = "Le fichier " + pakFileName + " a �t� cr��";
}
//----------------------------------------------------------------------------//

