//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "fPrefer.h"
#include "editor.h"
#include "consts.h"
#include "fObjOutils.h"
#include "fLibOutils.h"
#include "fMapOutils.h"


#include "uMapView.h"
//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TPrefer *Prefer;
//---------------------------------------------------------------------------
__fastcall TPrefer::TPrefer(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------


//== Position du Track-Bar changé ============================================//
void __fastcall TPrefer::TrackBarScrollRateChange(TObject *Sender)
{
    ScrollViewRate = TrackBarScrollRate->Position;
}
//---------------------------------------------------------------------------

//== Changement des paramètres d'affichage ===================================//
void __fastcall TPrefer::RadioButtonTonioClick(TObject *Sender)
{
	if (RadioButtonTonio->Checked)
      {
         ConfigType      = ConfigTonio;
		 ConfigBoxColor  = ColorBoxTonio;
         ConfigBoxColor2 = ColorBoxTonio2;
      }
    else
      {
         ConfigType      = ConfigNormal;
		 ConfigBoxColor  = ColorBoxNormal;
         ConfigBoxColor2 = ColorBoxNormal2;
      };
}
//---------------------------------------------------------------------------
void __fastcall TPrefer::FormClose(TObject *Sender, TCloseAction &Action)
{
    LibOutils->FormShow(Sender);
    MapOutils->FormShow(Sender);
    switch (LibOutils->TypeLibSelect)
    {
   	 case  (Lib_DECORS) : ObjOutils->FormShow(Sender); break;
    }; 
}
//---------------------------------------------------------------------------

//== Changement de rafrachaissement ==========================================//
void __fastcall TPrefer::TrackBarRefreshChange(TObject *Sender)
{
    MapView->RefreshFreq = TrackBarRefresh->Position;	// fréquence de rafraîchissement de la fenêtre
    MapView->TimerFlip->Interval = 1000 / MapView->RefreshFreq;
}
//---------------------------------------------------------------------------