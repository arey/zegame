////////////////////////////////////////////////////////////////////////////////
//  Unité "GameObject.cpp" : implémentation de la classe GameObjectList       //
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

// --------------------------------------------------------------------- include
#pragma hdrstop
#include "ZeNain.h"				//--- Special for Visual C++ 6 MFC ---
#include "consts.h"				// [A JARTER] -> CFG_GAME_RELEASE
#include "GameObject.h"			// Classe de base de chacun des objets du jeu
#include "GameObjectList.h"		// Header de la classe


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// ------------------------------------------------------------------- variables
GameObjectList	*GOList = NULL;	// Instance unique de la classe dans tout le jeu


////////////////////////////////////////////////////////////////////////////////
// Classe GAMEOBJECTLIST                                                      //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur par défaut ================================================//
GameObjectList::GameObjectList()
{	
	iUnknowPersosNextID = START_ID_ANY_PERSOS;
	iUnknowDecorsNextID = START_ID_ANY_DECORS;
	iUnknowScenarioNextID = START_ID_ANY_SCENARIO;
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
GameObjectList::~GameObjectList()
{	
	// Supprime tous les GameObject de la liste et vide le dico
	for (iter = mGameObjects.begin(); iter != mGameObjects.end(); iter++)
    {	// [ACF] les GameObject sont déjà supprimés lorsque l'on vide les listes de Décors et de Persos
		// delete (*iter).second;
    }
	mGameObjects.clear();
}
//----------------------------------------------------------------------------//


//=== Ajout à la liste d'un objet ============================================//
// Rmq : on ne vérifie pas si l'objet existe déjà dans la liste
void GameObjectList::AddGameObject(GameObject* gameObject)
{	int actualID = gameObject->GetID();

	// Vérifie si l'ID du GameObject a été donné et en attribue un si l'on 
	// a affaire à un objet/décors/perso n'en n'ayant pas de fixé
	if ((actualID == 0) && (gameObject->GetObjectStyle() == GAMEOBJECT_ANY))
	{	switch (gameObject->GetObjectType())
		{	case GAMEOBJECT_PERSO  : actualID = iUnknowPersosNextID++; break;
			case GAMEOBJECT_DECORS : actualID = iUnknowDecorsNextID++; break;
			case GAMEOBJECT_SCENARIO : actualID = iUnknowScenarioNextID++; break;
			default : actualID = -1;
		}
		gameObject->SetAnyID(actualID);
	}

	#ifdef DEBUG	// On vérifie si l'objet n'existe déjà pas
	if (IsGameObject(actualID)) _asm int 3;
	#endif
	// Insère le GameObject dans le dico
	mGameObjects.insert(std::make_pair(actualID, gameObject));
}
//----------------------------------------------------------------------------//

//=== Indique si l'objet existe dans la liste ================================//
bool GameObjectList::IsGameObject(int id)
{	iter = mGameObjects.find(id);
    return (iter != mGameObjects.end());	// vrai si la clé a été trouvée
}
//----------------------------------------------------------------------------//

//=== Renvoie un ptr sur le GameObject ayant ce n° d'ID ======================//
GameObject* GameObjectList::GetGameObject(int id)
{	
	#ifdef DEBUG	// On vérifie si l'objet existe bien
	if (!IsGameObject(id)) _asm int 3;
	#endif
	return mGameObjects[id];
}
//----------------------------------------------------------------------------//

//=== Supprime le GameObject n°id ============================================//
bool GameObjectList::DelGameObject(int id)
{	
	// Récupère un itérateur sur ce GameObject
	iter = mGameObjects.find(id);
	if (iter == mGameObjects.end()) return false;
	
	// Supprime le GameObject
	delete (*iter).second;	
	mGameObjects.erase(iter);

	return true;
}
//----------------------------------------------------------------------------//

//=== Gestion des scripts scénaritiques de chaque GameObject =================//
void GameObjectList::ScenarioManager()
{
	// Pour chaque GameObject du jeu
	for (iter = mGameObjects.begin(); iter != mGameObjects.end(); iter++)
	{	if ((*iter).second != NULL)
		{
			// - Traite les nouveaux évènements venant d'arriver
			(*iter).second->EventManager();
			// - Traite ensuite les scripts en cours d'exécution
			(*iter).second->ScriptsManager();
		}
	}

	// Efface réellement les objets
	for (int i=0 ; i < vGO2Delete.size() ; i++)
	{	DelGameObject(vGO2Delete[i]);	
	}
	vGO2Delete.clear();

}
//----------------------------------------------------------------------------//

//=== Rajoute un GameObject à la liste de ceux devant être bientôt effacés ===//
void GameObjectList::PrepareToDelete(int id)
{	vGO2Delete.push_back(id);
	mGameObjects[id] = NULL;	// [A CONFIRMER]
}
//----------------------------------------------------------------------------//

/////////////////////////// Fin de GAMEOBJECTLIST //////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//							 Fonctions Annexes                                //
////////////////////////////////////////////////////////////////////////////////

//=== Initialise la liste des GameObject du jeu ==============================//
void InitGame_GameObjectList()
{	GOList = new GameObjectList;
}
//----------------------------------------------------------------------------//

//=== Referme proprement la liste des GameObject =============================//
void CloseGame_GameObjectList()
{	delete GOList;
	GOList = NULL;
}
//----------------------------------------------------------------------------//