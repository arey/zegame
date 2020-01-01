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

// ---------------------------------------------------------------------- Define
#define	SOUND_MAX_CHAR	32

// ----------------------------------------------------------------------- Class
class TFileVirtual;		// Gestionnaire de fichiers "virtuels"


struct SoundEffect
{	int		iSoundID;						// ID du sound effect
	char	sSoundName[SOUND_MAX_CHAR];		// Path and name of this sound
};


class SoundManager
{	private:
	  std::map<int, SoundEffect*> vSounds;	// Liste de tous les bruitages du jeu
	  bool bSoundGame;						// Faut-il jouer les bruitages ?

	public:
	  SoundManager(bool sound=true);		// Constructeur par défaut
	  virtual ~SoundManager();				// Destructeur

	  void playSound(int id);				// Joue un bruitage

	  // --------------------------------------------------------------- Accessor
	  inline bool isSoundGame() { return bSoundGame; }
	  inline void setSoundGame(bool sound) { bSoundGame = sound; }
};






#endif	// SoundLibH
