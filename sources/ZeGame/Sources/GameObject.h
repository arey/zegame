////////////////////////////////////////////////////////////////////////////////
//  Unité "GameObject.h" : interface de la classe GameObject                  //
//                                                                            //
//	Description :  Classe mère de tous les objets du jeu, que ce soit les     //
//				   personnages, les décors ou des objets.					  //
//                                                                            //
//  Copyright (C) 1999 Antoine Rey    arey@insa-lyon.fr                       //
//                                    antrey@club-internet.fr                 //
//  Version      : beta 0.20                                                  //
//  Creation     : 14/07/1999 		                                          //
//  Last upgrade : <none>                                                     //
////////////////////////////////////////////////////////////////////////////////

#ifndef GameObjectH
#define GameObjectH

//---------------------------------------------------------------------- Include
#include <vector>	   	//	librairie STL de gestion de tableaux dynamiques
#include "ScnConsts.h"	// Constantes & define propres à la gestion du scénario

//----------------------------------------------------------------- Define & enum
// Attributions et convertions de n°ID distincts pour tous les ID du jeu
enum
{	FIRST_ID = 0,	
		// Personnages
		START_ID_PERSOS		=  FIRST_ID,
			START_ID_KNOW_PERSOS	= START_ID_PERSOS,						// 0
			MAX_ID_KNOW_PERSOS		= 50000,
			LAST_ID_KNOW_PERSOS		= START_ID_KNOW_PERSOS + MAX_ID_KNOW_PERSOS - 1,
			START_ID_ANY_PERSOS		= START_ID_PERSOS + MAX_ID_KNOW_PERSOS,
			MAX_ID_ANY_PERSOS		= 950000,
			LAST_ID_ANY_PERSOS		= START_ID_ANY_PERSOS + MAX_ID_ANY_PERSOS - 1,
		MAX_ID_PERSOS		=  MAX_ID_KNOW_PERSOS + MAX_ID_ANY_PERSOS,		// 1 000 000
		LAST_ID_PERSOS		=  MAX_ID_PERSOS + START_ID_PERSOS - 1,			//	 999 999
		
		// Décors
		START_ID_DECORS		=  LAST_ID_PERSOS + 1,
			START_ID_KNOW_DECORS	= START_ID_DECORS,						// 1 000 000
			MAX_ID_KNOW_DECORS		= 200000,
			START_ID_ANY_DECORS		= START_ID_DECORS + MAX_ID_KNOW_DECORS,	// 1 200 000
			LAST_ID_KNOW_DECORS		= START_ID_KNOW_DECORS + MAX_ID_KNOW_DECORS - 1,
			MAX_ID_ANY_DECORS		= 800000,	
			LAST_ID_ANY_DECORS		= START_ID_ANY_DECORS + MAX_ID_ANY_DECORS - 1,
		MAX_ID_DECORS		=  MAX_ID_KNOW_DECORS + MAX_ID_ANY_DECORS,		// 2 000 000
		LAST_ID_DECORS		=  MAX_ID_DECORS + START_ID_DECORS -1,			// 1 999 999

		// Scénario
		START_ID_SCENARIO	=  LAST_ID_DECORS + 1,							// 2 000 000
			START_ID_KNOW_SCENARIO	= START_ID_SCENARIO,
			MAX_ID_KNOW_SCENARIO	= 5000,
			START_ID_ANY_SCENARIO		= START_ID_SCENARIO + MAX_ID_KNOW_SCENARIO,
			LAST_ID_KNOW_SCENARIO		= START_ID_KNOW_SCENARIO + MAX_ID_KNOW_SCENARIO - 1,
			MAX_ID_ANY_SCENARIO		= 5000,	
			LAST_ID_ANY_SCENARIO	= START_ID_ANY_SCENARIO + MAX_ID_ANY_SCENARIO - 1,
		MAX_ID_SCENARIO		=  MAX_ID_KNOW_SCENARIO + MAX_ID_ANY_SCENARIO,	//   100 000
		LAST_ID_SCENARIO	=  MAX_ID_SCENARIO + START_ID_SCENARIO - 1,		// 2 099 999
	LAST_ID = LAST_ID_SCENARIO
};

// Types de classes qui pourront dériver de la classe GameObject
#define GAMEOBJECT_UNDEFINED	0
#define GAMEOBJECT_PERSO		1
#define GAMEOBJECT_DECORS		2
#define GAMEOBJECT_OBJET		4
#define GAMEOBJECT_SCENARIO		5

#define GAMEOBJECT_FIXED		1
#define GAMEOBJECT_ANY			2

