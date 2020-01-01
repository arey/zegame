/////////////////////////////////////////////////////////////////////////////////
//  fAnimView :  fenêtre représentant la vue d'une animation 		      //
//				 que l'on pourra manier dans tous les sens    //
////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#include <math>
#pragma hdrstop
#include "fAnimView.h"

#include "DDLibEditor.h"		// Librairie complémentaire à DDLib utilisée ds les editeurs
#include "fPrincipale.h"		// Accés aux variables globales
#include "fBarOutils.h"         // Barre d'outils
#include "fWorkStation.h"       // Fiche de travail
#include "uAnimsInfos.h"        // Fenêtre d'informations concernant les décors
#include "SpritesLib.h"
#include "GameDebug.h"

//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TAnimView *AnimView;


int ScrollViewRate = 10;  // Frame rate du rafraichissement de la fenêtre d'animation

// Résolution globale de la vue directdraw            OffsetRect
int eScreenViewX = 400, eScreenViewY = 380;
// Fenêtre directdraw visible
int eScreenVisibleX = 400, eScreenVisibleY = 380;

//---------------------------------------------------------------------------

//=== Constructeur de la fiche ===============================================//
__fastcall TAnimView::TAnimView(TComponent* Owner)
	: TForm(Owner)
{
    // Positionnement de la fiche sur la fiche MDI principale
    Left = 158; Top = 0;
    ClientWidth  = eScreenVisibleX+13;		// +13 pour les scroll-bars
    ClientHeight = eScreenVisibleY+13;
    // Positionnement des Scrollbars
    ScrollViewVert->Left = eScreenVisibleX;
    ScrollViewVert->Height = eScreenVisibleY;
    ScrollViewVert->Top = 0;
    ScrollViewHor->Left = 0;
    ScrollViewHor->Top = eScreenVisibleY;
    ScrollViewHor->Width = eScreenVisibleX;


    // Initialise la fenêtre DirectDraw
    if (DS_VA!=NULL) delete DS_VA;		// vérifie si elle n'est pas déjà créée
    DS_VA = new TDirectScreen_Editor(AnimView->Handle);
    DS_VA->InitDDraw(eScreenViewX, eScreenViewY);

    // Initialise DI
//    DI = new TDirectInput(WindowGame->Handle);
//    DI->CreateDevice(TRUE,FALSE,FALSE);

    // Initialise la Librairie
    SFL = new SpriteFrameList();

    STGL = new SpriteTileGameLib();	// Liste officielle des sprites ne faisant en particulier ni partie des objets, ni des persos
    STGL->LoadFileSpriteIna("AllSprites");
  

    // Ini du Timer
    RefreshFreq = 33;	// fréquence de rafraîchissement de la fenêtre
    TimerFlip->Interval = 1000 / RefreshFreq;

    // Ini d'autres paramètres
    ScrollHor = ScrollVert = 0;
    ScreenX = ScreenY = 0;
    Zoom = 1; // Valeur par défaut au départ

    workMode = NONE;    // ne travaille pas
}
//----------------------------------------------------------------------------//


//=== effectue quelques petits changements lors du rafraîchissement ==========//
void __fastcall TAnimView::UpGradeScreen()
{    // Barres de défilement
     ScreenX = ScrollViewHor->Position;
     ScreenY = ScrollViewVert->Position;

     // Recadre les barres de défilement
     int newAlignScrollPos = (ScrollViewHor->Position / Zoom);
     ScrollViewHor->Position = newAlignScrollPos*Zoom;
     newAlignScrollPos = (ScrollViewVert->Position / Zoom);
     ScrollViewVert->Position = newAlignScrollPos*Zoom;

     // Champs & Captions
     if (aPlayingAnimationState != OFF)
     {    //=== Dans le cas d'un PERSO ===
          if (AnimationObjet == OBJET_PERSOS)
          { BarOutils->LabelNbSprites->Caption = Perso->peNumSprite;
            BarOutils->LabelSurSprites->Caption = Perso->peNbSprite;
            BarOutils->ScrollBarStep->Position = Perso->peNumSprite;
          } else
          //=== Dans le cas d'un DECORS ===
          if (AnimationObjet == OBJET_DECORS)
          { BarOutils->LabelNbSprites->Caption = DecObj->peNumSprite;
            BarOutils->LabelSurSprites->Caption = DecObj->peNbSprite;
            BarOutils->ScrollBarStep->Position = DecObj->peNumSprite;
          }
     }
}
//----------------------------------------------------------------------------//

