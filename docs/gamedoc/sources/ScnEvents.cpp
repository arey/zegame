////////////////////////////////////////////////////////////////////////////////
//  Unité "ScnActions.h" implémentation de la classe TEvents				  //
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
#include "ZeNain.h"         //--- Special for Visual C++ 6 MFC ---
#ifndef MICROSOFT
#ifdef MAPEDITOR
// Unité principale du projet
#include "umapView.h"
#else   
#include "DirectLib.h"
#endif  // MapEditor
#endif  // Microsoft
#pragma hdrstop
#include "GameObject.h"		// Gestion des objets du jeu
#include "GameObjectList.h"	// Liste des GameObject du jeu
#include "TimeClock.h"		// Lib. de gestion temporelle du jeu
#include "ScnEvents.h"		// Header du fichier


// ---------------------------------------------------------- variables externes
TEventsManager*  EventsManager = NULL;	// Gestionnaire d'évènements


////////////////////////////////////////////////////////////////////////////////
// Classe TEVENTS                                                             //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur par défaut ================================================//
TEvent::TEvent(int id, int status, int type, int typeInf):
	iEventID(id), iEventStatus(status), iEventType(type), iEventTypeInf(typeInf) 
{	pSrcGO = NULL;
	pDestGO = NULL;
	iSource = -1;
	iSourceID = -1;
	iDestination = -1;
	iDestID = -1;
	dwTimeStart = GameHistory->GetTime();	// Récupère l'heure de création de cet évènement
	dwTimeLength = 5000;					// Durée max de 5s
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TEvent::~TEvent()
{ 
}
//----------------------------------------------------------------------------//

//=== Précise à l'évènement de qui et à qui il s'adresse =====================//
void TEvent::SetInfoEvent(GameObject *src, GameObject *dest)
{	if (src != NULL)
	{	pSrcGO = src;
		iSourceID = src->GetID();
		iSource = src->GetObjectType();
	}
	if (dest != NULL)
	{	pDestGO = dest;
		iDestID = dest->GetID();
	}
}
//----------------------------------------------------------------------------//

//=== Précise légèrement à l'évènement de qui et à qui il s'adresse ==========//
void TEvent::SetInfoEvent(int srcID, int destID)
{	iSourceID = srcID;
	iDestID = destID;
}
//----------------------------------------------------------------------------//

/////////////////// Fin de la classe TEVENTS ///////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Classe TEVENTSLIST                                                         //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur par défaut ================================================//
TEventsList::TEventsList()
{	// Met tout à faux
	memset(bNewEvents, false, sizeof(bNewEvents));
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TEventsList::~TEventsList()
{	// Détruit le tableau de vector
	for (int i=0; i < SCN_TRIGGER_MAX ; i++)
	{	for (int j=0 ; j < vEvent[i].size() ; j++)
		{	delete vEvent[i][j];
			vEvent[i][j] = NULL;
		}
		vEvent[i].clear();
	}
}
//----------------------------------------------------------------------------//

//=== Rajoute un évènement à la liste ========================================//
void TEventsList::AddEvent(TEvent* event)
{	vEvent[event->iEventTypeInf].push_back(event);		
	bNewEvents[event->iEventTypeInf] = true;			// Un nouvel évènement de ce type vient d'arriver
}
//----------------------------------------------------------------------------//

//=== Supprime les nouveaux évènements =======================================//
void TEventsList::ClearNewEvents()
{	
	for (int i=0; i < SCN_TRIGGER_MAX ; i++)
	{	if (bNewEvents[i])
		{	for (int j=0 ; j < vEvent[i].size() ; j++)
			{	delete vEvent[i][j];
				vEvent[i][j] = NULL;
			}
			vEvent[i].clear();
		}		
	}
	memset(bNewEvents, false, SCN_TRIGGER_MAX);
}
//----------------------------------------------------------------------------//

//////////////////// Fin de la classe TEVENTSLIST //////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Classe TEVENTSMANAGER                                                      //
////////////////////////////////////////////////////////////////////////////////

int TEventsManager::iLastID = 1;	// ID à attribuer au prochain évènement

//=== Constructeur par défaut ================================================//
TEventsManager::TEventsManager()
{
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TEventsManager::~TEventsManager()
{	// Détruit tous les évènements stockés ds la map
	std::map<int, TEvent*>::iterator iter;
	for (iter = mGeneralEvents.begin(); iter!=mGeneralEvents.end() ; iter++)
    {   delete (*iter).second;
		(*iter).second = NULL;
    }       
	mGeneralEvents.clear();
}
//----------------------------------------------------------------------------//

//=== Aide à la création d'un évènement ======================================//
TEvent* TEventsManager::EncodeEvent(int status, int type, int typeInf)
{	return new TEvent(iLastID++, status, type, typeInf);
}
//----------------------------------------------------------------------------//

//=== Redirige cet évènement vers l'objet concerné ===========================//
void TEventsManager::DispatchEvent(TEvent* event)
{	// Evènements envoyés directements au GameObject concerné
	if (event->iEventStatus == EVENTS_STATUS_DIRECT)
	{	// Si le ptr n'est pas encore valide, on le fait
		if (event->pDestGO == NULL)
		{ event->pDestGO = GOList->GetGameObject(event->iDestID);
		}
		event->pDestGO->AddEvent(event);		
	} else
	// Evènement n'ayant pas de cible particulière que l'on range ds la map générale
	{	mGeneralEvents.insert(std::make_pair(event->iEventID, event));
	}
}
//----------------------------------------------------------------------------//

//////////////////// Fin de la classe TEVENTSMANAGER ///////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Fonctions ANNEXES														  //	
////////////////////////////////////////////////////////////////////////////////

//=== Initialisation de l'EventsManager ======================================//
void InitGame_EventsManager()
{	EventsManager = new TEventsManager();
}
//----------------------------------------------------------------------------//

//=== Fermetrue de l'EventsManager ===========================================//
void CloseGame_EventsManager()
{	delete EventsManager;
	EventsManager = NULL;
}
//----------------------------------------------------------------------------//