////////////////////////////////////////////////////////////////////////////////
//  fWorkStation :  fen�tre permettant de travailler                          //
//					  - sur l'animation ouverte  							  //
//					  - les caract�ristiques des objets/persos				  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


//--- Faisons faire C++ Builder ----------------------------------------------//
#include <vcl\vcl.h>
#pragma hdrstop
#include "fWorkStation.h"
#pragma resource "*.dfm"
TWokStation *WokStation;
//----------------------------------------------------------------------------//

#include "fprincipale.h"    	// fen�tre principale de l'�diteur
#include "fBarOutils.h"         // Barre d'outils et de renseignements sur l'�tat de l'anim
#include "fAnimView.h"          // fen�tre de visualisation de l'animation
#include "fMenuScript.h"
#include "DDLibEditor.h"
#include "uAnimsInfos.h"        // Ch'tite fen�tre d'informations au sujet des anims
#include "SpritesLib.h"			// Gestion des anims "classiques"
#include "GameDebug.h"			// Permet de TRACER le programme en mode release

//---------------------------------------------------------------------------

//=== Constructeur de la fiche WorkStation ===================================//
__fastcall TWokStation::TWokStation(TComponent* Owner)
	: TForm(Owner)
{   // Positionnement et taille de la fiche sur la fiche MDI principale
    Left = 577; Top = 0;
    Width = 392+44; Height = 692;
    // Bouton de chargement du script d�sactiv�
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

//=== Compile le script en cours en m�moire et charge toutes ses infos =======//
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
    // On Ajoute � la Combobox, le nom des anims
    CBListAnims->Clear();
    for (int i=0; i < Perso->ModelePerso->peNbAnim ; i++)
     { CBListAnims->Items->Add(Perso->ModelePerso->AnimSpriteLib[i]->Nom);
     }
}
//----------------------------------------------------------------------------//

