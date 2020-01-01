////////////////////////////////////////////////////////////////////////////////
// Fichier : PNJLib.cpp  cr�� le 01/04/1999                                   //
// Classe  : PersoPNJ d�riv�e de la classe Personnage                         //
// R�le    : traite tout ce qui au Personnages Non Joueurs :                  //
//           IA, Comportements & r�action, Combats ...                        //
////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------- fichiers � inclure
#include <math.h>
#include "ZeNain.h"			// Fichier commun � tout le projet
#include "WorldSimulation.h"
#include "Animall.h"
#include "PNJLib.h"			// fichier d'en-t�te

#ifdef VISUALC
#pragma warning(disable : 4063)  
#endif


//=== Unique liste des persos du jeu ===
GamePersos LGP;  // Liste Game Persos


////////////////////////////////////////////////////////////////////////////////
// Classe PNJLib                                                              //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur ===========================================================//
PersoPNJ::PersoPNJ(int persoType)
   : Personnage(persoType)   // Appelle le constructeur de sa classe racine
{  PNJAction = PASSIF;       // Perso inactif au d�but
   PNJDeplac = INACTIF;      // Aucun d�placement en cours
   correctdx = correctdy = 0;

   //--- Variables d'�tat changeant par rapport au personnage joueur ---
   peAllCollisions = false;  // D�sactive les collisions !
   #ifdef CBUILDER
   randomize();
   #endif
}
//----------------------------------------------------------------------------//

//=== Calcule le chemin � suivre pour aller au point (DestU,DestV) � partir ==//
//    de la position actuelle du PNJ : renvoie un chemin linearis�
bool PersoPNJ::LetsGo(int DestU, int DestV)
{  CheminTrouve.clear();  // Reinitialise le chemin 
   TMap *pMap  = MapNiveau->PrincEtage->LayerDecors;
   PFStats = RechercheChemin(*pMap,GridCoord(peU,peV), GridCoord(DestU,DestV), CheminTrouve, true);

   // Si on n'a pas trouv� de chemins
   if (PFStats.chem_long==0) return false;
   return true;
}
//----------------------------------------------------------------------------//

