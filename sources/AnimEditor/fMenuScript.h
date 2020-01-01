//---------------------------------------------------------------------------
#ifndef fMenuScriptH
#define fMenuScriptH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\ComCtrls.hpp>
#include <vcl\Menus.hpp>
#include <vcl\Dialogs.hpp>
//---------------------------------------------------------------------------
class TFormScriptEditor : public TForm
{
__published:	// Composants gérés par l'EDI
	TRichEdit *RichEditScript;
	TMainMenu *ScriptMenu;
	TMenuItem *FichierScript;
	TMenuItem *ScrNew;
	TMenuItem *FichierAnimation1;
	TMenuItem *ScrSave;
	TMenuItem *ScrSaveAs;
	TSaveDialog *SaveDialogScriptAs;
	TMenuItem *N1;
	TMenuItem *N2;
	TMenuItem *ScrWizard;
	TMenuItem *ScrWizardAnimation;
        TMenuItem *Ouvrir1;
        TMenuItem *OuvrirScriptAnimation;
        TOpenDialog *DialogOuvrirScript;
        TMenuItem *ScriptNain1;
        TMenuItem *Hero1;
        TMenuItem *Ogre1;
        TMenuItem *Nain1;
        TMenuItem *OuvrirScript1;
        TMenuItem *Chercher1;
        TMenuItem *N3;
        TMenuItem *Pig1;
        TMenuItem *Bocal1;
	TMenuItem *Ouvrir2;
	void __fastcall FichierAnimation1Click(TObject *Sender);

	void __fastcall ScrSaveAsClick(TObject *Sender);

	void __fastcall ScrWizardAnimationClick(TObject *Sender);

	void __fastcall FichierScriptClick(TObject *Sender);

	void __fastcall ScrSaveClick(TObject *Sender);
        void __fastcall OuvrirScriptAnimationClick(TObject *Sender);
        void __fastcall Hero1Click(TObject *Sender);
        void __fastcall Ogre1Click(TObject *Sender);
        void __fastcall Nain1Click(TObject *Sender);
        void __fastcall Chercher1Click(TObject *Sender);
        void __fastcall Pig1Click(TObject *Sender);
        void __fastcall Bocal1Click(TObject *Sender);
private:	// Déclarations de l'utilisateur
public:		// Déclarations de l'utilisateur
	__fastcall TFormScriptEditor(TComponent* Owner);
        void __fastcall OpenAndLoadScript(AnsiString ScriptName);
        void __fastcall OpenAndLoadScriptDecors(AnsiString ScriptName);
        void __fastcall ShowFile(AnsiString filename);    // Charge le fichier dans le RichEdit 
};
//---------------------------------------------------------------------------
extern TFormScriptEditor *FormScriptEditor;
extern int typePERSO;

//---------------------------------------------------------------------------
#endif
