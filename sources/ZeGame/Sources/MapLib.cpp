////////////////////////////////////////////////////////////////////////////////
// MapLib.cpp  :  D�finitions de tous les �l�ments d'une carte				  //
// 	=> version bis r�nov�e pour le jeu (provient de l'�diteur)				  //
////////////////////////////////////////////////////////////////////////////////

//- Donn�es � inclure ------------------------------------------------------
#ifdef CBUILDER
#include <vcl\vcl.h>
#include <stdio.h>
#include "DirectLib.h"
#endif
#include <Math.h>
#include "ZeNain.h"
#include "animall.h"
#include "ImDecors2.h"
#include "PNJLib.h"
#include "PersoLib.h"
#include "D3DLib.h"
#include "DDLibAsm.h"
#include "GameDebug.h"
#include "GameObjectList.h"		// Liste de tous les GameObject du jeu
#include "MapZone.h"			// Zone sur la carte
#pragma hdrstop
#include "MapLib.h"				// Son header

#define MAPLIB_DEBUG

// Pour plus qu'il me fasse de warning de compilation
const void *cunivname = &cUniversName;
const void *ctypename = &cTypeTileName;

//---------------------------------------------------------------------------

//== Variables ===============================================================//
TViewMap *ViewMap	  = NULL;		// Gestion de la fiche de repr�sentation de la carte
TMapNiveau *MapNiveau = NULL;
TSolScroll *SolScroll = NULL;		// Objet permettant le scrolling
TYBuffer YBuffer;                   // Objet permettant d'afficher les sprites dans le bon ordre
TRefresh *FlipRefresh = NULL;		// Objet servant � rafra�chir l'�cran

int Coul_POSITION 		= 0xF800;	// Rouge
int Coul_TRAPPRINCIPAL  = 0x001F;	// Bleu
int Coul_TRAPSECONDAIRE = 0xFFE0;	// Jaune
int Coul_TRAPOBJET 		= 0x0FC0;	// Vert
int Coul_SELECT         = 0xFC0D;	// Orange
int Coul_OBJDESSUS      = 0xF800;

//== Fonctions annexes =======================================================//

// Affiche le trap�ze (U,V) � l'�cran
void DrawViewTrap(int U, int V, int Col, int Type)
{
    int dU = U - ViewMap->ScreenU;
    int dV = V - ViewMap->ScreenV;
	int X, Y;
    ViewMap->TransUVtoXY(dU, dV, &X, &Y);
    X-=ViewMap->dx; Y-=ViewMap->dy;
    DS->BackISOTrapeze(X, Y, Col, Type);
};


////////////////////////////////////////////////////////////////////////////////
// Objet TMAP																  //
////////////////////////////////////////////////////////////////////////////////

//== Constructeur ============================================================//
TMap::TMap()
{
//	MapNum = 0;
//    memset(Name,0,sizeof(DTLibName));
//    memset(DTLibName,0,sizeof(DTLibName));
    MapWidth = MapHeight = 0;
    Carte = NULL;
};
//----------------------------------------------------------------------------//

//== Destructeur =============================================================//
TMap::~TMap()
{
	if (Carte!=NULL) DessaloueCarte();		// lib�re la m�moire des cases
};
//----------------------------------------------------------------------------//

//== Initialise le Tableau dynamique des cases ===============================//
void TMap::AlloueCarte(int Width, int Height)
{
    MapWidth  = Width;
    MapHeight = Height;

    // Alloue la m�moire n�cessaire
    Carte = new TCase* [MapWidth];  									 // lignes
    for (int i=0 ; i < MapWidth; i++) Carte[i] = new TCase [MapHeight];  // colonnes

    // Initialise les donn�es des cases
    for (int x=0; x < MapWidth; x++)
    	for (int y=0; y < MapHeight; y++)
           {
              Carte[x][y].resetInfos();
              Carte[x][y].setPrincipal(FALSE);    // Carte[x][y].Principal = FALSE
              Carte[x][y].setAccessible(TRUE);   // Carte[x][y].Accessible = TRUE
              Carte[x][y].ptrObjet = NULL;
              Carte[x][y].TrapCollide = PARTIELLE;
           }
};
//----------------------------------------------------------------------------//

//== Lib�re la m�moire du tableau dynamique ==================================//
void TMap::DessaloueCarte()
{
    for (int i=0; i < MapWidth; i++) 
	{	// delete[] Carte[i];	    // lignes [BUG] ??????
	}
    delete[] Carte;				// colonnes
	Carte = NULL;
};
//----------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////////////////
// Classe TCAMERA															  //
////////////////////////////////////////////////////////////////////////////////

//== Constructeur ============================================================//
TCamera::TCamera()
{	// D�placement de la cam�ra avec le perso 
	iCameraDeplaceMode = CAMERA_MODE_DEPLACEMENT_HERO;
	// Position
	fCameraX = fCameraY = 0;
	fCamDestX = fCamDestY = 0;
	bMoving = false;
	// D�placement curviligne par d�faut
	iCameraMoveMode = CAMERA_MODE_MOVE_LINEAR;
}
//----------------------------------------------------------------------------//

//=== D�placement brutal de la cam�ra en (posX, posY) ========================//
void TCamera::SetNewPosition(float posX, float posY)
{	bMoving = false;
	fCameraX = posX;
	fCameraY = posY;
}
//----------------------------------------------------------------------------//

//=== D�placement de la cam�ra en douceur vers (newX, newY) ==================//
void TCamera::MoveCamera(float newX, float newY, int time, int mode)
{	// V�rifie si la cam�ra n'est pas d�j� arriv�e � destination
	if ((newX == fCameraX) && (newY == fCameraY)) return;
	
	// Node de d�placement � utiliser pour d�placer la cam�ra
	mode = iCameraMoveMode;
	bMoving = true;
	fCamDestX = newX;
	fCamDestY = newY;
	
	// Calcul du chemin � parcourir
	switch (mode)
	{	case CAMERA_MODE_MOVE_LINEAR:
			break;
		case CAMERA_MODE_MOVE_CURVE:
			break;
	}
}
//----------------------------------------------------------------------------//


////////////////////////////////////////////////////////////////////////////////
// Classe TVIEWSCREEN														  //
////////////////////////////////////////////////////////////////////////////////

//== Constructeur ============================================================//
TViewMap::TViewMap():
	TCamera()
{  // met tout � z�ro
   MapPixWidth = MapPixHeight = 0;
   NbTrapPerScreenWidth = NbTrapPerScreenHeight = 0;
   ScreenX = ScreenY = 0;
   dx = dy = 0;
   ScreenViewX = SCREEN_WIDTH;   // pour le moment
   ScreenViewY = SCREEN_HEIGHT;

   // Alloue les surfaces DDSBackFonds
       // Cr�ation des 2 tampons ds la surface complex
    pDDSBackFond1 = new DDSurface(DS,SCREEN_WIDTH, SCREEN_HEIGHT, VIDEO_MEMORY);
    pDDSBackFond2 = new DDSurface(DS,SCREEN_WIDTH, SCREEN_HEIGHT, VIDEO_MEMORY);
    pDDSBackFond1->ColorKey(0,0);
    pDDSBackFond2->ColorKey(0,0);

    // Alloue l'objet TScrollFond
    SolScroll = new TSolScroll(2,SCREEN_WIDTH,SCREEN_HEIGHT);

    HRESULT hr;
    // S'occupe du clipping Hardware de la surface complexe si voulu
    if (DS->HardwareClipping)
     {
        // Cr�e le cadre de clipping
    	hr = DS->pDD->CreateClipper(0,&DS->pDDClipper,NULL);
        if (FAILED(hr)) DS->pDDClipper->Release(); // on abandonne le clipping !

    	// Assigne le clipper � la fen�tre
    	hr = DS->pDDClipper->SetHWnd(0, DS->DDHandle);
    	if (FAILED(hr))  // on abandonne le clipping !
        	DS->pDDClipper->Release();

    	// zone de clipping de la surface pDDSBackFond1
    	hr = pDDSBackFond1->pDDSurf->SetClipper(DS->pDDClipper);
    	if (FAILED(hr))  DS->pDDClipper->Release();

 		// zone de clipping de la surface pDDSBackFond2
    	hr = pDDSBackFond2->pDDSurf->SetClipper(DS->pDDClipper);
    	if (FAILED(hr)) DS->pDDClipper->Release();

    	// plus besoin du clipper
		DS->pDDClipper->Release();
     };  // fin du clipping harware

    NewCamera = TRUE;	// toute nouvelle position
    BlitMode = BLIT_MODE;
};
//----------------------------------------------------------------------------//

//== Destructeur =============================================================//
TViewMap::~TViewMap()
{  
	// D�salloue l'objet TScrollFond
    delete SolScroll; SolScroll = NULL;
	
	// D�salloue les surfaces DDSBackFonds
    delete pDDSBackFond1; pDDSBackFond1 = NULL;
	delete pDDSBackFond2; pDDSBackFond2 = NULL;    
}
//----------------------------------------------------------------------------//

