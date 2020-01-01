////////////////////////////////////////////////////////////////////////////////
//  Unité "PhysicObject.h" : interface de la classe PhysicObject              //
//                                                                            //
//	Description :  Modelisation d'un objet quelconque, du monde mineral,	  //
//				   vegetal ou animal.										  //
//                                                                            //
//  Copyright (C) 2000 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@ifrance.com		                  //
//  Version      : beta 0.01                                                  //
//  Creation     : 27/08/2000 		                                          //
//  Last upgrade : <none>                                                     //
////////////////////////////////////////////////////////////////////////////////

#ifndef PhysicObjectH
#define PhysicObjectH

//---------------------------------------------------------------------- Include

//----------------------------------------------------------------- Define & enum

////////////////////////////////////////////////////////////////////////////////
// Classe PHYSICOBJECT
class PhysicObject
{ // --------------------------------------------------------- Attributs priivés
  protected:
	
	// -------------------------------------------------------- Méthodes publiques
  public:
    //--- Fonctions intervenant sur la Mémoire de l'Object ---
  	
	//--- Constructeurs & Destructeurs ---
	PhysicObject();						// Constructeur par defaut
	virtual ~PhysicObject();			// Destructeur

    
};
//===================== Fin de la classe PHYSICOBJECT =========================//

//------------------------------------------------------------- Fonctions annexes
#endif	// PhysicObjectH


