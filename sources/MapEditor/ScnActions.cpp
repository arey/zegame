////////////////////////////////////////////////////////////////////////////////
//  Unit� "ScnActions.cpp" impl�mentation de la classe TAction 				  //
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
// Unit� principale du projet
#ifdef MAPEDITOR
#include "uMapView.h"
#else
#include "DirectLib.h"
#endif  // MAPEDITOR
#endif
#include "ImDecors2.h"		// Gestion des d�cors
#include "PersoLib.h"		// Gestion des personnages
#include "GameObject.h"		// Gestion des objets du jeu
#include "GameObjectList.h"	// Liste des GameObject du jeu
#include "PNJLib.h"			// Gestion des PNJ
#include "MemLib.h"			// Lib. d'outils de communications
#include "ScnScript.h"		// Librairie des Scripts du jeu
#include "TimeClock.h"		// Lib. de gestion temporelle du jeu
#include "WorldSimulation.h"// Param�trages des lois physiques du monde
#include "Animall.h"
#pragma hdrstop
#include "ScnConsts.h"		// Constantes li�es � la gestion du sc�nario
#include "ScnActions.h"		// Header du fichier

////////////////////////////////////////////////////////////////////////////////
// Classe TACTION                                                             //
////////////////////////////////////////////////////////////////////////////////

int ZG::TAction::iLastID = 0;		// N�ID � attribuer � la prochaine action

//=== Constructeur par d�faut ================================================//
ZG::TAction::TAction()
{	iActionID = iLastID++;				// Attribution d'un n�ID libre
	iActionType = ACTION_TYPE_NONE;     // Type d'action ind�fini
	bEndSynchro = false;				// Aucune synchronisation, par d�faut
}
//----------------------------------------------------------------------------//

//=== Destructeur par d�faut =================================================//
ZG::TAction::~TAction()
{
}
//----------------------------------------------------------------------------//

//=== Effectue l'action, quel que soit son type ==============================//
void ZG::TAction::PlayAction(TRunningScript &RunningAction, GameObject *pGO)
{	// Cr�e le S�maphore de synchronisation
	if (bEndSynchro)
	{	RunningAction.iMutexID[RunningAction.iPosition] = MemMutexLib->NewMutex();
	}
}
//----------------------------------------------------------------------------//


/////////////////// Fin de la classe TACTION ///////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Classe TACTION_GO_NEWMEMORY												  //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur  ==========================================================//
TAction_GONewMemory::TAction_GONewMemory(int memID, int goID):
	iMemoryID(memID),
    iGoID(goID)
{	}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TAction_GONewMemory::~TAction_GONewMemory()
{	}
//----------------------------------------------------------------------------//

//=== Ajoute un �l�ment de M�moire au GameObject =============================//
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
{	}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TAction_GODelMemory::~TAction_GODelMemory()
{	}
//----------------------------------------------------------------------------//

