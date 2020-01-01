////////////////////////////////////////////////////////////////////////////////
//  Unité "PhysicInorganicObject.h" : interface de la classe PhysicObject     //
//                                                                            //
//	Description :  Modelisation physique d'un objet inorganic tel qu'une	  //	 
//				   pierre, une table (mais si elle est faite en bois), une    //
//				   fleche, une arme, ...									  //
//				   Notez qu'"inorganic" a plus le sens de non vivants 		  //
//                                                                            //
//  Copyright (C) 2000 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@ifrance.com		                  //
//  Version      : beta 0.01                                                  //
//  Creation     : 27/08/2000 		                                          //
//  Last upgrade : <none>                                                     //
////////////////////////////////////////////////////////////////////////////////

#ifndef PhysicInorganicObjectH
#define PhysicInorganicObjectH

//---------------------------------------------------------------------- Include
#include "PhysicObject.h"	// Modelisation physique d'un objet quelconque

//----------------------------------------------------------------- Define & enum
// Types de classes qui pourront directement dériver de la classe PhysicInorganicObject
// -> mineraux, 
enum
{	PHYSICINORGANICOBJECT_UNDEFINED		= 0,
	PHYSICINORGANICOBJECT_PROJECTILE	= 1
};


//------------------------------------------------------------------------ Class

////////////////////////////////////////////////////////////////////////////////
// Classe PHYSICINORGANICOBJECT
class PhysicInorganicObject : public PhysicObject
{ // ---------------------------------------------------------- Attributs privés
  protected:
	int m_iInorganicType;		// Type de cet objet inorganic

  // ------------------------------------------------------ Méthodes publiques
  public:
	
	//--- Constructeurs & Destructeurs ---
	PhysicInorganicObject();			// Constructeur par defaut
	PhysicInorganicObject(int type);	// Constructeur a utiliser
	virtual ~PhysicInorganicObject();	// Destructeur
};
//================= Fin de la classe PHYSICINORGANICOBJECT ===================//

//------------------------------------------------------------ Fonctions annexes
#endif	// PhysicInorganicObjectH


