////////////////////////////////////////////////////////////////////////////////
//  Unité "GameObject.h" : interface de la classe GameObjectList              //
//                                                                            //
//	Description :  Liste de tous les objets du jeu (GameObject), que ce soit  //
//				   les personnages, les décors ou des objets.				  //	
//                                                                            //
//  Copyright (C) 1999 Antoine Rey    arey@insa-lyon.fr                       //
//                                    antrey@club-internet.fr                 //
//  Version      : beta 0.20                                                  //
//  Creation     : 14/07/1999 		                                          //
//  Last upgrade : <none>                                                     //
////////////////////////////////////////////////////////////////////////////////

#ifndef GameObjectListH
#define GameObjectListH

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// --------------------------------------------------------------------- include
#include <vector>	   	//	librairie STL de gestion de tableaux dynamiques
#include <map>          // librairie STL de gestion de dictionnaires à clé unique

class GameObject;

////////////////////////////////////////////////////////////////////////////////
// Classe GAMEOBJECTLIST
class GameObjectList
{ protected:
	std::map<int, GameObject*>	mGameObjects;	// Dictionnaire contenant tous les objets du jeu
	std::map<int, GameObject*>::iterator iter;  // Itérateur parourant le dico
	std::vector<int> vGO2Delete;				// Liste GameObject à effacer

	int	iUnknowPersosNextID;					// Prochain ID à attribuer à un personnage sans trop d'importance
	int	iUnknowDecorsNextID;					//		"	 "		"			décors		"			"	

  public:
	GameObjectList();				// Constructeur par défaut
	virtual ~GameObjectList();		// Destructeur
	void AddGameObject(GameObject* gameObject);		// Ajout à la liste d'un objet
	bool IsGameObject(int id);						// Vrai si l'objet existe dans la liste
	bool DelGameObject(int id);						// Supprime le GameObject n°id
	GameObject* GetGameObject(int id);				// Renvoie un ptr sur le GameObject ayant ce n° d'ID
	void PrepareToDelete(int id);					// Rajoute un GameObject à la liste de ceux devant être bientôt effacés

	//--- Fonctions de Mise à Jour ---
	void ScenarioManager();							// Gestion des scripts scénaritiques de chaque GameObject
};
//==================== Fin de la classe GAMEOBJECTLIST ========================//

//----------------------------------------------------------- Variables externes
extern GameObjectList	*GOList;		// Instance unique de la classe dans tout le jeu

//------------------------------------------------------------ Fonctions annexes
void InitGame_GameObjectList();			// Initialise la liste des GameObject du jeu
void CloseGame_GameObjectList();		// Referme proprement la liste des GameObject

#endif // GameObjectListH
