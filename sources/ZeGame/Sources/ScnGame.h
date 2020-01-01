////////////////////////////////////////////////////////////////////////////////
//  Unité "ScnGame.h" interface de la classe TGameScenario					  //
//                                                                            //
//	Description : Regroupe tout un ensemble de fonctions liées au déroulement //
//                des scénarii du jeu					                      //
//                                                                            //
//  Copyright (C) 2000 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com	                  //
//  Version      : beta 0.01                                                  //
//  Creation     : 09/04/2000 		                                          //
//  Last upgrade : <none>	                                                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef ScnGameH
#define ScnGameH

//---------------------------------------------------------------------- Include
#ifdef MICROSOFT
#pragma warning(disable : 4100 4663 4018)  
#endif
#include <map>			// librairie STL de gestion de dictionnaires à clé unique
#include <vector>		//		"	  "	 de gestion de tableaux dynamiques

//------------------------------------------------------------------------ class
class GameObject;



// TGAMESCENARIO ///////////////////////////////////////////////////////////////
// Classe regroupant un tas de fonctions liées au scénario : 
// - permet d'associer dynamiquement un script scénaritique à un personnage
// - ...
class TGameScenario
{   //--------------------------------------------------- Attributs de la classe
	std::map<int, int> mPersoScript;	// Association n°Personnage / n°Script
	std::vector<int> vLevelMemory;		// Contient la mémoire "interne" d'un niveau
	std::vector<int> vGameMemory;		// Contient la mémoire "interne" du monde
  
	//---------------------------------------------------- Méthodes de la classe
  private:
	//--- Fonctions liées à mPersoScript ---
	void AddPersoWithScript(int perso, int script);	// Ajoute un couple à mPersoScript
	void ErasePersoWithScript(int perso);			// Supprime la paire du dictionnaire
	bool ExistPersoWithScript(int perso);			// Indique si le personnage existe ds mPersoScript
	
  public:
	TGameScenario();					// Constructeur par défaut
	~TGameScenario();					// Destructeur

	//--- Fonctions liées à mPersoScript ---
	int GetNumScriptOfPerso(int perso);				// Renvoi le n° de script associé au personnage (ou -1 si y'en a pas)
	
	//--- Fonctions intervenant sur la Mémoire du Level ---
  	bool isInLevelMemory(int memo);		// Indique si 'memo' fait parti de la mémoire interne du niveau
    void AddLevelMemory(int memo);		// Rajoute un élément de mémoire
    void DelLevelMemory(int memo);		// Perte de mémoire	
};
////////////////////////////// FIN de TGAMESCENARIO ////////////////////////////

// ---------------------------------------------------------- fonctions globales
void InitGame_GameScenario();		// Initialisation de GameScenario	
void CloseGame_GameScenario();		// Ferme en souplesse GameScenario

// ---------------------------------------------------------- variables externes
extern TGameScenario*  GameScenario;	// Instance unique de la classe





#endif  // ScnGameH
