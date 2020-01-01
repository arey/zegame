//---------------------------------------------------------------------------
#ifndef fMenuIDNameH
#define fMenuIDNameH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Buttons.hpp>
//---------------------------------------------------------------------------
class TFormIdentifier : public TForm
{
__published:	// Composants g�r�s par l'EDI
	TLabel *Label;
	TEdit *EditIDName;
	TBitBtn *BitBtnOK;
	TBitBtn *BitBtnAnnuler;
	void __fastcall BitBtnOKClick(TObject *Sender);
	void __fastcall BitBtnAnnulerClick(TObject *Sender);
private:	// D�clarations de l'utilisateur
public:		// D�clarations de l'utilisateur
	__fastcall TFormIdentifier(TComponent* Owner);

    bool sortieOK;
};
//---------------------------------------------------------------------------
extern TFormIdentifier *FormIdentifier;
//---------------------------------------------------------------------------
#endif
