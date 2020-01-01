////////////////////////////////////////////////////////////////////////////////
//  Unité "ScenarioObject.h" : interface de la classe ScenarioObject          //
//                                                                            //
//	Description :  Heritant du comportement d'un GameObject, cet object va	  //
//				   plus particulierement intervenir dans la gestion de scripts//
//				   scenaritiques bien precis : gestion des cinematiques, des  //
//				   bruitages, de la trame du scenario principale, des sous-   //
//				   quetes ...												  //
//                                                                            //
//  Copyright (C) 2000 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@ifrance.com		                  //
//  Version      : beta 0.01                                                  //
//  Creation     : 03/09/2000 		                                          //
//  Last upgrade : <none>                                                     //
////////////////////////////////////////////////////////////////////////////////

#ifndef ScenarioObjectH
#define ScenarioObjectH

//---------------------------------------------------------------------- Include
#include "GameObject.h"			// Gestion des objets du jeu

//----------------------------------------------------------------- Define & enum

////////////////////////////////////////////////////////////////////////////////
// Classe SCENARIOOBJECT													  //
class ScenarioObject : public GameObject
{ // --------------------------------------------------------- Attributs priivés
  protected:
	
	// -------------------------------------------------------- Méthodes publiques
  public:
	//--- Constructeurs & Destructeurs ---
	ScenarioObject(int id = START_ID_ANY_SCENARIO);	// Constructeur par defaut
	virtual ~ScenarioObject();						// Destructeur

    
};
//==================== Fin de la classe SCENARIOOBJECT ========================//

//------------------------------------------------------------- Fonctions annexes
#endif	// ScenarioObjectH


