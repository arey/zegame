////////////////////////////////////////////////////////////////////////////////
//  MapView :  fen�tre repr�sentant la vue du jeu							  //
//             ou tout sera visible
////////////////////////////////////////////////////////////////////////////////

// param�tres C++ Builder
#include <vcl\vcl.h>
#pragma hdrstop
#include "DDLibEditor.h"
#include "MapLib.h"
#include "ImDecors2.h"
#include "fObjOutils.h"
#include "fMapOutils.h"
#include "Editor.h"
#include "fLibOutils.h"
#include "fPathFinding.h"
#include "fDesignOutils.h"
#include "MapZone.h"
#include "WorldSimulation.h"
#include "GestFile.h"           // G�re les frames (.bmp) requis par le jeu
#include "DDLib.h"
#include "uMapView.h"

//-- Variables ---------------------------------------------------------------
#pragma resource "*.dfm"
TMapView *MapView;

int ScrollViewRate = 10;


BOOL ViewNormal = TRUE;
int ScreenViewX = 1024,           // fen�tre repr�sentant la vue de la carte
    ScreenViewY =  768;
int ScreenVisibleX = 640,
    ScreenVisibleY = 480;


// Affiche le trap�ze (U,V) � l'�cran
void DrawViewTrap(int U, int V, int Col, int Type)
{
    int dU = U - ViewMap->ScreenU;
    int dV = V - ViewMap->ScreenV;
    int X, Y;
    ViewMap->TransUVtoXY(dU, dV, &X, &Y);
    X-=ViewMap->dx; Y-=ViewMap->dy;
    DS_VE->BackISOTrapeze(X, Y, Col, Type);
}

void DrawViewText(int U, int V, int Col, LPCTSTR Texte)
{   int dU = U - ViewMap->ScreenU;
    int dV = V - ViewMap->ScreenV;
    int X, Y;
    ViewMap->TransUVtoXY(dU, dV, &X, &Y);
    X-=ViewMap->dx; Y-=ViewMap->dy;
    DS_VE->PutBackText(X, Y, Col, Texte);
}

void DrawCircle(double x, double y, double rayon, int Col)
{	int X, Y, R;
    ConvNivXYtoPeXY(x, y, X, Y);
	X -= ViewMap->ScreenX;
    Y -= ViewMap->ScreenY;
    R = rayon * WorldModel.PixPerMeter;
	DS_VE->BackCircle(X, Y, R, Col);
}

void DrawRectangle(int tab[4][2], int Col)
{	int dx = ViewMap->ScreenX;
	int dy = ViewMap->ScreenY;
	DS_VE->BackLine(tab[0][0]-dx, tab[0][1]-dy, tab[1][0]-dx, tab[1][1]-dy, Col);
	DS_VE->BackLine(tab[1][0]-dx, tab[1][1]-dy, tab[2][0]-dx, tab[2][1]-dy, Col);
    DS_VE->BackLine(tab[2][0]-dx, tab[2][1]-dy, tab[3][0]-dx, tab[3][1]-dy, Col);
    DS_VE->BackLine(tab[3][0]-dx, tab[3][1]-dy, tab[0][0]-dx, tab[0][1]-dy, Col);
}




//---------------------------------------------------------------------------
// Constructeur
__fastcall TMapView::TMapView(TComponent* Owner)
	: TForm(Owner)
{
    // Initialise l'�cran DirectDraw
    DS_VE = new TDirectScreen_Editor(Handle);
    DS_VE->InitDDraw(ScreenViewX, ScreenViewY);
    DS = DS_VE;

    // Initialise le TMapView
    ViewMap = new TViewMap;
    ScrollHor = ScrollVert = 0;
    MIDDLE_SCROLLING = FALSE;
    RefreshFreq = 33;				// fr�quence de rafra�chissement de la fen�tre
    TimerFlip->Interval = 1000 / RefreshFreq;

    //--- Charge les Bitmaps du jeu ---
    SFL = new SpriteFrameList();
    // VerifAllFiles();
    LoadFrames();
};
//---------------------------------------------------------------------------

