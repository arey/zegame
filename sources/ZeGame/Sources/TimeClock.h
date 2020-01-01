////////////////////////////////////////////////////////////////////////////////
//  File		: TimeClock.h												  //
//                                                                            //
//	Description : est une librairie regroupant plusieurs classes se chargeant //
//				  de gérer le temps, le climat, les conditions météo, les     //
//				  saisons, le passage jour/nuit ...							  //
//                                                                            //
//	- TClock    : permet d'évaluer le temps écoulé ds le jeu et de synchronier//
//			      l'horloge windows avec celle du jeu.						  //
//                                                                            //
//  Copyright (C) 1998 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@ifrance.com		                  //
//  Version      : beta 0.10                                                  //
//  Creation     : 26/01/1999												  //
////////////////////////////////////////////////////////////////////////////////

#ifndef TimeClockH
#define TimeClockH

// TCLOCK //////////////////////////////////////////////////////////////////////
class TClock
{
	DWORD dwCurrentGameTime;			// Temps écoulé depuis le début du jeu (en ms)
    DWORD dwCurrentWinTime;				// Temps écoulé depuis le démarrage de windows	
    DWORD dwDT;							// Intervalle de temps entre 2 update

    DWORD dwStartClockWin32;			// Nb de ms écoulé depuis le démarrage de windows correspondant à l'horloge de début du jeu
    DWORD dwStartClockGame;				// Heure du jeu correspondant à dwStartClockWin32 (synchronisation des horloges)

  public:
  	TClock(); 				// Constructeur par défaut
    virtual ~TClock();	   	// Destructeur
    void	SynchronizeClock(DWORD currentClockGame);	// Synchronise l'horloge du jeu avec l'horloge système
    void	UpdateClock();								// MaJ de l'horloge du jeu depuis le temps écoulé
    inline DWORD GetTime();                             // Renvoie l'heure du jeu
	inline DWORD GetDT();								// Renvoie l'intervalle de temps écoulé entre 2 updates
};


// TGAMEHISTORY ////////////////////////////////////////////////////////////////
class TGameHistory
{	TClock	Clock;						// Horloge du jeu

  public:
  	TGameHistory();         			// Constructeur par défaut
    virtual ~TGameHistory();			// Destructeur
  	void Update();						// MaJ de l'horloge, ...
	void SynchronizeClockAfterPause();	// Synchronise l'horloge du jeu avec l'horloge système	

    DWORD GetTime();					// Renvoie l'heure du jeu
    inline DWORD GetDT();				// Renvoie l'intervalle de temps écoulé entre 2 updates
};

//--- Instance de la classe utilisée par le jeu ---
extern TGameHistory *GameHistory;

////// Fonctions Externes //////////////////////////////////////////////////////
void InitPack_GameHistory(bool load = false);		// Initialise l'instance du jeu
void DisablePack_GameHistory();						// Désalloue tout
void UpdatePack_GameHistory();						// MaJ de GameHistory à chaque frame



#endif
