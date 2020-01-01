////////////////////////////////////////////////////////////////////////////////
//  Unité  "ForceFeedbackEffect.cpp" - Implementation de ForceFeedbackEffect  //
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

// --------------------------------------------------------------------- include
#include "ForceFeedbackEffect_BumpIntoWall.h"	// Header de la classe


// ------------------------------------------------------------------- variables
// Parametres par defaut des 2 intensites de la RampForce
int ForceFeedbackEffect_BumpIntoWall::DEFAULT_START_FORCE = 10000;
int ForceFeedbackEffect_BumpIntoWall::DEFAULT_END_FORCE	  =  6000;
float ForceFeedbackEffect_BumpIntoWall::DEFAULT_PUISSANCE =   100;


////////////////////////////////////////////////////////////////////////////////
//	Classe FORCEFEEDBACKEFFECT_BUMPINTOWALL								      //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur ===========================================================//
ForceFeedbackEffect_BumpIntoWall::ForceFeedbackEffect_BumpIntoWall(int id, LPDIRECTINPUTDEVICE2 lpDevice):
	ForceFeedbackEffect(id, lpDevice)
{	
	lDirection[0] = 0;	// Direction set before playing the effect
	lDirection[1] = 0 * DI_DEGREES;
		
	DIRAMPFORCE	diRampForce;
	fPuissance = DEFAULT_PUISSANCE;
	diRampForce.lStart = DEFAULT_START_FORCE;	// x100%
	diRampForce.lEnd = DEFAULT_END_FORCE;
	iStartForce = DEFAULT_START_FORCE;
	iEndForce = DEFAULT_END_FORCE;

	diEnvelope.dwAttackLevel = 0;
	diEnvelope.dwAttackTime = (DWORD) (0.2 * DI_SECONDS);
	diEnvelope.dwFadeLevel = 0;
	diEnvelope.dwFadeTime = (DWORD) (0.3 * DI_SECONDS);

	
	diEffect.dwDuration = (DWORD) (0.8 * DI_SECONDS);
	diEffect.cAxes = 2;
	diEffect.rglDirection = &lDirection[0];
	diEffect.lpEnvelope = &diEnvelope;
	diEffect.cbTypeSpecificParams = sizeof(DIRAMPFORCE);
	diEffect.lpvTypeSpecificParams = &diRampForce;
	diEffect.dwTriggerButton = DIEB_NOTRIGGER;				// no trigger
	
	// Cree l'effet et le download ds le joystick
	HRESULT hr = lpDevice->CreateEffect(GUID_RampForce, &diEffect, &lpdiEffect, NULL);
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
ForceFeedbackEffect_BumpIntoWall::~ForceFeedbackEffect_BumpIntoWall()
{	
}
//----------------------------------------------------------------------------//

//=== Lance l'effet dans telle direction et avec telles intensites ===========//
void ForceFeedbackEffect_BumpIntoWall::Play(int rotation, float puissance)
{	
	// Ne le joue pas si il est deja entrain d'etre joue
	if (isPlaying()) return;
	
	HRESULT hr;
	// Change les parametres de l'effet suivant ces nouvelles valeurs
	// - Change la direction de l'effet
	if (rotation != lDirection[0])
	{	lDirection[0] = rotation * DI_DEGREES;
		
		DIEFFECT        diEffect;           // parameters for effect
		diEffect.dwSize = sizeof(DIEFFECT);
		diEffect.dwFlags = DIEFF_POLAR | DIEFF_OBJECTOFFSETS;	// Polar coordinates
		diEffect.cAxes = 2;
		diEffect.rglDirection = &lDirection[0];
		hr = lpdiEffect->SetParameters(&diEffect, DIEP_DIRECTION);
	}
	
	// - Change les donnees de la RampForce
	if (puissance!=fPuissance)
	{	fPuissance = puissance;
		iStartForce = (int) (fPuissance * DEFAULT_START_FORCE / 100);
		iEndForce = (int) (fPuissance * DEFAULT_END_FORCE / 100);
		
		DIEFFECT        diEffect;           // parameters for effect
		DIRAMPFORCE	diRampForce;
		diRampForce.lStart = iStartForce;
		diRampForce.lEnd = iEndForce;

		diEffect.dwSize = sizeof(DIEFFECT); 
		diEffect.cbTypeSpecificParams = sizeof(DIRAMPFORCE); 
		diEffect.lpvTypeSpecificParams = &diRampForce;
		hr = lpdiEffect->SetParameters(&diEffect, DIEP_TYPESPECIFICPARAMS);
	}

	Play();
}
//----------------------------------------------------------------------------//

//=== Start to play this effect ==============================================//
void ForceFeedbackEffect_BumpIntoWall::Play()
{	// Seul effet
	lpdiEffect->Start(1, DIES_SOLO);
}
//----------------------------------------------------------------------------//

/////////////// Fin de la classe FORCEFEEDBACKEFFECT_BUMPINTOWALL //////////////



///////////////////////////////////////////////////////////////////////////////
//							Fonctions EXTERNES								 //
///////////////////////////////////////////////////////////////////////////////

