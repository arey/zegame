//---------------------------------------------------------------------------
#ifndef EditorH
#define EditorH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Menus.hpp>
#include <vcl\Dialogs.hpp>
#include "IDMap.h"

//---------------------------------------------------------------------------
class TEditeur : public TForm
{
__published:	// Composants gérés par l'EDI
	
	TMainMenu *EditorMenu;
	TMenuItem *Fichiers1;
	TMenuItem *NouvelleCarte1;
	
	TMenuItem *Librairies1;
	TMenuItem *Editerlibrairiedesdcors1;
	TMenuItem *SauverCarte1;
	TMenuItem *OuvrirCarte1;
	TOpenDialog *OpenMap;
	TMenuItem *Preferenc1;
	TMenuItem *Affichage1;
	TMenuItem *VueCarte;
	TMenuItem *N1;
	TMenuItem *Quitter1;
	TMenuItem *LibduLevel1;
        TMenuItem *Outils1;
        TMenuItem *MenuPathFinding;
	void __fastcall NouvelleCarte1Click(TObject *Sender);
	
	
	void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
	
	void __fastcall Editerlibrairiedesdcors1Click(TObject *Sender);
	
	void __fastcall SauverCarte1Click(TObject *Sender);
	void __fastcall OuvrirCarte1Click(TObject *Sender);
	void __fastcall VueCarteClick(TObject *Sender);
	
	void __fastcall Preferenc1Click(TObject *Sender);
	void __fastcall Quitter1Click(TObject *Sender);
	
	void __fastcall LibduLevel1Click(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
        
        
        void __fastcall MenuPathFindingClick(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// Déclarations de l'utilisateur
public:		// Déclarations de l'utilisateur
	__fastcall TEditeur(TComponent* Owner);

    bool levelLoaded;        // Vrai lorsqu'un niveau est charge
};
//---------------------------------------------------------------------------
extern TEditeur *Editeur;
extern AnsiString CheminEditor;

extern int ConfigType;								// type de config visuelles
extern TColor ConfigBoxColor;							// couleur des boîtes de textes
extern TColor ConfigBoxColor2;							// couleur des boîtes de textes


//---------------------------------------------------------------------------
#endif
