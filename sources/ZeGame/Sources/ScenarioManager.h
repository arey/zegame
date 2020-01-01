////////////////////////////////////////////////////////////////////////////////
//  Unité "ScenarioManager.h" : interface de la classe ScenarioManager        //
//                                                                            //
//	Description :  Cette classe est un singleton qui manage les differents    //
//				   ScenarioObject pourvus de scripts scenaritiques leur		  //
//				   permettant de, par exemple, gerer les cinematiques, les	  //
//				   bruitages ou les musiques, la trame du scenario principal  //
//				   ou encore celle de sous-quetes.							  // 
//                                                                            //
//  Copyright (C) 2000 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@ifrance.com		                  //
//  Version      : beta 0.01                                                  //
//  Creation     : 03/09/2000 		                                          //
//  Last upgrade : <none>                                                     //
////////////////////////////////////////////////////////////////////////////////

#ifndef ScenarioManagerH
#define ScenarioManagerH

//---------------------------------------------------------------------- Include
#include "Singleton.h"	// Template de toutes les classes singletons
#include <map>			// STL Library to handle xtree

//----------------------------------------------------------------------- Define
#define g_ScenarioManager ScenarioManager::getSingleton()

//------------------------------------------------------------------------- Enum
enum
{	// Initialisation du niveau venant d'etre charge
	SCENARIO_OBJECT_LEVEL_INIT			= 2000000,
	// Cloture la fin d'un niveau
	SCENARIO_OBJECT_LEVEL_CLOSE			= 2000001,
	
	// S'occupe de l'ambiance sonore specifique a un niveau
	SCENARIO_OBJECT_LEVEL_SFX_AMBIENT	= 2000010
};

//------------------------------------------------------------------------ Class
class ScenarioObject;
class GameObject;

////////////////////////////////////////////////////////////////////////////////
// Classe SCENARIOMANAGER													  //
class ScenarioManager  : public Singleton <ScenarioManager>
{ // ---------------------------------------------------------- Attributs privés
  protected:
	std::map<int, ScenarioObject*> mScenarioObject;	// Liste de ScenarioObject repertorie par ID
	
	// -------------------------------------------------------- Méthodes publiques
  public:
	//--- Constructeurs & Destructeurs ---
	ScenarioManager();					// Constructeur par defaut
	virtual ~ScenarioManager();			// Destructeur

	void init();						// Initialise tous les objets lies au scenario
	void update();						// MaJ le Scenario
	
	// --------------------------------------------------------- Méthodes privées
  private:
	void CreateEvent(int status, int type, int typeInf, GameObject* pDest); // Crée directement un évènement et l'ajoute à la liste
};
//=================== Fin de la classe SCENARIOMANAGER ========================//

//------------------------------------------------------------- Fonctions annexes
void InitGame_ScenarioManager();		// Initialise le manager du Scenarios
void CloseGame_ScenarioManager();		// Referme proprement le manager du Scenarios

#endif	// ScenarioManagerH


