////////////////////////////////////////////////////////////////////////////////
//  Unité "ScnCauses.h"  : interface de la classe TCause 					  //
//                                                                            //
//	Description :                                                             //
//                                                                            //
//  Copyright (C) 1999 Antoine Rey    arey@insa-lyon.fr                       //
//                                    antrey@club-internet.fr                 //
//  Version      : beta 0.20                                                  //
//  Creation     : 17/07/1999 		                                          //
//  Last upgrade : <none>	                                                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef ScnCausesH
#define ScnCausesH

//---------------------------------------------------------------------- Include
#include <vector>		// librairie STL de gestion de tableaux dynamiques
#include "ScnConsts.h"	// Constantes & define propres à la gestion du scénario

class TScriptManager;	// Structure de suivit des scripts
class Personnage;		// Personnage (PNJ, Monstre ou Héros)
class MapZone;			// Zones au sol	

// TCAUSE //////////////////////////////////////////////////////////////////////
// Conditions à réunir pour que les actions associées à cette ligne de script
// puissent être éxécutées.
class TCause
{ protected:
	int iCauseID;			// N° de cause unique
	int iCauseType;			// Type de cause
    static int iLastID;		// N°ID à attribuer à la prochaine cause
	
  public:
    TCause();						// Constructeur par défaut
    virtual ~TCause();				// Destructeur
    inline int GetTrigger();		// Renvoie le facteur déclencheur "iTrigger"
	
	virtual bool isCauseRealized(TScriptManager* SptMgr = NULL) = 0; // Indique si cette cause est vérifiée
};
////////////////////////// Fin de TCAUSE ///////////////////////////////////////


// TCAUSE_STRIKE_BY ////////////////////////////////////////////////////////////
// Cet Objet/Perso/Décors a été frappé par iPersoID
class TCause_StrikeBy : public TCause
{ 	int iThingID;			// ID du Objet/Perso/Décors originaire du coup

  public:
  	TCause_StrikeBy(int thingID);	// Constructeur
    ~TCause_StrikeBy();				// Destructeur
    bool isCauseRealized(TScriptManager* SptMgr = NULL);	// Indique si cette cause est vérifiée
};
///////////////////////// Fin de TCAUSE_STRIKE_BY //////////////////////////////

// TCAUSE_STRIKE_WITH ////////////////////////////////////////////////////////////
// Cet Objet/Perso/Décors a été frappé avec tel arme/objet
class TCause_StrikeWith : public TCause
{ 	int iThingID;			// ID de l'Arme/Objet avec lequel on a frappé

  public:
  	TCause_StrikeWith(int thingID);	// Constructeur
    ~TCause_StrikeWith();			// Destructeur
    virtual bool isCauseRealized(TScriptManager* SptMgr = NULL);	// Indique si cette cause est vérifiée
};
///////////////////////// Fin de TCAUSE_STRIKE_WITH //////////////////////////////

// TCAUSE_MEMORY_IN //////////////////////////////////////////////////////////////
// Cet Objet/Perso/Décors connait tel chose
class TCause_MemoryIn : public TCause
{ protected:
	int iMemoryID;			// Mémoire que l'on veut qu'il connaisse
  public:
  	TCause_MemoryIn(int thingID);	// Constructeur
    virtual ~TCause_MemoryIn();		// Destructeur
    virtual bool isCauseRealized(TScriptManager* SptMgr = NULL);	// Indique si cette cause est vérifiée
};
////////////////////////// Fin de TCAUSE_MEMORY_IN ///////////////////////////////

// TCAUSE_MEMORY_NO //////////////////////////////////////////////////////////////
// Cet Objet/Perso/Décors NE connait PAS tel chose
class TCause_MemoryNo : public TCause
{ protected:
	int iMemoryID;			// Mémoire que l'on veut qu'il ne sâche pas
  public:
  	TCause_MemoryNo(int thingID);	// Constructeur
    virtual ~TCause_MemoryNo();		// Destructeur
    bool isCauseRealized(TScriptManager* SptMgr = NULL);	// Indique si cette cause est vérifiée
};
////////////////////////// Fin de TCAUSE_MEMORY_NO ///////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
//								SCENARIO								  	    //
//////////////////////////////////////////////////////////////////////////////////

// TCAUSE_LEVEL_MEMORY_IN ////////////////////////////////////////////////////////
// Il est connu dans ce niveau telle chose
class TCause_LevelMemoryIn : public TCause_MemoryIn
{ public:
	TCause_LevelMemoryIn(int thingID);	// Constructeur
    virtual ~TCause_LevelMemoryIn();	// Destructeur	
	bool isCauseRealized(TScriptManager* SptMgr = NULL);	// Indique si cette cause est vérifiée
};
/////////////////////// Fin de TCAUSE_LEVEL_MEMORY_IN ////////////////////////////

