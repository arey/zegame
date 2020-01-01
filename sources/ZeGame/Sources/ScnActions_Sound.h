////////////////////////////////////////////////////////////////////////////////
//  Unité "ScnActions_SoundH.h"												  //
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

#ifndef ScnActions_SoundH
#define ScnActions_SoundH

//---------------------------------------------------------------------- Include

//----------------------------------------------------------------------- Define
#ifdef CBUILDER
#define NULL	0
#endif

//------------------------------------------------------------------------- Enum

//---------------------------------------------------------------------- Classes
class GameObject;		// Classe mère de tous les objets du jeu
class TRunningScript;	// Structure complétée par l'ExecuteAction de la ScriptLine 
class RandomFloatType;	// Nombre reel aleatoire


// TACTION_SOUND_PLAY  /////////////////////////////////////////////////////////
// Joue un bruitage
class TAction_SoundPlay : public TAction
{	int iSoundID;				// ID du bruitage a jouer (cf sound\SoundFiles.ini
	//float fVolume;			// Volume auquel il faut jouer le son (en dB)
	RandomFloatType* rfVolume;	// Volume aleatoire auquel il faut jouer le son (en dB)
	
  public:
  	// Constructeurs et destructeur
	TAction_SoundPlay(int id, float volume=100);
	TAction_SoundPlay(int id, RandomFloatType* volume);
	 ~TAction_SoundPlay();						// Destructeur
    // Joue le bruitage
	void PlayAction(TRunningScript &RunningAction, GameObject *pGO = NULL);
};
///////////////////// Fin de TACTION_SOUND_PLAY ////////////////////////////////



#endif	// ScnActions_SoundH

