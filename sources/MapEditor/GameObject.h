////////////////////////////////////////////////////////////////////////////////
//  Unit� "GameObject.h" : interface de la classe GameObject                  //
//                                                                            //
//	Description :  Classe m�re de tous les objets du jeu, que ce soit les     //
//				   personnages, les d�cors ou des objets.					  //
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
#include <stdio.h>
#include "ScnConsts.h"	// Constantes & define propres � la gestion du sc�nario

//----------------------------------------------------------------- Define & enum
// Attributions et convertions de n�ID distincts pour tous les ID du jeu
enum
{	FIRST_ID = 0,	
		// Personnages
		START_ID_PERSOS		=  FIRST_ID,
			START_ID_KNOW_PERSOS	= START_ID_PERSOS,
			MAX_ID_KNOW_PERSOS		= 50000,
			LAST_ID_KNOW_PERSOS		= START_ID_KNOW_PERSOS + MAX_ID_KNOW_PERSOS - 1,
			START_ID_ANY_PERSOS		= START_ID_PERSOS + MAX_ID_KNOW_PERSOS,
			MAX_ID_ANY_PERSOS		= 950000,
			LAST_ID_ANY_PERSOS		= START_ID_ANY_PERSOS + MAX_ID_ANY_PERSOS - 1,
		MAX_ID_PERSOS		=  MAX_ID_KNOW_PERSOS + MAX_ID_ANY_PERSOS,
		LAST_ID_PERSOS		=  MAX_ID_PERSOS - START_ID_PERSOS - 1,
		
		// D�cors
		START_ID_DECORS		=  MAX_ID_PERSOS,
			START_ID_KNOW_DECORS	= START_ID_DECORS,
			MAX_ID_KNOW_DECORS		= 200000,
			START_ID_ANY_DECORS		= START_ID_DECORS + MAX_ID_KNOW_DECORS,
			LAST_ID_KNOW_DECORS		= START_ID_KNOW_DECORS + MAX_ID_KNOW_DECORS - 1,
			MAX_ID_ANY_DECORS		= 800000,	
			LAST_ID_ANY_DECORS		= START_ID_ANY_DECORS + MAX_ID_ANY_DECORS - 1,
		MAX_ID_DECORS		=  MAX_ID_KNOW_DECORS + MAX_ID_ANY_DECORS,
		LAST_ID_DECORS		=  MAX_ID_DECORS - START_ID_DECORS - 1,
	LAST_ID = LAST_ID_DECORS
};

// Types de classes qui pourront d�river de la classe GameObject
#define GAMEOBJECT_UNDEFINED	0
#define GAMEOBJECT_PERSO		1
#define GAMEOBJECT_DECORS		2
#define GAMEOBJECT_OBJET		4

#define GAMEOBJECT_FIXED		1
#define GAMEOBJECT_ANY			2

class TScript;
class TScriptManager;
class TEventsList;
class TEvent;

////////////////////////////////////////////////////////////////////////////////
// Classe GAMEOBJECT
class GameObject
{ // --------------------------------------------------------- Attributs priiv�s
  protected:
	int iObjectID;			// ID unique pour tous les GameObject : persos, d�cors et objets
	int iObjectType;		// Type du GameObjects : persos, d�cors ou objets
	int iObjectStyle;		// Indique comment a �t� g�n�r� l'ID de cet objet : attribu� automatiquement ou saisi dans l'�diteur
    std::vector<int>	vStates;	// Tab. dynamique regroupant les diff�rents �tats que peut prendre un GameObject
    std::vector<int>	vMemory;	// Contient leur m�moire "interne" (ex: lieux o� ils sont d�j� all�s, dont ils ont entenuds parl�s, o� ils ont �t� trouv�s ...)
	
	int	iScriptID;					// N�ID du script associ� � cet objet
	TScript* pScript;				// Ptr vers ce m�me script
	TScriptManager *ScriptManager;	// Gestionnaire de d�roulement du script

	TEventsList	*EventsList;			// Liste d'�v�nements que peut recevoir ce GameObject

	// -------------------------------------------------------- M�thodes publiques
  public:
    //--- Fonctions intervenant sur la M�moire de l'Object ---
  	bool	   	 isInMemory(int memo);   	// Indique si 'memo' fait parti de la m�moire interne
    void		 AddMemory(int memo);		// Rajoute un �l�ment de m�moire
    void		 DelMemory(int memo);		// Perte de m�moire

    //--- Fonctions intervenant sur la liste des Etats de l'Object ---
  	bool		 isInState(int state);  	// Indique si l'Objet est dans cet �tat
    inline void	 AddState(int state);		// Rajoute un �tat � l'Objet
    void		 DelState(int state);		// Suppression d'un �tat 

	//--- Constructeurs & Destructeurs ---
	GameObject(int potentialID);			// Constructeur
	virtual ~GameObject();					// Destructeur

    //--- Attributions de param�tres ---
    void SetFixedID(int iID);          		// Attribution d'un nouveau n�ID fix� par l'�diteur
	void SetAnyID(int iID);					//		"			"		"	 attribu� au cours du jeu
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

	//--- Fonctions de Mise � Jour ---
	void EventManager();					// D�clenche les actions d'un script si ses conditions sont remplies
	void AddEvent(TEvent* event);			// Ajout d'un �v�nement � la liste
	void CreateEvent(int status, int type, int typeInf, GameObject* pDest);		// Cr�e directement un �v�nement et l'ajoute � la liste

	//-- Gestionnaire de scripts ---
	void AddRunningActionsScript(int iNumTrigger, int iNumScriptLine, TEvent* pEvent, int iNbActions);	// Ajoute un Scipt � la liste des scripts en cours d'ex�cution
	void ScriptsManager();					// Ex�cute les action des scripts en cours
};
//===================== Fin de la classe GAMEOBJECT ==========================//

//------------------------------------------------------------- Fonctions annexes
inline int GObj2Dec(int goID);		// Convertion d'un n� GameObject en DecorsObjet
inline int Dec2GObj(int decID);		//		"		 "	  DecorsObject en GameObjet	
inline int GObj2Per(int goID);		//		"		 "	  GameObject en Personnage
inline int Per2GObj(int perID);		//		"		 "	  Personnage en GameObjet



#endif


