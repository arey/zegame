//----------------------------------------------------------------------------//
// Rôle : Barre d'outils permettant de tester la recherche de chemin :        //
//        sélection d'un point de départ, d'arrivée et de divers paramètres   //
//----------------------------------------------------------------------------// 

#include <vcl\vcl.h>
#pragma hdrstop

#include "fPathFinding.h"            // son Header
#include "consts.h"                  // les constantes de l'appli
#include "MapLib.h"                  // Classe de gestion des maps
  

//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TPathFinding *PathFinding;

//---------------------------------------------------------------------------
__fastcall TPathFinding::TPathFinding(TComponent* Owner)
        : TForm(Owner)
{  pfChemTrouve = pfDep = pfArr = false;
}
//---------------------------------------------------------------------------

//=== Récupère le point de départ sur la grille ==============================//
void __fastcall TPathFinding::DepartBtnClick(TObject *Sender)
{ // Met le curseur de la carte en mode selection de point
  ViewMap->Pointeur = Pointeur_SELECTPOINT;
  PFPoint = PF_DEPART;
}
//----------------------------------------------------------------------------//

//=== Récupère un point sélectionné ==========================================//
void __fastcall TPathFinding::SendCaseCoord(int cU, int cV)
{  // Met à jours les coords et les infos de la barre d'outils
   switch (PFPoint)
   {  case (PF_DEPART) :
      {  depU = cU; depV = cV;
         depULabel->Caption = depU; depVLabel->Caption = depV;
         pfDep = true;
      } break;
      case (PF_ARRIVEE) :
      {  arrU = cU; arrV = cV;
         arrULabel->Caption = arrU; arrVLabel->Caption = arrV;
         pfArr = true;
      } break;
    }
    ViewMap->Pointeur = Pointeur_NOTHING;
    if ((pfArr) && (pfDep))
    { FindPathBtn->Enabled = true;  // Bouton de recherche activé
      // Calcule la longueur réelle du plus court chemin
      int dx = abs(arrU - depU);
      int dy = abs(arrV - depV);
      int dmin = min(dx,dy);
      int dmax = max(dx,dy);
      RealLongLabel->Caption = (dmin*14 + (dmax-dmin)*10)/10;
      // abs(arrU - depU) + abs(arrV - depV);
    }
}
//----------------------------------------------------------------------------//

//=== Récupère le point d'arrivée sur la grille ==============================//
void __fastcall TPathFinding::ArriveeBtnClick(TObject *Sender)
{ ViewMap->Pointeur = Pointeur_SELECTPOINT;
  PFPoint = PF_ARRIVEE;
}
//----------------------------------------------------------------------------//

//=== On initialise l'objet de PathFinding ===================================//
// -> On doit y faire appel à chaque fois que l'on charge une nouvelle carte
//    ou que l'on en crée une nouvelle     
void __fastcall TPathFinding::InitPF()
{
   TEtage *Etage = MapNiveau->PrincEtage;
   TMap *pMap = pMap = Etage->LayerDecors;
   // Pour le Layer du Sol, on aurait pu prendre pMap = Etage->LayerSols;
   MapInitRechercheChemin(*pMap);
}

//=== Lorsque cette fenêtre devient visible ==================================//
void __fastcall TPathFinding::FormShow(TObject *Sender)
{ // On initialise l'objet de PathFinding
  // Rmq : pour l'instant, on ne considère que l'étage principal d'un niveau
  if (MapNiveau!=NULL)  // Si un niveau a déjà été chargé
  {  InitPF();
  }
}
//----------------------------------------------------------------------------//

//=== Recherche du chemin le plus rapide =====================================//
void __fastcall TPathFinding::FindPathBtnClick(TObject *Sender)
{ CheminTrouve.clear();                                    // Réinitialise le chemin
  TMap *pMap = pMap = MapNiveau->PrincEtage->LayerDecors;  // Recherche du chemin le plus rapide
  PFStats = RechercheChemin(*pMap,GridCoord(depU,depV), GridCoord(arrU,arrV), CheminTrouve, !CBComplet->Checked);
  pfChemTrouve = true;   // Un chemin a ete trouve
  // Complète les infos s
  LongueurLabel->Caption  = PFStats.chem_long;
  CoutLabel->Caption      = PFStats.chem_cout;
  RechercheLabel->Caption = PFStats.nodes_recherches;
  AjouteLabel->Caption    = PFStats.nodes_ajoutes;
  VisiteLabel->Caption    = PFStats.nodes_visites;
}
//----------------------------------------------------------------------------//

//=== Lorsque le paramètre 'Chemin Complet' change ===========================//
void __fastcall TPathFinding::CBCompletClick(TObject *Sender)
{ // Si les 2 points ont été placés, on recalcule le chemin avec la nouvelle option
  // de linéarisation
  if (pfChemTrouve)
  { CheminTrouve.clear();                                    // Réinitialise le chemin
    TMap *pMap = pMap = MapNiveau->PrincEtage->LayerDecors;  // Recherche du chemin le plus rapide
    PFStats = RechercheChemin(*pMap,GridCoord(depU,depV), GridCoord(arrU,arrV), CheminTrouve, !CBComplet->Checked);
    // Complète les infos s
    LongueurLabel->Caption  = PFStats.chem_long;
    CoutLabel->Caption      = PFStats.chem_cout;
    RechercheLabel->Caption = PFStats.nodes_recherches;
    AjouteLabel->Caption    = PFStats.nodes_ajoutes;
    VisiteLabel->Caption    = PFStats.nodes_visites;
  }
}
//----------------------------------------------------------------------------//

