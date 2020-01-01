//---------------------------------------------------------------------------
#ifndef fEditLibDecorsH
#define fEditLibDecorsH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\ExtCtrls.hpp>
#include <vcl\FileCtrl.hpp>
#include <vcl\Menus.hpp>
#include <vcl\Dialogs.hpp>
//---------------------------------------------------------------------------
class TEditLibDecors : public TForm
{
__published:	// Composants gérés par l'EDI
	TListBox *TileList;
	TLabel *Label7;
	TEdit *TileLibName;
	TImage *TileImage;
	TFileListBox *FileListDecors;
	TLabel *NbTileLabel;
	TGroupBox *GroupBox1;
	TButton *TileSupr;
	TButton *TileAdd;
	TLabel *ImageY;
	TLabel *Label9;
	TLabel *ImageX;
	TLabel *Label8;
	TMainMenu *MainMenu1;
	TMenuItem *Fichier1;
	TMenuItem *NouvelleLib1;
	TMenuItem *ChargerLib1;
	TMenuItem *SauverLib1;
	TOpenDialog *OpenFile;
	TLabel *Label1;
	TButton *Button1;
	TLabel *Label2;
	TButton *Button2;
	TButton *Button3;
	TMenuItem *Quitter1;
	TLabel *Label3;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FileListDecorsChange(TObject *Sender);

	void __fastcall TileAddClick(TObject *Sender);

	void __fastcall SauverLib1Click(TObject *Sender);
	void __fastcall ChargerLib1Click(TObject *Sender);
	void __fastcall NouvelleLib1Click(TObject *Sender);
	void __fastcall TileListClick(TObject *Sender);
	void __fastcall TileSuprClick(TObject *Sender);
	void __fastcall Quitter1Click(TObject *Sender);
private:	// Déclarations de l'utilisateur
public:		// Déclarations de l'utilisateur
	__fastcall TEditLibDecors(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern TEditLibDecors *EditLibDecors;
//---------------------------------------------------------------------------
#endif
