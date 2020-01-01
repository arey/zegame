////////////////////////////////////////////////////////////////////////////////
//  fWorkStation :  fenêtre permettant de travailler                          //
//					  - sur l'animation ouverte  							  //
//					  - les caractéristiques des objets/persos				  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


//--- Faisons faire C++ Builder ----------------------------------------------//
#include <vcl\vcl.h>
#pragma hdrstop
#include "fWorkStation.h"
#pragma resource "*.dfm"
TWokStation *WokStation;
//----------------------------------------------------------------------------//

#include "fprincipale.h"    	// fenêtre principale de l'éditeur
#include "fBarOutils.h"         // Barre d'outils et de renseignements sur l'état de l'anim
#include "fAnimView.h"          // fenêtre de visualisation de l'animation
#include "fMenuScript.h"
#include "DDLibEditor.h"
#include "uAnimsInfos.h"        // Ch'tite fenêtre d'informations au sujet des anims
#include "SpritesLib.h"			// Gestion des anims "classiques"
#include "GameDebug.h"			// Permet de TRACER le programme en mode release

//---------------------------------------------------------------------------

//=== Constructeur de la fiche WorkStation ===================================//
__fastcall TWokStation::TWokStation(TComponent* Owner)
	: TForm(Owner)
{   // Positionnement et taille de la fiche sur la fiche MDI principale
    Left = 577; Top = 0;
    Width = 392+44; Height = 692;
    // Bouton de chargement du script désactivé
    BtnLoadScript->Enabled = false;

    // Initialiise le Groupe-Box des sprites
    ShowZoneAuSol->Enabled = false;   ButtonZoneAuSol->Enabled = false;
    ShowZonePied->Enabled = false;   ButtonZonePied->Enabled = false;
    ShowZoneAuSolPrec->Enabled = false;
    ShowZonePiedPrec->Enabled = false;
    ShowGravAuSol->Enabled = false;   ButtonGravAuSol->Enabled = false;
    ShowTrapPrinc->Enabled = false;    
    GBSpriteInfos = GBSI_EMPTY;    // Groupe-Box des sprites infos vides
}
//----------------------------------------------------------------------------//

//=== Compile le script en cours en mémoire et charge toutes ses infos =======//
void __fastcall TWokStation::BtnLoadScriptClick(TObject *Sender)
{
    // *** 1) Compilation du Script ***
    // Initialise la SpriteTileLib
    if (STLGuy!=NULL) delete STLGuy;
    STLGuy = new SpriteTilePersoLib;
    // Personnage
    if (Perso!=NULL) delete Perso;
    Perso = new Personnage(typePERSO);
    Perso->ModelePerso->STLPersonnage = STLGuy;

    // Charge les *.bmp et *.spi
    Perso->ModelePerso->LoadIna(IDScriptShortFilename.c_str(),&STLGuy);
    STLGuy->CreateSprIndex();
    Perso->ModelePerso->STLFrameSuperp->CreateSprIndex();
    // Let's go
    ScriptCompiled = true;

    // *** 2) Initialise la WorkStation ***
    // On Ajoute à la Combobox, le nom des anims
    CBListAnims->Clear();
    for (int i=0; i < Perso->ModelePerso->peNbAnim ; i++)
     { CBListAnims->Items->Add(Perso->ModelePerso->AnimSpriteLib[i]->Nom);
     }
}
//----------------------------------------------------------------------------//