//== Clic sur la grille ======================================================//
void __fastcall TMapView::FormClick(TObject *Sender)
{
    // S'occupe de l'�tage principal, pour l'instant; et apr�s de l'�tage suivant
    if (MapNiveau==NULL) return;
    TEtage *Etage = MapNiveau->PrincEtage;

    // 	Si le pointeur est en mode "Ajout d'�l�ments du d�cors"
    if ((ViewMap->Pointeur == Pointeur_PLACERDECORS) && (Etage!=NULL))
     if (    (ViewMap->MouseU >= 0) && (ViewMap->MouseV >= 0)	// si le curseur est sur la carte
          && (ViewMap->MouseU < Etage->EtageWidth) && (ViewMap->MouseV < Etage->EtageHeight) )
      {
       //  V�rifie si tous les trap�zes de l'objet appartiennent � la carte
       for (int i=1; i < DOSelect->DecTileObj->NbTrapeze;  i++)
        { int U = DOSelect->DecTileObj->TrapezTab[i].TrapU + ViewMap->MouseU;
          int V = DOSelect->DecTileObj->TrapezTab[i].TrapV + ViewMap->MouseV;
          if ((U<0) || (U >= Etage->EtageWidth) || (V<0) || (V >= Etage->EtageHeight) ) // plantage
             { MessageBoxA(NULL,"L'Objet n'est pas tout � fait positionn� sur la grille","Impossible de placer le d�cors !",0); return; };
        };

          TMap *pMap;
          switch (LibOutils->TypeLibSelect)
           {
            case (Lib_DECORS): pMap = Etage->LayerDecors; break;
            case (Lib_SOLS)  : pMap = Etage->LayerSols;	break;
           };
        // & v�rifie si l'objet ne chevauche pas d�j� un objet existant
        // Suivant le plan sur lequel on travaille
        if (ViewMap->ObjDecorsPlacerDessus)
         {
           	// Si c'est le trap�ze princpial : impossible � placer, sf si efface l'objet
        	if (pMap->Carte[ViewMap->MouseU][ViewMap->MouseV].isPrincipal())
               { MessageBoxA(NULL,"Rmq: vous devez supprimer l'objet auparavant","Impossible d'�craser l'objet",0); return; }
            else if (MessageBoxA(NULL,"Voulez-vous supperposer ces 2 objets ?","Attention !",2) == IDNO) return;
         };

        // On place l'objet-d�cors dans la liste
        DOLib->AddDecObj(DOSelect);
        // On remplit quelques nouvelles propri�t�s de l'objet
        DOSelect->PosU = ViewMap->MouseU;		// Positionnement sur la carte
        DOSelect->PosV = ViewMap->MouseV;
        DOSelect->ObjEtage = Etage_PRINCIPAL;	// Etage sur lequel il se situe
        switch (LibOutils->TypeLibSelect)		// Layer auquel il appartient
           {
            case (Lib_DECORS): DOSelect->ObjLayer = Layer_DECORS; break;
            case (Lib_SOLS)  : DOSelect->ObjLayer = Layer_SOLS;	break;
           };

        // on remplit la Map des indications de l'objet
          // Trap�ze Principal
          int PosU = DOSelect->PosU;
          int PosV = DOSelect->PosV;

          //*** Nouveau ***
          pMap->Carte[PosU][PosV].ptrObjet = DOSelect;
          pMap->Carte[PosU][PosV].resetInfos();
    	  pMap->Carte[PosU][PosV].setPrincipal(TRUE);
          pMap->Carte[PosU][PosV].setAccessible(DOSelect->DecTileObj->TrapezTab[0].Accessible);
          pMap->Carte[PosU][PosV].TrapCollide = DOSelect->DecTileObj->TrapezTab[0].TrapCollide;

          // Trap�ze Secondaire
          for (int i=1;  i < DOSelect->DecTileObj->NbTrapeze;  i++)
            {
              int dU = DOSelect->DecTileObj->TrapezTab[i].TrapU;
              int dV = DOSelect->DecTileObj->TrapezTab[i].TrapV;

            //*** Nouveau ***
              pMap->Carte[PosU + dU][PosV + dV].ptrObjet = DOSelect;
              pMap->Carte[PosU + dU][PosV + dV].resetInfos();
              pMap->Carte[PosU + dU][PosV + dV].setPrincipal(FALSE);
              pMap->Carte[PosU + dU][PosV + dV].setAccessible(DOSelect->DecTileObj->TrapezTab[i].Accessible);
              pMap->Carte[PosU + dU][PosV + dV].TrapCollide = DOSelect->DecTileObj->TrapezTab[i].TrapCollide;
            };

        // et on renouvelle le pointeur d'objet s�lectionn�
        DOSelect = new DecorsObjet;
     //   DOSelect->DecorsLibName = DULib->Nom;
        DOSelect->DecTileObj = TDSelect;
      };

   // Si le pointeur est en mode "S�lection d'un �l�ment" et s�lectionne qqchose
   if (((ViewMap->Pointeur == Pointeur_SELECTDECORS) || (ViewMap->Pointeur == Pointeur_GOMMESOLS))
         && (Etage!=NULL) && (ViewMap->ObjDecorsSelectionne))
      {
          // On a un nouvel DOSelect
          //*** Nouveau ***
          TMap *pMap;
          switch (LibOutils->TypeLibSelect)
           {
            case (Lib_DECORS): pMap = Etage->LayerDecors; break;
            case (Lib_SOLS)  : pMap = Etage->LayerSols;	break;
           };
          DOSelect = pMap->Carte[ViewMap->MouseU][ViewMap->MouseV].ptrObjet;
          // <=>DOSelect = DOLib->SearchObj(pMap->Carte[ViewMap->MouseU][ViewMap->MouseV].NumObjet);
          ObjOutils->UpgradeFicheObj();
          ViewMap->ObjDecorsSelect = TRUE;
          MapOutils->BitBtnSuppr->Enabled = TRUE;
          MapView->SupprimerObjet->Enabled = TRUE;
      };
   // Si le pointeur est en mode "S�lection d'un point sur la carte" pour le PathFinding
   if ((ViewMap->Pointeur == Pointeur_SELECTPOINT) && (Etage!=NULL))
   {  // Renvoie � la barre d'outils du PathFinding les coordonn�es de cette case
      if (  (Etage->LayerSols->Carte[ViewMap->MouseU][ViewMap->MouseV].isAccessible())
          &&(Etage->LayerDecors->Carte[ViewMap->MouseU][ViewMap->MouseV].isAccessible()) )
      { PathFinding->SendCaseCoord(ViewMap->MouseU, ViewMap->MouseV);


      } else
      { MessageBoxA(NULL,"Cette case est d�j� occup�e par un d�cors ...","Impossible s�lectionner cette case",0); return; };
   }

   // Si le pointeur est en mode "S�lection d'une Zone au Sol" 
   if ((ViewMap->Pointeur == Pointeur_SELECTZONE) && (Etage!=NULL))
   {	// R�cup�re le point sur lequel on a cliqu� -> conversion en coodonn�es map
        double nivX, nivY;
		ConvPeXYtoNivXY(ViewMap->MouseX, ViewMap->MouseY, nivX, nivY);
        MapZone* zone = NULL;
        // Regarde si l'on a d�j� une zone de s�lectionn�
        int numID = FormDesignOutils->iObjSelected;
        if (numID != -1)
        {	zone = MapNiveau->pMapZoneList->GetNextZoneOn(nivX, nivY, numID);
			if (zone == NULL)
            { // on r�essaye, si ca se trouve elle est au d�but de la liste
	          zone = MapNiveau->pMapZoneList->GetFirstZoneOn(nivX, nivY);
            }
        } else
        { // Sinon, on prends la 1�re venue si elle existe
          zone = MapNiveau->pMapZoneList->GetFirstZoneOn(nivX, nivY);
        }

        if (zone == NULL)
        {	// Zone d�selectionn�e
        	FormDesignOutils->iObjSelected = -1;
        } else
        {	// Zone s�lectionn�e
        	FormDesignOutils->iObjSelected = zone->GetID();
			FormDesignOutils->pMapZone = zone;
            ViewMap->ObjZoneSelect = TRUE;
        }
        FormDesignOutils->UpdateView();
   }

	 // Si le pointeur est en mode "S�lection d'un �l�ment � associer � une zone" et s�lectionne qqchose
     if ((ViewMap->Pointeur == Pointeur_SELECTOBJECT) && (Etage!=NULL) && (ViewMap->ObjDecorsSelectionne))
     {    ViewMap->Pointeur = Pointeur_NOTHING;
     	  TMap *pMap = Etage->LayerDecors;
          DecorsObjet* decors = pMap->Carte[ViewMap->MouseU][ViewMap->MouseV].ptrObjet;
          // Ajoute cet objet � la zone
       	  FormDesignOutils->AddObjectToZone(decors);
     }

}
//---------------------------------------------------------------------------

