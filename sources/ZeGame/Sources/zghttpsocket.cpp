////////////////////////////////////////////////////////////////////////////////
//  File: zghttpsocket.cpp													  //
//	Desc: Permet d'executer des requetes http avec le serveur web multiplayer //
//  Copyright (C) 2001 Antoine Rey    antoine.rey@free.fr					  //
//	Vers: 0.8																  //
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "zghttpsocket.h"
#include "INILib.h"

//const char* ZGHTTPSocket::ZeGameServerPage = "http://antoine.rey.free.fr/php3/network/connect.php3";
const char* ZGHTTPSocket::ZeGameServerPage = "http://antoine.rey.free.fr/php3/network/connectmysql.php3";

//--- Sections ---
const char* ZGHTTPSocket::IniSectionNameServerInfos = "ServerInfos";
const char* ZGHTTPSocket::IniSectionNameServerNew = "ServerNew";
const char* ZGHTTPSocket::IniSectionNameServerDisconnect = "ServerDeCnx";
const char* ZGHTTPSocket::IniSectionNameClientConnexion = "ClientCnx";
//--- Server labels ----
const char* ZGHTTPSocket::IniLabelNameServerIP = "ServerIP";
const char* ZGHTTPSocket::IniLabelNameServerName = "ServerName";
const char* ZGHTTPSocket::IniLabelNameServerVersion = "ServerVersion";
const char* ZGHTTPSocket::IniLabelNameServerDate = "ServerDate";
const char* ZGHTTPSocket::IniLabelNameServerCnxDate = "ServerCnx";
// Client labels ----
const char* ZGHTTPSocket::IniLabelNameClientIP = "ClientIP";


char ZGHTTPSocket::szTemp[1024];


// Constructeur par defaut
ZGHTTPSocket::ZGHTTPSocket()
{	requestMode = REQUEST_MODE_NONE;
	requestSynchro = REQUEST_SYNCHRO_NONE;
	bServerInaccesible = false;
	bServerExist = false;
	szServerIP[0] = '\0';
	szServerCnxDate[0] = '\0';
	szClientIP[0] = '\0';
	hOnResponse = CreateEvent(NULL, FALSE, FALSE, NULL);
}

//error trigger
void ZGHTTPSocket::OnError()
{
	switch (requestMode)
	{	// Serveur innacessible lors de la connexion, on distingue 2 cas possibles :
		//	1)  Le poste et n'a donc pas acces au serveur (mal configure, non connecte au net ...)
		//	2)  Le serveur web est HS et ne repond pas, ou alors il a change d'adresse
		//      => dans les 2 cas, le mode reseau est annule
		case REQUEST_MODE_SERVERINFOS :
			switch (m_nErrCode)
			{	// Erreur au niveau de l'URL (null, syntaxe ou trop long)
				case ERR_PROTOPARSE :
				case ERR_URLNOTHING :
				case ERR_URLTOOLONG :
				case ERR_BADHOST :
				case ERR_BADPORT :
				// Echec de la connexion au serveur
				case ERR_PROXYUNKNOWN :
				case ERR_WSAINTERNAL :
				default:
					// => on s'arrete la, tant pis .... le serveur est injoignable et on n'y peut rien ...
					bServerInaccesible = true;
			}
	}
	
//	wsprintf(szErrMessage,"Error: %d, %d, %s",m_nErrCode,m_nExtErrCode,m_nErrInfo);
	CHTTPSocket::OnError();
};