//=== Contenu de la ComboxBox modifié ========================================//
void __fastcall TWokStation::CBListAnimsChange(TObject *Sender)
{   // Regarde à partir de la map des IDAnim laquelle anim est sélectionnée
    AnsiString champs = CBListAnims->Text;
    // Vérifie si le champs existe bien dans la map
    if (!IDAnimation->IsCle(champs))  // ne devrait jamais arriver
      {  MessageBoxA(NULL,"Le nom d'animation n'est pas valable .... ???","Warning !",1);
      	 //exit;	// sort de la fonction
      }
    // Indique l'animation à afficher
    aNumAnimation = IDAnimation->NumID(champs);

    // Animation vers le Sud
    AnimDirection = S;
    BarOutils->SpdBtnS->Down = true;
    BarOutils->SpdBtnS->Enabled = true;

    //=== Dans le cas d'un PERSO ===
    if (AnimationObjet == OBJET_PERSOS)
    {  Perso->peDirection = AnimDirection;
       // Change les valeurs de la barre d'outils
       CalculeEtatPerso();   // Change des paramètres du personnage

       BarOutils->LabelSurSprites->Caption = Perso->peNbSprite - 1;
       BarOutils->LabelNbSprites->Caption = Perso->peNumSprite;
       BarOutils->ScrollBarStep->Position = 0;
       BarOutils->ScrollBarStep->Max = Perso->peNbSprite - 1;
    }
    //=== Dans le cas d'un DECORS ===
    if (AnimationObjet == OBJET_DECORS)
    {  DecObj->peDirection = AnimDirection;
       DecObj->peEtat = IDAnimation->NumID(CBListAnims->Text); 
       // Change les valeurs de la barre d'outils
       CalculeEtatDecors();   // Change des paramètres des décors

       BarOutils->LabelSurSprites->Caption = DecObj->peNbSprite - 1;
       BarOutils->LabelNbSprites->Caption = DecObj->peNumSprite;
       BarOutils->ScrollBarStep->Position = 0;
       BarOutils->ScrollBarStep->Max = DecObj->peNbSprite - 1;
    }

    //=== En commun ===
    if (aPlayingAnimationState != PLAY) BarOutils->ScrollBarStep->Enabled = true;

    // Actionne presque tous les boutons de la barre d'outils
    BarOutils->SpdBtnForward->Enabled = true;
    BarOutils->SpdBtnBackward->Enabled = true;
    BarOutils->SpdBtnPlay->Enabled = true;

    // Anim à l'arrêt
    if (aPlayingAnimationState == OFF) aPlayingAnimationState = STOP;

    // Recadre la vue d'aprés les dimensions de la nouvelle anim
    AnimView->RecadreView();

    // Met à jour le groupe Box des sprites
    MaJGBSpriteInfos();

    //=== met à jour le ComboBox CBTypeFrame ===
    MaJCBTypeFrame();

    //=== Si c'est un DECORS -> Boîte d'infos complémentaires accessibles ===
    if (AnimationObjet == OBJET_DECORS)
    {  // Enable le BitBtnInfos si c'est une vrai anim
       if (!DecObj->Element->peShowTile) BitBtnInfos->Enabled = true;
       // Ou si c'est un TileDecors secondaire (différent de ARRET1)
       else if (DecObj->peEtat!=ARRET1) BitBtnInfos->Enabled = true;

    } else
    { BitBtnInfos->Enabled = false;
    }
}
//----------------------------------------------------------------------------//

//=== Calcule l'état du personnage aprés avoir changé certaines mouvements ===//
//  => de caméra, ...
void __fastcall TWokStation::CalculeEtatPerso()
{
  // peDirection du personnage à modifier
  switch (AnimDirection)
  { case O :
    case SO :
    case S :
    case SE :
    case E :
    case NE :
    case N :
    case NO :  Perso->peDirection = AnimDirection; break;
    case CENTRAL : break;
    case AUCUNE  : break;
  }

  Perso->peEtat = aNumAnimation;      // peEtat du personnage à changer
  Perso->peNumSprite = 0;             // Retourne à la 1ère image
  Perso->ResultAction();              // Calcul : peNbSprite & peSprNumLib
}
//----------------------------------------------------------------------------//

//=== Calcule l'état du personnage aprés avoir changé certaines mouvements ===//
//  => de caméra, ...
void __fastcall TWokStation::CalculeEtatDecors()
{
  // peDirection du personnage à modifier
  DecObj->peDirection = AnimDirection;

  DecObj->peEtat = aNumAnimation;      // peEtat du personnage à changer
  DecObj->peNumSprite = 0;             // Retourne à la 1ère image
  DecObj->ResultAction();              // Calcul : peNbSprite & peSprNumLib
}
//----------------------------------------------------------------------------//

