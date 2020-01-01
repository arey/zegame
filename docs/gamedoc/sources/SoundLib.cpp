////////////////////////////////////////////////////////////////////////////////
//  Unité  "SoundLib.cpp" : Manage all the sound effects of ZeGame by using	  //
//							DirectSound.									  //				
//                                                                            //  
//  Copyright (C) 2000 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com                      //
//					                                                          //
//  Version      : beta 0.01                                                  //
//  Creation     : 16/09/2000                                                 //
//  Last upgrade :															  //
////////////////////////////////////////////////////////////////////////////////


// --------------------------------------------------------------------- include
#include "ZeNain.h"			// Parametrages du projet
#include "Consts.h"			// Constantes globales du jeu
#include "SoundLib.h"		// Header


// ------------------------------------------------------------------- variables



////////////////////////////////////////////////////////////////////////////////
//	Classe SOUNDMANAGER													      //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur par défaut ================================================//
SoundManager::SoundManager(bool sound)
{	// Test si on doit jouer les bruitages pendant le jeu oui ou non ?
	bSoundGame = sound;
	if (!bSoundGame) return;	// si non, pas besoin d'initializer cette instance

	// Récupère les fichiers wav de bruitages
	for (IDSoundEffect->MapIDIter = IDSoundEffect->MapID.begin(); IDSoundEffect->MapIDIter!=IDSoundEffect->MapID.end(); IDSoundEffect->MapIDIter++)
    {   // Cree une nouvelle instance de SoundEffect
		SoundEffect *sound = new SoundEffect();
		sound->iSoundID = (*(IDSoundEffect->MapIDIter)).second;
		#ifdef CBUILDER
		strcpy(sound->sSoundName, (*IDSoundEffect->MapIDIter).first.c_str());
		#else
		strcpy(sound->sSoundName, LPCSTR((*IDSoundEffect->MapIDIter).first));
		#endif
		// Puis on l'ajoute a la map
		vSounds[sound->iSoundID] = sound;
    }

}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
SoundManager::~SoundManager()
{	for (int i=0; i < vSounds.size() ; i++)
	{	delete vSounds[i];
	}
	vSounds.clear();
}
//----------------------------------------------------------------------------//

//=== Joue un bruitage =======================================================//
void SoundManager::playSound(int id)
{	
}
//----------------------------------------------------------------------------//

//////////////////////// Fin de la classe SoundLib /////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//							Fonctions EXTERNES								 //
///////////////////////////////////////////////////////////////////////////////
