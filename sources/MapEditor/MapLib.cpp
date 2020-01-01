////////////////////////////////////////////////////////////////////////////////
// MapLib.cpp  :  D�finitions de tous les �l�ments d'une carte				  //
////////////////////////////////////////////////////////////////////////////////

//- Donn�es � inclure ------------------------------------------------------
#include <vcl\vcl.h>
#include <Math.h>
#pragma hdrstop
#include "consts.h"
#include "uMapView.h"
#include "fMapOutils.h"
#include "ImDecors2.h"
#include "DDLibEditor.h"
#include "fLibOutils.h"
#include "fPathFinding.h"
#include "MapZone.h"
#include "fDesignOutils.h"
#include "GameDebug.h"
#include "MapLib.h"

//---------------------------------------------------------------------------

//== Variables ===============================================================//
//TMap *Map = NULL;					// Carte d'un niveau
TViewMap *ViewMap = NULL;			// Gestion de la fiche de repr�sentation de la carte
TMapNiveau *MapNiveau = NULL;		// Carte d'un niveau

const int Coul_POSITION       = 0xF800;	// Rouge
const int Coul_TRAPPRINCIPAL  = 0x001F;	// Bleu
const int Coul_TRAPSECONDAIRE = 0xFFE0;	// Jaune
const int Coul_TRAPOBJET      = 0x0FC0;	// Vert
const int Coul_SELECT         = 0xFC0D;	// Orange
const int Coul_OBJDESSUS      = 0xF800; // Rouge
const int Coul_SELECTPOINT    = 0xF00D; // Rouge p�le
const int Coul_WHITE          = 0xFFFD; // Blanc

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
           {  // Rmq : pas oblig� car d�j� ds le constructeur de Case 
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
   for (int i=0; i < MapWidth; i++) delete[] Carte[i];	    // lignes
   delete[] Carte;					    // colonnes
   Carte = NULL;
};
//----------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////////////////
// Objet TVIEWSCREEN														  //
////////////////////////////////////////////////////////////////////////////////

//== Constructeur ============================================================//
TViewMap::TViewMap()
{  // met tout � z�ro
   MapPixWidth = MapPixHeight = 0;
   NbTrapPerScreenWidth = NbTrapPerScreenHeight = 0;
   ScreenX = ScreenY = 0;
   dx = dy = 0;
   VIEWTRAPPRINC = TRUE;
   ObjPeindre = FALSE;
   ObjZoneSelect = FALSE;

   ScreenViewX = SCREEN_WIDTH;   
   ScreenViewY = SCREEN_HEIGHT;
};
//----------------------------------------------------------------------------//

//== Calcule certaines donn�es de la classe � partir des infos de la map =====//
void TViewMap::CalcInfos(int MapTrapWidth, int MapTrapHeight)
{
   // Nombre de pixels du rectangle contenant la carte
	MapPixWidth  = ISO_WIDTH_HALF  * ( MapTrapWidth + MapTrapHeight)   + 1;
    MapPixHeight = ISO_HEIGHT_HALF * ( MapTrapWidth + MapTrapHeight)   + 1;
   // Nombre de trap�ze minimal par vue d'�cran
   NbTrapPerScreenWidth  = ceil ( (float) ScreenViewX / (float) ISO_WIDTH  );
   NbTrapPerScreenHeight = ceil ( (float) ScreenViewY / (float) ISO_HEIGHT );
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
};
//----------------------------------------------------------------------------//

//== Changement de rep�re : ISO -> Cart�sien =================================//
void TViewMap::TransUVtoXY(int U, int V, int *X, int *Y)
{   // Coordonn�es de l'angle sup�rieur du trap�ze
	*X = ( U - V ) * ISO_WIDTH_HALF;
    *Y = ( U + V ) * ISO_HEIGHT_HALF;
};
//----------------------------------------------------------------------------//