// TCAUSE_LEVEL_MEMORY_NO ////////////////////////////////////////////////////////
// Dans ce niveau, on NE connait PAS telle chose
class TCause_LevelMemoryNo : public TCause_MemoryNo
{  public:
	TCause_LevelMemoryNo(int thingID);	// Constructeur
    virtual ~TCause_LevelMemoryNo();	// Destructeur	
	bool isCauseRealized(TScriptManager* SptMgr = NULL);	// Indique si cette cause est vérifiée
};
/////////////////////// Fin de TCAUSE_LEVEL_MEMORY_NO ////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//	 							  PERSOS									    //
//////////////////////////////////////////////////////////////////////////////////

// CAUSE_TYPE_PERSO_ZONE_IN //////////////////////////////////////////////////////
// Vérifie si un personnage est dans telle zone
class TCause_CharactZoneIn : public TCause
{ protected: 	
	int iCharID;		// N° du personnage
	Personnage* perso;	// Ptr vers ce personnage
	int iZoneID;		// N° de la zone
	MapZone* zone;		// Ptr vers cette zone
  public:
  	TCause_CharactZoneIn(int charID, int zoneID);		// Constructeur
    virtual ~TCause_CharactZoneIn();					// Destructeur
    virtual bool isCauseRealized(TScriptManager* SptMgr = NULL);	// Indique si cette cause est vérifiée
};
/////////////////////// Fin de CAUSE_TYPE_PERSO_ZONE_IN //////////////////////////

// CAUSE_TYPE_HERO_ZONE_IN ///////////////////////////////////////////////////////
// Vérifie si le héro est dans telle zone
class TCause_HeroZoneIn : public TCause_CharactZoneIn
{ public:
	TCause_HeroZoneIn(int zoneID):			// Constructeur
			TCause_CharactZoneIn(0, zoneID) { };
};
/////////////////////// Fin de CAUSE_TYPE_HERO_ZONE_IN ///////////////////////////

// CAUSE_TYPE_PERSO_ZONE_OUT /////////////////////////////////////////////////////
// Vérifie si tel personnage est à l'extérieur de telle zone
class TCause_CharactZoneOut : public TCause_CharactZoneIn
{ public:
	TCause_CharactZoneOut(int charID, int zoneID):				// Constructeur
			TCause_CharactZoneIn(charID, zoneID) { };
	virtual bool isCauseRealized(TScriptManager* SptMgr = NULL) // Indique si cette cause est vérifiée
	{	return !TCause_CharactZoneIn::isCauseRealized(SptMgr);
	}	
};
///////////////////// Fin de CAUSE_TYPE_PERSO_ZONE_OUT ///////////////////////////

// CAUSE_TYPE_HERO_ZONE_OUT //////////////////////////////////////////////////////
// Vérifie si le héro est à l'extérieur de telle zone
class TCause_HeroZoneOut : public TCause_CharactZoneOut
{ public:
	TCause_HeroZoneOut(int zoneID):								// Constructeur
			TCause_CharactZoneOut(0, zoneID) { };
};
////////////////////// Fin de CAUSE_TYPE_HERO_ZONE_OUT ///////////////////////////


//////////////////////////////////////////////////////////////////////////////////
//	 							  SYNCHRO									    //
//////////////////////////////////////////////////////////////////////////////////

// TCAUSE_RANDOM /////////////////////////////////////////////////////////////////
// Cause réalisée aléatoirement tous les N fois
class TCause_Random : public TCause
{ 	int iNumber;			// Nombre aléatoire

  public:
  	TCause_Random(int numb);		// Constructeur
    ~TCause_Random();				// Destructeur
    bool isCauseRealized(TScriptManager* SptMgr = NULL);	// Indique si cette cause est vérifiée
};
//////////////////////////// Fin de TCAUSE_RANDOM ////////////////////////////////

// TCAUSE_TIMER //////////////////////////////////////////////////////////////////
// Horloge déclenchée au bout de N ms
class TCause_Timer : public TCause
{ 	int iNbMs;			// Nombre de ms

  public:
  	TCause_Timer(int numb);			// Constructeur
    ~TCause_Timer();				// Destructeur
    bool isCauseRealized(TScriptManager* SptMgr = NULL);	// Indique si cette cause est vérifiée
};
//////////////////////////// Fin de TCAUSE_TIMER /////////////////////////////////



#endif
