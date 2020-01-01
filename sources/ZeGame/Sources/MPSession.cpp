////////////////////////////////////////////////////////////////////////////////
//  Unité "MPSession.cpp" : Couche session multi-joueur de ZeGame			  //
//                                                                            //
//	Description : Gere l'etablissement de la connexion multi-joueur en allant //
//				  chercher sur le serveur web si un serveur existe deja, le	  //
//				  creer si aucun serveur n'est lance et active le client.	  //
//                                                                            //
//  Copyright (C) 2001 Antoine Rey    antoine.rey@free.fr					  //
//                                    elric@ifrance.com		                  //
//  Version      : 0.1			                                              //
//  Creation     : 06/03/2001 		                                          //
//  Last upgrade : <none>                                                     //
////////////////////////////////////////////////////////////////////////////////

#include "ZeNain.h"					//--- Special for Visual C++ 6 MFC ---
#include "consts.h"					// Constantes globales à tout le projet
#include "MPServer.h"				// Serveur multi-joueur utilisant DirectPlay
#include "MPClient.h"				// Client multi-joueur utilisant DirectPlay
#pragma hdrstop
#include "ZGMultiplayer.h"			// Constantes communes a toute la partir reseau de ZeGame
#include "MPSession.h"				// Header de la classe


////////////////////////////////////////////////////////////////////////////////
// Classe MPSESSION					                                          //
////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------- Variables 
int MPSession::SERVER_SESSION = 1;		// Arrete le serveur
int MPSession::CLIENT_SESSION = 2;		// Arrete le client
int MPSession::DUAL_SESSION = MPSession::SERVER_SESSION | MPSession::CLIENT_SESSION;

HANDLE MPSession::hSessionStop = NULL;	// Arrete la session multi-joueur
HANDLE MPSession::hSessionStopped = CreateEvent(NULL, FALSE, FALSE, NULL);

HANDLE MPSession::hSessionServerStarted = CreateEvent(NULL, FALSE, FALSE, NULL);

bool MPSession::s_bDefaultLocalNetwork = false;
bool MPSession::s_bDefaultServer = false;

