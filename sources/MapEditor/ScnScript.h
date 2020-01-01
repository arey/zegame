////////////////////////////////////////////////////////////////////////////////
//  Unit� "ScnScript.h"  :  Interface des classes ScriptLine, TScript et	  //
//							GameScriptList	                                  //
//                                                                            //
//	Description :  ces classes permettent de lire, stocker et ex�cuter un     //
//				   script sc�naris� d'un objet, d'un d�cors ou d'un perso.	  //
//                                                                            //
//  Copyright (C) 1999 Antoine Rey    arey@insa-lyon.fr                       //
//                                    antrey@club-internet.fr                 //
//  Version      : beta 0.20                                                  //
//  Creation     : 16/07/1999 		                                          //
//  Last upgrade : <none>	                                                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef ScnScriptH
#define ScnScriptH

#ifdef MICROSOFT
#pragma warning(disable : 4100 4663 4018 4786)  
#endif

//---------------------------------------------------------------------- Include
#include <vector>	// Librairie STL de gestion de tableaux dynamiques
#include <map>          // Librairie STL de gestion de dictionnaires � cl� unique
#include <list>         // Librairie STL de gestion de doubles listes cha�n�es
#include "ScnConsts.h"	// Constantes & define propres � la gestion du sc�nario
#include "IDMap.h"	// Gestion des identificateurs du jeu
#include "ZeNain.h"     // Fichier de pragmas utilises afin de naviguer entre C++Builder et Visual C++

// ------------------------------------------------------------ Classes externes
namespace ZG
{
class TAction;
}
class TCause;
class TINILib;
class TEvent;
class GameObject;

// ---------------------------------------------------------------------- Define
#define MAX_SCRIPT_NAME			64	// Nom du script limit� � 64 caract�res
#define MAX_SCRIPT_FUNCTION_PARAMETER	 6	// 6 param�tres maximums	

// TScript : type de script
#define	SCRIPT_TYPE_MODELE		1	 // Script pouvant l'�tre de plusieurs objets
#define	SCRIPT_TYPE_PERSONNALISE	2	 // Script �crit sp�cifiquement pour un objet du jeu

// TScript : erreur et warning de compilation du script
#define SCRIPT_NOERROR			0      	// Pas d'erreur, c'est leu.....
#define SCRIPT_ERROR_NOTHING		1	// Aucune condition/action n'a �t� rencontr�e
#define	SCRIPT_ERROR_NOTACAUSE		2	// Le mot-cl� trouv� n'est pas une condition
#define SCRIPT_ERROR_NOTANACTION	3	// Le mot-cl� trouv� n'est pas une action

// -------------------------------------------------- Classes d�finies plus loin
class TRunningScript;	// Structure compl�t�e par l'ExcuteAction de la ScriptLine 


// SCRIPTLINE //////////////////////////////////////////////////////////////////
// Ligne d'un script d'un GameObject o� un ensemble de conditions r�unies     //
// peuvent enclencher une ou plusieurs actions.                               //
class ScriptLine
{	//--------------------------------------------------- Attributs de la classe
	int iTrigger;						// Facteur principalement responsable du d�clenchement de la lecture de cette ligne de script
	std::vector<ZG::TAction*>    vActions;	// Liste d'actions r�sultants d'une ligne d'un script
	std::vector<TCause*>	vCauses;	// Conditions validant cette portion de script

	bool isInvalidChar(char car);		// Renvoie vrai si c'est un espace, une tab, un car. de fin de ligne ...
	bool AddAction(int typeAction, ...);// D'apr�s le type d'action et ses divers param�tres, on ajoute une action � la liste
	bool AddCause(int typeCause, ...);	// D'apr�s le type de cause et ses divers param�tres, on ajoute une cause � la liste
    //---------------------------------------------------- M�thodes de la classe
  public:
    ScriptLine();						// Constructeur par d�faut
	~ScriptLine();						// Destructeur
	int EncodeActions(char *actions);	// Compile une cha�ne de car. en une liste d'actions encod�es
	int EncodeCauses(char *causes);		// Compile une cha�ne de car. en des conditions � r�unir
		
	inline int GetTrigger();		   	// renvoie le facteur d�clencheur    
	inline void SetTrigger(int trig);	// ajoute  le    "		" 
	inline int GetNbActions() { return vActions.size(); }	// Renvoie le nb d'actions contenus dans le tableau

	// Vrai si toutes les conditions sont v�rifi�es
	bool isAllCausesRealized(GameObject* pGO = NULL, TEvent *pEvent = NULL);
	// Lance telle action (n�iPosition) et r�cup�re les valeurs d'attentes
	void ExecuteAction(int iPos, TRunningScript &RunningAction, GameObject* pGO = NULL);
};
//////////////////////////// Fin de SCRIPTLINE /////////////////////////////////