//=== Affiche le layer des infos du sprite ===================================//
void __fastcall TAnimView::SpriteInfos()
{
    // Remonte jusqu'au bon sprite de la SpriteListe
    SpriteTilePerso *Sprite;
    HRESULT hr;

    if (AnimationObjet == OBJET_PERSOS)
    { Sprite = (SpriteTilePerso*) Charact->STLPersonnage->SprIndex[Perso->peSprNumLib];
    } else
    if (AnimationObjet == OBJET_DECORS)
    { Sprite = (SpriteTilePerso*) DecObj->Element->STLObjets->SprIndex[DecObj->peSprNumLib];
    }

    // 1) Affiche le centre de gravité du perso
    int gx, gy;
    if ((AnimationObjet == OBJET_DECORS) && (DecObj->Element->peShowTile))
    {  // Centre de gravité du TileDecors
       gx = DecObj->Element->MainTile->TrapPrincX;
       gy = DecObj->Element->MainTile->TrapPrincY;
    } else  // Centre de gravité du sprite
    {  gx = Sprite->GravX;
       gy = Sprite->GravY;
    }
    int pX = ((spriteX - ScreenX)) + gx*Zoom;
    int pY = ((spriteY - ScreenY)) + gy*Zoom;
    DS_VA->PutCroix(pX,pY,Zoom,RGBto16(0,255,0)); // Affiche une croix verte

    // 2) Affiche la forme qu'il est entrain de dessiner
    switch (workMode)
    {  case RECTANGLE : if (P1x != -1)  // Si l'utilisateur a commencé à dessiner un rectangle
          {  // Affiche le rectangle dessiné
             int mx = mouseScreenX/Zoom;
             int my = mouseScreenY/Zoom;
             DS_VA->PutRectangleClip(P1x*Zoom+spriteX-ScreenX,P1y*Zoom+spriteY-ScreenY,mx*Zoom,my*Zoom,Zoom,RGBto16(0,0,255));
          }; break;
       case RECTANGLE_ORIENTE :
          if (P1x != -1)  // Si l'utilisateur a commencé à dessiner un rectangle
          {  // Affiche le rectangle dessiné
             int mx = mouseScreenX/Zoom;
             int my = mouseScreenY/Zoom;
             DS_VA->PutRectangleOrienteClip(P1x*Zoom+spriteX-ScreenX,P1y*Zoom+spriteY-ScreenY,mx*Zoom,my*Zoom,Zoom,RGBto16(0,0,255),Perso->peDirection);
          }; break;
       case POINT : break;   // Pas besoin de dessiner un point
    };

    // 3) Affiche les caractéristiques demandées de l'anim
    //    a) Zone au sol (rectangle)
    if ((WokStation->ShowZoneAuSol->Down) && (Sprite->GSprInf->ZoneSol.Enabled))
      {  int p1x = Sprite->GSprInf->ZoneSol.x1;
         int p1y = Sprite->GSprInf->ZoneSol.y1;
         int p2x = Sprite->GSprInf->ZoneSol.x2;
         int p2y = Sprite->GSprInf->ZoneSol.y2;
         p1x = p1x*Zoom + spriteX - ScreenX;
         p1y = p1y*Zoom + spriteY - ScreenY;
         p2x = p2x*Zoom + spriteX - ScreenX;
         p2y = p2y*Zoom + spriteY - ScreenY;
         DS_VA->PutRectangleClip(p1x,p1y,p2x,p2y,Zoom,RGBto16(0,127,255));
      }
    //    a bis) Zone des pieds (rectangle)
    if ((WokStation->ShowZonePied->Down) && (Sprite->GSprInf->ZonePied.Enabled))
      {  int p1x = Sprite->GSprInf->ZonePied.x1;
         int p1y = Sprite->GSprInf->ZonePied.y1;
         int p2x = Sprite->GSprInf->ZonePied.x3;
         int p2y = Sprite->GSprInf->ZonePied.y3;
         p1x = p1x*Zoom + spriteX - ScreenX;
         p1y = p1y*Zoom + spriteY - ScreenY;
         p2x = p2x*Zoom + spriteX - ScreenX;
         p2y = p2y*Zoom + spriteY - ScreenY;
         DS_VA->PutRectangleOrienteClip(p1x,p1y,p2x,p2y,Zoom,RGBto16(0,127,255),Perso->peDirection);
      }
    //     b) Affiche le Centre de gravité du persoprojeté au sol
    if ((WokStation->ShowGravAuSol->Down) && (Sprite->GSprInf->GravSol.Enabled))
      {   int p1x = Sprite->GSprInf->GravSol.x;
          int p1y = Sprite->GSprInf->GravSol.y;
          int pX = spriteX - ScreenX + p1x*Zoom;
          int pY = spriteY - ScreenY + p1y*Zoom;
          DS_VA->PutCroix(pX,pY,Zoom,RGBto16(0,127,127)); // Affiche une croix en vert-grisé
      }
   //     c) Affichage de la zone au sol du sprite précédent
   if (WokStation->ShowZoneAuSolPrec->Down)
      {  if ((Perso->peNumSprite>0) && (Sprite->Suiv!=NULL) && (((SpriteTilePerso*) Sprite->Suiv)->GSprInf->ZoneSol.Enabled))
         { int p1x = ((SpriteTilePerso*) Sprite->Suiv)->GSprInf->ZoneSol.x1;
           int p1y = ((SpriteTilePerso*) Sprite->Suiv)->GSprInf->ZoneSol.y1;
           int p2x = ((SpriteTilePerso*) Sprite->Suiv)->GSprInf->ZoneSol.x2;
           int p2y = ((SpriteTilePerso*) Sprite->Suiv)->GSprInf->ZoneSol.y2;
           p1x = p1x*Zoom + spriteX - ScreenX;
           p1y = p1y*Zoom + spriteY - ScreenY;
           p2x = p2x*Zoom + spriteX - ScreenX;
           p2y = p2y*Zoom + spriteY - ScreenY;
           DS_VA->PutRectangleClip(p1x,p1y,p2x,p2y,Zoom,RGBto16(0,255,196));
        }
      }
   //     c bis) Affichage de la zone des pieds du sprite précédent
   if (WokStation->ShowZonePiedPrec->Down)
      {  if ((Perso->peNumSprite>0) && (Sprite->Suiv!=NULL) && (((SpriteTilePerso*) Sprite->Suiv)->GSprInf->ZonePied.Enabled))
         { int p1x = ((SpriteTilePerso*) Sprite->Suiv)->GSprInf->ZonePied.x1;
           int p1y = ((SpriteTilePerso*) Sprite->Suiv)->GSprInf->ZonePied.y1;
           int p2x = ((SpriteTilePerso*) Sprite->Suiv)->GSprInf->ZonePied.x3;
           int p2y = ((SpriteTilePerso*) Sprite->Suiv)->GSprInf->ZonePied.y3;
           p1x = p1x*Zoom + spriteX - ScreenX;
           p1y = p1y*Zoom + spriteY - ScreenY;
           p2x = p2x*Zoom + spriteX - ScreenX;
           p2y = p2y*Zoom + spriteY - ScreenY;
           DS_VA->PutRectangleOrienteClip(p1x,p1y,p2x,p2y,Zoom,RGBto16(0,255,196),Perso->peDirection);
        }
      }

   //     d) Affichage du trapèze principal d'un objet décors
   if ((WokStation->ShowTrapPrinc->Down) && (AnimationObjet == OBJET_DECORS))
   {  // Si c'est un TileDecors
      if (DecObj->Element->peShowTile)
      {   int scX = (spriteX - ScreenX) + DecObj->Element->MainTile->TrapPrincX*Zoom;
          int scY = (spriteY - ScreenY) + DecObj->Element->MainTile->TrapPrincY*Zoom;
          DS_VA->BackZoomISOTrapeze(scX, scY, RGBto16(255,255,255), TrapTYPE_DOUBLE, Zoom);
      }
   }
}
//----------------------------------------------------------------------------//

