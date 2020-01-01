////////////////////////////////////////////////////////////////////////////////
//  Unité  "SoundEffectID.h"												  //
//																			  //
//	Definie une liste d'ID des effets sonores. Ce sont les memes numeros que  //
//	ceux definis dans Sound\SoundEffect.id									  //
//																			  //
//  Copyright (C) 2000 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com                      //
//					                                                          //
//  Version      : 0.01		                                                  //
//  Creation     : 06/08/2000                                                 //
//  Last upgrade :															  //
////////////////////////////////////////////////////////////////////////////////

#ifndef SoundEffectIDH
#define SoundEffectIDH

//------------------------------------------------------------------------- enum
enum
{	FIRST_SOUNDEFFECT_ID	=   0,
		
		FIRST_SOUND_EFFECT_SWORD_CLASHES_ID	= 0,
			ID_SOUNDEFFECT_SWORD_CLASHES_1		= 0, 
			ID_SOUNDEFFECT_SWORD_CLASHES_2		= 1,
			ID_SOUNDEFFECT_SWORD_CLASHES_3		= 2,
			ID_SOUNDEFFECT_SWORD_CLASHES_4		= 3,
			ID_SOUNDEFFECT_SWORD_CLASHES_5		= 4,
			ID_SOUNDEFFECT_SWORD_CLASHES_6		= 5,
		LAST_SOUND_EFFECT_SWORD_CLASHES_ID	= 5,
		NB_SOUND_EFFECT_SWORD_CLASHES_ID	= LAST_SOUND_EFFECT_SWORD_CLASHES_ID - FIRST_SOUND_EFFECT_SWORD_CLASHES_ID + 1,

		FIRST_SOUND_EFFECT_CHIN_PUNCH_ID	= 10,
			ID_SOUNDEFFECT_CHIN_PUNCH_1			= 10,
			ID_SOUNDEFFECT_CHIN_PUNCH_2			= 11,
			ID_SOUNDEFFECT_CHIN_PUNCH_3			= 12,
			ID_SOUNDEFFECT_CHIN_PUNCH_4			= 13,
			ID_SOUNDEFFECT_CHIN_PUNCH_5			= 14,
			ID_SOUNDEFFECT_CHIN_PUNCH_6			= 15,
			ID_SOUNDEFFECT_CHIN_PUNCH_7			= 16,
			ID_SOUNDEFFECT_CHIN_PUNCH_8			= 17,
		LAST_SOUND_EFFECT_CHIN_PUNCH_ID		= 17,
		NB_SOUND_EFFECT_CHIN_PUNCH_ID		= LAST_SOUND_EFFECT_CHIN_PUNCH_ID - FIRST_SOUND_EFFECT_CHIN_PUNCH_ID + 1,

		FIRST_SOUND_EFFECT_SWORD_RUN_THROUGH_ID	= 20,
			ID_SOUNDEFFECT_SWORD_RUN_THROUGH_1	= 20,
			ID_SOUNDEFFECT_SWORD_RUN_THROUGH_2	= 21,
			ID_SOUNDEFFECT_SWORD_RUN_THROUGH_3	= 22,
			ID_SOUNDEFFECT_SWORD_RUN_THROUGH_4	= 23,
			ID_SOUNDEFFECT_SWORD_RUN_THROUGH_5	= 24,
		LAST_SOUND_EFFECT_SWORD_RUN_THROUGH_ID	= 24,
		NB_SOUND_EFFECT_SWORD_RUN_THROUGH_ID	= LAST_SOUND_EFFECT_SWORD_RUN_THROUGH_ID - FIRST_SOUND_EFFECT_SWORD_RUN_THROUGH_ID + 1,

		FIRST_SOUND_EFFECT_DEATH_SCREAMS_ID	= 30,
			ID_SOUNDEFFECT_DEATH_SCREAMS_1		= 30,
			ID_SOUNDEFFECT_DEATH_SCREAMS_2		= 31,
		LAST_SOUND_EFFECT_DEATH_SCREAMS_ID	= 31,
		NB_SOUND_EFFECT_DEATH_SCREAMS_ID	= LAST_SOUND_EFFECT_DEATH_SCREAMS_ID - FIRST_SOUND_EFFECT_DEATH_SCREAMS_ID + 1,

		FIRST_SOUND_EFFECT_BLACK_POWDER_EXPLOSION_ID	= 40,
			ID_SOUNDEFFECT_BLACK_POWDER_EXPLOSION_1		= 40,
			ID_SOUNDEFFECT_BLACK_POWDER_EXPLOSION_2		= 41,
		LAST_SOUND_EFFECT_BLACK_POWDER_EXPLOSION_ID	= 41,
		NB_SOUND_EFFECT_BLACK_POWDER_EXPLOSION_ID	= LAST_SOUND_EFFECT_BLACK_POWDER_EXPLOSION_ID - FIRST_SOUND_EFFECT_BLACK_POWDER_EXPLOSION_ID + 1

};
#endif // SoundEffectIDH



