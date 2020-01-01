////////////////////////////////////////////////////////////////////////////////
//  Unit� "ScnEvents.h" interface de la classe TEvents						  //
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
#include <map>			// librairie STL de gestion de dictionnaires � cl� unique
#include "ScnConsts.h"	// Constantes & define propres � la gestion du sc�nario


//----------------------------------------------------------------------- Define

// TEvents : indication sur ce qui a d�clench� l'�v�nement
#define	EVENTS_SOURCE_OBJET			1
#define	EVENTS_SOURCE_SCENARIO		2
#define	EVENTS_SOURCE_JOUEUR		4

// TEvents : statut de l'�v�nement auquel on a affaire
#define EVENTS_STATUS_DIRECT		1		// l'�v�nement ne s'adresse qu'� un seul destinataire
#define EVENTS_STATUS_INDIRECT		2		// l'�v�nement peut affecter plusieurs persos/objets/d�cors

// TEvents : type de l'�v�nement auquel on a affaire -> indication sur ce qui a d�clench� l'�v�nement
#define EVENTS_TYPE_JOUEUR 			1		// c'est le joueur qui a d�clench� cet �v�nement
#define EVENTS_TYPE_INTERNE         2		// c'est le prog. qui l'a d�clench�
#define EVENTS_TYPE_SCENARIO        4       // le sc�nario est � l'origine de cet �v�nement

// TEvents : sous-type auquel on a affaire -> pr�cision sur ce qui a d�clench� l'�v�nement
// Equivalent des SCN_TRIGGER_PLAYER_UTILISER & SCN_TRIGGER_PLAYER_FRAPPER ...
/*#define EVENTS_TYPEINF_NONE			1		// pas de pr�cision
#define EVENTS_TYPEINF_UTILISER		2		// le joueur a utiliser un objet ou un d�cors
#define EVENTS_TYPEINF_FRAPPER		4		// le joueur a frapper sur un d�cors*/

class GameObject;

// TEVENTS /////////////////////////////////////////////////////////////////////
// Ev�nement pouvant �tre d�clench� � n'importe quel moment du jeu :
//	- par une action du joueur,
//	- par un script du sc�nario,
//  - ou m�me par un autre �v�nement.
class TEvent
{   //--------------------------------------------------- Attributs de la classe
    int iEventID;			// n�ID de l'�v�nement unique � chaque �v�nement
    int iEventStatus;		// Statut de l'�v�nement auquel on a affaire : direct ou indirect
    int iEventType;			// Type de l'�v�nement : indication sur ce qui a d�clench� l'�v�nement
    int iEventTypeInf;		// Sous-Type : pr�cision sur ce qui a d�clench� l'�v�nement

    int iSource;			// Type de provenance de l'�v�nement : d'un objet, d'un type d'objet, du sc�nario, du joueur
    int	iSourceID;			// n� de l'objet/d�cors/perso ayant d�clench� cet �v�nement
    GameObject *pSrcGO;		// adresse de l'objet/d�cors/perso ayant d�clench� cet �v�nement

    int iDestination;		// Indication sur le  destinaire de cet �v�nement
    int iDestID;			// n� de l'objet/d�cors/perso � qui l'�v�nement s'adresse
    GameObject* pDestGO;	// adresse de l'objet/d�cors/perso � qui l'�v�nement s'adresse

    DWORD dwTimeStart;		// Heure du jeu � laquelle l'�v�nement � �t� d�clench�
    DWORD dwTimeDeclench;	// Heure du jeu � laquelle il va falloir d�clencher cet �v�nement
    DWORD dwTimeLength;		// Dur�e pendant laquelle l'�v�nement doit rester dans la file de message

	friend class TEventsManager;
	friend class TEventsList;
	//---------------------------------------------------- M�thodes de la classe
  public:
	 TEvent(int id, int status, int type, int typeInf);	// Constructeur par d�faut
    ~TEvent();											// Destructeur

	void SetInfoEvent(GameObject *src, GameObject *dest);	// Pr�cise � l'�v�nement de qui et � qui il s'adresse
	void SetInfoEvent(int srcID, int destID);				// Pr�cise l�g�rement � l'�v�nement de qui et � qui il s'adresse	
};
///////////////////////////////// FIN de TEVENT ////////////////////////////////



// TEVENTSLIST /////////////////////////////////////////////////////////////////
// Liste des �v�nement survenu durant le jeu
class TEventsList
{ private:
	std::vector<TEvent*> vEvent[SCN_TRIGGER_MAX];	// Tableau dynamique d'�v�nements
	bool bNewEvents[SCN_TRIGGER_MAX];				// Indique si l'on vient de recevoir de nouveaux �v�nements
  public:
	TEventsList();		// Constructeur par d�faut
	~TEventsList();		// Destructeur
	void AddEvent(TEvent* event);		// Rajoute un �v�nement � la liste
	void ClearNewEvents();				// Supprime les nouveaux �v�nements apr�s avoir v�rifi� si des scripts devaient �tre lanc�s
	
	inline bool isNewEvent(int ind) { return bNewEvents[ind]; }
};
/////////////////////////////// FIN de TEVENTSLIST /////////////////////////////

// TEVENTSMANAGER //////////////////////////////////////////////////////////////
// Regroupe tous les �v�nements en cours									  //
class TEventsManager
{ private:
	static int iLastID;						// N�ID � attribuer au prochain �v�nement
	std::map<int, TEvent*> mGeneralEvents;	// Biblioth�ques d'�v�nements : ID Events / TEvents*  
  public:
	TEventsManager();		// Constructeur par d�faut
	~TEventsManager();		// Destructeur
	  
	static TEvent* EncodeEvent(int status, int type, int typeInf);	// Aide � la cr�ation d'un �v�nement
	void DispatchEvent(TEvent* event);								// Redirige cet �v�nement vers l'objet concern�
};
///////////////////////////// FIN de TEVENTSMANAGER ////////////////////////////

// ---------------------------------------------------------- fonctions globales
void InitGame_EventsManager();		// Initialisation de l'EventsManager	
void CloseGame_EventsManager();		// Ferme en souplesse l'EventsManager

// ---------------------------------------------------------- variables externes
extern TEventsManager*  EventsManager;	// Gestionnaire d'�v�nements





#endif
