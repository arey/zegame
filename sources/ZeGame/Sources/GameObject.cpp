////////////////////////////////////////////////////////////////////////////////
//  Unité "GameObject.cpp" : implémentation de la classe GameObject           //
//                                                                            //
//	Description :  Classe mère de tous les objets du jeu, que ce soit les     //
//				   personnages, les décors ou des objets.					  //
//                                                                            //
//  Copyright (C) 1999 Antoine Rey    arey@insa-lyon.fr                       //
//                                    antrey@club-internet.fr                 //
//  Version      : beta 0.20                                                  //
//  Creation     : 14/07/1999 		                                          //
//  Last upgrade : <none>                                                     //
////////////////////////////////////////////////////////////////////////////////

#include "ZeNain.h"         //--- Special for Visual C++ 6 MFC ---
#pragma hdrstop
#include "ScnScript.h"		// Librairie des Scripts du jeu
#include "ScnEvents.h"		// Gestionnaire des évènements
#include "GameObject.h"		// Header de la classe


////////////////////////////////////////////////////////////////////////////////
// Classe GAMEOBJECT                                                          //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur ===========================================================//
GameObject::GameObject(int potentialID)
{	// D'après le n°ID potentiel passé en paramètre, on détermine : 
	// le type d'objet, l'ID réel et la manière dont il a été attribué
	
	// Cas du personnage qui n'a pas d'ID déjà défini 
	if (potentialID == START_ID_ANY_PERSOS)
	{	iObjectType  = GAMEOBJECT_PERSO;
		iObjectStyle = GAMEOBJECT_ANY;
		iObjectID	 = 0;		
	} 
	// Cas du décors qui n'a pas d'ID déjà défini
	else
	if (potentialID == START_ID_ANY_DECORS)
	{	iObjectType  = GAMEOBJECT_DECORS;
		iObjectStyle = GAMEOBJECT_ANY;
		iObjectID	 = 0;		
	} 
	// Cas du scénario qui n'a pas d'ID déjà défini
	else
	if (potentialID == START_ID_ANY_SCENARIO)
	{	iObjectType  = GAMEOBJECT_SCENARIO;
		iObjectStyle = GAMEOBJECT_ANY;
		iObjectID	 = 0;		
	} 
	else
	// Cas du personnage qui a déjà un ID
	if ((potentialID >= START_ID_KNOW_PERSOS) && (potentialID <= LAST_ID_KNOW_PERSOS))
	{	iObjectType  = GAMEOBJECT_PERSO;
		iObjectID    = potentialID;
		iObjectStyle = GAMEOBJECT_FIXED;
	} 
	else
	// Cas du décors qui a déjà un ID
	if ((potentialID >= START_ID_KNOW_DECORS) && (potentialID <= LAST_ID_KNOW_DECORS))
	{	iObjectType	 = GAMEOBJECT_DECORS;
		iObjectID	 = potentialID;
		iObjectStyle = GAMEOBJECT_FIXED;
	}
		else
	// Cas du scénario qui a déjà un ID
	if ((potentialID >= START_ID_KNOW_SCENARIO) && (potentialID <= LAST_ID_KNOW_SCENARIO))
	{	iObjectType	 = GAMEOBJECT_SCENARIO;
		iObjectID	 = potentialID;
		iObjectStyle = GAMEOBJECT_FIXED;
	}
	else
	// ... Erreur ...
	{	iObjectType  = GAMEOBJECT_UNDEFINED;
		iObjectID	 = -1;
		iObjectStyle = GAMEOBJECT_UNDEFINED;
	}

	iScriptID = 0;						// Aucun script associé
	pScript = NULL;
	ScriptManager = new TScriptManager;	// Gestionnaire de scripts
	ScriptManager->SetSrcGO(this);
	EventsList = new TEventsList();		// Gestionnaire d'évènements perçus	
	bVisible = true;					// Visible à l'écran et dans le jeu (par défaut)
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
GameObject::~GameObject()
{	// Vide les listes d'états et de mémoire
	vStates.clear();
	vMemory.clear();
	// Supprime les gestionnaire de scripts et des évènements perçus
	delete EventsList; EventsList = NULL;
	delete ScriptManager; ScriptManager = NULL;
}	
//----------------------------------------------------------------------------//


//=== Indique si 'memo' fait parti de la mémoire interne =====================//
bool GameObject::isInMemory(int memo)
{	for (unsigned int i=0; i < vMemory.size() ; i++)
	{ if (vMemory[i] == memo) return true;
    }
    return false;
}
//----------------------------------------------------------------------------//

//=== Rajoute un élément de mémoire ==========================================//
void GameObject::AddMemory(int memo)
{	vMemory.push_back(memo);
}
//----------------------------------------------------------------------------//

//=== Perte de mémoire =======================================================//
void GameObject:: DelMemory(int memo)
{	std::vector<int>::iterator iter;  // itérateur sur le tab. dynamique d'entier
	for (iter = vMemory.begin() ; iter != vMemory.end() ; iter++)
    {   if (*iter == memo)
    	{	vMemory.erase(iter);
        	return;
		}
    }
}
//----------------------------------------------------------------------------//

//=== Indique si l'Objet est dans cet état ===================================//
bool GameObject::isInState(int state)
{   for (unsigned int i=0; i < vStates.size() ; i++)
	{ if (vStates[state] == state) return true;
    }
    return false;
}
//----------------------------------------------------------------------------//

//=== Rajoute un état à l'Objet ==============================================//
void GameObject::AddState(int state)
{	vMemory.push_back(state);
}
//----------------------------------------------------------------------------//

//=== Supression d'un état ===================================================//
void GameObject:: DelState(int state)
{	std::vector<int>::iterator iter;  // itérateur sur le tab. dynamique d'entier
	for (iter = vStates.begin() ; iter != vStates.end() ; iter++)
    {   if (*iter == state)
    	{	vStates.erase(iter);
        	return;
		}
    }
}
//----------------------------------------------------------------------------//

//=== Attribution d'un n°ID fixé par l'éditeur ===============================//
void GameObject::SetFixedID(int iID)
{
	if ((iID >= START_ID_KNOW_PERSOS) && (iID <= LAST_ID_KNOW_PERSOS))
    {	iObjectType = GAMEOBJECT_PERSO;
    } else
   	if ((iID >= START_ID_KNOW_DECORS) && (iID <= LAST_ID_KNOW_DECORS))
    {	iObjectType = GAMEOBJECT_DECORS;
    } else
	if ((iID >= START_ID_KNOW_SCENARIO) && (iID <= LAST_ID_KNOW_SCENARIO))
    {	iObjectType = GAMEOBJECT_SCENARIO;
    } else
    {   iObjectType = GAMEOBJECT_UNDEFINED;	
    }
    iObjectID = iID;
	iObjectStyle = GAMEOBJECT_FIXED;
}
//----------------------------------------------------------------------------//


//=== Attribution d'un n°ID attribué au cours du jeu =========================//
void GameObject::SetAnyID(int iID)
{	iObjectID = iID;
}
//----------------------------------------------------------------------------//

//=== Attribution d'un script .zgs ===========================================//
void GameObject::SetScriptID(int iID)
{	iScriptID = iID;
	pScript = GameScripts->GetScript(iScriptID);
}
//----------------------------------------------------------------------------//

//=== Sauvegarde des informations relatives au GameObject ====================//
void GameObject::SaveGOInfosToFile(FILE *f)
{   int i, size;

	// 1) ID & type de GO
	fwrite(&iObjectID, sizeof(iObjectID), 1, f);
	fwrite(&iObjectType, sizeof(iObjectType), 1, f);
    fwrite(&iObjectStyle, sizeof(iObjectStyle), 1, f);

    // 2) Sauvegarde des différents états que peut prendre un GameObject
    size = vStates.size();
    fwrite(&size, sizeof(int), 1, f);
    for (i=0 ; i < size ; i++)
    {	fwrite(&vStates[i], sizeof(vStates[i]), 1, f);
    }

    // 3) Sauvegarde de la mémoire "interne" de l'objet/perso/décors 
    size = vMemory.size();
    fwrite(&size, sizeof(int), 1, f);
    for (i=0 ; i < size ; i++)
    {	fwrite(&vMemory[i], sizeof(vMemory[i]), 1, f);
    }

    // 4) Sauve le n° du Script
    fwrite(&iScriptID, sizeof(iScriptID), 1, f);
}
//----------------------------------------------------------------------------//

//=== Chargement des informations relatives au GameObject ====================//
void GameObject::LoadGOInfosFromFile(FILE *f, float &version)
{	int i, size;

	// 1) ID & type de GO
	fread(&iObjectID, sizeof(iObjectID), 1, f);
	fread(&iObjectType, sizeof(iObjectType), 1, f);
    fread(&iObjectStyle, sizeof(iObjectStyle), 1, f);

    // 2) Chargement des différents états que peut prendre un GameObject
    fread(&size, sizeof(int), 1, f);
    for (i=0 ; i < size ; i++)
    {	fread(&vStates[i], sizeof(vStates[i]), 1, f);
    }

    // 3) Chargement de la mémoire "interne" de l'objet/perso/décors 
    fread(&size, sizeof(int), 1, f);
    for (i=0 ; i < size ; i++)
    {	fread(&vMemory[i], sizeof(vMemory[i]), 1, f);
    }

    // 4) Charge le n° du Script et récupère le pointeur associé à ce script
    if (version >= 2.2-0.0001)
    {	fread(&iScriptID, sizeof(iScriptID), 1, f);
		pScript = GameScripts->GetScript(iScriptID);
    }
}
//----------------------------------------------------------------------------//


//=== Déclenche les actions d'un script si ses conditions sont remplies ======//
void GameObject::EventManager()
{	int ind;
	
	// Vérifie si l'on a bien un script
	if ((iScriptID != 0) && (pScript != NULL))
	{	// 1) Vérifie les conditions et exécute les scripts 
		//    devant être vérifiés à chaque fois
		pScript->LaunchScript(SCN_TRIGGER_EVERY_TIME, this, NULL);
				
		// 2) Puis exécute les scripts réagissant à certaines causes d'évènement
		for (ind = SCN_TRIGGER_SCRIPT_FIRST ; ind <= SCN_TRIGGER_SCRIPT_LAST ; ind++)
		{	// Si il y'a du nouveau
			if (EventsList->isNewEvent(ind))
			{	pScript->LaunchScript(ind, this, NULL);
			}
		}

		// 3) Exécute aussi les scripts declenches par le scénario
		for (ind = SCN_TRIGGER_SCENARIO_FIRST ; ind <= SCN_TRIGGER_SCENARIO_LAST ; ind++)
		{	// Uniquement si il y'a du nouveau
			if (EventsList->isNewEvent(ind))
			{	pScript->LaunchScript(ind, this, NULL);
			}
		}

		EventsList->ClearNewEvents();
	}
}
//----------------------------------------------------------------------------//

//=== Crée directement un évènement et l'ajoute à la liste ===================//
void GameObject::CreateEvent(int status, int type, int typeInf, GameObject* pDest)
{	TEvent*	event = TEventsManager::EncodeEvent(status, type, typeInf);
	event->SetInfoEvent(this, pDest);
	EventsManager->DispatchEvent(event);
}

//----------------------------------------------------------------------------//

//=== Ajout d'un évènement à la liste ========================================//
void GameObject::AddEvent(TEvent* event)
{	EventsList->AddEvent(event);
}
//----------------------------------------------------------------------------//

	
//=== Ajoute un Script à la liste des scripts en cours d'exécution ===========//
void GameObject::AddRunningActionsScript(int iNumTrigger, int iNumScriptLine, TEvent* pEvent, int iNbActions)
{	ScriptManager->AddRunningActionsScript(iNumTrigger, iNumScriptLine, iNbActions);
}
//----------------------------------------------------------------------------//

//=== Indique si cette ScripLine est en cours d'execution ====================//
bool GameObject::isScriptLineRunning(int line)
{	return ScriptManager->isScriptLineRunning(line);
}
//----------------------------------------------------------------------------//

//=== Indique si le script assigne a cet object est en cours d'execution =====//
bool GameObject::isScriptRunning()
{	return	ScriptManager->isScriptRunning();
}
//----------------------------------------------------------------------------//

//=== Exécute les action des scripts en cours ================================//
void GameObject::ScriptsManager()
{	ScriptLine *scriptLine = NULL;
	TRunningScript *running = NULL;
	int nbTotalActions;
	std::list<TRunningScript*>::iterator iter;

	if (pScript == NULL) return;
	// Pour tous les scripts du GameObject en cours d'exécution
	for (iter = ScriptManager->vRunningScripts.begin(); iter != ScriptManager->vRunningScripts.end() ; )
	{	// On récupère tout d'abord la ligne de script que l'on doit exécuter
		running = (*iter);
		scriptLine = pScript->GetScriptLine(running->iNumTrigger, running->iNumScriptLine);
		nbTotalActions = scriptLine->GetNbActions();

		// Execute toutes les actions de la ScriptLine jusqu'à ce qu'on tombe sur une synchro
		do 
		{	scriptLine->ExecuteAction(running->iPosition, *running, this);	
			// Si l'on est ps en attente, on passe à l'action suivante
			if (!running->bWaiting)
			{	running->iPosition++;	
			}
		} while ((!running->bWaiting) && (running->iPosition < nbTotalActions));

		// Si l'on a fini d'exécuter toutes les actions du script
		if (running->iPosition == nbTotalActions)
		{	// On peut le supprimer de la liste et l'on récupère la cellule suivante
 			delete running; running = NULL;
			iter = ScriptManager->vRunningScripts.erase(iter);
		} else
		{	// Sinon, on passe au script suivant
			iter++;
		}		
	}
}
//----------------------------------------------------------------------------//

///////////////////////////// Fin de GAMEOBJECT ////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
// Fonctions ANNEXES														  //
////////////////////////////////////////////////////////////////////////////////

//=== Fonctions de convertions GameObject <-> Personnage/DecorsObjet ===
int GObj2Dec(int goID)		// Convertion d'un n° GameObject en DecorsObjet
{	return goID - START_ID_DECORS;
}
int Dec2GObj(int decID)		//		"		 "	  DecorsObject en GameObjet	
{	
	#ifdef DEBUG	// Vérifie si c'est bien un n° de DecorsObjet
	if ((decID < START_ID_DECORS) || (decID >= START_ID_DECORS + MAX_ID_DECORS )) return -1;
	#endif goID
	return decID + START_ID_DECORS;
}
int GObj2Per(int goID)		//		"		 "	  GameObject en Personnage
{	return goID - START_ID_PERSOS;
}
int Per2GObj(int perID)		//		"		 "	  Personnage en GameObjet
{	
	#ifdef DEBUG	// Vérifie si c'est bien un n° de DecorsObjet
	if ((perID < START_ID_PERSOS) || (perID >= START_ID_PERSOS + MAX_ID_PERSOS )) return -1;
	#endif goID
	return perID + START_ID_PERSOS;
}
