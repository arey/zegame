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
#include "WorldSimulation.h"// Paramètrages des lois physiques du monde
#include "INILib.h"			// Permet de lire des .ini
#include "SoundLib.h"		// Header


// ------------------------------------------------------------------- variables

SoundManager *GameSoundManager = NULL;	// Unique instance de la classe SoundManager dans le jeu


////////////////////////////////////////////////////////////////////////////////
//	Classe SOUNDMANAGER													      //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur par défaut ================================================//
SoundManager::SoundManager(bool sound)
{	char		Soundpath[512]; 
	char		SoundpathName[512];
	char		SoundName[128];
	char		SoundID[128];
	char		Section[128];
	AnsiString	name;
				
	// Test si on doit jouer les bruitages pendant le jeu oui ou non ?
	bSoundGame = sound;
	if (!bSoundGame) return;	// si non, pas besoin d'initializer cette instance

	// Récupère le nom des fichiers sonores (wav, mp3,...) utilises pour les bruitages
	TINILib	ini((char*) cIniSoundFiles, true);
	strcpy(Soundpath, LPCSTR(CheminApplication));
	for (std::IDMap::Iterat iter = IDSoundEffect->MapID.begin(); iter != IDSoundEffect->MapID.end(); iter++)
    {	name = (*iter).first;
		int soundID = (*iter).second;
		#ifdef CBUILDER
		strcpy(SoundID, name.c_str());
		#else
		strcpy(SoundID, LPCSTR(name));
		#endif
		// Recupere le chemin relatif du bruitage
		ini.GetStringProfile("Sounds", SoundID, SoundName);
		#ifndef NEW_FILE_MANAGER
		strcpy(SoundpathName, Soundpath);
		strcat(SoundpathName, cPathSounds);
		#else
		strcpy(SoundpathName, cPathSounds);
		#endif // NEW_FILE_MANAGER
		strcat(SoundpathName, SoundName);
		// Ensuite, on recupere la distance minimale (si elle est specifiee)
		strcpy(Section, SoundID);
		strcat(Section, ".MinDistance");
		double minDistance = ini.GetFloatProfile("Sounds", Section, WorldModel.SOUND_DEFAULTMINDISTANCE); 
		// Puis enfin on recupere la distance maximale (si elle est specifiee)
		strcpy(Section, SoundID);
		strcat(Section, ".MaxDistance");
		double maxDistance = ini.GetFloatProfile("Sounds", Section, WorldModel.SOUND_DEFAULTMAXDISTANCE);

		// Cree une nouvelle instance de SoundEffect
		SoundEffect *sound = new SoundEffect();
		sound->iSoundID = soundID;
		sound->dMinDistance = minDistance;
		sound->dMaxDistance = maxDistance;
		strcpy(sound->sSoundName, SoundpathName);
		// Et l'ajoute a la map
		vSounds[sound->iSoundID] = sound;
    }

	// Volume sonore des bruitages initialise a son maximal
	iSFXGlobalVolume = 0;		// 0 dB
	fSFXGlobalVolume = 100.0;	// 100%
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
bool SoundManager::playSound(int id)
{	// Joue ce son avec le volume sonore maximal (celui d'origine)
	// et avec de meme intensite dans les 2 enceintes
	return playSound(id, 0, 0);
}
//----------------------------------------------------------------------------//

//=== Joue un bruitage (precision du volume sonore) ==========================//
bool SoundManager::playSound(int id, int volume)
{	// Joue ce son avec la meme intensite dans les 2 enceintes
	return playSound(id, volume, 0);
}
//----------------------------------------------------------------------------//

//=== Joue un son en precisant le volume sonore en pourentage ================//
bool SoundManager::playSoundVol(int id, float volper)
{	if (volper == 100.0)
	{	return playSound(id, 0, 0);
	}
	return playSound(id, getDBVolumeFromPercent(volper), 0);
}
//----------------------------------------------------------------------------//

//=== Joue un bruitage (precision du volume sonore et de la stereo) ==========//
bool SoundManager::playSound(int id, int volume, int pan)
{	if (!bSoundGame) return false;

	// Test if id exists
	std::map<int, SoundEffect*>::iterator t = vSounds.find(id);
	if (t == vSounds.end()) return false;

	SoundEffect *soundEffect = vSounds[id];
	// Ouvre le fichier sonore
	TDSSoundFile* pSoundFile = TDSMuzikLib::AllocateSoundFile(soundEffect->sSoundName);
	if (pSoundFile == NULL) return false;

	// Et l'ajoute a la liste des bruitages en cours de lecture
	bool res = pSoundFile->Open(soundEffect->sSoundName, soundEffect->sSoundName);
	if (!res) return false;
	vSoundsPlaying.push_back(pSoundFile);

	// Puis on commence enfin a le jouer
	pSoundFile->setPan(pan);								// Reglage de la stereo
	pSoundFile->increaseVolume(volume+iSFXGlobalVolume);	// et du volume
	pSoundFile->Play();
	return true;
}
//----------------------------------------------------------------------------//

//=== Attribue un semaphore de synchronisation au dernier son que l'on a =====//
//	  rajoute a la liste
bool SoundManager::setMutexSynchro(int mutex)
{	// On part a partir de la fin de la liste afin de trouver le bon son
	std::list<TDSSoundFile*>::reverse_iterator riter = vSoundsPlaying.rbegin();
	if (riter == vSoundsPlaying.rend()) return false;
	(*riter)->setEndSoundSynchro(mutex);
	return true;
}
//----------------------------------------------------------------------------//

//=== S'occupe de regarder quels sont les bruitages qui sont entrain d'etre ==//
//    joues et met a jour les buffers sonores								  //
void SoundManager::SoundEffectManage()
{	// Sort directement si les bruitages sont off
	if (!bSoundGame) return;

	std::list<TDSSoundFile*>::iterator iter;
	int count = 0;
	for (iter = vSoundsPlaying.begin() ; iter!=vSoundsPlaying.end() ; )
	{	// Si le bruitage est enc cours de lecture, on continue de le lire
		TDSSoundFile *sound = (*iter);
		if (sound!=NULL)
		{
			count++;
			if (sound->isPlaying())
			{	sound->DSWinMain();
				++iter;
			} else
			// On l'enleve de la liste
			{	delete sound;
				(*iter) = NULL;
				iter =vSoundsPlaying.erase(iter); // [A TESTER] risque de bug a cause des effets de bord
			}
		} else
		{	++iter;
		}
	}
}
//----------------------------------------------------------------------------//

//=== Renvoie une reference sur les informations du son num ID ===============//
SoundEffect* SoundManager::getSoundEffect(int id)
{	return vSounds[id];
}
//----------------------------------------------------------------------------//

//=== Fixe le volume sonore (en %) ===========================================//
void SoundManager::setGlobalVolume(float vol)
{	fSFXGlobalVolume = vol;
	// Convertion % -> dB
	iSFXGlobalVolume = getDBVolumeFromPercent(vol);
}
//----------------------------------------------------------------------------//

//=== Convertion d'un volume en % vers dB ====================================//
int SoundManager::getDBVolumeFromPercent(float per)
{	// Rmq: je fais ca lineairement pour le moment
	// a 100% -> 0dB et a 0% -> -5000dB
	return (int) (-50*(100-per));
}
//----------------------------------------------------------------------------//

//=== Arrete ou reprends tous les bruitages ==================================//
void SoundManager::setPause(bool pause)
{	if (!bSoundGame) return;

	std::list<TDSSoundFile*>::iterator iter;
	for (iter = vSoundsPlaying.begin() ; iter!=vSoundsPlaying.end() ; )
	{	TDSSoundFile *sound = (*iter);
		if (sound!=NULL)
		{	sound->SetPause(pause);
		}
		++iter;
	}
}
//----------------------------------------------------------------------------//


//////////////////////// Fin de la classe SoundLib /////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//							Fonctions EXTERNES								 //
///////////////////////////////////////////////////////////////////////////////

//=== Initialise la liste des bruitages du jeu ==============================//
void InitPack_SoundManager()
{	// Recupere quelques parametres de ZeGame.ini
	char sSFX[128];
	TINILib	ini((char*) cIniZeGame);
	ini.GetStringProfile("Audio", "SFX", sSFX);
	bool bSFX= (strcmp(sSFX, "Yes") == 0) ;
	
	// Instancie le manager des effets sonores
	GameSoundManager = new SoundManager(bSFX);

	// Volume sonore des bruitages
	float volume = (float) ini.GetFloatProfile("Audio", "SFXVolume", 100.0);
	if (volume<0) volume = 0;
	else if (volume>100) volume = 100;
	GameSoundManager->setGlobalVolume(volume);
}
//----------------------------------------------------------------------------//

//=== Referme proprement le sound effect manager =============================//
void DisablePack_SoundManager()
{	// Desinstalle le manager des effets sonores
	delete GameSoundManager;	GameSoundManager = NULL;
}
//----------------------------------------------------------------------------//
