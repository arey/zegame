//---------------------------------------------------------------------------
#ifndef fWorkStationH
#define fWorkStationH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Buttons.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TWokStation : public TForm
{
__published:	// Composants gérés par l'EDI
	TBitBtn *BtnLoadScript;
	TComboBox *CBListAnims;
        TLabel *Label2;
        TGroupBox *GroupBox1;
        TLabel *Label3;
        TButton *ButtonZoneAuSol;
        TSpeedButton *ShowZoneAuSol;
        TComboBox *CBListTypePersos;
        TLabel *LabelTypePersosObjets;
        TLabel *Label4;
        TButton *ButtonGravAuSol;
        TSpeedButton *ShowGravAuSol;
        TGroupBox *GroupBox2;
        TListBox *ListBoxMembres;
        TLabel *Label5;
        TComboBox *CBTypeFrame;
        TLabel *Label6;
        TRadioGroup *RGTypePos;
        TBitBtn *AnimSupepPosit;
        TLabel *Label7;
        TLabel *LabelNbFrame;
        TLabel *Label8;
        TLabel *LabelPosition;
        TSpeedButton *ShowZoneAuSolPrec;
        TBitBtn *BitBtnInfos;
        TLabel *Label1;
        TSpeedButton *ShowTrapPrinc;
        TLabel *Label9;
        TButton *ButtonZonePied;
        TSpeedButton *ShowZonePied;
        TSpeedButton *ShowZonePiedPrec;
	TGroupBox *GroupBoxAnimsClassqiues;
	TListBox *ListBoxAnims;
	TLabel *Label10;
	TButton *BtnPositionnerAnim;
	TLabel *Label11;
	TLabel *Label12;
	TLabel *LabelAnimX;
	TLabel *LabelAnimY;
	TButton *BtnPlayAnimClassik;
	void __fastcall BtnLoadScriptClick(TObject *Sender);
	
	void __fastcall CBListAnimsChange(TObject *Sender);
        void __fastcall ButtonZoneAuSol2Click(TObject *Sender);
        
        void __fastcall CBListTypePersos2Change(TObject *Sender);
        
        
        
        
        void __fastcall ButtonGravAuSolClick(TObject *Sender);
        
        void __fastcall ListBoxMembresClick(TObject *Sender);
        void __fastcall CBTypeFrameChange(TObject *Sender);
        void __fastcall AnimSupepPositClick(TObject *Sender);

        void __fastcall BitBtnInfosClick(TObject *Sender);
        
        
        
        void __fastcall ButtonZonePiedClick(TObject *Sender);
        
        
	
	
	void __fastcall ListBoxAnimsClick(TObject *Sender);
	void __fastcall BtnPositionnerAnimClick(TObject *Sender);
	
	void __fastcall BtnPlayAnimClassikClick(TObject *Sender);
private:

public:		// Déclarations de l'utilisateur
	__fastcall TWokStation(TComponent* Owner);

        void __fastcall CalculeEtatPerso();
        void __fastcall CalculeEtatDecors();

        enum eGBSpriteInfos { GBSI_EMPTY, GBSI_PERSO, GBSI_DECORS };
        eGBSpriteInfos GBSpriteInfos;         // Indique ds quel état se trouve le Groupe-Box des sprites-infos
        void __fastcall MaJGBSpriteInfos();   // met à jour le contenu du GroupeBox spriteInfos
        void __fastcall MaJCBTypeFrame();     // met à jour le ComboBox CBTypeFrame
};
//---------------------------------------------------------------------------
extern TWokStation *WokStation;
//---------------------------------------------------------------------------
#endif
