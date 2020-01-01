////////////////////////////////////////////////////////////////////////////////
//  Unité "MPMonitor.cpp" : S'occupe de l'affichage des informations liees	  //
//							a la partie multi-joueur de ZeGame				  //
//                                                                            //
//	Description :															  //
//                                                                            //
//  Copyright (C) 2001 Antoine Rey    antoine.rey@free.fr					  //
//                                    elric@ifrance.com		                  //
//  Version      : 0.1			                                              //
//  Creation     : 04/04/2001 		                                          //
//  Last upgrade : <none>                                                     //
////////////////////////////////////////////////////////////////////////////////

#include "ZeNain.h"					//--- Special for Visual C++ 6 MFC ---
#include "consts.h"					// Constantes globales à tout le projet
#include "DDLib.h"					// Librairie maison de routines DirectDraw
#include "TimeClock.h"				// Lib. de gestion temporelle du jeu
#pragma hdrstop
#include "MPSession.h"				// Module gerant les sessions multi-joueurs
#include "MPClient.h"				// Client multi-joueur utilisant DirectPlay
#include "MPMonitor.h"				// Header de la classe

extern unsigned long RGB24(unsigned long Red,unsigned long Green,unsigned long Blue);

////////////////////////////////////////////////////////////////////////////////
// Classe MPMONITOR					                                          //
////////////////////////////////////////////////////////////////////////////////

MPMonitor *g_pMPMonitor = NULL;		// Instance du jeu de la classe MPMonitor

const DWORD	MPMonitor::s_dwTextColor = RGB24(200,200,200);
const DWORD MPMonitor::s_dwClientConnectionDisplayDelay = 5000;	// Affichage 5 sec
const char*	MPMonitor::s_szClientConnectionDisplayText = "Multiplayer game started";

const DWORD MPMonitor::s_dwNbPlayersDisplayDelay = 4000;
const char*	MPMonitor::s_szNbPlayersDisplayText = "Number of players online : ";
const char*	MPMonitor::s_szZeroPlayerDisplayText = "Welcome, you're currently the only one online";

const DWORD	MPMonitor::s_dwClientAlreadyConnectedDisplayDelay = 3000;
const DWORD MPMonitor::s_dwPlayerExitDisplayDelay = 3000;
const DWORD MPMonitor::s_dwPlayerEnterDisplayDelay = 3000;


