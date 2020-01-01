////////////////////////////////////////////////////////////////////////////////
// Fichier : PNJLib.h  créé le 01/04/1999                                     //
// Classe  : PersoPNJ dérivée de la classe Personnage                         //
// Rôle    : traite tout ce qui au Personnages Non Joueurs :                  //
//           IA, Comportements & réaction, Combats ...                        //
//                                                                            //
// Classe  : GamePersos                                                       //
// Rôle    : Liste des personnages intervenants dans le jeu                   //
//                                                                            //
//  Copyright (C) 1999 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@ifrance.com						  //
////////////////////////////////////////////////////////////////////////////////
#ifndef PNJLibH
#define PNJLibH

//----------------------------------------------------------- fichiers à inclure
#include "PersoLib.h"        // Classe Personnage
#include "FPath.h"           // Algo de recherche de chemin

//========================= Classe PersoPNJ ==================================//
class PersoPNJ : public Personnage
{  public:
     PersoPNJ(int persoType);				// Constructeur
     void Update();							// Gère PROVISOIREMENT l'IA du PNJ
     void Keyb2FutEtat(void);
	 bool MoveTo(int DestU, int DestV);		// Déplace le perso sur la case (DestU,DestV)

     enum ePNJAction { PASSIF=0,  DEPLACEMENT, ATTENDRE };                   // Action entreprise actuellement par le PNJ
     enum ePNJDeplac { INACTIF=0, CHERCHEPOSITION, POSITIONNEMENT, AVANCE }; // Sous-Action en cours lors d'un DEPLACEMENT


   private:
	 bool LetsGo(int DestU, int DestV);   // Calcule le chemin à suivre pour se rendre en telle case (si possible)
     ePNJAction   PNJAction;              // Action en cours
     ePNJDeplac   PNJDeplac;              // Utilisé lors d'un déplacament
     bool PersoPNJ::IsHere(GridCoord gc, double diam);

     PathVector   CheminTrouve;           // Chemin trouver par l'algo de recherche de chemin
     PFStatistics PFStats;                // Statistiques résultantes de ce chemin
     double correctdx, correctdy;         // Correcteur à ajouter aux coordonnées lorsque le perso se déplace
     int NbCorrect;

   private:
     PersoPNJ();    // Constructeur par défaut à ne pas appeler directement
};
//----------------------------------------------------------------------------//

/// FONCTIONS ANNEXES //////////////////////////////////////////////////////////
//--- Change de repère : carte vue du dessus -> vue du jeu ---
int DirectionMap2Vue(MapDirection2 mp);
int DirectionMap2Vue(MapDirection  mp);

//========================= Classe GamePersos ================================//
class GamePersos
{  // ----------------------------------------------------------- Atttributs ---
   private:
     std::vector <Personnage*> ListePersos;    // Liste des PNJ du jeu
	
	// ------------------------------------------------------------ Méthodes ---
   public:
     GamePersos();                        // Constructeur par défaut
     ~GamePersos();                       // Destructeur par défaut

     void Update();                       // Fait bouger tout ce petit monde
     void AddPerso(Personnage* perso);    // Ajoute un personnage déjà créé à la liste
	 void DelPerso(int id);				  // Supprime un perso de la liste
	 void Vide();                         // Vide et efface les PNJ de la liste
	 Personnage* GetPersonnage(int id);	  // Renvoi rel personnage

	 // Renvoi le 1er personnage trouvé dans ce cerlce n'étant pas celui ayant le n°noID
	 Personnage* GetFirstPersoInCircle(double& centX, double &centY, double &rayon, int noID=-1);	

	 // --- Fonctions inline ---
	 inline Personnage* getPersoAtIndex(int index) { return ListePersos[index]; }
	 inline int getCharacterCount()	{ return ListePersos.size(); }

   
};
//----------------------------------------------------------------------------//

//=== Unique liste des persos du jeu ===
extern GamePersos LGP;  // Liste Game Persos

#endif
