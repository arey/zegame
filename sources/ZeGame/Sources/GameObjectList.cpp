////////////////////////////////////////////////////////////////////////////////
//  Unit� "GameObject.cpp" : impl�mentation de la classe GameObjectList       //
//                                                                            //
//	Description :  Liste de tous les objets du jeu (GameObject), que ce soit  //
//				   les personnages, les d�cors ou des objets.				  //	
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

//=== Constructeur par d�faut ================================================//
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
    {	// [ACF] les GameObject sont d�j� supprim�s lorsque l'on vide les listes de D�cors et de Persos
		// delete (*iter).second;
    }
	mGameObjects.clear();
}
//----------------------------------------------------------------------------//


//=== Ajout � la liste d'un objet ============================================//
// Rmq : on ne v�rifie pas si l'objet existe d�j� dans la liste
void GameObjectList::AddGameObject(GameObject* gameObject)
{	int actualID = gameObject->GetID();

	// V�rifie si l'ID du GameObject a �t� donn� et en attribue un si l'on 
	// a affaire � un objet/d�cors/perso n'en n'ayant pas de fix�
	if ((actualID == 0) && (gameObject->GetObjectStyle() == GAMEOBJECT_ANY))
	{	switch (gameObject->GetObjectType())
		{	case GAMEOBJECT_PERSO  : actualID = iUnknowPersosNextID++; break;
			case GAMEOBJECT_DECORS : actualID = iUnknowDecorsNextID++; break;
			case GAMEOBJECT_SCENARIO : actualID = iUnknowScenarioNextID++; break;
			default : actualID = -1;
		}
		gameObject->SetAnyID(actualID);
	}

	#ifdef DEBUG	// On v�rifie si l'objet n'existe d�j� pas
	if (IsGameObject(actualID)) _asm int 3;
	#endif
	// Ins�re le GameObject dans le dico
	mGameObjects.insert(std::make_pair(actualID, gameObject));
}
//----------------------------------------------------------------------------//

//=== Indique si l'objet existe dans la liste ================================//
bool GameObjectList::IsGameObject(int id)
{	iter = mGameObjects.find(id);
    return (iter != mGameObjects.end());	// vrai si la cl� a �t� trouv�e
}
//----------------------------------------------------------------------------//

//=== Renvoie un ptr sur le GameObject ayant ce n� d'ID ======================//
GameObject* GameObjectList::GetGameObject(int id)
{	
	#ifdef DEBUG	// On v�rifie si l'objet existe bien
	if (!IsGameObject(id)) _asm int 3;
	#endif
	return mGameObjects[id];
}
//----------------------------------------------------------------------------//

//=== Supprime le GameObject n�id ============================================//
bool GameObjectList::DelGameObject(int id)
{	
	// R�cup�re un it�rateur sur ce GameObject
	iter = mGameObjects.find(id);
	if (iter == mGameObjects.end()) return false;
	
	// Supprime le GameObject
	delete (*iter).second;	
	mGameObjects.erase(iter);

	return true;
}
//----------------------------------------------------------------------------//

//=== Gestion des scripts sc�naritiques de chaque GameObject =================//
void GameObjectList::ScenarioManager()
{
	// Pour chaque GameObject du jeu
	for (iter = mGameObjects.begin(); iter != mGameObjects.end(); iter++)
	{	if ((*iter).second != NULL)
		{
			// - Traite les nouveaux �v�nements venant d'arriver
			(*iter).second->EventManager();
			// - Traite ensuite les scripts en cours d'ex�cution
			(*iter).second->ScriptsManager();
		}
	}

	// Efface r�ellement les objets
	for (int i=0 ; i < vGO2Delete.size() ; i++)
	{	DelGameObject(vGO2Delete[i]);	
	}
	vGO2Delete.clear();

}
//----------------------------------------------------------------------------//

//=== Rajoute un GameObject � la liste de ceux devant �tre bient�t effac�s ===//
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