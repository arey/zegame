//---------------------------------------------------------------------------
#ifndef fMapOutilsH
#define fMapOutilsH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Buttons.hpp>
//---------------------------------------------------------------------------
class TMapOutils : public TForm
{
__published:	// Composants gérés par l'EDI
	TGroupBox *GroupBoxDonnes;
	TGroupBox *GroupBoxBoutons;
	TEdit *MapName;
	TEdit *MapNumber;
	TLabel *Label1;
	TLabel *Label2;
	TEdit *MapWidth;
	TEdit *MapHeight;
	TLabel *Label3;
	TLabel *Taille;
	TLabel *Label4;
	TLabel *Label5;
	TEdit *SourisX;
	TEdit *SourisY;
	TLabel *Label6;
	TEdit *SourisU;
	TLabel *Label7;
	TEdit *SourisV;
	TBitBtn *PlacerObjBitBtn;
	TBitBtn *BitBtnSelect;
	TBitBtn *BitBtnSuppr;
	TBitBtn *BitBtnBigScreen;
	TBitBtn *BitBtnTrapPrinc;
	TBitBtn *PeindreBitBtn;
	TBitBtn *GommeBitBtn;
	void __fastcall FormCreate(TObject *Sender);
	
	
	
	
	
	
	void __fastcall PlacerObjBitBtnClick(TObject *Sender);
	
	void __fastcall BitBtnSelectClick(TObject *Sender);
	void __fastcall BitBtnSupprClick(TObject *Sender);
	void __fastcall GroupBoxDonnesMouseMove(TObject *Sender, TShiftState Shift,
	int X, int Y);
	void __fastcall GroupBoxBoutonsMouseMove(TObject *Sender, TShiftState Shift,
	int X, int Y);
	
	void __fastcall BitBtnBigScreenClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);

	
	void __fastcall BitBtnTrapPrincClick(TObject *Sender);
	void __fastcall PeindreBitBtnClick(TObject *Sender);
	void __fastcall GommeBitBtnClick(TObject *Sender);
private:	// Déclarations de l'utilisateur
public:		// Déclarations de l'utilisateur
	__fastcall TMapOutils(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern TMapOutils *MapOutils;
//---------------------------------------------------------------------------
#endif