//=== Contenu de la ComboxBox modifi� ========================================//
void __fastcall TWokStation::CBListAnimsChange(TObject *Sender)
{   // Regarde � partir de la map des IDAnim laquelle anim est s�lectionn�e
    AnsiString champs = CBListAnims->Text;
    // V�rifie si le champs existe bien dans la map
    if (!IDAnimation->IsCle(champs))  // ne devrait jamais arriver
      {  MessageBoxA(NULL,"Le nom d'animation n'est pas valable .... ???","Warning !",1);
      	 //exit;	// sort de la fonction
      }
    // Indique l'animation � afficher
    aNumAnimation = IDAnimation->NumID(champs);

    // Animation vers le Sud
    AnimDirection = S;
    BarOutils->SpdBtnS->Down = true;
    BarOutils->SpdBtnS->Enabled = true;

    //=== Dans le cas d'un PERSO ===
    if (AnimationObjet == OBJET_PERSOS)
    {  Perso->peDirection = AnimDirection;
       // Change les valeurs de la barre d'outils
       CalculeEtatPerso();   // Change des param�tres du personnage

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
       CalculeEtatDecors();   // Change des param�tres des d�cors

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

    // Anim � l'arr�t
    if (aPlayingAnimationState == OFF) aPlayingAnimationState = STOP;

    // Recadre la vue d'apr�s les dimensions de la nouvelle anim
    AnimView->RecadreView();

    // Met � jour le groupe Box des sprites
    MaJGBSpriteInfos();

    //=== met � jour le ComboBox CBTypeFrame ===
    MaJCBTypeFrame();

    //=== Si c'est un DECORS -> Bo�te d'infos compl�mentaires accessibles ===
    if (AnimationObjet == OBJET_DECORS)
    {  // Enable le BitBtnInfos si c'est une vrai anim
       if (!DecObj->Element->peShowTile) BitBtnInfos->Enabled = true;
       // Ou si c'est un TileDecors secondaire (diff�rent de ARRET1)
       else if (DecObj->peEtat!=ARRET1) BitBtnInfos->Enabled = true;

    } else
    { BitBtnInfos->Enabled = false;
    }
}
//----------------------------------------------------------------------------//

//=== Calcule l'�tat du personnage apr�s avoir chang� certaines mouvements ===//
//  => de cam�ra, ...
void __fastcall TWokStation::CalculeEtatPerso()
{
  // peDirection du personnage � modifier
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

  Perso->peEtat = aNumAnimation;      // peEtat du personnage � changer
  Perso->peNumSprite = 0;             // Retourne � la 1�re image
  Perso->ResultAction();              // Calcul : peNbSprite & peSprNumLib
}
//----------------------------------------------------------------------------//

//=== Calcule l'�tat du personnage apr�s avoir chang� certaines mouvements ===//
//  => de cam�ra, ...
void __fastcall TWokStation::CalculeEtatDecors()
{
  // peDirection du personnage � modifier
  DecObj->peDirection = AnimDirection;

  DecObj->peEtat = aNumAnimation;      // peEtat du personnage � changer
  DecObj->peNumSprite = 0;             // Retourne � la 1�re image
  DecObj->ResultAction();              // Calcul : peNbSprite & peSprNumLib
}
//----------------------------------------------------------------------------//

//=== Capture la zone au sol du peronnage ====================================//
void __fastcall TWokStation::ButtonZoneAuSol2Click(TObject *Sender)
{ // Indique le mode utilis�
  AnimView->workMode = TAnimView::RECTANGLE;   // Zone <=> rectangle
  AnimView->workZone = TAnimView::ZONESOL;
  AnimView->P1x = AnimView->P2x = AnimView->P1y = AnimView->P2y = -1;  // Points initialis�s
}
//----------------------------------------------------------------------------//

//=== Mise � Jour du Groupe-Box Sprite-Infos =================================//
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

//=== mise � jour le ComboBox CBTypeFrame ====================================//
void __fastcall TWokStation::MaJCBTypeFrame()
{  if (AnimationObjet == OBJET_PERSOS)
   {   // R�cup�re le n�ID de l'anim s�lectionn�
       int numIDAnimation = IDAnimation->NumID(CBListAnims->Text);
       // R�cup�re le n�ID de la frame associ�e � cette anim � partir du sprite courant affich�
       SpriteTilePerso *Sprite = (SpriteTilePerso*) Perso->ModelePerso->STLPersonnage->SprIndex[Perso->peSprNumLib];
       int numIDFrameAnim = Sprite->pFrame->IDFrame;

       // Si l'utilisateur a s�lectionn� un membre
       // Alors on affiche les frame que l'on peut y supperposer
       if (ListBoxMembres->ItemIndex != -1)
       { // R�cup�re le n�ID du membre s�lectionn�
         AnsiString texte = ListBoxMembres->Items->Strings[ListBoxMembres->ItemIndex];
         int numIDMemb = IDMembres->NumID(texte);
         // Compl�te la ComboBox CBTypeFrame
         int nbTypeFrame = 0;
         CBTypeFrame->Clear();
         for (int i=0; i < Perso->ModelePerso->ObjetFrame[numIDMemb].TabObjetFrame.size(); i++)
         { if (Perso->ModelePerso->ObjetFrame[numIDMemb].TabObjetFrame[i].numIDFrameOriginal == numIDFrameAnim)
           { int numtypeobj = Perso->ModelePerso->ObjetFrame[numIDMemb].TabObjetFrame[i].numIDTypeObjet;
             CBTypeFrame->Items->Add(IDTypeObjSprite->CleID(numtypeobj));
             nbTypeFrame++;
           }
         }
         // Affiche le nombre de frame trouv�
         LabelNbFrame->Caption = nbTypeFrame;
       }
   }
}
//----------------------------------------------------------------------------//

void __fastcall TWokStation::CBListTypePersos2Change(TObject *Sender)
{ // Regarde le type de sprite choisi
  AnsiString TypePersos = CBListTypePersos->Text;
  if ( TypePersos == "HUMAIN" )
  { ShowZoneAuSol->Enabled = true;  ButtonZoneAuSol->Enabled = true;  // Zone au sol � prendre
    ShowZonePied->Enabled = true;  ButtonZonePied->Enabled = true;    // Zone des pieds
    ShowZoneAuSolPrec->Enabled = true;                                // Zone au sol � prendre du sprite pr�c�dent
    ShowZonePiedPrec->Enabled = true;                                 // Zone au sol des pieds pr�c�dent � voir
    ShowGravAuSol->Enabled = true;  ButtonGravAuSol->Enabled = true;  // Centre de gravit� projet� au sol � prendre
    ShowTrapPrinc->Enabled = false;                                   // Trap�ze principal pour les objets anim�s
    // Change le type de sprite du personnage
    Perso->ModelePerso->typePerso = IDPersosType->NumID(TypePersos);
  }
  else
  if ( TypePersos == "OBJET_ANIME" )
  { ShowZoneAuSol->Enabled = false;  ButtonZoneAuSol->Enabled = false;  // Zone au sol � prendre
    ShowZonePied->Enabled = false;  ButtonZonePied->Enabled = false;
    ShowZoneAuSolPrec->Enabled = false;                                 // Zone au sol � prendre du sprite pr�c�dent
    ShowZonePiedPrec->Enabled = false;  
    ShowGravAuSol->Enabled = true;  ButtonGravAuSol->Enabled = true;    // Centre de gravit� projet� au sol � prendre
    ShowTrapPrinc->Enabled = true;                                      // Trap�ze principal pour les objets anim�s
  }
  else
  { ShowZoneAuSol->Enabled = false;  ButtonZoneAuSol->Enabled = false;   // Zone au sol non primordiale
    ShowZonePied->Enabled = false;  ButtonZonePied->Enabled = false; 
    ShowZoneAuSolPrec->Enabled = false;                                  // Zone au sol non primordiale du sprite pr�c�dent
    ShowZonePiedPrec->Enabled = false; 
    ShowGravAuSol->Enabled = true;  ButtonGravAuSol->Enabled = true;     // Centre de gravit� projet� au sol � prendre
    ShowTrapPrinc->Enabled = false;                                      // Trap�ze principal pour les objets anim�s
  }


}
//----------------------------------------------------------------------------//

//=== Capture le centre de gravit� au sol du peronnage =======================//
void __fastcall TWokStation::ButtonGravAuSolClick(TObject *Sender)
{ // Indique le mode utilis�
  AnimView->workMode = TAnimView::POINT;   // Zone <=> rectangle
  AnimView->workZone = TAnimView::PT_GRAV_SOL;
  AnimView->P1x = AnimView->P1y = -1;  // Points initialis�s
}
//----------------------------------------------------------------------------//



//=== Quand l'utilisateur s�lectionne un membre ==============================//
void __fastcall TWokStation::ListBoxMembresClick(TObject *Sender)
{  //=== met � jour le ComboBox CBTypeFrame ===
   MaJCBTypeFrame();
}
//----------------------------------------------------------------------------//



//=== S�lection du type d'objet � calquer sur l'anim =========================//
void __fastcall TWokStation::CBTypeFrameChange(TObject *Sender)
{  // R�cup�re le num�ro du type d'objet s�lectionn�
   int numIDTypeObjSprite = IDTypeObjSprite->NumID(CBTypeFrame->Text);
   // R�cup�re le num�ro du membre s�lectionn�
   int numIDMemb = IDMembres->NumID(ListBoxMembres->Items->Strings[ListBoxMembres->ItemIndex]);


   // Regarde dans le PersoSprite courant si ce sprite superposable n'a pas
   // d�j� �t� supperpos� par ailleurs
   SpriteTilePerso *Sprite = (SpriteTilePerso*) Perso->ModelePerso->STLPersonnage->SprIndex[Perso->peSprNumLib];
   if (Sprite->GSprInf->ListAnimSuperp.Existe(numIDTypeObjSprite))
   {  // Si le sprite a d�j� �t� plac� auparavant -> on va devoir l'afficher
      LabelPosition->Caption =  "OUI";
/*      int posX, posY;
      Sprite->GSprInf->ListAnimSuperp.GetCoord(numIDTypeObjSprite, posX, posY);*/
      aSpriteSuperposeeToScreen = true;   // On peut afficher un sprite supperposable

   } else
   {  // Sinon on ?????
      LabelPosition->Caption =  "NON";

      //*** Affichage du sprite superposable � l'�cran ***
      // On r�cup�re le n� de la frame sur lequel il doit �tre
      int numIDFrameSuperp = Perso->ModelePerso->ObjetFrame[numIDMemb].Search(numIDTypeObjSprite);
      // On parcours la liste des sprites superposables jusqu'� trouver le dernier correspondant
      // au num�ro numIDFrameSuperp
      SpriteTile* SpriteSuperp;
      SpriteSuperp = Perso->ModelePerso->STLFrameSuperp->SL_List; // dernier sprite
      bool trouve = false;
      for (int i = Perso->ModelePerso->STLFrameSuperp->NbSprites-1  ;  i >=0 || !trouve ; i--)
      {   if (SpriteSuperp->pFrame->IDFrame == numIDFrameSuperp)
          {  trouve = true;
          }
          else SpriteSuperp = (SpriteTile*) SpriteSuperp->Suiv;
      };
      // Si on ne l'a pas trouve, c'est qu'il ya probl�me
      if (!trouve) MessageBoxA(NULL,"le n� de sprite n'a pas �t� trouv�","Error : TWokStation::CBTypeFrameChange",1);
      aSpriteSuperposeeTileEnd = SpriteSuperp;
      // Remonte au d�but de la frame
      int nbSpritesParFrame = SpriteSuperp->pFrame->NbSprites;
      for (int i=0 ; i < nbSpritesParFrame-1 ; i++) SpriteSuperp = (SpriteTile*) SpriteSuperp->Suiv;
      aSpriteSuperposeeTileDeb = SpriteSuperp;

      // Remonte jusqu'au bon sprite d'apres la cam�ra et le n� du sprite de l'anim en cours
      int numSprite = Perso->peNumSprite;  // n� de sprite de l'anim en cours
      int posCamera = Perso->peDirection;  // position de la cam�ra
      int nbSpriteParAnim = nbSpritesParFrame / 8;
      int nbSpriteARemonter = posCamera*nbSpriteParAnim + numSprite;
      for (int i=0; i < nbSpriteARemonter ; i++)
      {  SpriteSuperp = (SpriteTile*) SpriteSuperp->Prec;
      }
      // Pr�t � l'afficher
      aSpriteSuperposeeToScreen = true;   // On peut afficher un sprite supperposable
      aSpriteSuperposeeMoving   = true;   // On peut d�placer un sprite supperposable
      aSpriteSuperposeeNumero = SpriteSuperp->NumSprite;
      aSpritePersoNumAnim = Perso->peNumSprite;
   }
   // Verifie si le bmp est en m�moire
   int numIDFrameSuperp = Perso->ModelePerso->ObjetFrame[numIDMemb].Search(numIDTypeObjSprite);
   SpriteFrame *sprFrame = Perso->ModelePerso->FrameSuperpList->SF_List;
   while ((sprFrame!=NULL) && (sprFrame->IDFrame !=numIDFrameSuperp))
   { sprFrame = sprFrame->SF_Suiv;
   }
   if (sprFrame==NULL) MessageBoxA(NULL,"le n� de frame n'a pas �t� trouv�","Error : TWokStation::CBTypeFrameChange",1);
   else
   if (!sprFrame->BmpLoadIntoMemory)
   {  sprFrame->ReserveMemoryAndLoadBmp(&(DS_VA->pDD));
   }
}
//----------------------------------------------------------------------------//

//=== Repositionner un sprite superposable ===================================//
void __fastcall TWokStation::AnimSupepPositClick(TObject *Sender)
{   // R�cup�re le num�ro du type d'objet s�lectionn�
   if (CBTypeFrame->Text=="") return;
   int numIDTypeObjSprite = IDTypeObjSprite->NumID(CBTypeFrame->Text);
   // R�cup�re le num�ro du membre s�lectionn�
   if (ListBoxMembres->ItemIndex==-1) return;
   int numIDMemb = IDMembres->NumID(ListBoxMembres->Items->Strings[ListBoxMembres->ItemIndex]);

   // Regarde dans le PersoSprite courant si ce sprite superposable n'a pas
   // d�j� �t� supperpos� par ailleurs
   SpriteTilePerso *Sprite = (SpriteTilePerso*) Perso->ModelePerso->STLPersonnage->SprIndex[Perso->peSprNumLib];
   LabelPosition->Caption =  "Repositionnement";
      //*** Affichage du sprite superposable � l'�cran ***
      // On r�cup�re le n� de la frame sur lequel il doit �tre
      int numIDFrameSuperp = Perso->ModelePerso->ObjetFrame[numIDMemb].Search(numIDTypeObjSprite);
      // On parcours la liste des sprites superposables jusqu'� trouver le dernier correspondant
      // au num�ro numIDFrameSuperp
      SpriteTile* SpriteSuperp;
      SpriteSuperp = Perso->ModelePerso->STLFrameSuperp->SL_List; // dernier sprite
      bool trouve = false;
      for (int i = Perso->ModelePerso->STLFrameSuperp->NbSprites-1  ;  i >=0 || !trouve ; i--)
      {   if (SpriteSuperp->pFrame->IDFrame == numIDFrameSuperp)
          {  trouve = true;
          }
          else SpriteSuperp = (SpriteTile*) SpriteSuperp->Suiv;
      };
      // Si on ne l'a pas trouve, c'est qu'il ya probl�me
      if (!trouve) MessageBoxA(NULL,"le n� de sprite n'a pas �t� trouv�","Error : TWokStation::CBTypeFrameChange",1);
      aSpriteSuperposeeTileEnd = SpriteSuperp;
      // Remonte au d�but de la frame
      int nbSpritesParFrame = SpriteSuperp->pFrame->NbSprites;
      for (int i=0 ; i < nbSpritesParFrame-1 ; i++) SpriteSuperp = (SpriteTile*) SpriteSuperp->Suiv;
      aSpriteSuperposeeTileDeb = SpriteSuperp;

      // Remonte jusqu'au bon sprite d'apres la cam�ra et le n� du sprite de l'anim en cours
      int numSprite = Perso->peNumSprite;  // n� de sprite de l'anim en cours
      int posCamera = Perso->peDirection;  // position de la cam�ra
      int nbSpriteParAnim = nbSpritesParFrame / 8;
      int nbSpriteARemonter = posCamera*nbSpriteParAnim + numSprite;
      for (int i=0; i < nbSpriteARemonter ; i++)
      {  SpriteSuperp = (SpriteTile*) SpriteSuperp->Prec;
      }
      // Si le bmp est charg�
      aSpriteSuperposeeToScreen = true;   // On peut afficher un sprite supperposable
      aSpriteSuperposeeMoving   = true;   // On peut d�placer un sprite supperposable
      aSpriteSuperposeeNumero = SpriteSuperp->NumSprite;
      aSpritePersoNumAnim = Perso->peNumSprite;
}
//----------------------------------------------------------------------------//

//=== Ouvre une ch'tite fen�tre d'informations ===============================//
void __fastcall TWokStation::BitBtnInfosClick(TObject *Sender)
{ AnimsInfos->Show();

}
//----------------------------------------------------------------------------//

//=== Capture la zone des pieds du peronnage =================================//
void __fastcall TWokStation::ButtonZonePiedClick(TObject *Sender)
{ // Indique le mode utilis�
  AnimView->workMode = TAnimView::RECTANGLE_ORIENTE;   // Zone <=> rectangle pouvant prendre 8 orientations diff�rentes
  AnimView->workZone = TAnimView::ZONEPIED;
  AnimView->P1x = AnimView->P2x = AnimView->P1y = AnimView->P2y = -1;  // Points initialis�s
}
//---------------------------------------------------------------------------




//=== S�lection d'une anim superposable classique ============================//
void __fastcall TWokStation::ListBoxAnimsClick(TObject *Sender)
{
    // Affiche les coordonn�es 
    AnsiString animID = ListBoxAnims->Items->Strings[ListBoxAnims->ItemIndex];
    aNumAnimClassik = IDAnimation->NumID(animID);
	SClassikAnimInfos* animInfos = DecObj->Element->GetClassikAnim(aNumAnimClassik);
    LabelAnimX->Caption = animInfos->dX;
    LabelAnimY->Caption = animInfos->dY;

    // R�cup�re un pointeur sur l'anim s�lectionn�e
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

   // On commence par afficher l'image n�0 de l'anim 
   DecObj->peNumClassikAnim[aIndexAnimClassik]=0;

   aAnimClassikMoving = false;
   aSpriteClassikToScreen = true;
   BtnPlayAnimClassik->Enabled = true;
}
//----------------------------------------------------------------------------//

void __fastcall TWokStation::BtnPositionnerAnimClick(TObject *Sender)
{  // R�cup�re le num�ro de l'anim s�lectionn�e
   if (ListBoxAnims->ItemIndex==-1) return;
   AnsiString animID = ListBoxAnims->Items->Strings[ListBoxAnims->ItemIndex];
   aNumAnimClassik = IDAnimation->NumID(animID);

   // R�cup�re un pointeur sur l'anim s�lectionn�e
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

   // On commence par afficher l'image n�0 de l'anim 
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

