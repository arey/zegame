////////////////////////////////////////////////////////////////////////////////
//  Unité "ZoneManager.h" : interface de la classe ZoneManager				  //
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

#ifndef ZoneManagerH
#define ZoneManagerH

//---------------------------------------------------------------------- Include

//----------------------------------------------------------------- Define & enum

class MapZoneList;
class MapZone;

////////////////////////////////////////////////////////////////////////////////
// Classe ZONEMANAGER
class ZoneManager
{ // ---------------------------------------------------------- Attributs privés
  protected:
	MapZoneList	*pLevelZoneList;		// Liste des zones pouvant se trouver sur la carte
	
	unsigned int uLastPersosUpdate;		// Heure à laquelle on a vérifié si les personnages appartenaient à telle zone
	unsigned int uTimeIntPersosUpdate;	// Tps de réponse (en ms) entre 2 vérifications

	// ---------------------------------------------------------- Méthodes privées
  private:
	// Vérifie si les persos ne se trouvent pas sur une zone
	// et vérifie aussi pour chaque perso si vient tout juste de sortir d'une zone
	void TestPersosOnZones();			
									
	// Envoi un évènement à tous les objets associés à cette zone
	void SetEvent2AllObjectsOfZone(MapZone* zone, int typeInf);

  // -------------------------------------------------------- Méthodes publiques
  public:
	//--- Constructeurs & Destructeurs ---
	ZoneManager();						// Constructeur par défaut
	ZoneManager(MapZoneList *zones);	// Constructeur
	~ZoneManager();						// Destructeur

	void Update();						// MaJ tout ce qui est relatif aux zones

	inline MapZoneList* GetZoneList() { return pLevelZoneList; }
};
//====================== Fin de la classe ZONEMANAGER =========================//

extern ZoneManager *GameZoneManager;

//------------------------------------------------------------- Fonctions annexes
void InitPack_ZoneManager(MapZoneList *zones);		// Initialise l'instance du jeu
void DisablePack_ZoneManager();						// Désalloue tout
void UpdatePack_ZoneManager();						// MaJ de ZoneManager ttes les N ms


#endif	// ZoneManagerH


