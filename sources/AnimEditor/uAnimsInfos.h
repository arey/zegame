//---------------------------------------------------------------------------
#ifndef uAnimsInfosH
#define uAnimsInfosH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TAnimsInfos : public TForm
{
__published:	// Composants g�r�s par l'EDI
        TComboBox *CBAnimAssoc;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *LabelX;
        TLabel *LabelY;
        TEdit *EditdY;
        TEdit *EditdX;
        TLabel *Label5;
        TLabel *LabelTileDecors;
        TButton *ButtonNew;
        TButton *ButtonPos;
        TStatusBar *SBHelp;
        TButton *ButtonOk;
        TLabel *LabeldX;
        TLabel *LabeldY;
        TLabel *LabeldX2;
        TLabel *LabeldY2;
        void __fastcall FormShow(TObject *Sender);
        void __fastcall CBAnimAssocChange(TObject *Sender);
        void __fastcall ButtonPosClick(TObject *Sender);
        
        
        
        void __fastcall ButtonOkClick(TObject *Sender);
        void __fastcall ButtonNewClick(TObject *Sender);
private:	// D�clarations de l'utilisateur
public:		// D�clarations de l'utilisateur
        bool infosLoaded;      // Indique si les infos son charg�es oui ou non
        int numTileDecors;     // n� du TileDecors � afficher
        int numAnimTileDecors; // n� de l'anim ki poss�de ce TileDecors 
        void RegenScreen();    // R�g�n�re les infos de la fiche


        __fastcall TAnimsInfos(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TAnimsInfos *AnimsInfos;
//---------------------------------------------------------------------------
#endif