//=== Capture la zone au sol du peronnage ====================================//
void __fastcall TWokStation::ButtonZoneAuSol2Click(TObject *Sender)
{ // Indique le mode utilisé
  AnimView->workMode = TAnimView::RECTANGLE;   // Zone <=> rectangle
  AnimView->workZone = TAnimView::ZONESOL;
  AnimView->P1x = AnimView->P2x = AnimView->P1y = AnimView->P2y = -1;  // Points initialisés
}
//----------------------------------------------------------------------------//

//=== Mise à Jour du Groupe-Box Sprite-Infos =================================//
void __fastcall TWokStation::MaJGBSpriteInfos()
{ // Regarde si elle est vide
  if (GBSpriteInfos == GBSI_EMPTY)
     {  if (AnimationObjet == OBJET_PERSOS)
        { GBSpriteInfos = GBSI_PERSO;
          CBListTypePersos->Clear();
          for (int i=0; i <= IDPersosType->MaxAttribut() ; i++)
          { if (IDPersosType->IsElement(i))
            { CBListTypePersos->Items->Add(IDPersosType->CleID(i));
            }
            else  MessageBoxA(NULL,"Erreur ds le fichier des ID des types de personnages","Warning !",1);
          }
         } else
         if (AnimationObjet == OBJET_DECORS)
         { GBSpriteInfos = GBSI_DECORS;
           CBListTypePersos->Clear();
           for (int i=0; i <= IDPersosType->MaxAttribut() ; i++)
          { if (IDPersosType->IsElement(i))
            { CBListTypePersos->Items->Add(IDPersosType->CleID(i));
            }
            else  MessageBoxA(NULL,"Erreur ds le fichier des ID des types de personnages","Warning !",1);
          }
         }
     }
}
//----------------------------------------------------------------------------//

//=== mise à jour le ComboBox CBTypeFrame ====================================//
void __fastcall TWokStation::MaJCBTypeFrame()
{  if (AnimationObjet == OBJET_PERSOS)
   {   // Récupère le n°ID de l'anim sélectionné
       int numIDAnimation = IDAnimation->NumID(CBListAnims->Text);
       // Récupère le n°ID de la frame associée à cette anim à partir du sprite courant affiché
       SpriteTilePerso *Sprite = (SpriteTilePerso*) Perso->ModelePerso->STLPersonnage->SprIndex[Perso->peSprNumLib];
       int numIDFrameAnim = Sprite->pFrame->IDFrame;

       // Si l'utilisateur a sélectionné un membre
       // Alors on affiche les frame que l'on peut y supperposer
       if (ListBoxMembres->ItemIndex != -1)
       { // Récupère le n°ID du membre sélectionné
         AnsiString texte = ListBoxMembres->Items->Strings[ListBoxMembres->ItemIndex];
         int numIDMemb = IDMembres->NumID(texte);
         // Complète la ComboBox CBTypeFrame
         int nbTypeFrame = 0;
         CBTypeFrame->Clear();
         for (int i=0; i < Perso->ModelePerso->ObjetFrame[numIDMemb].TabObjetFrame.size(); i++)
         { if (Perso->ModelePerso->ObjetFrame[numIDMemb].TabObjetFrame[i].numIDFrameOriginal == numIDFrameAnim)
           { int numtypeobj = Perso->ModelePerso->ObjetFrame[numIDMemb].TabObjetFrame[i].numIDTypeObjet;
             CBTypeFrame->Items->Add(IDTypeObjSprite->CleID(numtypeobj));
             nbTypeFrame++;
           }
         }
         // Affiche le nombre de frame trouvé
         LabelNbFrame->Caption = nbTypeFrame;
       }
   }
}
//----------------------------------------------------------------------------//

