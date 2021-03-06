////////////////////////////////////////////////////////////////////////////////
//  Unit� "MPClient.cpp" : Client Multi-player sous DirectPlay				  //
//                                                                            //
//	Description :															  //
//                                                                            //
//  Copyright (C) 2001 Antoine Rey    antoine.rey@free.fr					  //
//                                    elric@ifrance.com		                  //
//  Version      : 0.1			                                              //
//  Creation     : 04/03/2001 		                                          //
//  Last upgrade : <none>                                                     //
////////////////////////////////////////////////////////////////////////////////

#include "ZeNain.h"					//--- Special for Visual C++ 6 MFC ---
#include "consts.h"					// Constantes globales � tout le projet
#include <dxerr8.h>
#pragma hdrstop
#include "DXUtil.h"					// DXUtil_ConvertGenericStringToWide()
#include "MPClient.h"				// Header de la classe

#pragma warning (disable: 4201)
#include <mmsystem.h>				// getTime()


////////////////////////////////////////////////////////////////////////////////
// Classe MPCLIENT					                                          //
////////////////////////////////////////////////////////////////////////////////

HANDLE MPClient::hHostServerFounded = CreateEvent(NULL,	FALSE, FALSE, NULL);
HANDLE MPClient::m_hConnectCompleteEvent = CreateEvent(NULL,	FALSE, FALSE, NULL);

