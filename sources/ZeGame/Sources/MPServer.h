////////////////////////////////////////////////////////////////////////////////
//  Unité "MPServer.h" : Serveur Multi-player sous DirectPlay				  //
//                                                                            //
//	Description :															  //
//                                                                            //
//  Copyright (C) 2001 Antoine Rey    antoine.rey@free.fr					  //
//                                    elric@ifrance.com		                  //
//  Version      : 0.1			                                              //
//  Creation     : 04/03/2001 		                                          //
//  Last upgrade : <none>                                                     //
////////////////////////////////////////////////////////////////////////////////

#ifndef MPServerH
#define MPServerH

//---------------------------------------------------------------------- Include
#include "Singleton.h"		// Template de toutes les classes singletons
#include <dplay8.h>			// DirectPlay8
#include <dpaddr.h>
#include <dxerr8.h>
#include "ZGMultiplayer.h"	// Constantes communes a toute la partir reseau de ZeGame
#include "DXUtil.h"

//----------------------------------------------------------------------- Define
#define g_MPServer MPServer::getSingleton()
#define g_pDPServer MPServer::getSingleton().getDPServer()

//------------------------------------------------------------------------- Enum

//------------------------------------------------------------------------ Class
class GameDebug;

////////////////////////////////////////////////////////////////////////////////
// Classe MPSERVER															  //
class MPServer : public Singleton <MPServer>
{ // ---------------------------------------------------------- Attributs privés
  protected:
	IDirectPlay8Server* m_pDPServer;                // DirectPlay server object
	LONG               m_lNumberOfActivePlayers;    // Nombre de player connectes au serveur
	TCHAR              m_strAppName[256];           // Nom de l'application
	CHAR			   m_strServerURL[512];			// URL au format DirectPlay du Server (caractere ANSI)
	TCHAR              m_strSessionName[MAX_PATH];	// Nom de la Session
	DWORD              m_dwPort;                    // Port
	BOOL               m_bServerStarted;            // TRUE si le serveur a demarre
	CRITICAL_SECTION   m_csServerContext;			// A utiliser dans un contexte multi-thread
	GameDebug*		   m_pLog;						// Fichier de log		

	HWND               m_hDlg;                      // HWND of main dialog

	
  public:
	static HANDLE hServerLaunched;					// Evenement envoye lorsque le serveur a demarre
	static HANDLE hServerStop;						// Signal arretant le server
	
	static DWORD	   m_dwDefaultServerPort;		// Port que le serveur utilise par defaut

	CRITICAL_SECTION   m_csPlayerContext;

  // ------------------------------------------------------ Méthodes publiques
  public:
	
	//--- Constructeurs & Destructeurs ---
	MPServer();							// Constructeur par defaut
	virtual ~MPServer();				// Destructeur

	HRESULT  StartServer();			// Demarre le serveur DirectPlay
	void     StopServer();			// Arrete le Serveur multi-joueur
	// Previent le joueur #dpnidTarget qu'un nouveau joueur #pPlayerInfo vient de se connecter au serveur
	HRESULT  SendCreatePlayerMsg( ZEGAME_PLAYER_INFO* pPlayerInfo, DPNID dpnidTarget );
	// Previent le joueur #dpnidTarget que tel joueur #pPlayerInfo est deja connecte au serveur 
	HRESULT  SendPlayerOnlineMsg( ZEGAME_PLAYER_INFO* pPlayerInfo, DPNID dpnidTarget );

	// Envoi 
	HRESULT  SendWorldStateToNewPlayer( DPNID dpnidPlayer );

	void incrementNumberOfActivePlayer();
	void decrementNumberOfActivePlayer();

	//INT_PTR CALLBACK ServerDlgProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
	//VOID     StopServer( HWND hDlg );
	//VOID     DisplayPlayers( HWND hDlg );
	//HRESULT  SendDestroyPlayerMsgToAll( ZEGAME_PLAYER_INFO* pPlayerInfo );
	//HRESULT  SendWaveMessageToAll( DPNID dpnidFrom );

	//--- Fonctions inline ---
	inline CRITICAL_SECTION& getServerContext() { return m_csServerContext; }
	inline IDirectPlay8Server* getDPServer() { return m_pDPServer; }
	inline GameDebug* getLog() { return m_pLog; }
	inline CHAR* getServerURL() { return m_strServerURL; }
	inline BOOL isServerStarted() { return m_bServerStarted; }

	inline void setDlgHandle(HWND h) { m_hDlg = h; }
	inline HWND getDlgHandle() { return m_hDlg; }

	inline LONG getNumberOfActivePlayers() { return m_lNumberOfActivePlayers; }

	HRESULT	SendDestroyPlayerMsgToAll( ZEGAME_PLAYER_INFO* pPlayerInfo );

	// -------------------------------------------------------- Méthodes privees
  protected:
	  

	

    
};
//=========================== Fin de la classe MPSERVER ======================//


//------------------------------------------------------------ Fonctions annexes
void InitGame_MPServer();					// Initialise le serveur multi-joueur
void CloseGame_MPServer();					// Referme proprement le serveur
DWORD WINAPI LaunchServer(LPVOID lpParam);	// Demarre le serveur multi-joueur


// Fonction CallBack recevant tous les messages destines au serveur multi-joueur DirectPlay
HRESULT WINAPI	DirectPlayServerMessageHandler( PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer );


#endif	// MPServerH


