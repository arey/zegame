////////////////////////////////////////////////////////////////////////////////
//  Unité "ScnActions.cpp" implémentation de la classe TAction 				  //
//                                                                            //
//	Description :                                                             //
//                                                                            //
//  Copyright (C) 1999 Antoine Rey    arey@insa-lyon.fr                       //
//                                    antrey@club-internet.fr                 //
//  Version      : beta 0.20                                                  //
//  Creation     : 14/07/1999 		                                          //
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
#include "ImDecors2.h"		// Gestion des décors
#include "PersoLib.h"		// Gestion des personnages
#include "GameObject.h"		// Gestion des objets du jeu
#include "GameObjectList.h"	// Liste des GameObject du jeu
#include "PNJLib.h"			// Gestion des PNJ
#include "MemLib.h"			// Lib. d'outils de communications
#include "ScnScript.h"		// Librairie des Scripts du jeu
#include "TimeClock.h"		// Lib. de gestion temporelle du jeu
#include "WorldSimulation.h"// Paramètrages des lois physiques du monde
#include "Animall.h"		// Lib. d'animations
#include "ScnGame.h"		// Fonctionnalités liées au scénario
#pragma hdrstop
#include "ScnConsts.h"		// Constantes liées à la gestion du scénario
#include "ScnActions.h"		// Rassemble un certains nombre d'actions elementaires
#include "ScnActions_PersosMove.h"		// Header du fichier


////////////////////////////////////////////////////////////////////////////////
// Classe TACTION_PERSOS_MOVE		                                          //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur ===========================================================//
TAction_PersosMove::TAction_PersosMove(int moveType, double posX, double posY, int persoID, int radius):
	iPersoID(persoID),
	iMoveType(moveType),
	iRadius(radius)
{	switch (iMoveType)
	{	case (ACTION_PERSOSMOVE_DESTINATION_XY) :
			fPosX = posX; fPosY = posY; 
			break;
		case (ACTION_PERSOSMOVE_OFFSET) :
			fDepX = posX; fDepY = posY; 
			break;
	}
}
TAction_PersosMove::TAction_PersosMove(int moveType, int posU, int posV, int persoID, int radius):
	iPersoID(persoID),
	iMoveType(moveType),
	iRadius(radius)
{	switch (iMoveType)
	{	case (ACTION_PERSOSMOVE_DESTINATION_UV) :
			iPosU = posU; iPosV = posV; 
			break;
	}
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TAction_PersosMove::~TAction_PersosMove()
{	}
//----------------------------------------------------------------------------//

//=== Changement de l'état actuel du décors ==================================//
void TAction_PersosMove::PlayAction(TRunningScript &RunningAction, GameObject *pGO)
{	TAction::PlayAction(RunningAction);

	PersoPNJ* perso = NULL;
	double destU=0, destV=0;
	double fNewPosX, fNewPosY;
	
	// Détermine si c'est le perso qui a déclenché l'action ou un autre 
	// qui doit se déplacer
	if (iPersoID == 0)
	{	// Si c'est le perso du script
		perso = (PersoPNJ*) pGO;
	} else
	// Sinon, ca peut être le dernier perso manipulé (créé, déplacer) du script
	if (iPersoID == -1)
	{	perso = (PersoPNJ*) GOList->GetGameObject(RunningAction.iLastGoID);
	} else
	{	perso = (PersoPNJ*) GOList->GetGameObject(iPersoID);
	}
	
	// Récupère les coordonnées (U,V) où il doit se rendre
	switch (iMoveType)
	{	case (ACTION_PERSOSMOVE_DESTINATION_XY) :
			// Détermine le cas d'un random
			fNewPosX = fPosX;
			fNewPosY = fPosY;
			if (iRadius != 0)
			{	fNewPosX = fNewPosX + ((float) (rand() % (iRadius*2*10)))/10 - iRadius;
				fNewPosY = fNewPosY + ((float) (rand() % (iRadius*2*10))) /10 - iRadius;	
			}
			destU = fNewPosX / WorldModel.SQR_LENGTH; 
			destV = fNewPosY / WorldModel.SQR_LENGTH;
			break;
		case (ACTION_PERSOSMOVE_OFFSET) :
			destU = perso->PersoNivU + fDepX / WorldModel.SQR_LENGTH;
			destV = perso->PersoNivV + fDepY / WorldModel.SQR_LENGTH;
			break;
		case (ACTION_PERSOSMOVE_DESTINATION_UV) :
			destU = iPosU;
			destV = iPosV; 
			break;
	}
		
	// Déplace le PNJ
	perso->MoveTo((int) destU,(int) destV);
}
//----------------------------------------------------------------------------//
////////////////////// Fin de la classe TACTION_PERSOS_MOVE ////////////////////



