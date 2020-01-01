////////////////////////////////////////////////////////////////////////////////
//  Unité "PhysicObject.h" : implementation de la classe PhysicObject         //
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

#include "ZeNain.h"         //--- Special for Visual C++ 6 MFC ---
#pragma hdrstop
#include "PhysicObject.h"	// Header de la classe


////////////////////////////////////////////////////////////////////////////////
// Classe PHYSICOBJECT                                                        //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur par defaut ================================================//
PhysicObject::PhysicObject():
	m_iPhysicType(PHYSICOBJECT_UNDEFINED)
{	m_iPhysicID = 0;	// ID non defini
}
//----------------------------------------------------------------------------//

//=== Constructeur a utiliser ================================================//
PhysicObject::PhysicObject(int type):
	m_iPhysicType(type)
{	// Variables provisoires
	iLife = 100;
	bDead = false;
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
PhysicObject::~PhysicObject()
{
}	
//----------------------------------------------------------------------------//

//////////////////////////// Fin de PHYSICOBJECT ///////////////////////////////



////////////////////////////////////////////////////////////////////////////////
// Fonctions ANNEXES														  //
////////////////////////////////////////////////////////////////////////////////

