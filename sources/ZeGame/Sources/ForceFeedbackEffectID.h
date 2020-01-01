////////////////////////////////////////////////////////////////////////////////
//  Unité  "ForceFeedbackEffectID.h"										  //
//																			  //
//	Definie une liste d'ID des effets disponibles pour un Joystick a retour   //
//	de force.																  //
//																			  //
//  Copyright (C) 2000 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@ifrance.com			              //
//					                                                          //
//  Version      : 0.01		                                                  //
//  Creation     : 15/09/2000                                                 //
//  Last upgrade :															  //
////////////////////////////////////////////////////////////////////////////////

#ifndef ForceFeedbackEffectIDH
#define ForceFeedbackEffectIDH

//------------------------------------------------------------------------- enum
enum
{	FIRST_FORCEFEEDBACKEFFECT_ID	=   0,
		
		ID_FORCEFEEDBACKEFFECT_BUMP_INTO_WALL_1 = 0,
		
	LAST_FORCEFEEDBACKEFFECT_ID = ID_FORCEFEEDBACKEFFECT_BUMP_INTO_WALL_1,
	NB_FORCEFEEDBACKEFFECT_ID = LAST_FORCEFEEDBACKEFFECT_ID - FIRST_FORCEFEEDBACKEFFECT_ID + 1

};
#endif // ForceFeedbackEffectIDH



