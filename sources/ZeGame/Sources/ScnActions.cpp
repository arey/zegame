////////////////////////////////////////////////////////////////////////////////
//  Unité "ScnActions.cpp" implémentation de la classe TAction 				  //
//                                                                            //
//	Description :                                                             //
//                                                                            //
//  Copyright (C) 1999 Antoine Rey    arey@insa-lyon.fr                       //
//                                    antrey@club-internet.fr                 //
//  Version      : beta 0.20                                                  //
//  Creation     : 14/07/1999 		                                          //
//  Last upgrade : <none>	                                                  //
////////////////////////////////////////////////////////////////////////////////


//---------------------------------------------------------------------- Include
#include "ZeNain.h"         
#ifndef MICROSOFT
// Unité principale du projet
#ifdef MAPEDITOR
#include "uMapView.h"
#else
#include "DirectLib.h"
#endif  // MAPEDITOR
#endif
#include "ImDecors2.h"		// Gestion des décors
#include "PersoLib.h"		// Gestion des personnages
#include "GameObject.h"		// Gestion des objets du jeu
#include "GameObjectList.h"	// Liste des GameObject du jeu
#include "PNJLib.h"			// Gestion des PNJ
#include "MemLib.h"			// Lib. d'outils de communications
#include "ScnScript.h"		// Librairie des Scripts du jeu
#include "TimeClock.h"		// Lib. de gestion temporelle du jeu
#include "WorldSimulation.h"// Paramètrages des lois physiques du monde
#include "Animall.h"		// Lib. d'animations
#include "ScnGame.h"		// Fonctionnalités liées au scénario
#include "RandomType.h"		// Types aleatoires
#pragma hdrstop
#include "ScnConsts.h"		// Constantes liées à la gestion du scénario
#include "ScnActions.h"		// Header du fichier


////////////////////////////////////////////////////////////////////////////////
// Classe TACTION                                                             //
////////////////////////////////////////////////////////////////////////////////

int TAction::iLastID = 0;		// N°ID à attribuer à la prochaine action

//=== Constructeur par défaut ================================================//
TAction::TAction()
{	iActionID = iLastID++;				// Attribution d'un n°ID libre
	iActionType = ACTION_TYPE_NONE;     // Type d'action indéfini
	bEndSynchro = false;				// Aucune synchronisation, par défaut
}
//----------------------------------------------------------------------------//

//=== Destructeur par défaut =================================================//
TAction::~TAction()
{
}
//----------------------------------------------------------------------------//

//=== Effectue l'action, quel que soit son type ==============================//
void TAction::PlayAction(TRunningScript &RunningAction, GameObject *pGO)
{	// Crée le Sémaphore de synchronisation
	if (bEndSynchro)
	{	RunningAction.iMutexID[RunningAction.iPosition] = MemMutexLib->NewMutex();
		RunningAction.iActionMutex[RunningAction.iActionMutexIndex++] = RunningAction.iPosition;
	}
}
//----------------------------------------------------------------------------//
///////////////////////// Fin de la classe TACTION /////////////////////////////



