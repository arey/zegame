//---------------------------------------------------------------------------
#ifndef fMenuBankGravH
#define fMenuBankGravH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Buttons.hpp>
#include <vcl\ExtCtrls.hpp>
#include <vcl\ComCtrls.hpp>
//---------------------------------------------------------------------------
class TFormGrilleGrav : public TForm
{
__published:	// Composants gérés par l'EDI
	TBitBtn *BitBtn1;
	TLabel *Label5;
	TEdit *FCentY;
	TEdit *FCentX;
	TLabel *Label7;
	TLabel *FNumFrame;
	TLabel *FSurNbSprite;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *fNumAnim;
	TLabel *Label4;
	TLabel *FSurNbAnim;
	TScrollBar *SpriteScroll;
	TLabel *Designation;
	TImage *PartOfGrille;
	TImage *SpriteImage;
	TTrackBar *TrackBarZoom;
	TLabel *Label3;
	TLabel *LabelZoom;
	TEdit *EditMouseX;
	TLabel *Label6;
	TEdit *EditMouseY;
	TRichEdit *RichEdit1;
	TLabel *BmpName;
        TBitBtn *BitBtn2;
        TBitBtn *BitBtn3;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall SpriteScrollChange(TObject *Sender);
	void __fastcall TrackBarZoomChange(TObject *Sender);
	
	
	
	void __fastcall SpriteImageMouseMove(TObject *Sender, TShiftState Shift, int X,
	int Y);
	void __fastcall SpriteImageClick(TObject *Sender);
	
	void __fastcall BitBtn1Click(TObject *Sender);
	
        void __fastcall BitBtn2Click(TObject *Sender);
        
        void __fastcall BitBtn3Click(TObject *Sender);
private:	// Déclarations de l'utilisateur
public:		// Déclarations de l'utilisateur
	__fastcall TFormGrilleGrav(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern TFormGrilleGrav *FormGrilleGrav;
//---------------------------------------------------------------------------
#endif
