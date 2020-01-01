//---------------------------------------------------------------------------
#ifndef fObjOutilsH
#define fObjOutilsH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\ExtCtrls.hpp>
#include <vcl\ComCtrls.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TObjOutils : public TForm
{
__published:	// Composants gérés par l'EDI
	TImage *DecObjImage;
	TImage *DecObjShow;
	TEdit *DecorsHeight;
	TLabel *Label6;
	TEdit *DecorsWidth;
	TLabel *Label5;
	TLabel *Label10;

	TLabel *Numero;
	TLabel *NbTrapeze;
	TLabel *Label1;
	TRichEdit *RichEdit1;
        TGroupBox *GBAnimation;
        TLabel *Label2;
        TLabel *LabelEtat;
        TLabel *Label3;
        TLabel *LabelNumFrame;
        TComboBox *CBEtatCourant;
	TCheckBox *CBGameObjectKnown;
	TLabel *LabelYo;
	TLabel *GameObjectID;
	TBitBtn *BitBtnChooseGameObjectID;
	TLabel *LabelNameID;
	TLabel *GameObjectNameID;
	TLabel *Label4;
	TLabel *ScriptID;
	TLabel *ScriptNameID;
	TBitBtn *BitBtnChooseScriptID;
	TBitBtn *BitBtnEditScript;
	void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
	void __fastcall FormShow(TObject *Sender);
        void __fastcall CBEtatCourantChange(TObject *Sender);
	
	
	void __fastcall BitBtnChooseGameObjectIDClick(TObject *Sender);
	void __fastcall CBGameObjectKnownClick(TObject *Sender);
	void __fastcall BitBtnChooseScriptIDClick(TObject *Sender);
private:	// Déclarations de l'utilisateur
public:		// Déclarations de l'utilisateur
	void __fastcall UpgradeFicheObj();	     // permet de rénover la fiche avec un nouvel objet
        void __fastcall Update();                    // MaJ des paramètres de l'objet sélectionné
	__fastcall TObjOutils(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern TObjOutils *ObjOutils;
//---------------------------------------------------------------------------
#endif