//=== Changement de mode-vid�o : restauration et rechargement des surfaces ===//
void TViewMap::ChangeSolScrollVideoMode(int newScrW, int newScrH)
{	
	// Partie de la map � afficher � l'�cran
	ScreenViewX = newScrW;   
	ScreenViewY = newScrH;

	// D�salloue puis r�alloue les surfaces DDSBackFonds
	delete pDDSBackFond1; pDDSBackFond1 = NULL;
	delete pDDSBackFond2; pDDSBackFond2 = NULL;
    pDDSBackFond1 = new DDSurface(DS,ScreenViewX, ScreenViewY, VIDEO_MEMORY);
    pDDSBackFond2 = new DDSurface(DS,ScreenViewX, ScreenViewY, VIDEO_MEMORY);
    pDDSBackFond1->ColorKey(0,0);
    pDDSBackFond2->ColorKey(0,0); 

    // Nouvelle position de la cam�ra
	NewCamera = TRUE;
	BlitMode = BLIT_MODE;
}

//== Calcule certaines donn�es de la classe � partir des infos de la map =====//
void TViewMap::CalcInfos(int MapTrapWidth, int MapTrapHeight)
{
   // Nombre de pixels du rectangle contenant la carte
	MapPixWidth  = ISO_WIDTH_HALF  * ( MapTrapWidth + MapTrapHeight)   + 1;
    MapPixHeight = ISO_HEIGHT_HALF * ( MapTrapWidth + MapTrapHeight)   + 1;
   // Nombre de trap�ze minimal par vue d'�cran
   NbTrapPerScreenWidth  = int ( (float) ScreenViewX / (float) ISO_WIDTH  );
   NbTrapPerScreenHeight = int ( (float) ScreenViewY / (float) ISO_HEIGHT );
};
//----------------------------------------------------------------------------//

//== Transforme les coordonn�es (X,Y) en (U,V) et calcule (dx, dy) ===========//
void TViewMap::PosCoinUV()
{
    // Calcul de (ScreenU, ScreenV)
    TransXYtoUV(ScreenX, ScreenY, &ScreenU, &ScreenV);
    // Calcul de (dx,dy)
    int tX, tY;  // Coordonn�es de l'angle sup�rieur du trap�ze du coin sup�rieur-gauche de l'�cran
    TransUVtoXY(ScreenU, ScreenV, &tX, &tY);
    dx = ScreenX - tX;
    dy = ScreenY - tY;
}
//----------------------------------------------------------------------------//

//=== Changement de rep�re : ISO -> Cart�sien ================================//
void TViewMap::TransUVtoXY(int U, int V, int *X, int *Y)
{   // Coordonn�es de l'angle sup�rieur du trap�ze
    *X = ( U - V ) * ISO_WIDTH_HALF;
    *Y = ( U + V ) * ISO_HEIGHT_HALF;
}
//----------------------------------------------------------------------------//

//=== Renvoit le Y correspondant au couple Iso (U,V) =========================//
int  TViewMap::TransUVtoY(int U, int V)
{  return (U + V) * ISO_HEIGHT_HALF;
}
//----------------------------------------------------------------------------//

//== Changement de rep�re : Cart�sien -> ISO ================================//
void TViewMap::TransXYtoUV(int X, int Y, int *U, int*V)
{
    double rx = X* ISO_HEIGHT_HALF;
    double ry = Y * ISO_WIDTH_HALF;
    *U = int( ( rx + ry ) / ISO_DENOMINATEUR );
    *V = int( ( ry - rx ) / ISO_DENOMINATEUR );
};
//----------------------------------------------------------------------------//

//== compl�te les bandes du layer manquant ===================================//
void TViewMap::CompleteMap(DDSurface *pScreen,int dx, int dy, int ScrX, int ScrY)
{
   // � calculer d'apr�s dx & dy
   int IntervalMargeU = (dx / ISO_WIDTH)  +1,
   IntervalMargeV = (dy / ISO_HEIGHT) +1;

   // Nb de trap�ze par longeur & largeur de la surface
   int NbTrapPerScreenWidth2  = int ( (float) pScreen->PixWidth  / (float) ISO_WIDTH  );
   int NbTrapPerScreenHeight2 = int ( (float) pScreen->PixHeight / (float) ISO_HEIGHT );


   // Affichage des 2 layers
   if (MapNiveau==NULL) return;
   TEtage *Etage = MapNiveau->PrincEtage;

   TMap *pMap = Etage->LayerSols;
   // Autres calculs de DebV & DebV
   int incU, decV;
   int DebU, DebV;
   BOOL IncremV = TRUE;
   // Tra�age horizontal
   if (dy!=0)
   {  if (dy>0) TransXYtoUV(ScreenX, OldScreenY + ScreenViewY, &DebU, &DebV);
      else TransXYtoUV(ScreenX, OldScreenY, &DebU, &DebV);
      for ( int cV = 0 ; cV < (2*IntervalMargeV); cV++)
        {
           incU = DebU;  decV = DebV;
           for ( int cU = 0 ; cU < (NbTrapPerScreenWidth2+2*IntervalMargeU)*2  ; cU++)
             {
              if ((incU>=0) && (incU < Etage->EtageWidth) && (decV>=0) && (decV < Etage->EtageHeight))
                 {

                   DecorsObjet* DecPtr = pMap->Carte[incU][decV].ptrObjet;
                   // Si il n'y a rien sur la carte : dessine les trap�ze lors du 1er passage ds un layer
                   if (DecPtr!=NULL)
                    {  if (DecPtr->ObjDessine == 0)  // si l'objet n'a pas �t� encore dessin�
                          {
                            AfficheObjDecors(DecPtr , pScreen, ScrX, ScrY);
                            DecPtr->ObjDessine = 1;
                          }
                    }
                 };
               incU++;
               decV--;
             };
           if (IncremV) DebV++; else DebU++;
           IncremV = !IncremV;
        };
    };
   // Tra�age horizontal
   if (dx!=0)
	{ if (dx>0) TransXYtoUV(OldScreenX + ScreenViewX, OldScreenY, &DebU, &DebV);
      else TransXYtoUV(OldScreenX, ScreenY, &DebU, &DebV);
      for ( int cV = 0 ; cV < (2*IntervalMargeV+NbTrapPerScreenHeight2)*2; cV++)
        {
           incU = DebU;  decV = DebV;
           for ( int cU = 0 ; cU < 2*IntervalMargeU ; cU++)
             {
              if ((incU>=0) && (incU < Etage->EtageWidth) && (decV>=0) && (decV < Etage->EtageHeight))
                 {
                    DecorsObjet* DecPtr = pMap->Carte[incU][decV].ptrObjet;
                   // Si il n'y a rien sur la carte : dessine les trap�ze lors du 1er passage ds un layer
                   if (DecPtr!=NULL)
                    {  if (DecPtr->ObjDessine == 0)  // si l'objet n'a pas �t� encore dessin�
                          {
                            AfficheObjDecors(DecPtr , pScreen, ScrX, ScrY);
                            DecPtr->ObjDessine = 1;
                          }
                    }

                 };
               incU++;
               decV--;
             };
           if (IncremV) DebV++; else DebU++;
           IncremV = !IncremV;
        };
    };
};
//----------------------------------------------------------------------------//