//=== Evenement du timer =====================================================//
void __fastcall TAnimView::TimerFlipTimer(TObject *Sender)
{
    DS_VA->Fill(32000);       // Efface le fond
    if (aPlayingAnimationState != OFF)
      {
          //=== Dans le cas d'un PERSO ===
          if (AnimationObjet == OBJET_PERSOS)
          { // Fait avancer le personnage
            if (aPlayingAnimationState == PLAY) Perso->peBouge();
            else Perso->ResultAction();

            int scX = (spriteX - ScreenX)/Zoom;
            scX*=Zoom;
            int scY = (spriteY - ScreenY)/Zoom;
            scY*=Zoom;
            SpriteDrawEditor( scX, scY,Zoom,Perso->peSprNumLib,DS_VA->pDDSBack,Charact->STLPersonnage);

            UpGradeScreen();     	         // Apporte quelques modifs au contenu de l'écran
            MouseInfos();			 // Calcule les nouveaux paramètres de la souris

            SpriteInfos();                 // Remplit la vue avec toutes les infos du sprite disponibles
         }
         //=== Dans le cas d'un DECORS ===
         if (AnimationObjet == OBJET_DECORS)
         {  // Fait avancer le personnage
            if (aPlayingAnimationState == PLAY) DecObj->peBouge();
            else DecObj->ResultAction();

            int scX = (spriteX - ScreenX)/Zoom;
            scX*=Zoom;
            int scY = (spriteY - ScreenY)/Zoom;
            scY*=Zoom;
            SpriteDecorsDrawEditor( scX, scY, Zoom,DS_VA->pDDSBack, DecObj);

            UpGradeScreen();        // Apporte quelques modifs au contenu de l'écran
            MouseInfos();	    // Calcule les nouveaux paramètres de la souris
            SpriteInfos();          // Remplit la vue avec toutes les infos du sprite disponibles
         }
      }

    //=== On doit aussi afficher un sprite à supperposer ===
    if (aSpriteSuperposeeToScreen)
    { // Cas du Perso
      if (AnimationObjet == OBJET_PERSOS)
      { // Si l'on est en modification -> on doit le positionnner sur le pointeur
        SpriteTile *SpriteSuperp;
        // de la souris sur le centre de gravité du sprite superposable
        if (aSpriteSuperposeeMoving)
        {  // Recherche de la largeur et de la hauteur du sprite
           SpriteSuperp = Charact->STLFrameSuperp->SprIndex[aSpriteSuperposeeNumero];
           int Hauteur = SpriteSuperp->GravY*Zoom;
           int Largeur = SpriteSuperp->GravX*Zoom;
           SpriteDrawEditor(mouseScreenX-Largeur,mouseScreenY-Hauteur,Zoom,aSpriteSuperposeeNumero,DS_VA->pDDSBack,Charact->STLFrameSuperp);
        } else // On regarde si il n'y a pas un sprite superposable à afficher
        { // Récupère le numéro du type d'objet sélectionné
          int numIDTypeObjSprite = IDTypeObjSprite->NumID(WokStation->CBTypeFrame->Text);
          SpriteTilePerso *SpritePers = (SpriteTilePerso*) Charact->STLPersonnage->SprIndex[Perso->peSprNumLib];
          // Récupère le bon numéro de sprite à afficher
          int num = SpritePers->GSprInf->ListAnimSuperp.GetIDS(numIDTypeObjSprite);
          SpriteSuperp = Charact->STLFrameSuperp->SprIndex[num];
          // Regarde dans le PersoSprite courant si ce sprite superposable n'a pas déjà été supperposé par ailleurs
          if (SpritePers->GSprInf->ListAnimSuperp.Existe(numIDTypeObjSprite))
          {  int posX, posY;
             SpritePers->GSprInf->ListAnimSuperp.GetCoord(numIDTypeObjSprite, posX, posY);
             int numIDSprSuperp = SpritePers->GSprInf->ListAnimSuperp.GetIDS(numIDTypeObjSprite);
             int pX = (spriteX-ScreenX) + (SpritePers->GravX + posX - SpriteSuperp->GravX)*Zoom;
             int pY = (spriteY-ScreenY) + (SpritePers->GravY + posY - SpriteSuperp->GravY)*Zoom;
             SpriteDrawEditor(pX,pY,Zoom,numIDSprSuperp,DS_VA->pDDSBack,Charact->STLFrameSuperp);
          }
        }
      }
    }

    //=== On doit afficher un TileDecors ===
    if (aTileDecorsToScreen)
    { // Si il est visible
      if (aTileDecorsIsVisible)
      { // Recherche du bon TileDecors sélectionné parmi ceux du decors
        TileDecors* TileAssoc;
        for (int i=0; i < DecObj->Element->peNbAnim ; i++)
        { if (IDAnimation->NumID(DecObj->Element->AnimSpriteLib[i]->Nom)== AnimsInfos->numAnimTileDecors)
          {  TileAssoc = DULib->getTileDecors(DecObj->Element->AnimSpriteLib[i]->FirstSprite);
          }
        }
        // Recherche de la "largeur et de la hauteur" du sprite
        int Hauteur = TileAssoc->TrapPrincY*Zoom;
        int Largeur = TileAssoc->TrapPrincX*Zoom;
        TileDecorsDrawEditor(mouseScreenX-Largeur,mouseScreenY-Hauteur,Zoom,TileAssoc,DS_VA->pDDSBack);
      }
    }

    //=== Affichage d'un sprite superposable "classique" à un objet animé ===
    if (aSpriteClassikToScreen)
    {
        // Récupère quelques infos sur l'animation & les sprites
        AnimSprite* animsprite = STGL->GetAnimSprite(aNumAnimClassik);
        int numSprite = animsprite->FirstSprite + DecObj->peNumClassikAnim[aIndexAnimClassik];
        aSpriteClassik = STGL->GetSpriteTile(numSprite);
        SClassikAnimInfos*	animInfos = DecObj->Element->GetClassikAnim(aNumAnimClassik);
   
        // Recherche du centre de Gravité du sprite du décors affiché
        SpriteTile* sprDec = (SpriteTile*) DecObj->Element->STLObjets->SprIndex[DecObj->peSprNumLib];
        int gx = 0, gy = 0;
        if ((AnimationObjet == OBJET_DECORS) && (DecObj->Element->peShowTile))
        {  // Centre de gravité du TileDecors
           gx = DecObj->Element->MainTile->TrapPrincX;
           gy = DecObj->Element->MainTile->TrapPrincY;
        } else  // Centre de gravité du sprite
        {  gx = sprDec->GravX;
           gy = sprDec->GravY;
        }
        int scX = ((spriteX - ScreenX)/Zoom)*Zoom + gx*Zoom;
        int scY = ((spriteY - ScreenY)/Zoom)*Zoom + gy*Zoom;
        scX -= aSpriteClassik->GravX*Zoom;
        scY -= aSpriteClassik->GravY*Zoom;
        scX += animInfos->dX*Zoom;
        scY += animInfos->dY*Zoom;
        ClassikSpriteDrawEditor(scX, scY, Zoom, numSprite, DS_VA->pDDSBack, STGL);
    } else
    if (aAnimClassikMoving)
    {	// On doit le positionnner sur le pointeur de la souris sur le centre de gravité du sprite "classique" superposable
        // Récupère quelques infos sur l'animation & les sprites
        AnimSprite* animsprite = STGL->GetAnimSprite(aNumAnimClassik);
        int numSprite = animsprite->FirstSprite + DecObj->peNumClassikAnim[aIndexAnimClassik];
        aSpriteClassik = STGL->GetSpriteTile(numSprite);
        SClassikAnimInfos*	animInfos = DecObj->Element->GetClassikAnim(aNumAnimClassik);
        int Hauteur = aSpriteClassik->GravX*Zoom;
        int Largeur = aSpriteClassik->GravY*Zoom;
        ClassikSpriteDrawEditor(mouseScreenX-Largeur, mouseScreenY-Hauteur, Zoom, numSprite, DS_VA->pDDSBack, STGL);
   }

   // Si l'animation d'une anim classique est en cours
   if (aSpriteClassikPlaying)
   {
   		DecObj->peNumClassikAnim[aIndexAnimClassik]++;  // Frame suivante de l'anim
        // Si l'on est arrivé au bout, on revient au début
        AnimSprite* animsprite = STGL->GetAnimSprite(aNumAnimClassik);
   		if (DecObj->peNumClassikAnim[aIndexAnimClassik] >= animsprite->NbSprites)
        {	DecObj->peNumClassikAnim[aIndexAnimClassik] =0;
        }
   }

    DS_VA->FlipSurfaces();
}
//----------------------------------------------------------------------------//