//=== G�re PROVISOIREMENT l'IA du PNJ ========================================//
void PersoPNJ::Update()
{  // Si le perso est inactif, on essaye de le faire aller quelque part
   if (PNJAction == PASSIF)
   { if ((peEtat==ARRET1) && (peFutEtat==ARRET1))
     {  if ((rand()  %  120)== 0)
        { peFutEtat=STAND1;
          PNJAction = ATTENDRE;
        }
     }
   }

   if (PNJAction == ATTENDRE)
   { if (peEtat==ARRET1)
     {  PNJAction = PASSIF;
     }
   }

   static int pt1u = 5,
              pt1v = 5,
              pt2u = 23,
              pt2v = 35;
   static bool Aller = true;

   // On peut faire d�placer le perso si il a un script et si SE_BALLADE_LIBREMENT_SUR_LA_CARTE est pr�sent
   if (hasScript() && isInMemory(106))  // SE_BALLADE_LIBREMENT_SUR_LA_CARTE = 106
   {	if (PNJAction == PASSIF)
		if ((rand() % 100)==0)  // Une chance sur 100 de se d�placer
		{ // On r�cup�re un point au hazard sur la map
		  int ptW = rand() % MapNiveau->PrincEtage->EtageWidth;
		  int ptH = rand() % MapNiveau->PrincEtage->EtageHeight;

		  if (LetsGo(ptW,ptH))
		  { PNJAction = DEPLACEMENT;      // On peut y aller
			PNJDeplac = CHERCHEPOSITION;  // Il faut commencer par se trouner dans la bonne direction
		  }
		}
   }

   // Si le perso est en train de se d�placer vers un point
   if (PNJAction == DEPLACEMENT)
   { // Selon les sous-�tats dans lesquels il se trouve
     switch (PNJDeplac)
     { case (CHERCHEPOSITION):  //  Recherche la bonne direction
       { // R�cup�re les 2 points de d�part et d'arriv�e et d�termine la direction � prendre
         GridCoord gDep, gArr;
         gDep = CheminTrouve.back();
         CheminTrouve.pop_back();  // Supprime le point de d�part
         gArr = CheminTrouve.back();
         CheminTrouve.push_back(gDep);  // Supprime le point de d�part
         MapDirection2 dirASuivre;
         SearchDirection(gDep, gArr, dirASuivre);
         // dirASuivre = M_DROITE;
         // Change de rep�re : carte vue du dessus -> vue du jeu
         int FutOrient = DirectionMap2Vue(dirASuivre);
         peFutDirection = FutOrient;
         // Rmq: Suivant son orientation actuelle, Fut2PresEtat d"termine si le
         // perso va devoir tourner � gauche, � droite ou carr�ment se retourner.

         PNJDeplac = POSITIONNEMENT;
       } break; // ** Rmq: peut-�tre pas indispensable
       case (POSITIONNEMENT):   // En cours de repositionnement
       { // On regarde si le perso est dans la bonne direction
         if (peFutDirection == peDirection)
         {  peFutEtat=MARCHE1;   // On avance
            PNJDeplac = AVANCE;

            // Cible exactement le point d'arriv�e en ajustement le d�plcament du perso
            GridCoord gDep = CheminTrouve.back();
            CheminTrouve.pop_back();  // Supprime le point de d�part
            double diffX = ((double)gDep.x+0.5)*WorldModel.SQR_LENGTH - PersoNivX;
            double diffY = ((double)gDep.y+0.5)*WorldModel.SQR_LENGTH - PersoNivY;
            GridCoord gArr = CheminTrouve.back();
            double distX = ((double)gArr.x+0.5)*WorldModel.SQR_LENGTH - PersoNivX;
            double distY = ((double)gArr.y+0.5)*WorldModel.SQR_LENGTH - PersoNivY;
            double longueur = sqrt(distX*distX + distY*distY);

            NbCorrect = (int) longueur*10;
            correctdx = diffX / NbCorrect;
            correctdy = diffY / NbCorrect;
         }
       } break;
       case (AVANCE):
       { // On regarde si l'on est pas arriv� proche d'un point de destination
         GridCoord gArr = CheminTrouve.back();
         peFutEtat=MARCHE1;   // On avance
         // Corrige sa trajectoire
         if (NbCorrect>0)
         { PersoNivX += correctdx;
           PersoNivY += correctdy;
           PersoNivXY2UV();
           PersoNivXYtoPeXY();
           NbCorrect--;
         }
         // Attention : On fait �� grossi�rement pour l'instant
         //if ((peU == gArr.x) && (peV == gArr.y))
         //double longueur = peSpeed * 2 *3/ peNbSprite;
         if (IsHere(gArr,0.2))
         {
            peFutEtat = ARRET1;
            peEtat = ARRET1;

            if (CheminTrouve.size()>1)  // Si on est pas encore arriv� au bout
            { PNJDeplac = CHERCHEPOSITION;  // Il faut commencer par se trouner dans la bonne direction
            }
            else // On est arriv� au bout du chemin
            { PNJAction = PASSIF;
              PNJDeplac = INACTIF;
              peFutEtat = ARRET1;
//              Application->Terminate();
            }
         }
       } break;
     }

   }
}
//----------------------------------------------------------------------------//

void PersoPNJ::Keyb2FutEtat(void)
{	Update();
}

////////////////////////////////////////////////////////////////////////////////
// Fonctions Annexes                                                          //
////////////////////////////////////////////////////////////////////////////////

//--- Change de rep�re : carte vue du dessus -> vue du jeu -------------------//
int DirectionMap2Vue(MapDirection2 mp)
{ switch (mp)
  { case (M_GAUCHE)    : return or_NO;
    case (M_GAUCHAUT)  : return or_N;
    case (M_HAUT)      : return or_NE;
    case (M_DROITHAUT) : return or_E;
    case (M_DROITE)    : return or_SE;
    case (M_DROITBAS)  : return or_S;
    case (M_BAS)       : return or_SO;
    case (M_GAUCHBAS)  : return or_O;
  }
  return -1;
}
int DirectionMap2Vue(MapDirection mp)
{ switch (mp)
  { case (M_GAUCHE) : return or_NO;
    case (M_HAUT)   : return or_NE;
    case (M_DROITE) : return or_SE;
    case (M_BAS)    : return or_SO;
  }
  return -1;
}
//----------------------------------------------------------------------------//

