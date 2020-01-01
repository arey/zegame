////////////////////////////////////////////////////////////////////////////////
//  Unité "PhysicObjectManager.h" : interface de la classe PhysicObjectManager//
//                                                                            //
//	Description :  Cette classe est un singleton qui manage les differents    //
//				   modeles PhysicObject  et permet d'en creer a la volee	  //
//				   permettant de, par exemple, gerer les cinematiques, les	  //
//				   ayant les meme caracteristiques que ces derniers.		  //
//                                                                            //
//  Copyright (C) 2000 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@ifrance.com		                  //
//  Version      : beta 0.01                                                  //
//  Creation     : 28/09/2000 		                                          //
//  Last upgrade : <none>                                                     //
////////////////////////////////////////////////////////////////////////////////

#ifndef PhysicObjectManagerH
#define PhysicObjectManagerH

//---------------------------------------------------------------------- Include
#include "Singleton.h"	// Template de toutes les classes singletons
#include <map>			// STL Library to handle xtree
#include <string>		// STL Library that handle text string

//----------------------------------------------------------------------- Define
#define g_PhysicObjectManager PhysicObjectManager::getSingleton()

//------------------------------------------------------------------------- Enum
enum
{	CLASS_PhysicProjectileObject = 1
};

//------------------------------------------------------------------------ Class
class PhysicObject;		// Classe modelisant la physique d'un objet


////////////////////////////////////////////////////////////////////////////////
// Classe PHYSICOBJECTMANAGER
class PhysicObjectManager : public Singleton <PhysicObjectManager>
{ // ---------------------------------------------------------- Attributs privés
  protected:
	int m_iPhysicObjectNextID;								// Identificateur a attribuer au prochain PhysicObject qui sera cree
	std::map<int, PhysicObject*> mTemplatePhysicObjects;	// Librairie de modeles d'objets physiques
	std::map<std::string, PhysicObject*> mTemplatePhysicObjectsByName;	
	typedef std::map<int, PhysicObject*>::iterator iterTemplatePhysicObjects;

  // ------------------------------------------------------ Méthodes publiques
  public:
	
	//--- Constructeurs & Destructeurs ---
	PhysicObjectManager();				// Constructeur par defaut
	virtual ~PhysicObjectManager();		// Destructeur

	void Init();							// Initialise tous les modeles d'objets physiques du jeu
	PhysicObject* CreateObject(char* name);	// Cree une instance de cette classe a partir de son nom
	PhysicObject* CreateObject(int classID);//	 "		   "				"		 "	   "  "	 identifiant

  // -------------------------------------------------------- Méthodes privees
  protected:
	int getIDFormClassName(char* name);		// Renvoi l'ID correspondant au nom d'une classe
	void addTemplate(PhysicObject* pPO);	// Rajoute un modele de PhysicObject
    
};
//================= Fin de la classe PHYSICOBJECTMANAGER =====================//

//------------------------------------------------------------ Fonctions annexes
void InitGame_PhysicObjectManager();		// Initialise le manager des objets physiques
void CloseGame_PhysicObjectManager();		// Referme proprement le manager

#endif	// PhysicObjectManagerH


