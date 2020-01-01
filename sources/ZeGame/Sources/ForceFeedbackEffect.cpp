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
#include "ForceFeedbackEffect.h"	// Header de la classe


// ------------------------------------------------------------------- variables


////////////////////////////////////////////////////////////////////////////////
//	Classe FORCEFEEDBACKEFFECT											      //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur ===========================================================//
ForceFeedbackEffect::ForceFeedbackEffect(int id, LPDIRECTINPUTDEVICE2 lpDevice):
	lpDevice(lpDevice), iEffectID(id)
{	// Common or usual parameters
	static DWORD dwAxes[2] = { DIJOFS_X, DIJOFS_Y };		// 2 Axes X & Y
	
	diEnvelope.dwSize = sizeof(DIENVELOPE);
	
	diEffect.dwSize = sizeof(DIEFFECT); 
	diEffect.dwFlags = DIEFF_POLAR | DIEFF_OBJECTOFFSETS;	// Polar coordinates
	diEffect.dwTriggerButton = DIEB_NOTRIGGER;				// no trigger
	diEffect.dwTriggerRepeatInterval = 0;					// not applicable
	diEffect.cAxes = 2;
	diEffect.rgdwAxes = dwAxes;	
	diEffect.dwSamplePeriod = 0;							// = default value
	diEffect.lpEnvelope = NULL;
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
ForceFeedbackEffect::~ForceFeedbackEffect()
{	if (lpdiEffect!=NULL)
	{	//lpdiEffect->Release();
		lpdiEffect = NULL;
	}
}
//----------------------------------------------------------------------------//

//=== Return true if the @rguid effect is supported by this device ===========//
bool ForceFeedbackEffect::isEffectSupported(REFGUID rguid)
{	DIEFFECTINFO diEffectInfo;
	diEffectInfo.dwSize = sizeof(DIEFFECTINFO);
	lpDevice->GetEffectInfo(&diEffectInfo, rguid);

	// ... tests ...

	return true;
}
//----------------------------------------------------------------------------//

//=== Start to play this effect ==============================================//
void ForceFeedbackEffect::Play()
{	lpdiEffect->Start(1, 0);
}
//----------------------------------------------------------------------------//

//=== Stop playing this effect ===============================================//
void ForceFeedbackEffect::Stop()
{	lpdiEffect->Stop();
}
//----------------------------------------------------------------------------//

//=== Return true if this effect is now playing ==============================//
bool ForceFeedbackEffect::isPlaying()
{	DWORD dwFlags;
	HRESULT hr = lpdiEffect->GetEffectStatus(&dwFlags);
	if FAILED(hr) return false;
	return (dwFlags & DIEGES_PLAYING == DIEGES_PLAYING);
}
//----------------------------------------------------------------------------//


////////////////// Fin de la classe FORCEFEEDBACKEFFECT ////////////////////////



///////////////////////////////////////////////////////////////////////////////
//							Fonctions EXTERNES								 //
///////////////////////////////////////////////////////////////////////////////