void __fastcall TWokStation::CBListTypePersos2Change(TObject *Sender)
{ // Regarde le type de sprite choisi
  AnsiString TypePersos = CBListTypePersos->Text;
  if ( TypePersos == "HUMAIN" )
  { ShowZoneAuSol->Enabled = true;  ButtonZoneAuSol->Enabled = true;  // Zone au sol à prendre
    ShowZonePied->Enabled = true;  ButtonZonePied->Enabled = true;    // Zone des pieds
    ShowZoneAuSolPrec->Enabled = true;                                // Zone au sol à prendre du sprite précédent
    ShowZonePiedPrec->Enabled = true;                                 // Zone au sol des pieds précédent à voir
    ShowGravAuSol->Enabled = true;  ButtonGravAuSol->Enabled = true;  // Centre de gravité projeté au sol à prendre
    ShowTrapPrinc->Enabled = false;                                   // Trapèze principal pour les objets animés
    // Change le type de sprite du personnage
    Perso->ModelePerso->typePerso = IDPersosType->NumID(TypePersos);
  }
  else
  if ( TypePersos == "OBJET_ANIME" )
  { ShowZoneAuSol->Enabled = false;  ButtonZoneAuSol->Enabled = false;  // Zone au sol à prendre
    ShowZonePied->Enabled = false;  ButtonZonePied->Enabled = false;
    ShowZoneAuSolPrec->Enabled = false;                                 // Zone au sol à prendre du sprite précédent
    ShowZonePiedPrec->Enabled = false;  
    ShowGravAuSol->Enabled = true;  ButtonGravAuSol->Enabled = true;    // Centre de gravité projeté au sol à prendre
    ShowTrapPrinc->Enabled = true;                                      // Trapèze principal pour les objets animés
  }
  else
  { ShowZoneAuSol->Enabled = false;  ButtonZoneAuSol->Enabled = false;   // Zone au sol non primordiale
    ShowZonePied->Enabled = false;  ButtonZonePied->Enabled = false; 
    ShowZoneAuSolPrec->Enabled = false;                                  // Zone au sol non primordiale du sprite précédent
    ShowZonePiedPrec->Enabled = false; 
    ShowGravAuSol->Enabled = true;  ButtonGravAuSol->Enabled = true;     // Centre de gravité projeté au sol à prendre
    ShowTrapPrinc->Enabled = false;                                      // Trapèze principal pour les objets animés
  }


}
//----------------------------------------------------------------------------//

//=== Capture le centre de gravité au sol du peronnage =======================//
void __fastcall TWokStation::ButtonGravAuSolClick(TObject *Sender)
{ // Indique le mode utilisé
  AnimView->workMode = TAnimView::POINT;   // Zone <=> rectangle
  AnimView->workZone = TAnimView::PT_GRAV_SOL;
  AnimView->P1x = AnimView->P1y = -1;  // Points initialisés
}
//----------------------------------------------------------------------------//



//=== Quand l'utilisateur sélectionne un membre ==============================//
void __fastcall TWokStation::ListBoxMembresClick(TObject *Sender)
{  //=== met à jour le ComboBox CBTypeFrame ===
   MaJCBTypeFrame();
}
//----------------------------------------------------------------------------//



