////////////////////////////////////////////////////////////////////////////////
//  Unit� "ZoneManager.h" : interface de la classe ZoneManager				  //
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
{ // ---------------------------------------------------------- Attributs priv�s
  protected:
	MapZoneList	*pLevelZoneList;		// Liste des zones pouvant se trouver sur la carte
	
	unsigned int uLastPersosUpdate;		// Heure � laquelle on a v�rifi� si les personnages appartenaient � telle zone
	unsigned int uTimeIntPersosUpdate;	// Tps de r�ponse (en ms) entre 2 v�rifications

	// ---------------------------------------------------------- M�thodes priv�es
  private:
	// V�rifie si les persos ne se trouvent pas sur une zone
	// et v�rifie aussi pour chaque perso si vient tout juste de sortir d'une zone
	void TestPersosOnZones();			
									
	// Envoi un �v�nement � tous les objets associ�s � cette zone
	void SetEvent2AllObjectsOfZone(MapZone* zone, int typeInf);

  // -------------------------------------------------------- M�thodes publiques
  public:
	//--- Constructeurs & Destructeurs ---
	ZoneManager();						// Constructeur par d�faut
	ZoneManager(MapZoneList *zones);	// Constructeur
	~ZoneManager();						// Destructeur

	void Update();						// MaJ tout ce qui est relatif aux zones

	inline MapZoneList* GetZoneList() { return pLevelZoneList; }
};
//====================== Fin de la classe ZONEMANAGER =========================//

extern ZoneManager *GameZoneManager;

//------------------------------------------------------------- Fonctions annexes
void InitPack_ZoneManager(MapZoneList *zones);		// Initialise l'instance du jeu
void DisablePack_ZoneManager();						// D�salloue tout
void UpdatePack_ZoneManager();						// MaJ de ZoneManager ttes les N ms


#endif	// ZoneManagerH


