////////////////////////////////////////////////////////////////////////////////
//  Unité "MPSession.h" : Couche session multi-joueur de ZeGame				  //
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

#ifndef MPSessionH
#define MPSessionH

//---------------------------------------------------------------------- Include
#include "Singleton.h"		// Template de toutes les classes singletons
#include "zghttpsocket.h"	// Communique avec le serveur Web en utilisant WinSock
#include <dplay8.h>			// DirectPlay8

//----------------------------------------------------------------------- Define
#define g_MPSession MPSession::getSingleton()

//------------------------------------------------------------------------- Enum

//------------------------------------------------------------------------ Class
class MPServer;
class MPClient;
struct IDirectPlay8Address;


////////////////////////////////////////////////////////////////////////////////
// Classe MPSESSION															  //
class MPSession : public Singleton <MPSession>
{ // ---------------------------------------------------------- Attributs privés
  protected:
	  ZGHTTPSocket	m_httpSocket;		// Acces au socket HTPP via WinSocket
	  bool m_bMultiPlayerActivate;		// Vrai lorsque le mode multi-joueur est active
	  int m_iSessionFlag;				// Permet de savoir si le serveur multi-joueur a ete cree

	  bool m_bLocalNetwork;				// Reseau local sans passer par Internet (et donc par le serveur web)
	  char m_szServerIP[IP_MAX_CHAR];	// Adresse IP du serveur
	  bool m_bServer;					// Force cet ordinateur à être serveur

	  MPServer	*m_pMPServer;			// Serveur multi-joueur
	  MPClient	*m_pMPClient;			// Client ZeGame

	  IDirectPlay8Address*   pDP8AddressHost;
	  IDirectPlay8Address*   pDP8AddressLocal;
	  DPNHANDLE              m_hEnumAsyncOp;

	  static bool s_bDefaultLocalNetwork;	// Valeur par defaut de l'attribut #m_bLocalNetwork
	  static bool s_bDefaultServer;			//								   #m_bServer

  public:
	 static HANDLE hSessionStop;			// Signal arretant la session multi-joueur
	 static HANDLE hSessionServerStarted;	// Evenement envoye lorsque le serveur a demarre
	 
	 static HANDLE hSessionServerStopped;	// Signal envoye lorsque le server est arrete
	 static HANDLE hSessionStopped;			// Signal envoye lorsque la session est arretee
	  
  // ------------------------------------------------------- Attributs publiques
  public:
	  static int SERVER_SESSION;		// flag a passer a stop session pour arreter le server
	  static int CLIENT_SESSION;		//	 "		"		"		"			"	 le client
	  static int DUAL_SESSION;			// or des 2 flags precedents

  // ------------------------------------------------------ Méthodes publiques
  public:
	
	//--- Constructeurs & Destructeurs ---
	MPSession(int flag=DUAL_SESSION);			// Constructeur par defaut
	virtual ~MPSession();						// Destructeur

	void InitSession();							// Initialise la session multi-joueur en creant le cote client et eventuellement le serveur
	void StopSession(int flag);					// Arrete la session multi-joueur en cours (serveur et/ou client)

	//--- Fonctions inline ---
	inline MPClient* getMPClient() { return m_pMPClient;	}
	inline bool isMultiPlayerActive() { return m_bMultiPlayerActivate; }	// Renvoi vrai lorsque le mode multi-joueur est actif
	inline void setLocalNetwork(bool l) { m_bLocalNetwork = l; }
	inline bool getLocalNetwork() { return m_bLocalNetwork; }
	inline char* getServerIP() { return m_szServerIP; }
	inline void setServerIP(const char* ip) { strcpy(m_szServerIP, ip); }

	//--- Accesseurs sur le singleton ---
	static MPSession& getSingleton() { return *ms_Singleton; }		// Recupere une reference sur le singleton
	static MPSession* getSingletonPtr() { return ms_Singleton;} 	// Recupere un pointeur sur le singleton


	static int LaunchSession(MPSession *session);   // Lance la session multi-joueur
	

	// -------------------------------------------------------- Méthodes privees
  protected:
	bool createMPServer();			// Cree le serveur multi-joueur
	bool createMPClient();			// Cree le client du cote du joueur
	
	bool connectServer();

	

    
};
//=========================== Fin de la classe MPSESSION ======================//

//------------------------------------------------------------ Fonctions annexes
void InitGame_MPSession(int flag=MPSession::DUAL_SESSION);	// Initialise le serveur multi-joueur
void CloseGame_MPSession();									// Referme proprement le serveur

#endif	// MPSessionH


