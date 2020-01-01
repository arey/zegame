////////////////////////////////////////////////////////////////////////////////
//  Unit� "fIDUnits.h" interface de la classe TIDOutils 					  //
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


#ifndef fIDOutilsH
#define fIDOutilsH

// --------------------------------------------------------------------- include
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "IDMap.h"
#include <Buttons.hpp>			// Librairie de gestion des ID

class IDMap;

// --------------------------------------------------------------------- classes
class TIDOutils : public TForm
{
// -------------------------------------------------- Composants g�r�s par l'EDI
__published:

// ------------------------------------------------------------ Attributs priv�s
	TListBox *IDListBox;
	TLabel *Label1;
	TEdit *NumIDEdit;
	TButton *IDNew;
	TEdit *NameIDEdit;
	TButton *IDRename;
	TButton *IDSuppr;
	TButton *IDOk;
	TBitBtn *BitBtn1;
	TBitBtn *BitBtn2;
	void __fastcall IDListBoxClick(TObject *Sender);
	void __fastcall IDOkClick(TObject *Sender);
	
	void __fastcall IDNewClick(TObject *Sender);
	
	void __fastcall IDRenameClick(TObject *Sender);
	
	void __fastcall BitBtn1Click(TObject *Sender);
	void __fastcall BitBtn2Click(TObject *Sender);
	void __fastcall IDListBoxDblClick(TObject *Sender);
private:
    std::IDMap*		lpIDMap;		// Librairie d'ID actuellement �dit�e
    int			iIDSelect;		// n�ID actuellement s�lectionn�
    enum { IDMODE_NONE, IDMODE_NEW, IDMODE_RENAME };
    AnsiString	IDRenamed;		// Nom de l'ID en cours de renomage
    int			IDMode;

// ------------------------------------------------------------ M�thodes priv�es
    void RefreshIDList();		// MaJ de la liste des nom d'ID

// ---------------------------------------------------------- M�thodes publiques
public:
	__fastcall TIDOutils(TComponent* Owner);		// Constructeur
    inline int GetIDSelect() { return iIDSelect; }  // Retourne le n� dID s�lectionn�
    void LetsGo(std::IDMap* map);						// Lance la fen�tre
};

// ---------------------------------------------------------- variables externes
extern PACKAGE TIDOutils *IDOutils;


#endif // fIDOutilsH
