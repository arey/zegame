////////////////////////////////////////////////////////////////////////////////
//  Unité "PhysicProjectileObject.h" : interface de  PhysicProjectileObject   //
//                                                                            //
//	Description :  Modelisation physique d'un projectile quelconque			  //
//				   fleche, lance, pierre, balle, missile ...				  //
//                                                                            //
//  Copyright (C) 2000 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@ifrance.com		                  //
//  Version      : beta 0.01                                                  //
//  Creation     : 08/09/2000 		                                          //
//  Last upgrade : <none>                                                     //
////////////////////////////////////////////////////////////////////////////////

#ifndef PhysicProjectileObjectH
#define PhysicProjectileObjectH

//---------------------------------------------------------------------- Include
#include "PhysicInorganicObject.h"	// Modelisation physique d'un objet inorganic

//----------------------------------------------------------------- Define & enum
// Types de classes qui pourront directement dériver de la classe PhysicInorganicObject
// -> mineraux, 
enum
{	PHYSICPROJECTILEOBJECT_UNDEFINED	= 0,
	PHYSICPROJECTILEOBJECT_FLECHE		= 1
};


//------------------------------------------------------------------------ Class

////////////////////////////////////////////////////////////////////////////////
// Classe PHYSIC_PROJECTILE_OBJECT											  //
class PhysicProjectileObject : public PhysicInorganicObject
{ // ---------------------------------------------------------- Attributs privés
  protected:
	int m_iProjectileType;		// Type de cet objet inorganic
	
	//--- Proprietes physique communes a tout projectile ---
	float m_fAirResistance;		// Resistance a l'air (grammes/metre/sec)

	//--- 

  // ------------------------------------------------------ Méthodes publiques
  public:
	
	//--- Constructeurs & Destructeurs ---
	PhysicProjectileObject();			// Constructeur par defaut
	PhysicProjectileObject(int type);	// Constructeur a utiliser
	virtual ~PhysicProjectileObject();	// Destructeur
};
//============== Fin de la classe PHYSIC_PROJECTILE_OBJECT ===================//

//------------------------------------------------------------ Fonctions annexes
#endif	// PhysicProjectileObjectH


