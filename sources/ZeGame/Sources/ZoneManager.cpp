////////////////////////////////////////////////////////////////////////////////
//  Unité "ZoneManager.cpp" : implémentation de la classe ZoneManager		  //
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
#include "PersoLib.h"		// Personnages et modèles de persos du jeu
#include "PNJLib.h"			// Gestion des PNJ
#include "MapZone.h"		// Gestion des différents types de zones
#include "GameObjectList.h"	// Liste de tous les GameObject du jeu
#include "ScnEvents.h"		// Gestionnaire d'évènements
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
{	uLastPersosUpdate = 0;
	uTimeIntPersosUpdate = 3000;	// Tests toutes les 3s
}
//----------------------------------------------------------------------------//

//=== 2nd Constructeur ===========================================================//
ZoneManager::ZoneManager(MapZoneList *zones):
	pLevelZoneList(zones)
{	uLastPersosUpdate = 0;
	uTimeIntPersosUpdate = 3000;	// Tests toutes les 3s
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
ZoneManager::~ZoneManager()
{
}
//----------------------------------------------------------------------------//

struct ZoneManagerTestZone
{	int zoneid;
	bool current;
	ZoneManagerTestZone(int id):zoneid(id), current(false) { };			
};

//=== Vérifie si les persos ne se trouvent pas sur une zone ==================//
// et vérifie aussi pour chaque perso si vient tout juste de sortir d'une zone
void ZoneManager::TestPersosOnZones()
{	Personnage* perso = NULL;
	MapZone* zone = NULL;
	double x, y;
	mapMapZones& zones = pLevelZoneList->GetMapZonesMap();
	std::vector<ZoneManagerTestZone> vIsOldZone;
	std::list<int>::iterator iter;
	
	// Pour chaque personnage, on vérifie si il n'est pas dans une zone de la map
	for (int i=0 ; i < LGP.getCharacterCount() ; i++)
	{	perso = LGP.getPersoAtIndex(i);
		if (perso != NULL)
		{	int numZone = 0;
			x = perso->PersoNivX;
			y = perso->PersoNivY;

			// Prépare la liste contenant les n° des anciennes zones où se trouvaient
			// ce personnage avant l'étape suivante
			std::list<int>&	lInsideZone = perso->GetListInsideZone();
			vIsOldZone.clear();
			for (iter = lInsideZone.begin() ; iter != lInsideZone.end() ; iter++)
			{	vIsOldZone.push_back(ZoneManagerTestZone((*iter)));
			}

			// On scrute toutes les zones	
			for (iterMapZones iter = zones.begin() ; iter!=zones.end() ; iter++)
			{	zone = (*iter).second;
				numZone = 0;
				// Si le perso se trouve dans la zone, 
				if (zone->isPointInZone(x, y))
				{	// a) on alerte les objets associés à cette zone
					int id = zone->GetID();
					SetEvent2AllObjectsOfZone(zone, SCN_TRIGGER_PERSO_DSZONE);	// Personnage qui se trouve dans telle zone
				
					// b) On prévoit le déclenchement des SCN_TRIGGER_PERSO_ZONEOUT
					if (perso->isInInsideZone(id))
					{	// Zone inchangée
						while (vIsOldZone[numZone].zoneid != id) numZone++;
						vIsOldZone[numZone++].current = true;
					} else
					// c) et là, c'est celui des SCN_TRIGGER_PERSO_ZONEIN
					{	// on alerte les objets associés à cette zone que qqn vient de s'en aller
						SetEvent2AllObjectsOfZone(zone, SCN_TRIGGER_PERSO_ZONEIN);	// Personnage qui se trouve dans telle zone
						// on la rajoute à la liste des zones dans lesquelles se trouve le perso
						perso->AddInsideZone(id);
					}
				}				
			}

			// Puis on prévient lorsque le personnage vient de quitter une zone
			for (int v=0 ; v < vIsOldZone.size() ; v++)
			{	if (!vIsOldZone[v].current)
				{	perso->DelInsideZone(vIsOldZone[v].zoneid);
					// on alerte les objets associés à cette zone que qqn vient de s'en aller
					SetEvent2AllObjectsOfZone(pLevelZoneList->GetZone(vIsOldZone[v].zoneid), SCN_TRIGGER_PERSO_ZONEOUT);	// Personnage qui se trouve dans telle zone
				}
			}
		}
	}
}
//----------------------------------------------------------------------------//

//=== MaJ tout ce qui est relatif aux zones ==================================//
void ZoneManager::Update()
{	unsigned int curentTime = GameHistory->GetTime();

	// [BUG] Bug qui traine peut-être par là
	//if (CFG_GAME_RELEASE == false)
	{
		// Vérifie si il est l'heure de vérifier si les persos sont dans telle zone
		if ((curentTime - uLastPersosUpdate) > uTimeIntPersosUpdate)
		{	uLastPersosUpdate = curentTime;
			TestPersosOnZones();		
		}		
	}
}
//----------------------------------------------------------------------------//

//=== Envoi un évènement à tous les objets associés à cette zone =============//
void ZoneManager::SetEvent2AllObjectsOfZone(MapZone* zone, int typeInf)
{	GameObject* go = NULL;
	TEvent*	event = NULL;
	for (int j=0 ; j < zone->GetNbJointObject() ; j++)
	{	go = GOList->GetGameObject(zone->GetObjectID(j));
		event = TEventsManager::EncodeEvent(
				EVENTS_STATUS_DIRECT,		// Cet évènement est adressé à chaque GameObject associé à cette zone
				EVENTS_TYPE_INTERNE,		//  "	  "		 peut être déclenché par le Héro comme par tout autre personnage
				typeInf						// Type d'évènement : rentre/sort/est dans une zone	
				);
		event->SetInfoEvent(NULL, go);
		EventsManager->DispatchEvent(event);
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

//=== Désalloue tout =========================================================//
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
