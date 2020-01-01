////////////////////////////////////////////////////////////////////////////////
//  Unité "PhysicObjectManager.cpp" : implemenation de la classe du meme nom  //
//                                                                            //
//	Description :  Cette classe est un singleton qui manage les differents    //
//				   effets pre-programmes ds ZeGame pour un joystick a retour  //
//				   de force.												  //
//                                                                            //
//  Copyright (C) 2000 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@ifrance.com		                  //
//  Version      : beta 0.01                                                  //
//  Creation     : 16/09/2000 		                                          //
//  Last upgrade : <none>                                                     //
////////////////////////////////////////////////////////////////////////////////

#include "ZeNain.h"					//--- Special for Visual C++ 6 MFC ---
#include "consts.h"					// Constantes globales à tout le projet
#include "PersoLib.h"				// Personnages et modèles de persos du jeu
#include "ImDecors2.h"				// Modelisation des differents décors du jeu
#include "AnimAll.h"				// Librairie de lecture/compilation des animations et de leurs transitions
#pragma hdrstop
#include "ForceFeedbackEffect.h"				// Classe parente a tout effet pour joy a retour de force
#include "ForceFeedbackEffect_BumpIntoWall.h"	// Effet utilise lorsque le joueur se cogne contre un mur
#include "ForceFeedbackEffectID.h"				// ID de tous mes effets
#include "ForceFeedbackEffectManager.h"			// Header de la classe


////////////////////////////////////////////////////////////////////////////////
// Classe FORCEFEEDBACKEFFECTMANAGER                                          //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur par defaut ================================================//
ForceFeedbackEffectManager::ForceFeedbackEffectManager(LPDIRECTINPUTDEVICE2 joy):
	lpDevice(joy)
{	
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
ForceFeedbackEffectManager::~ForceFeedbackEffectManager()
{	Clear();
}	
//----------------------------------------------------------------------------//

//=== Initialise tous les modeles d'objets physiques du jeu ==================//
void ForceFeedbackEffectManager::Init()
{	ForceFeedbackEffect *effect = NULL;
	
	effect = new ForceFeedbackEffect_BumpIntoWall(ID_FORCEFEEDBACKEFFECT_BUMP_INTO_WALL_1, lpDevice);
	mForceFeedbackEffects[effect->getEffectID()] = effect;
}
//----------------------------------------------------------------------------//

//=== Desinstancie tous les ForceFeedbackEffect ==============================//
void ForceFeedbackEffectManager::Clear()
{	std::map<int, ForceFeedbackEffect*>::iterator iter;
	for (iter = mForceFeedbackEffects.begin(); iter!=mForceFeedbackEffects.end() ; iter++)
    {   delete (*iter).second;
		(*iter).second = NULL;
    }       
	mForceFeedbackEffects.clear();
}
//----------------------------------------------------------------------------//

//=== Joue l'effet a retour de force approprie lorsque =======================//
// - le joueur fonce dans un mur (ou tout objet du decors)
void ForceFeedbackEffectManager::playBumpIntoWall(Personnage* player, DecorsObjet* wall)
{	// Suivant l'orientation du personnage, la vitesse a laquelle il s'est heurte
	// et la resistance du mur, on determine l'effet a retour de force a executer

	int angle = getAngleFromDirection(player->peDirection);
	ForceFeedbackEffect_BumpIntoWall* effect = (ForceFeedbackEffect_BumpIntoWall*) mForceFeedbackEffects[ID_FORCEFEEDBACKEFFECT_BUMP_INTO_WALL_1];
	effect->Play(angle);
}
//----------------------------------------------------------------------------//

//=== Renvoi l'angle (en degre) de la direction correspondante ===============//
int ForceFeedbackEffectManager::getAngleFromDirection(int direction)
{	
	switch(direction)
	{	case or_N  : return 0;
		case or_NE : return 45;
		case or_E  : return 90;
		case or_SE : return 135;
		case or_S  : return 180;
		case or_SO : return 225;
		case or_O  : return 270;
		case or_NO : return 315;
	}
	return 0;
}
//----------------------------------------------------------------------------//

///////////////////// Fin de FORCEFEEDBACKEFFECTMANAGER ////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Fonctions ANNEXES														  //
////////////////////////////////////////////////////////////////////////////////

//=== Initialise le manager du Scenarios =====================================//
void InitGame_ForceFeedbackEffectManager()
{	if (DI==NULL) return;
	if (!DI->isForceFeedback()) return;
	ForceFeedbackEffectManager *pFFEM = new ForceFeedbackEffectManager(DI->getJoystickDevice());
	pFFEM->Init();
}
//----------------------------------------------------------------------------//

//=== Referme proprement le manager du Scenarios =============================//
void CloseGame_ForceFeedbackEffectManager()
{	ForceFeedbackEffectManager *pFFEM = ForceFeedbackEffectManager::getSingletonPtr();
	delete pFFEM;
}
//----------------------------------------------------------------------------//