//== Changement de rep�re : Cart�sien -> ISO ================================//
void TViewMap::TransXYtoUV(int X, int Y, int *U, int*V)
{
	float rx = X* ISO_HEIGHT_HALF;
    float ry = Y * ISO_WIDTH_HALF;
    *U = floor( ( rx + ry ) / ISO_DENOMINATEUR );
    *V = floor( ( ry - rx ) / ISO_DENOMINATEUR );
};
//----------------------------------------------------------------------------//

//== Affichage optimis�e de la grille � l'�cran ==============================//
void TViewMap::AfficheFastMap()
{
  // Affichage des 2 layers
	if (MapNiveau==NULL) return;
    TEtage *Etage = MapNiveau->PrincEtage;

    TMap *pMap;
    int DrawEtape, DrawEtapeEnd;  // D�but et fin de parcours des Layers
    BOOL BlackTrapeze = TRUE;	  // indique si c'est la 1er layer ds lequel on passe
	if (LibOutils->FondShow) DrawEtape = 0;
    else  DrawEtape = 1;
    if (LibOutils->DecorsShow) DrawEtapeEnd = 2;
    else DrawEtapeEnd = 1;

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
       //    int MoinsU=0, MoinsV=0;
           int DebU = ScreenU-5;
           int DebV = ScreenV-5;
       /*	if (DebU<0) { int MoinsV = -DebU; DebU =0; };
           if (DebV<0) { int moinsV = -DebV; DebV =0; };*/
           int incU, decV;
           BOOL IncremV = TRUE;
           for ( int cV = 0 ; cV < (NbTrapPerScreenHeight+5)*2; cV++)
             {
                incU = DebU;  decV = DebV;
                for ( int cU = 0 ; cU < (NbTrapPerScreenWidth+5)*2  ; cU++)
                  {
                   if ((incU>=0) && (incU < Etage->EtageWidth) && (decV>=0) && (decV < Etage->EtageHeight))
                      {

                        // Si il n'y a rien sur la carte : dessine les trap�ze lors du 1er passage ds un layer
                        if (pMap->Carte[incU][decV].ptrObjet==NULL)
                          { if (BlackTrapeze)
                             DrawViewTrap(incU,decV,0,TrapTYPE_SIMPLE); // trap�ze vide
                          }
                        else  // Si il y a quelque-chose
                         { /*DecorsObjet *dec = pMap->Carte[incU][decV].ptrObjet;
                           if (dec->DecTileObj->DecorsNum == 8875)
                           {	AfficheObjDecors(dec ,incU, decV);
                           }*/

                          if (pMap->Carte[incU][decV].isPrincipal())  // si c'est un triangle principal
                           {
                             if (VIEWTRAPPRINC) DrawViewTrap(incU,decV,Coul_TRAPOBJET,TrapTYPE_SIMPLE); // trap�ze principal

                             AfficheObjDecors(pMap->Carte[incU][decV].ptrObjet ,incU, decV);

                             // Si l'objet a �t� s�lectionn�
                             if ( (ObjDecorsSelect == TRUE) &&
                                  (pMap->Carte[incU][decV].ptrObjet->NumObjet == DOSelect->NumObjet))
                                {
                                 // Affiche tous les trap�zes lui appartenant
                                 for (int i = DOSelect->DecTileObj->NbTrapeze -1 ; i>=0 ; i--)  // ordre d�croissant
                                 {  if (!DOSelect->DecorsAnime)
                                    {  DrawViewTrap(DOSelect->PosU + DOSelect->DecTileObj->TrapezTab[i].TrapU, DOSelect->PosV + DOSelect->DecTileObj->TrapezTab[i].TrapV, Coul_TRAPSECONDAIRE, TrapTYPE_DOUBLE);
                                    } else
                                    {  DrawViewTrap(DOSelect->PosU + DOSelect->SecondTile->TrapezTab[i].TrapU, DOSelect->PosV + DOSelect->SecondTile->TrapezTab[i].TrapV, Coul_TRAPSECONDAIRE, TrapTYPE_DOUBLE);
                                    }
                                 }
                                };
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
     BlackTrapeze = FALSE;
    };

   //*** Affichages annexes ***
   // 1) Relatifs au PATHFINDING
   // Affichage des points de d�part et d'arriv�e du pathfinder
   if (PathFinding->pfDep)  // Point de d�part
   {  DrawViewTrap(PathFinding->depU, PathFinding->depV , Coul_SELECTPOINT, TrapTYPE_DOUBLE);
      DrawViewText(PathFinding->depU, PathFinding->depV, Coul_SELECTPOINT,"Start");
   }
   if (PathFinding->pfArr)  // Point de d�part
   {  DrawViewTrap(PathFinding->arrU, PathFinding->arrV , Coul_SELECTPOINT, TrapTYPE_DOUBLE);
      DrawViewText(PathFinding->arrU, PathFinding->arrV, Coul_SELECTPOINT,"End");
   }
   // Affichage du chemin
   if (PathFinding->pfChemTrouve)
   { for(PathVector::iterator iter = PathFinding->CheminTrouve.begin() ; iter != PathFinding->CheminTrouve.end() ; iter++)
     {  DrawViewTrap((*iter).x, (*iter).y , Coul_SELECT, TrapTYPE_SIMPLE);
     }
   }

   // 2) Relatifs aux ZONES au Sol
   if (FormDesignOutils->SBtnNoZone->Down == false)
   {    MapZone* zone = NULL;
        MapZoneRect* rect = NULL;
        MapZoneCircle* circle = NULL;
        double X, Y, X2, Y2;
        int scrXY[4][2];

   		// Affichage de toutes les zones
   		if (FormDesignOutils->SBtnAllZone->Down)
        {	for (MapZone* zone = MapNiveau->pMapZoneList->GetFirstZone() ; zone != NULL ; )
            {   switch (zone->GetType())
                {	case (MAPZONE_TYPE_CIRCLE) :
				 		circle = (MapZoneCircle*) zone;
                       	circle->GetCenter(X, Y);
                        if (circle->GetRadius() > 0)
                        {  	DrawCircle(X, Y, circle->GetRadius(), 12);
                        }
                       	break;
       	            case (MAPZONE_TYPE_RECT) :
                        // R�cup�re les coordonn�es du rectangle sur la map
                        rect = (MapZoneRect*) zone;
                        rect->GetOrigine(X, Y);
                        X2 = rect->GetWidth() + X;
                        Y2 = rect->GetHeight() + Y;
                        if (rect->GetWidth() != 0)
                        {
                        	// Calcul du losange � afficher � l'�cran
                        	ConvNivXYtoPeXY( X,  Y, scrXY[0][0], scrXY[0][1]);
                        	ConvNivXYtoPeXY(X2,  Y, scrXY[1][0], scrXY[1][1]);
                        	ConvNivXYtoPeXY(X2, Y2, scrXY[2][0], scrXY[2][1]);
                        	ConvNivXYtoPeXY( X, Y2, scrXY[3][0], scrXY[3][1]);
                        	DrawRectangle(scrXY, 12);
                        }
                        break;
                }
                zone = MapNiveau->pMapZoneList->GetNextZone(zone->GetID());
            }
        }
		// Si une zone au sol est s�lectionn�e ou que l'on est en train de
        // tracer une zone
        if ((ObjZoneSelect) || (ObjTracerZone))
        {   switch (FormDesignOutils->pMapZone->GetType())
        	{	case (MAPZONE_TYPE_CIRCLE) :
				 	circle = (MapZoneCircle*) FormDesignOutils->pMapZone;
                   	circle->GetCenter(X, Y);
                    if (circle->GetRadius() > 0)
                    {	DrawCircle(X, Y, circle->GetRadius(), 0xF500);
                    }
                   	break;
	            case (MAPZONE_TYPE_RECT) :
                    // R�cup�re les coordonn�es du rectangle sur la map
				 	rect = (MapZoneRect*) FormDesignOutils->pMapZone;
                   	rect->GetOrigine(X, Y);
                    X2 = rect->GetWidth() + X;
                    Y2 = rect->GetHeight() + Y;
                    if (rect->GetWidth() != 0)
                    {   // Calcul du loasange � afficher � l'�cran
                        ConvNivXYtoPeXY( X,  Y, scrXY[0][0], scrXY[0][1]);
                        ConvNivXYtoPeXY(X2,  Y, scrXY[1][0], scrXY[1][1]);
                        ConvNivXYtoPeXY(X2, Y2, scrXY[2][0], scrXY[2][1]);
                        ConvNivXYtoPeXY( X, Y2, scrXY[3][0], scrXY[3][1]);
                     	DrawRectangle(scrXY, 0xF500);
                    }
                   	break;
            }
        }
   }
}
//----------------------------------------------------------------------------//

//== Calcule les param�tres de la souris d'apr�s (SourisX,SourisY) ===========//
void TViewMap::MouseInfos()
{
  // Calcul des coordonn�es de la Mouse
    MouseX = ScreenX + SourisX;
    MouseY = ScreenY + SourisY;
    MapOutils->SourisX->Text = MouseX;
    MapOutils->SourisY->Text = MouseY;
    TransXYtoUV(MouseX, MouseY, &MouseU, &MouseV);
    MapOutils->SourisU->Text = MouseU;
    MapOutils->SourisV->Text = MouseV;

    // Suivant le type de pointeurs, marque son emplacement
    if (MapNiveau!=NULL)
    if (   (MouseU >= 0) && (MouseV >= 0)	// si le curseur est sur la carte
    	&& (MouseU < MapNiveau->NivWidth) && (MouseV < MapNiveau->NivHeight) )
     {
   // Suivant le plan sur lequel on travaille
     TEtage *Etage = MapNiveau->PrincEtage;
     TMap *pMap;
      switch (LibOutils->TypeLibSelect)
      {
         case (Lib_DESIGN):
         case (Lib_DECORS): pMap = Etage->LayerDecors; break;
         case (Lib_SOLS)  : pMap = Etage->LayerSols;	break;
         default		  : pMap = Etage->LayerSols; 
      }
      if (pMap==NULL) return;
      switch (Pointeur)
      {
         case Pointeur_NOTHING :  // Pointeur se balladant sans but
           if (pMap->Carte[MouseU][MouseV].ptrObjet==NULL)  DrawViewTrap(MouseU, MouseV, Coul_POSITION, TrapTYPE_DOUBLE); // trap�ze vide
           break;
         case Pointeur_PEINDRESOLS  : // Sols � peindre
         case Pointeur_PLACERDECORS : // D�cors � placer
           {
           	  ObjDecorsPlacerDessus = FALSE;  // par d�faut : le pointeur n'est par dessus aucun objet
              // Recherche si l'objet � placer n'est pas au-dessus un autre objet d�j� plac�
              DecorsObjet *DecObj = DOSelect;
              for (int i=DecObj->DecTileObj->NbTrapeze -1 ; i>=0 ; i--)  // ordre d�croissant
                 { int tu = MouseU + DecObj->DecTileObj->TrapezTab[i].TrapU;
                   int tv = MouseV + DecObj->DecTileObj->TrapezTab[i].TrapV;
                   if ((tv>=0) && (tu>=0) && (tu<Etage->EtageWidth) && (tv<Etage->EtageHeight))
                     if (pMap->Carte[tu][tv].ptrObjet !=NULL) ObjDecorsPlacerDessus = TRUE;
                 };
           	  AfficheSelectDecors();
           };break;
         case Pointeur_GOMMESOLS :
         case Pointeur_SELECTDECORS : // S�lectionner un d�cors
         case Pointeur_SELECTOBJECT:  // S�lection d'un objet � associ� � une zone
           {
            ObjDecorsSelectionne = FALSE;
            if (pMap->Carte[MouseU][MouseV].ptrObjet==NULL) DrawViewTrap(MouseU, MouseV, Coul_SELECT, TrapTYPE_DOUBLE); // trap�ze vide
            else // �l�ment de d�cors
             {
               ObjDecorsSelectionne = TRUE;
               // Recherche de quel �l�ment c'est
               DecorsObjet *DecObj = pMap->Carte[MouseU][MouseV].ptrObjet;
               // Affiche tous les trap�zes lui appartenant
               for (int i=DecObj->DecTileObj->NbTrapeze -1 ; i>=0 ; i--)  // ordre d�croissant
               {  if (!DecObj->DecorsAnime)
                  {  DrawViewTrap(DecObj->PosU + DecObj->DecTileObj->TrapezTab[i].TrapU, DecObj->PosV + DecObj->DecTileObj->TrapezTab[i].TrapV, Coul_TRAPSECONDAIRE, TrapTYPE_DOUBLE);
                  } else
                  {  DrawViewTrap(DecObj->PosU + DecObj->SecondTile->TrapezTab[i].TrapU, DecObj->PosV + DecObj->SecondTile->TrapezTab[i].TrapV, Coul_TRAPSECONDAIRE, TrapTYPE_DOUBLE);
                  }
               }
             };
           }; break;
        case Pointeur_SELECTPOINT : // S�lectionner une case de la carte
           if ((pMap->Carte[MouseU][MouseV].ptrObjet==NULL) || (pMap->Carte[MouseU][MouseV].isAccessible()))
           {  // trap�ze vide ou trap�ze accessible
              DrawViewTrap(MouseU, MouseV, Coul_SELECTPOINT, TrapTYPE_DOUBLE);
              DrawViewText(MouseU, MouseV, Coul_SELECTPOINT,"D�part");
           } else
           {  // trap�ze inaccessible
              DrawViewTrap(MouseU, MouseV, Coul_WHITE, TrapTYPE_DOUBLE);
              DrawViewText(MouseU, MouseV, Coul_SELECTPOINT,"Arriv�e");
           }
           break;
		// En-train de dessiner une zone au sol
        case Pointeur_PLACERZONE :
            if (ObjTracerZone)
            {	  MapZoneCircle* circle = NULL;
                  MapZoneRect* rect = NULL;
                  double nivX, nivY, centX, centY, radius;
                  if (FormDesignOutils->pMapZone != NULL)	// pr�vention
                  switch (FormDesignOutils->pMapZone->GetType())
                  {  case (MAPZONE_TYPE_CIRCLE):
                        circle = (MapZoneCircle*) FormDesignOutils->pMapZone;
                        // Calcul du rayon
                        circle->GetCenter(centX, centY);
                        ConvPeXYtoNivXY(ViewMap->MouseX, ViewMap->MouseY, nivX, nivY);
                        radius = sqrt((centX - nivX)*(centX - nivX) + (centY - nivY)*(centY - nivY));
                        circle->SetRadius(radius);
                        FormDesignOutils->UpdateView();
                         // V�rifie si l'on ne peut pas enable le btn "Cr�er" zone
                        if (FormDesignOutils->CBTracerZone->Checked && FormDesignOutils->CBChoisirNumero->Checked)
                        {	  FormDesignOutils->BtnCreerZone->Enabled = true;
                        }
                        break;
                     case (MAPZONE_TYPE_RECT):
                     	rect = (MapZoneRect*) FormDesignOutils->pMapZone;
                        rect->GetOrigine(centX, centY);
		                ConvPeXYtoNivXY(ViewMap->MouseX, ViewMap->MouseY, nivX, nivY);
          			    rect->SetZone(centX, centY, nivX-centX, nivY-centY);
						FormDesignOutils->UpdateView();
                         // V�rifie si l'on ne peut pas enable le btn "Cr�er" zone
                        if (FormDesignOutils->CBTracerZone->Checked && FormDesignOutils->CBChoisirNumero->Checked)
                        {	  FormDesignOutils->BtnCreerZone->Enabled = true;
	                    }
                     	break;
                     default:
                        ViewMap->ObjTracerZone = FALSE;
                  }
            }
        	break;
       };
    };
};
//----------------------------------------------------------------------------//

//== Affiche un �l�ment du d�cors sur la back-surface ========================//
void TViewMap::AfficheSelectDecors()
{
    // Position du Trap�ze principal de l'objet s�lectionn� sur la case o� se tient la souris
    AfficheObjDecors(DOSelect, MouseU, MouseV);

    // Puis affiche les trap�zes
    for (int i=DOSelect->DecTileObj->NbTrapeze -1 ; i>=0 ; i--)  // ordre d�croissant
     {
       if (i==0) DrawViewTrap(MouseU, MouseV, Coul_TRAPPRINCIPAL, TrapTYPE_DOUBLE); // Trap. principal
       else		// Trap�zes secondaires.
         if (ObjDecorsPlacerDessus) // si Objet par dessus un autre => pr�vient par un cadrillage rouge
         DrawViewTrap(MouseU + DOSelect->DecTileObj->TrapezTab[i].TrapU, MouseV + DOSelect->DecTileObj->TrapezTab[i].TrapV, Coul_OBJDESSUS, TrapTYPE_DOUBLE);
         else // normal
         DrawViewTrap(MouseU + DOSelect->DecTileObj->TrapezTab[i].TrapU, MouseV + DOSelect->DecTileObj->TrapezTab[i].TrapV, Coul_TRAPSECONDAIRE, TrapTYPE_DOUBLE);
     };

};
//----------------------------------------------------------------------------//

//== Affiche l'�l�ment du d�cors sp�cifi� sur la grille en (U,V) =============//
void TViewMap::AfficheObjDecors(DecorsObjet *DecObj, int U, int V)
{
    // Position du Trap�ze principal de l'objet s�lectionn� sur la case o� se tient la souris
    int X, Y;
    TransUVtoXY(U, V, &X, &Y);
    X -= ScreenX;
    Y -= ScreenY;

    // 1) Dans le cas d'un objet FIXE ou d'un objet ANIME repr�sent� par un TileDecors
    //    => On affiche un TileDecors
    if ((!DecObj->DecorsAnime) || (DecObj->peShowTile))
    {  TileDecors* tile;
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
       SurfDD->DestRect.top = Y;
       SurfDD->DestRect.left = X;
       SurfDD->DestRect.bottom = Y + SurfDD->PixHeight;
       SurfDD->DestRect.right = X + SurfDD->PixWidth;


      // G�re le clipping
      if (SurfDD->DestRect.top < 0) { SurfDD->SrcRect.top -= SurfDD->DestRect.top; SurfDD->DestRect.top = 0; };
      if (SurfDD->DestRect.bottom >= ScreenViewY) { SurfDD->SrcRect.bottom = ScreenViewY - SurfDD->DestRect.top; SurfDD->DestRect.bottom = ScreenViewY; };
      if (SurfDD->DestRect.left < 0) { SurfDD->SrcRect.left -= SurfDD->DestRect.left; SurfDD->DestRect.left = 0; };
      if (SurfDD->DestRect.right >= ScreenViewX) { SurfDD->SrcRect.right += ScreenViewX - SurfDD->DestRect.right; SurfDD->DestRect.right = ScreenViewX; };

  /*    SurfDD->DestRect.top = 0;
      SurfDD->DestRect.bottom  = 50;
      SurfDD->DestRect.left = 0;
      SurfDD->DestRect.right = 50;

      SurfDD->SrcRect.top = 0;
      SurfDD->SrcRect.bottom  = 50;
      SurfDD->SrcRect.left = 0;
      SurfDD->SrcRect.right = 50;  */
      SurfDD->Draw(DS_VE->pDDSBack);

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
      TileDecors* tileAssoc = DULib->getTileDecors(DecObj->peTileDecorsAssoc);
      //  X -= tileAssoc->TrapPrincX; et  Y -= tileAssoc->TrapPrincY;
      X -= DecObj->Element->AnimInfos[anm].dX;
      Y -= DecObj->Element->AnimInfos[anm].dY;
      X -= Sprite->GravX;
      Y -= Sprite->GravY;

      // Clipping
      int ScX = X;
      int ScY = Y;
      int Height = Sprite->Height;
      int Width  = Sprite->Height;
      RECT rectg = Sprite->SrcRect;
      // a) Regarde si le sprite n'est pas en dehors de la fen�tre
      if ((X>=ScreenViewX) || (Y>=ScreenViewY)) return;
      if ((X+Height<0) || (Y+Width<0)) return;
      // b) R�ajuste le rectangle                                        // BIDOUILLE
      if (ScX+Width  >= ScreenViewX) { rectg.right-=  ScX+Width - ScreenViewX -4; } // d�borde � droite
      if (ScY+Height >= ScreenViewY) { rectg.bottom-= ScY+Height - ScreenViewY; } // d�borde en bas
      if (ScX<0) { rectg.left-=ScX; ScX = 0; }   // d�borde � gauche
      if (ScY<0) { rectg.top-=ScY; ScY = 0; }    // d�borde en haut
      // c) Affiche le sprite � l'�cran
      HRESULT hr;
      hr = DS_VE->pDDSBack->pDDSurf->BltFast(ScX,ScY,Sprite->pFrame->pDDSurf,&rectg,DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
    }

};
//----------------------------------------------------------------------------//

//=== Met � jour les infos de la carte =======================================//
// -> en particulier avec les objets anim�s du d�cors rafraichis
void TViewMap::Upgrade()
{  // 1) Upgrade les objets anim�s du d�cors ayant chang� de TileDecors associ�s
  for (int j=0; j < DOLib->DecorsUpgraded.size(); j++)
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
//      MessageBoxA(NULL,"DEBUG 1 : upgrade","",1);
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
      pMap->Carte[PosU][PosV].setAccessible(decPtr->SecondTile->TrapezTab[0].Accessible);
      pMap->Carte[PosU][PosV].TrapCollide = decPtr->SecondTile->TrapezTab[0].TrapCollide;
      // Trap�ze Secondaire
      for (int i=1;  i < decPtr->DecTileObj->NbTrapeze;  i++)
       {
         int dU = decPtr->SecondTile->TrapezTab[i].TrapU;
         int dV = decPtr->SecondTile->TrapezTab[i].TrapV;
         if ((PosU + dU>=0) && (PosU + dU<MapNiveau->NivWidth) && (PosV + dV>=0) && (PosV + dV < MapNiveau->NivHeight))
           if (pMap->Carte[PosU + dU][PosV + dV].isPrincipal() == FALSE)
            {pMap->Carte[PosU + dU][PosV + dV].resetInfos();  // init
             pMap->Carte[PosU + dU][PosV + dV].setPrincipal(false);
             pMap->Carte[PosU + dU][PosV + dV].ptrObjet = decPtr;
             pMap->Carte[PosU + dU][PosV + dV].setAccessible(decPtr->SecondTile->TrapezTab[i].Accessible);
             pMap->Carte[PosU + dU][PosV + dV].TrapCollide = decPtr->SecondTile->TrapezTab[i].TrapCollide;
            };
        };

  }
  DOLib->DecorsUpgraded.clear();   // Efface les objtets contenus de cette bo�te une fois celle-ci parcourue
}
//----------------------------------------------------------------------------//


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
};
//----------------------------------------------------------------------------//

//== Pr�pare la carte au chargement ==========================================//
void TMapNiveau::PrepareLevel(int PrincWidth, int PrincHeight)
{
	NivWidth  = PrincWidth;
    NivHeight = PrincHeight;
    NbEtage = 1;  // ??
    PrincEtage->AlloueCarte(NivWidth, NivHeight);
};
//----------------------------------------------------------------------------//