//== Barre de d�filement Horizontale de l'�cran activ�e ======================//
void __fastcall TMapView::ScrollViewHorScroll(TObject *Sender,
	TScrollCode ScrollCode, int &ScrollPos)
{
	ViewMap->ScreenX = ScrollPos;
}
//---------------------------------------------------------------------------

//== Barre de d�filement Vertiacale de l'�cran activ�e =======================//
void __fastcall TMapView::ScrollViewVertScroll(TObject *Sender,
	TScrollCode ScrollCode, int &ScrollPos)
{
	ViewMap->ScreenY = ScrollPos;
}
//---------------------------------------------------------------------------

//== Affiche l'�cran =========================================================//
void __fastcall TMapView::UpGradeScreen()
{
    ScrollViewHor->Position += ScrollHor;
    ViewMap->ScreenX = ScrollViewHor->Position;
    ScrollViewVert->Position += ScrollVert;
    ViewMap->ScreenY = ScrollViewVert->Position;
};
//---------------------------------------------------------------------------

//== Affiche l'�cran =========================================================//
void __fastcall TMapView::TimerFlipTimer(TObject *Sender)
{
    DS_VE->Fill(65535); 	     // Efface le fond
    UpGradeScreen();		     // Apporte quelques modifs

    if (DOLib!=NULL)
    {   DOLib->Update();                 // S'occupe de mettre � jour tous les objets anim�s
        ViewMap->Upgrade();              //        "                  la carte du niveau
        ObjOutils->Update();             // Met � jour la bo�te d'outils

        ViewMap->AfficheFastMap();	     // Affiche la grille
        ViewMap->MouseInfos();	     // Calcule les nouveaux param�tres de la souris
    }
    DS_VE->FlipSurfaces();           // Basculement des surfaces
}
//---------------------------------------------------------------------------

