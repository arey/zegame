////////////////////////////////////////////////////////////////////////////////
//  File: ZGMultiplayer.h													  //
//	Desc: Constantes communes aux classes reseaux serveurs et clients de ZG	  //
//  Copyright (C) 2001 Antoine Rey    antoine.rey@free.fr					  //
//	Vers: 0.2																  //
////////////////////////////////////////////////////////////////////////////////

#ifndef ZGMultiplayerH
#define ZGMultiplayerH

//----------------------------------------------------------------------- Define
#define MAX_PLAYER_NAME         64
#define WM_APP_UPDATE_STATS    (WM_APP + 0)
#define WM_APP_DISPLAY_WAVE    (WM_APP + 1)

// App specific DirectPlay messages and structures 
#define GAME_MSGID_WAVE             1
#define GAME_MSGID_CREATE_PLAYER    2		// Un nouveau joueur vient de se connecter au serveur
#define GAME_MSGID_DESTROY_PLAYER   3
#define GAME_MSGID_SET_ID           4
#define GAME_MSGID_ONLINE_PLAYER	5		// Joueurs deja online lorsque l'on vient de se connecter
//------------------------------------------------------------------------ Const
// This GUID allows DirectPlay to find other instances of the same game on
// the network.  So it must be unique for every game, and the same for 
// every instance of that game.  // {2E6A2E98-2001-5b28-1D2E-A1B2C3D4E5F6}
extern GUID g_guidZeGame;

// Change compiler pack alignment to be BYTE aligned, and pop the current value
#pragma pack( push, 1 )


//---------------------------------------------------------------------- Struct
struct GAMEMSG_GENERIC
{
    DWORD dwType;
};

struct GAMEMSG_WAVE : public GAMEMSG_GENERIC
{
    DWORD dpnidPlayer;                 // dpnid of the player created
};

struct GAMEMSG_SET_ID : public GAMEMSG_GENERIC
{
    DWORD dpnidPlayer;                 // dpnid of the player 
	DWORD dwNbPlayersOnline;		   // nombre de joueurs connectes
};

struct GAMEMSG_DESTROY_PLAYER : public GAMEMSG_GENERIC
{
    DWORD dpnidPlayer;                 // dpnid of the player destroyed
};

struct GAMEMSG_CREATE_PLAYER : public GAMEMSG_GENERIC
{
    DWORD dpnidPlayer;                      // dpnid of the player created
    TCHAR strPlayerName[MAX_PLAYER_NAME];   // name of the player created
};

struct GAMEMSG_ONLINE_PLAYER : public GAMEMSG_GENERIC
{
    DWORD dpnidPlayer;                      // dpnid of the player created
    TCHAR strPlayerName[MAX_PLAYER_NAME];   // name of the player created
};




struct APP_PLAYER_INFO
{
    LONG  lRefCount;                        // Ref count so we can cleanup when all threads are done w/ this object
    DPNID dpnidPlayer;                      // DPNID du joueur
    TCHAR strPlayerName[MAX_PLAYER_NAME];   // Nom du joueur
};

struct ZEGAME_PLAYER_INFO : public APP_PLAYER_INFO
{
};


// Pop the old pack alignment
#pragma pack( pop )

#endif // ZGMultiplayer



