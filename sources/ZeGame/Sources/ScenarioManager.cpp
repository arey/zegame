////////////////////////////////////////////////////////////////////////////////
//  Unité "ScenarioManager.h" : implementation de la classe ScenarioManager   //
//                                                                            //
//	Description :  Cette classe est un singleton qui manage les differents    //
//				   ScenarioObject pourvus de scripts scenaritiques leur		  //
//				   permettant de, par exemple, gerer les cinematiques, les	  //
//				   bruitages ou les musiques, la trame du scenario principal  //
//				   ou encore celle de sous-quetes.							  // 
//                                                                            //
//  Copyright (C) 2000 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@ifrance.com		                  //
//  Version      : beta 0.01                                                  //
//  Creation     : 03/09/2000 		                                          //
//  Last upgrade : <none>                                                     //
////////////////////////////////////////////////////////////////////////////////

#include "ZeNain.h"				//--- Special for Visual C++ 6 MFC ---
#include "GameObjectList.h"		// Liste de tous les GameObject du jeu
#include "ScnEvents.h"			// Gestionnaire d'évènements
#pragma hdrstop
#include "ScenarioObject.h"		// GameObjets lies au scenario
#include "ScenarioManager.h"	// Header de la classe


////////////////////////////////////////////////////////////////////////////////
// Classe SCENARIOMANAGER                                                     //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur par defaut ================================================//
ScenarioManager::ScenarioManager()
{	
}
//----------------------------------------------------------------------------//


//=== Destructeur ============================================================//
ScenarioManager::~ScenarioManager()
{	mScenarioObject.clear();
}	
//----------------------------------------------------------------------------//

//=== Initialise tous les objets lies au scenario ============================//
void ScenarioManager::init()
{	ScenarioObject *pSO = NULL;
	
	// --- Cree tous les objets responsables de certaines taches du scenario ---
	// LEVEL_INIT
	pSO = new ScenarioObject(SCENARIO_OBJECT_LEVEL_INIT);
	mScenarioObject[SCENARIO_OBJECT_LEVEL_INIT] = pSO;
	GOList->AddGameObject(pSO);
	// LEVEL_CLOSE
	pSO = new ScenarioObject(SCENARIO_OBJECT_LEVEL_CLOSE);
	mScenarioObject[SCENARIO_OBJECT_LEVEL_CLOSE] = pSO;
	GOList->AddGameObject(pSO);

	// LEVEL_SFX_AMBIENT
	pSO = new ScenarioObject(SCENARIO_OBJECT_LEVEL_SFX_AMBIENT);
	mScenarioObject[SCENARIO_OBJECT_LEVEL_SFX_AMBIENT] = pSO;
	GOList->AddGameObject(pSO);

	// --- Assigne des scripts particuliers a ces objets ---
	mScenarioObject[SCENARIO_OBJECT_LEVEL_INIT]->SetScriptID(10001);		// NewLevelScenario.zfs
	mScenarioObject[SCENARIO_OBJECT_LEVEL_SFX_AMBIENT]->SetScriptID(10002);	// NewLevelAmbienSFX.zfs

}
//----------------------------------------------------------------------------//

//=== MaJ le Scenario ========================================================//
void ScenarioManager::update()
{	static bool bFirstTime = true;
	// Si c'est la toute 1ere fois que cette fonction est appelee dans un 
	// nouveau niveau, on envoie l'evenement au ScenarioObject dedie.
	if (bFirstTime)
	{	CreateEvent(EVENTS_STATUS_DIRECT, EVENTS_TYPE_SCENARIO, 
 					SCN_TRIGGER_SCENARIO_LEVEL_INIT, mScenarioObject[SCENARIO_OBJECT_LEVEL_INIT]);
		bFirstTime = false;
	}
}
//----------------------------------------------------------------------------//

//=== Crée directement un évènement et l'ajoute à la liste ===================//
void ScenarioManager::CreateEvent(int status, int type, int typeInf, GameObject* pDest)
{	TEvent*	event = TEventsManager::EncodeEvent(status, type, typeInf);
	event->SetInfoEvent(NULL, pDest);
	EventsManager->DispatchEvent(event);
}
//----------------------------------------------------------------------------//

/////////////////////////// Fin de SCENARIOMANAGER /////////////////////////////



////////////////////////////////////////////////////////////////////////////////
// Fonctions ANNEXES														  //
////////////////////////////////////////////////////////////////////////////////

//=== Initialise le manager du Scenarios =====================================//
void InitGame_ScenarioManager()
{	ScenarioManager *pSM = new ScenarioManager();
	pSM->init();
}
//----------------------------------------------------------------------------//

//=== Referme proprement le manager du Scenarios =============================//
void CloseGame_ScenarioManager()
{	ScenarioManager *pSM = ScenarioManager::getSingletonPtr();
	delete pSM;
}
//----------------------------------------------------------------------------//
