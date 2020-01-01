////////////////////////////////////////////////////////////////////////////////
//  Unité "ScnEvents.h" interface de la classe TEvents						  //
//                                                                            //
//	Description :                                                             //
//                                                                            //
//  Copyright (C) 1999 Antoine Rey    arey@insa-lyon.fr                       //
//                                    antrey@club-internet.fr                 //
//  Version      : beta 0.20                                                  //
//  Creation     : 14/07/1999 		                                          //
//  Last upgrade : <none>	                                                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef ScnEventsH
#define ScnEventsH

//---------------------------------------------------------------------- Include
#ifdef MICROSOFT
#pragma warning(disable : 4100 4663 4018)  
#endif
#include <vector>		// librairie STL de gestion de tableaux dynamiques
#include <map>			// librairie STL de gestion de dictionnaires à clé unique
#include "ScnConsts.h"	// Constantes & define propres à la gestion du scénario


//----------------------------------------------------------------------- Define

// TEvents : indication sur ce qui a déclenché l'évènement
#define	EVENTS_SOURCE_OBJET			1
#define	EVENTS_SOURCE_SCENARIO		2
#define	EVENTS_SOURCE_JOUEUR		4

// TEvents : statut de l'évènement auquel on a affaire
#define EVENTS_STATUS_DIRECT		1		// l'évènement ne s'adresse qu'à un seul destinataire
#define EVENTS_STATUS_INDIRECT		2		// l'évènement peut affecter plusieurs persos/objets/décors

// TEvents : type de l'évènement auquel on a affaire -> indication sur ce qui a déclenché l'évènement
#define EVENTS_TYPE_JOUEUR 			1		// c'est le joueur qui a déclenché cet évènement
#define EVENTS_TYPE_INTERNE         2		// c'est le prog. qui l'a déclenché
#define EVENTS_TYPE_SCENARIO        4       // le scénario est à l'origine de cet évènement

// TEvents : sous-type auquel on a affaire -> précision sur ce qui a déclenché l'évènement
// Equivalent des SCN_TRIGGER_PLAYER_UTILISER & SCN_TRIGGER_PLAYER_FRAPPER ...
/*#define EVENTS_TYPEINF_NONE			1		// pas de précision
#define EVENTS_TYPEINF_UTILISER		2		// le joueur a utiliser un objet ou un décors
#define EVENTS_TYPEINF_FRAPPER		4		// le joueur a frapper sur un décors*/

class GameObject;

// TEVENTS /////////////////////////////////////////////////////////////////////
// Evènement pouvant être déclenché à n'importe quel moment du jeu :
//	- par une action du joueur,
//	- par un script du scénario,
//  - ou même par un autre évènement.
class TEvent
{   //--------------------------------------------------- Attributs de la classe
    int iEventID;			// n°ID de l'évènement unique à chaque évènement
    int iEventStatus;		// Statut de l'évènement auquel on a affaire : direct ou indirect
    int iEventType;			// Type de l'évènement : indication sur ce qui a déclenché l'évènement
    int iEventTypeInf;		// Sous-Type : précision sur ce qui a déclenché l'évènement

    int iSource;			// Type de provenance de l'évènement : d'un objet, d'un type d'objet, du scénario, du joueur
    int	iSourceID;			// n° de l'objet/décors/perso ayant déclenché cet évènement
    GameObject *pSrcGO;		// adresse de l'objet/décors/perso ayant déclenché cet évènement

    int iDestination;		// Indication sur le  destinaire de cet évènement
    int iDestID;			// n° de l'objet/décors/perso à qui l'évènement s'adresse
    GameObject* pDestGO;	// adresse de l'objet/décors/perso à qui l'évènement s'adresse

    DWORD dwTimeStart;		// Heure du jeu à laquelle l'évènement à été déclenché
    DWORD dwTimeDeclench;	// Heure du jeu à laquelle il va falloir déclencher cet évènement
    DWORD dwTimeLength;		// Durée pendant laquelle l'évènement doit rester dans la file de message

	friend class TEventsManager;
	friend class TEventsList;
	//---------------------------------------------------- Méthodes de la classe
  public:
	 TEvent(int id, int status, int type, int typeInf);	// Constructeur par défaut
    ~TEvent();											// Destructeur

	void SetInfoEvent(GameObject *src, GameObject *dest);	// Précise à l'évènement de qui et à qui il s'adresse
	void SetInfoEvent(int srcID, int destID);				// Précise légèrement à l'évènement de qui et à qui il s'adresse	
};
///////////////////////////////// FIN de TEVENT ////////////////////////////////



// TEVENTSLIST /////////////////////////////////////////////////////////////////
// Liste des évènement survenu durant le jeu
class TEventsList
{ private:
	std::vector<TEvent*> vEvent[SCN_TRIGGER_MAX];	// Tableau dynamique d'évènements
	bool bNewEvents[SCN_TRIGGER_MAX];				// Indique si l'on vient de recevoir de nouveaux évènements
  public:
	TEventsList();		// Constructeur par défaut
	~TEventsList();		// Destructeur
	void AddEvent(TEvent* event);		// Rajoute un évènement à la liste
	void ClearNewEvents();				// Supprime les nouveaux évènements après avoir vérifié si des scripts devaient être lancés
	
	inline bool isNewEvent(int ind) { return bNewEvents[ind]; }
};
/////////////////////////////// FIN de TEVENTSLIST /////////////////////////////

// TEVENTSMANAGER //////////////////////////////////////////////////////////////
// Regroupe tous les évènements en cours									  //
class TEventsManager
{ private:
	static int iLastID;						// N°ID à attribuer au prochain évènement
	std::map<int, TEvent*> mGeneralEvents;	// Bibliothèques d'évènements : ID Events / TEvents*  
  public:
	TEventsManager();		// Constructeur par défaut
	~TEventsManager();		// Destructeur
	  
	static TEvent* EncodeEvent(int status, int type, int typeInf);	// Aide à la création d'un évènement
	void DispatchEvent(TEvent* event);								// Redirige cet évènement vers l'objet concerné
};
///////////////////////////// FIN de TEVENTSMANAGER ////////////////////////////

// ---------------------------------------------------------- fonctions globales
void InitGame_EventsManager();		// Initialisation de l'EventsManager	
void CloseGame_EventsManager();		// Ferme en souplesse l'EventsManager

// ---------------------------------------------------------- variables externes
extern TEventsManager*  EventsManager;	// Gestionnaire d'évènements





#endif