//=== Constructeur par defaut ================================================//
// @param	flag	permet de forcer la session a lancer uniquement le serveur
//					multi-joueur soit le client ou soit les 2 a la fois
MPSession::MPSession(int flag):m_iSessionFlag(flag)
{	m_bMultiPlayerActivate = true;	// Mode multi-joueur active par defaut
	m_pMPServer = NULL;
	m_pMPClient = NULL;

	pDP8AddressHost  = NULL;
    pDP8AddressLocal = NULL;
	m_hEnumAsyncOp = NULL;

	m_bLocalNetwork = s_bDefaultLocalNetwork;
	m_szServerIP[0] = '\0';
	m_bServer = s_bDefaultServer;

	hSessionStop = CreateEvent(NULL, FALSE, FALSE, NULL);
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
MPSession::~MPSession()
{	ms_Singleton = NULL;
}	
//----------------------------------------------------------------------------//

//=== Initialise la session multi-joueur en creant le cote client et =========//
//	  eventuellement le serveur
void MPSession::InitSession()
{	bool serverExists = false;

	
	// Si c'est une partie online, on accède tout d'abord au serveur web pour
	// savoir si un serveur multi-joueurs existe ?
	if (!m_bLocalNetwork)
	{	
		// Recupere sur www.zegame.net les informations sur l'etat du serveur multi-joueur
		m_httpSocket.requestServerInfos();
		
		// Si le serveur web est injoignable, on n'active pas la session multi-joueur
		if (!m_httpSocket.isServerAccesible())
		{	m_bMultiPlayerActivate = false;
			return;
		}

		serverExists = m_httpSocket.isServerExists();

		// Un serveur semble exister, mais on test si le serveur est qd meme en ligne
		// et ne s'est pas mal deconnecte.
		if (serverExists)	//TODO
		{	
		}
	} else if (m_bServer)
	// Force cet ordinateur à être server
	{	
		strcpy(m_szServerIP, "134.214.165.39");
		serverExists = false;

	} else
	// On se connecte à un serveur connu
	{	
		//strcpy(m_szServerIP, "134.214.165.60");		// PII 350
		strcpy(m_szServerIP, "134.214.165.39");	// PIII 650
		// On suppose que le serveur existe
		serverExists = true;
	}

	// Si aucun serveur multi-joueur n'existe, c'est a ce joueur de le creer
	if ((!serverExists) && ( (SERVER_SESSION & m_iSessionFlag) == SERVER_SESSION) )
	{	
		bool created = createMPServer();
		// On tente alors de s'y connecter, meme si il est en local sur la machine,
		// en demandant au site Web de nous renvoyer sa propre adresse.

		if (!created)
		{	m_bMultiPlayerActivate = false;
			StopSession(SERVER_SESSION | CLIENT_SESSION);
			return;
		}

		SetEvent(hSessionServerStarted);

		
		// Risque de BUG -> verifier ds le php
/*		m_httpSocket.requestServerInfos();
		// Si le serveur web est toujours injoignable, on laisse tomber
		// et on n'active pas la session multi-joueur
		if (!m_httpSocket.isServerAccesible())
		{	m_bMultiPlayerActivate = false;
			return;
		}*/

	}
	
	// Une fois que le serveur multi-joueur a ete mis en place, on peut lancer la session Joueur
	if ( (CLIENT_SESSION & m_iSessionFlag) == CLIENT_SESSION)
	{	HRESULT hr;

		// On n'arrive meme pas a creer le client
		if (!createMPClient())
		{	m_bMultiPlayerActivate = false;
			return;
		}
		
	
		// Essaye de se connecter au serveur
		hr = m_pMPClient->ConnectToServer(m_szServerIP, MPServer::m_dwDefaultServerPort);
		if (hr!=0) 
		{	m_bMultiPlayerActivate = false;
			return;
		}
		// Attend que l'on ai trouve la session multi-joueur du serveur
		DWORD dwErr = WaitForSingleObject(MPClient::hHostServerFounded, 30000);	
		// Impossible de demarrer le serveur : on stop la session multi-joueur
		if ((dwErr==WAIT_TIMEOUT)  || (dwErr==WAIT_ABANDONED) || (dwErr==WAIT_FAILED))
		{	m_bMultiPlayerActivate = false;
			return;
		}

		// On se connecte alors a cette session
		m_pMPClient->setPlayerName((char*) (LPCSTR) g_sPlayerName);
		hr = m_pMPClient->JoinSession(m_pMPClient->getConnectingSession() );
		// Attend que la connexion soit reellement finie
		dwErr = WaitForSingleObject(MPClient::m_hConnectCompleteEvent, 30000);	
		// Impossible de demarrer le serveur : on stop la session multi-joueur
		if ((dwErr==WAIT_TIMEOUT)  || (dwErr==WAIT_ABANDONED) || (dwErr==WAIT_FAILED))
		{	m_bMultiPlayerActivate = false;
			return;
		}
		
		
		if (hr==0)
		{	// Si tout c'est bien passe, on l'indique au serveur php3
			if (!m_bLocalNetwork)
			{	m_httpSocket.requestClientConnect(m_pMPClient->getClientURL(), (char*) m_httpSocket.getClientIP());
			}
		}		
		
	}
}
//----------------------------------------------------------------------------//

//=== Cree le serveur multi-joueur sur cette machine =========================//
// => previent le serveur web qu'un nouveau serveur multi-joueur existe quelque part
bool MPSession::createMPServer()
{	// Initialise et lance le serveur multi-joueur
	InitGame_MPServer();
	// On attend 30 sec avant de declarer que le serveur n'a pas pu se
	DWORD dwErr = WaitForSingleObject(MPServer::hServerLaunched, 30000);	
	// Impossible de demarrer le serveur : on stop la session multi-joueur
	if ((dwErr==WAIT_TIMEOUT)  || (dwErr==WAIT_ABANDONED) || (dwErr==WAIT_FAILED))
	{	int i = 0;
		if (dwErr==WAIT_TIMEOUT) i = 1;
		else if (dwErr==WAIT_ABANDONED) i = 2;
		else if (dwErr==WAIT_FAILED) i = 3;
		return false;
	}
	m_pMPServer = &MPServer::getSingleton();

	// Si c'est une partie online, on informe le serveur web qu'un serveur vient d'être 
	// créé quelque pqrt sur le Net
	if (!m_bLocalNetwork)
	{	m_httpSocket.requestServerNew(m_pMPServer->getServerURL(), NULL);	// NULL -> On laisse php s'occuper de prendre l'@IP
	}
	return true;
}
//----------------------------------------------------------------------------//

//=== Cree le client du cote du joueur =======================================//
bool MPSession::createMPClient()
{	m_pMPClient = new MPClient();
	m_pMPClient->InitClient();
	return true;
}
//----------------------------------------------------------------------------//

//=== Arrete la session multi-joueur en cours ================================//
// @param flag SERVER_SESSION et/ou CLIENT_SESSION
void MPSession::StopSession(int flag)
{	// On arrete la session joueur
	if ((flag & CLIENT_SESSION) == CLIENT_SESSION)
	{	if (m_pMPClient!=NULL)
		{	// ....
			// log-off TODO
			// ....

			m_pMPClient->StopClient();	
		}
	}

	// Puis on arrete le serveur
	if ((flag & SERVER_SESSION) == SERVER_SESSION)
	{	if ((m_pMPServer != NULL) && (m_pMPServer->isServerStarted()))
		{	
			// Informe le serveur web que le serveur va s'arrêter
			if (!m_bLocalNetwork)
			{	// Arrete le serveur
				char date[32];
				strcpy(date, m_httpSocket.getServerCnxDate());
				date[10]='+';
				m_httpSocket.requestServerDisconnect(m_pMPServer->getServerURL(), date);
			}
		
			SetEvent(MPServer::hServerStop);
		}
	}

	// La session multi-joeur est close, on peut la desinstancier
	SetEvent(MPSession::hSessionStopped);
}
//----------------------------------------------------------------------------//

bool MPSession::connectServer()
{	HRESULT hr;
	WCHAR* wszHostName = NULL;
	
	// Create the local device address object
    if( FAILED( hr = CoCreateInstance( CLSID_DirectPlay8Address, NULL, 
                                       CLSCTX_ALL, IID_IDirectPlay8Address,
                                       (LPVOID*) &pDP8AddressLocal ) ) )
    {
        DXTRACE_ERR( TEXT("CoCreateInstance"), hr );
        goto LCleanup;
    }

    // Set IP service provider
    if( FAILED( hr = pDP8AddressLocal->SetSP( &CLSID_DP8SP_TCPIP ) ) )
    {
        DXTRACE_ERR( TEXT("SetSP"), hr );
        goto LCleanup;
    }


    // Create the remote host address object
    if( FAILED( hr = CoCreateInstance( CLSID_DirectPlay8Address, NULL, 
                                       CLSCTX_ALL, IID_IDirectPlay8Address,
                                       (LPVOID*) &pDP8AddressHost ) ) )
    {
        DXTRACE_ERR( TEXT("CoCreateInstance"), hr );
        goto LCleanup;
    }

    // Set IP service provider
    if( FAILED( hr = pDP8AddressHost->SetSP( &CLSID_DP8SP_TCPIP ) ) )
    {
        DXTRACE_ERR( TEXT("SetSP"), hr );
        goto LCleanup;
    }

    if (!m_bLocalNetwork)
	{	strcpy(m_szServerIP, m_httpSocket.getServerIP());
	}
	
	// Set the remote host name (if provided)
    if( m_szServerIP != NULL && m_szServerIP[0] != 0 )
    {   DWORD dwPort = 0;
		// Parse out port if it exists (expected form of "xxx.xxx.xxx.xxx:port")
		TCHAR* strPort = _tcschr( m_szServerIP, TEXT(':') );
		if( NULL != strPort )
		{   // Chop off :port from end of strIPAddress
			TCHAR* strEndOfIP = strPort;
			*strEndOfIP = 0;

			// Get port number from strPort
			strPort++;
			dwPort = _ttoi( strPort );
		}
		// On convertit l'IP de l'Hote en adresse standard
		wszHostName = new WCHAR[_tcslen(m_szServerIP)+1];
		DXUtil_ConvertGenericStringToWide( wszHostName, m_szServerIP );

		hr = pDP8AddressHost->AddComponent( DPNA_KEY_HOSTNAME, wszHostName, 
											(wcslen(wszHostName)+1)*sizeof(WCHAR), 
											DPNA_DATATYPE_STRING );
		if( FAILED(hr) )
		{
			DXTRACE_ERR( TEXT("AddComponent"), hr );
			goto LCleanup;
		}

		// Si le port a ete specifie dans la chaine IP, on la rahoute au pDP8AddressHost
		if( dwPort != 0 )
		{	hr = pDP8AddressHost->AddComponent( DPNA_KEY_PORT, &dwPort, sizeof(dwPort),
												DPNA_DATATYPE_DWORD );
			if( FAILED(hr) )
			{	DXTRACE_ERR( TEXT("AddComponent"), hr );
				goto LCleanup;
			}
        }
    }


	// DPN_APPLICATION_DESC Describes the settings for a DirectPlay application.
	DPN_APPLICATION_DESC   dpnAppDesc;
    ZeroMemory( &dpnAppDesc, sizeof( DPN_APPLICATION_DESC ) );
    dpnAppDesc.dwSize = sizeof( DPN_APPLICATION_DESC );
    dpnAppDesc.guidApplication = g_guidZeGame;

    // Enumerate all StressMazeApp hosts running on IP service providers
    hr = m_pMPClient->getDPClient()->EnumHosts( &dpnAppDesc, pDP8AddressHost, 
                                 pDP8AddressLocal, NULL, 
                                 0, INFINITE, 0, INFINITE, NULL, 
                                 &m_hEnumAsyncOp, 0 );
    if( FAILED(hr) )
    {
        if( hr != DPNERR_INVALIDDEVICEADDRESS && 
            hr != DPNERR_ADDRESSING ) // This will be returned if the ip address is is invalid. 
            DXTRACE_ERR( TEXT("EnumHosts"), hr );
        goto LCleanup;
    }

LCleanup:
    SAFE_RELEASE( pDP8AddressHost);
    SAFE_RELEASE( pDP8AddressLocal );
    SAFE_DELETE( wszHostName );

    if( hr == DPNERR_PENDING )
        hr = DPN_OK;

    return (hr == 0);
}
//----------------------------------------------------------------------------//

//=== Initialise la session multi-joueurs ====================================//
int MPSession::LaunchSession(MPSession *session)
{	session->InitSession();
	// Attend qu'on lui dise d'arreter la session multi-joueur avant de stopper la tache
	WaitForSingleObject(MPSession::hSessionStop, INFINITE);
	if (&g_MPSession != NULL)
	{	g_MPSession.StopSession(MPSession.SERVER_SESSION | MPSession.CLIENT_SESSION);
	}
	return 0;
}
//----------------------------------------------------------------------------//

/////////////////////////////// Fin de MPSESSION ////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Fonctions ANNEXES														  //
////////////////////////////////////////////////////////////////////////////////

//=== Initialise le Serveur multi-joueur =====================================//
// @param	flag	permet de forcer la session a lancer uniquement le serveur multi-joueur
//					soit le client ou soit les 2 a la fois
void InitGame_MPSession(int flag)
{	MPSession *pMPSession = new MPSession(flag);

	DWORD dwThreadId;
    HANDLE hThread; 
    hThread = CreateThread( 
        NULL,                        // no security attributes 
        0,                           // use default stack size  
        (LPTHREAD_START_ROUTINE) pMPSession->LaunchSession,               // method function 
        pMPSession,					 // this  
        0,                           // use default creation flags 
        &dwThreadId);                // returns the thread identifier
	SetThreadPriority(hThread, THREAD_PRIORITY_BELOW_NORMAL);
}
//----------------------------------------------------------------------------//

//=== Referme proprement la session en cours =================================//
// Attention : si le joueur fait office de server, le server s'arret aussi
void CloseGame_MPSession()
{	MPSession *pMPSession = MPSession::getSingletonPtr();
	if (pMPSession != NULL)
	{	SetEvent(MPSession::hSessionStop);	// Envoi l'evenement "Arreter le Serveur"
		// Attends que la session se soit desinitialiser proprement avant de le desinstancier
		WaitForSingleObject(MPSession::hSessionStopped, INFINITE);
		delete pMPSession;
	}
}
//----------------------------------------------------------------------------//

