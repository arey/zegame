////////////////////////////////////////////////////////////////////////////////
//  Unité  "SoundLib.h" : Manage all the sound effects of ZeGame by using	  //
//						  DirectSound.										  //				
//                                                                            //  
//  Copyright (C) 2000 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com                      //
//					                                                          //
//  Version      : beta 0.01                                                  //
//  Creation     : 16/09/2000                                                 //
//  Last upgrade :															  //
////////////////////////////////////////////////////////////////////////////////

#ifndef SoundLibH
#define SoundLibH

#ifdef MICROSOFT
#pragma warning(disable : 4100 4663 4018 4786 4201)  
#endif


// --------------------------------------------------------------------- Include
#include "DSLib.h"		// Gere tout ce qui touche a DirectSound 
#include <map>			// STL Library to handle xtree
#include <list>			// STL Library to handle linked list

// ---------------------------------------------------------------------- Define
#define	SOUND_MAX_CHAR	32

// ----------------------------------------------------------------------- Class
class TFileVirtual;		// Gestionnaire de fichiers "virtuels"


struct SoundEffect
{	int		iSoundID;							// ID du sound effect
	char	sSoundName[SOUND_MAX_CHAR];			// Path and name of this sound
	
	// La distance minimale se definie ainsi : a partir de la source sonore, a 
	// chaque fois que l'on s'eloigne de cette distance, le volume sonore diminue
	// de moitie (environ 6dB)
	// Ainsi si dMinimumDistance = 100m, lorsque l'on se trouve a 200m, le volume
	// a diminue de 50% et de 75% a 400m. Cela permet de compenser la difference
	// de volume entre le bruit d'une bouche et celui d'un avion au decollage
	double	dMinDistance;
	// Distance a partir de laquelle on considere ce son comme inaudible
	double	dMaxDistance;
};

////////////////////////////////////////////////////////////////////////////////
// Class SoundManager														  //
// Manage all the sound effects of the game									  //
class SoundManager
{	// --- Attributs prives ----------------------------------------------------
	private:
	  std::map<int, SoundEffect*> vSounds;		// Liste de tous les bruitages du jeu
	  bool bSoundGame;							// Indique si les bruitages sont actifs

	  std::list<TDSSoundFile*> vSoundsPlaying;	// Liste des bruitages en cours

	  int   iSFXGlobalVolume;					// Volume general des bruitages (en dB)
	  float fSFXGlobalVolume;					//	 "		 "			 "		(en %)

	// --- Methodes publiques --------------------------------------------------
	public:
	  SoundManager(bool sound=true);			// Constructeur par défaut
	  virtual ~SoundManager();					// Destructeur
	  
	  void SoundEffectManage();					// S'occupe de jouer les bruitages en cours
	  bool playSound(int id);					// Joue un bruitage
	  bool playSound(int id, int volume);		//   "		 "		mais en precisant le volume sonore
	  bool playSound(int id, int vol, int pan); //	 "		 "								"			et la dirence de volume entre les 2 enceintes	
	  bool playSoundVol(int id, float volper);	// Joue un son en precisant le volume sonore en pourentage
	  
	  bool setMutexSynchro(int mutex);			// Attribue un semaphore de synchronisation au dernier son que l'on a rajoute a la liste
	  SoundEffect*	getSoundEffect(int id);		// Renvoie une reference sur les informations du son num ID

	  void	setGlobalVolume(float vol);			// Fixe le volume sonore (en %)
	  void setPause(bool pause);				// Arrete ou reprends le cours de lecture des bruitages
	  
	  // ------------------------------------------------------------- Accessor
	  inline bool isSoundGame() { return bSoundGame; }
	  inline void setSoundGame(bool sound) { bSoundGame = sound; }
	  inline float getGlobalVolume() { return fSFXGlobalVolume; }	// Recupere la valeur courante du volume sonore (en %)

	// --- Methodes privees --------------------------------------------------  
	private:
	  int getDBVolumeFromPercent(float per);	// Convertion d'un volume en % vers dB
};
////////////////////////////////////////////////////////////////////////////////

//--- Instance de la classe utilisée par le jeu ---
extern SoundManager *GameSoundManager;

//------------------------------------------------------------ Fonctions annexes
void InitPack_SoundManager();				// Initialise l'instance du jeu
void DisablePack_SoundManager();			// Désalloue tout

#endif	// SoundLibH
