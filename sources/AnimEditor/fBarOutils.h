//---------------------------------------------------------------------------
#ifndef fBarOutilsH
#define fBarOutilsH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Buttons.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TBarOutils : public TForm
{
__published:	// Composants gérés par l'EDI
	TLabel *Title;
	TLabel *LabelID;
	TSpeedButton *SpdBtnNO;
	TSpeedButton *SpdBtnN;
	TSpeedButton *SpdBtnNE;
	TSpeedButton *SpdBtnMiddle;
	TSpeedButton *SpdBtnE;
	TSpeedButton *SpdBtnO;
	TSpeedButton *SpdBtnSE;
	TSpeedButton *SpdBtnS;
	TSpeedButton *SpdBtnSO;
	TLabel *Label1;
	TLabel *LabelNbSprites;
	TLabel *Label3;
	TLabel *LabelSurSprites;
	TSpeedButton *SpdBtnPlay;
	TSpeedButton *SpdBtnBackward;
	TSpeedButton *SpdBtnStop;
	TSpeedButton *SpdBtnPause;
	TSpeedButton *SpdBtnForward;
        TTrackBar *TrackBarZoom;
        TLabel *Label2;
        TScrollBar *ScrollBarStep;
        TLabel *Label4;
        TLabel *Label5;
        TLabel *Label6;
        TLabel *LabelSourisX;
        TLabel *LabelSourisY;
        TLabel *Label7;
        TLabel *Label8;
        TLabel *Label9;
        TLabel *LabelSourisGravX;
        TLabel *Label11;
        TLabel *LabelSourisGravY;
        void __fastcall SpdBtnNOClick(TObject *Sender);
        void __fastcall SpdBtnOClick(TObject *Sender);
        void __fastcall SpdBtnSOClick(TObject *Sender);
        void __fastcall SpdBtnSClick(TObject *Sender);
        void __fastcall SpdBtnSEClick(TObject *Sender);
        void __fastcall SpdBtnEClick(TObject *Sender);
        void __fastcall SpdBtnNEClick(TObject *Sender);
        void __fastcall SpdBtnNClick(TObject *Sender);
        void __fastcall SpdBtnMiddleClick(TObject *Sender);
        
        void __fastcall SpdBtnPlayClick(TObject *Sender);
        void __fastcall TrackBarZoomChange(TObject *Sender);
        
        void __fastcall SpdBtnStopClick(TObject *Sender);
        void __fastcall SpdBtnPauseClick(TObject *Sender);
        void __fastcall SpdBtnBackwardClick(TObject *Sender);
        void __fastcall SpdBtnForwardClick(TObject *Sender);
        void __fastcall ScrollBarStepChange(TObject *Sender);
        
private:	// Déclarations de l'utilisateur
public:		// Déclarations de l'utilisateur
	__fastcall TBarOutils(TComponent* Owner);

        // modifie les paramètres du sprite superposable d'aprés les param. de l'anim courante
        void __fastcall SpriteSuperposeeMoving();
};
//---------------------------------------------------------------------------
extern TBarOutils *BarOutils;
//---------------------------------------------------------------------------
#endif