//=== Constructeur par defaut ================================================//
MPMonitor::MPMonitor()
{	m_pSession = NULL;
	m_pScreen = NULL;

	m_iClientConnection = MONITOR_NONE;
	m_dwClientConnectionDisplayTime = 0;
	m_iNbPlayers = MONITOR_NONE;
	m_dwNbPlayersDisplayTime = 0;
	m_iClientAlreadyConnected = MONITOR_NONE;
	m_dwClientAlreadyConnectedDisplayTime = 0;
	m_iPlayerExit = MONITOR_NONE;
	m_dwPlayerExitDisplayTime = 0;	
	m_iPlayerEnter = MONITOR_NONE;
	m_dwPlayerEnterDisplayTime = 0;	
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
MPMonitor::~MPMonitor()
{
}	
//----------------------------------------------------------------------------//

//=== MaJ a l'ecran des infos multi-joueurs ==================================//
void MPMonitor::Update()
{	
	static char cNumString[16];
	static char cText[256];
    
	if ( (m_pSession == NULL) || (m_pScreen == NULL) ) return;

	int lineY = m_pScreen->Screen_Height-24;
	int lineX = 15;

	// Connexion au serveur multi-player
	if (m_iClientConnection == MONITOR_NONE)
	{	if (   (m_pSession->getMPClient()!=NULL) 
			&& (m_pSession->getMPClient()->isClientConnected()) )
		{	m_dwClientConnectionDisplayTime = GameHistory->GetTime();
			m_iClientConnection = MONITOR_DISPLAYING;
		}	

	} else if (m_iClientConnection == MONITOR_DISPLAYING)
	{	
		m_pScreen->pDDSBack->TextSurf(lineX, lineY, s_dwTextColor, s_szClientConnectionDisplayText);

		// Verifie si la duree d'affichage ne s'est pas ecoulee
		if ((GameHistory->GetTime() - m_dwClientConnectionDisplayTime) > s_dwClientConnectionDisplayDelay)
		{	m_iClientConnection = MONITOR_DONE;
		}
	}
	
	// Nombre de joueurs connectes
	if ((m_iNbPlayers == MONITOR_NONE) && (m_iClientConnection == MONITOR_DONE))
	{	if (   (m_pSession->getMPClient()!=NULL) 
			&& (m_pSession->getMPClient()->getNbPlayersOnline()!=-1 ) )
		{	m_dwNbPlayersDisplayTime = GameHistory->GetTime();
			m_iNbPlayers = MONITOR_DISPLAYING;
		}
	} else if (m_iNbPlayers == MONITOR_DISPLAYING)
	{	
		if (m_pSession->getMPClient()->getNbPlayersOnline()>0)
		{	itoa(m_pSession->getMPClient()->getNbPlayersOnline(), cNumString, 10);
			strcpy(cText, s_szNbPlayersDisplayText);
			strcat(cText, cNumString);
		} else
		{	strcpy(cText, s_szZeroPlayerDisplayText);
		}

		m_pScreen->pDDSBack->TextSurf(lineX, lineY, s_dwTextColor, cText);

		// Verifie si la duree d'affichage ne s'est pas ecoulee
		if ((GameHistory->GetTime() - m_dwNbPlayersDisplayTime) > s_dwNbPlayersDisplayDelay)
		{	m_iNbPlayers = MONITOR_DONE;
		}
	}

	// Nom des joueurs deja connectes lors de l'ouverture de la session
	if ((m_iNbPlayers == MONITOR_DONE) && (m_iClientAlreadyConnected == MONITOR_NONE))
	{	if (   (m_pSession->getMPClient()!=NULL) 
			&& (m_pSession->getMPClient()->getNbPlayersOnline()>0) )
		{	m_dwClientAlreadyConnectedDisplayTime = GameHistory->GetTime();
			m_iClientAlreadyConnected = MONITOR_DISPLAYING;
			m_iClientDisplayingIndex = 0;
		} else
		{	m_iClientAlreadyConnected = MONITOR_DONE;
			std::vector<char*> &list = m_pSession->getMPClient()->getEnterPlayerInfo();
				for (int i=0 ; i < list.size() ; i++)
				{	delete list[i];
				}
				list.clear();
		}
	} else if (m_iClientAlreadyConnected == MONITOR_DISPLAYING)
	{	
		ZEGAME_PLAYER_INFO* player = m_pSession->getMPClient()->getClientInfos(m_iClientDisplayingIndex);
		strcpy(cText, "Player ");
		strcat(cText, player->strPlayerName);
		strcat(cText, " is already in ZeGame");
		m_pScreen->pDDSBack->TextSurf(lineX, lineY, s_dwTextColor, cText);

		// Verifie si la duree d'affichage ne s'est pas ecoulee
		if ((GameHistory->GetTime() - m_dwClientAlreadyConnectedDisplayTime) > s_dwClientAlreadyConnectedDisplayDelay)
		{	m_iClientDisplayingIndex++;
			m_dwClientAlreadyConnectedDisplayTime = GameHistory->GetTime();
			m_iClientAlreadyConnected = MONITOR_DONE;
			std::vector<char*> &list = m_pSession->getMPClient()->getEnterPlayerInfo();
			for (int i=0 ; i < list.size() ; i++)
			{	delete list[i];
			}
			list.clear();			
		}
	}

	// Nom des joueurs ayant quittés la partie
	if ((m_iClientConnection == MONITOR_DONE) && (m_iPlayerExit == MONITOR_NONE)
		&& (m_iClientAlreadyConnected == MONITOR_DONE))
	{	std::vector<char*> &list = m_pSession->getMPClient()->getExitPlayerInfo();
		if (list.size()>0)
		{	m_dwPlayerExitDisplayTime = GameHistory->GetTime();
			m_iPlayerExit = MONITOR_DISPLAYING;
			strcpy(m_szPlayerExitText, "The following players have left ZeGame : ");
			for (int i=0 ; i < list.size() ; i++)
			{	if (i>0) strcat(m_szPlayerExitText, ", ");
				strcat(m_szPlayerExitText, list[i]);
				delete list[i];
			}
			list.clear();
		}
	} else if (m_iPlayerExit == MONITOR_DISPLAYING)
	{	m_pScreen->pDDSBack->TextSurf(lineX, lineY, s_dwTextColor, m_szPlayerExitText);
		// Verifie si la duree d'affichage ne s'est pas ecoulee
		if ((GameHistory->GetTime() - m_dwPlayerExitDisplayTime) > s_dwPlayerExitDisplayDelay)
		{	m_iPlayerExit = MONITOR_NONE;
		}
	}

	// Nom des joueurs ayant rejoints la partie
	if ((m_iClientConnection == MONITOR_DONE) && (m_iPlayerEnter == MONITOR_NONE)
		&& (m_iClientAlreadyConnected == MONITOR_DONE) && (m_iPlayerExit != MONITOR_DISPLAYING) )
	{	std::vector<char*> &list = m_pSession->getMPClient()->getEnterPlayerInfo();
		if (list.size()>0)
		{	m_dwPlayerEnterDisplayTime = GameHistory->GetTime();
			m_iPlayerEnter= MONITOR_DISPLAYING;
			strcpy(m_szPlayerEnterText, "The following players have join ZeGame : ");
			for (int i=0 ; i < list.size() ; i++)
			{	if (i>0) strcat(m_szPlayerEnterText, ", ");
				strcat(m_szPlayerEnterText, list[i]);
				delete list[i];
			}
			list.clear();
		}
	} else if (m_iPlayerEnter == MONITOR_DISPLAYING)
	{	m_pScreen->pDDSBack->TextSurf(lineX, lineY, s_dwTextColor, m_szPlayerEnterText);
		// Verifie si la duree d'affichage ne s'est pas ecoulee
		if ((GameHistory->GetTime() - m_dwPlayerEnterDisplayTime) > s_dwPlayerEnterDisplayDelay)
		{	m_iPlayerEnter = MONITOR_NONE;
		}
	}
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//

/////////////////////////////// Fin de MPMONITOR ///////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Fonctions ANNEXES														  //
////////////////////////////////////////////////////////////////////////////////

//=== Initialise le Moniteur multi-joueurs ===================================//
void InitGame_MPMonitor()
{	g_pMPMonitor = new MPMonitor();
	g_pMPMonitor->setScreen(DS);
	g_pMPMonitor->setSession(&g_MPSession);
}
//----------------------------------------------------------------------------//

//=== Referme proprement le monitor multi-joueurs ============================//
void CloseGame_MPMonitor()
{	delete g_pMPMonitor;
	g_pMPMonitor = NULL;
}
//----------------------------------------------------------------------------//