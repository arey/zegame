////////////////////////////////////////////////////////////////////////////////
//  Unité "PhysicInorganicObject.cpp": implementation de PhysicInorganicObject//
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

#include "ZeNain.h"					//--- Special for Visual C++ 6 MFC ---
#pragma hdrstop
#include "PhysicInorganicObject.h"	// Header de la classe


////////////////////////////////////////////////////////////////////////////////
// Classe PHYSICINORGANICOBJECT                                               //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur par defaut ================================================//
PhysicInorganicObject::PhysicInorganicObject():
	PhysicObject(PHYSICOBJECT_INORGANIC),
	m_iInorganicType(PHYSICINORGANICOBJECT_UNDEFINED)
{
}
//----------------------------------------------------------------------------//

//=== Constructeur a utiliser ================================================//
PhysicInorganicObject::PhysicInorganicObject(int type):
	PhysicObject(PHYSICOBJECT_INORGANIC),
	m_iInorganicType(type)
{	
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
PhysicInorganicObject::~PhysicInorganicObject()
{
}	
//----------------------------------------------------------------------------//

//////////////////////// Fin de PHYSICINORGANICOBJECT //////////////////////////



////////////////////////////////////////////////////////////////////////////////
// Fonctions ANNEXES														  //
////////////////////////////////////////////////////////////////////////////////