//=== Barres de Scrollings Verticale utilisée ================================//
void __fastcall TAnimView::ScrollViewVertScroll(TObject *Sender,
	TScrollCode ScrollCode, int &ScrollPos)
{ int newAlignScrollPos = (ScrollPos / Zoom);
  newAlignScrollPos *=Zoom;
  ScrollViewVert->Position = newAlignScrollPos;
  ScreenY = newAlignScrollPos;
}
//----------------------------------------------------------------------------//

//=== Barres de Scrollings Horizontale utilisée ==============================//
void __fastcall TAnimView::ScrollViewHorScroll(TObject *Sender,
	TScrollCode ScrollCode, int &ScrollPos)
{  int newAlignScrollPos = (ScrollPos / Zoom);
   newAlignScrollPos *=Zoom;
   ScrollViewHor->Position = newAlignScrollPos;
   ScreenX = newAlignScrollPos;
}
//----------------------------------------------------------------------------//

///// SpriteDrawEditor /////////////////////////////////////////////////////////
// Nouvelle routine d'affichage de sprites exprés pour th éditor ///////////////
void SpriteDrawEditor(int scX2,int scY2,double Zoom,int NumSprite,DDSurface *pSurface,SpriteTileLib* STLperso)
{
    if ((NumSprite<0) || (STLperso==NULL) || (STLperso->SL_List==NULL)) return;  // vérifications
    if (NumSprite>=STLperso->NbSprites) return;

    // Remonte jusqu'au bon sprite de la SpriteListe
    SpriteTile *Sprite;
    HRESULT hr;
    Sprite = STLperso->SprIndex[NumSprite];
    RECT DestRect;

    // ATTENTION BUG ? ScY doit être égal à ZERO
    RECT Backup = Sprite->SrcRect;
    Backup.bottom -=2; //Zoom;
    Backup.right -= 1;//Zoom;

    DestRect.top = scY2;
    DestRect.left = scX2;
    DestRect.bottom = scY2 + ((Backup.bottom-Backup.top) * Zoom);
    DestRect.right  = scX2 + ((Backup.right-Backup.left) * Zoom);
    // Rectifie le tir
    // Vérifie si c'est la peine d'afficher une image
    if (    (DestRect.left < eScreenVisibleX) && (DestRect.top < eScreenVisibleY)
         && (DestRect.right > 0) && (DestRect.bottom >0 ) )
       {
          // Gère le clipping
          if (DestRect.top < 0) { Backup.top -= DestRect.top/Zoom; DestRect.top = 0; };
          if (DestRect.bottom >= eScreenVisibleY) { Backup.bottom += (eScreenVisibleY - DestRect.bottom)/Zoom; DestRect.bottom = (eScreenVisibleY/Zoom); DestRect.bottom*=Zoom; };
          if (DestRect.left < 0) { Backup.left -= DestRect.left/Zoom; DestRect.left = 0; };
          if (DestRect.right >= eScreenVisibleX){ Backup.right += (eScreenVisibleX - DestRect.right)/Zoom; DestRect.right = (eScreenVisibleX/Zoom);DestRect.right*=Zoom; };

          DestRect.bottom = DestRect.top + ((Backup.bottom-Backup.top) * Zoom);
          DestRect.right  = DestRect.left + ((Backup.right-Backup.left) * Zoom);


          hr = pSurface->pDDSurf->Blt(&DestRect,Sprite->pFrame->pDDSurf,&Backup,DDBLT_WAIT | DDBLT_KEYSRC , NULL);
          // if FAILED(hr) TraceErrorDD(hr,99);
      }
};
////////////////////////////////////////////////////////////////////////////////