//== S'occupe de r�pertorier les actions de la souris en mvt =================//
void __fastcall TMapView::FormMouseMove(TObject *Sender, TShiftState Shift,
	int X, int Y)
{
   // Nouvelles coordonn�es de la souris
   ViewMap->SourisX = X;
   ViewMap->SourisY = Y;


   if (MIDDLE_SCROLLING)		// scrolling avec le btn du milieu
   { 	ScrollHor  = (X - LastMouseViewX) * 3;
      	ScrollVert = (Y - LastMouseViewY) * 3;
   	LastMouseViewX = X;
        LastMouseViewY = Y;
   }
   else
    {
     //  Fait d�filer l'�cran lorsque le pointeur de la souris se trouve
     //  sur les bords de la fiche.
       if (X > ScreenVisibleX-10) ScrollHor = ScrollViewRate;
       else if (X < 10) ScrollHor = -ScrollViewRate;
       else ScrollHor = 0;
       if ((Y > ScreenVisibleY-10) && ( Y<ScreenVisibleY)) ScrollVert = ScrollViewRate;
       else if (Y < 10) ScrollVert = -ScrollViewRate;
       else ScrollVert = 0;
     // Changement de forme de curseur
       if (    ((ScrollHor>0) && (ScrollVert>0))	// Nord-Ouest <=>  Sud-Est
            || ((ScrollHor<0) && (ScrollVert<0))  ) Cursor = crSizeNWSE;
       else
       if (    ((ScrollHor>0) && (ScrollVert<0))	// Nord-Est  <=> Sud-Ouest
            || ((ScrollHor<0) && (ScrollVert>0))  ) Cursor = crSizeNESW;
       else
       if (ScrollHor!=0) Cursor =crSizeWE;         // Est <=> Ouest
       else
       if (ScrollVert!=0) Cursor =crSizeNS;		// Sud <=> Nord
       else Cursor = crDefault;
    };


    // Si le pointeur est en mode "Peinture" pour les sols
    if (MapNiveau==NULL) return;
    TEtage *Etage = MapNiveau->PrincEtage;  // Etage Principal, for the moment ...

     if ((ViewMap->Pointeur == Pointeur_PEINDRESOLS) && (ViewMap->ObjPeindre==TRUE) && (Etage!=NULL))
      if (    (ViewMap->MouseU >= 0) && (ViewMap->MouseV >= 0)	// si le curseur est sur la carte
          && (ViewMap->MouseU < Etage->EtageWidth) && (ViewMap->MouseV < Etage->EtageHeight) )
      {
        //  V�rifie si tous les trap�zes de l'objet appartiennent � la carte, sinon EXIT ...
        for (int i=1; i < DOSelect->DecTileObj->NbTrapeze;  i++)
         { int U = DOSelect->DecTileObj->TrapezTab[i].TrapU + ViewMap->MouseU;
           int V = DOSelect->DecTileObj->TrapezTab[i].TrapV + ViewMap->MouseV;
           if ((U<0) || (U >= Etage->EtageWidth) || (V<0) || (V >= Etage->EtageHeight) ) return;
         };

        TMap *pMap;
        switch (LibOutils->TypeLibSelect)
         {
          case (Lib_DECORS): pMap = Etage->LayerDecors; break;
          case (Lib_SOLS)  : pMap = Etage->LayerSols;	break;
         };

        // & v�rifie si l'objet ne chevauche pas d�j� un objet existant
		// => EXIT ds ce cas l�
        if (ViewMap->ObjDecorsPlacerDessus) return;

        // On place l'objet-d�cors dans la liste
        DOLib->AddDecObj(DOSelect);
        // On remplit quelques nouvelles propri�t�s de l'objet
        DOSelect->PosU = ViewMap->MouseU;		// Positionnement sur la carte
        DOSelect->PosV = ViewMap->MouseV;
        DOSelect->ObjEtage = Etage_PRINCIPAL;
        switch (LibOutils->TypeLibSelect)
           {
            case (Lib_DECORS): DOSelect->ObjLayer = Layer_DECORS; break;
            case (Lib_SOLS)  : DOSelect->ObjLayer = Layer_SOLS;	break;
           };

        // on remplit la Map des indications de l'objet
        // Trap�ze Principal
        int PosU = DOSelect->PosU;
        int PosV = DOSelect->PosV;

        //*** Nouveau ***
        pMap->Carte[PosU][PosV].ptrObjet = DOSelect;
	pMap->Carte[PosU][PosV].resetInfos();
    	pMap->Carte[PosU][PosV].setPrincipal(TRUE);
        pMap->Carte[PosU][PosV].setAccessible(DOSelect->DecTileObj->TrapezTab[0].Accessible);
        pMap->Carte[PosU][PosV].TrapCollide = DOSelect->DecTileObj->TrapezTab[0].TrapCollide;


        // Trap�ze Secondaire
        for (int i=1;  i < DOSelect->DecTileObj->NbTrapeze;  i++)
        {
            int dU = DOSelect->DecTileObj->TrapezTab[i].TrapU;
            int dV = DOSelect->DecTileObj->TrapezTab[i].TrapV;

            //*** Nouveau ***
            pMap->Carte[PosU + dU][PosV + dV].ptrObjet = DOSelect;
            pMap->Carte[PosU + dU][PosV + dV].resetInfos();
            pMap->Carte[PosU + dU][PosV + dV].setPrincipal(FALSE);
            pMap->Carte[PosU + dU][PosV + dV].setAccessible(DOSelect->DecTileObj->TrapezTab[0].Accessible);
            pMap->Carte[PosU + dU][PosV + dV].TrapCollide = DOSelect->DecTileObj->TrapezTab[0].TrapCollide;
        };
        // et on renouvelle le pointeur d'objet s�lectionn�
        DOSelect = new DecorsObjet;
    //    DOSelect->DecorsLibName = DULib->Nom;
        DOSelect->DecTileObj = TDSelect;
      };

   // Si le pointeur est en mode GOMMAGE et s�lectionne qqchose
   // BUG : whyyyyyyyyyy ????
   if ((ViewMap->Pointeur == Pointeur_GOMMESOLS) && (ViewMap->ObjGomme) && (ViewMap->ObjDecorsSelectionne) && (Etage!=NULL))
    {
          MapView->FormClick(Sender);
     //   MapOutils->BitBtnSupprClick(Sender);
    };
}
//---------------------------------------------------------------------------

