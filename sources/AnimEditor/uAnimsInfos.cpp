//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "uAnimsInfos.h"
#include "fPrincipale.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TAnimsInfos *AnimsInfos;

//---------------------------------------------------------------------------
__fastcall TAnimsInfos::TAnimsInfos(TComponent* Owner)
        : TForm(Owner)
{ infosLoaded = false;
  numTileDecors = -1;
  numAnimTileDecors = -1;
}
//---------------------------------------------------------------------------
void __fastcall TAnimsInfos::FormShow(TObject *Sender)
{  // Charge les infos & les affiche
  /* if (!infosLoaded)
   { DecObj->Element->LoadInfos();
   }*/

   RegenScreen();  // Rafraîchit l'écran

   // Indique si c'est une anim 'classique' ou un TileDecors
   if (DecObj->Element->peShowTile) LabelTileDecors->Caption = "OUI";
   else LabelTileDecors->Caption = "NON";

   // Remplit la barre des TileDecors possibles à associer avec cette anim
   CBAnimAssoc->Clear();
    for (int i=0; i < DecObj->Element->peNbAnim ; i++)
     {  //  Si c'est TileDecors différent de l'anim courante
        if (    (DecObj->Element->AnimSpriteLib[i]->Type == 0)
             && (aNumAnimation != IDAnimation->NumID(DecObj->Element->AnimSpriteLib[i]->Nom)) )
        {  CBAnimAssoc->Items->Add(DecObj->Element->AnimSpriteLib[i]->Nom);
        }
     }

  // Sélection du TileDecors associé dans la ComboBox
  if (DecObj->Element->AnimInfos[DecObj->getNumAnim()].numTileDecorsAssoc != DecObj->getNumAnim())
  { CBAnimAssoc->SelText = DecObj->Element->AnimInfos[DecObj->getNumAnim()].numTileDecorsAssoc;
  }

  SBHelp->SimpleText = "Sélectionner une animation";
}
//----------------------------------------------------------------------------//

//=== ComboBox OnChange ======================================================//
void __fastcall TAnimsInfos::CBAnimAssocChange(TObject *Sender)
{ SBHelp->SimpleText = "et supperposer les 2 sprites";
  ButtonPos->Enabled = true;
  // Récupère le n° du TileDecors Associé à afficher
  numAnimTileDecors = IDAnimation->NumID(CBAnimAssoc->Text);
  numTileDecors = -1;
  int anm = 0;
  while ((anm < DecObj->Element->peNbAnim) && (numTileDecors == -1))
  { if (DecObj->Element->AnimSpriteLib[anm]->Numero==numAnimTileDecors)
    { numTileDecors = DecObj->Element->AnimSpriteLib[anm]->FirstSprite; }
    anm++;
  }; 
  DecObj->Element->AnimInfos[DecObj->getNumAnim()].numTileDecorsAssoc = numTileDecors;
}
//----------------------------------------------------------------------------//

//=== Positionne le TileDecors selectionner par dessus le sprite affiché =====//
void __fastcall TAnimsInfos::ButtonPosClick(TObject *Sender)
{ aTileDecorsToScreen = true;       // On peut afficher le TileDecors sur la fAnimView
  aTileDecorsIsVisible = true;      // Le TileDecors doit être visible ou non
}
//----------------------------------------------------------------------------//


//=== Régénère les infos de la fiche =========================================//
void TAnimsInfos::RegenScreen()
{  EditdX->Text = DecObj->Element->AnimInfos[DecObj->getNumAnim()].dX;
   EditdY->Text = DecObj->Element->AnimInfos[DecObj->getNumAnim()].dY;
   if (!DecObj->Element->peShowTile)
        { LabeldX->Caption = "dX"; LabeldY->Caption = "dY";
          LabelX->Caption = "Variation en X :";
          LabelY->Caption = "Variation en Y :";
        }
   else { LabeldX->Caption = "dU"; LabeldY->Caption = "dV";
          LabelX->Caption = "Variation en U :";
          LabelY->Caption = "Variation en V :";
        }
   LabeldX2->Caption = 0; LabeldY2->Caption = 0;
}
//----------------------------------------------------------------------------//

//=== Sauve et ferme la fiche ================================================//
void __fastcall TAnimsInfos::ButtonOkClick(TObject *Sender)
{  Close();
}
//----------------------------------------------------------------------------//

//=== Remet à zéro les infos =================================================//
void __fastcall TAnimsInfos::ButtonNewClick(TObject *Sender)
{ // ComboBox non sélectionnée
  CBAnimAssoc->ItemIndex = -1;
  ButtonPos->Enabled = false;
  // Infos de SAnimInfos à zéro
  DecObj->Element->AnimInfos[DecObj->getNumAnim()].numTileDecorsAssoc = DecObj->getNumAnim();
  DecObj->Element->AnimInfos[DecObj->getNumAnim()].dX = 0;
  DecObj->Element->AnimInfos[DecObj->getNumAnim()].dY = 0;
  // Regénère la fiche
  RegenScreen();
}
//----------------------------------------------------------------------------//