//response trigger
void ZGHTTPSocket::OnResponse()
{	TINILib *ini = NULL;
//	int length = strlen((char*) m_szResponse);
	switch(requestMode)
	{	case REQUEST_MODE_SERVERINFOS :
			// Place la reponse dans un fichier ini
			ini = new TINILib((char*) m_szResponse, (int) m_ulResponseSize+1);
			/*[ServerInfos]
			ServerIP=134.214.165.46
			ServerName=server
			ServerVersion=1
			ServerDate=98362165*/
			ini->GetStringProfile((char*) IniSectionNameServerInfos, (char*) IniLabelNameServerIP, (char*) &szTemp, "???.???.???.???");
			bServerExist = (strcmp(szTemp, "???.???.???.???") != 0);
			strcpy(szServerIP, szTemp);
			// Un serveur existe deja (parait-il), on recupere des infos dessus
			if (bServerExist)
			{	ini->GetStringProfile((char*) IniSectionNameServerInfos, (char*) IniLabelNameServerName, (char*) &szTemp, "????");
				ini->GetStringProfile((char*) IniSectionNameServerInfos, (char*) IniLabelNameServerVersion, (char*) &szTemp, "?.?");
				ini->GetStringProfile((char*) IniSectionNameServerInfos, (char*) IniLabelNameServerDate, (char*) &szTemp, "??/??/????");
				strcpy(szServerCnxDate, szTemp);
			}
			delete ini; ini = NULL;
			break;

		case REQUEST_MODE_SERVERNEW:
			ini = new TINILib((char*) m_szResponse, (int) m_ulResponseSize+1);
			// On recupere la date precise de connexion du serveur
			ini->GetStringProfile((char*) IniSectionNameServerNew, (char*) "ServerStatus", (char*) &szTemp, "KO");
			ini->GetStringProfile((char*) IniSectionNameServerNew, (char*) IniLabelNameServerCnxDate, (char*) &szTemp, "??/??/????");
			strcpy(szServerCnxDate, szTemp);
			delete ini; ini = NULL;
			break;

		case REQUEST_MODE_SERVER_DISCONNECT :
			ini = new TINILib((char*) m_szResponse, (int) m_ulResponseSize+1);
			ini->GetStringProfile((char*) IniSectionNameServerNew, (char*) "ServerDeCnx", (char*) &szTemp, "??/??/????");
			strcpy(szServerCnxDate, &((char*) m_szResponse)[150]);
			delete ini; ini = NULL;
			break;
		
		// Connexion du client au serveur multi-joueur
		case REQUEST_MODE_CLIENT_CONNECT:
			/*	echo "[ClientCnx]\n";
				echo "ClientStatus=OK\n";
				echo "ClientIP=$ip\n";
				echo "ClientCnx=$dateCnx\n";	*/
				ini = new TINILib((char*) m_szResponse, (int) m_ulResponseSize+1);
				ini->GetStringProfile((char*) IniSectionNameClientConnexion, (char*) "ClientStatus", (char*) &szTemp, "KO");
				ini->GetStringProfile((char*) IniSectionNameClientConnexion, (char*) IniLabelNameClientIP, (char*) &szTemp, "???.???.???.???");
				strcpy(szClientIP, szTemp);
				delete ini; ini = NULL;
			break;

		case REQUEST_MODE_NONE :
		default: ;
	}
  
	// printf("%s\r\n",(char *)m_szResponse);
	CHTTPSocket::OnResponse();

	// Resultat de la requete arrive
	SetEvent(hOnResponse);
}

// Demande au site web, via php3, de lui renvoyer les informations sur le serveur
// multijoueur actif de ZeGame
void ZGHTTPSocket::requestServerInfos(bool synchro)
{	
	requestMode = REQUEST_MODE_SERVERINFOS;
	m_nServerPort = 80;
	strcpy(szTemp, getDefaultServerPage());
	strcat(szTemp, "?serverinfos=1");
	
	requestSynchro = (synchro) ? REQUEST_SYNCHRO_SYNC : REQUEST_SYNCHRO_ASYNC;
	Request(szTemp);
	if (synchro)
	{	WaitForSingleObject(hOnResponse, INFINITE);		
	}
	requestSynchro = REQUEST_SYNCHRO_NONE;	
}

// L'ordi dont l'IP et le Nom sont passes en parametres devient le nouveau
// serveur multijouer du jeu
void ZGHTTPSocket::requestServerNew(char* serverName, char* serverIP = NULL, bool synchro)
{	requestMode = REQUEST_MODE_SERVERNEW;
	m_nServerPort = 80;
	strcpy(szTemp, getDefaultServerPage());
	strcat(szTemp, "?serverconnect=1");
	if (serverIP!=NULL)
	{	strcat(szTemp, "&ServerIP=");
		strcat(szTemp, serverIP);
	}
	strcat(szTemp, "&ServerName=");
	strcat(szTemp, serverName);
	requestSynchro = (synchro) ? REQUEST_SYNCHRO_SYNC : REQUEST_SYNCHRO_ASYNC;
	Request(szTemp);
	if (synchro)
	{	WaitForSingleObject(hOnResponse, INFINITE);		
	}
	requestSynchro = REQUEST_SYNCHRO_NONE;	
}

// Le Serveur dont le Nom DirectPlay sont passes en parametres souhaite se deconnecter
void ZGHTTPSocket::requestServerDisconnect(char* serverName, char *serverCnxDate, bool server, bool synchro)
{	requestMode = REQUEST_MODE_SERVER_DISCONNECT;
	m_nServerPort = 80;
	strcpy(szTemp, getDefaultServerPage());
	if (server)
	{	strcat(szTemp, "?serverdisconnect=2");
	} else
	{	strcat(szTemp, "?serverdisconnect=1");
	}
	strcat(szTemp, "&ServerName=");
	strcat(szTemp, serverName);
	strcat(szTemp, "&dateCnx=");
	strcat(szTemp, serverCnxDate);
	
	requestSynchro = (synchro) ? REQUEST_SYNCHRO_SYNC : REQUEST_SYNCHRO_ASYNC;
	Request(szTemp);
	if (synchro)
	{	WaitForSingleObject(hOnResponse, INFINITE);		
	}
	requestSynchro = REQUEST_SYNCHRO_NONE;	
}

// Le client se connecte au serveur multi-joueur
void ZGHTTPSocket::requestClientConnect(char* clientName, char* clientIP)
{	requestMode = REQUEST_MODE_CLIENT_CONNECT;
	m_nServerPort = 80;
	strcpy(szTemp, getDefaultServerPage());
	strcat(szTemp, "?clientconnect=1");
	strcat(szTemp, "&ClientName=");
	strcat(szTemp, clientName);
	strcat(szTemp, "&ClientIP=");
	strcat(szTemp, clientIP);
	
	Request(szTemp);
}