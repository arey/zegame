////////////////////////////////////////////////////////////////////////////////
//  Unité "ScnActions_Sound.cpp" implentation du .h du meme nom				  //
//                                                                            //
//	Description : Les classes de ce fichier derivent toutes de la classe	  //
//				  TAction et sont liees a la gestion du son et de la musique. //
//				  - ScnActions_SoundPlay : joue un bruitage					  //
//                                                                            //
//  Copyright (C) 2000 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@ifrance.com		                  //
//  Version      : beta 0.10                                                  //
//  Creation     : 05/08/2000 		                                          //
//  Last upgrade : <none>	                                                  //
////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------- Include
#include "ZeNain.h"         
#ifndef MICROSOFT
// Unité principale du projet
#ifdef MAPEDITOR
#include "uMapView.h"
#else
#include "DirectLib.h"
#endif  // MAPEDITOR
#endif
#include "GameObject.h"			// Gestion des objets du jeu
#include "GameObjectList.h"		// Liste des GameObject du jeu
#pragma hdrstop
#include "ScnConsts.h"			// Constantes liées à la gestion du scénario
#include "ScnActions.h"			// Rassemble un certains nombre d'actions elementaires
#include "ScnScript.h"			// Librairie des Scripts du jeu
#include "SoundEffect.h"		// Manager des bruitages du jeu
#include "SoundLib.h"			// Gere les banques de bruitages sonores
#include "PersoLib.h"           // Définitions, Actions & Etats des personnages
#include "ImDecors2.h"			// Modelisation des décors
#include "RandomType.h"			// Types aleatoires
#include "ScnActions_Sound.h"	// Header du fichier


////////////////////////////////////////////////////////////////////////////////
// Classe TACTION_PERSOS_MOVE		                                          //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeurs ==========================================================//
TAction_SoundPlay::TAction_SoundPlay(int id, float volume):
	iSoundID(id)
{	rfVolume = new RandomFloatType(volume);
}

TAction_SoundPlay::TAction_SoundPlay(int id, RandomFloatType* volume):
	iSoundID(id), rfVolume(volume) 
{ }
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TAction_SoundPlay::~TAction_SoundPlay()
{	delete rfVolume;
}
//----------------------------------------------------------------------------//

//=== Joue le son ============================================================//
void TAction_SoundPlay::PlayAction(TRunningScript &RunningAction, GameObject *pGO)
{	TAction::PlayAction(RunningAction);
	if (pGO==NULL) return;

	if ((pGO->GetObjectType()==GAMEOBJECT_PERSO) || (pGO->GetObjectType()==GAMEOBJECT_DECORS))
	{	// C'est un objet ou un perso qui est a l'origine de ce son, donc
		// je recupere sa position afin de calculer le volume sonore et
		// regler la stereo
		double posX=0, posY=0;
		switch (pGO->GetObjectType())
		{	case GAMEOBJECT_PERSO :
				posX = ((Personnage*) pGO)->PersoNivX;
				posY = ((Personnage*) pGO)->PersoNivY;
				break;
			case GAMEOBJECT_DECORS :
				ConvNivUV2XY(((DecorsObjet*) pGO)->PosU, ((DecorsObjet*) pGO)->PosV, posX, posY);
				break;
		}
		GameSoundEffectManager->playSoundFrom(iSoundID, posX, posY);
	} else 
	if (pGO->GetObjectType()==GAMEOBJECT_SCENARIO) 
	{	// Sinon, ca peut etre un bruitage d'ambiance musicale n'ayant
		// pas vraiment d'origine -> on utilise le volume
		GameSoundManager->playSoundVol(iSoundID, rfVolume->random());
	}

	if (bEndSynchro)
	{	GameSoundManager->setMutexSynchro(RunningAction.iMutexID[RunningAction.iPosition]);
	}	
}
//----------------------------------------------------------------------------//
////////////////////// Fin de la classe TACTION_PERSOS_MOVE ////////////////////



