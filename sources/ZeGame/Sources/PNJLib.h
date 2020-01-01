////////////////////////////////////////////////////////////////////////////////
// Fichier : PNJLib.h  cr�� le 01/04/1999                                     //
// Classe  : PersoPNJ d�riv�e de la classe Personnage                         //
// R�le    : traite tout ce qui au Personnages Non Joueurs :                  //
//           IA, Comportements & r�action, Combats ...                        //
//                                                                            //
// Classe  : GamePersos                                                       //
// R�le    : Liste des personnages intervenants dans le jeu                   //
//                                                                            //
//  Copyright (C) 1999 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@ifrance.com						  //
////////////////////////////////////////////////////////////////////////////////
#ifndef PNJLibH
#define PNJLibH

//----------------------------------------------------------- fichiers � inclure
#include "PersoLib.h"        // Classe Personnage
#include "FPath.h"           // Algo de recherche de chemin

//========================= Classe PersoPNJ ==================================//
class PersoPNJ : public Personnage
{  public:
     PersoPNJ(int persoType);				// Constructeur
     void Update();							// G�re PROVISOIREMENT l'IA du PNJ
     void Keyb2FutEtat(void);
	 bool MoveTo(int DestU, int DestV);		// D�place le perso sur la case (DestU,DestV)

     enum ePNJAction { PASSIF=0,  DEPLACEMENT, ATTENDRE };                   // Action entreprise actuellement par le PNJ
     enum ePNJDeplac { INACTIF=0, CHERCHEPOSITION, POSITIONNEMENT, AVANCE }; // Sous-Action en cours lors d'un DEPLACEMENT


   private:
	 bool LetsGo(int DestU, int DestV);   // Calcule le chemin � suivre pour se rendre en telle case (si possible)
     ePNJAction   PNJAction;              // Action en cours
     ePNJDeplac   PNJDeplac;              // Utilis� lors d'un d�placament
     bool PersoPNJ::IsHere(GridCoord gc, double diam);

     PathVector   CheminTrouve;           // Chemin trouver par l'algo de recherche de chemin
     PFStatistics PFStats;                // Statistiques r�sultantes de ce chemin
     double correctdx, correctdy;         // Correcteur � ajouter aux coordonn�es lorsque le perso se d�place
     int NbCorrect;

   private:
     PersoPNJ();    // Constructeur par d�faut � ne pas appeler directement
};
//----------------------------------------------------------------------------//

/// FONCTIONS ANNEXES //////////////////////////////////////////////////////////
//--- Change de rep�re : carte vue du dessus -> vue du jeu ---
int DirectionMap2Vue(MapDirection2 mp);
int DirectionMap2Vue(MapDirection  mp);

//========================= Classe GamePersos ================================//
class GamePersos
{  // ----------------------------------------------------------- Atttributs ---
   private:
     std::vector <Personnage*> ListePersos;    // Liste des PNJ du jeu
	
	// ------------------------------------------------------------ M�thodes ---
   public:
     GamePersos();                        // Constructeur par d�faut
     ~GamePersos();                       // Destructeur par d�faut

     void Update();                       // Fait bouger tout ce petit monde
     void AddPerso(Personnage* perso);    // Ajoute un personnage d�j� cr�� � la liste
	 void DelPerso(int id);				  // Supprime un perso de la liste
	 void Vide();                         // Vide et efface les PNJ de la liste
	 Personnage* GetPersonnage(int id);	  // Renvoi rel personnage

	 // Renvoi le 1er personnage trouv� dans ce cerlce n'�tant pas celui ayant le n�noID
	 Personnage* GetFirstPersoInCircle(double& centX, double &centY, double &rayon, int noID=-1);	

	 // --- Fonctions inline ---
	 inline Personnage* getPersoAtIndex(int index) { return ListePersos[index]; }
	 inline int getCharacterCount()	{ return ListePersos.size(); }

   
};
//----------------------------------------------------------------------------//

//=== Unique liste des persos du jeu ===
extern GamePersos LGP;  // Liste Game Persos

#endif
