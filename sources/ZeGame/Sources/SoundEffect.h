////////////////////////////////////////////////////////////////////////////////
//  Unité  "SoundEffect.h" : Manage all the sound effects during the game	  //
//							 according to the actions done by the characters, //
//							 the objects, the weather, ...					  //
//                                                                            //
//  Copyright (C) 2000 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com                      //
//					                                                          //
//  Version      : beta 0.01                                                  //
//  Creation     : 06/08/2000                                                 //
//  Last upgrade :															  //
////////////////////////////////////////////////////////////////////////////////

#ifndef SoundEffectH
#define SoundEffectH

// --------------------------------------------------------------------- Include

// ---------------------------------------------------------------------- Define

// ----------------------------------------------------------------------- Class
class Personnage;			// Classe de base d'un perso (PNJ, Hero, Streums ...)
class SoundManager;			// Gere la collection d'effets sonore

////////////////////////////////////////////////////////////////////////////////
// Class SoundEffectManager													  //
// S'occupe de jouer le bon bruitage suivant les circonstances du jeu		  //
class SoundEffectManager
{  private:
	SoundManager*	lpSoundManager;				// Reference sur la classe gerant la collection de bruitages
	
   public:
	SoundEffectManager(SoundManager* sm);		// Constructeur
	virtual ~SoundEffectManager();				// Destructeur

	// Joue le bruitage approprie lorsque :
	// - un personnage attaque un autre avec une arme blanche
	void playFightSoundAttack(Personnage* attacker, Personnage* defender);
	// - tire avec une arme a feu
	void playShootSound(Personnage* attacker);
	// Joue tel son a partir de tel endroit
	void playSoundFrom(int soundID, double posX, double posY);
  
  private:
	double	getSqrDistanceFromHero(double X, double Y);					// Renvoie le carre de la distance avec le Hero
	double	getSqrHorizontalDistanceFromHero(double X);					//	  "			"			"	   horizontale avec le Hero		
	int		getVolumeFromDistance(double sqrDst, double minDistance);	// Renvoie le volume sonore selon la distance passee en parametre	  
	int		getPanFromDistance(double horDst);							// Renvoie la diffence de volume entre les 2 enceintes
  // ------------------------------------------------------------------ Accessor

};
////////////////////////////////////////////////////////////////////////////////

//--- Instance de la classe utilisée par le jeu ---
extern SoundEffectManager *GameSoundEffectManager;

//------------------------------------------------------------ Fonctions annexes
void InitPack_SoundEffectManager();			// Initialise l'instance du jeu
void DisablePack_SoundEffectManager();		// Désalloue tout

#endif	// SoundEffectH
