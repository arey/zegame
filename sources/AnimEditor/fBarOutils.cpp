////////////////////////////////////////////////////////////////////////////////
//  fBarOutils :  fenêtre permettant de contrôler une animation				  //	
////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "fBarOutils.h"
#include "fPrincipale.h"
#include "fAnimView.h"
//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TBarOutils *BarOutils;
//---------------------------------------------------------------------------
__fastcall TBarOutils::TBarOutils(TComponent* Owner)
	: TForm(Owner)
{
    // Positionnement de la fiche sur la fiche MDI principale
    Left = 0;
    Top = 0;
    Height = 418;

    // Désactionne tous les boutons
    SpdBtnForward->Enabled = false;
    SpdBtnBackward->Enabled = false;
    SpdBtnPause->Enabled = false;
    SpdBtnStop->Enabled = false;
    SpdBtnPlay->Enabled = false;

    // Elements neutres
    ScrollBarStep->Position = 0;
    ScrollBarStep->Enabled = false;
}
//---------------------------------------------------------------------------

//=== Orientations cardinaux des animations pour les 9 boutuons ==============//
void __fastcall TBarOutils::SpdBtnNOClick(TObject *Sender)
{    AnimDirection = NO; AnimView->RecadreView();
}
void __fastcall TBarOutils::SpdBtnOClick(TObject *Sender)
{    AnimDirection = O; AnimView->RecadreView();
}
void __fastcall TBarOutils::SpdBtnSOClick(TObject *Sender)
{    AnimDirection = SO; AnimView->RecadreView();
}
void __fastcall TBarOutils::SpdBtnSClick(TObject *Sender)
{    AnimDirection = S; AnimView->RecadreView();
}
void __fastcall TBarOutils::SpdBtnSEClick(TObject *Sender)
{    AnimDirection = SE; AnimView->RecadreView();
}
void __fastcall TBarOutils::SpdBtnEClick(TObject *Sender)
{    AnimDirection = E; AnimView->RecadreView();
}
void __fastcall TBarOutils::SpdBtnNEClick(TObject *Sender)
{    AnimDirection = NE; AnimView->RecadreView();
}
void __fastcall TBarOutils::SpdBtnNClick(TObject *Sender)
{    AnimDirection = N; AnimView->RecadreView();
}
void __fastcall TBarOutils::SpdBtnMiddleClick(TObject *Sender)
{    AnimDirection = CENTRAL; AnimView->RecadreView();
}
//----------------------------------------------------------------------------//

//=== Bouton "Jouer Animation" ===============================================//
void __fastcall TBarOutils::SpdBtnPlayClick(TObject *Sender)
{  // Animation en route
   aPlayingAnimationState = PLAY;
   BarOutils->SpdBtnPause->Enabled = true;
   BarOutils->SpdBtnStop->Enabled = true;
   BarOutils->SpdBtnBackward->Enabled = false;
   BarOutils->SpdBtnForward->Enabled = false;
   ScrollBarStep->Enabled = false;
}
//----------------------------------------------------------------------------//

//=== TrackBar des zooms modifiée ============================================//
void __fastcall TBarOutils::TrackBarZoomChange(TObject *Sender)
{ // On change la valeur du zoom
  AnimView->Zoom = TrackBarZoom->Position;
  AnimView->ScrollViewVert->LargeChange = AnimView->Zoom;
  AnimView->ScrollViewHor->LargeChange = AnimView->Zoom;
  AnimView->ScrollViewVert->SmallChange = AnimView->Zoom;
  AnimView->ScrollViewHor->SmallChange = AnimView->Zoom;
  // Et l'on recadre la fenêtre
  AnimView->RecadreView();
}
//----------------------------------------------------------------------------//

//=== Bouton Stop ============================================================//
void __fastcall TBarOutils::SpdBtnStopClick(TObject *Sender)
{  // Si Animation se joue ou est en pause, la stop
   if ((aPlayingAnimationState == PLAY) || (aPlayingAnimationState == PAUSE))
   {  aPlayingAnimationState = STOP;
      BarOutils->SpdBtnBackward->Enabled = true;
      BarOutils->SpdBtnForward->Enabled = true;
      BarOutils->SpdBtnPause->Enabled = false;
      ScrollBarStep->Enabled = true;
   }
}
//----------------------------------------------------------------------------//

//=== Bouton Pause ===========================================================//
void __fastcall TBarOutils::SpdBtnPauseClick(TObject *Sender)
{  // Si Animation se joue, la stop
   if (aPlayingAnimationState == PLAY)
   {  aPlayingAnimationState = PAUSE;
   }
}
//----------------------------------------------------------------------------//

//=== Bouton Retour en arrière ===============================================//
void __fastcall TBarOutils::SpdBtnBackwardClick(TObject *Sender)
{  // Revient à la 1ère image de l'anim
   if (aPlayingAnimationState == STOP)
   {  if (AnimationObjet == OBJET_PERSOS) Perso->peNumSprite = 0;
      else if (AnimationObjet == OBJET_DECORS) DecObj->peNumSprite = 0;
   }
}
//----------------------------------------------------------------------------//

//=== Bouton Avance à la fin  ================================================//
void __fastcall TBarOutils::SpdBtnForwardClick(TObject *Sender)
{  // Se place sur la dernière image de l'anim
   if (aPlayingAnimationState == STOP)
   {  if (AnimationObjet == OBJET_PERSOS)      Perso->peNumSprite = Perso->peNbSprite - 1;
      else if (AnimationObjet == OBJET_DECORS) DecObj->peNumSprite = DecObj->peNbSprite - 1;
   }
}
//----------------------------------------------------------------------------//

//=== Scroll Bar des numéros de sprites ======================================//
void __fastcall TBarOutils::ScrollBarStepChange(TObject *Sender)
{ // Se place sur l'image souhaitée
  if (aPlayingAnimationState == STOP)
   {  if (AnimationObjet == OBJET_PERSOS)      Perso->peNumSprite = ScrollBarStep->Position;
      else if (AnimationObjet == OBJET_DECORS) DecObj->peNumSprite = ScrollBarStep->Position;
   }
  // Si l'on est entrain de positionner un sprite superposable
  // --> on doit le faire avancer aussi
  SpriteSuperposeeMoving(); 
}
//----------------------------------------------------------------------------//

//=== Modifie les paramètres du sprite superposable d'aprés les param. =======//
//    de l'anim courante
void __fastcall TBarOutils::SpriteSuperposeeMoving()
{     // ????????????,



}
//----------------------------------------------------------------------------//

