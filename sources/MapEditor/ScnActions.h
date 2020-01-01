////////////////////////////////////////////////////////////////////////////////
//  Unité "ScnActions.h"  : interface de la classe TAction et de ses dérivées //
//                                                                            //
//	Description :                                                             //
//                                                                            //
//  Copyright (C) 1999 Antoine Rey    arey@insa-lyon.fr                       //
//                                    antrey@club-internet.fr                 //
//  Version      : beta 0.20                                                  //
//  Creation     : 14/07/1999 		                                          //
//  Last upgrade : <none>	                                                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef ScnActionsH
#define ScnActionsH

//----------------------------------------------------------------------- Define
#ifdef CBUILDER
#define NULL	0
#endif

//------------------------------------------------------------------------- Enum
enum 
{	ACTION_PERSOSMOVE_DESTINATION,
	ACTION_PERSOSMOVE_OFFSET
};

//---------------------------------------------------------------------- Classes
class GameObject;		// Classe mère de toues les objets du jeu
class TScriptManager;	// Structure de suivit des scripts
class TRunningScript;	// Structure complétée par l'ExecuteAction de la ScriptLine 

namespace ZG
{
// TACTION /////////////////////////////////////////////////////////////////////
// Action à accomplir sur un objet, un personnage, un décors mais encore sur
// l'historique du jeu, les bruitages, les dialogues, ...
class TAction
{ protected:
	int iActionID;			// N° de l'action unique
	int iActionType;		// Type de l'action : utiliser un objet, joueur un son, lancer un évènement, ...
	bool bEndSynchro;		// Indique si l'on va devoir se synchroniser sur la fin de cette action

	static int iLastID;		// N°ID à attribuer à la prochaine action
  public:
    TAction();						// Constructeur par défaut
    virtual ~TAction();				// Destructeur par défaut
  	// Effectue l'action, quel que soit son type
	virtual void PlayAction(TRunningScript &RunningAction, GameObject *pGO = NULL);
	inline void SetEndSynchro(bool sync) { bEndSynchro = sync; }
};
////////////////////////// Fin de TACTION //////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////
//								GAMEOBJECT									  //
////////////////////////////////////////////////////////////////////////////////

// TACTION_GO_NEWMEMORY ////////////////////////////////////////////////////////
// Ajoute un élément de mémoire au GameObject
//  - iGoID = 0  : celui possédant ce script
//  - iGoID = -1 : celui ayant déclenché le script
class TAction_GONewMemory : public ZG::TAction
{ 	int iMemoryID;		// ID de la mémoire que l'on va rajouter
	int iGoID;			// ID du GameObject auquel il va falloir rajouter cette mémoire
	
  public:
  	TAction_GONewMemory(int memID, int goID = 0); // Constructeur
    ~TAction_GONewMemory();						  // Destructeur
    // Joue une animation superposée "classique" au décors
	void PlayAction(TRunningScript &RunningAction, GameObject *pGO = NULL);	
};
//////////////////////// Fin de TACTION_GO_NEWMEMORY ///////////////////////////

// TACTION_GO_DELMEMORY ////////////////////////////////////////////////////////
// Supprime un memory au GameObject
//  - iGoID = 0  : celui possédant ce script
//  - iGoID = -1 : celui ayant déclenché le script
class TAction_GODelMemory : public ZG::TAction
{ 	int iMemoryID;		// ID de la mémoire que l'on va rajouter
	int iGoID;			// ID du GameObject auquel il va falloir effacer cette mémoire
	
  public:
  	TAction_GODelMemory(int memID, int goID = 0); // Constructeur
    ~TAction_GODelMemory();						  // Destructeur
    // Joue une animation superposée "classique" au décors
	void PlayAction(TRunningScript &RunningAction, GameObject *pGO = NULL);	
};
//////////////////////// Fin de TACTION_GO_DELMEMORY ///////////////////////////



////////////////////////////////////////////////////////////////////////////////
//	 							  DECORS									  //
////////////////////////////////////////////////////////////////////////////////


// TACTION_DECORS_PLAY_SUPANIM /////////////////////////////////////////////////
class TAction_DecorsPlaySupAnim : public ZG::TAction
{ 	int iDecorsID;			// ID du décors dont on va jouer une anim
	int iAnimID;			// ID de l'anim superposable "classique" à jouer par dessus le décors 

  public:
  	TAction_DecorsPlaySupAnim(int decID, int anmID);	// Constructeur
    ~TAction_DecorsPlaySupAnim();						// Destructeur
    // Joue une animation superposée "classique" au décors
	void PlayAction(TRunningScript &RunningAction, GameObject *pGO = NULL);	
};
///////////////////// Fin de TACTION_DECORS_PLAY_SUPANIM ///////////////////////


// TACTION_DECORS_PLAY_NEWSTATE ////////////////////////////////////////////////
class TAction_DecorsNewState : public ZG::TAction
{ 	int iDecorsID;			// ID du décors dont on va jouer une anim
	int iFutState;			// ID du nouvel état de l'objet  