//== PopUpMenu ===============================================================//
//== Poser Objet =============================================================//
void __fastcall TMapView::PoserObjetClick(TObject *Sender)
{  MapOutils->PlacerObjBitBtnClick(Sender); }
//== Poser Objet =============================================================//
void __fastcall TMapView::SlectionnerObjetClick(TObject *Sender)
{ MapOutils->BitBtnSelectClick(Sender); }
//== Poser Objet =============================================================//
void __fastcall TMapView::SupprimerObjetClick(TObject *Sender)
{ MapOutils->BitBtnSupprClick(Sender); }
//----------------------------------------------------------------------------//

//== Change de r�solution ====================================================//
void __fastcall TMapView::ChangeViewScreen()
{
  if (ViewNormal)
   {
    MapView->Height+=176;     MapView->Width +=350;
    ScrollViewHor->Top+=176;  ScrollViewVert->Left+=350;
    ScreenVisibleY+=176;      ScreenVisibleX+=350;
    Editeur->VueCarte->Caption = "Ch'tite Fen�tre";
    ScrollViewVert->Height += 176;  ScrollViewHor->Width += 350;
   }
  else
   {
    MapView->Height-=176;     MapView->Width -=350;
    ScrollViewHor->Top-=176;  ScrollViewVert->Left-=350;
    ScreenVisibleY-=176;      ScreenVisibleX-=350;
    Editeur->VueCarte->Caption = "Grand Ecran";
    ScrollViewVert->Height -= 176;  ScrollViewHor->Width -= 350;
   };
  ViewNormal = !ViewNormal;
};
//----------------------------------------------------------------------------//

