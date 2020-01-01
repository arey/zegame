//---------------------------------------------------------------------------
#ifndef fMenuBankGrilleH
#define fMenuBankGrilleH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\ExtCtrls.hpp>
#include <vcl\Buttons.hpp>
//---------------------------------------------------------------------------
class TFormImageGrille : public TForm
{
__published:	// Composants gérés par l'EDI
	TImage *Grille;
	TBitBtn *BitBtn1;
	
	void __fastcall BitBtn1Click(TObject *Sender);
private:	// Déclarations de l'utilisateur
public:		// Déclarations de l'utilisateur
	__fastcall TFormImageGrille(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern TFormImageGrille *FormImageGrille;
//---------------------------------------------------------------------------
#endif
