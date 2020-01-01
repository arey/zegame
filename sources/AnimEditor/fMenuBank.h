//---------------------------------------------------------------------------
#ifndef fMenuBankH
#define fMenuBankH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\ComCtrls.hpp>
#include <vcl\Buttons.hpp>
#include "DDLibEditor.h"

//---------------------------------------------------------------------------
class TMenuNewBank : public TForm
{
__published:	// Composants gérés par l'EDI
	TGroupBox *GroupBox1;
	TRadioButton *Point1;
	TRadioButton *Point8;
	TRichEdit *RichEdit1;
	TLabel *Label1;
	TEdit *ETName;
	TLabel *Label2;
	TEdit *ETNbFrame;
	TLabel *Label3;
	TEdit *ETColFond;
	TEdit *ETcf_R;
	TEdit *ETcf_G;
	TEdit *ETcf_B;
	TEdit *ETco_B;
	TEdit *ETco_G;
	TEdit *ETco_R;
	TEdit *ETColOmb;
	TLabel *Label4;
	TLabel *Label9;
	TLabel *Label5;
	TEdit *ETFirst;
	TBitBtn *BitBtn1;
	TBitBtn *BitBtn2;
	TLabel *Label7;
	TEdit *ETBmp;
        TCheckBox *CheckBoxSpaceOptimization;
	TRadioButton *RadioButton1;
	TRadioButton *RadioButton2;
	TRadioButton *RadioButton3;
	TLabel *Label6;
	TCheckBox *CBTexture3D;
	void __fastcall BitBtn1Click(TObject *Sender);
private:	// Déclarations de l'utilisateur
public:		// Déclarations de l'utilisateur
	// informations sur la banque
	FrInfo  FrameInfo;

	// informations relatives à chaque Sprite de la banque
	SprInfo SpriteInfo;

	__fastcall TMenuNewBank(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern TMenuNewBank *MenuNewBank;

    // Type de lecture des BMP
	enum eBmpType { CARS_8, ZEROS_2, ZEROS_3 };
	extern eBmpType  BmpLectureType;

    extern int indiceSpriteIni;


//---------------------------------------------------------------------------
#endif
