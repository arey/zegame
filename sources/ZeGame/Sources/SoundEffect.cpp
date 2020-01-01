////////////////////////////////////////////////////////////////////////////////
//  Unité  "SoundEffect.cpp" (implemente le header du meme nom)				  //
//                                                                            //
//	Manage all the sound effects during the game according to the actions	  //	
//	done by the characters, the objects, the weather, ...					  //
//                                                                            //
//  Copyright (C) 2000 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com                      //
//					                                                          //
//  Version      : beta 0.01                                                  //
//  Creation     : 06/08/2000                                                 //
//  Last upgrade :															  //
////////////////////////////////////////////////////////////////////////////////

// --------------------------------------------------------------------- include
#include <math.h>				// Librairie de fonctions mathematiques
#include "ZeNain.h"				// Parametrages du projet
#include "Consts.h"				// Constantes globales du jeu
#include "SoundLib.h"			// Librairie interfacant DirectSound et la gestion des bruitages
#include "PersoLib.h"			// Personnages et modèles de persos du jeu
#include "WorldSimulation.h"	// Paramètrages des lois physiques du monde
#include "SoundEffectID.h"		// ID de tous les effets sonores disponibles
#include "SoundEffect.h"		// Header


// ------------------------------------------------------------------- variables

SoundEffectManager *GameSoundEffectManager = NULL;	// Unique instance de la classe SoundManager dans le jeu


