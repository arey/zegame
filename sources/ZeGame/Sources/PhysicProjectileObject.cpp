////////////////////////////////////////////////////////////////////////////////
//  Unité "PhysicProjectileObject.cpp": implementation de  PhysicProjectileObject //
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

#include "ZeNain.h"					//--- Special for Visual C++ 6 MFC ---
#pragma hdrstop
#include "PhysicProjectileObject.h"	// Header de la classe


////////////////////////////////////////////////////////////////////////////////
// Classe PHYSIC_PROJECTILE_OBJECT                                            //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur par defaut ================================================//
PhysicProjectileObject::PhysicProjectileObject():
	PhysicInorganicObject(PHYSICINORGANICOBJECT_PROJECTILE),
	m_iProjectileType(PHYSICPROJECTILEOBJECT_UNDEFINED)
{
}
//----------------------------------------------------------------------------//

//=== Constructeur a utiliser ================================================//
PhysicProjectileObject::PhysicProjectileObject(int type):
	PhysicInorganicObject(PHYSICINORGANICOBJECT_PROJECTILE),
	m_iProjectileType(type)
{	
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
PhysicProjectileObject::~PhysicProjectileObject()
{
}	
//----------------------------------------------------------------------------//

//////////////////////// Fin de PHYSIC_PROJECTILE_OBJECT ////////////////////////



////////////////////////////////////////////////////////////////////////////////
// Fonctions ANNEXES														  //
////////////////////////////////////////////////////////////////////////////////

