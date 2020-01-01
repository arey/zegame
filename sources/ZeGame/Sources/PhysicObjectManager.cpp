////////////////////////////////////////////////////////////////////////////////
//  Unité "PhysicObjectManager.cpp" : implemenation de PhysicObjectManager	  //
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

#include "ZeNain.h"					//--- Special for Visual C++ 6 MFC ---
#include "consts.h"					// Constantes globales à tout le projet
#include "INILib.h"					// Lib. de gestion des fichiers .ini 
#pragma hdrstop
#include "PhysicObject.h"			// Classe modelisant la physique d'ub objet
#include "PhysicProjectileObject.h"	// Classe de modelisation de projectiles
#include "PhysicObjectManager.h"	// Header de la classe


////////////////////////////////////////////////////////////////////////////////
// Classe PHYSICOBJECTMANAGER                                                 //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur par defaut ================================================//
PhysicObjectManager::PhysicObjectManager()
{	// Le 1er identificateur a attribuer au prochain PhysicObject qui sera cree
	// commence a 1 (0 etant reserve a ceux cree sans etre passe par un manager)
	m_iPhysicObjectNextID = 1;
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
PhysicObjectManager::~PhysicObjectManager()
{	mTemplatePhysicObjects.clear();
	mTemplatePhysicObjectsByName.clear();
}	
//----------------------------------------------------------------------------//

//=== Initialise tous les modeles d'objets physiques du jeu ==================//
void PhysicObjectManager::Init()
{	char settingsPath[ZENAIN_MAX_CHAR];
	char settingsFilename[ZENAIN_MAX_CHAR];
	char sectionName[ZENAIN_MAX_CHAR];
	char labels[ZENAIN_MAX_CHAR];
	int hr;

	// Chemin d'acces aux fichiers de config
	//strcpy(settingsPath, LPCSTR(CheminApplication));
	strcpy(settingsPath, cPathWorld);
	strcat(settingsPath, cPathSettings);

	// Lit le contenu d'un fichier de configuration
	strcpy(settingsFilename, settingsPath);
	strcat(settingsFilename, "Projectile.cfg");
	TINILib	ini(settingsFilename);
	ini.begin();
	while (ini.getNextSection(sectionName)!=-1)
	{	labels[0]='\0';
		hr = ini.GetStringProfile(sectionName, "Class", labels);
		if (hr==-1) continue;		// Class non definie

		// Instancie cet objet
		PhysicObject* pPO = CreateObject(labels);
		if (pPO==NULL) continue;	// Nom de classe inconnue

		// Parametres de l'objet
		// - Nom
		ini.GetStringProfile(sectionName, "Name", labels, "<none>");
		pPO->setName(labels);
		// - Description
		ini.GetStringProfile(sectionName, "Description", labels, "<none>");
		pPO->setDescription(labels);	
		

	}
}
//----------------------------------------------------------------------------//

//=== Cree une instance de cette classe ======================================//
// @param	name	nom de la classe dont on souhaite creer l'instance	(input)
// @return	ptr sur une instance de la classe derivee de PhysicObject demandee (NULL si inexistante)
PhysicObject* PhysicObjectManager::CreateObject(char* name)
{	int classID = getIDFormClassName(name);
	return (classID==-1) ? NULL : CreateObject(classID);
}
//----------------------------------------------------------------------------//

//=== Cree une instance de cette classe ======================================//
// @param	classID	identifiant de la classe dont on souhaite creer l'instance	(input)
// @return	ptr sur une instance de la classe derivee de PhysicObject demandee (NULL si inexistante)
PhysicObject* PhysicObjectManager::CreateObject(int classID)
{	PhysicObject* pClass = NULL;
	
	switch(classID)
	{	case CLASS_PhysicProjectileObject :
			pClass = new PhysicProjectileObject();
			break;
	default:
			return NULL;
	}
	// Attribution d'un ID
	pClass->setPhysicID(m_iPhysicObjectNextID++);
	return pClass;
}
//----------------------------------------------------------------------------//


//=== Renvoi l'ID correspondant au nom d'une classe ==========================//
// @param	name	nom de la classe dont on cherche le numero (input)
// @return	numero de la classe recherchee (-1 si introuvable)
int PhysicObjectManager::getIDFormClassName(char* name)
{	
	if (strcmp(name, "PhysicProjectileObject")==0) return CLASS_PhysicProjectileObject;
	return -1;
}
//----------------------------------------------------------------------------//

//=== Rajoute un modele de PhysicObject ======================================//
// @param pGO	Modele de PhysicObject a rajouter a mTemplatePhysicObjects
void PhysicObjectManager::addTemplate(PhysicObject* pPO)
{	mTemplatePhysicObjects[pPO->getPhysicID()] = pPO;
	mTemplatePhysicObjectsByName[pPO->getName()] = pPO;
}
//----------------------------------------------------------------------------//

//////////////////////// Fin de PHYSICOBJECTMANAGER ////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Fonctions ANNEXES														  //
////////////////////////////////////////////////////////////////////////////////

//=== Initialise le manager du Scenarios =====================================//
void InitGame_PhysicObjectManager()
{	PhysicObjectManager *pPOM = new PhysicObjectManager();
	pPOM->Init();
}
//----------------------------------------------------------------------------//

//=== Referme proprement le manager du Scenarios =============================//
void CloseGame_PhysicObjectManager()
{	PhysicObjectManager *pPOM = PhysicObjectManager::getSingletonPtr();
	delete pPOM;
}
//----------------------------------------------------------------------------//