//=== Supprime un �l�ment de M�moire au GameObject ===========================//
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
// Classe TACTION_DECORS_PLAY_SUPANIM                                         //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur  ==========================================================//
TAction_DecorsPlaySupAnim::TAction_DecorsPlaySupAnim(int decID, int anmID):
   //	TAction(),	// Constructeur de la classe ma�tre
	iDecorsID(decID),
    iAnimID(anmID)
{ 	iActionType = ACTION_TYPE_DECORS_PLAYSUPANIM;
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TAction_DecorsPlaySupAnim::~TAction_DecorsPlaySupAnim()
{	}
//----------------------------------------------------------------------------//

//=== Joue une animation superpos�e "classique" au d�cors ====================//
void TAction_DecorsPlaySupAnim::PlayAction(TRunningScript &RunningAction, GameObject *pGO)
{	ZG::TAction::PlayAction(RunningAction);
	
	DecorsObjet* decors = NULL;
	
	// R�cup�re le ptr de l'objet du d�cors
	if ((iDecorsID == 0) && (pGO != NULL))
	{	// D�cors ex�cutant le script
		decors = (DecorsObjet*) pGO;
	} else
	{	// Autre DecorsObject pass� en param�tre
		decors = (DecorsObjet*) GOList->GetGameObject(iDecorsID);
	}

	// Joue l'anim superpos�e si elle n'est pas d�j� lanc�e
    int index = decors->Element->GetClassikAnimIndex(iAnimID);
    if (index==-1) return;	// Anim superposable non trouv�e
    if (decors->peNumClassikAnim[index] == -1)
  	{	decors->peNumClassikAnim[index] = 0;	// D�but de l'anim
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
   //	TAction(),	// Constructeur de la classe ma�tre
	iDecorsID(decID),
    iFutState(futStateID)
{ 	iActionType = ACTION_TYPE_DECORS_NEWSTATE;
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TAction_DecorsNewState::~TAction_DecorsNewState()
{	}
//----------------------------------------------------------------------------//

//=== Changement de l'�tat actuel du d�cors ==================================//
void TAction_DecorsNewState::PlayAction(TRunningScript &RunningAction, GameObject *pGO)
{	ZG::TAction::PlayAction(RunningAction);	
	
	GameObject *obj = NULL;
	
	// Changement d'�tat du GameDecors poss�dant ce script
	if (iDecorsID == 0)
	{	obj = pGO;
	} else
	// Changement d'�tat d'un autre perso
	{	obj = GOList->GetGameObject(iDecorsID);
	}

	switch (obj->GetObjectType())
	{	case (GAMEOBJECT_PERSO):
			((Personnage*) obj)->peFutEtat = iFutState;
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
// Classe TACTION_PERSOS_CREATE                                        //
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

//=== Changement de l'�tat actuel du d�cors ==================================//
void TAction_PersosCreate::PlayAction(TRunningScript &RunningAction, GameObject *pGO)
{	double posX=1.0, posY=1.0;
	Personnage *pnj = new PersoPNJ(iPersoType);

    if (!bPosGameObject)
	{	// On place le pnj � la place du GameObject ayant d�lcench� cette action
		if (pGO==NULL) return;
		switch (pGO->GetObjectType()) 
		{	case (GAMEOBJECT_DECORS):
				ConvNivUV2XY( ((DecorsObjet*) pGO)->PosU, ((DecorsObjet*) pGO)->PosV, posX, posY);
				pnj->SetPosition(posX, posY);
				break;
		}
	} else
	{	if (iApparition == PERSOCREATE_OFFSET)
		{	// On rajoute l'offset de d�placement
			if (pGO==NULL) return;
			switch (pGO->GetObjectType()) 
			{	case (GAMEOBJECT_DECORS):
					ConvNivUV2XY( ((DecorsObjet*) pGO)->PosU, ((DecorsObjet*) pGO)->PosV, posX, posY);
					posX += fPosX; posY += fPosY;
					break;
			}
			// On place le perso � l'endroit d�fini ds le script
			pnj->SetPosition(posX, posY);
		} else
		// if (iApparition == PERSOCREATE_OFFSET)
		{	// On place le perso � l'endroit d�fini ds le script
			pnj->SetPosition(fPosX, fPosY);
		}
	}
	
	// Attribution d'un n�ID au perso (persos importants)
	if (iPersoID !=0)
	{	pnj->SetFixedID(iPersoID);
	}

    LGP.AddPerso(pnj);
	GOList->AddGameObject(pnj);	// Liste des GameObject  du jeu
	
	// R�cup�re le n�ID de ce perso pour usage ult�rieur
	RunningAction.iLastGoID = pnj->GetID();		

	ZG::TAction::PlayAction(RunningAction);
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

//=== Changement de l'�tat actuel du d�cors ==================================//
void TAction_PersosDelete::PlayAction(TRunningScript &RunningAction, GameObject *pGO)
{
	LGP.DelPerso(iPersoID);
	GOList->PrepareToDelete(iPersoID);
	//GOList->DelGameObject(iPersoID);
	ZG::TAction::PlayAction(RunningAction);
}
//----------------------------------------------------------------------------//
////////////////// Fin de la classe TACTION_PERSOS_DELETE //////////////////////


////////////////////////////////////////////////////////////////////////////////
// Classe TACTION_PERSOS_MOVE		                                          //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur ===========================================================//
TAction_PersosMove::TAction_PersosMove(int moveType, double posX, double posY, int persoID):
	iPersoID(persoID),
	iMoveType(moveType)
{	switch (iMoveType)
	{	case (ACTION_PERSOSMOVE_DESTINATION) :
			fPosX = posX; fPosY = posY; 
			break;
		case (ACTION_PERSOSMOVE_OFFSET) :
			fDepX = posX; fDepY = posY; 
			break;
	}
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TAction_PersosMove::~TAction_PersosMove()
{	}
//----------------------------------------------------------------------------//

//=== Changement de l'�tat actuel du d�cors ==================================//
void TAction_PersosMove::PlayAction(TRunningScript &RunningAction, GameObject *pGO)
{	ZG::TAction::PlayAction(RunningAction);

	PersoPNJ* perso = NULL;
	double destU=0, destV=0;
	
	// D�termine si c'est le perso qui a d�clench� l'action ou un autre 
	// qui doit se d�placer
	if (iPersoID == 0)	
	{	// Si c'est le perso du script
		perso = (PersoPNJ*) pGO;
	} else
	// Sinon, ca peut �tre le dernier perso manipul� (cr��, d�placer) du script
	if (iPersoID == -1)
	{	perso = (PersoPNJ*) GOList->GetGameObject(RunningAction.iLastGoID);
	} else
	{	perso = (PersoPNJ*) GOList->GetGameObject(iPersoID);
	}
	
	// R�cup�re les coordonn�es (U,V) o� il doit se rendre
	switch (iMoveType)
	{	case (ACTION_PERSOSMOVE_DESTINATION) :
			destU = fPosX / WorldModel.SQR_LENGTH; 
			destV = fPosY / WorldModel.SQR_LENGTH;
			break;
		case (ACTION_PERSOSMOVE_OFFSET) :
			destU = perso->PersoNivU + fDepX / WorldModel.SQR_LENGTH;
			destV = perso->PersoNivV + fDepY / WorldModel.SQR_LENGTH;
			break;
	}
		
	// D�place le PNJ
	perso->MoveTo((int) destU,(int) destV);
}
//----------------------------------------------------------------------------//
////////////////////// Fin de la classe TACTION_PERSOS_MOVE ////////////////////



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

//=== V�rifie si l'action est finie ==========================================//
void TAction_WaitEnd::PlayAction(TRunningScript &RunningAction, GameObject *pGO)
{	// Lorsque le Mutex est repass� � 0, c'est que l'on peut continuer
	RunningAction.bWaiting =  (MemMutexLib->GetValMutex(RunningAction.iMutexID[iPosAction]) == 1);
}
//----------------------------------------------------------------------------//
/////////////////// Fin de la classe TACTION_SYNCHRO_WAITEND ///////////////////

////////////////////////////////////////////////////////////////////////////////
// Classe TACTION_SYNCHRO_SLEEP		                                          //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur ===========================================================//
TAction_Sleep::TAction_Sleep(int nbMs):
	iNbSleepTime(nbMs)
{ }
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TAction_Sleep::~TAction_Sleep()
{ }
//----------------------------------------------------------------------------//

//=== V�rifie si l'action est finie ==========================================//
void TAction_Sleep::PlayAction(TRunningScript &RunningAction, GameObject *pGO)
{	// Initialisations si c'est la 1�re fois qu'on rentre ds la boucle
	if (!RunningAction.bWaiting)
	{	RunningAction.iFinalTimer = GameHistory->GetTime() + iNbSleepTime;
	}
	// Lorsque l'heure courante du jeu d�passe l'heure
	RunningAction.bWaiting = ((unsigned int) RunningAction.iFinalTimer >= GameHistory->GetTime());
}
//----------------------------------------------------------------------------//
//////////////////// Fin de la classe TACTION_SYNCHRO_SLEEP ////////////////////