// TRUNNINGSCRIPT //////////////////////////////////////////////////////////////
// Permet de suivre le d�roulement des suites d'actions d'un script           //
class TRunningScript
{	// -------------------------------------------------- Attributs de la classe
  public:
	  int iNumTrigger;			// Type de script auquel il appartient
	  int iNumScriptLine;		// N� de la ligne du script
	  int iPosition;			// N� d'action en cours
	  int iFinalTimer;			// Valeur de l'horloge � attendre avant de continuer le script
	  int iNbActions;			// Nb d'actions de la ScriptLine
	  int *iMutexID;			// Tableau indexant les n�ID des mutex 
	  bool bWaiting;			// Indique si l'on est en attente de synchro oui ou non
	  int iLastGoID;			// ID du dernier GameObject cr�� ou d�plac�
	// --------------------------------------------------- M�thodes de la classe
  public:
	  // Constructeur 
	  TRunningScript(int numTrig, int numLine, int pos, int finalTimer, int nbact);
	  // Destructeur
	  ~TRunningScript();
};
/////////////////////////// Fin de TRunningScript //////////////////////////////


// TSCRIPTMANAGER //////////////////////////////////////////////////////////////
// Classe permettant de suivre le d�roulement d'un script au fur et � mesure  //
// de son �x�cution															  //	
class TScriptManager
{ //--------------------------------------------------- Attributs de la classe	
  private:	
	std::map<int, int>	mTimer;			// Biblioth�ques d'Horloges pouvant servir � synchroniser des �v�nements
	GameObject*			pSrcGO;			// GameObject auquel appartient ce manager de scripts

  public:
	std::list<TRunningScript*> vRunningScripts;	// Liste de scripts en cours d'ex�cution

  //---------------------------------------------------- M�thodes de la classe
  public:
	TScriptManager();					// Constructeur par d�faut
	~TScriptManager();					// Destructeur
	
	bool IsTimer(int iID);					// Renvoie vrai si il a d�j� un �l�ment associ�
	int  GetTimer(int iID);					// Renvoie la valeur tu timer associ� au script n�iID
	void SetNewTimer(int iID, int iTimer);	// Associe un n� de script � un timer
	void SetTimer(int iID, int iTimer);		// MaJ de la valeur du futur timer

	void AddRunningActionsScript(int iNumTrigger, int iNumScriptLine, int iNbActions);	// Ajout d'un Script � la liste des scripts en cours d'ex�cution
	
	inline void SetSrcGO(GameObject* go) { pSrcGO = go; }
	inline GameObject* GetSrcGO() { return pSrcGO; }
};
/////////////////////////// Fin de TScriptManager //////////////////////////////


// TSCRIPT /////////////////////////////////////////////////////////////////////
// Ensemble de ligne de scripts (ScriptLine) engendrant le script d'un        //
// GameObject ou d'un patron de GameObject.                                   //
class TScript
{	//--------------------------------------------------- Attributs de la classe
    int iScriptID;							// N�ID unique � chaque script du jeu
    int iScriptType;						// Type de script : mod�le de script ou script personnalis� 
	char Name[MAX_SCRIPT_NAME];				// Nom du script
    std::vector<ScriptLine*> vScript[SCN_TRIGGER_MAX];	// Lignes composant un script class�es par facteurs de d�clenchement

    //---------------------------------------------------- M�thodes de la classe
  private:
	bool AddScriptLine(TINILib &script, char* scriptLine);	// Ajoute une ligne de script
  public:
	TScript(int zgsID);					// Constructeur
	~TScript();							// Destructeur
    bool LoadScript(char *scriptname);  // Charge le script � partir d'un fichier texte .zgs
	// Ex�cution des scripts r�pondants � l'�v�nement iTrigger
	void LaunchScript(int iTrigger, GameObject* pGO = NULL, TEvent* pEvent = NULL);	
	
	inline ScriptLine* GetScriptLine(int trig, int line) { return vScript[trig][line]; }

};
///////////////////////////// Fin de TSCRIPT ///////////////////////////////////


// GAMESCRIPTLIST //////////////////////////////////////////////////////////////
// Scripts de tous les objets et de tous les patrons du jeu
class GameScriptList
{	//--------------------------------------------------- Attributs de la classe
    std::map<int, TScript*>		ScriptsList;	// Dictionnaires regroupant tous les scripts du jeu directement accessibles par leur n�
    std::map<int, TScript*>::iterator	iter;	// It�rateur utilis� par la classe
	std::IDMap *IDGameScripts;					// Map des ID des scripts du jeu (leur nom sert de r�f�rence au fichier .zgs)

    //---------------------------------------------------- M�thodes de la classe
  public:
	GameScriptList();							// Constructeur par d�faut
	~GameScriptList();	 						// Destructeur
    std::IDMap*  GetScriptMap();			// R�cup�re le dico des ID des scripts du jeu
    TScript*	GetScript(int iID);				// Renvoie un pointeur vers le script n�iID
};
////////////////////////// Fin de GAMESCRIPTLIST ///////////////////////////////


//----------------------------------------------------------- Variables externes
extern std::IDMap *IDGameDecors;	 // Map des ID des diff�rents d�cors importants du jeu
extern std::IDMap *IDGamePersos;	 // Map des ID des diff�rents personnages importants du jeu
extern GameScriptList* GameScripts;	 // Librairie incorporant tous les scripts du jeu

//------------------------------------------------------------ Fonctions annexes
void InitGame_GameScriptList();			// Initialise la liste de scripts du jeu
void CloseGame_GameScriptList();		// Referme proprement la liste des scripts



#endif
