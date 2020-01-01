////////////////////////////////////////////////////////////////////////////////
//  Unité "ScnCauses.h"  : implémentation de la classe TCause				  //
//                                                                            //
//	Description :                                                             //
//                                                                            //
//  Copyright (C) 1999 Antoine Rey    arey@insa-lyon.fr                       //
//                                    antrey@club-internet.fr                 //
//  Version      : beta 0.20                                                  //
//  Creation     : 17/07/1999 		                                          //
//  Last upgrade : <none>	                                                  //
////////////////////////////////////////////////////////////////////////////////

#ifdef MICROSOFT
#pragma warning(disable : 4786 4100)  
#endif

//---------------------------------------------------------------------- Include
#include "ZeNain.h"             //--- Special for Visual C++ 6 MFC ---
#include "TimeClock.h"			// Lib. de gestion temporelle du jeu
#include "GameObject.h"			// Gestion des objets du jeu
#include "ScnScript.h"			// Librairie des Scripts du jeu
#pragma hdrstop
#include "ScnCauses.h"          // son Header

////////////////////////////////////////////////////////////////////////////////
// Classe TCause                                                              //
////////////////////////////////////////////////////////////////////////////////

int TCause::iLastID = 0;		// N°ID à attribuer à la prochaine cause

//=== Constructeur par défaut ================================================//
TCause::TCause()
{	iCauseID = iLastID++;				// Attribution d'un n°ID libre
	iCauseType = CAUSE_TYPE_NONE;
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TCause::~TCause()
{
}
//----------------------------------------------------------------------------//
/////////////////// Fin de la classe TCause ///////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
// Classe TCAUSE_STRIKE_BY										              //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur  ==========================================================//
TCause_StrikeBy::TCause_StrikeBy(int thingID):
   	iThingID(thingID)
{ 	iCauseType = CAUSE_TYPE_THING_STRIKE_BY;
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TCause_StrikeBy::~TCause_StrikeBy()
{	}
//----------------------------------------------------------------------------//

//=== Joue une animation superposée "classique" au décors ====================//
bool TCause_StrikeBy::isCauseRealized(TScriptManager* SptMgr)
{	
	return false;
}
//----------------------------------------------------------------------------//
////////////////// Fin de la classe TCAUSE_STRIKE_BY //////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Classe TCAUSE_STRIKE_WITH									              //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur  ==========================================================//
TCause_StrikeWith::TCause_StrikeWith(int thingID):
   	iThingID(thingID)
{ 	iCauseType = CAUSE_TYPE_THING_STRIKE_WITH;
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TCause_StrikeWith::~TCause_StrikeWith()
{	}
//----------------------------------------------------------------------------//

//=== Joue une animation superposée "classique" au décors ====================//
bool TCause_StrikeWith::isCauseRealized(TScriptManager* SptMgr)
{	
	return false;
}
//----------------------------------------------------------------------------//
///////////////// Fin de la classe TCAUSE_STRIKE_WITH //////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Classe TCAUSE_RANDOM											              //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur  ==========================================================//
TCause_Random::TCause_Random(int numb):
   	iNumber(numb)
{ 	iCauseType = CAUSE_TYPE_RANDOM;
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TCause_Random::~TCause_Random()
{	}
//----------------------------------------------------------------------------//

//=== Joue une animation superposée "classique" au décors ====================//
bool TCause_Random::isCauseRealized(TScriptManager* SptMgr)
{	return ( (rand() % iNumber) == iNumber-1 );
}
//----------------------------------------------------------------------------//
////////////////////// Fin de la classe TCAUSE_RANDOM //////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Classe TCAUSE_MEMORY_IN										              //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur  ==========================================================//
TCause_MemoryIn::TCause_MemoryIn(int numb):
   	iMemoryID(numb)
{ 	iCauseType = CAUSE_TYPE_THING_MEMORY_IN;
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TCause_MemoryIn::~TCause_MemoryIn()
{	}
//----------------------------------------------------------------------------//

//=== Joue une animation superposée "classique" au décors ====================//
bool TCause_MemoryIn::isCauseRealized(TScriptManager* SptMgr)
{	return SptMgr->GetSrcGO()->isInMemory(iMemoryID);
}
//----------------------------------------------------------------------------//
///////////////////// Fin de la classe TCAUSE_MEMORY_IN ////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Classe TCAUSE_MEMORY_NO										              //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur  ==========================================================//
TCause_MemoryNo::TCause_MemoryNo(int numb):
   	iMemoryID(numb)
{ 	iCauseType = CAUSE_TYPE_THING_MEMORY_NO;
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TCause_MemoryNo::~TCause_MemoryNo()
{	}
//----------------------------------------------------------------------------//

//=== Joue une animation superposée "classique" au décors ====================//
bool TCause_MemoryNo::isCauseRealized(TScriptManager* SptMgr)
{	return !SptMgr->GetSrcGO()->isInMemory(iMemoryID);
}
//----------------------------------------------------------------------------//
///////////////////// Fin de la classe TCAUSE_MEMORY_NO ////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Classe TCAUSE_RANDOM											              //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur  ==========================================================//
TCause_Timer::TCause_Timer(int numb):
   	iNbMs(numb)
{ 	iCauseType = CAUSE_TYPE_TIMER;
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TCause_Timer::~TCause_Timer()
{	}
//----------------------------------------------------------------------------//

//=== Renvoie vrai lorsque l'on est arrivé au bout du tps ====================//
bool TCause_Timer::isCauseRealized(TScriptManager* SptMgr)
{	// Quitte si l'on a omis le gestionaire de scripts
	if (SptMgr==NULL) return false;
	
	DWORD dwTimer;
	DWORD dwCurrentTime = GameHistory->GetTime();

	// Regarde si le script est déjà en cours d'évaluation
	if (!SptMgr->IsTimer(iCauseID))
	{	// Si ce n'est pas le cas, on crée cette paire
		SptMgr->SetTimer(iCauseID, dwCurrentTime + iNbMs);
	} else
	{	// Sinon, on récupère la valeur et on regarde si l'heure actuelle lui est supérieure
		dwTimer = SptMgr->GetTimer(iCauseID);
		if (dwTimer <= dwCurrentTime)
		{	SptMgr->SetTimer(iCauseID, dwCurrentTime + iNbMs);
			return true;
		}
	}	
	return false;
}
//----------------------------------------------------------------------------//
////////////////////// Fin de la classe TCAUSE_RANDOM //////////////////////////