//=== Sélection du type d'objet à calquer sur l'anim =========================//
void __fastcall TWokStation::CBTypeFrameChange(TObject *Sender)
{  // Récupère le numéro du type d'objet sélectionné
   int numIDTypeObjSprite = IDTypeObjSprite->NumID(CBTypeFrame->Text);
   // Récupère le numéro du membre sélectionné
   int numIDMemb = IDMembres->NumID(ListBoxMembres->Items->Strings[ListBoxMembres->ItemIndex]);


   // Regarde dans le PersoSprite courant si ce sprite superposable n'a pas
   // déjà été supperposé par ailleurs
   SpriteTilePerso *Sprite = (SpriteTilePerso*) Perso->ModelePerso->STLPersonnage->SprIndex[Perso->peSprNumLib];
   if (Sprite->GSprInf->ListAnimSuperp.Existe(numIDTypeObjSprite))
   {  // Si le sprite a déjà été placé auparavant -> on va devoir l'afficher
      LabelPosition->Caption =  "OUI";
/*      int posX, posY;
      Sprite->GSprInf->ListAnimSuperp.GetCoord(numIDTypeObjSprite, posX, posY);*/
      aSpriteSuperposeeToScreen = true;   // On peut afficher un sprite supperposable

   } else
   {  // Sinon on ?????
      LabelPosition->Caption =  "NON";

      //*** Affichage du sprite superposable à l'écran ***
      // On récupère le n° de la frame sur lequel il doit être
      int numIDFrameSuperp = Perso->ModelePerso->ObjetFrame[numIDMemb].Search(numIDTypeObjSprite);
      // On parcours la liste des sprites superposables jusqu'à trouver le dernier correspondant
      // au numéro numIDFrameSuperp
      SpriteTile* SpriteSuperp;
      SpriteSuperp = Perso->ModelePerso->STLFrameSuperp->SL_List; // dernier sprite
      bool trouve = false;
      for (int i = Perso->ModelePerso->STLFrameSuperp->NbSprites-1  ;  i >=0 || !trouve ; i--)
      {   if (SpriteSuperp->pFrame->IDFrame == numIDFrameSuperp)
          {  trouve = true;
          }
          else SpriteSuperp = (SpriteTile*) SpriteSuperp->Suiv;
      };
      // Si on ne l'a pas trouve, c'est qu'il ya problème
      if (!trouve) MessageBoxA(NULL,"le n° de sprite n'a pas été trouvé","Error : TWokStation::CBTypeFrameChange",1);
      aSpriteSuperposeeTileEnd = SpriteSuperp;
      // Remonte au début de la frame
      int nbSpritesParFrame = SpriteSuperp->pFrame->NbSprites;
      for (int i=0 ; i < nbSpritesParFrame-1 ; i++) SpriteSuperp = (SpriteTile*) SpriteSuperp->Suiv;
      aSpriteSuperposeeTileDeb = SpriteSuperp;

      // Remonte jusqu'au bon sprite d'apres la caméra et le n° du sprite de l'anim en cours
      int numSprite = Perso->peNumSprite;  // n° de sprite de l'anim en cours
      int posCamera = Perso->peDirection;  // position de la caméra
      int nbSpriteParAnim = nbSpritesParFrame / 8;
      int nbSpriteARemonter = posCamera*nbSpriteParAnim + numSprite;
      for (int i=0; i < nbSpriteARemonter ; i++)
      {  SpriteSuperp = (SpriteTile*) SpriteSuperp->Prec;
      }
      // Prêt à l'afficher
      aSpriteSuperposeeToScreen = true;   // On peut afficher un sprite supperposable
      aSpriteSuperposeeMoving   = true;   // On peut déplacer un sprite supperposable
      aSpriteSuperposeeNumero = SpriteSuperp->NumSprite;
      aSpritePersoNumAnim = Perso->peNumSprite;
   }
   // Verifie si le bmp est en mémoire
   int numIDFrameSuperp = Perso->ModelePerso->ObjetFrame[numIDMemb].Search(numIDTypeObjSprite);
   SpriteFrame *sprFrame = Perso->ModelePerso->FrameSuperpList->SF_List;
   while ((sprFrame!=NULL) && (sprFrame->IDFrame !=numIDFrameSuperp))
   { sprFrame = sprFrame->SF_Suiv;
   }
   if (sprFrame==NULL) MessageBoxA(NULL,"le n° de frame n'a pas été trouvé","Error : TWokStation::CBTypeFrameChange",1);
   else
   if (!sprFrame->BmpLoadIntoMemory)
   {  sprFrame->ReserveMemoryAndLoadBmp(&(DS_VA->pDD));
   }
}
//----------------------------------------------------------------------------//

