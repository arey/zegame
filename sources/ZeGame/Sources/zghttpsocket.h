////////////////////////////////////////////////////////////////////////////////
//  File: zghttpsocket.h													  //
//	Desc: Permet d'executer des requetes http avec le serveur web multiplayer //
//  Copyright (C) 2001 Antoine Rey    antoine.rey@free.fr					  //
//	Vers: 0.8																  //
////////////////////////////////////////////////////////////////////////////////


#ifndef ZGHTTPSocketH
#define ZGHTTPSocketH

#define IP_MAX_CHAR		40		// Nombe maximum de caracters formant une @IP (17 normalement en IPv4)

#include "httpsocket.h"

enum
{	REQUEST_MODE_NONE,
	REQUEST_MODE_SERVERINFOS,
	REQUEST_MODE_SERVERNEW,
	REQUEST_MODE_SERVER_DISCONNECT,

	REQUEST_MODE_CLIENT_CONNECT
};

enum 
{	REQUEST_SYNCHRO_NONE,
	REQUEST_SYNCHRO_SYNC,
	REQUEST_SYNCHRO_ASYNC
};

/** ZeGame HTTP Socket class based on CHTTPSocket
 */
class ZGHTTPSocket : public CHTTPSocket
{
		static const char* ZeGameServerPage;
		static char szTemp[1024];
		char szErrMessage[255];

		static const char* IniSectionNameServerInfos;	   // "ServerInfos"
		static const char* IniSectionNameServerNew;		   // "ServerNew"
		static const char* IniSectionNameServerDisconnect; // "ServerDeCnx"
		static const char* IniSectionNameClientConnexion;  // "ClientCnx"

		static const char* IniLabelNameServerIP;		// "ServerIP"
		static const char* IniLabelNameServerName;		// "ServerName"
		static const char* IniLabelNameServerVersion;	// "ServerVersion"
		static const char* IniLabelNameServerDate;		// "ServerDate"
		static const char* IniLabelNameServerCnxDate;	// "ServerCnx"

		static const char* IniLabelNameClientIP;		// "ClientIP";

		int requestMode;				// Type de requete demandee
		int requestSynchro;				// Indique si la derniere requete doit etre synchro ou non

		bool bServerInaccesible;		// Vrai lorsque le serveur n'a pas pu etre joint
		bool bServerExist;				// Vrai si un serveur existe
		char szServerIP[IP_MAX_CHAR];	// Adresse IP du serveur
		char szServerCnxDate[128];		// Date de connexion du serveur

		char szClientIP[IP_MAX_CHAR];	// Adresse IP du client

		HANDLE hOnResponse;				// Evenement envoye une fois qu'on a recu la reponse de la requete http

	public:
		void OnError();
		void OnResponse();
		
		ZGHTTPSocket();		// Constructeur par defaut

		void requestServerInfos(bool synchro=false);									// Recupere les infos sur le serveur courant
		void requestServerNew(char* serverName, char* serverIP, bool synchro=false);	// Devient le nouveau serveur
		void requestServerDisconnect(char* serverName, char *serverCnxDate, bool server=true, bool synchro=false);	// Demande de deconnexion du serveur

		void requestClientConnect(char* clientName, char* clientIP);

		inline const char* getDefaultServerPage() { return ZeGameServerPage; }
		inline bool isServerExists() { return bServerExist; }				// Renvoi vrai si un serveur existe
		inline bool isServerAccesible () { return !bServerInaccesible; }	// Renvoi vrai lorsque le serveur a pu etre joint ou lorsque l'on n'a pas encore essaye de s'y connecte
		inline const char* getServerIP() { return szServerIP; }				// Renvoi l'adresse IP du serveur
		inline const char* getClientIP() { return szClientIP; }				// Renvoi l'adresse IP du client

		inline char* getServerCnxDate() { return szServerCnxDate; }
};

#endif // ZGHTTPSocketH