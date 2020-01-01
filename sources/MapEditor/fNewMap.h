//---------------------------------------------------------------------------
#ifndef fNewMapH
#define fNewMapH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
//---------------------------------------------------------------------------
class TNewCarte : public TForm
{
__published:	// Composants gérés par l'EDI
	TEdit *MapName;
	TEdit *MapNumber;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TEdit *MapWidth;
	TEdit *MapHeight;
	TLabel *Label4;
	TLabel *Label5;
	TButton *NewMapBtn;
	void __fastcall NewMapBtnClick(TObject *Sender);
	
	void __fastcall FormShow(TObject *Sender);
private:	// Déclarations de l'utilisateur
public:		// Déclarations de l'utilisateur
	__fastcall TNewCarte(TComponent* Owner);

    bool bMapCreated;	// Indicate if a map has been created
    inline bool isMapCreated() { return bMapCreated; }
};
//---------------------------------------------------------------------------
extern TNewCarte *NewCarte;
//---------------------------------------------------------------------------
#endif
