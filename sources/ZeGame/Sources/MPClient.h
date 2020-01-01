////////////////////////////////////////////////////////////////////////////////
//  Unité "MPClient.h" : Client Multi-player sous DirectPlay				  //
//                                                                            //
//	Description :															  //
//                                                                            //
//  Copyright (C) 2001 Antoine Rey    antoine.rey@free.fr					  //
//                                    elric@ifrance.com		                  //
//  Version      : 0.1			                                              //
//  Creation     : 04/03/2001 		                                          //
//  Last upgrade : <none>                                                     //
////////////////////////////////////////////////////////////////////////////////

#ifndef MPClientH
#define MPClientH

//---------------------------------------------------------------------- Include
#include "Singleton.h"		// Template de toutes les classes singletons
#include <dplay8.h>			// DirectPlay8 lib
#include <dpaddr.h>			// other DirecPlay lib
#include <vector>			// Librairie STL de tableaux dynamiques
#include "ZGMultiplayer.h"	// Constantes communes a toute la partir reseau de ZeGame

//----------------------MPClient------------------------------------------------ Define
#define g_MPClient MPClient::getSingleton()
#define g_pDPClient MPClient::getSingleton().getDPClient()

//------------------------------------------------------------------------- Enum

//------------------------------------------------------------------------ Class

////////////////////////////////////////////////////////////////////////////////
// Classe MPCLIENT															  //
class MPClient : public Singleton <MPClient>
{ // ---------------------------------------------------------- Attributs privés
  protected:
	IDirectPlay8Client* m_pDPClient;                // DirectPlay server object

	DPN_APPLICATION_DESC   dpnAppDesc;
    IDirectPlay8Address*   pDP8AddressHost;
    IDirectPlay8Address*   pDP8AddressLocal;
    WCHAR*                 wszHostName;
	
	char*				   pPlayerName;				// Nom du joueur
	CHAR				   m_strClientURL[512];		// URL au format DirectPlay du Client (caractere ANSI)

	struct DPHostEnumInfo
    {
        DWORD                 dwRef;
        DPN_APPLICATION_DESC* pAppDesc;
        IDirectPlay8Address* pHostAddr;
        IDirectPlay8Address* pDeviceAddr;
        TCHAR                szSession[MAX_PATH];
        DWORD                dwLastPollTime;
        BOOL                 bValid;
        DPHostEnumInfo*      pNext;
    };

	DPHostEnumInfo          m_DPHostEnumHead;		// Link list
	CRITICAL_SECTION        m_csHostEnum;
	BOOL                    m_bEnumListChanged;

	DWORD					m_dpnidLocalPlayer;			// ID affecte au client par le Serveur
	std::vector<ZEGAME_PLAYER_INFO*> m_pPlayerInfo;		// Liste de tous les joueurs actuellement connectes ou non
	std::vector<char*>		m_pExitPlayerInfo;			// Liste des joueurs venant de quitter la partie
	std::vector<char*>		m_pEnterPlayerInfo;			// Liste des joueurs venant de joindre la partie en cours
	bool					m_bNewPlayerConnected;		// Vrai lorsqu'un nouveau joueur vient de se connecter
	bool					m_bClientConnected;			// Vrai lorsque le client est connecte au serveur

	DWORD					m_dwNbPlayersOnline;		// Nombre de joueurs connectes au demarrage
	  
  public:
    DPNHANDLE              m_hEnumAsyncOp;
	DPNHANDLE              m_hConnectAsyncOp;
	bool				   bHostServerFounded;
	HRESULT                m_hrConnectComplete;
	CRITICAL_SECTION	   m_csPlayerContext;

	static HANDLE hHostServerFounded;		// Evenement envoye lorsque le serveur a ete trouve
	static HANDLE m_hConnectCompleteEvent;	//							le client s'est connecte a la session

  // ------------------------------------------------------ Méthodes publiques
  public:
	
	//--- Constructeurs & Destructeurs ---
	MPClient();							// Constructeur par defaut
	virtual ~MPClient();				// Destructeur

	void setPlayerName(char* name);		// Permet de donner un pseudo au joueur
	
	HRESULT  InitClient();				// Initialise le Client DirectPlay
	HRESULT ConnectToServer(const char* serverIP, int port);
	HRESULT JoinSession(DPHostEnumInfo* session);
	HRESULT	 StopClient();				// Deconnecte le client de la session multi-joueur

	//HRESULT  StartClient();			// Demarre le Client DirectPlay
	//void     StopClient();			// Arrete le Client multi-joueur

	//--- Fonctions inline ---
	inline IDirectPlay8Client* getDPClient() { return m_pDPClient; }
	inline CHAR* getClientURL() { return m_strClientURL; }
	inline char* getPlayerName() { return pPlayerName; }
	inline DPHostEnumInfo* getConnectingSession() { return m_DPHostEnumHead.pNext; }
	inline DWORD getDPNIDLocalPlayer() { return m_dpnidLocalPlayer; }
	inline void setDPNIDLocalPlayer(DWORD id) { m_dpnidLocalPlayer = id; }
	inline int getActivePlayersNumber() { return m_pPlayerInfo.size(); }
	inline bool isClientConnected() { return m_bClientConnected; }
	inline void setClientConnected(bool c) { m_bClientConnected = c; }
	inline DWORD getNbPlayersOnline() { return m_dwNbPlayersOnline; }
	inline void setNbPlayersOnline(int n) { m_dwNbPlayersOnline = n; }
	inline bool getNewPlayerConnected() { return m_bNewPlayerConnected; }
	inline ZEGAME_PLAYER_INFO* getClientInfos(int i) { return m_pPlayerInfo[i]; }
	inline std::vector<char*> &getExitPlayerInfo() { return m_pExitPlayerInfo; }
	inline std::vector<char*> &getEnterPlayerInfo() { return m_pEnterPlayerInfo; }

	
	HRESULT SessionsDlgNoteEnumResponse( PDPNMSG_ENUM_HOSTS_RESPONSE pEnumHostsResponse );
	VOID    SessionsDlgEnumListCleanup();

	bool addPlayer(ZEGAME_PLAYER_INFO* player);		// Ajoute un joueur a la liste des joueurs deja connectes a la session multi-joueur
	bool removePlayer(ZEGAME_PLAYER_INFO* player);	// Supprime tel joueur de la liste des joueurs online
	ZEGAME_PLAYER_INFO* GetPlayerStruct(int dpnid);	// Renvoi une référence sur les infos de tel joueur
				

	// -------------------------------------------------------- Méthodes privees
  protected:
	

	

    
};
//=========================== Fin de la classe MPCLIENT ======================//

//------------------------------------------------------------ Fonctions annexes
// Fonction CallBack recevant tous les messages destines au client DirectPlay
HRESULT WINAPI	DirectPlayClientMessageHandler( PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer );

#endif	// MPClientH