//=== Constructeur par defaut ================================================//
MPClient::MPClient()
{	m_pDPClient = NULL;

	pDP8AddressHost = NULL;
    pDP8AddressLocal = NULL;
    wszHostName = NULL;
	m_hEnumAsyncOp = NULL;

	pPlayerName = NULL;
	m_strClientURL[0]='\0';

	m_bEnumListChanged = FALSE;
	bHostServerFounded = false;
	m_bClientConnected = false;
	m_dwNbPlayersOnline = 0;

	m_hConnectAsyncOp = NULL;
	m_hEnumAsyncOp = NULL;
	m_hrConnectComplete = 0;

	InitializeCriticalSection( &m_csHostEnum );

	// Setup the m_DPHostEnumHead circular linked list
    ZeroMemory( &m_DPHostEnumHead, sizeof( DPHostEnumInfo ) );
    m_DPHostEnumHead.pNext = &m_DPHostEnumHead;
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
MPClient::~MPClient()
{	if (pPlayerName!=NULL)
	{	delete [] pPlayerName;
		pPlayerName = NULL;
	}

	for (int i=0 ; i < m_pPlayerInfo.size() ; i++)
	{	delete m_pPlayerInfo[i];
	}
	m_pPlayerInfo.clear();

	DeleteCriticalSection( &m_csPlayerContext );
}	
//----------------------------------------------------------------------------//

void MPClient::setPlayerName(char* name)
{	if (name!=NULL)
	{	if (pPlayerName!=NULL)
		{	delete [] pPlayerName;
			pPlayerName = NULL;
		}
		pPlayerName = new char[strlen(name)+1];
		strcpy(pPlayerName, name);
	}
}

//=== Initialise le Client DirectPlay ========================================//
HRESULT MPClient::InitClient()
{	HRESULT hr;
	// Cree l'IDirectPlay8Client
    if( FAILED( hr = CoCreateInstance( CLSID_DirectPlay8Client, NULL, 
                                       CLSCTX_INPROC_SERVER,
                                       IID_IDirectPlay8Client, 
                                       (LPVOID*) &m_pDPClient ) ) )
    return DXTRACE_ERR( TEXT("CoCreateInstance"), hr );

	// Initialise le Client en lui specifiant la fonction callback
	if( FAILED( hr = m_pDPClient->Initialize( NULL, DirectPlayClientMessageHandler, 0 ) ) )
	return DXTRACE_ERR( TEXT("Initialize"), hr );

	// Cree l'adresse locale du client
    if( FAILED( hr = CoCreateInstance( CLSID_DirectPlay8Address, NULL, 
                                       CLSCTX_ALL, IID_IDirectPlay8Address,
                                       (LPVOID*) &pDP8AddressLocal ) ) )
	return DXTRACE_ERR( TEXT("AdressLocal"), hr );
	
	// Set IP service provider
    if( FAILED( hr = pDP8AddressLocal->SetSP( &CLSID_DP8SP_TCPIP ) ) )
    {	return DXTRACE_ERR( TEXT("AddressLocal->SetSP"), hr );
    }

	InitializeCriticalSection(&m_csPlayerContext);

	return 0;
}
//----------------------------------------------------------------------------//

//=== Deconnecte le client de la session multi-joueur ========================//
HRESULT	 MPClient::StopClient()
{	if( m_pDPClient )
    {
        m_pDPClient->Close(0);
        SAFE_RELEASE( m_pDPClient );
    }
	return 0;
}
//----------------------------------------------------------------------------//

//=== Connexion au Serveur ===================================================//
HRESULT MPClient::ConnectToServer(const char* serverIP, int port)
{	HRESULT hr;
	DWORD numChars = 512;

	
	DPN_APPLICATION_DESC   dpnAppDesc;
    IDirectPlay8Address*   pDP8AddressHost  = NULL;
    IDirectPlay8Address*   pDP8AddressLocal = NULL;
    WCHAR*                 wszHostName      = NULL;

    // Create the local device address object
    if( FAILED( hr = CoCreateInstance( CLSID_DirectPlay8Address, NULL, 
                                       CLSCTX_ALL, IID_IDirectPlay8Address,
                                       (LPVOID*) &pDP8AddressLocal ) ) )
    {
        DXTRACE_ERR( TEXT("MPClient::ConnectToServer::CoCreateInstance"), hr );
        goto LCleanup;
    }

    // Set IP service provider
    if( FAILED( hr = pDP8AddressLocal->SetSP( &CLSID_DP8SP_TCPIP ) ) )
    {
        DXTRACE_ERR( TEXT("MPClient::ConnectToServer::SetSP"), hr );
        goto LCleanup;
    }


    // Create the remote host address object
    if( FAILED( hr = CoCreateInstance( CLSID_DirectPlay8Address, NULL, 
                                       CLSCTX_ALL, IID_IDirectPlay8Address,
                                       (LPVOID*) &pDP8AddressHost ) ) )
    {
        DXTRACE_ERR( TEXT("MPClient::ConnectToServer::CoCreateInstance"), hr );
        goto LCleanup;
    }

    // Set IP service provider
    if( FAILED( hr = pDP8AddressHost->SetSP( &CLSID_DP8SP_TCPIP ) ) )
    {
        DXTRACE_ERR( TEXT("MPClient::ConnectToServer::SetSP"), hr );
        goto LCleanup;
    }

    // Set the remote host name (if provided)
	if( serverIP != NULL && serverIP[0] != 0 )
    {
        DWORD dwPort = 0;

        // Parse out port if it exists (expected form of "xxx.xxx.xxx.xxx:port")
        TCHAR* strPort = _tcschr( serverIP, TEXT(':') );
        if( NULL != strPort )
        {   // Chop off :port from end of strIPAddress
            TCHAR* strEndOfIP = strPort;
            *strEndOfIP = 0;
            // Get port number from strPort
            strPort++;
            dwPort = _ttoi( strPort );
        } 
		else if (port != 0)
		{	dwPort = port;
		}

        wszHostName = new WCHAR[_tcslen(serverIP)+1];
        DXUtil_ConvertGenericStringToWide( wszHostName, serverIP );

        hr = pDP8AddressHost->AddComponent( DPNA_KEY_HOSTNAME, wszHostName, 
                                            (wcslen(wszHostName)+1)*sizeof(WCHAR), 
                                            DPNA_DATATYPE_STRING );
        if( FAILED(hr) )
        {
            DXTRACE_ERR( TEXT("MPClient::ConnectToServer::AddComponent"), hr );
            goto LCleanup;
        }

        // If a port was specified in the IP string, then add it.
        // Games will typically hard code the port so the user need not know it
        if( dwPort != 0 )
        {
            hr = pDP8AddressHost->AddComponent( DPNA_KEY_PORT, 
                                                &dwPort, sizeof(dwPort),
                                                DPNA_DATATYPE_DWORD );
            if( FAILED(hr) )
            {
                DXTRACE_ERR( TEXT("MPClient::ConnectToServer::AddComponent"), hr );
                goto LCleanup;
            }
        }
    }

	m_bEnumListChanged = TRUE;

	ZeroMemory( &dpnAppDesc, sizeof( DPN_APPLICATION_DESC ) );
    dpnAppDesc.dwSize = sizeof( DPN_APPLICATION_DESC );
    dpnAppDesc.guidApplication = g_guidZeGame;
    // Enumerate all StressMazeApp hosts running on IP service providers
    hr = m_pDPClient->EnumHosts( &dpnAppDesc, pDP8AddressHost, 
                                 pDP8AddressLocal, NULL, 
                                 0, INFINITE, 0, INFINITE, NULL, 
                                 &m_hEnumAsyncOp, 0 );
/*	hr = m_pDPClient->EnumHosts( &dpnAppDesc, pDP8AddressHost, 
                                 pDP8AddressLocal, NULL, 
                                 0, INFINITE, 0, INFINITE, NULL, 
                                 NULL, DPNENUMHOSTS_SYNC );*/
    if( FAILED(hr) )
    {
        if( hr != DPNERR_INVALIDDEVICEADDRESS && 
            hr != DPNERR_ADDRESSING ) // This will be returned if the ip address is is invalid. 
            DXTRACE_ERR( TEXT("MPClient::ConnectToServer::EnumHosts"), hr );
        goto LCleanup;
    }

	hr = pDP8AddressLocal->GetURLA(m_strClientURL, &numChars);


LCleanup:
    SAFE_RELEASE( pDP8AddressHost);
    SAFE_RELEASE( pDP8AddressLocal );
    SAFE_DELETE( wszHostName );

    if( hr == DPNERR_PENDING )
        hr = DPN_OK;

    return hr;
}
//----------------------------------------------------------------------------//

//=== Ajoute un joueur a la liste des joueurs deja connectes a la session  ===//
//	multi-joueurs
//	@return	faux si ce joueur existait deja ds la liste
bool MPClient::addPlayer(ZEGAME_PLAYER_INFO* player)
{	int size = m_pPlayerInfo.size();
	// Verifie tout d'abord si ce joueur n'est pas deja present ds la liste
	for (int i=0 ; i < size ; i++)
	{	if (m_pPlayerInfo[i]->dpnidPlayer == player->dpnidPlayer)
		{	delete player;
			return false;
		}
	}
	// Sinon, on l'ajoute a la fin de la liste des joueurs online
	m_pPlayerInfo.push_back(player);
	setNbPlayersOnline(m_pPlayerInfo.size());
	m_bNewPlayerConnected = true;
	char* name = new char[strlen(player->strPlayerName)+1];
	strcpy(name, player->strPlayerName);
	m_pEnterPlayerInfo.push_back(name);
	return true;
}


//=== Supprime tel joueur de la liste des joueurs online =====================//
// Attention : le pointeur #player devient invalide
bool MPClient::removePlayer(ZEGAME_PLAYER_INFO* player)
{	std::vector<ZEGAME_PLAYER_INFO*>::iterator iter;
	for (iter = m_pPlayerInfo.begin() ; iter != m_pPlayerInfo.end() ; iter++)
	{	if ((*iter)->dpnidPlayer == player->dpnidPlayer)
		{	m_pPlayerInfo.erase(iter);
			char* name = new char[strlen(player->strPlayerName)+1];
			strcpy(name, player->strPlayerName);
			m_pExitPlayerInfo.push_back(name);
			delete player;
			setNbPlayersOnline(m_pPlayerInfo.size());
			return true;
		}
	}
	return false;
}
//----------------------------------------------------------------------------//

//=== Renvoi une r�f�rence sur les infos de tel joueur =======================//
ZEGAME_PLAYER_INFO* MPClient::GetPlayerStruct(int dpnid)
{	for (int i=0 ; i < m_pPlayerInfo.size() ; i++)
	{	if (m_pPlayerInfo[i]->dpnidPlayer == dpnid)
		{	return m_pPlayerInfo[i];
		}
	}
	return NULL;
}
//----------------------------------------------------------------------------//

/////////////////////////////// Fin de MPCLIENT ////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Fonctions ANNEXES														  //
////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// Name: DirectPlayMessageHandler
// Desc: Handler for DirectPlay messages.  This function is called by
//       the DirectPlay message handler pool of threads, so be careful of thread
//       synchronization problems with shared memory
//-----------------------------------------------------------------------------
HRESULT WINAPI DirectPlayClientMessageHandler( PVOID pvUserContext, 
                                         DWORD dwMessageId, 
                                         PVOID pMsgBuffer )
{
    // This function is called by the DirectPlay message handler pool of 
    // threads, so be careful of thread synchronization problems with shared memory

    switch( dwMessageId )
    {
		
		case DPN_MSGID_ENUM_HOSTS_RESPONSE:
		{
            PDPNMSG_ENUM_HOSTS_RESPONSE pEnumHostsResponseMsg;
            pEnumHostsResponseMsg = (PDPNMSG_ENUM_HOSTS_RESPONSE)pMsgBuffer;
            // Take note of the host response
            g_MPClient.SessionsDlgNoteEnumResponse( pEnumHostsResponseMsg );
			// Si c'est la 1ere fois qu'on recoit une reponse disant qu'une
			// session du jeu a ete trouve a l'adresse du serveur, on previent
			// tout le monde
			if (!g_MPClient.bHostServerFounded)
			{	g_MPClient.bHostServerFounded = true;
				SetEvent(MPClient::hHostServerFounded);				
			}

            break;
        }

		case DPN_MSGID_ASYNC_OP_COMPLETE:
        {
            PDPNMSG_ASYNC_OP_COMPLETE pAsyncOpCompleteMsg;
            pAsyncOpCompleteMsg = (PDPNMSG_ASYNC_OP_COMPLETE)pMsgBuffer;

            if( pAsyncOpCompleteMsg->hAsyncOp == g_MPClient.m_hEnumAsyncOp )
            {
                g_MPClient.SessionsDlgEnumListCleanup();
                g_MPClient.m_hEnumAsyncOp = NULL;
            }
            break;
        }
		
		case DPN_MSGID_CONNECT_COMPLETE:
        {
            PDPNMSG_CONNECT_COMPLETE pConnectCompleteMsg;
            pConnectCompleteMsg = (PDPNMSG_CONNECT_COMPLETE)pMsgBuffer;

            // Set m_hrConnectComplete, then set an event letting
            // everyone know that the DPN_MSGID_CONNECT_COMPLETE msg
            // has been handled
            g_MPClient.m_hrConnectComplete = pConnectCompleteMsg->hResultCode;
			g_MPClient.setClientConnected(true);
            SetEvent( MPClient::m_hConnectCompleteEvent );
            break;
        }
		
		case DPN_MSGID_TERMINATE_SESSION:
        {
        /*    PDPNMSG_TERMINATE_SESSION pTerminateSessionMsg;
            pTerminateSessionMsg = (PDPNMSG_TERMINATE_SESSION)pMsgBuffer;

            g_hrDialog = DPNERR_CONNECTIONLOST;
            EndDialog( g_hDlg, 0 );*/
            break;
        }

        case DPN_MSGID_RECEIVE:
        {
            HRESULT hr;
            PDPNMSG_RECEIVE pReceiveMsg;
            pReceiveMsg = (PDPNMSG_RECEIVE)pMsgBuffer;

            GAMEMSG_GENERIC* pMsg = (GAMEMSG_GENERIC*) pReceiveMsg->pReceiveData;
            switch( pMsg->dwType )
            {
                // Message venant du Serveur Online
				// => Previent le client qu'il lui a affecte l'ID passe en parametre
				case GAME_MSGID_SET_ID:
                {	// The host is tell us the DPNID for this client
                    GAMEMSG_SET_ID* pSetIDMsg;
                    pSetIDMsg = (GAMEMSG_SET_ID*) pReceiveMsg->pReceiveData;
                    g_MPClient.setDPNIDLocalPlayer(pSetIDMsg->dpnidPlayer);
					g_MPClient.setNbPlayersOnline(pSetIDMsg->dwNbPlayersOnline);
                    break;
                }

                // Message venant du Serveur Online
				// => Indique que tel nouveau joueur vient de rejoindre la partie
				case GAME_MSGID_CREATE_PLAYER:
				case GAME_MSGID_ONLINE_PLAYER:
                {
                    // The host is telling us about a new player 
					GAMEMSG_CREATE_PLAYER* pCreatePlayerMsg;
                    pCreatePlayerMsg = (GAMEMSG_CREATE_PLAYER*) pReceiveMsg->pReceiveData;

                    // Create a new and fill in a ZEGAME_PLAYER_INFO
                    ZEGAME_PLAYER_INFO* pPlayerInfo = new ZEGAME_PLAYER_INFO;
                    ZeroMemory(pPlayerInfo, sizeof(ZEGAME_PLAYER_INFO));
                    pPlayerInfo->lRefCount   = 1;
                    pPlayerInfo->dpnidPlayer = pCreatePlayerMsg->dpnidPlayer;
                    strcpy(pPlayerInfo->strPlayerName, pCreatePlayerMsg->strPlayerName);
				
					EnterCriticalSection( &g_MPClient.m_csPlayerContext );
                    g_MPClient.addPlayer(pPlayerInfo);
					LeaveCriticalSection( &g_MPClient.m_csPlayerContext );
                    break;
                };
				
				
                case GAME_MSGID_DESTROY_PLAYER:
                {
                    // The host is telling us about a player that's been destroyed
                    ZEGAME_PLAYER_INFO* pPlayerInfo = NULL;
                    GAMEMSG_DESTROY_PLAYER* pDestroyPlayerMsg;
                    pDestroyPlayerMsg = (GAMEMSG_DESTROY_PLAYER*) pReceiveMsg->pReceiveData;

                    // Get the player struct accosicated with this DPNID
                    EnterCriticalSection( &g_MPClient.m_csPlayerContext );
                    pPlayerInfo = g_MPClient.GetPlayerStruct(pDestroyPlayerMsg->dpnidPlayer);
                    LeaveCriticalSection( &g_MPClient.m_csPlayerContext );

                    if(pPlayerInfo == NULL)
                    {	// The player who sent this may have gone away before this 
                        // message was handled, so just ignore it
                        break;
                    }
            
                    // Release the player struct
                    EnterCriticalSection( &g_MPClient.m_csPlayerContext );
                    g_MPClient.removePlayer(pPlayerInfo);  // Release player and cleanup if needed
                    LeaveCriticalSection( &g_MPClient.m_csPlayerContext );
                    break;
                };

                case GAME_MSGID_WAVE:
                {
                    // The host is telling us that someone waved to this client
 /*                   APP_PLAYER_INFO* pPlayerInfo = NULL;
                    GAMEMSG_WAVE* pWaveMsg;
                    pWaveMsg = (GAMEMSG_WAVE*)pReceiveMsg->pReceiveData;

                    // Ignore wave messages set by the local player
                    if( pWaveMsg->dpnidPlayer == g_dpnidLocalPlayer )
                        break;

                    // Get the player struct accosicated with this DPNID
                    PLAYER_LOCK(); // enter player struct CS
                    hr = GetPlayerStruct( pWaveMsg->dpnidPlayer, &pPlayerInfo );
                    PLAYER_ADDREF( pPlayerInfo ); // addref player, since we are using it now
                    PLAYER_UNLOCK(); // leave player struct CS

                    if( FAILED(hr) || pPlayerInfo == NULL )
                    {
                        // The player who sent this may have gone away before this 
                        // message was handled, so just ignore it
                        break;
                    }

                    // This message is sent when a player has waved to us, so 
                    // post a message to the dialog thread to update the UI.  
                    // This keeps the DirectPlay threads from blocking, and also
                    // serializes the recieves since DirectPlayMessageHandler can
                    // be called simultaneously from a pool of DirectPlay threads.
                    PostMessage( g_hDlg, WM_APP_DISPLAY_WAVE, pPlayerInfo->dpnidPlayer, 0 );*/
                    break;
                };
            }

            break;
        }
    }

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: SessionsDlgNoteEnumResponse()
// Desc: Stores them in the linked list, m_DPHostEnumHead.  This is
//       called from the DirectPlay message handler so it could be
//       called simultaneously from multiple threads.
//-----------------------------------------------------------------------------
HRESULT MPClient::SessionsDlgNoteEnumResponse( PDPNMSG_ENUM_HOSTS_RESPONSE pEnumHostsResponseMsg )
{
    HRESULT hr = S_OK;
    BOOL    bFound;

    // This function is called from the DirectPlay message handler so it could be
    // called simultaneously from multiple threads, so enter a critical section
    // to assure that it we don't get race conditions.  Locking the entire
    // function is crude, and could be more optimal but is effective for this
    // simple sample
    EnterCriticalSection( &m_csHostEnum );

    DPHostEnumInfo* pDPHostEnum          = m_DPHostEnumHead.pNext;
    DPHostEnumInfo* pDPHostEnumNext      = NULL;
    const DPN_APPLICATION_DESC* pResponseMsgAppDesc =
                            pEnumHostsResponseMsg->pApplicationDescription;

    // Look for a matching session instance GUID.
    bFound = FALSE;
    while ( pDPHostEnum != &m_DPHostEnumHead )
    {
        if( pResponseMsgAppDesc->guidInstance == pDPHostEnum->pAppDesc->guidInstance )
        {
            bFound = TRUE;
            break;
        }

        pDPHostEnumNext = pDPHostEnum;
        pDPHostEnum = pDPHostEnum->pNext;
    }

    if( !bFound )
    {
        m_bEnumListChanged = TRUE;

        // If there's no match, then look for invalid session and use it
        pDPHostEnum = m_DPHostEnumHead.pNext;
        while ( pDPHostEnum != &m_DPHostEnumHead )
        {
            if( !pDPHostEnum->bValid )
                break;

            pDPHostEnum = pDPHostEnum->pNext;
        }

        // If no invalid sessions are found then make a new one
        if( pDPHostEnum == &m_DPHostEnumHead )
        {
            // Found a new session, so create a new node
            pDPHostEnum = new DPHostEnumInfo;
            if( NULL == pDPHostEnum )
            {
                hr = E_OUTOFMEMORY;
                goto LCleanup;
            }

            ZeroMemory( pDPHostEnum, sizeof(DPHostEnumInfo) );

            // Add pDPHostEnum to the circular linked list, m_DPHostEnumHead
            pDPHostEnum->pNext = m_DPHostEnumHead.pNext;
            m_DPHostEnumHead.pNext = pDPHostEnum;
        }
    }

    // Update the pDPHostEnum with new information
    TCHAR strName[MAX_PATH];
    if( pResponseMsgAppDesc->pwszSessionName )
    {
        DXUtil_ConvertWideStringToGeneric( strName, pResponseMsgAppDesc->pwszSessionName );
    }

    // Cleanup any old enum
    if( pDPHostEnum->pAppDesc )
    {
        SAFE_DELETE_ARRAY( pDPHostEnum->pAppDesc->pwszSessionName );
        SAFE_DELETE_ARRAY( pDPHostEnum->pAppDesc );
    }
    SAFE_RELEASE( pDPHostEnum->pHostAddr );
    SAFE_RELEASE( pDPHostEnum->pDeviceAddr );

    //
    // Duplicate pEnumHostsResponseMsg->pAddressSender in pDPHostEnum->pHostAddr.
    // Duplicate pEnumHostsResponseMsg->pAddressDevice in pDPHostEnum->pDeviceAddr.
    //
    if( FAILED( hr = pEnumHostsResponseMsg->pAddressSender->Duplicate( &pDPHostEnum->pHostAddr ) ) )
    {
        DXTRACE_ERR( TEXT("Duplicate"), hr );
        goto LCleanup;
    }

    if( FAILED( hr = pEnumHostsResponseMsg->pAddressDevice->Duplicate( &pDPHostEnum->pDeviceAddr ) ) )
    {
        DXTRACE_ERR( TEXT("Duplicate"), hr );
        goto LCleanup;
    }

    // Deep copy the DPN_APPLICATION_DESC from
    pDPHostEnum->pAppDesc = new DPN_APPLICATION_DESC;
    ZeroMemory( pDPHostEnum->pAppDesc, sizeof(DPN_APPLICATION_DESC) );
    memcpy( pDPHostEnum->pAppDesc, pResponseMsgAppDesc, sizeof(DPN_APPLICATION_DESC) );
    if( pResponseMsgAppDesc->pwszSessionName )
    {
        pDPHostEnum->pAppDesc->pwszSessionName = new WCHAR[ wcslen(pResponseMsgAppDesc->pwszSessionName)+1 ];
        wcscpy( pDPHostEnum->pAppDesc->pwszSessionName,
                pResponseMsgAppDesc->pwszSessionName );
    }

    // Update the time this was done, so that we can expire this host
    // if it doesn't refresh w/in a certain amount of time
    pDPHostEnum->dwLastPollTime = timeGetTime();

    // Check to see if the current number of players changed
    TCHAR szSessionTemp[MAX_PATH];
    if( pResponseMsgAppDesc->dwMaxPlayers > 0 )
    {
        wsprintf( szSessionTemp, TEXT("%s (%d/%d) (%dms)"), strName,
                  pResponseMsgAppDesc->dwCurrentPlayers - 1,  // ignore the host player
                  pResponseMsgAppDesc->dwMaxPlayers - 1,      // ignore the host player
                  pEnumHostsResponseMsg->dwRoundTripLatencyMS );
    }
    else
    {
        wsprintf( szSessionTemp, TEXT("%s (%d) (%dms)"), strName,
                  pResponseMsgAppDesc->dwCurrentPlayers - 1,  // ignore the host player
                  pEnumHostsResponseMsg->dwRoundTripLatencyMS );
    }

    // if this node was previously invalidated, or the session name is now
    // different the session list in the dialog needs to be updated
    if( ( pDPHostEnum->bValid == FALSE ) ||
        ( _tcscmp( pDPHostEnum->szSession, szSessionTemp ) != 0 ) )
    {
        m_bEnumListChanged = TRUE;
    }
    _tcscpy( pDPHostEnum->szSession, szSessionTemp );

    // This host is now valid
    pDPHostEnum->bValid = TRUE;

LCleanup:
    LeaveCriticalSection( &m_csHostEnum );

    return hr;
}

//-----------------------------------------------------------------------------
// Name: SessionsDlgEnumListCleanup()
// Desc: Deletes the linked list, g_DPHostEnumInfoHead
//-----------------------------------------------------------------------------
VOID MPClient::SessionsDlgEnumListCleanup()
{
    DPHostEnumInfo* pDPHostEnum = m_DPHostEnumHead.pNext;
    DPHostEnumInfo* pDPHostEnumDelete;

    while ( pDPHostEnum != &m_DPHostEnumHead )
    {
        pDPHostEnumDelete = pDPHostEnum;
        pDPHostEnum = pDPHostEnum->pNext;

        if( pDPHostEnumDelete->pAppDesc )
        {
            SAFE_DELETE_ARRAY( pDPHostEnumDelete->pAppDesc->pwszSessionName );
            SAFE_DELETE_ARRAY( pDPHostEnumDelete->pAppDesc );
        }

        // Changed from array delete to Release
        SAFE_RELEASE( pDPHostEnumDelete->pHostAddr );
        SAFE_RELEASE( pDPHostEnumDelete->pDeviceAddr );
        SAFE_DELETE( pDPHostEnumDelete );
    }

    // Re-link the g_DPHostEnumInfoHead circular linked list
    m_DPHostEnumHead.pNext = &m_DPHostEnumHead;
}

//-----------------------------------------------------------------------------
// Name: SessionsDlgJoinGame()
// Desc: Joins the selected DirectPlay session
//-----------------------------------------------------------------------------
HRESULT MPClient::JoinSession(DPHostEnumInfo* session)
{
    HRESULT         hr;
    DPHostEnumInfo* pDPHostEnumSelected = session;

    EnterCriticalSection( &m_csHostEnum );

    if( NULL == pDPHostEnumSelected )
    {
        LeaveCriticalSection( &m_csHostEnum );
        return S_OK;
    }


    // Set the peer info
    WCHAR wszPeerName[MAX_PATH];
    DXUtil_ConvertGenericStringToWide( wszPeerName, getPlayerName());

    DPN_PLAYER_INFO dpPlayerInfo;
    ZeroMemory( &dpPlayerInfo, sizeof(DPN_PLAYER_INFO) );
    dpPlayerInfo.dwSize = sizeof(DPN_PLAYER_INFO);
    dpPlayerInfo.dwInfoFlags = DPNINFO_NAME;
    dpPlayerInfo.pwszName = wszPeerName;

    // Set the peer info, and use the DPNOP_SYNC since by default this
    // is an async call.  If it is not DPNOP_SYNC, then the peer info may not
    // be set by the time we call Connect() below.
    if( FAILED( hr = m_pDPClient->SetClientInfo( &dpPlayerInfo, NULL, NULL, DPNOP_SYNC ) ) )
        return DXTRACE_ERR( TEXT("MPClient::JoinSession::SetPeerInfo"), hr );

    ResetEvent( m_hConnectCompleteEvent );

    // Connect to an existing session. DPNCONNECT_OKTOQUERYFORADDRESSING allows
    // DirectPlay to prompt the user using a dialog box for any device address
    // or host address information that is missing
    // We also pass in copies of the app desc and host addr, since pDPHostEnumSelected
    // might be deleted from another thread that calls SessionsDlgExpireOldHostEnums().
    // This process could also be done using reference counting instead.
    hr = m_pDPClient->Connect( pDPHostEnumSelected->pAppDesc,       // the application desc
                               pDPHostEnumSelected->pHostAddr,      // address of the host of the session
                               pDPHostEnumSelected->pDeviceAddr,    // address of the local device the enum responses were received on
                               NULL, NULL,                          // DPN_SECURITY_DESC, DPN_SECURITY_CREDENTIALS
                               NULL, 0,                             // user data, user data size
                               NULL, &m_hConnectAsyncOp,            // async context, async handle,
                               DPNCONNECT_OKTOQUERYFORADDRESSING ); // flags
    if( hr != E_PENDING && FAILED(hr) )
        return DXTRACE_ERR( TEXT("MPClient::JoinSession::Connect"), hr );

    LeaveCriticalSection( &m_csHostEnum );

    // Set a timer to wait for m_hConnectCompleteEvent to be signaled.
    // This will tell us when DPN_MSGID_CONNECT_COMPLETE has been processed
    // which lets us know if the connect was successful or not.
    //SetTimer( hDlg, TIMERID_CONNECT_COMPLETE, 100, NULL );

    return S_OK;
}

			
			
			
