////////////////////////////////////////////////////////////////////////////////
//  Unit� "ScnActions.h"  : interface de la classe TAction et de ses d�riv�es //
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
class GameObject;		// Classe m�re de toues les objets du jeu
class TScriptManager;	// Structure de suivit des scripts
class TRunningScript;	// Structure compl�t�e par l'ExecuteAction de la ScriptLine 

namespace ZG
{
// TACTION /////////////////////////////////////////////////////////////////////
// Action � accomplir sur un objet, un personnage, un d�cors mais encore sur
// l'historique du jeu, les bruitages, les dialogues, ...
class TAction
{ protected:
	int iActionID;			// N� de l'action unique
	int iActionType;		// Type de l'action : utiliser un objet, joueur un son, lancer un �v�nement, ...
	bool bEndSynchro;		// Indique si l'on va devoir se synchroniser sur la fin de cette action

	static int iLastID;		// N�ID � attribuer � la prochaine action
  public:
    TAction();						// Constructeur par d�faut
    virtual ~TAction();				// Destructeur par d�faut
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
// Ajoute un �l�ment de m�moire au GameObject
//  - iGoID = 0  : celui poss�dant ce script
//  - iGoID = -1 : celui ayant d�clench� le script
class TAction_GONewMemory : public ZG::TAction
{ 	int iMemoryID;		// ID de la m�moire que l'on va rajouter
	int iGoID;			// ID du GameObject auquel il va falloir rajouter cette m�moire
	
  public:
  	TAction_GONewMemory(int memID, int goID = 0); // Constructeur
    ~TAction_GONewMemory();						  // Destructeur
    // Joue une animation superpos�e "classique" au d�cors
	void PlayAction(TRunningScript &RunningAction, GameObject *pGO = NULL);	
};
//////////////////////// Fin de TACTION_GO_NEWMEMORY ///////////////////////////

// TACTION_GO_DELMEMORY ////////////////////////////////////////////////////////
// Supprime un memory au GameObject
//  - iGoID = 0  : celui poss�dant ce script
//  - iGoID = -1 : celui ayant d�clench� le script
class TAction_GODelMemory : public ZG::TAction
{ 	int iMemoryID;		// ID de la m�moire que l'on va rajouter
	int iGoID;			// ID du GameObject auquel il va falloir effacer cette m�moire
	
  public:
  	TAction_GODelMemory(int memID, int goID = 0); // Constructeur
    ~TAction_GODelMemory();						  // Destructeur
    // Joue une animation superpos�e "classique" au d�cors
	void PlayAction(TRunningScript &RunningAction, GameObject *pGO = NULL);	
};
//////////////////////// Fin de TACTION_GO_DELMEMORY ///////////////////////////



////////////////////////////////////////////////////////////////////////////////
//	 							  DECORS									  //
////////////////////////////////////////////////////////////////////////////////


// TACTION_DECORS_PLAY_SUPANIM /////////////////////////////////////////////////
class TAction_DecorsPlaySupAnim : public ZG::TAction
{ 	int iDecorsID;			// ID du d�cors dont on va jouer une anim
	int iAnimID;			// ID de l'anim superposable "classique" � jouer par dessus le d�cors 

  public:
  	TAction_DecorsPlaySupAnim(int decID, int anmID);	// Constructeur
    ~TAction_DecorsPlaySupAnim();						// Destructeur
    // Joue une animation superpos�e "classique" au d�cors
	void PlayAction(TRunningScript &RunningAction, GameObject *pGO = NULL);	
};
///////////////////// Fin de TACTION_DECORS_PLAY_SUPANIM ///////////////////////


// TACTION_DECORS_PLAY_NEWSTATE ////////////////////////////////////////////////
class TAction_DecorsNewState : public ZG::TAction
{ 	int iDecorsID;			// ID du d�cors dont on va jouer une anim
	int iFutState;			// ID du nouvel �tat de l'objet  