///// SpriteDecorsDrawEditor ///////////////////////////////////////////////////
// Nouvelle routine d'affichage de sprites exprés pour th éditor ///////////////
void SpriteDecorsDrawEditor(int scX2,int scY2,double Zoom, DDSurface *pSurface,DecorsObjet *drawObj)
{   // Vérifs
    if ((drawObj==NULL) || (drawObj->Element==NULL)) return;

    HRESULT hr;
    RECT DestRect, Backup;
    SpriteTile *Sprite;

    if (drawObj->Element->peShowTile)
    { // Affichage d'un TileDecors
      Backup = drawObj->Element->MainTile->TileSurface->SrcRect;
    } else
    { // Remonte jusqu'au bon sprite de la SpriteListe
      Sprite = DecObj->Element->STLObjets->SprIndex[DecObj->peSprNumLib];
      Backup = Sprite->SrcRect;
    }

    // ATTENTION BUG ? ScY doit être égal à ZERO

    Backup.bottom -=2; //Zoom;
    Backup.right -= 1;//Zoom;

    DestRect.top = scY2;
    DestRect.left = scX2;
    DestRect.bottom = scY2 + ((Backup.bottom-Backup.top) * Zoom);
    DestRect.right  = scX2 + ((Backup.right-Backup.left) * Zoom);
    // Rectifie le tir
    // Vérifie si c'est la peine d'afficher une image
    if (    (DestRect.left < eScreenVisibleX) && (DestRect.top < eScreenVisibleY)
         && (DestRect.right > 0) && (DestRect.bottom >0 ) )
       {
          // Gère le clipping
          if (DestRect.top < 0) { Backup.top -= DestRect.top/Zoom; DestRect.top = 0; };
          if (DestRect.bottom >= eScreenVisibleY) { Backup.bottom += (eScreenVisibleY - DestRect.bottom)/Zoom; DestRect.bottom = (eScreenVisibleY/Zoom); DestRect.bottom*=Zoom; };
          if (DestRect.left < 0) { Backup.left -= DestRect.left/Zoom; DestRect.left = 0; };
          if (DestRect.right >= eScreenVisibleX){ Backup.right += (eScreenVisibleX - DestRect.right)/Zoom; DestRect.right = (eScreenVisibleX/Zoom);DestRect.right*=Zoom; };

          DestRect.bottom = DestRect.top + ((Backup.bottom-Backup.top) * Zoom);
          DestRect.right  = DestRect.left + ((Backup.right-Backup.left) * Zoom);

          if (drawObj->Element->peShowTile)
          { hr = pSurface->pDDSurf->Blt(&DestRect, drawObj->Element->MainTile->TileSurface->pDDSurf,&Backup,DDBLT_WAIT | DDBLT_KEYSRC , NULL);
          } else
          { hr = pSurface->pDDSurf->Blt(&DestRect,Sprite->pFrame->pDDSurf,&Backup,DDBLT_WAIT | DDBLT_KEYSRC , NULL);
          }
          // if FAILED(hr) TraceErrorDD(hr,99);
      }
};

///// TileDecorsDrawEditor /////////////////////////////////////////////////////
// Nouvelle routine d'affichage de sprites exprés pour th éditor ///////////////
void TileDecorsDrawEditor(int scX2,int scY2,double Zoom,TileDecors* tileDec,DDSurface *pSurface)
{   if ((tileDec==NULL) || (pSurface==NULL)) return;

    RECT Backup = tileDec->TileSurface->SrcRect;
    // Rectifie le tir
    Backup.bottom -=2; //Zoom;
    Backup.right -= 1;//Zoom;
    RECT DestRect;
    DestRect.top = scY2;
    DestRect.left = scX2;
    DestRect.bottom = scY2 + ((Backup.bottom-Backup.top) * Zoom);
    DestRect.right  = scX2 + ((Backup.right-Backup.left) * Zoom);
    // Vérifie si c'est la peine d'afficher une image
    if (    (DestRect.left < eScreenVisibleX) && (DestRect.top < eScreenVisibleY)
         && (DestRect.right > 0) && (DestRect.bottom >0 ) )
       {  // Gère le clipping
          if (DestRect.top < 0) { Backup.top -= DestRect.top/Zoom; DestRect.top = 0; };
          if (DestRect.bottom >= eScreenVisibleY) { Backup.bottom += (eScreenVisibleY - DestRect.bottom)/Zoom; DestRect.bottom = (eScreenVisibleY/Zoom); DestRect.bottom*=Zoom; };
          if (DestRect.left < 0) { Backup.left -= DestRect.left/Zoom; DestRect.left = 0; };
          if (DestRect.right >= eScreenVisibleX){ Backup.right += (eScreenVisibleX - DestRect.right)/Zoom; DestRect.right = (eScreenVisibleX/Zoom);DestRect.right*=Zoom; };
          DestRect.bottom = DestRect.top + ((Backup.bottom-Backup.top) * Zoom);
          DestRect.right  = DestRect.left + ((Backup.right-Backup.left) * Zoom);
          pSurface->pDDSurf->Blt(&DestRect, tileDec->TileSurface->pDDSurf,&Backup,DDBLT_WAIT | DDBLT_KEYSRC , NULL);
      }
}
////////////////////////////////////////////////////////////////////////////////