  public:
  	TAction_DecorsNewState(int decID, int futStateID);	// Constructeur
    ~TAction_DecorsNewState();							// Destructeur
    // Change l'état d'animation du décors 
	void PlayAction(TRunningScript &RunningAction, GameObject *pGO = NULL);
};
///////////////////// Fin de TACTION_DECORS_NEWSTATE ///////////////////////////



////////////////////////////////////////////////////////////////////////////////
//	 							  PERSOS									  //
////////////////////////////////////////////////////////////////////////////////

// TACTION_PERSOS_CREATE ///////////////////////////////////////////////////////
// Fait apparaître un personnage non joueur à tel endroit					  //
//	- soit en une position (fPosX,fPosY) sur la map
//	- soit à l'endroit de l'objet ayant déclenché l'action
class TAction_PersosCreate : public ZG::TAction
{ 	int iPersoType;			// Type de personnage
	double fPosX, fPosY;
	// Position où ce PNJ doit apparaître
	bool bPosGameObject;
	int iApparition;		// Type d'apparition (en un point, en coord relatives, ...)
	int iPersoID;			// N°de perso déjà connu

  public:
	enum { PERSOCREATE_POINT=0, PERSOCREATE_OFFSET };
  	// 1er Constructeur
	TAction_PersosCreate(int persoType, double posX, double posY, int where=0, int idPerso=0);
	// 2nd Constructeur
	TAction_PersosCreate(int persoType, int idPerso=0);										
    ~TAction_PersosCreate();													// Destructeur
    // Change l'état d'animation du décors 
	void PlayAction(TRunningScript &RunningAction, GameObject *pGO = NULL);
};
///////////////////// Fin de TACTION_PERSOS_CREATE /////////////////////////////

// TACTION_PERSOS_DELETE ///////////////////////////////////////////////////////
// Fait disparaitre un personnage non joueur								  //
class TAction_PersosDelete : public ZG::TAction
{   int iPersoID;	// N° du personnage à killer

  public:
	TAction_PersosDelete(int id);
    ~TAction_PersosDelete();													// Destructeur
    // Change l'état d'animation du décors 
	void PlayAction(TRunningScript &RunningAction, GameObject *pGO = NULL);
};
///////////////////// Fin de TACTION_PERSOS_DELETE /////////////////////////////


// TACTION_PERSOS_MOVE /////////////////////////////////////////////////////////
// Fait avancer un personnage en											  //	
//  - soit au point (fPosX,fPosY) de la map,								  //
//	- soit d'un déplacement de (fDepX,fDepY) à partir de sa position courante //

class TAction_PersosMove : public ZG::TAction
{	int iPersoID;			// N° du perso à déplacer (0 indique que c'est celui qui a exécuté le script et -1 que c'est celui du RunningScript)
	int	iMoveType;			// Type de déplacement
	union
	{	double fPosX;		// Coordonnée X où le Perso doit se déplacer
		double fDepX;		// Offset de déplacement en X
	};
	union
	{	double fPosY;		// Coordonnée Y où le Perso doit se déplacer
		double fDepY;		// Offset de déplacement en Y
	};	

  public:
  	TAction_PersosMove(int moveType, double posX, double posY, int persoID = 0); // 1er Constructeur
    ~TAction_PersosMove();											// Destructeur
    // Change l'état d'animation du décors 
	void PlayAction(TRunningScript &RunningAction, GameObject *pGO = NULL);
};
///////////////////// Fin de TACTION_PERSOS_MOVE ///////////////////////////////



////////////////////////////////////////////////////////////////////////////////
//	 							  SYNCHRO									  //
////////////////////////////////////////////////////////////////////////////////

// TACTION_SYNCHRO_WAITEND /////////////////////////////////////////////////////
// Attends la fin d'une action précédente
class TAction_WaitEnd : public ZG::TAction
{ 	int iPosAction;		// Indice ds la ScriptLine de l'action sur laquelle il va falloir se synchroniser

  public:
  	TAction_WaitEnd(int posAction);		// Constructeur
    ~TAction_WaitEnd();					// Destructeur
	// Vérifie si l'action est finie 
	void PlayAction(TRunningScript &RunningAction, GameObject *pGO = NULL);
};
///////////////////// Fin de TACTION_SYNCHRO_WAITEND ///////////////////////////

// TACTION_SYNCHRO_SLEEP ///////////////////////////////////////////////////////
// Attends un certain temps avant de continuer
class TAction_Sleep : public ZG::TAction
{ 	int iNbSleepTime;	// Temps (en ms) à attendre avant de continuer 

  public:
  	TAction_Sleep(int nbMs);			// Constructeur
    ~TAction_Sleep();					// Destructeur
	// Vérifie si l'action est finie 
	void PlayAction(TRunningScript &RunningAction, GameObject *pGO = NULL);
};
/////////////////////// Fin de TACTION_SYNCHRO_SLEEP ///////////////////////////


#endif	// ScnActionsH