//=== Repositionner un sprite superposable ===================================//
void __fastcall TWokStation::AnimSupepPositClick(TObject *Sender)
{   // Récupère le numéro du type d'objet sélectionné
   if (CBTypeFrame->Text=="") return;
   int numIDTypeObjSprite = IDTypeObjSprite->NumID(CBTypeFrame->Text);
   // Récupère le numéro du membre sélectionné
   if (ListBoxMembres->ItemIndex==-1) return;
   int numIDMemb = IDMembres->NumID(ListBoxMembres->Items->Strings[ListBoxMembres->ItemIndex]);

   // Regarde dans le PersoSprite courant si ce sprite superposable n'a pas
   // déjà été supperposé par ailleurs
   SpriteTilePerso *Sprite = (SpriteTilePerso*) Perso->ModelePerso->STLPersonnage->SprIndex[Perso->peSprNumLib];
   LabelPosition->Caption =  "Repositionnement";
      //*** Affichage du sprite superposable à l'écran ***
      // On récupère le n° de la frame sur lequel il doit être
      int numIDFrameSuperp = Perso->ModelePerso->ObjetFrame[numIDMemb].Search(numIDTypeObjSprite);
      // On parcours la liste des sprites superposables jusqu'à trouver le dernier correspondant
      // au numéro numIDFrameSuperp
      SpriteTile* SpriteSuperp;
      SpriteSuperp = Perso->ModelePerso->STLFrameSuperp->SL_List; // dernier sprite
      bool trouve = false;
      for (int i = Perso->ModelePerso->STLFrameSuperp->NbSprites-1  ;  i >=0 || !trouve ; i--)
      {   if (SpriteSuperp->pFrame->IDFrame == numIDFrameSuperp)
          {  trouve = true;
          }
          else SpriteSuperp = (SpriteTile*) SpriteSuperp->Suiv;
      };
      // Si on ne l'a pas trouve, c'est qu'il ya problème
      if (!trouve) MessageBoxA(NULL,"le n° de sprite n'a pas été trouvé","Error : TWokStation::CBTypeFrameChange",1);
      aSpriteSuperposeeTileEnd = SpriteSuperp;
      // Remonte au début de la frame
      int nbSpritesParFrame = SpriteSuperp->pFrame->NbSprites;
      for (int i=0 ; i < nbSpritesParFrame-1 ; i++) SpriteSuperp = (SpriteTile*) SpriteSuperp->Suiv;
      aSpriteSuperposeeTileDeb = SpriteSuperp;

      // Remonte jusqu'au bon sprite d'apres la caméra et le n° du sprite de l'anim en cours
      int numSprite = Perso->peNumSprite;  // n° de sprite de l'anim en cours
      int posCamera = Perso->peDirection;  // position de la caméra
      int nbSpriteParAnim = nbSpritesParFrame / 8;
      int nbSpriteARemonter = posCamera*nbSpriteParAnim + numSprite;
      for (int i=0; i < nbSpriteARemonter ; i++)
      {  SpriteSuperp = (SpriteTile*) SpriteSuperp->Prec;
      }
      // Si le bmp est chargé
      aSpriteSuperposeeToScreen = true;   // On peut afficher un sprite supperposable
      aSpriteSuperposeeMoving   = true;   // On peut déplacer un sprite supperposable
      aSpriteSuperposeeNumero = SpriteSuperp->NumSprite;
      aSpritePersoNumAnim = Perso->peNumSprite;
}
//----------------------------------------------------------------------------//

//=== Ouvre une ch'tite fenêtre d'informations ===============================//
void __fastcall TWokStation::BitBtnInfosClick(TObject *Sender)
{ AnimsInfos->Show();

}
//----------------------------------------------------------------------------//

//=== Capture la zone des pieds du peronnage =================================//
void __fastcall TWokStation::ButtonZonePiedClick(TObject *Sender)
{ // Indique le mode utilisé
  AnimView->workMode = TAnimView::RECTANGLE_ORIENTE;   // Zone <=> rectangle pouvant prendre 8 orientations différentes
  AnimView->workZone = TAnimView::ZONEPIED;
  AnimView->P1x = AnimView->P2x = AnimView->P1y = AnimView->P2y = -1;  // Points initialisés
}
//---------------------------------------------------------------------------




