////////////////////////////////////////////////////////////////////////////////
//  Unité  "ForceFeedbackEffect.h" - Header de la classe ForceFeedbackEffect  //
//																			  //
//	Description : Classe permettant de creer un effet a retour de force, a le //
//				  downloader dans le joystick, a le modifier et a tester si   //
//				  le joy peut effectivement bien le jouer					  //
//                                                                            //
//  Copyright (C) 2000 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com                      //
//					                                                          //
//  Version      : beta 0.01                                                  //
//  Creation     : 16/09/2000                                                 //
//  Last upgrade :															  //
////////////////////////////////////////////////////////////////////////////////

#ifndef ForceFeedbackEffectH
#define ForceFeedbackEffectH

// --------------------------------------------------------------------- Include
#include "DInput.h"			// Bibliothèque Microft DirectInput

// ---------------------------------------------------------------------- Define

// ----------------------------------------------------------------------- Class


////////////////////////////////////////////////////////////////////////////////
// Class FORCEFEEDBACKEFFECT												  //
class ForceFeedbackEffect
{ // ---------------------------------------------------------- Attributs prives
  protected:
	int iEffectID;				// Identifiant unique

	DIPERIODIC	diPeriodic;		// type-specific information for periodic effect
	DIENVELOPE	diEnvelope;		// optionnal envelope parameter for an effect
	DIEFFECT	diEffect;		// general parameters of the ForceFeedbackEffect
	
	LPDIRECTINPUTEFFECT	lpdiEffect;	// interface vers l'objet effect de DirectInput
	LPDIRECTINPUTDEVICE2 lpDevice;	// interface vers le joystick
	
  protected:
	bool isEffectSupported(REFGUID rguid);	// Return true if the @rguid effect is supported by this device
	
   public:
	ForceFeedbackEffect(int id, LPDIRECTINPUTDEVICE2 lpDevice);	// Constructeur
	virtual ~ForceFeedbackEffect();								// Destructeur

	virtual void Play();					// Start to play this effect
	virtual void Stop();					// Stop playing this effect

	bool isPlaying();						// Return true if this effect is now playing
  
  // ------------------------------------------------------------------ Accessor
	inline int getEffectID() { return iEffectID; }

};
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------ Fonctions annexes

#endif	// ForceFeedbackEffectH
