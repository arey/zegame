//---------------------------------------------------------------------------
#ifndef fDesignOutilsH
#define fDesignOutilsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>

enum
{	DESIGN_LEVEL_NONE = 0,
	DESIGN_LEVEL_ZONES,
    DESIGN_LEVEL_CHEMINS
};

enum
{   DESIGN_LEVEL_MODE_NONE = 0,
	DESIGN_LEVEL_MODE_ZONES_CREATION = 1,
    DESIGN_LEVEL_MODE_ZONES_SELECTION = 2,
	DESIGN_LEVEL_MODE_ZONES_TRACER = 4
};


class MapZone;

//---------------------------------------------------------------------------
class TFormDesignOutils : public TForm
{
__published:	// Composants g�r�s par l'EDI
	TLabel *NumeroZone;
	TLabel *LabelDesignNumber;
	TPanel *PanelElements;
	TLabel *LabelBarreTitle;
	TLabel *NameElement;
	TLabel *Label5;
	TLabel *NumElement;
	TBitBtn *BitBtnRecul;
	TBitBtn *BitBtnAvance;
	TLabel *Label1;
	TStatusBar *StatusBarLevelDesign;
	TLabel *LabelType;
	TLabel *LabelParam1;
	TLabel *LabelParam2;
	TLabel *LabelParam1Text;
	TLabel *LabelParam2Text;
	TLabel *LabelParam3;
	TLabel *LabelParam3Text;
	TLabel *LabelParam4Text;
	TLabel *LabelParam4;
	TGroupBox *GBZoneCreate;
	TBitBtn *BitBtnChooseGameObjectID;
	TBitBtn *BitBtnTracerZone;
	TBitBtn *BtnNewZone;
	TRadioButton *RBZoneCircle;
	TRadioButton *RBZoneRect;
	TCheckBox *CBTracerZone;
	TCheckBox *CBChoisirNumero;
	TBitBtn *BtnCreeZoneAnnuler;
	TLabel *LabelDesignerNomZone;
	TLabel *NomZone;
	TBitBtn *BtnCreerZone;
	TGroupBox *GBVisualisation;
	TSpeedButton *SBtnAllZone;
	TSpeedButton *SBtnNoZone;
	TSpeedButton *SBtnOneZone;
	TGroupBox *GBContoles;
	TUpDown *PosZoneX;
	TUpDown *PosZoneY;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label4;
	TBitBtn *BtnSupprimerZone;
	TButton *BtnSelectZone;
	TGroupBox *GBObjetsAssocies;
	TLabel *Label6;
	TListBox *ListBoxObjetsAssocies;
	TBitBtn *BtnSelectObject;
	TBitBtn *BtnSupprObject;
	TLabel *Label7;
	void __fastcall BtnNewZoneClick(TObject *Sender);
	void __fastcall BitBtnTracerZoneClick(TObject *Sender);
	

	void __fastcall BtnCreeZoneAnnulerClick(TObject *Sender);
	void __fastcall BitBtnChooseGameObjectIDClick(TObject *Sender);
	void __fastcall BtnCreerZoneClick(TObject *Sender);
	void __fastcall BitBtnReculClick(TObject *Sender);
	void __fastcall BitBtnAvanceClick(TObject *Sender);
	
	
	
	void __fastcall PosZoneXClick(TObject *Sender, TUDBtnType Button);
	void __fastcall PosZoneYClick(TObject *Sender, TUDBtnType Button);
	void __fastcall BtnSupprimerZoneClick(TObject *Sender);
	void __fastcall BtnSelectZoneClick(TObject *Sender);
	
	void __fastcall BtnSelectObjectClick(TObject *Sender);
	void __fastcall ListBoxObjetsAssociesClick(TObject *Sender);
	void __fastcall BtnSupprObjectClick(TObject *Sender);
private:	// D�clarations de l'utilisateur
public:		// D�clarations de l'utilisateur
	__fastcall TFormDesignOutils(TComponent* Owner);
    void __fastcall UpdateView();		// MaJ de la barre de LevelDesign
    void __fastcall UpdateZoneInfos();	// Affiche les informations de la zone s�lectionn�e
    void __fastcall UpdateControles();	// MaJ des contr�les
    void __fastcall UpdateListeObjets();// MaJ de la liste des objets associ�s � la zone
    void __fastcall AddObjectToZone(DecorsObjet* decors);	// Ajoute cet object � la zone

    int iModeDesignLevel;				// Mode de fonctionnement
    int iTypeDesignLevel;				// Type d'objets sur lequel on est en train de bosser : chemin, zone, ...
	int iObjSelected;					// N� de l'objet s�lectionn�
    MapZone* pMapZone;					// Ptr sur une zone s�lectionn�e
};
//---------------------------------------------------------------------------
extern PACKAGE TFormDesignOutils *FormDesignOutils;
//---------------------------------------------------------------------------
#endif