//== Affichage non optimis� de la grille � l'�cran ===========================//
void TViewMap::AfficheMap(DDSurface *pScreen, eAffichMode Layer, int ScrX, int ScrY)
{
  	int IntervalMargeU = +30,   // 5
    	IntervalMargeV = +30;   // 15

//    int NbTrapPerScreenWidth2  = ceil ( (float) pScreen->PixWidth  / (float) ISO_WIDTH  );
//    int NbTrapPerScreenHeight2 = ceil ( (float) pScreen->PixHeight / (float) ISO_HEIGHT );


  // Affichage des 2 layers
 	if (MapNiveau==NULL) return;
    TEtage *Etage = MapNiveau->PrincEtage;

    TMap *pMap = NULL;
    // D'apr�s le "Mode d'affichage" : LAYER_SOL, LAYER_DECORS & LAYER_ALL,
    // on d�termine quels layers dessiner
    int DrawEtape = 0, DrawEtapeEnd = 0;
    switch (Layer)	// D�but et fin de parcours des Layers
     {
     	case (LAYER_SOL)    : { DrawEtape = 0; DrawEtapeEnd = 1; }; break;
     	case (LAYER_DECORS) : { DrawEtape = 1; DrawEtapeEnd = 2; }; break;
     	case (LAYER_ALL)    : { DrawEtape = 0; DrawEtapeEnd = 2; }; break;

     };

    for ( ; DrawEtape <  DrawEtapeEnd; DrawEtape++)
    {
     switch(DrawEtape)
      {
        case 0 : pMap = Etage->LayerSols; break;  //  Dessine le Sol
        case 1 : pMap = Etage->LayerDecors; break;  //  Dessine les Decors
      };

      // Pour regrouper les dessins
     switch (DrawEtape)
      {
        case 0:
        case 1:
         { // Vide le Y-Buffer
           YBuffer.Clear();
           // Que se soit le Sol ou le Decors
           if (pMap==NULL) return;
           PosCoinUV();			// Coords du coin de l'�cran d'apr�s la carte
           // Autres calculs de DebV & DebV
           int DebU = ScreenU; int DebV = ScreenV;
           for (int n=0; n < IntervalMargeU ; n++)
                { DebU--; DebV++;  };
           for (int n2=0; n2 < IntervalMargeV ; n2++)
                { DebU--; DebV--;  };
           int incU, decV;
           BOOL IncremV = TRUE;
           for ( int cV = 0 ; cV < (NbTrapPerScreenHeight+ 2*IntervalMargeV)*2; cV++)
             {
                incU = DebU;  decV = DebV;
                for ( int cU = 0 ; cU < (NbTrapPerScreenWidth+2*IntervalMargeU)*2  ; cU++)
                  {
                   if ((incU>=0) && (incU < Etage->EtageWidth) && (decV>=0) && (decV < Etage->EtageHeight))
                      {
                       // Si il n'y a rien sur la carte : dessine les trap�zes lors du 1er passage ds un layer

                        if ((AFFICH_MODE == LAYER_ALL) || (AFFICH_MODE == LAYER_DECORS))
                        if (pMap->Carte[incU][decV].ptrObjet!=NULL)
                          if (pMap->Carte[incU][decV].isPrincipal())  // si c'est un triangle principal
                          {    // Rajoute le d�cors au Y-Buffer
                               YBuffer.AddDecors(TransUVtoY(incU,decV),pMap->Carte[incU][decV].ptrObjet);
                          }
                       };
                    incU++;
                    decV--;
                  };

                if (IncremV) DebV++; else DebU++;
                IncremV = !IncremV;
             };

             // Rajoute le h�ro au Y-Buffer
             if (DrawEtape==1)  // moment de dessiner les persos ?
             {  /*Personnage *pPerso;
                // Ajoute le h�ro
                if (bPerso) pPerso = Hero;
                else        pPerso = Ogre;
                YBuffer.AddPersos(pPerso->peY,pPerso);*/
                // Pour tous les persos : H�ro + Pnj
				for (uint i=0; i < LGP.getCharacterCount() ; i++)
                {	Personnage *pPerso = LGP.getPersoAtIndex(i);
					if ((pPerso != NULL) && (pPerso->isVisible()))
					{	YBuffer.AddPersos(pPerso->peY, pPerso); 
					}
                }
             }
         };
      };
   };

   //  Affiche tous les sprites du dernier layer
   YBuffer.Affiche(pScreen);
};
//----------------------------------------------------------------------------//


//== Affichage optimis�e de la grille � l'�cran ==============================//
// => utilis� pour le sol
void TViewMap::AfficheFastMap(DDSurface *pScreen, eAffichMode Layer, int ScrX, int ScrY)
{
  	int IntervalMargeU = +1,   // 5
    	IntervalMargeV = +1;   // 15

  // Affichage des 2 layers
 	if (MapNiveau==NULL) return;
    TEtage *Etage = MapNiveau->PrincEtage;

 // Nb de trap�ze par longeur & largeur de la surface
    int NbTrapPerScreenWidth2  = int ( (float) pScreen->PixWidth  / (float) ISO_WIDTH  );
    int NbTrapPerScreenHeight2 = int ( (float) pScreen->PixHeight / (float) ISO_HEIGHT );


    TMap *pMap = NULL;
    // D'apr�s le "Mode d'affichage" : LAYER_SOL, LAYER_DECORS & LAYER_ALL,
    // on d�termine quels layers dessiner
    int DrawEtape = 0, DrawEtapeEnd = 0;
    switch (Layer)	// D�but et fin de parcours des Layers
     {
     	case (LAYER_SOL) 	: { DrawEtape = 0; DrawEtapeEnd = 1; }; break;
     	case (LAYER_DECORS) : { DrawEtape = 1; DrawEtapeEnd = 2; }; break;
     	case (LAYER_ALL)    : { DrawEtape = 0; DrawEtapeEnd = 2; }; break;

     };

    for ( ; DrawEtape <  DrawEtapeEnd; DrawEtape++)
    {
     switch(DrawEtape)
      {
        case 0 : pMap = Etage->LayerSols; break;  //  Dessine le Sol
        case 1 : pMap = Etage->LayerDecors; break;  //  Dessine les Decors
      };

      // Pour regrouper les dessins
     switch (DrawEtape)
      {
        case 0:
        case 1:
         {  // Que se soit le Sol ou le Decors
           if (pMap==NULL) return;
           PosCoinUV();			// Coords du coin de l'�cran d'apr�s la carte
           // Autres calculs de DebV & DebV
           int DebU, DebV;
               TransXYtoUV(ScrX, ScrY, &DebU, &DebV);

           for (int n=0; n < IntervalMargeU ; n++)
                { DebU--; DebV++;  };
           for (int n2=0; n2 < IntervalMargeV ; n2++)
                { DebU--; DebV--;  };
           int incU, decV;
           BOOL IncremV = TRUE;
           for ( int cV = 0 ; cV < (NbTrapPerScreenHeight2 + 2*IntervalMargeV)*2; cV++)
             {
                incU = DebU;  decV = DebV;
                for ( int cU = 0 ; cU < (NbTrapPerScreenWidth2+ 2*IntervalMargeU)*2  ; cU++)
                  {
                   if ((incU>=0) && (incU < Etage->EtageWidth) && (decV>=0) && (decV < Etage->EtageHeight))
                      {
                         DecorsObjet* DecPtr = pMap->Carte[incU][decV].ptrObjet;
                         // Si il n'y a rien sur la carte : dessine les trap�ze lors du 1er passage ds un layer
                         if (DecPtr!=NULL)
                          {  if (DecPtr->ObjDessine == 0)  // si l'objet n'a pas �t� encore dessin�
                                {
                                  AfficheObjDecors(DecPtr , pScreen, ScrX, ScrY);
                                  DecPtr->ObjDessine = 1;
                                }
                          }
                      };
                    incU++;
                    decV--;
                  };
                if (IncremV) DebV++; else DebU++;
                IncremV = !IncremV;
             };
         };
      };
   };

};
//----------------------------------------------------------------------------//


