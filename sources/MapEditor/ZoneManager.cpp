////////////////////////////////////////////////////////////////////////////////
//  Unit� "ZoneManager.cpp" : impl�mentation de la classe ZoneManager		  //
//                                                                            //
//	Description : Gestion des zones et de leurs interractions avec les objets //
//				  du niveau.												  // 
//                                                                            //
//  Copyright (C) 2000 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com	                  //
//  Version      : beta 0.1                                                   //
//  Creation     : 23/01/2000 		                                          //
//  Last upgrade : <none>                                                     //
////////////////////////////////////////////////////////////////////////////////

#include "ZeNain.h"         //--- Special for Visual C++ 6 MFC ---
#pragma hdrstop
#include "PersoLib.h"		// Personnages et mod�les de persos du jeu
#include "PNJLib.h"			// Gestion des PNJ
#include "MapZone.h"		// Gestion des diff�rents types de zones
#include "GameObjectList.h"	// Liste de tous les GameObject du jeu
#include "ScnEvents.h"		// Gestionnaire d'�v�nements
#include "TimeClock.h"		// Gestion de l'horloge du jeu
#include "ZoneManager.h"	// Header de la classe


//----------------------------------------------------------- variables externes
ZoneManager *GameZoneManager = NULL;


////////////////////////////////////////////////////////////////////////////////
// Classe ZONEMANAGER                                                         //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur ===========================================================//
ZoneManager::ZoneManager():
	pLevelZoneList(NULL)
{
	uLastPersosUpdate = 0;
	uTimeIntPersosUpdate = 3000;	// Tests toutes les 3s
}
//----------------------------------------------------------------------------//

//=== 2nd Constructeur ===========================================================//
ZoneManager::ZoneManager(MapZoneList *zones):
	pLevelZoneList(zones)
{	
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
ZoneManager::~ZoneManager()
{
}
//----------------------------------------------------------------------------//

//=== V�rifie si les persos ne se trouvent pas sur une zone ==================//
void ZoneManager::TestPersosOnZones()
{	Personnage* perso = NULL;
	MapZone* zone = NULL;
	GameObject* go = NULL;
	TEvent*	event = NULL;
	double x, y;
	mapMapZones& zones = pLevelZoneList->GetMapZonesMap();

	// Pour chaque personnage, on v�rifie si il n'est pas dans une zone de la map
	for (int i=0 ; i < LGP.ListePersos.size() ; i++)
	{	perso = LGP.ListePersos[i];
		x = perso->PersoNivX;
		y = perso->PersoNivY;
		// On scrute toutes les zones	
		for (iterMapZones iter = zones.begin() ; iter!=zones.end() ; iter++)
		{	zone = (*iter).second;
			// Si le perso se trouve dans la zone, on alerte les objets associ�s � cette zone
			if (zone->isPointInZone(x, y))
			{	for (int j=0 ; j < zone->GetNbJointObject() ; j++)
				{	go = GOList->GetGameObject(zone->GetObjectID(j));
					event = TEventsManager::EncodeEvent(
							EVENTS_STATUS_DIRECT,		// Cet �v�nement est adress� � chaque GameObject associ� � cette zone
							EVENTS_TYPE_INTERNE,		//  "	  "		 peut �tre d�lcench� par le H�ro comme par tout autre personnage
							SCN_TRIGGER_PERSO_DSZONE	// Personnage qui se trouve dans telle zone
							);
					event->SetInfoEvent(NULL, go);
					EventsManager->DispatchEvent(event);
				}
			}
		}
	}
}
//----------------------------------------------------------------------------//

//=== MaJ tout ce qui est relatif aux zones ==================================//
void ZoneManager::Update()
{	unsigned int curentTime = GameHistory->GetTime();

	// V�rifie si il est l'heure de v�rifier si les persos sont dans telle zone
	if ((curentTime - uLastPersosUpdate) > uTimeIntPersosUpdate)
	{	uLastPersosUpdate = curentTime;
		TestPersosOnZones();		
	}		
}
//----------------------------------------------------------------------------//

/////////////////////////////// Fin de ZONEMANAGER /////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Fonctions EXTERNES														  //		
////////////////////////////////////////////////////////////////////////////////

//=== Initialise l'instance du jeu ===========================================//
void InitPack_ZoneManager(MapZoneList *zones)
{	GameZoneManager = new ZoneManager(zones);
}
//----------------------------------------------------------------------------//

//=== D�salloue tout =========================================================//
void DisablePack_ZoneManager()
{	delete GameZoneManager;
	GameZoneManager = NULL;
}
//----------------------------------------------------------------------------//

//=== MaJ de ZoneManager ttes les N ms =======================================//
void UpdatePack_ZoneManager()
{	GameZoneManager->Update();
}
//----------------------------------------------------------------------------//
