////////////////////////////////////////////////////////////////////////////////
//  Unité  "ForceFeedbackEffect_BumpIntoWall.h"								  //
//																			  //
//	Description : Effet a retour de force utilise lorsque le perso que le 	  //
//				  joueur dirige se heurte a un mur.							  //
//                                                                            //
//  Copyright (C) 2000 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com                      //
//					                                                          //
//  Version      : beta 0.01                                                  //
//  Creation     : 16/09/2000                                                 //
//  Last upgrade :															  //
////////////////////////////////////////////////////////////////////////////////

#ifndef ForceFeedbackEffect_BumpIntoWallH
#define ForceFeedbackEffect_BumpIntoWallH

// --------------------------------------------------------------------- Include
#include "ForceFeedbackEffect.h"	// Classe parente a tout force feedback effect

// ---------------------------------------------------------------------- Define

// ----------------------------------------------------------------------- Class


////////////////////////////////////////////////////////////////////////////////
// Class FORCEFEEDBACKEFFECT_BUMPINTOWALL									  //
class ForceFeedbackEffect_BumpIntoWall : public ForceFeedbackEffect
{ // ---------------------------------------------------------- Attributs prives
  private:
	// Parametres par defaut des 2 intensites de la RampForce
	static int DEFAULT_START_FORCE;		// 10000
	static int DEFAULT_END_FORCE;		//  6000
	static float DEFAULT_PUISSANCE;		//   100%

	int	iStartForce;		// beginning intensity of ramp force
	int iEndForce;			// end intensity of ramp force
	float fPuissance;		// puissance (in %) to give to this effect
	LONG lDirection[2];		// direction of the effect on X & Y axis
  
  protected:
	void Play();			// Start to play this effect
	
  public:
	ForceFeedbackEffect_BumpIntoWall(int id, LPDIRECTINPUTDEVICE2 lpDevice);	// Constructeur
	virtual ~ForceFeedbackEffect_BumpIntoWall();								// Destructeur

	// Lance l'effet dans telle direction et avec telles intensites
	void Play(int rotation, float puissance = DEFAULT_PUISSANCE);
  
// ------------------------------------------------------------------ Accessor

};
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------ Fonctions annexes

#endif	// ForceFeedbackEffect_BumpIntoWallH
