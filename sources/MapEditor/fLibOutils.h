//---------------------------------------------------------------------------
#ifndef fLibOutilsH
#define fLibOutilsH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Buttons.hpp>
#include <vcl\ExtCtrls.hpp>
#include <vcl\Dialogs.hpp>
#include <vcl\ComCtrls.hpp>
//---------------------------------------------------------------------------
class TLibOutils : public TForm
{
__published:	// Composants gérés par l'EDI
	TBevel *Bevel1;
	TSpeedButton *SpeedButtonFonds;
	TSpeedButton *SpeedButtonPersos;
	TSpeedButton *SpeedButtonEvents;
	TSpeedButton *SpeedButtonDecors;
	TLabel *Label1;
	TLabel *LabelNomLib;
	TEdit *EditLibName;
	TBitBtn *BitBtnOuvrirLib;
	TOpenDialog *OpenDialogLib;
	TLabel *LabelNbSprites;
	TEdit *EditLibNbSprite;
	TPanel *PanelElements;
	TBitBtn *BitBtnRecul;
	TBitBtn *BitBtnAvance;
	TLabel *Label4;
	TLabel *NameElement;
	TLabel *Label5;
	TLabel *NumElement;
	TLabel *Label6;
	TBevel *Bevel2;
	TLabel *Label7;
	TSpeedButton *SpdBtnDecorsVis;
	TSpeedButton *SpdBtnFondsVis;
	TSpeedButton *SpdBtnPersosVis;
	TSpeedButton *SpdBtnEventsVis;
	TTabControl *TabControlSolTile;
	TLabel *LabelTypePlan;
	TTabControl *TabControlDecorsTile;
	TSpeedButton *SpeedButtonDesign;
	TGroupBox *GBDesign;
	TRadioButton *RBZones;
	TRadioButton *RBChemins;
    TGroupBox *GBPersos;
    TTabControl *TabControl1;
    TLabel *Label2;
    TPanel *Panel1;
    TLabel *Label3;
    TLabel *LabelNomPerso;
    TLabel *Label9;
    TLabel *LabelNumPerso;
    TBitBtn *BitBtn1;
    TBitBtn *BitBtn2;
	void __fastcall BitBtnOuvrirLibClick(TObject *Sender);

	void __fastcall SpeedButtonDecorsClick(TObject *Sender);
	
	
	
	void __fastcall BitBtnReculClick(TObject *Sender);
	void __fastcall BitBtnAvanceClick(TObject *Sender);
	void __fastcall BitBtnAvanceMouseMove(TObject *Sender, TShiftState Shift,
	int X, int Y);
	void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall SpdBtnDecorsVisClick(TObject *Sender);
	void __fastcall SpdBtnFondsVisClick(TObject *Sender);
	void __fastcall SpdBtnPersosVisClick(TObject *Sender);
	void __fastcall SpdBtnEventsVisClick(TObject *Sender);
	void __fastcall TabControlDecorsTileChange(TObject *Sender);
	void __fastcall TabControlSolTileChange(TObject *Sender);
private:	// Déclarations de l'utilisateur
public:		// Déclarations de l'utilisateur
	__fastcall TLibOutils(TComponent* Owner);
    int TypeLibSelect;		// Type de Librairie sélectionnée :  Lib_DECORS, Lib_FONDS, Lib_PERSOS, Lib_EVENTS
    BOOL DecorsShow, FondShow, PersosShow, EventsShow;	// Vue des différents niveaux à l'écran
};
//---------------------------------------------------------------------------
extern TLibOutils *LibOutils;
extern int NumDecorsSelect;	// n° ds la lib du décors sélectionné
//---------------------------------------------------------------------------
#endif