//=== recadre la vue en tenant compte du format de l'animation en cours ======//
void __fastcall TAnimView::RecadreView()
{
    // 0) Choppe la direction
    // peDirection du personnage à modifier
    if (AnimationObjet == OBJET_PERSOS)
    {   switch (AnimDirection)
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
    }

    // 1) Acquière les dimensions de l'anim
    // Remonte d'abord jusqu'au bon sprite de la SpriteListe
    if (AnimationObjet == OBJET_PERSOS)
    {  SpriteTilePerso *Sprite = (SpriteTilePerso*) Charact->STLPersonnage->SprIndex[Perso->peSprNumLib];
       animWidth = Sprite->Width;
       animHeight = Sprite->Height;
    }
    if (AnimationObjet == OBJET_DECORS)
    {  if (!DecObj->Element->peShowTile)
       { SpriteTilePerso *Sprite = (SpriteTilePerso*) DecObj->Element->STLObjets->SprIndex[DecObj->peNumSprite];
         animWidth = Sprite->Width;
         animHeight = Sprite->Height;
       } else
       { animWidth = DecObj->Element->MainTile->Width;
         animHeight = DecObj->Element->MainTile->Height;
       }
    }



    // 2)  Recadre la vue d'aprés ces dimensions
    viewWidth = animWidth * 2 * Zoom;      // vue 2x plus grande
    if (viewWidth < eScreenVisibleX) { viewWidth = (eScreenVisibleX/Zoom); viewWidth*=Zoom; }
    viewHeight = animHeight * 2 * Zoom;
    if (viewHeight < eScreenVisibleY) { viewHeight = (eScreenVisibleY/Zoom); viewHeight*=Zoom; }

    // 3)  On place le sprite au milieu de la vue
    spriteX = (viewWidth - animWidth*Zoom)/2;
    spriteX = spriteX - (spriteX % (int) Zoom);
//    spriteY = 0; // Zoom ki déconne
    spriteY = (((viewHeight - animHeight*Zoom) / 2)/Zoom)*Zoom;

    // 4) Positionnement de l'écran sur la vue
    if (viewWidth < eScreenVisibleX) ScreenX = 0;
    else { ScreenX = ((viewWidth - eScreenVisibleX) / (2*Zoom));
           ScreenX *= Zoom; }

//    ScreenY = 0; // Zoom ki déconne
    if (viewHeight < eScreenVisibleY) ScreenY = 0;
    else ScreenY = (((viewHeight - eScreenVisibleY) / 2)/Zoom) * Zoom;

    // 3) Recadre les barres de défilement
    if (viewWidth > eScreenVisibleX)              // Barre horizontale
    {  ScrollViewHor->Max = ((viewWidth - eScreenVisibleX) /Zoom);
       ScrollViewHor->Max *=Zoom;
       ScrollViewHor->Position = ScreenX;
       ScrollViewHor->Enabled = true;
    }
    else
    {   ScrollViewHor->Max = 0;
        ScrollViewHor->Position = 0;
        ScrollViewHor->Enabled = false;
    }

    if (viewHeight > eScreenVisibleY)              // Barre verticale
    {  ScrollViewVert->Max = ((viewHeight - eScreenVisibleY) / Zoom);
       ScrollViewVert->Max *=Zoom;
       ScrollViewVert->Position = ScreenY;
       ScrollViewVert->Enabled = true;
    }
    else
    {   ScrollViewVert->Max = 0;
        ScrollViewVert->Position = 0;
        ScrollViewVert->Enabled = false;
    }
}
//============================================================================//

//=== Mouvement de la souris détecté =========================================//
void __fastcall TAnimView::FormMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{   // Prélève les coordonnées de la souris
    mouseScreenX = X;
    mouseScreenY = Y;
    BarOutils->LabelSourisX->Caption = X;
    BarOutils->LabelSourisY->Caption = Y;

    // Si l'on est en train de positionner un TileDecors par desuss l'anim
    if (aTileDecorsToScreen)
    {  // Si c'est une anim
       if (!DecObj->Element->peShowTile)
       {  AnimsInfos->LabeldX2->Caption = mouseGravX;
          AnimsInfos->LabeldY2->Caption = mouseGravY;
       }
       else  // Si c'est un TileDecors, on prends la différence en trapèze et non plus en pixels
       { double rx = mouseGravX * ISO_HEIGHT_HALF;
         double ry = mouseGravY * ISO_WIDTH_HALF;
         int dU = floor( ( rx + ry ) / ISO_DENOMINATEUR );
         int dV = floor( ( ry - rx ) / ISO_DENOMINATEUR );
         AnimsInfos->LabeldX2->Caption = dU;
         AnimsInfos->LabeldY2->Caption = dV;
       }
    }

}
//----------------------------------------------------------------------------//

void __fastcall TAnimView::MouseInfos()
{   // Convertit les coordonnées par rapport au centre du sprite en cours
    if (aPlayingAnimationState != OFF)
    {   int cPersoX, cPersoY;
        //=== Dans le cas d'un PERSO ===
        if (AnimationObjet == OBJET_PERSOS)
        {   // Remonte jusqu'au bon sprite de la SpriteListe
            SpriteTile *Sprite;
            HRESULT hr;
            Sprite = Charact->STLPersonnage->SprIndex[Perso->peSprNumLib];

            cPersoX = spriteX - ScreenX + Sprite->GravX*Zoom;
            cPersoY = spriteY - ScreenY + Sprite->GravY*Zoom;
        } else
        //=== Dans le cas d'un DECORS ===
        if (AnimationObjet == OBJET_DECORS)
        {   // Remonte jusqu'au bon sprite de la SpriteListe
            SpriteTile *Sprite;
            HRESULT hr;
            Sprite = DecObj->Element->STLObjets->SprIndex[DecObj->peSprNumLib];

            if (DecObj->Element->peShowTile)
            { cPersoX = spriteX - ScreenX + DecObj->Element->MainTile->TrapPrincX*Zoom;
              cPersoY = spriteY - ScreenY + DecObj->Element->MainTile->TrapPrincY*Zoom;
            }
            else
            { cPersoX = spriteX - ScreenX + Sprite->GravX*Zoom;
              cPersoY = spriteY - ScreenY + Sprite->GravY*Zoom;
            }
        }

        //=== Commun aux 2 ===
        mouseGravX = (mouseScreenX - cPersoX) / Zoom;
        mouseGravY = (mouseScreenY - cPersoY) / Zoom;
        BarOutils->LabelSourisGravX->Caption = mouseGravX;
        BarOutils->LabelSourisGravY->Caption = mouseGravY;
    }
}
//----------------------------------------------------------------------------//

