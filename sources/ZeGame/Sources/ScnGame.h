////////////////////////////////////////////////////////////////////////////////
//  Unit� "ScnGame.h" interface de la classe TGameScenario					  //
//                                                                            //
//	Description : Regroupe tout un ensemble de fonctions li�es au d�roulement //
//                des sc�narii du jeu					                      //
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
#include <map>			// librairie STL de gestion de dictionnaires � cl� unique
#include <vector>		//		"	  "	 de gestion de tableaux dynamiques

//------------------------------------------------------------------------ class
class GameObject;



// TGAMESCENARIO ///////////////////////////////////////////////////////////////
// Classe regroupant un tas de fonctions li�es au sc�nario : 
// - permet d'associer dynamiquement un script sc�naritique � un personnage
// - ...
class TGameScenario
{   //--------------------------------------------------- Attributs de la classe
	std::map<int, int> mPersoScript;	// Association n�Personnage / n�Script
	std::vector<int> vLevelMemory;		// Contient la m�moire "interne" d'un niveau
	std::vector<int> vGameMemory;		// Contient la m�moire "interne" du monde
  
	//---------------------------------------------------- M�thodes de la classe
  private:
	//--- Fonctions li�es � mPersoScript ---
	void AddPersoWithScript(int perso, int script);	// Ajoute un couple � mPersoScript
	void ErasePersoWithScript(int perso);			// Supprime la paire du dictionnaire
	bool ExistPersoWithScript(int perso);			// Indique si le personnage existe ds mPersoScript
	
  public:
	TGameScenario();					// Constructeur par d�faut
	~TGameScenario();					// Destructeur

	//--- Fonctions li�es � mPersoScript ---
	int GetNumScriptOfPerso(int perso);				// Renvoi le n� de script associ� au personnage (ou -1 si y'en a pas)
	
	//--- Fonctions intervenant sur la M�moire du Level ---
  	bool isInLevelMemory(int memo);		// Indique si 'memo' fait parti de la m�moire interne du niveau
    void AddLevelMemory(int memo);		// Rajoute un �l�ment de m�moire
    void DelLevelMemory(int memo);		// Perte de m�moire	
};
////////////////////////////// FIN de TGAMESCENARIO ////////////////////////////

// ---------------------------------------------------------- fonctions globales
void InitGame_GameScenario();		// Initialisation de GameScenario	
void CloseGame_GameScenario();		// Ferme en souplesse GameScenario

// ---------------------------------------------------------- variables externes
extern TGameScenario*  GameScenario;	// Instance unique de la classe





#endif  // ScnGameH