//=== Sélection d'une anim superposable classique ============================//
void __fastcall TWokStation::ListBoxAnimsClick(TObject *Sender)
{
    // Affiche les coordonnées 
    AnsiString animID = ListBoxAnims->Items->Strings[ListBoxAnims->ItemIndex];
    aNumAnimClassik = IDAnimation->NumID(animID);
	SClassikAnimInfos* animInfos = DecObj->Element->GetClassikAnim(aNumAnimClassik);
    LabelAnimX->Caption = animInfos->dX;
    LabelAnimY->Caption = animInfos->dY;

    // Récupère un pointeur sur l'anim sélectionnée
   AnimSprite* animsprite = STGL->GetAnimSprite(aNumAnimClassik);
   if (animsprite->Type == 3)	// 1 Orientation
   {    aSpriteClassik = STGL->GetSpriteTile(animsprite->FirstSprite);
   		TRACEDBG("animsprite->FirstSprite : %d\n", animsprite->FirstSprite);
   }

   // Recheche de l'index ds "peNumClassikAnim" de l'anim
    for (int i=0 ; i < DecObj->Element->peNbClassikAnim ; i++)
   { if (DecObj->Element->ClassikAnimInfos[i].iAnimID == aNumAnimClassik)
   	 {	aIndexAnimClassik = i;
        break;
     }
   }

   // On commence par afficher l'image n°0 de l'anim 
   DecObj->peNumClassikAnim[aIndexAnimClassik]=0;

   aAnimClassikMoving = false;
   aSpriteClassikToScreen = true;
   BtnPlayAnimClassik->Enabled = true;
}
//----------------------------------------------------------------------------//

void __fastcall TWokStation::BtnPositionnerAnimClick(TObject *Sender)
{  // Récupère le numéro de l'anim sélectionnée
   if (ListBoxAnims->ItemIndex==-1) return;
   AnsiString animID = ListBoxAnims->Items->Strings[ListBoxAnims->ItemIndex];
   aNumAnimClassik = IDAnimation->NumID(animID);

   // Récupère un pointeur sur l'anim sélectionnée
   AnimSprite* animsprite = STGL->GetAnimSprite(aNumAnimClassik);
   if (animsprite->Type == 3)	// 1 Orientation
   {    aSpriteClassik = STGL->GetSpriteTile(animsprite->FirstSprite);
   		TRACEDBG("animsprite->FirstSprite : %d\n", animsprite->FirstSprite);
   }

   // Recheche de l'index ds "peNumClassikAnim" de l'anim
   TRACEDBG("DecObj->Element->peNbClassikAnim : %d\n", DecObj->Element->peNbClassikAnim);
   for (int i=0 ; i < DecObj->Element->peNbClassikAnim ; i++)
   { if (DecObj->Element->ClassikAnimInfos[i].iAnimID == aNumAnimClassik)
   	 {	aIndexAnimClassik = i;
     	TRACEDBG("aIndexAnimClassik : %d\n", aIndexAnimClassik);
        break;
     }
   }

   // On commence par afficher l'image n°0 de l'anim 
   DecObj->peNumClassikAnim[aIndexAnimClassik]=0;

   aAnimClassikMoving = true;
   aSpriteClassikToScreen = false;
}
//---------------------------------------------------------------------------

//=== Joue/Stop l'animation classique superposable ================================//
void __fastcall TWokStation::BtnPlayAnimClassikClick(TObject *Sender)
{	aSpriteClassikPlaying = !aSpriteClassikPlaying;
    if (!aSpriteClassikPlaying)
    {	BtnPlayAnimClassik->Caption = "Jouer Anim";
    } else
    {	BtnPlayAnimClassik->Caption = "Stopper Anim";
    }
}
//----------------------------------------------------------------------------//

