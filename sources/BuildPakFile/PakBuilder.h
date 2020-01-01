//---------------------------------------------------------------------------
#ifndef PakBuilderH
#define PakBuilderH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "PakFile.h"
#include <Menus.hpp>
#include <Dialogs.hpp>
#include "INILib.h"
#include <ComCtrls.hpp>

//---------------------------------------------------------------------------
class TPakBuilderForm : public TForm
{
__published:	// Composants g�r�s par l'EDI
	TMainMenu *PakBuilderMenu;
	TMenuItem *File1;
	TMenuItem *Nouveau1;
	TMenuItem *Ouvrir1;
	TMenuItem *SauverIni;
	TMenuItem *N1;
	TMenuItem *Quitter1;
	TMenuItem *Pak1;
	TMenuItem *Build1;
	TOpenDialog *PakFileOpen;
	TMemo *PakFileMemo;
	TStatusBar *PakStatusBar;
	void __fastcall Ouvrir1Click(TObject *Sender);
	void __fastcall SauverIniClick(TObject *Sender);
	void __fastcall PakFileMemoChange(TObject *Sender);
	void __fastcall Build1Click(TObject *Sender);
private:	// D�clarations de l'utilisateur
public:		// D�clarations de l'utilisateur
    __fastcall TPakBuilderForm(TComponent* Owner);

	TPakFile PakFile;		// Instance de la classe PakFile
    TINILib  IniFile;		// Fichier .ini �crivant le .pak
  	AnsiString iniFileName; // Nom de ce .ini
    bool bIniChanged;		// Indique si le .ini a �t� modifi�
};
//---------------------------------------------------------------------------
extern PACKAGE TPakBuilderForm *PakBuilderForm;
extern AnsiString CheminApplication;
//---------------------------------------------------------------------------
#endif
