////////////////////////////////////////////////////////////////////////////////
//  Unit� "ScnActions.h" impl�mentation de la classe TEvents				  //
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
// Unit� principale du projet
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
TEventsManager*  EventsManager = NULL;	// Gestionnaire d'�v�nements


////////////////////////////////////////////////////////////////////////////////
// Classe TEVENTS                                                             //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur par d�faut ================================================//
TEvent::TEvent(int id, int status, int type, int typeInf):
	iEventID(id), iEventStatus(status), iEventType(type), iEventTypeInf(typeInf) 
{	pSrcGO = NULL;
	pDestGO = NULL;
	iSource = -1;
	iSourceID = -1;
	iDestination = -1;
	iDestID = -1;
	dwTimeStart = GameHistory->GetTime();	// R�cup�re l'heure de cr�ation de cet �v�nement
	dwTimeLength = 5000;					// Dur�e max de 5s
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TEvent::~TEvent()
{ 
}
//----------------------------------------------------------------------------//

//=== Pr�cise � l'�v�nement de qui et � qui il s'adresse =====================//
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

//=== Pr�cise l�g�rement � l'�v�nement de qui et � qui il s'adresse ==========//
void TEvent::SetInfoEvent(int srcID, int destID)
{	iSourceID = srcID;
	iDestID = destID;
}
//----------------------------------------------------------------------------//

/////////////////// Fin de la classe TEVENTS ///////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Classe TEVENTSLIST                                                         //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur par d�faut ================================================//
TEventsList::TEventsList()
{	// Met tout � faux
	memset(bNewEvents, false, sizeof(bNewEvents));
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TEventsList::~TEventsList()
{	// D�truit le tableau de vector
	for (int i=0; i < SCN_TRIGGER_MAX ; i++)
	{	for (int j=0 ; j < vEvent[i].size() ; j++)
		{	delete vEvent[i][j];
			vEvent[i][j] = NULL;
		}
		vEvent[i].clear();
	}
}
//----------------------------------------------------------------------------//

//=== Rajoute un �v�nement � la liste ========================================//
void TEventsList::AddEvent(TEvent* event)
{	vEvent[event->iEventTypeInf].push_back(event);		
	bNewEvents[event->iEventTypeInf] = true;			// Un nouvel �v�nement de ce type vient d'arriver
}
//----------------------------------------------------------------------------//

//=== Supprime les nouveaux �v�nements =======================================//
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

int TEventsManager::iLastID = 1;	// ID � attribuer au prochain �v�nement

//=== Constructeur par d�faut ================================================//
TEventsManager::TEventsManager()
{
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TEventsManager::~TEventsManager()
{	// D�truit tous les �v�nements stock�s ds la map
	std::map<int, TEvent*>::iterator iter;
	for (iter = mGeneralEvents.begin(); iter!=mGeneralEvents.end() ; iter++)
    {   delete (*iter).second;
		(*iter).second = NULL;
    }       
	mGeneralEvents.clear();
}
//----------------------------------------------------------------------------//

//=== Aide � la cr�ation d'un �v�nement ======================================//
TEvent* TEventsManager::EncodeEvent(int status, int type, int typeInf)
{	return new TEvent(iLastID++, status, type, typeInf);
}
//----------------------------------------------------------------------------//

//=== Redirige cet �v�nement vers l'objet concern� ===========================//
void TEventsManager::DispatchEvent(TEvent* event)
{	// Ev�nements envoy�s directements au GameObject concern�
	if (event->iEventStatus == EVENTS_STATUS_DIRECT)
	{	// Si le ptr n'est pas encore valide, on le fait
		if (event->pDestGO == NULL)
		{ event->pDestGO = GOList->GetGameObject(event->iDestID);
		}
		event->pDestGO->AddEvent(event);		
	} else
	// Ev�nement n'ayant pas de cible particuli�re que l'on range ds la map g�n�rale
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