//---------------------------------------------------------------------------
#ifndef TileDecorsH
#define TileDecorsH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\ExtCtrls.hpp>
#include <vcl\Dialogs.hpp>
#include <vcl\Menus.hpp>
#include <vcl\ComCtrls.hpp>
#include <vcl\Buttons.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// Composants gérés par l'EDI
	TImage *ImageGrille;
	TEdit *gCoordX;
	TEdit *gCoordY;
	TEdit *gTrapU;
	TEdit *gTrapV;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label4;
	TGroupBox *GroupBox1;
	TImage *ImaDec;
	TButton *PlaceObjBtn;
	TButton *ImageBtn;
	TEdit *ImageName;
	TOpenDialog *SelectDialog;
	TEdit *DecorsWidth;
	TEdit *DecorsHeight;
	TLabel *Label5;
	TLabel *Label6;
	TButton *Button1;
	TLabel *Label7;
	TEdit *CompR;
	TEdit *CompV;
	TEdit *CompB;
	TLabel *Label8;
	TEdit *numPal;
	TLabel *Label9;
	TEdit *Numero;
	TLabel *Label10;
	TGroupBox *GroupBox2;
	TButton *TrapBtn;
	TLabel *Help;
	TEdit *gTrapNb;
	TLabel *Label11;
	TPopupMenu *PopUpColor;
	TMenuItem *FondPurple1;
	TMenuItem *FondBlue1;
	TMenuItem *FondPink1;
	TMenuItem *FondYellow1;
	TMenuItem *FondBlack1;
	TMenuItem *FondGreen1;
	TMenuItem *FondWhite1;
	TMenuItem *FondOlive1;
	TMenuItem *FondRed1;
	TMainMenu *DecorsMenu;
	TMenuItem *OuvrirDecors1;
	TMenuItem *SauverDecors1;
	TMenuItem *Nou1;
	TMenuItem *SauverDecors2;
	TLabel *LabelTypTrap;
	TEdit *EditTrapU;
	TEdit *EditTrapV;
	TLabel *Label12;
	TLabel *Label13;
	TLabel *Label14;
	TLabel *Label15;
	TLabel *Label18;
	TEdit *NumEdit;
	TComboBox *ComboBoxSurface;
	TLabel *Label19;
	TLabel *Label20;
        TEdit *EditHauteur;
	TButton *InfosBtn;
	TOpenDialog *SelectDecorsDialog;
	TButton *TrapBtn2;
	TMenuItem *CouleurFonds1;
	TMenuItem *White1;
	TMenuItem *Back1;
	TMenuItem *Purple1;
	TMenuItem *Yellow1;
	TMenuItem *Balck1;
	TMenuItem *Green1;
	TMenuItem *Olive1;
	TMenuItem *Red1;
	TMenuItem *Pink;
        TBitBtn *BtnZoom;
        TSpeedButton *SpdBtnx2;
        TSpeedButton *SpdBtnx4;
        TSpeedButton *SpdBtnx6;
        TButton *InfoBtn;
        TCheckBox *CheckBoxMarcher;
        TLabel *Label16;
        TComboBox *ComboBoxCollide;
        TCheckBox *CBAffichagePartiel;
        TCheckBox *CBDrawTrapezeKey;
        TButton *SelectZoneBtn;
        TButton *BtnReInit;
        TButton *BtnPrendreCercle;
        TCheckBox *CBZoneCirculaire;
	TCheckBox *CBShadow;
	void __fastcall ImageGrilleMouseMove(TObject *Sender, TShiftState Shift, int X,
	int Y);



	void __fastcall PlaceObjBtnClick(TObject *Sender);
	void __fastcall ImageBtnClick(TObject *Sender);
	void __fastcall ImageGrilleClick(TObject *Sender);

	void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	
	
	void __fastcall TrapBtnClick(TObject *Sender);
	
	void __fastcall FondPurple1Click(TObject *Sender);
	
	void __fastcall FondBlue1Click(TObject *Sender);
	void __fastcall FondPink1Click(TObject *Sender);
	void __fastcall FondYellow1Click(TObject *Sender);
	void __fastcall FondBlack1Click(TObject *Sender);
	void __fastcall FondGreen1Click(TObject *Sender);
	void __fastcall FondWhite1Click(TObject *Sender);
	void __fastcall FondOlive1Click(TObject *Sender);
	void __fastcall FondRed1Click(TObject *Sender);
	
	void __fastcall InfosBtnClick(TObject *Sender);
	void __fastcall ComboBoxActionChange(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	
	
	
	
	void __fastcall ImaDecMouseMove(TObject *Sender, TShiftState Shift, int X,
	int Y);
	
	
	void __fastcall ImaDecClick(TObject *Sender);
	
	void __fastcall SauverDecors2Click(TObject *Sender);
	void __fastcall Nou1Click(TObject *Sender);
	
	void __fastcall SauverDecors1Click(TObject *Sender);
	void __fastcall TrapBtn2Click(TObject *Sender);
	
        void __fastcall BtnZoomClick(TObject *Sender);
        void __fastcall SpdBtnx2Click(TObject *Sender);
        void __fastcall SpdBtnx4Click(TObject *Sender);
        void __fastcall SpdBtnx6Click(TObject *Sender);
        
        void __fastcall InfoBtnClick(TObject *Sender);
        void __fastcall SelectZoneBtnClick(TObject *Sender);
        void __fastcall ImageGrilleMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        
        void __fastcall ImageGrilleMouseUp(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        
        
        void __fastcall BtnReInitClick(TObject *Sender);
        void __fastcall BtnPrendreCercleClick(TObject *Sender);
        void __fastcall CBZoneCirculaireClick(TObject *Sender);
	void __fastcall CBShadowClick(TObject *Sender);
private:	// Déclarations de l'utilisateur

public:		// Déclarations de l'utilisateur
    void __fastcall AfficheGrille(TObject *Sender);
    void __fastcall AfficheDecors(TObject *Sender,int MouseX,int MouseY);
    void __fastcall SelectTrapeze(TObject *Sender, int effet);
    void __fastcall SelectTrapeze2(TObject *Sender,int Coul,int PosU,int PosV );
    void __fastcall ResetAll(TObject *Sender);
    void __fastcall DrawRectangle(int x1, int y1, int x2, int y2, int c);
    void __fastcall DrawCircle(int x, int y, int rayon, int color);
    __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
