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
#include <string>

//----------------------------------------------------------------- Define & enum

// Types de classes qui pourront directement dériver de la classe PhysicObject : objets vivants ou mineraux
enum
{	PHYSICOBJECT_UNDEFINED	= 0,
	PHYSICOBJECT_ORGANIC	= 1,
	PHYSICOBJECT_INORGANIC	= 2
};

//------------------------------------------------------------------------ Class
class Personnage;

////////////////////////////////////////////////////////////////////////////////
// Classe PHYSICOBJECT
class PhysicObject
{ // ---------------------------------------------------------- Attributs privés
  protected:
	int		m_iPhysicID;			// Identificateur unique d'un PhysicObject
	int		m_iPhysicType;			// Type de cet objet : organic ou inorganic
	std::string	m_sName;			// Nom de l'objet
	std::string	m_sDescription;		// Description succincte

	//--- Proprietes physique d'un objet ---
	float m_fWeight;			// Masse de l'objet (en grammes)
	float m_fVolume;			// Volume au repos de l'objet (en metre cube)


	//--- Variables provisoires ---
	int iLife;					// Points de vie
	bool bDead;					// vrai si le perso est mort	(-> a mettre ds OrganicObject)
	
  // ------------------------------------------------------ Méthodes publiques
  public:
	
	//--- Constructeurs & Destructeurs ---
	PhysicObject();						// Constructeur par defaut
	PhysicObject(int type);				// Constructeur a utiliser
	virtual ~PhysicObject();			// Destructeur

	// Appele quand cet objet a ete frappe par un personnage : calcul, puis deduit les degats
	virtual void hitBy(Personnage* opponent) { };

	//--- Accessor sur les attributs de l'objet ---
	inline void setPhysicID(int id) { m_iPhysicID = id; }					// PhysicID
	inline int getPhysicID() { return m_iPhysicID; }
	inline bool isDead()  { return bDead; }									// bDead
	inline bool isAlive() { return !bDead; }
	inline void setName(std::string name) { m_sName = name; }				// Nom
	inline std::string getName() { return m_sName; }
	inline void setDescription(std::string desc) { m_sDescription = desc; }	// Description
	inline std::string getDescription() { return m_sDescription; }

};
//===================== Fin de la classe PHYSICOBJECT ========================//

//------------------------------------------------------------ Fonctions annexes
#endif	// PhysicObjectH


