//---------------------------------------------------------------------------
#ifndef fEditLibLevelH
#define fEditLibLevelH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\ComCtrls.hpp>
#include <vcl\ExtCtrls.hpp>
#include <vcl\Buttons.hpp>
#include <vcl\Menus.hpp>
#include <vcl\Dialogs.hpp>
//---------------------------------------------------------------------------
class TLevelLibEditor : public TForm
{
__published:	// Composants gérés par l'EDI
	TListBox *UniversList;
	TTabControl *TabControlUnivers;
	TTabControl *TabControlSolTile;
	TTabControl *TabControlDecorsTile;
	TLabel *Label7;
	TLabel *NbTileLabel;
	TImage *TileImage;
	TLabel *Label8;
	TLabel *ImageX;
	TLabel *Label9;
	TLabel *ImageY;
	TLabel *Label1;
	TLabel *MilieuName;
	TBevel *Bevel1;
	TBevel *Bevel2;
	TListBox *TypeTileList;
	TBitBtn *AjoutTileBtn;
	TBitBtn *SupprTileBtn;
	TLabel *Label2;
	TLabel *NbTileLevelLabel;
	TPopupMenu *PopupMenu1;
	TMenuItem *AjouterTileLevel;
	TPopupMenu *PopupMenu2;
	TMenuItem *SupprimerduLevel1;
	TButton *AjoutAllBtn;
	TBitBtn *SupprOneCol;
	TBevel *Bevel3;
	TLabel *Label3;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *Label6;
	TMainMenu *MainMenuELL;
	TMenuItem *LibrairieNiveau1;
	TMenuItem *SauverLib1;
	TMenuItem *ChargeLib1;
	TMenuItem *SauverLib2;
	TMenuItem *N1;
	TMenuItem *Quitter1;
	TOpenDialog *OpenFile;
	TEdit *LevelName;
	void __fastcall FormShow(TObject *Sender);

	void __fastcall UniversListClick(TObject *Sender);
	void __fastcall TabControlSolTileChange(TObject *Sender);
	void __fastcall TabControlDecorsTileChange(TObject *Sender);
	void __fastcall TabControlUniversChange(TObject *Sender);
	void __fastcall UniversListDblClick(TObject *Sender);
	void __fastcall SupprTileBtnClick(TObject *Sender);
	
	void __fastcall TypeTileListClick(TObject *Sender);

	
	void __fastcall AjoutAllBtnClick(TObject *Sender);
	void __fastcall SupprOneColClick(TObject *Sender);
	
	void __fastcall SauverLib2Click(TObject *Sender);
	
	void __fastcall ChargeLib1Click(TObject *Sender);
	void __fastcall SauverLib1Click(TObject *Sender);
	
	void __fastcall Quitter1Click(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	
private:	// Déclarations de l'utilisateur
public:		// Déclarations de l'utilisateur
	__fastcall TLevelLibEditor(TComponent* Owner);

    // Change la liste
    void __fastcall ChangeListUnivers(int NewNumTypeUnivers, int NewNumTypeTile);
    void __fastcall ChangeListNivTile(int NewNumTypeTile);
};
//---------------------------------------------------------------------------
extern TLevelLibEditor *LevelLibEditor;
//---------------------------------------------------------------------------
#endif