bool PersoPNJ::IsHere(GridCoord gc, double diam)
{ // Calcule en m�tres les coords du milieu du trap�ze situ� en 'gc'
  double midleNivX = ((double)gc.x+0.5)*WorldModel.SQR_LENGTH;
  double midleNivY = ((double)gc.y+0.5)*WorldModel.SQR_LENGTH;

  double dim = sqrt((PersoNivX-midleNivX)*(PersoNivX-midleNivX)+(PersoNivY-midleNivY)*(PersoNivY-midleNivY));

  return (dim < diam);
}
//----------------------------------------------------------------------------//

//=== D�place le perso sur la case (DestU,DestV) =============================//
bool PersoPNJ::MoveTo(int DestU,int DestV)
{	if (LetsGo(DestU,DestV))
    {	PNJAction = PersoPNJ::DEPLACEMENT;      // On peut y aller
		PNJDeplac = PersoPNJ::CHERCHEPOSITION;  // Il faut commencer par se trouner dans la bonne direction
		return true;
    }
	return false;
}
//----------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////////////////
// Classe GamePersos                                                          //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur par d�faut ================================================//
GamePersos::GamePersos()
{
}
//----------------------------------------------------------------------------//

//=== Destructeur par d�faut =================================================//
GamePersos::~GamePersos()
{  Vide();   // Vide la liste des persos
}
//----------------------------------------------------------------------------//

//=== Vide la liste des PNJ ==================================================//
void GamePersos::Vide()
{ for (unsigned int i=0; i < ListePersos.size(); i++)
  {  delete ListePersos[i];
  }
  ListePersos.clear();
}
//----------------------------------------------------------------------------//

//=== Fait vivre tous les persos =============================================//
void GamePersos::Update()
{ Personnage* perso = NULL;
  for (unsigned int i=0; i < ListePersos.size(); i++)
  {  // Certaines actions ne doivent pas �tre faites pour le joueur humain
	 perso = ListePersos[i];
	 if (perso != NULL)
	 if ((perso->ModelePersoType != HERO1) || (CFG_DEMO_TOURNANTE))
	 {	
		perso->Fut2PresEtat();
		perso->Update();  // <=> Keyb2FutEtat()
		perso->peBouge();
	 }
  }

}
//----------------------------------------------------------------------------//

//=== Ajoute un personnage d�j� cr�� � la liste ==============================//
void GamePersos::AddPerso(Personnage* perso)
{ ListePersos.push_back(perso);
}
//----------------------------------------------------------------------------//

//=== Supprime un personnage d�j� cr�� � la liste ============================//
// Rmq : mais ne d�truit pas son ptr (travail de la liste de GameObject)
void GamePersos::DelPerso(int id)
{	Personnage* perso = NULL;
	// [A MODIFIER]
	for (unsigned int i=0; i < ListePersos.size(); i++)
	{	perso = ListePersos[i];
		if ((perso!=NULL) && (perso->GetID() == id))
		{	ListePersos[i] = NULL;
			return;
		}
	}
}
//----------------------------------------------------------------------------//

//=== Renvoi rel personnage ==================================================//
Personnage* GamePersos::GetPersonnage(int id)
{	for (unsigned int i=0; i < ListePersos.size(); i++)
	{	if (ListePersos[i]->GetID() == id)
		{ return ListePersos[i];	
		}
	}
	return NULL;
}
//----------------------------------------------------------------------------//

//=== Renvoi le 1er personnage trouv� dans ce cerlce n'�tant pas celui ayant =//
//    le n�'noID'
Personnage* GamePersos::GetFirstPersoInCircle(double& centX, double &centY, double &rayon, int noID)
{	for (unsigned int i=0; i < ListePersos.size(); i++)
	{	Personnage *perso = ListePersos[i];
		if ((perso!=NULL) &&(perso->GetID() != noID))
		{	if (perso->isInsideCircle(centX, centY, rayon)) return perso;
		}
	}
	return NULL;
}
//----------------------------------------------------------------------------//