//== Change de r�solution ====================================================//
void __fastcall TMapView::FormDblClick(TObject *Sender)
{	ChangeViewScreen();
}
//---------------------------------------------------------------------------

//== Scrolling : DEBUT du d�placement sur la carte ===========================//
void __fastcall TMapView::FormMouseDown(TObject *Sender, TMouseButton Button,
	TShiftState Shift, int X, int Y)
{
    // Scrolling avec btn central
	if (Button==mbMiddle)
    {    MIDDLE_SCROLLING = TRUE;
         LastMouseViewX = X;
         LastMouseViewY = Y;
         Cursor = crCross;
    }

    // Mode Peinture de sols
    if (MapNiveau==NULL) return;
    TEtage *Etage = MapNiveau->PrincEtage;
    if ((ViewMap->Pointeur == Pointeur_PEINDRESOLS) && (Etage!=NULL)) ViewMap->ObjPeindre = TRUE;
    if ((ViewMap->Pointeur == Pointeur_GOMMESOLS)   && (Etage!=NULL)) ViewMap->ObjGomme = TRUE;

    // Dessine moi une Zone au sol
    if (Button==mbLeft)
    if ((ViewMap->Pointeur == Pointeur_PLACERZONE) && (Etage!=NULL))
    {  // si le curseur est sur la carte
       if (    (ViewMap->MouseU >= 0) && (ViewMap->MouseV >= 0)
             && (ViewMap->MouseU < Etage->EtageWidth) && (ViewMap->MouseV < Etage->EtageHeight) )
       {    MapZoneCircle* circle = NULL;
	        MapZoneRect* rect = NULL;
       		double nivX, nivY;

       		switch (FormDesignOutils->pMapZone->GetType())
            {	case (MAPZONE_TYPE_CIRCLE):
                    circle = (MapZoneCircle*) FormDesignOutils->pMapZone;
                    ConvPeXYtoNivXY(ViewMap->MouseX, ViewMap->MouseY, nivX, nivY);
                    circle->SetCenter(nivX, nivY);
                    //ViewMap->ObjZoneSelect = TRUE;
               		ViewMap->ObjTracerZone = TRUE;
            		break;
				case (MAPZONE_TYPE_RECT):
                    rect = (MapZoneRect*) FormDesignOutils->pMapZone;
                    ConvPeXYtoNivXY(ViewMap->MouseX, ViewMap->MouseY, nivX, nivY);
                    rect->SetZone(nivX, nivY, 0 , 0);
                    //ViewMap->ObjZoneSelect = TRUE;
		       		ViewMap->ObjTracerZone = TRUE;
            		break;
                default:
                	ViewMap->ObjTracerZone = FALSE;
                    ViewMap->ObjZoneSelect = FALSE;
            }
       }
    }
}
//---------------------------------------------------------------------------

