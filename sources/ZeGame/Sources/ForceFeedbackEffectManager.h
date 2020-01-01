////////////////////////////////////////////////////////////////////////////////
//  Unité "ForceFeedbackEffectManager.h" : interface de la classe du meme nom //
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

#ifndef ForceFeedbackEffectManagerH
#define ForceFeedbackEffectManagerH

//---------------------------------------------------------------------- Include
#include "Singleton.h"	// Template de toutes les classes singletons
#include <map>			// STL Library to handle xtree
#include "DILib.h"		// Home-made DirectInput librairy


//----------------------------------------------------------------------- Define
#define g_ForceFeedbackEffectManager ForceFeedbackEffectManager::getSingleton()

//------------------------------------------------------------------------- Enum

//------------------------------------------------------------------------ Class
class ForceFeedbackEffect;	// Effet pour joystick a retour de force
class Personnage;			// Classe de base d'un perso (PNJ, Hero, Streums ...)
class DecorsObjet;			//   "		   "  d'un decors (mur, table, ...)


////////////////////////////////////////////////////////////////////////////////
// Classe FORCEFEEDBACKEFFECTMANAGER										  //
class ForceFeedbackEffectManager : public Singleton <ForceFeedbackEffectManager>
{ // ---------------------------------------------------------- Attributs privés
  protected:
	std::map<int, ForceFeedbackEffect*> mForceFeedbackEffects;	// Librairie des effets a retour de force instancie
	LPDIRECTINPUTDEVICE2 lpDevice;								// interface vers le joystick
	
  // ------------------------------------------------------ Méthodes publiques
  public:
	
	//--- Constructeurs & Destructeurs ---
	ForceFeedbackEffectManager(LPDIRECTINPUTDEVICE2 joy);		// Constructeur par defaut
	virtual ~ForceFeedbackEffectManager();						// Destructeur

	void Init();							// Initialise tous les effets a retour de force programmes
	void Clear();							// Desinstancie tous les ForceFeedbackEffect

	// Joue l'effet a retour de force approprie lorsque :
	// - le joueur fonce dans un mur (ou tout objet du decors)
	void playBumpIntoWall(Personnage* player, DecorsObjet* wall);
	
	//--- Fonctions inline ---
	inline int getNbEffects() { return mForceFeedbackEffects.size(); }		// Nombre d'effets pouvant etre utilises

	// -------------------------------------------------------- Méthodes privees
  protected:
	int getAngleFromDirection(int direction);	// Renvoi l'angle (en degre) de la direction correspondante
	

    
};
//============== Fin de la classe FORCEFEEDBACKEFFECTMANAGER ==================//

//------------------------------------------------------------ Fonctions annexes
void InitGame_ForceFeedbackEffectManager();		// Initialise le manager des effets du joystick a retour de force
void CloseGame_ForceFeedbackEffectManager();	// Referme proprement le manager

#endif	// ForceFeedbackEffectManagerH