////////////////////////////////////////////////////////////////////////////////
//	Classe SOUNDEFFECTMANAGER											      //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur ===========================================================//
SoundEffectManager::SoundEffectManager(SoundManager* sm):
	lpSoundManager(sm)
{
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
SoundEffectManager::~SoundEffectManager()
{	lpSoundManager = NULL;
}
//----------------------------------------------------------------------------//

// === Joue le bruitage approprie lorsqu'un personnage attaque un autre ======//
void SoundEffectManager::playFightSoundAttack(Personnage* attacker, Personnage* defender)
{	// Regarde avec quelle arme l'attaque a ete faite
	int weaponAttack = attacker->ArmeEnMain;

	// Recupere l'armure du defenseur
	int armorDefender = -1;
	armorDefender = armorDefender;	// eneleve le warning de visual

	// Suivant les facteurs precedents, on determine le bruitage que l'on peut jouer
	int soundEffectID = -1;
	switch (weaponAttack)
	{	// Avec le poing 
		// Rmq : a modifier, puisque cela pourrait etre avec les pieds, la queue ou les griffes 
		case NOWEAPON:
			// On prends l'un des 8 uppercuts ds la machoire
			//if (defender->isAlive())
			{	soundEffectID = FIRST_SOUND_EFFECT_CHIN_PUNCH_ID + (rand() % NB_SOUND_EFFECT_CHIN_PUNCH_ID); 
			} //else
			{
			}
			break;
		// Attaque avec une epee
		case EPEE1:
			// On prend l'un des 6 bruitages entre le fer de 2 epees
			//soundEffectID = FIRST_SOUND_EFFECT_SWORD_CLASHES_ID + (rand() % NB_SOUND_EFFECT_SWORD_CLASHES_ID);
			if (defender->isAlive())
			{	int pif = rand() % 2;
				switch (pif)
				{	case 0 : soundEffectID = ID_SOUNDEFFECT_SWORD_RUN_THROUGH_5; break;
					case 1 : soundEffectID = ID_SOUNDEFFECT_SWORD_RUN_THROUGH_3; break;
				}
			} else
			{	soundEffectID =ID_SOUNDEFFECT_SWORD_RUN_THROUGH_4;
			}
		break;
	}
 
	// Si on est arrive la sans trouve quoi que se soit, c'est que l'on n'a pas
	// de bruitage fait pour cette situation -> on peut sortir
	if (soundEffectID == -1) return;

	playSoundFrom(soundEffectID, attacker->PersoNivX, attacker->PersoNivY);
}
//----------------------------------------------------------------------------//


// === Joue le bruitage approprie lorsqu'un personnage tire sur un autre =====//
// avec une arme a feu
void SoundEffectManager::playShootSound(Personnage* attacker)
{	// Regarde avec quelle arme l'attaque a ete faite
	int weaponAttack = attacker->ArmeEnMain;

	// Suivant le flingue, on determine le bruitage que l'on peut jouer
	int soundEffectID = -1;
	switch (weaponAttack)
	{	case PETOIRE1:
			soundEffectID = ID_SOUNDEFFECT_BLACK_POWDER_EXPLOSION_1; 
			break;
	}
 
	// Si on est arrive la sans trouve quoi que se soit, c'est que l'on n'a pas de bruitage fait pour cette situation -> on peut sortir
	if (soundEffectID == -1) return;

	playSoundFrom(soundEffectID, attacker->PersoNivX, attacker->PersoNivY);
}
//----------------------------------------------------------------------------//

//=== Joue tel son a partir de tel endroit ===================================//
void SoundEffectManager::playSoundFrom(int soundID, double posX, double posY)
{	if (!lpSoundManager->isSoundGame()) return;

	SoundEffect* soundEffect = lpSoundManager->getSoundEffect(soundID);

	// Calcul du volume sonore d'apres la distance entre la source sonore et le Hero
	double sqrDst = getSqrDistanceFromHero(posX, posY);
	// Pas la peine de jouer ce son si l'on est trop loin
	if (sqrDst > soundEffect->dMaxDistance*soundEffect->dMaxDistance) return;
	int volume = getVolumeFromDistance(sqrDst, soundEffect->dMinDistance);

	// Regle la stereo selon que le Hero se trouve a gauche ou a droite de
	// la source sonore
	double sqrHorDst = getSqrHorizontalDistanceFromHero(posX);
	int pan = getPanFromDistance(sqrHorDst);

	if (volume > -15000)
	{	lpSoundManager->playSound(soundID, volume, pan);
	}

	
}
//----------------------------------------------------------------------------//

//=== Renvoie le carre de sa distance avec le Hero  ==========================//
double	SoundEffectManager::getSqrDistanceFromHero(double X, double Y)
{	return   getSqrHorizontalDistanceFromHero(X)
		   + (Hero->PersoNivY - Y)*(Hero->PersoNivY - Y);
}
//----------------------------------------------------------------------------//

//=== Renvoie le carre de sa distance horizontale avec le Hero  ==============//
double	SoundEffectManager::getSqrHorizontalDistanceFromHero(double X)
{	return   (Hero->PersoNivX - X);
}
//----------------------------------------------------------------------------//

//=== Renvoie le volume sonore selon la distance passee en parametre =========//
int	SoundEffectManager::getVolumeFromDistance(double sqrDst, double minDistance)
{	static const double Log2 = 0.693147180559945309417232121458177;
	static const double fadingSqrDist = 10*10;
	static double fadingVolume = ((log(fadingSqrDist) / (2*Log2)) + 1) * WorldModel.SOUND_DEFAULTDBDECREASE * minDistance;
	double volume;
	if (sqrDst < fadingSqrDist)
	{	volume = ((log(sqrDst) / (2*Log2)) + 1) * WorldModel.SOUND_DEFAULTDBDECREASE * minDistance;
	} else
	{	volume = fadingVolume + -5000*(sqrDst-fadingSqrDist)/(20*20);
	}
	return (int) volume;

	/*// A 30m, on considere qu'on entends plus rien (-5000dB)
	return (int) (-5000*sqrDst/(30*30));*/
}
//----------------------------------------------------------------------------//

//=== Renvoie la diffence de volume entre les 2 enceintes ====================//
int	SoundEffectManager::getPanFromDistance(double horDst)
{	static const double Log2 = 0.693147180559945309417232121458177;
	double volume = ((log(horDst) / (Log2)) + 1) * WorldModel.SOUND_DEFAULTDBDECREASE;
	return (int) volume;
//	return (int) (-10000*horDst/30);
}
//----------------------------------------------------------------------------//




//////////////////////// Fin de la classe SoundLib /////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//							Fonctions EXTERNES								 //
///////////////////////////////////////////////////////////////////////////////

//=== Initialise la liste des bruitages du jeu ==============================//
void InitPack_SoundEffectManager()
{	// Instancie le manager des effets sonores
	GameSoundEffectManager = new SoundEffectManager(GameSoundManager);
}
//----------------------------------------------------------------------------//

//=== Referme proprement le sound effect manager =============================//
void DisablePack_SoundEffectManager()
{	// Desinstalle le manager des effets sonores
	delete GameSoundEffectManager;	GameSoundEffectManager = NULL;
}
//----------------------------------------------------------------------------//