//=== Clic sur la vue de l'éditeur ===========================================//
void __fastcall TAnimView::FormMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{ // Calcule les coordonnées du point par rapport au sprite
  P1x = (X + ScreenX - spriteX)/Zoom;
  P1y = (Y + ScreenY - spriteY)/Zoom;

  // Si l'on est en train de positionner un sprite par dessus l'anim
  if (aSpriteSuperposeeMoving)
  { // Si l'on a cliqué sur le bouton droit --> on fait avancer le sprite d'un pas
    if (Button==mbRight)
    { // On récupère d'abord le nb de sprite par anim
      int nbSprite = Perso->peNbSprite;
      // Puis augmente le n° du sprite superposable actuel
      if (aSpritePersoNumAnim+1<nbSprite)
      { aSpritePersoNumAnim++;
        aSpriteSuperposeeNumero++;
      } else
      {  aSpritePersoNumAnim = 0;  // Si on revient au 1er sprite
         aSpriteSuperposeeNumero -= nbSprite-1;
      }
    } else // Si l'on a cliqué sur le bouton gauche --> on récupère les coordonnées relatives de l'épée
    if (Button==mbLeft)
    {  // on Sauvegarde ces coords suivant le type de positionnement (pour un seul sprite ou pour toute l'anim)
       if (WokStation->RGTypePos->ItemIndex==1)
       {  // Positionner le sprite supperposable à cette position que pour 1 sprite
          // Remonte jusqu'au bon sprite de la SpriteListe
          SpriteTilePerso *Sprite = (SpriteTilePerso*) Charact->STLPersonnage->SprIndex[Perso->peSprNumLib];
          int numIDTypeObjSprite = IDTypeObjSprite->NumID(WokStation->CBTypeFrame->Text);
          if (Sprite->GSprInf->ListAnimSuperp.Existe(numIDTypeObjSprite))
          { Sprite->GSprInf->ListAnimSuperp.SetCoord(numIDTypeObjSprite, mouseGravX, mouseGravY);
          } else
          { Sprite->GSprInf->ListAnimSuperp.Add(mouseGravX, mouseGravY, numIDTypeObjSprite,aSpriteSuperposeeNumero);
          }
          aSpriteSuperposeeMoving = false;
       }
       else if (WokStation->RGTypePos->ItemIndex==0)
       {  // Positionner le sprite supperposable à cette position pour tous les sprites de l'anim
          // Se place au 1er sprite de l'anim
          int numSpr = Perso->peSprNumLib - Perso->peNumSprite;
          int numSprSup = aSpriteSuperposeeNumero - Perso->peNumSprite;
          int numIDTypeObjSprite = IDTypeObjSprite->NumID(WokStation->CBTypeFrame->Text);
          // Pour tous les sprites de même orientation de l'anim en cours
          for (int i=0; i < Perso->peNbSprite ; i++)
          { SpriteTilePerso *Sprite = (SpriteTilePerso*) Charact->STLPersonnage->SprIndex[numSpr];
            if (Sprite->GSprInf->ListAnimSuperp.Existe(numIDTypeObjSprite))
            {  // Si on a deja positionne un sprite superpose, on change simplement ses coordonnées
               Sprite->GSprInf->ListAnimSuperp.SetCoord(numIDTypeObjSprite, mouseGravX, mouseGravY);
            } else
            {  Sprite->GSprInf->ListAnimSuperp.Add(mouseGravX, mouseGravY, numIDTypeObjSprite,numSprSup);
            }
            numSpr++;
            numSprSup++;
          }
          aSpriteSuperposeeMoving = false;
       }
    }
  }

  // Si l'on est en train de positionner un TileDecors par desuss l'anim
  if (aTileDecorsToScreen)
  { // Btn Droit -> on rends visible/invisible le TileDecors pour voir ce qu'il y a derrière
    if (Button==mbRight)
    { aTileDecorsIsVisible = !aTileDecorsIsVisible;
    }
    // Si Btn Gauche -> on prends la différence de coordonnées
    if (Button==mbLeft)
    {  // en (dX,dY) si c'est une anim
       if (!DecObj->Element->peShowTile)
       {  DecObj->Element->AnimInfos[DecObj->getNumAnim()].dX = mouseGravX;
          DecObj->Element->AnimInfos[DecObj->getNumAnim()].dY = mouseGravY;
       } else // en (dU, dV) si c'est un TileDecors
       { double rx = mouseGravX * ISO_HEIGHT_HALF;
         double ry = mouseGravY * ISO_WIDTH_HALF;
         int U = floor( ( rx + ry ) / ISO_DENOMINATEUR );
         int V = floor( ( ry - rx ) / ISO_DENOMINATEUR );
         DecObj->Element->AnimInfos[DecObj->getNumAnim()].dX = U;
         DecObj->Element->AnimInfos[DecObj->getNumAnim()].dY = V;
       }
       AnimsInfos->RegenScreen();   // Rafraîchit la fiche
    }
  }

  // Si l'on est en train de positionner un sprite "classique" par dessus l'anim
  if (aAnimClassikMoving)
  { // Si l'on a cliqué sur le bouton droit --> on fait avancer le sprite d'un pas
    if (Button==mbLeft)
    {	// Si l'on a cliqué sur le bouton gauche --> on récupère les coordonnées relatives du sprite
    	SClassikAnimInfos* animInfos = DecObj->Element->GetClassikAnim(aNumAnimClassik);
        animInfos->dX = mouseGravX;
        animInfos->dY = mouseGravY;         
    	WokStation->LabelAnimX->Caption = animInfos->dX;
    	WokStation->LabelAnimY->Caption = animInfos->dY;
        aAnimClassikMoving = false;
        aSpriteClassikToScreen = true;
    }
  }
}
//----------------------------------------------------------------------------//