//== Scrolling : FIN du d�placement sur la carte =============================//
void __fastcall TMapView::FormMouseUp(TObject *Sender, TMouseButton Button,
	TShiftState Shift, int X, int Y)
{
    // fin du scrolling
	if (Button==mbMiddle)
      {
         MIDDLE_SCROLLING = FALSE;
         Cursor = crDefault;
      };

    // fin de la peinture
    ViewMap->ObjPeindre = FALSE;
    ViewMap->ObjGomme   = FALSE;

    // Fin de dessin d'une Zone
    if (Button==mbLeft)
	if ((ViewMap->Pointeur == Pointeur_PLACERZONE) && (ViewMap->ObjTracerZone))
    {   MapZoneCircle* circle = NULL;
    	MapZoneRect* rect = NULL;
   		double nivX, nivY, centX, centY, radius;

    	switch (FormDesignOutils->pMapZone->GetType())
	    {  case (MAPZONE_TYPE_CIRCLE):
              circle = (MapZoneCircle*) FormDesignOutils->pMapZone;
              // Calcul du rayon
              circle->GetCenter(centX, centY);
              ConvPeXYtoNivXY(ViewMap->MouseX, ViewMap->MouseY, nivX, nivY);
              radius = sqrt((centX - nivX)*(centX - nivX) + (centY - nivY)*(centY - nivY));
              circle->SetRadius(radius);
              FormDesignOutils->UpdateView();
              FormDesignOutils->CBTracerZone->Checked = true;
              FormDesignOutils->StatusBarLevelDesign->SimpleText = "Zone au sol tra��e";
               // V�rifie si l'on ne peut pas enable le btn "Cr�er" zone
		      if (FormDesignOutils->CBTracerZone->Checked && FormDesignOutils->CBChoisirNumero->Checked)
        	  {	  FormDesignOutils->BtnCreerZone->Enabled = true;
	          }
              ViewMap->ObjTracerZone = FALSE;
              ViewMap->ObjZoneSelect = TRUE;
              break;
        	case (MAPZONE_TYPE_RECT):
              rect = (MapZoneRect*) FormDesignOutils->pMapZone;
              // Calcul du pt de destination
              rect->GetOrigine(centX, centY);
              ConvPeXYtoNivXY(ViewMap->MouseX, ViewMap->MouseY, nivX, nivY);
              rect->SetZone(centX, centY, nivX-centX, nivY-centY);
              FormDesignOutils->UpdateView();
              FormDesignOutils->CBTracerZone->Checked = true;
              FormDesignOutils->StatusBarLevelDesign->SimpleText = "Zone au sol tra��e";
               // V�rifie si l'on ne peut pas enable le btn "Cr�er" zone
		      if (FormDesignOutils->CBTracerZone->Checked && FormDesignOutils->CBChoisirNumero->Checked)
        	  {	  FormDesignOutils->BtnCreerZone->Enabled = true;
	          }
              ViewMap->ObjTracerZone = FALSE;
              ViewMap->ObjZoneSelect = TRUE;
              break;
            default:
              ViewMap->ObjTracerZone = FALSE;
      	}
    }
}
//---------------------------------------------------------------------------