class TScript;
class TScriptManager;
class TEventsList;
class TEvent;

////////////////////////////////////////////////////////////////////////////////
// Classe GAMEOBJECT
class GameObject
{ // --------------------------------------------------------- Attributs priivés
  protected:
	int iObjectID;			// ID unique pour tous les GameObject : persos, décors et objets
	int iObjectType;		// Type du GameObjects : persos, décors ou objets
	int iObjectStyle;		// Indique comment a été généré l'ID de cet objet : attribué automatiquement ou saisi dans l'éditeur
    std::vector<int>	vStates;	// Tab. dynamique regroupant les différents états que peut prendre un GameObject
    std::vector<int>	vMemory;	// Contient leur mémoire "interne" (ex: lieux où ils sont déjà allés, dont ils ont entenuds parlés, où ils ont été trouvés ...)
	
	int	iScriptID;					// N°ID du script associé à cet objet
	TScript* pScript;				// Ptr vers ce même script
	TScriptManager *ScriptManager;	// Gestionnaire de déroulement du script

	TEventsList	*EventsList;			// Liste d'évènements que peut recevoir ce GameObject
	bool bVisible;			// Indique si ce GO est visible ou non à l'écran et dans le jeu

	// -------------------------------------------------------- Méthodes publiques
  public:
    //--- Fonctions intervenant sur la Mémoire de l'Object ---
  	bool	   	 isInMemory(int memo);   	// Indique si 'memo' fait parti de la mémoire interne
    void		 AddMemory(int memo);		// Rajoute un élément de mémoire
    void		 DelMemory(int memo);		// Perte de mémoire

    //--- Fonctions intervenant sur la liste des Etats de l'Object ---
  	bool		 isInState(int state);  	// Indique si l'Objet est dans cet état
    inline void	 AddState(int state);		// Rajoute un état à l'Objet
    void		 DelState(int state);		// Suppression d'un état 

	//--- Constructeurs & Destructeurs ---
	GameObject(int potentialID);			// Constructeur
	virtual ~GameObject();					// Destructeur

    //--- Attributions de paramètres ---
    void SetFixedID(int iID);          		// Attribution d'un nouveau n°ID fixé par l'éditeur
	void SetAnyID(int iID);					//		"			"		"	 attribué au cours du jeu
    void SetScriptID(int iID);				// Attribution d'un script .zgs
    
	//--- Fonctions de Sauvegarde/Chargement ---
	void SaveGOInfosToFile(FILE *f);					// Sauvegarde des informations relatives au GameObject
    void LoadGOInfosFromFile(FILE *f, float &version);	// Chargement des informations relatives au GameObject

	//--- Fonctions renvoyant certains attributs de l'objet ---
	inline int GetID() { return iObjectID; }
    inline int GetObjectType() { return iObjectType; }
	inline int GetObjectStyle() { return iObjectStyle; }
    inline int GetScriptID()	{ return iScriptID; }
	inline TScriptManager* GeScriptManager() { return ScriptManager; }
	inline bool hasScript() { return pScript!=NULL; } 

	//--- Fonctions de Mise à Jour ---
	void EventManager();					// Déclenche les actions d'un script si ses conditions sont remplies
	void AddEvent(TEvent* event);			// Ajout d'un évènement à la liste
	void CreateEvent(int status, int type, int typeInf, GameObject* pDest);		// Crée directement un évènement et l'ajoute à la liste

	//--- Gestionnaire de scripts ---
	void AddRunningActionsScript(int iNumTrigger, int iNumScriptLine, TEvent* pEvent, int iNbActions);	// Ajoute un Scipt à la liste des scripts en cours d'exécution
	bool isScriptLineRunning(int line);		// Indique si cette ScripLine est en cours d'execution
	bool isScriptRunning();					// Indique si le script assigne a cet object est en cours d'execution
	void ScriptsManager();							// Exécute les action des scripts en cours
	
	//--- Visibilité du GameObject ---
	inline void SetVisible(bool vis=true) { bVisible = vis; }	// Rend visible (ou invisible) le GO
	inline bool isVisible() { return bVisible; }				// Indique si ce GO est visible ou non à l'écran
	
};
//===================== Fin de la classe GAMEOBJECT ==========================//

//------------------------------------------------------------- Fonctions annexes
inline int GObj2Dec(int goID);		// Convertion d'un n° GameObject en DecorsObjet
inline int Dec2GObj(int decID);		//		"		 "	  DecorsObject en GameObjet	
inline int GObj2Per(int goID);		//		"		 "	  GameObject en Personnage
inline int Per2GObj(int perID);		//		"		 "	  Personnage en GameObjet



#endif


