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

#include "ZeNain.h"         //--- Special for Visual C++ 6 MFC ---
#pragma hdrstop
#include "ScenarioObject.h"	// Header de la classe


////////////////////////////////////////////////////////////////////////////////
// Classe SCENARIOOBJECT                                                      //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur par defaut ================================================//
ScenarioObject::ScenarioObject(int id):
		GameObject(id)
{	
}
//----------------------------------------------------------------------------//


//=== Destructeur ============================================================//
ScenarioObject::~ScenarioObject()
{
}	
//----------------------------------------------------------------------------//

/////////////////////////// Fin de SCENARIOOBJECT //////////////////////////////



////////////////////////////////////////////////////////////////////////////////
// Fonctions ANNEXES														  //
////////////////////////////////////////////////////////////////////////////////