//--- Outils de convertion des divers coordonn�es du perso -------------------//
//--- et fonctions supl�mentaires de convertion ------------------------------//
//=== Convertion des PersoNiv(X,Y) en PersoNiv(U,V) ==========================//
void ConvNivXY2UV(double X, double Y, double &U, double &V)
{	 U = X / WorldModel.SQR_LENGTH;
     V = Y / WorldModel.SQR_LENGTH;
}
//----------------------------------------------------------------------------//

//=== Convertion des PersoNiv(U,V) en PersoNiv(X,Y) ==========================//
void ConvNivUV2XY(double U, double V, double &X, double &Y)
{	 X = U * WorldModel.SQR_LENGTH;
     Y = V * WorldModel.SQR_LENGTH;
}
//----------------------------------------------------------------------------//

//=== Convertion des PersoNiv(X,Y) en pe(X,Y)=================================//
void ConvNivUVtoPeXY(double U, double V, int &pex, int &pey)
{	 pex = (int) (( U - V ) * ISO_WIDTH_HALF);
     pey = (int) (( U + V ) * ISO_HEIGHT_HALF);
}
//----------------------------------------------------------------------------//

//=== Convertion de pe(X,Y) en PersoNiv(U,V) =================================//
void ConvNivPeXYtoUV(int pex, int pey, double &U, double &V)
{	 double rx = pex* ISO_HEIGHT_HALF;
     double ry = pey * ISO_WIDTH_HALF;
     U = ( rx + ry ) / ISO_DENOMINATEUR;
     V = ( ry - rx ) / ISO_DENOMINATEUR;
}
//----------------------------------------------------------------------------//

//=== Convertion de pe(X,Y) en PersoNiv(X,Y) =================================//
//--- Convertions Totales ----------------------------------------------------//
//=== Convertion de PersoNiv(X,Y) en PersoNiv(U,V) et pe(X,Y) ================//
void ConvNivXYtoAll(double X, double Y, double &U, double &V, int &pex, int &pey)
{	ConvNivXY2UV(X, Y, U, V);
	ConvNivUVtoPeXY(U, V, pex, pey);
}
//=== Convertion de PersoNiv(U,V) en PersoNiv(X,Y) et pe(X,Y) ================//
void ConvNivUVtoAll(double U, double V, double &X, double &Y, int &pex, int &pey)
{	ConvNivUV2XY(U, V, X, Y);
	ConvNivUVtoPeXY(U, V, pex, pey);
}
//=== Convertion de pe (X,Y) en PersoNiv(U,V) et PersoNiv(X,Y) ===============//
void ConvNivUVtoAll(int pex, int pey, double &U, double &V, double &X, double &Y)
{	ConvNivPeXYtoUV(pex, pey, U, V);
	ConvNivUV2XY(U, V, X, Y);
}
//----------------------------------------------------------------------------//

// -> Implant�es sp�cialement depuis les Zones au sol
void ConvPeXYtoNivXY(int pex, int pey, double &x, double &y)
{   double u, v;
	ConvNivPeXYtoUV(pex, pey, u, v);
	ConvNivUV2XY(u, v, x, y);
}
void ConvNivXYtoPeXY(double x, double y, int &pex, int &pey)
{	double u, v;
	ConvNivXY2UV(x, y, u, v);
    ConvNivUVtoPeXY(u, v, pex, pey);
}

