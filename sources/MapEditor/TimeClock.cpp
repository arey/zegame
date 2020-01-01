////////////////////////////////////////////////////////////////////////////////
// TimeClock.cpp est une librairie regroupant plusieurs classes se chargeant  //
//				 de gérer le temps, le climat, les conditions météo, les      //
//				 saisons, le passage jour/nuit ...							  //
//                                                                            //
//	- TClock   : permet d'évaluer le temps écoulé ds le jeu et de synchronier //
//			     l'horloge windows avec celle du jeu.						  //
////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------- include
#ifdef CBUILDER
#include <vcl.h>
#elif VISUALC
#include "StdAfx.h"
#endif
#pragma hdrstop
#include "TimeClock.h"			// Son Header


////////////////////////////////////////////////////////////////////////////////
// Classe TCLOCK	                                                          //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur par défaut ================================================//
TClock::TClock()
{	dwCurrentGameTime = 0;		// Temps écoulé depuis le début du jeu à 0
	dwCurrentWinTime = GetTickCount();

    // Synchronisation des horloges : heure du jeu à zéro <=> heure système
	dwStartClockWin32 = GetTickCount();
    dwStartClockGame  = 0;
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TClock::~TClock()
{
}
//----------------------------------------------------------------------------//

//=== Synchronise l'horloge du jeu avec l'horloge système ====================//
void TClock::SynchronizeClock(DWORD currentClockGame)
{	dwCurrentGameTime = currentClockGame;
	dwStartClockWin32 = GetTickCount();
	dwCurrentWinTime  = dwStartClockWin32;
    dwStartClockGame  = currentClockGame;
}
//----------------------------------------------------------------------------//

//=== MaJ de l'horloge du jeu depuis le temps écoulé =========================//
void TClock::UpdateClock()
{   // Récupère l'intervalle de temps depuis le dernier update
	DWORD time = GetTickCount();
	dwDT = time - dwCurrentWinTime;

    // MaJ de l'horloge du jeu
    dwCurrentGameTime += dwDT;
    dwCurrentWinTime = time;
}
//----------------------------------------------------------------------------//

//=== Renvoie l'heure du jeu =================================================//
DWORD TClock::GetTime()
{	return dwCurrentGameTime;
}
//----------------------------------------------------------------------------//

//=== Renvoie l'intervalle de temps écoulé entre 2 updates ===================//
DWORD TClock::GetDT()
{	return dwDT;
}
//----------------------------------------------------------------------------//


////////////////////////////////////////////////////////////////////////////////
// Classe TGAMEHISTORY                                                        //
////////////////////////////////////////////////////////////////////////////////

//--- Instance de la classe utilisée par le jeu ---
TGameHistory *GameHistory = NULL;

//=== Constructeur par défaut ================================================//
TGameHistory::TGameHistory()
{
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TGameHistory::~TGameHistory()
{
}
//----------------------------------------------------------------------------//

//=== MaJ de tout ============================================================//
void TGameHistory::Update()
{   // maj de l'horloge du jeu
	Clock.UpdateClock();      
}
//----------------------------------------------------------------------------//

//=== Renvoie l'heure du jeu =================================================//
DWORD TGameHistory::GetTime()
{	return Clock.GetTime();
}
//----------------------------------------------------------------------------//

//=== Renvoie l'intervalle de temps écoulé entre 2 updates ===================//
DWORD TGameHistory::GetDT()
{	return Clock.GetDT();
}
//----------------------------------------------------------------------------//

//=== Synchronise l'horloge du jeu avec l'horloge système ====================//
void TGameHistory::SynchronizeClockAfterPause()
{	Clock.SynchronizeClock(Clock.GetTime());
}
//----------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////////////////
//	Fonctions EXTERNES														  //
////////////////////////////////////////////////////////////////////////////////

//=== Initialisation de GameHistory ==========================================//
void InitPack_GameHistory(bool load)
{   // Allocation de GameHistory
	if (GameHistory != NULL) delete GameHistory;
	GameHistory = new TGameHistory;

    if (!load) return;
    // On charge les paramètres climatiques, saisonniers, ... à partir de la
    // sauvegarde du joueur
    // ...
    // ...
}
//----------------------------------------------------------------------------//

//=== Libération de GameHistory ==============================================//
void DisablePack_GameHistory()
{	if (GameHistory != NULL) delete GameHistory;
	GameHistory = NULL;
}
//----------------------------------------------------------------------------//

//=== MaJ de GameHistory à chaque frame ======================================//
void UpdatePack_GameHistory()
{	if (GameHistory == NULL) return;
	GameHistory->Update();
}
//----------------------------------------------------------------------------//
