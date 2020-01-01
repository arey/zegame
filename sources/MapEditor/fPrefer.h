//---------------------------------------------------------------------------
#ifndef fPreferH
#define fPreferH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\ComCtrls.hpp>
//---------------------------------------------------------------------------
class TPrefer : public TForm
{
__published:	// Composants gérés par l'EDI
	TLabel *Label1;
	TTrackBar *TrackBarScrollRate;
	TRadioButton *RadioButtonTonio;
	TRadioButton *RadioButtonNormal;
	TLabel *Label2;
	TTrackBar *TrackBarRefresh;
	TLabel *Label3;
	TLabel *Label4;
	TLabel *Label5;
	void __fastcall TrackBarScrollRateChange(TObject *Sender);
	void __fastcall RadioButtonTonioClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall TrackBarRefreshChange(TObject *Sender);
private:	// Déclarations de l'utilisateur
public:		// Déclarations de l'utilisateur
	__fastcall TPrefer(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern TPrefer *Prefer;
//---------------------------------------------------------------------------
#endif