  public:
  	TAction_DecorsNewState(int decID, int futStateID);	// Constructeur
    ~TAction_DecorsNewState();							// Destructeur
    // Change l'�tat d'animation du d�cors 
	void PlayAction(TRunningScript &RunningAction, GameObject *pGO = NULL);
};
///////////////////// Fin de TACTION_DECORS_NEWSTATE ///////////////////////////



////////////////////////////////////////////////////////////////////////////////
//	 							  PERSOS									  //
////////////////////////////////////////////////////////////////////////////////

// TACTION_PERSOS_CREATE ///////////////////////////////////////////////////////
// Fait appara�tre un personnage non joueur � tel endroit					  //
//	- soit en une position (fPosX,fPosY) sur la map
//	- soit � l'endroit de l'objet ayant d�clench� l'action
class TAction_PersosCreate : public ZG::TAction
{ 	int iPersoType;			// Type de personnage
	double fPosX, fPosY;
	// Position o� ce PNJ doit appara�tre
	bool bPosGameObject;
	int iApparition;		// Type d'apparition (en un point, en coord relatives, ...)
	int iPersoID;			// N�de perso d�j� connu

  public:
	enum { PERSOCREATE_POINT=0, PERSOCREATE_OFFSET };
  	// 1er Constructeur
	TAction_PersosCreate(int persoType, double posX, double posY, int where=0, int idPerso=0);
	// 2nd Constructeur
	TAction_PersosCreate(int persoType, int idPerso=0);										
    ~TAction_PersosCreate();													// Destructeur
    // Change l'�tat d'animation du d�cors 
	void PlayAction(TRunningScript &RunningAction, GameObject *pGO = NULL);
};
///////////////////// Fin de TACTION_PERSOS_CREATE /////////////////////////////

// TACTION_PERSOS_DELETE ///////////////////////////////////////////////////////
// Fait disparaitre un personnage non joueur								  //
class TAction_PersosDelete : public ZG::TAction
{   int iPersoID;	// N� du personnage � killer

  public:
	TAction_PersosDelete(int id);
    ~TAction_PersosDelete();													// Destructeur
    // Change l'�tat d'animation du d�cors 
	void PlayAction(TRunningScript &RunningAction, GameObject *pGO = NULL);
};
///////////////////// Fin de TACTION_PERSOS_DELETE /////////////////////////////


// TACTION_PERSOS_MOVE /////////////////////////////////////////////////////////
// Fait avancer un personnage en											  //	
//  - soit au point (fPosX,fPosY) de la map,								  //
//	- soit d'un d�placement de (fDepX,fDepY) � partir de sa position courante //

class TAction_PersosMove : public ZG::TAction
{	int iPersoID;			// N� du perso � d�placer (0 indique que c'est celui qui a ex�cut� le script et -1 que c'est celui du RunningScript)
	int	iMoveType;			// Type de d�placement
	union
	{	double fPosX;		// Coordonn�e X o� le Perso doit se d�placer
		double fDepX;		// Offset de d�placement en X
	};
	union
	{	double fPosY;		// Coordonn�e Y o� le Perso doit se d�placer
		double fDepY;		// Offset de d�placement en Y
	};	

  public:
  	TAction_PersosMove(int moveType, double posX, double posY, int persoID = 0); // 1er Constructeur
    ~TAction_PersosMove();											// Destructeur
    // Change l'�tat d'animation du d�cors 
	void PlayAction(TRunningScript &RunningAction, GameObject *pGO = NULL);
};
///////////////////// Fin de TACTION_PERSOS_MOVE ///////////////////////////////



////////////////////////////////////////////////////////////////////////////////
//	 							  SYNCHRO									  //
////////////////////////////////////////////////////////////////////////////////

// TACTION_SYNCHRO_WAITEND /////////////////////////////////////////////////////
// Attends la fin d'une action pr�c�dente
class TAction_WaitEnd : public ZG::TAction
{ 	int iPosAction;		// Indice ds la ScriptLine de l'action sur laquelle il va falloir se synchroniser

  public:
  	TAction_WaitEnd(int posAction);		// Constructeur
    ~TAction_WaitEnd();					// Destructeur
	// V�rifie si l'action est finie 
	void PlayAction(TRunningScript &RunningAction, GameObject *pGO = NULL);
};
///////////////////// Fin de TACTION_SYNCHRO_WAITEND ///////////////////////////

// TACTION_SYNCHRO_SLEEP ///////////////////////////////////////////////////////
// Attends un certain temps avant de continuer
class TAction_Sleep : public ZG::TAction
{ 	int iNbSleepTime;	// Temps (en ms) � attendre avant de continuer 

  public:
  	TAction_Sleep(int nbMs);			// Constructeur
    ~TAction_Sleep();					// Destructeur
	// V�rifie si l'action est finie 
	void PlayAction(TRunningScript &RunningAction, GameObject *pGO = NULL);
};
/////////////////////// Fin de TACTION_SYNCHRO_SLEEP ///////////////////////////


#endif	// ScnActionsH