//== Affiche l'�l�ment du d�cors sp�cifi� sur la grille en (U,V) =============//
void TViewMap::AfficheObjDecors(DecorsObjet *DecObj, DDSurface *pScreen,int ScrX, int ScrY, int keyTrap)
{   // R�cup�re les dimensions de la fen�tre de visibilit� sur l'�cran
    int SurfViewX = pScreen->PixWidth;
    int SurfViewY = pScreen->PixHeight;

    #ifdef DEBUG
	if ((DecObj->DecorsAnime)  && (DecObj->peEtat==ARRET1) && (!DecObj->peShowTile))
    {  
		#ifdef CBUILDER
		AnsiString texte = "DecorsAnime : ";
		if (DecObj->DecorsAnime) texte+="OUI"; else texte+="NON";
		texte+="  // peEtat = "; texte+=DecObj->peEtat;
		texte+="  // peShowTile : ";
		if (DecObj->peShowTile) texte+="OUI"; else texte+="NON";
		texte+="  // peNumAnim = ";texte+=DecObj->peNumAnim;
		MessageBoxA(NULL,texte.c_str(),"Warning !",0);
		#elif VISUALC
		_asm int 3;
		#endif
    }
	#endif	// DEBUG


    // Position du Trap�ze principal de l'objet s�lectionn� sur la case o� se tient la souris
    int X, Y;
    TransUVtoXY(DecObj->PosU, DecObj->PosV, &X, &Y);
    X -= ScrX;
    Y -= ScrY;

    // 1) Dans le cas d'un objet FIXE ou d'un objet ANIME repr�sent� par un TileDecors
    //    => On affiche un TileDecors
    if ((!DecObj->DecorsAnime) || (DecObj->peShowTile))
    { TileDecors* tile;
      if (!DecObj->DecorsAnime)  // Decors fixe
      {  tile = DecObj->DecTileObj;
      } else                     // Decors Anim�
      {  tile = DecObj->SecondTile;
      }
	
      // Tiens compte du centre de gravit� <=> sommet nord du trap�ze principal
      X -= tile->TrapPrincX;
      Y -= tile->TrapPrincY;

      // Puis place le d�cors exactement l� o� il le faut
      DDSurface *SurfDD = tile->TileSurface;

      if (keyTrap!=-1) // Si on ne doit afficher qu'une lamelle verticale de l'objet
      { SurfDD->DestRect.top    = Y + tile->TrapezTab[keyTrap].lamY1;
        SurfDD->DestRect.left   = X + tile->TrapezTab[keyTrap].lamX1;
        SurfDD->DestRect.bottom = Y + tile->TrapezTab[keyTrap].lamY2;
        SurfDD->DestRect.right  = X + tile->TrapezTab[keyTrap].lamX2;
        SurfDD->SrcRect.top     = tile->TrapezTab[keyTrap].lamY1;
        SurfDD->SrcRect.left    = tile->TrapezTab[keyTrap].lamX1;
        SurfDD->SrcRect.bottom  = tile->TrapezTab[keyTrap].lamY2;
        SurfDD->SrcRect.right   = tile->TrapezTab[keyTrap].lamX2;
      }
      else  // Sinon placarde toute la surface
      {  SurfDD->DestRect.top = Y;
         SurfDD->DestRect.left = X;
         SurfDD->DestRect.bottom = Y + SurfDD->PixHeight;
         SurfDD->DestRect.right = X + SurfDD->PixWidth;
      }

      // G�re le clipping
      if (SurfDD->DestRect.top < 0) { SurfDD->SrcRect.top -= SurfDD->DestRect.top; SurfDD->DestRect.top = 0; };
      if (SurfDD->DestRect.bottom >= SurfViewY) { SurfDD->SrcRect.bottom = SurfViewY - SurfDD->DestRect.top; SurfDD->DestRect.bottom = SurfViewY; };
      if (SurfDD->DestRect.left < 0) { SurfDD->SrcRect.left -= SurfDD->DestRect.left; SurfDD->DestRect.left = 0; };
      if (SurfDD->DestRect.right >= SurfViewX) { SurfDD->SrcRect.right += SurfViewX - SurfDD->DestRect.right; SurfDD->DestRect.right = SurfViewX; };

      // Applique le filtre d'ombre
      if ((tile->DrawMethode & TILEDECORS_DRAW_SHADOW) == TILEDECORS_DRAW_SHADOW)
      {	SpriteDrawShadowClip(pScreen, SurfDD);
      } else
      { SurfDD->Draw(pScreen);
      }

      // Remet la surface source correctemnt
      SurfDD->SrcRect.top = 0;
      SurfDD->SrcRect.left  = 0;
      SurfDD->SrcRect.bottom = SurfDD->PixHeight;
      SurfDD->SrcRect.right = SurfDD->PixWidth;
    }
    else // 2) Dans le cas d'un objet ANIME en plein animation
    { // Remonte jusqu'au bon sprite de la SpriteListe
      SpriteTile *Sprite = DecObj->Element->STLObjets->SprIndex[DecObj->peSprNumLib];

      // Tiens compte du centre de gravit� <=> sommet nord du trap�ze principal
      int anm = DecObj->peNumAnim; // Trouve le n� de l'action courante
      //TileDecors* tileAssoc = DULib->getTileDecors(DecObj->peTileDecorsAssoc);
      //  X -= tileAssoc->TrapPrincX; et  Y -= tileAssoc->TrapPrincY;
      X -= DecObj->Element->AnimInfos[anm].dX;
      Y -= DecObj->Element->AnimInfos[anm].dY;
      X -= Sprite->GravX;
      Y -= Sprite->GravY;

      // Affichage de l'ombrage transluente si son TileDecors le repr�sentant a une ombre
      if ((DecObj->DecTileObj->DrawMethode & TILEDECORS_DRAW_SHADOW) == TILEDECORS_DRAW_SHADOW)
      {	SpriteDrawShadowClip(X, Y, pScreen, Sprite);
      }
      else
      // Affichage classique sans ombre
      { // Clipping
        int ScX = X;
        int ScY = Y;
        int Height = Sprite->Height;
        int Width  = Sprite->Height;
        RECT rectg = Sprite->SrcRect;
        // a) Regarde si le sprite n'est pas en dehors de la fen�tre
        if ((X>=SurfViewX) || (Y>=SurfViewY)) return;
        if ((X+Height<0) || (Y+Width<0)) return;
        // b) R�ajuste le rectangle                                        // BIDOUILLE
        if (ScX+Width  >= SurfViewX) { rectg.right-=  ScX+Width - SurfViewX -4; } // d�borde � droite
        if (ScY+Height >= SurfViewY) { rectg.bottom-= ScY+Height - SurfViewY; } // d�borde en bas
        if (ScX<0) { rectg.left-=ScX; ScX = 0; }   // d�borde � gauche
        if (ScY<0) { rectg.top-=ScY; ScY = 0; }    // d�borde en haut
        // c) Affiche le sprite � l'�cran
        HRESULT hr;
        hr = pScreen->pDDSurf->BltFast(ScX,ScY,Sprite->pFrame->pDDSurf,&rectg,DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
      }
    }

    // Affiche les animations superposables "classiques"
    if (DecObj->DecorsAnime) DecObj->RenderClassikAnims(ScrX, ScrY);

};
//----------------------------------------------------------------------------//

//== Affiche l'�cran finale ==================================================//
void TViewMap::AfficheViewCamera()
{
// BlitMode = DOUBLE_BUFFERING_SM ou RECALCULE, au choix ...
   if ((AFFICH_MODE == LAYER_ALL) || (AFFICH_MODE == LAYER_SOL))
    {
     switch (BlitMode)
      {
        case (DOUBLE_BUFFERING_SM) :
         {
          // V�rifie si la cam�ra est � une nouvelle position tr�s �loign�e de l'ancienne
          //  => ds ce cas, recalcule la vue globale
          if (NewCamera)   // Calcul le Layer du sol
          { AfficheMap(pDDSBackFond1, LAYER_SOL, ScreenX, ScreenY);
            NewCamera = FALSE; };
          // Copie d'une partie de pDDSBackFond' ds pDDSBackFond" d'apr�s le chgt de carte
          // Calcule la zone � d�placer
           int dx = ScreenX - OldScreenX;
           int dy = ScreenY - OldScreenY;
           // init
           pDDSBackFond1->SrcRect.top = pDDSBackFond1->DestRect.top = 0;
           pDDSBackFond1->SrcRect.left = pDDSBackFond1->DestRect.left = 0;
           pDDSBackFond1->SrcRect.bottom = pDDSBackFond1->DestRect.bottom = ScreenViewY;
           pDDSBackFond1->SrcRect.right = pDDSBackFond1->DestRect.right = ScreenViewX;
           pDDSBackFond2->SrcRect.top = pDDSBackFond2->DestRect.top = 0;
           pDDSBackFond2->SrcRect.left = pDDSBackFond2->DestRect.left = 0;
           pDDSBackFond2->SrcRect.bottom = pDDSBackFond2->DestRect.bottom = ScreenViewY;
           pDDSBackFond2->SrcRect.right = pDDSBackFond2->DestRect.right = ScreenViewX;
           if ((dx!=0) || (dy!=0))
            {
               if (dx!=0)
                {
                  if (dx<0) { pDDSBackFond1->SrcRect.right += dx; pDDSBackFond1->DestRect.left -= dx; }
                  else { pDDSBackFond1->DestRect.right -= dx; pDDSBackFond1->SrcRect.left += dx; };
                };
               if (dy!=0)
                {
                  if (dy<0) { pDDSBackFond1->SrcRect.bottom += dy; pDDSBackFond1->DestRect.top -= dy; }
                  else	{ pDDSBackFond1->DestRect.bottom -= dy; pDDSBackFond1->SrcRect.top += dy; };
                };
           // Copie ds le buffer de travail => pDDSBackFond1 sauvegard�

           // Travail sur le nouveau buffer de sortie
           DOLib->ObjetNonDessine();
           pDDSBackFond2->Fill(0);
           pDDSBackFond1->DrawNoKey(pDDSBackFond2);	// puis �crit par dessus
           CompleteMap(pDDSBackFond2, dx, dy, ScreenX, ScreenY);  		// compl�te d'abord la carte

           pDDSBackFond2->DrawNoKey(DS->pDDSBack);	// Affiche le sols � l'�cran
           // permutation de pointeur
           DDSurface *temp;
           temp = pDDSBackFond2;
           pDDSBackFond2 = pDDSBackFond1;
           pDDSBackFond1 = temp;
          }
         else  pDDSBackFond1->DrawNoKey(DS->pDDSBack);			// Affiche tout simplement le sols � l'�cran
         }; break;

        case (RECALCULE) :
         {  DS->Fill(0);
            DOLib->ObjetNonDessine();
            AfficheFastMap(DS->pDDSBack, LAYER_SOL, ScreenX, ScreenY);
         }; break;
        case (CALCUL_TAB) :
         {  if (NewCamera)   // Calcul le Layer du sol
             { SolScroll->DrawTabBloc(ViewMap->ScreenX - SolScroll->BlocWidth, ViewMap->ScreenY - SolScroll->BlocHeight);
               NewCamera = FALSE; };
               // DS->Fill(0);
               SolScroll->Tab2Screen(ViewMap->ScreenX, ViewMap->ScreenY);
         }; break;

      };
     AfficheMap(DS->pDDSBack, LAYER_DECORS, ScreenX, ScreenY);			// Affiche les d�cors
   };

   if (AFFICH_MODE == LAYER_DECORS)
     {  DS->Fill(RGBto16(10,130,10));					// peint en vers le sol
        AfficheMap(DS->pDDSBack, LAYER_DECORS, ScreenX, ScreenY);			// Affiche les d�cors
     };
};
//----------------------------------------------------------------------------//

//=== Met � jour les infos de la carte =======================================//
// -> en particulier avec les objets anim�s du d�cors rafraichis
void TViewMap::Upgrade()
{  // 1) Upgrade les objets anim�s du d�cors ayant chang� de TileDecors associ�s
   for (uint j=0; j < DOLib->DecorsUpgraded.size(); j++)
  {   // Initialisations
      DecorsObjet* decPtr = DOLib->DecorsUpgraded[j];
      TMap *pMap =  MapNiveau->PrincEtage->LayerDecors;
      // a) Efface de la carte les donn�es de l'ancien TileDecors
      //    -> d'abord du trap�ze principal
      int PosU = decPtr->peOldPosU;
      int PosV = decPtr->peOldPosV;
      pMap->Carte[PosU][PosV].resetInfos();   // init
      pMap->Carte[PosU][PosV].setPrincipal(false);
      pMap->Carte[PosU][PosV].ptrObjet = NULL;
      pMap->Carte[PosU][PosV].setAccessible(true);
      pMap->Carte[PosU][PosV].TrapCollide = PARTIELLE;
      //    -> puis des trap�zes secondaires
      for (int i=1;  i < decPtr->DecTileObj->NbTrapeze;  i++)
      {  int dU = decPtr->OldSecondTile->TrapezTab[i].TrapU;
         int dV = decPtr->OldSecondTile->TrapezTab[i].TrapV;
         if ((PosU + dU>=0) && (PosU + dU<MapNiveau->NivWidth) && (PosV + dV>=0) && (PosV + dV < MapNiveau->NivHeight))
            if (pMap->Carte[PosU + dU][PosV + dV].isPrincipal() == false)
            { pMap->Carte[PosU + dU][PosV + dV].resetInfos();  // init
              pMap->Carte[PosU + dU][PosV + dV].setPrincipal(false);
              pMap->Carte[PosU + dU][PosV + dV].ptrObjet = NULL;
              pMap->Carte[PosU + dU][PosV + dV].setAccessible(true);
              pMap->Carte[PosU + dU][PosV + dV].TrapCollide = PARTIELLE;
            }
      }
      // b) Puis ajoute sur la carte les donn�es du nouveau TileDecors
      //    -> d'abord du trap�ze principal
      PosU = decPtr->PosU;
      PosV = decPtr->PosV;
      pMap->Carte[PosU][PosV].resetInfos();   // init
      pMap->Carte[PosU][PosV].setPrincipal(TRUE);
      pMap->Carte[PosU][PosV].ptrObjet = decPtr;
      pMap->Carte[PosU][PosV].setAccessible(decPtr->SecondTile->TrapezTab[0].Accessible==TRUE);
      pMap->Carte[PosU][PosV].TrapCollide = decPtr->SecondTile->TrapezTab[0].TrapCollide;
      // Trap�ze Secondaire
      for (int i2=1;  i2 < decPtr->DecTileObj->NbTrapeze;  i2++)
       {
         int dU = decPtr->SecondTile->TrapezTab[i2].TrapU;
         int dV = decPtr->SecondTile->TrapezTab[i2].TrapV;
         if ((PosU + dU>=0) && (PosU + dU<MapNiveau->NivWidth) && (PosV + dV>=0) && (PosV + dV < MapNiveau->NivHeight))
           if (pMap->Carte[PosU + dU][PosV + dV].isPrincipal() == FALSE)
            {pMap->Carte[PosU + dU][PosV + dV].resetInfos();  // init
             pMap->Carte[PosU + dU][PosV + dV].setPrincipal(false);
             pMap->Carte[PosU + dU][PosV + dV].ptrObjet = decPtr;
             pMap->Carte[PosU + dU][PosV + dV].setAccessible(decPtr->SecondTile->TrapezTab[i2].Accessible==TRUE);
             pMap->Carte[PosU + dU][PosV + dV].TrapCollide = decPtr->SecondTile->TrapezTab[i2].TrapCollide;
            };
        };

  }
  DOLib->DecorsUpgraded.clear();   // Efface les objtets contenus de cette bo�te une fois celle-ci parcourue
}
//----------------------------------------------------------------------------//

////////////////////////////// Fin de VIEWMAP //////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Objet TETAGE      													      //
////////////////////////////////////////////////////////////////////////////////

//== Constructeur ============================================================//
TEtage::TEtage()
{
  // Alloue la mem pour les diff�rents layers
  LayerSols   = new TMap;
  LayerDecors = new TMap;
};
//----------------------------------------------------------------------------//

//== Destructeur =============================================================//
TEtage::~TEtage()
{
	LayerDecors->DessaloueCarte();
	LayerSols->DessaloueCarte();
};
//----------------------------------------------------------------------------//

//== Initialise les diff�rents layers ========================================//
void TEtage::AlloueCarte(int Width, int Height)
{
    // de l'�tage
    EtageWidth  = Width;
    EtageHeight = Height;
    // des layers
    LayerSols->AlloueCarte(Width, Height);
    LayerDecors->AlloueCarte(Width, Height);
};
//----------------------------------------------------------------------------//


////////////////////////////////////////////////////////////////////////////////
// Objet TMAPNIVEAU  													      //
////////////////////////////////////////////////////////////////////////////////

//== Constructeur ============================================================//
TMapNiveau::TMapNiveau()
{
  MapNum = 0;
  strcpy(Name,"");
  strcpy(DTLibName,"");
  NbEtage = 0;
  NivWidth = NivHeight = 0;

  // Alloue la mem pour l'�tage de r�f�rence/principal
  PrincEtage = new TEtage;
  PrincEtage->Reference = TRUE;
  PrincEtage->Altitude = 0;
  PrincEtage->EtageU = PrincEtage->EtageV = 0;

  // On cr�e la liste des zones
  pMapZoneList = new MapZoneList();
}
//----------------------------------------------------------------------------//

//== Destructeur =============================================================//
TMapNiveau::~TMapNiveau()
{
	delete pMapZoneList;
	delete PrincEtage;
}
//----------------------------------------------------------------------------//

//== Fourni les Donn�es aux Layers & � l'Etage ===============================//
void TMapNiveau::SetInfos(int num,char nom[15])
{
    // de l'�tage
    MapNum = num;
    strcpy(Name, nom);
}
//----------------------------------------------------------------------------//

//== Pr�pare la carte au chargement ==========================================//
void TMapNiveau::PrepareLevel(int PrincWidth, int PrincHeight)
{
    NivWidth  = PrincWidth;
    NivHeight = PrincHeight;
    NbEtage = 1;  // ??
    PrincEtage->AlloueCarte(NivWidth, NivHeight);
}
//----------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////////////////
// Fonctions Annexes 														  //
////////////////////////////////////////////////////////////////////////////////

//== Fonctions Chargement de map *.niv =======================================//
void LoadMapNiveau(TMapNiveau *MapNiv, AnsiString NivName)
{
        if (NivName == "") { MessageBoxA(NULL,"Aucun Fichier","Warning !",0); return; };

        // On peut charger le fichier
        FILE *f;
		#ifdef CBUILDER
		if ((f = fopen(NivName.c_str(),"rb"))==NULL)
    		{ MessageBoxA(NULL,"Impossible de charger la carte :",NivName.c_str(),0); return; };
		#else
		if ((f = fopen(LPCSTR(NivName),"rb"))==NULL)
    		{ MessageBoxA(NULL,"Impossible de charger la carte :",LPCSTR(NivName),0); return; };
		#endif

	    // Version d'un niveau
        float Version;
		fread(&Version, sizeof(float), 1, f);
        if (Version > VersLiL) 
		{	
			#ifdef CBUILDER
			MessageBoxA(NULL,NivName.c_str(),"Version trop ancienne !",0); 
			#else
			MessageBoxA(NULL,LPCSTR(NivName),"Version trop ancienne !",0); 
			#endif
			return; 
		};

     //===================== CREATION DE LA CARTE ==============================
        if (MapNiv!=NULL) delete MapNiv;
        MapNiv = new TMapNiveau;

        // Infos sur la carte
        fread(MapNiv->Name, sizeof(MapNiv->Name), 1, f);
	    fread(&MapNiv->MapNum, sizeof(int), 1, f);
    	fread(&MapNiv->NivWidth, sizeof(int), 1, f);
    	fread(&MapNiv->NivHeight, sizeof(int), 1, f);
    	fread(MapNiv->DTLibName, sizeof(MapNiv->DTLibName), 1, f);
        fread(&MapNiv->NbEtage, sizeof(int), 1, f);		// Nb d'�tage du niveau

        // Prepare UNIQUEMENT l'�tage PRINCIPAL
		MapNiv->PrepareLevel(MapNiv->NivWidth, MapNiv->NivHeight);
        MapNiv->SetInfos(MapNiv->MapNum, MapNiv->Name );

 	    // Calcule certaines infos de la MapView
        //**New**
        if (ViewMap!=NULL) delete ViewMap;
        ViewMap = new TViewMap;

		ViewMap->CalcInfos(MapNiv->NivWidth, MapNiv->NivHeight);
    	ViewMap->PosCoinUV();
        //**Try**
		ViewMap->OldScreenX = ViewMap->ScreenX = - ViewMap->ScreenViewX/2;
		ViewMap->OldScreenY = ViewMap->ScreenY = 0;

     //===================== CHARGEMENT DE LA LIB ==============================
        if (DULib!=NULL) delete DULib;
        DULib = new TLevelTileLib;
        AnsiString NameLib;
        NameLib =  MapNiv->DTLibName;
        NameLib += ".LiL";
        DULib->LoadFile(CheminApplication + cPathLevels + NameLib);
		#ifdef CBUILDER
		strcpy(MapNiv->DTLibName, DULib->Nom.c_str());
		#else
		strcpy(MapNiv->DTLibName, LPCSTR(DULib->Nom));
		#endif

        // Puis alloue les surfaces DDraw et charge les bitmaps dedans
        DULib->AlloueDDSurface();

        // Nouvelle liste d'objet
        if (DOLib!=NULL) delete DOLib;
        DOLib = new DecorsObjetLib;

     //===================== CHARGEMENT DES DECORS =============================
     	int NbObjLib;
        fread(&NbObjLib, sizeof(int), 1, f);		       // nb d'objet
        DecorsObjet	*ptr;
        for (int n=0; n < NbObjLib ; n++)
        {
           ptr = new DecorsObjet;
           int NumObjDecors;
           fread(&NumObjDecors, sizeof(int), 1, f);	       // n� de l'objet
           fread(&ptr->PosU, sizeof(int), 1, f);            // position de l'objet
           fread(&ptr->PosV, sizeof(int), 1, f);
           fread(&ptr->ObjEtage, sizeof(int), 1, f);	       // Etage auquel il appartient
           fread(&ptr->ObjLayer, sizeof(int), 1, f);	       // Layer    "    "    "

			//--- Charge les informations concernant le GameObject ---
			// Rmq : rajout� depuis la version 2.1 le 13/09/99
			if (Version >= 2.1-0.0001)
			{	ptr->LoadGOInfosFromFile(f, Version);
			}

			// charge toutes les autres infos relatives � l'objet-d�cors
			// ...


           // On cherche sur quelle Image-d�cors il pointe
           // A partir du n� de l'objet, on parcourt la liste d'objet
           int i=0;
           TileDecors* yo = DULib->TileMiniLibList[i];
           ptr->DecTileObj = NULL;
           while ((ptr->DecTileObj==NULL) && (i < cNbTypeTiles))
              {
                  if (yo!=NULL)
                  {
                    if (yo->DecorsNum == NumObjDecors)  
					{	ptr->DecTileObj = yo;
					} else
					{   yo = yo->Suiv;
					}
                  }
                  if (yo==NULL)  { i++; if (i<cNbTypeTiles) yo = DULib->TileMiniLibList[i]; }
              };
                  // Si objet trouv�, (sinon ... ou a-t-il disparu ?)
             if ((i < cNbTypeTiles) && (ptr->DecTileObj!=NULL) && (ptr->DecTileObj->DecorsNum==NumObjDecors))
              {
				  // Ajout de l'objet-d�cors aux listes : 
                  DOLib->AddDecObj(ptr);		// des DecorsObjet
				  GOList->AddGameObject(ptr);	// des GameObject 

				  // on remplit la Map des indications de l'objet suivant l'Etage & le Layer auquel il appartient
                  TEtage *Etage;		// On d�termine l'�tage
                  switch (ptr->ObjEtage)
                   {
						case (Etage_PRINCIPAL) : Etage = MapNiv->PrincEtage; break;
						default : 
							#ifdef CBUILDER
							Application->Terminate();
							#else
							_asm int 3;
							#endif
                   };
                   TMap *pMap;			// puis le Layer
                  switch (ptr->ObjLayer)
                   {
                    case (Layer_DECORS): pMap = Etage->LayerDecors; break;
                    case (Layer_SOLS)  : pMap = Etage->LayerSols;	break;
                    default : 
						#ifdef CBUILDER
						Application->Terminate();
						#else
						_asm int 3;
						#endif
                   };

                 // Trap�ze Principal
                 int PosU = ptr->PosU;
                 int PosV = ptr->PosV;
                 pMap->Carte[PosU][PosV].resetInfos();   // init
                 pMap->Carte[PosU][PosV].setPrincipal(TRUE);
                 pMap->Carte[PosU][PosV].ptrObjet = ptr;
                 pMap->Carte[PosU][PosV].setAccessible(ptr->DecTileObj->TrapezTab[0].Accessible==TRUE);
                 pMap->Carte[PosU][PosV].TrapCollide = ptr->DecTileObj->TrapezTab[0].TrapCollide;
                 // Trap�ze Secondaire
                 for (int i=1;  i < ptr->DecTileObj->NbTrapeze;  i++)
                   {
                     int dU = ptr->DecTileObj->TrapezTab[i].TrapU;
                     int dV = ptr->DecTileObj->TrapezTab[i].TrapV;
                     if ((PosU + dU>=0) && (PosU + dU<MapNiv->NivWidth) && (PosV + dV>=0) && (PosV + dV < MapNiv->NivHeight))
                       if (pMap->Carte[PosU + dU][PosV + dV].isPrincipal() == FALSE)
                        {pMap->Carte[PosU + dU][PosV + dV].resetInfos();  // init
                         pMap->Carte[PosU + dU][PosV + dV].setPrincipal(FALSE);
                         pMap->Carte[PosU + dU][PosV + dV].ptrObjet = ptr;
                         pMap->Carte[PosU + dU][PosV + dV].setAccessible(ptr->DecTileObj->TrapezTab[i].Accessible==TRUE);
                         pMap->Carte[PosU + dU][PosV + dV].TrapCollide = ptr->DecTileObj->TrapezTab[i].TrapCollide;
                        };
                    };
              }
               else // pas trouv�
              {
                delete ptr;
              }

           //*** Nouveau depuis le 11/05/1999 ***
           // => V�rifie si ce n'est pas un objet anim� ayant un .ina associ�
           TAnimTileDecors* ptrAnimTileDecors;
           ptrAnimTileDecors = getAnimTileDecors(ptr->DecTileObj->DecorsNum);
           if (ptrAnimTileDecors!=NULL)  // Il a une anim associ�e
           { // ptr->Element = ptrAnimTileDecors;
             ptr->SetAnimTileDecors(ptrAnimTileDecors);
             ptr->DecorsAnime = true;
             // Param�tres sp�ciaux pour objets anim�s � initialiser
             ptr->peTileDecorsAssoc = ptr->Element->AnimInfos[ptr->peNumAnim].numTileDecorsAssoc;
             ptr->SecondTile = DULib->getTileDecors(ptr->Element->AnimSpriteLib[ptr->peNumAnim]->FirstSprite);
           }
        }

		// On peut charger les diverses zones du niveau
		if (((float) Version) >= 2.5f)	
		{	MapNiv->pMapZoneList->LoadZonesFromFile(*f);
		}

     	fclose(f);
        MapNiveau = MapNiv;

 };
//----------------------------------------------------------------------------//

//=== D�charge les infos relatives au niveau =================================//
void UnLoadMapNiveau(TMapNiveau *MapNiv)
{
	//--- D�charge certaines librairies  ---
	delete DOLib; DOLib = NULL;		// Objet-D�cors
	delete DULib; DULib = NULL;		// D�cors-Tiles 

	//--- Supprime le gestionnaire de repr�sentation graphique de la map ---
	delete ViewMap; ViewMap = NULL;
	
	//--- Lib�re le niveau courant ---
	delete MapNiv; MapNiv = NULL;
}
//----------------------------------------------------------------------------//

//////////////////// FIN des fonctions Annexes /////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Objet TREFRESH															  //
////////////////////////////////////////////////////////////////////////////////

//== Constructeur ============================================================//
TRefresh::TRefresh()
{
  // tout � z�ro
  RefreshFixed = FALSE;
  FramePerSecond = 0;
  FrameInterval = 0;
  BestFreqAvail = 0;

  // V�rifie si l'on peut utiliser QueryPerformanceFrequency()
  BestFreqAvail=QueryPerformanceFrequency( &BestFreq );
  LastFrameTiming = GetTime();	// au d�part
}
//----------------------------------------------------------------------------//

//== Renvoie le temps �coul� depuis le d�marage de l'ordi ====================//
DWORD TRefresh::GetTime()
{
	DWORD MilliSec;
	LARGE_INTEGER qpcTicks;

	if(BestFreqAvail)
	{   QueryPerformanceCounter( &qpcTicks );
		MilliSec=(DWORD) (1000.0 * ((double)(qpcTicks.QuadPart)) /  ((double)(BestFreq.QuadPart)));
	}
	else MilliSec=GetTickCount();
	return MilliSec;
}
//----------------------------------------------------------------------------//

//== Nouveau Rafraichissement ================================================//
void TRefresh::NewFps(double fps)
{
	FramePerSecond = fps;
    FrameInterval = (DWORD) ((double) 1000 / fps);
}
//----------------------------------------------------------------------------//

//== Renvoie VRAI si c'est le moment de changer de frame =====================//
BOOL  TRefresh::FlipFrame()
{
	DWORD time;
    time = GetTime();
    if (time - LastFrameTiming >= FrameInterval)  // on peut y aller
      {
      	 LastFrameTiming = time;
		 return TRUE;
      }
    else return FALSE;
}
//----------------------------------------------------------------------------//

////////////////////////// FIN Objet TRefresh //////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Objet TSOLSCROLL															  //
////////////////////////////////////////////////////////////////////////////////

//== Constructeur ============================================================//
TSolScroll::TSolScroll(int nbblocs, int screenW, int screenH)
{
	NbSizeBloc = nbblocs;				// d�coupage en tel nb de blocs
    BlocWidth  = screenW / nbblocs;     // dimensions d'un bloc
    BlocHeight = screenH / nbblocs;
    ResWidth   = screenW;				// r�solution actuelle
    ResHeight  = screenH;

    // Pr�paration de TabBloc
    // Alloue la m�moire n�cessaire au tableau 2D
	TabBloc = new DDSurface** [NbSizeBloc+2];// lignes
    for (int i=0 ; i < NbSizeBloc+2; i++)   // colonnes
    		TabBloc[i] = new DDSurface* [NbSizeBloc+2];
    // Alloue ensuite les DDSurfaces
	for (int i2=0 ; i2 < NbSizeBloc+2 ; i2++)
    	for (int j=0 ; j < NbSizeBloc+2 ; j++)
            TabBloc[i2][j] = new DDSurface(DS, BlocWidth, BlocHeight, VIDEO_MEMORY);
};
//----------------------------------------------------------------------------//

//== Destructeur =============================================================//
TSolScroll::~TSolScroll()
{
    // D�salloue toutes les DDSurfaces
	for (int i=0 ; i < NbSizeBloc+2 ; i++)
    	for (int j=0 ; j < NbSizeBloc+2 ; j++)
            delete TabBloc[i][j];
	for (int i2=0; i2 < NbSizeBloc+2; i2++) delete[] TabBloc[i2]; // lignes
    delete[] TabBloc;											  // colonnes

};
//----------------------------------------------------------------------------//

//== Remplit les DDSurfaces de TabBloc =======================================//
void TSolScroll::DrawTabBloc(int tabscrX, int tabscrY)
{
	TabScreenX = tabscrX;
    TabScreenY = tabscrY;
    // => fait appel aux fonctions de TViewMap
   	for (int i=0 ; i < NbSizeBloc+2 ; i++)
    	for (int j=0 ; j < NbSizeBloc+2 ; j++) DrawBloc(i,j);
};
//----------------------------------------------------------------------------//

//== Remplit Une DDSurface de TabBloc ========================================//
void TSolScroll::DrawBloc(int blocI, int blocJ)
{
    // Transformer AfficheMap -> AfficheFastMap
    DOLib->ObjetNonDessine();
    TabBloc[blocI][blocJ]->Fill(0);
    ViewMap->AfficheFastMap(TabBloc[blocI][blocJ], LAYER_SOL, TabScreenX + blocI*BlocWidth, TabScreenY + blocJ*BlocHeight);
};
//----------------------------------------------------------------------------//
//== Dessine la Tab � l'�cran ================================================//
void TSolScroll::Tab2Screen(int PosX, int PosY)
{
	// D�finit le bloc sur lequel d�buter
    int  DiffX = PosX - TabScreenX;
    int  DiffY = PosY - TabScreenY;
    int TabU = DiffX / BlocWidth;
    int TabV = DiffY / BlocHeight;

    // Coordonn�es int�rieures au 1er Bloc par laquelle commencer
    int ScrX = DiffX - TabU * BlocWidth;
    int ScrY = DiffY - TabV * BlocHeight;

     int AffY = 0;

     if ((TabU == 2) || (TabV==2))
	 { 
		#ifdef CBUILDER
		Application->Terminate();
		#else
		_asm int 3;
		#endif
	 }     

	 int NbScrollBlocX;
     int NbScrollBlocY;
     if (ScrX==0) NbScrollBlocX = NbSizeBloc;
     else NbScrollBlocX = NbSizeBloc + 1;		// Nb de blocs max par �crans
     if (ScrY==0) NbScrollBlocY = NbSizeBloc;
     else NbScrollBlocY = NbSizeBloc + 1;		// Nb de blocs max par �crans

//   if ((ScrX!=0) && (ScrY!=0))  // cas Particulier
     for (int j=0; j < NbScrollBlocY ; j++)
      { int AffX = 0;
        for (int i=0; i < NbScrollBlocX ; i++)
        {
      //      if ((TabU+i > NbSizeBloc+1) || (TabV+j > NbSizeBloc+1)) break;

            RECT tempDest = TabBloc[TabU+i][TabV+j]->DestRect;
            RECT tempSrc  = TabBloc[TabU+i][TabV+j]->SrcRect;

            TabBloc[TabU+i][TabV+j]->DestRect.left = AffX;
            TabBloc[TabU+i][TabV+j]->DestRect.top  = AffY;
            TabBloc[TabU+i][TabV+j]->DestRect.right   = AffX + BlocWidth - ScrX;
            TabBloc[TabU+i][TabV+j]->DestRect.bottom   = AffY + BlocHeight - ScrY;

            if (i==0)  // coin � gauche
              {  TabBloc[TabU+i][TabV+j]->DestRect.left = AffX;
                 TabBloc[TabU+i][TabV+j]->SrcRect.left = ScrX;
                 TabBloc[TabU+i][TabV+j]->DestRect.right   = AffX + BlocWidth - ScrX;
                 TabBloc[TabU+i][TabV+j]->SrcRect.right = BlocWidth;
              }
            else if ((i>0) & (i<NbSizeBloc))
              {	TabBloc[TabU+i][TabV+j]->DestRect.left = AffX - ScrX;
                 TabBloc[TabU+i][TabV+j]->DestRect.right = AffX + BlocWidth - ScrX;
                 TabBloc[TabU+i][TabV+j]->SrcRect.right = BlocWidth;
              }
            else if (i==NbSizeBloc)
              {	TabBloc[TabU+i][TabV+j]->DestRect.left = AffX - ScrX;
                 TabBloc[TabU+i][TabV+j]->DestRect.right = AffX + BlocWidth - ScrX;
                 TabBloc[TabU+i][TabV+j]->SrcRect.right = ResWidth - AffX+ScrX;

              };
            if (j==0)  // coin en haut
              {  TabBloc[TabU+i][TabV+j]->DestRect.top = AffY;
                 TabBloc[TabU+i][TabV+j]->SrcRect.top = ScrY;
                 TabBloc[TabU+i][TabV+j]->DestRect.bottom   = AffY + BlocHeight - ScrY;
                 TabBloc[TabU+i][TabV+j]->SrcRect.bottom = BlocHeight;
              }
            else if ((j>0) & (j<NbSizeBloc))
              {	 TabBloc[TabU+i][TabV+j]->DestRect.top = AffY - ScrY;
                 TabBloc[TabU+i][TabV+j]->DestRect.bottom = AffY + BlocHeight - ScrY;
                 TabBloc[TabU+i][TabV+j]->SrcRect.bottom = BlocHeight;
              }
            else if (j==NbSizeBloc)
              {	TabBloc[TabU+i][TabV+j]->DestRect.top = AffY - ScrY;
                TabBloc[TabU+i][TabV+j]->DestRect.bottom = AffY + BlocHeight - ScrY;
                TabBloc[TabU+i][TabV+j]->SrcRect.bottom = ResHeight - AffY+ScrY;

              }
	      SolScroll->TabBloc[TabU+i][TabV+j]->DrawFast(DS->pDDSBack, TabBloc[TabU+i][TabV+j]->DestRect.left, TabBloc[TabU+i][TabV+j]->DestRect.top);


            TabBloc[TabU+i][TabV+j]->SrcRect = tempSrc;
            TabBloc[TabU+i][TabV+j]->DestRect = tempDest;
            AffX += BlocWidth;
        };
      AffY += BlocHeight;
      };
//   SolScroll->TabBloc[TabU][TabV]->DrawFast(DS->pDDSBack,ScrX,ScrY);


    BOOL Decal = FALSE;
    // V�rifications des d�bordements
    if ((TabU == 0) && (ScrX < BlocWidth/4))     //  D�placement � gauche
        { Tab2Left(); Decal = TRUE; }
    if ((TabU == 1) && (ScrX >= 3*BlocWidth/4))  //  D�placement � droite
        { Tab2Right(); Decal = TRUE; }
    if ((TabV == 0) && (ScrY < BlocHeight/4))    //  D�placement en haut
        { Tab2Top(); Decal = TRUE; }
    if ((TabV == 1) && (ScrY >= 3*BlocHeight/4)) //  D�placement � bas
        { Tab2Bottom(); Decal = TRUE; }

};
//----------------------------------------------------------------------------//

//== D�calement de TabBloc vers la gauche ====================================//
void TSolScroll::Tab2Left()
{
	TabScreenX -= BlocWidth;

    // Permutation circulaire vers la doite
    for (int j=0 ; j < NbSizeBloc+2 ; j++)
      {	 // Sauvegarde du dernier �l�ment => premier � la fin
         DDSurface  *Temp = TabBloc[NbSizeBloc+1][j];
         for (int i= NbSizeBloc+1 ; i >= 0; i--)
           {  if ( i == 0) TabBloc[i][j] = Temp;
              else          TabBloc[i][j] = TabBloc[i-1][j];
           };
      };
    // Puis recalcule les bords
    for (int u=0; u < NbSizeBloc+2 ; u++) { TabBloc[0][u]->Fill(0); DrawBloc(0,u); }
};
//----------------------------------------------------------------------------//

//== D�calement de TabBloc vers le haut  =====================================//
void TSolScroll::Tab2Top()
{
	TabScreenY -= BlocHeight;
    // Permutation circulaire vers le bas
    for (int i= 0 ; i < NbSizeBloc+2; i++)
      { // Sauvegarde du dernier �l�ment => premier � la fin
        DDSurface  *Temp = TabBloc[i][NbSizeBloc+1];
	   	for (int j= NbSizeBloc+2 ; j >= 0; j--)
           {  if ( j == 0) TabBloc[i][j] = Temp;
              else          TabBloc[i][j] = TabBloc[i][j-1];
           };
      };
    // Puis recalcule les bords
    for (int u=0; u < NbSizeBloc+2 ; u++) { TabBloc[u][0]->Fill(0); DrawBloc(u,0); }
};
//----------------------------------------------------------------------------//

//== D�calement de TabBloc vers la gauche ====================================//
void TSolScroll::Tab2Right()
{
	TabScreenX += BlocWidth;
    // Permutation circulaire vers la gauche
   	for (int j=0 ; j < NbSizeBloc+2 ; j++)
      { // Sauvegarde du dernier �l�ment => premier � la fin
        DDSurface  *Temp = TabBloc[0][j];
    	for (int i= 0 ; i < NbSizeBloc+2 ; i++)
           {  if ( i == NbSizeBloc+1) TabBloc[i][j] = Temp;
              else          TabBloc[i][j] = TabBloc[i+1][j];
           };
      }
    // Puis recalcule les bords
    for (int u=0; u < NbSizeBloc+2 ; u++) {TabBloc[NbSizeBloc+1][u]->Fill(0); DrawBloc(NbSizeBloc+1,u); }
};
//----------------------------------------------------------------------------//

//== D�calement de TabBloc vers le bas =======================================//
void TSolScroll::Tab2Bottom()
{
	TabScreenY += BlocHeight;
    // Permutation circulaire vers le haut
    for (int i= 0 ; i < NbSizeBloc+2 ; i++)
      { // Sauvegarde du dernier �l�ment => premier � la fin
        DDSurface  *Temp = TabBloc[i][0];
	   	for (int j=0 ; j < NbSizeBloc+2 ; j++)
           {  if ( j == NbSizeBloc+1) TabBloc[i][j] = Temp;
              else          TabBloc[i][j] = TabBloc[i][j+1];
           };
      }
    // Puis recalcule les bords
    for (int u=0; u < NbSizeBloc+2 ; u++) { TabBloc[u][NbSizeBloc+1]->Fill(0);DrawBloc(u,NbSizeBloc+1); }
};
//----------------------------------------------------------------------------//

//=== Changement de mode-vid�o : restauration et rechargement des surfaces ===//
void TSolScroll::ChangeSolScrollVideoMode(int newScrW, int newScrH)
{	int i, j;
	// D�salloue les surfaces
	for (i=0 ; i < NbSizeBloc+2 ; i++)
	{	for (int j=0 ; j < NbSizeBloc+2 ; j++)
		{	delete TabBloc[i][j];
		}
	}	
	// R�alloue ensuite les DDSurfaces
	BlocWidth  = newScrW / NbSizeBloc;  // dimensions d'un bloc
    BlocHeight = newScrH / NbSizeBloc;
    ResWidth   = newScrW;				// r�solution actuelle
    ResHeight  = newScrH;
	for (i=0 ; i < NbSizeBloc+2 ; i++)
	{	for (j=0 ; j < NbSizeBloc+2 ; j++)
		{	TabBloc[i][j] = new DDSurface(DS, BlocWidth, BlocHeight, VIDEO_MEMORY);
		}
	}
}
//----------------------------------------------------------------------------//

////////////////////////////// FIN TSOLSCROLL //////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
// Objet TYBUFFER                                                             //
////////////////////////////////////////////////////////////////////////////////

//=== Supprime tous les �l�ments de la YFile =================================//
void TYBuffer::Clear()
{  YFile.clear();
}
//----------------------------------------------------------------------------//

//=== Ajoute � la liste un d�cors ============================================//
void TYBuffer::AddDecors(int cY, DecorsObjet* pDecors)
{  // Regarde si l'objet doit s'afficher enti�rement ou en lamelles
   if (pDecors->DecTileObj->TotalDraw)  // Affichage entier
   {  YFile.insert(std::make_pair(cY,tObjetAffichable(pDecors)));
   }
   else // Affichage en lamelles verticales
   {  // On regarde pour tous les trap�zes constituant le d�cors, lequel est clef d'affichage
      for (int i=0 ; i < pDecors->DecTileObj->NbTrapeze ; i++)
      { if (pDecors->DecTileObj->TrapezTab[i].DrawTrapezekey)  // On en a trouv� un
        { // Calcul le Y de ce trap�ze
          int newcY = cY + ViewMap->TransUVtoY(pDecors->DecTileObj->TrapezTab[i].TrapU,pDecors->DecTileObj->TrapezTab[i].TrapV);
          YFile.insert(std::make_pair(newcY,tObjetAffichable(pDecors,i)));
        }
      }
   }
}
//----------------------------------------------------------------------------//

//=== Ajoute � la liste un perso =============================================//
void TYBuffer::AddPersos(int cY, Personnage*  pPersos)
{  YFile.insert(std::make_pair(cY,tObjetAffichable(pPersos)));
}
//----------------------------------------------------------------------------//

//=== Affiche � l'�cran tous les objets de la liste ==========================//
void TYBuffer::Affiche(DDSurface *pScreen)
{ tFileObjetAffichable::iterator iter;
  for (iter = YFile.begin() ; iter!=YFile.end() ; iter++)
  {  switch ((*iter).second.typeObjY)
     {  case (OBJDECORS) :    // D�cors � afficher
          ViewMap->AfficheObjDecors((*iter).second.ptrDecorsObjet, pScreen, ViewMap->ScreenX, ViewMap->ScreenY,(*iter).second.KeyTrap);
          break;
        case (PERSOS) :       // Personnage � afficher
          (*iter).second.ptrPersonnage->peFastDraw(pScreen);
          break;
        case (UNKNOW) :
          #ifdef MAPLIB_DEBUG
          MessageBoxA(NULL,"TYBuffer::Affiche = type d'objet inconnu","Warning !",0);
          #ifdef CBUILDER
		  Application->Terminate();
		  #else
		  _asm int 3;
		  #endif
          #endif
          break;
     }
  }
}
//----------------------------------------------------------------------------//

//////////////////////////////// FIN TYBUFFER //////////////////////////////////

