//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "uMapView.h"
#include "fMapOutils.h"
#include "MapLib.h"
#include "consts.h"
#include "fObjOutils.h"
#include "editor.h"
#include "fLibOutils.h"

//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TMapOutils *MapOutils;
//---------------------------------------------------------------------------
__fastcall TMapOutils::TMapOutils(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

//== Taille & Positions de la fiche à sa création ============================//
void __fastcall TMapOutils::FormCreate(TObject *Sender)
{
     Left = 0;
     Top = 515;
     Width = 664;
     Height = 176;
}
//---------------------------------------------------------------------------

//== Place un objet sélectionné sur la grille ================================//
void __fastcall TMapOutils::PlacerObjBitBtnClick(TObject *Sender)
{
    ViewMap->ObjDecorsSelect = FALSE;
    if (  (ViewMap->Pointeur == Pointeur_SELECTDECORS)   // Prend le décors de la Lib et non celui sélectionné
    	 || (ViewMap->Pointeur == Pointeur_GOMMESOLS)  )
      {
         // Nouvel Objet sélectionné
         DOSelect = new DecorsObjet;
      //   DOSelect->DecorsLibName = DTLib->Nom;
         DOSelect->DecTileObj = TDSelect;
         ObjOutils->UpgradeFicheObj();
        };
    if (TDSelect!=NULL) ViewMap->Pointeur = Pointeur_PLACERDECORS;
    else { MessageBoxA(NULL,"Aucun Objet n'est sélectionné ...","Impossible de poser un Objet !",0); return; };
}
//----------------------------------------------------------------------------//

//== Sélectionne un décors ===================================================//
void __fastcall TMapOutils::BitBtnSelectClick(TObject *Sender)
{   ViewMap->ObjDecorsSelect = FALSE;
	if ((LibOutils->SpeedButtonDecors->Down) || (LibOutils->SpeedButtonFonds->Down))
    {	// Sélection d'un décors
	    ViewMap->Pointeur = Pointeur_SELECTDECORS;
    } else
    if (LibOutils->SpeedButtonDesign->Down)
    {	// Sélection d'une Zone au sol
    	ViewMap->Pointeur = Pointeur_SELECTZONE;
    }
}
//----------------------------------------------------------------------------//

//== Supprime l'élément sélectionné ==========================================//
void __fastcall TMapOutils::BitBtnSupprClick(TObject *Sender)
{
    if (MapNiveau==NULL) return;
    TEtage *Etage = MapNiveau->PrincEtage;

	if (ViewMap->ObjDecorsSelect == TRUE)
     {
         // Supprime l'objet de la carte
         int PosU = DOSelect->PosU;
         int PosV = DOSelect->PosV;
         // Trapèze Secondaire
         for (int i=0;  i < DOSelect->DecTileObj->NbTrapeze;  i++)
           {
             int dU = DOSelect->DecTileObj->TrapezTab[i].TrapU;
             int dV = DOSelect->DecTileObj->TrapezTab[i].TrapV;
             //*** Nouveau ***
             TMap *pMap;
             switch (LibOutils->TypeLibSelect)
              {
               case (Lib_DECORS): pMap = Etage->LayerDecors; break;
               case (Lib_SOLS)  : pMap = Etage->LayerSols;	break;
              };
             pMap->Carte[PosU + dU][PosV + dV].ptrObjet = NULL;
             pMap->Carte[PosU + dU][PosV + dV].resetInfos();
             pMap->Carte[PosU + dU][PosV + dV].setPrincipal(FALSE);
           };

         // Supprime l'objet de la liste d'objets
         DOLib->SupprDecObj(DOSelect);

         // Nouvel Objet sélectionné
         DOSelect = new DecorsObjet;
     //    DOSelect->DecorsLibName = DULib->Nom;
         DOSelect->DecTileObj = TDSelect;
         ObjOutils->UpgradeFicheObj();
         ViewMap->ObjDecorsSelect = FALSE;
         MapOutils->BitBtnSuppr->Enabled = FALSE;
         MapView->SupprimerObjet->Enabled = FALSE;
     };
}
//---------------------------------------------------------------------------

//== Lorsque la souris passe dessus, désactive qq trucs ======================//
void __fastcall TMapOutils::GroupBoxDonnesMouseMove(TObject *Sender,
	TShiftState Shift, int X, int Y)
{  // Regarde si le pointer sort de la fenêtre de représentation du jeu
   if  ( (MapView->ScrollHor!=0) || (MapView->ScrollVert!=0))
     {  MapView->ScrollHor = MapView->ScrollVert = 0; };
}
//---------------------------------------------------------------------------

//== Lorsque la souris passe dessus, désactive qq trucs ======================//
void __fastcall TMapOutils::GroupBoxBoutonsMouseMove(TObject *Sender,
	TShiftState Shift, int X, int Y)
{  // Regarde si le pointer sort de la fenêtre de représentation du jeu
   if  ( (MapView->ScrollHor!=0) || (MapView->ScrollVert!=0))
     {  MapView->ScrollHor = MapView->ScrollVert = 0; };
}
//---------------------------------------------------------------------------

//== Aggrandie la fenêtre visible de l'écran =================================////----------------------------------------------------------------------------//
void __fastcall TMapOutils::BitBtnBigScreenClick(TObject *Sender)
{
  MapView->Show();
  MapView->ChangeViewScreen();
}
//---------------------------------------------------------------------------

//== Ajuste les paramètres de la fiche : couleurs, polices ... ===============//
void __fastcall TMapOutils::FormShow(TObject *Sender)
{
  	MapName->Color   = ConfigBoxColor;
    MapNumber->Color = ConfigBoxColor;
    MapWidth->Color  = ConfigBoxColor;
    MapHeight->Color = ConfigBoxColor;
}
//----------------------------------------------------------------------------//

//== Ajoute/Supprime l'affichage des Trapèzes principaux sur la carte ========//
void __fastcall TMapOutils::BitBtnTrapPrincClick(TObject *Sender)
{
	ViewMap->VIEWTRAPPRINC = !ViewMap->VIEWTRAPPRINC;
}
//----------------------------------------------------------------------------//

//== Bouton Peindre : pour positionner + facilement les sprites du sols ======//
void __fastcall TMapOutils::PeindreBitBtnClick(TObject *Sender)
{
  ViewMap->ObjDecorsSelect = FALSE;
  if (   (ViewMap->Pointeur == Pointeur_SELECTDECORS)   // Prend le décors de la Lib et non celui sélectionné
       || (ViewMap->Pointeur == Pointeur_GOMMESOLS)  )
      {
         // Nouvel Objet sélectionné
         DOSelect = new DecorsObjet;
     //    DOSelect->DecorsLibName = DTLib->Nom;
         DOSelect->DecTileObj = TDSelect;
         ObjOutils->UpgradeFicheObj();
        };
  if (TDSelect!=NULL) ViewMap->Pointeur = Pointeur_PEINDRESOLS;
    else { MessageBoxA(NULL,"Aucun Objet n'est sélectionné ...","Impossible de poser un Objet !",0); return; };
}
//----------------------------------------------------------------------------//

//== Bouton pour gommer le sol en un clin d'oeil =============================//
void __fastcall TMapOutils::GommeBitBtnClick(TObject *Sender)
{
  ViewMap->ObjDecorsSelect = FALSE;
  ViewMap->Pointeur = Pointeur_GOMMESOLS;
}
//----------------------------------------------------------------------------//
