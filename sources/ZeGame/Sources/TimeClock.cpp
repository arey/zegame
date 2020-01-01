////////////////////////////////////////////////////////////////////////////////
// TimeClock.cpp est une librairie regroupant plusieurs classes se chargeant  //
//				 de g�rer le temps, le climat, les conditions m�t�o, les      //
//				 saisons, le passage jour/nuit ...							  //
//                                                                            //
//	- TClock   : permet d'�valuer le temps �coul� ds le jeu et de synchronier //
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

//=== Constructeur par d�faut ================================================//
TClock::TClock()
{	dwCurrentGameTime = 0;		// Temps �coul� depuis le d�but du jeu � 0
	dwCurrentWinTime = GetTickCount();

    // Synchronisation des horloges : heure du jeu � z�ro <=> heure syst�me
	dwStartClockWin32 = GetTickCount();
    dwStartClockGame  = 0;
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TClock::~TClock()
{
}
//----------------------------------------------------------------------------//

//=== Synchronise l'horloge du jeu avec l'horloge syst�me ====================//
void TClock::SynchronizeClock(DWORD currentClockGame)
{	dwCurrentGameTime = currentClockGame;
	dwStartClockWin32 = GetTickCount();
	dwCurrentWinTime  = dwStartClockWin32;
    dwStartClockGame  = currentClockGame;
}
//----------------------------------------------------------------------------//

//=== MaJ de l'horloge du jeu depuis le temps �coul� =========================//
void TClock::UpdateClock()
{   // R�cup�re l'intervalle de temps depuis le dernier update
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

//=== Renvoie l'intervalle de temps �coul� entre 2 updates ===================//
DWORD TClock::GetDT()
{	return dwDT;
}
//----------------------------------------------------------------------------//


////////////////////////////////////////////////////////////////////////////////
// Classe TGAMEHISTORY                                                        //
////////////////////////////////////////////////////////////////////////////////

//--- Instance de la classe utilis�e par le jeu ---
TGameHistory *GameHistory = NULL;

//=== Constructeur par d�faut ================================================//
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

//=== Renvoie l'intervalle de temps �coul� entre 2 updates ===================//
DWORD TGameHistory::GetDT()
{	return Clock.GetDT();
}
//----------------------------------------------------------------------------//

//=== Synchronise l'horloge du jeu avec l'horloge syst�me ====================//
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
    // On charge les param�tres climatiques, saisonniers, ... � partir de la
    // sauvegarde du joueur
    // ...
    // ...
}
//----------------------------------------------------------------------------//

//=== Lib�ration de GameHistory ==============================================//
void DisablePack_GameHistory()
{	if (GameHistory != NULL) delete GameHistory;
	GameHistory = NULL;
}
//----------------------------------------------------------------------------//

//=== MaJ de GameHistory � chaque frame ======================================//
void UpdatePack_GameHistory()
{	if (GameHistory == NULL) return;
	GameHistory->Update();
}
//----------------------------------------------------------------------------//