//=== Relâche la souris =======================================================//
void __fastcall TAnimView::FormMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{ switch (workMode)
    {  case RECTANGLE :
          { // Stock les coordonnées du second point
            P2x = (X + ScreenX - spriteX)/Zoom;
            P2y = (Y + ScreenY - spriteY)/Zoom;
            // Ordonne les coordonnées
            if (P1x > P2x) { int t=P1x; P1x=P2x; P2x=t; }
            if (P1y > P2y) { int t=P1y; P1y=P2y; P2y=t; }

            // Remonte jusqu'au bon sprite de la SpriteListe
            SpriteTilePerso *Sprite;
            HRESULT hr;
            Sprite = (SpriteTilePerso*) Charact->STLPersonnage->SprIndex[Perso->peSprNumLib];

            // Et lui transmet les coordonnées suivant la zone sur laquelle il bosse actuellement
            switch (workZone)
            {  case ZONESOL : Sprite->GSprInf->ZoneSol.remplitRect(P1x,P1y,P2x,P2y); break;
            }
            workZone = AUCUN;
            workMode = NONE;  // fin
          }; break;
       case RECTANGLE_ORIENTE :
          { // Stock les coordonnées du second point
            P2x = (X + ScreenX - spriteX)/Zoom;
            P2y = (Y + ScreenY - spriteY)/Zoom;
            int Px3, Py3, Px4, Py4;

            // Détermine le type de rectangle à tracer
            enum eGenreRect { RECT_DROIT, RECT_DIAG_NE, RECT_DIAG_SE, RECT_NONE };
            eGenreRect genreRect = RECT_NONE;
            switch (Perso->peDirection)
            { case 0 :  // Ouest - Sud - Est - Nord
              case 2 :
              case 4 :
              case 6 : genreRect = RECT_DROIT; break;
              case 3 : // Sud-Est - Nord-Ouest
              case 7 : genreRect = RECT_DIAG_SE; break;
              case 1 : // Sud-Ouest - Nord-Est
              case 5 : genreRect = RECT_DIAG_NE; break;
            }

            // ordonne les coordonnées avant pour que le 1er point soit en haut à gauche
            switch (genreRect)
            { case (RECT_DROIT) :
              case (RECT_DIAG_NE) :
                   // Pt(1) en haut à gauche et Pt(2) en bas à droite
                   if (P1y > P2y) { int t=P1y; P1y=P2y; P2y=t; t=P1x; P1x=P2x; P2x=t;}
                   break;
              case (RECT_DIAG_SE) :
                   // Pt(1) en haut à droite et Pt(2) en bas à gauche
                   if (P1y > P2y) { int t=P1y; P1y=P2y; P2y=t; t=P1x; P1x=P2x; P2x=t; }
                   break;
              case (RECT_NONE) :  MessageBoxA(NULL,"Problem ds TAnimView::FormMouseUp","Orientation inconnue",0); break;
            }

            // Cas du rectangle en diagonale
            if ((genreRect==RECT_DIAG_NE) || (genreRect==RECT_DIAG_SE))
            {  // Décalage pouvant prendre une moyenne de 0.5
               float decalage = abs( (abs(P2y-P1y) - abs(P2x-P1x)) / 2);

               // Calcule des coords des 2 autres points
               if (P1x < P2x)//(genreRect==RECT_DIAG_NE)
               { Px3 = P2x + int(decalage);
                 Py3 = P2y - int(decalage);
                 Px4 = P1x - int(decalage);
                 Py4 = P1y + int(decalage);
               } else // (genreRect==RECT_DIAG_SE)
               { Px3 = P1x + int(decalage);
                 Py3 = P1y + int(decalage);
                 Px4 = P2x - int(decalage);
                 Py4 = P2y - int(decalage);
               }
            }

            // Remonte jusqu'au bon sprite de la SpriteListe
            SpriteTilePerso *Sprite;
            HRESULT hr;
            Sprite = (SpriteTilePerso*) Charact->STLPersonnage->SprIndex[Perso->peSprNumLib];

            // Et lui transmet les coordonnées suivant la zone sur laquelle il bosse actuellement
            switch (workZone)
            {  case ZONEPIED :
               // Attention : faire gaffe au sens des points
               Sprite->GSprInf->ZonePied.remplitRect(P1x,P1y,Px3,Py3,P2x,P2y,Px4,Py4); break;
            }
            workZone = AUCUN;
            workMode = NONE;  // fin
          }; break;
       case POINT :
          { // Remonte jusqu'au bon sprite de la SpriteListe
            SpriteTilePerso *Sprite;
            HRESULT hr;
            Sprite = (SpriteTilePerso*) Charact->STLPersonnage->SprIndex[Perso->peSprNumLib];
            //Sprite->GSprInf->ZoneSol;
            // Et lui transmet les coordonnées suivant la zone sur laquelle il bosse actuellement
            switch (workZone)
            {  case PT_GRAV_SOL : Sprite->GSprInf->GravSol.remplitPoint(P1x,P1y); break;
            }
          //  workZone = AUCUN;
          //  workMode = NONE;  // fin
            // Focus sur la barre de défilement de
            BarOutils->ScrollBarStep->SetFocus();

          }; break;
    };
}
//----------------------------------------------------------------------------//

///// ClassikSpriteDrawEditor //////////////////////////////////////////////////
// Nouvelle routine d'affichage de sprites exprés pour th éditor ///////////////
void ClassikSpriteDrawEditor(int scX2,int scY2,double Zoom,int NumSprite, DDSurface *pSurface,SpriteTileGameLib *stgl)
{
    if ((NumSprite<0) || (stgl==NULL)) return;  // vérifications

    // Remonte jusqu'au bon sprite de la SpriteListe
    SpriteTile*	Sprite = stgl->GetSpriteTile(NumSprite);

    // ATTENTION BUG ? ScY doit être égal à ZERO
    RECT Backup = Sprite->SrcRect;
    Backup.bottom -=2; //Zoom;
    Backup.right -= 1;//Zoom;
    RECT DestRect;
    DestRect.top = scY2;
    DestRect.left = scX2;
    DestRect.bottom = scY2 + ((Backup.bottom-Backup.top) * Zoom);
    DestRect.right  = scX2 + ((Backup.right-Backup.left) * Zoom);
    // Rectifie le tir
    // Vérifie si c'est la peine d'afficher une image
    if (    (DestRect.left < eScreenVisibleX) && (DestRect.top < eScreenVisibleY)
         && (DestRect.right > 0) && (DestRect.bottom >0 ) )
       {
          // Gère le clipping
          if (DestRect.top < 0) { Backup.top -= DestRect.top/Zoom; DestRect.top = 0; };
          if (DestRect.bottom >= eScreenVisibleY) { Backup.bottom += (eScreenVisibleY - DestRect.bottom)/Zoom; DestRect.bottom = (eScreenVisibleY/Zoom); DestRect.bottom*=Zoom; };
          if (DestRect.left < 0) { Backup.left -= DestRect.left/Zoom; DestRect.left = 0; };
          if (DestRect.right >= eScreenVisibleX){ Backup.right += (eScreenVisibleX - DestRect.right)/Zoom; DestRect.right = (eScreenVisibleX/Zoom);DestRect.right*=Zoom; };

          DestRect.bottom = DestRect.top + ((Backup.bottom-Backup.top) * Zoom);
          DestRect.right  = DestRect.left + ((Backup.right-Backup.left) * Zoom);

          HRESULT hr = pSurface->pDDSurf->Blt(&DestRect,Sprite->pFrame->pDDSurf,&Backup,DDBLT_WAIT | DDBLT_KEYSRC , NULL);
          if FAILED(hr) ; 
      }
};
////////////////////////////////////////////////////////////////////////////////






