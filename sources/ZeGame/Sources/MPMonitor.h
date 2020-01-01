////////////////////////////////////////////////////////////////////////////////
//  Unité "MPMonitor.h" : S'occupe de l'affichage des informations liees	  //
//						  a la partie multi-joueur de ZeGame				  //
//                                                                            //
//	Description :															  //
//                                                                            //
//  Copyright (C) 2001 Antoine Rey    antoine.rey@free.fr					  //
//                                    elric@ifrance.com		                  //
//  Version      : 0.1			                                              //
//  Creation     : 04/04/2001 		                                          //
//  Last upgrade : <none>                                                     //
////////////////////////////////////////////////////////////////////////////////

#ifndef MPMonitorH
#define MPMonitorH

//---------------------------------------------------------------------- Include

//----------------------------------------------------------------------- Define

//------------------------------------------------------------------------- Enum

//------------------------------------------------------------------------ Class
class MPSession;
class TDirectScreen;



////////////////////////////////////////////////////////////////////////////////
// Classe MPMONITOR															  //
class MPMonitor
{ // ---------------------------------------------------------- Attributs privés
  protected:
	  MPSession*		m_pSession;		// Pointeur sur la session multi-joueur
	  TDirectScreen*	m_pScreen;		// Pointeur sur le module d'affichage 2D

	  enum 
	  {	MONITOR_NONE,			// Pas encore affiche
		MONITOR_DISPLAYING,		// En cours d'affichage
		MONITOR_DONE			// Message deja affiche
	  };
	  
	  // Connexion
	  int		m_iClientConnection;							// Traite l'affichage de la connexion 
	  DWORD		m_dwClientConnectionDisplayTime;
	  static const DWORD	s_dwClientConnectionDisplayDelay;	// Duree d'affichage du texte
	  static const char*	s_szClientConnectionDisplayText;

	  // Nombre de joueurs deja connectes
	  int		m_iNbPlayers;									// Traite l'affichage du nombre de joueurs deja online
	  DWORD		m_dwNbPlayersDisplayTime;
	  static const DWORD	s_dwNbPlayersDisplayDelay;			// Duree d'affichage du texte
	  static const char*	s_szNbPlayersDisplayText;
	  static const char*	s_szZeroPlayerDisplayText;
	  
	  // Liste des joueurs already connectes
	  int		m_iClientAlreadyConnected;						// Traite l'affichage des noms des joueurs deja online (flag)
	  DWORD		m_dwClientAlreadyConnectedDisplayTime;			// Heure a laquelle ce module a pris la main
	  int		m_iClientDisplayingIndex;
	  static const DWORD	s_dwClientAlreadyConnectedDisplayDelay;	// Duree d'affichage du texte

	  // Joueurs ayant quittés le jeu
	  int		m_iPlayerExit;									// Traite l'affichage des noms des joueurs ayant quitté la partie (flag)
	  char		m_szPlayerExitText[512];						// Texte affiché
	  DWORD		m_dwPlayerExitDisplayTime;						// Heure a laquelle ce module a pris la main
	  static const DWORD	s_dwPlayerExitDisplayDelay;			// Duree d'affichage du texte	  

	  // Joueurs ayant rejoints la partie
	  int		m_iPlayerEnter;									// Traite l'affichage des noms des joueurs venant de joindre la partie (flag)
	  char		m_szPlayerEnterText[512];						// Texte affiché
	  DWORD		m_dwPlayerEnterDisplayTime;						// Heure a laquelle ce module a pris la main
	  static const DWORD	s_dwPlayerEnterDisplayDelay;		// Duree d'affichage du texte	  

	  static const DWORD	s_dwTextColor;	// Couleur d'affichage des textes
	  
	  
	  	
  // ------------------------------------------------------ Méthodes publiques
  public:
	
	//--- Constructeurs & Destructeurs ---
	MPMonitor();					// Constructeur par defaut
	virtual ~MPMonitor();			// Destructeur

	void Update();					// MaJ a l'ecran des infos multi-joueurs

	//--- Fonctions inline ---
	void setSession(MPSession* s) { m_pSession = s; }
	void setScreen(TDirectScreen* s) { m_pScreen = s; }
	

	// -------------------------------------------------------- Méthodes privees
  protected:
	

    
};
//========================= Fin de la classe MPMONITOR =======================//

// Instance du jeu de la classe MPMonitor
extern MPMonitor *g_pMPMonitor;

//------------------------------------------------------------ Fonctions annexes
void InitGame_MPMonitor();		// Initialise le Moniteur multi-joueurs
void CloseGame_MPMonitor();		// Referme proprement le gestionnaire d'affichage

#endif	// MPMonitorH