////////////////////////////////////////////////////////////////////////////////
//								GAMEOBJECT									  //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Classe TACTION_GO_NEWMEMORY												  //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur  ==========================================================//
TAction_GONewMemory::TAction_GONewMemory(int memID, int goID):
	iMemoryID(memID),
    iGoID(goID)
{	iActionType = ACTION_TYPE_GO_NEWMEMORY;
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TAction_GONewMemory::~TAction_GONewMemory()
{	}
//----------------------------------------------------------------------------//

//=== Ajoute un élément de Mémoire au GameObject =============================//
void TAction_GONewMemory::PlayAction(TRunningScript &RunningAction, GameObject *pGO)
{	if (iGoID == 0)
	{	pGO->AddMemory(iMemoryID);
	} else
	if (iGoID == -1)
	{
	} else
	{	GOList->GetGameObject(iGoID)->AddMemory(iMemoryID);
	}
}
//----------------------------------------------------------------------------//
/////////////////// Fin de la classe TACTION_GO_NEWMEMORY //////////////////////

////////////////////////////////////////////////////////////////////////////////
// Classe TACTION_GO_DELMEMORY												  //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur  ==========================================================//
TAction_GODelMemory::TAction_GODelMemory(int memID, int goID):
	iMemoryID(memID),
    iGoID(goID)
{	iActionType = ACTION_TYPE_GO_DELMEMORY;	
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TAction_GODelMemory::~TAction_GODelMemory()
{	}
//----------------------------------------------------------------------------//

//=== Supprime un élément de Mémoire au GameObject ===========================//
void TAction_GODelMemory::PlayAction(TRunningScript &RunningAction, GameObject *pGO)
{	if (iGoID == 0)
	{	pGO->DelMemory(iMemoryID);
	} else
	if (iGoID == -1)
	{
	} else
	{	GOList->GetGameObject(iGoID)->DelMemory(iMemoryID);
	}
}
//----------------------------------------------------------------------------//
/////////////////// Fin de la classe TACTION_GO_DELMEMORY //////////////////////


////////////////////////////////////////////////////////////////////////////////
// Classe TACTION_GO_SETSCRIPT												  //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur  ==========================================================//
TAction_GOSetScript::TAction_GOSetScript(int scrID, int goID):
	iScriptID(scrID),
    iGoID(goID)
{	iActionType = ACTION_TYPE_GO_SETSCRIPT;
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TAction_GOSetScript::~TAction_GOSetScript()
{	}
//----------------------------------------------------------------------------//

//=== Associe un script scénaritique à ce GO =================================//
void TAction_GOSetScript::PlayAction(TRunningScript &RunningAction, GameObject *pGO)
{	GameObject* go = NULL;
	// Détermine si c'est le perso qui a déclenché l'action ou un autre auquel on doit associer le script
	if (iGoID == 0)
	{	// Si c'est le perso du script
		go = pGO;	
	} else
	// Ou encore, ca peut être le dernier perso manipulé (créé, déplacer) du script
	if (iGoID == -1)
	{	go = GOList->GetGameObject(RunningAction.iLastGoID);
	} else
	// Sinon, çà ne peut être que le GO précisé dans ce script	
	{	go = GOList->GetGameObject(iGoID);
	}

	// Assigne le script à ce GameObject
	go->SetScriptID(iScriptID);
}
//----------------------------------------------------------------------------//
/////////////////// Fin de la classe TACTION_GO_SETSCRIPT //////////////////////

////////////////////////////////////////////////////////////////////////////////
// Classe TACTION_GO_VISIBLE												  //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur  ==========================================================//
TAction_GOSetVisible::TAction_GOSetVisible(bool vis, int goID):
	bVisible(vis),
    iGoID(goID)
{	iActionType = ACTION_TYPE_GO_VISIBLE;
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TAction_GOSetVisible::~TAction_GOSetVisible()
{	}
//----------------------------------------------------------------------------//

//=== Fait apparaître/disparaître le GameObject ==============================//
void TAction_GOSetVisible::PlayAction(TRunningScript &RunningAction, GameObject *pGO)
{	GameObject* go = NULL;
	// Détermine si c'est le GO qui a déclenché l'action ou un autre auquel on doit associer le script
	if (iGoID == 0)
	{	// Si c'est le GO du script
		go = pGO;	
	} else
	// Ou encore, ca peut être le dernier GO manipulé (créé, déplacer) du script
	if (iGoID == -1)
	{	go = GOList->GetGameObject(RunningAction.iLastGoID);
	} else
	// Sinon, çà ne peut être que le GO précisé dans ce script	
	{	go = GOList->GetGameObject(iGoID);
	}

	// Fait apparaître/disparaître ce GameObject
	go->SetVisible(bVisible);
}
//----------------------------------------------------------------------------//
///////////////////// Fin de la classe TACTION_GO_VISIBLE //////////////////////


////////////////////////////////////////////////////////////////////////////////
//								  DECORS									  //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Classe TACTION_DECORS_PLAY_SUPANIM                                         //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur  ==========================================================//
TAction_DecorsPlaySupAnim::TAction_DecorsPlaySupAnim(int decID, int anmID):
   //	TAction(),	// Constructeur de la classe maître
	iDecorsID(decID),
    iAnimID(anmID)
{ 	iActionType = ACTION_TYPE_DECORS_PLAYSUPANIM;
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TAction_DecorsPlaySupAnim::~TAction_DecorsPlaySupAnim()
{	}
//----------------------------------------------------------------------------//

//=== Joue une animation superposée "classique" au décors ====================//
void TAction_DecorsPlaySupAnim::PlayAction(TRunningScript &RunningAction, GameObject *pGO)
{	TAction::PlayAction(RunningAction);
	
	DecorsObjet* decors = NULL;
	
	// Récupère le ptr de l'objet du décors
	if ((iDecorsID == 0) && (pGO != NULL))
	{	// Décors exécutant le script
		decors = (DecorsObjet*) pGO;
	} else
	{	// Autre DecorsObject passé en paramètre
		decors = (DecorsObjet*) GOList->GetGameObject(iDecorsID);
	}

	// Joue l'anim superposée si elle n'est pas déjà lancée
    int index = decors->Element->GetClassikAnimIndex(iAnimID);
    if (index==-1) return;	// Anim superposable non trouvée
    if (decors->peNumClassikAnim[index] == -1)
  	{	decors->peNumClassikAnim[index] = 0;	// Début de l'anim
		// if (bEndSynchro) decors->RunningAction.iMutexID[RunningAction.iPosition];
    }	
}
//----------------------------------------------------------------------------//
//////////////// Fin de la classe TACTION_DECORS_PLAY_SUPANIM //////////////////


////////////////////////////////////////////////////////////////////////////////
// Classe TACTION_DECORS_PLAY_NEWSTATE                                        //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur  ==========================================================//
TAction_DecorsNewState::TAction_DecorsNewState(int decID, int futStateID):
   //	TAction(),	// Constructeur de la classe maître
	iDecorsID(decID),
    iFutState(futStateID)
{ 	iActionType = ACTION_TYPE_DECORS_NEWSTATE;
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TAction_DecorsNewState::~TAction_DecorsNewState()
{	}
//----------------------------------------------------------------------------//

//=== Changement de l'état actuel du décors ==================================//
void TAction_DecorsNewState::PlayAction(TRunningScript &RunningAction, GameObject *pGO)
{	TAction::PlayAction(RunningAction);	
	
	GameObject *obj = NULL;
	
	// Changement d'état du GameDecors possédant ce script
	if (iDecorsID == -1)
	{	obj = pGO;
	} else
	// Changement d'état d'un autre perso
	{	obj = GOList->GetGameObject(iDecorsID);
	}

	switch (obj->GetObjectType())
	{	case (GAMEOBJECT_PERSO):
			((Personnage*) obj)->peFutEtat = iFutState;
			if (bEndSynchro)
			{ ((Personnage*) obj)->iEndAnimSynchro = RunningAction.iMutexID[RunningAction.iPosition];
			}
			break;
		case (GAMEOBJECT_DECORS):
			((DecorsObjet*) obj)->peFutEtat = iFutState;
			if (bEndSynchro)
			{ ((DecorsObjet*) obj)->iEndAnimSynchro = RunningAction.iMutexID[RunningAction.iPosition];
			}
			break;
	}
}
//----------------------------------------------------------------------------//
//////////////// Fin de la classe TACTION_DECORS_PLAY_NEWSTATE /////////////////

////////////////////////////////////////////////////////////////////////////////
// Classe TACTION_DECORS_PLAY_FORCENEWSTATE                                   //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur  ==========================================================//
TAction_DecorsForceNewState::TAction_DecorsForceNewState(int decID, int futStateID):
	TAction_DecorsNewState(decID, futStateID)
{ 	iActionType = ACTION_TYPE_DECORS_FORCENEWSTATE;
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TAction_DecorsForceNewState::~TAction_DecorsForceNewState()
{	}
//----------------------------------------------------------------------------//

//=== Changement forcé de l'état actuel du décors ============================//
void TAction_DecorsForceNewState::PlayAction(TRunningScript &RunningAction, GameObject *pGO)
{	TAction::PlayAction(RunningAction);	
	
	GameObject *obj = NULL;
	
	// Changement d'état du GameDecors possédant ce script
	if (iDecorsID == -1)
	{	obj = pGO;
	} else
	// Changement d'état d'un autre perso
	{	obj = GOList->GetGameObject(iDecorsID);
	}

	switch (obj->GetObjectType())
	{	case (GAMEOBJECT_PERSO):
			((Personnage*) obj)->peEtat = iFutState;
			break;
		case (GAMEOBJECT_DECORS):
			((DecorsObjet*) obj)->peEtat = iFutState;
			if (bEndSynchro)
			{ ((DecorsObjet*) obj)->iEndAnimSynchro = RunningAction.iMutexID[RunningAction.iPosition];
			}
			break;
	}
}
//----------------------------------------------------------------------------//
////////////// Fin de la classe TACTION_DECORS_PLAY_FORCENEWSTATE //////////////



////////////////////////////////////////////////////////////////////////////////
//								PERSONNAGES									  //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Classe TACTION_PERSOS_CREATE												  //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeurs  =========================================================//
TAction_PersosCreate::TAction_PersosCreate(int persoType, double posX, double posY, int where, int idPerso):
	iPersoType(persoType),
    fPosX(posX), fPosY(posY)
{ 	bPosGameObject = true;
	iApparition = where;
	iPersoID = idPerso;
}
TAction_PersosCreate::TAction_PersosCreate(int persoType, int idPerso):
	iPersoType(persoType)
{ 	bPosGameObject = false;
	iApparition = -1;
	iPersoID = idPerso;
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TAction_PersosCreate::~TAction_PersosCreate()
{	}
//----------------------------------------------------------------------------//

//=== Changement de l'état actuel du décors ==================================//
void TAction_PersosCreate::PlayAction(TRunningScript &RunningAction, GameObject *pGO)
{	double posX=1.0, posY=1.0;
	Personnage *pnj = new PersoPNJ(iPersoType);

    if (!bPosGameObject)
	{	// On place le pnj à la place du GameObject ayant délcenché cette action
		if (pGO==NULL) return;
		switch (pGO->GetObjectType()) 
		{	case (GAMEOBJECT_DECORS):
				ConvNivUV2XY( ((DecorsObjet*) pGO)->PosU, ((DecorsObjet*) pGO)->PosV, posX, posY);
				pnj->SetPosition(posX, posY);
				break;
		}
	} else
	{	if (iApparition == PERSOCREATE_OFFSET)
		{	// On rajoute l'offset de déplacement
			if (pGO==NULL) return;
			switch (pGO->GetObjectType()) 
			{	case (GAMEOBJECT_DECORS):
					ConvNivUV2XY( ((DecorsObjet*) pGO)->PosU, ((DecorsObjet*) pGO)->PosV, posX, posY);
					posX += fPosX; posY += fPosY;
					break;
			}
			// On place le perso à l'endroit défini ds le script
			pnj->SetPosition(posX, posY);
		} else
		// if (iApparition == PERSOCREATE_OFFSET)
		{	// On place le perso à l'endroit défini ds le script
			pnj->SetPosition(fPosX, fPosY);
		}
	}
	
	// Attribution d'un n°ID au perso (persos importants)
	if (iPersoID !=0)
	{	pnj->SetFixedID(iPersoID);
	}

    // Attribut un script au personnage
	int scnID = GameScenario->GetNumScriptOfPerso(iPersoID);
	if (scnID != -1)
	{	pnj->SetScriptID(scnID);
	}	
	
	LGP.AddPerso(pnj);
	GOList->AddGameObject(pnj);	// Liste des GameObject  du jeu
	
	// Récupère le n°ID de ce perso pour usage ultérieur
	RunningAction.iLastGoID = pnj->GetID();		

	TAction::PlayAction(RunningAction);
}
//----------------------------------------------------------------------------//
//////////////////// Fin de la classe TACTION_PERSOS_CREATE ////////////////////

////////////////////////////////////////////////////////////////////////////////
// Classe TACTION_PERSOS_DELETE		                                          //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeurs  =========================================================//
TAction_PersosDelete::TAction_PersosDelete(int id):
	iPersoID(id)
{
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TAction_PersosDelete::~TAction_PersosDelete()
{	}
//----------------------------------------------------------------------------//

//=== Changement de l'état actuel du décors ==================================//
void TAction_PersosDelete::PlayAction(TRunningScript &RunningAction, GameObject *pGO)
{	
	// Détermine si c'est le perso qui a déclenché l'action ou un autre que l'on doit supprimer
	int id = iPersoID;
	if (iPersoID == 0)
	{	// Si c'est le perso du script
		id = pGO->GetID();
	} else
	// Sinon, ca peut être le dernier perso manipulé (créé, déplacer) du script
	if (iPersoID == -1)
	{	id = RunningAction.iLastGoID;
	} 

	LGP.DelPerso(id);
	GOList->PrepareToDelete(id);
	TAction::PlayAction(RunningAction);
}
//----------------------------------------------------------------------------//
////////////////// Fin de la classe TACTION_PERSOS_DELETE //////////////////////


////////////////////////////////////////////////////////////////////////////////
//								SCENARIO									  //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Classe ACTION_TYPE_SCENARIO_NEWLEVELMEMORY								  //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur  ==========================================================//
TAction_ScenarioNewLevelMemory::TAction_ScenarioNewLevelMemory(int memID, int levelID):
	iMemoryID(memID),
    iLevelID(levelID)
{	}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TAction_ScenarioNewLevelMemory::~TAction_ScenarioNewLevelMemory()
{	}
//----------------------------------------------------------------------------//

//=== Ajoute un élément de Mémoire au Niveau =================================//
void TAction_ScenarioNewLevelMemory::PlayAction(TRunningScript &RunningAction, GameObject *pGO)
{	GameScenario->AddLevelMemory(iMemoryID);
}
//----------------------------------------------------------------------------//
////////////// Fin de la classe ACTION_TYPE_SCENARIO_NEWLEVELMEMORY ////////////

////////////////////////////////////////////////////////////////////////////////
// Classe ACTION_TYPE_SCENARIO_DELLEVELMEMORY								  //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur  ==========================================================//
TAction_ScenarioDelLevelMemory::TAction_ScenarioDelLevelMemory(int memID, int levelID):
	iMemoryID(memID),
    iLevelID(levelID)
{	}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TAction_ScenarioDelLevelMemory::~TAction_ScenarioDelLevelMemory()
{	}
//----------------------------------------------------------------------------//

//=== Supprime un élément de Mémoire au Niveau ===============================//
void TAction_ScenarioDelLevelMemory::PlayAction(TRunningScript &RunningAction, GameObject *pGO)
{	GameScenario->DelLevelMemory(iMemoryID);
}
//----------------------------------------------------------------------------//
/////////// Fin de la classe ACTION_TYPE_SCENARIO_DELLEVELMEMORY ///////////////



////////////////////////////////////////////////////////////////////////////////
// Classe TACTION_SYNCHRO_WAITEND	                                          //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur ===========================================================//
TAction_WaitEnd::TAction_WaitEnd(int posAction):
	iPosAction(posAction)
{ }
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TAction_WaitEnd::~TAction_WaitEnd()
{ }
//----------------------------------------------------------------------------//

//=== Vérifie si l'action est finie ==========================================//
void TAction_WaitEnd::PlayAction(TRunningScript &RunningAction, GameObject *pGO)
{	// Lorsque le Mutex est repassé à 0, c'est que l'on peut continuer
	RunningAction.bWaiting =  (MemMutexLib->GetValMutex(RunningAction.iMutexID[RunningAction.iActionMutex[iPosAction]]) == 1);
	/* Modifie le 06/08/2000
	// [ACF] Problème avec le RunningAction.iPosition-1, cela suppose que le '!' doit
	//       être placé sur l'action juste avant le Sleep(0)      
	RunningAction.bWaiting =  (MemMutexLib->GetValMutex(RunningAction.iMutexID[RunningAction.iPosition-1]) == 1);
	*/
}
//----------------------------------------------------------------------------//
/////////////////// Fin de la classe TACTION_SYNCHRO_WAITEND ///////////////////

////////////////////////////////////////////////////////////////////////////////
// Classe TACTION_SYNCHRO_SLEEP		                                          //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeurs ==========================================================//
// Constructeur prenant simplement un entier
TAction_Sleep::TAction_Sleep(int nbMs)
{	riSleepTime = new RandomType(nbMs);
}
// Constructeur prenant un entier aleatoire
TAction_Sleep::TAction_Sleep(RandomType *nbMs):
	riSleepTime(nbMs)
{ }
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TAction_Sleep::~TAction_Sleep()
{	delete riSleepTime;
}
//----------------------------------------------------------------------------//


//=== Vérifie si l'action est finie ==========================================//
void TAction_Sleep::PlayAction(TRunningScript &RunningAction, GameObject *pGO)
{	// Initialisations si c'est la 1ère fois qu'on rentre ds la boucle
	if (!RunningAction.bWaiting)
	{	RunningAction.iFinalTimer = GameHistory->GetTime() + riSleepTime->random();
	}
	// Lorsque l'heure courante du jeu dépasse l'heure
	RunningAction.bWaiting = ((unsigned int) RunningAction.iFinalTimer >= GameHistory->GetTime());
}
//----------------------------------------------------------------------------//
//////////////////// Fin de la classe TACTION_SYNCHRO_SLEEP ////////////////////

