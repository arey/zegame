//---------------------------------------------------------------------------
#ifndef fPersosOutilsH
#define fPersosOutilsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TFormPersosOutils : public TForm
{
__published:	// Composants g�r�s par l'EDI
    TLabel *NumeroPerso;
    TLabel *LabelDesignNumber;
    TLabel *Label1;
    TLabel *LabelType;
    TLabel *LabelParam1;
    TLabel *LabelParam2;
    TLabel *LabelParam1Text;
    TLabel *LabelParam2Text;
    TLabel *LabelDesignerNomZone;
    TLabel *NomPerso;
private:	// D�clarations de l'utilisateur
public:		// D�clarations de l'utilisateur
    __fastcall TFormPersosOutils(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormPersosOutils *FormPersosOutils;
//---------------------------------------------------------------------------
#endif
