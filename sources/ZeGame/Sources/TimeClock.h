////////////////////////////////////////////////////////////////////////////////
//  File		: TimeClock.h												  //
//                                                                            //
//	Description : est une librairie regroupant plusieurs classes se chargeant //
//				  de g�rer le temps, le climat, les conditions m�t�o, les     //
//				  saisons, le passage jour/nuit ...							  //
//                                                                            //
//	- TClock    : permet d'�valuer le temps �coul� ds le jeu et de synchronier//
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
	DWORD dwCurrentGameTime;			// Temps �coul� depuis le d�but du jeu (en ms)
    DWORD dwCurrentWinTime;				// Temps �coul� depuis le d�marrage de windows	
    DWORD dwDT;							// Intervalle de temps entre 2 update

    DWORD dwStartClockWin32;			// Nb de ms �coul� depuis le d�marrage de windows correspondant � l'horloge de d�but du jeu
    DWORD dwStartClockGame;				// Heure du jeu correspondant � dwStartClockWin32 (synchronisation des horloges)

  public:
  	TClock(); 				// Constructeur par d�faut
    virtual ~TClock();	   	// Destructeur
    void	SynchronizeClock(DWORD currentClockGame);	// Synchronise l'horloge du jeu avec l'horloge syst�me
    void	UpdateClock();								// MaJ de l'horloge du jeu depuis le temps �coul�
    inline DWORD GetTime();                             // Renvoie l'heure du jeu
	inline DWORD GetDT();								// Renvoie l'intervalle de temps �coul� entre 2 updates
};


// TGAMEHISTORY ////////////////////////////////////////////////////////////////
class TGameHistory
{	TClock	Clock;						// Horloge du jeu

  public:
  	TGameHistory();         			// Constructeur par d�faut
    virtual ~TGameHistory();			// Destructeur
  	void Update();						// MaJ de l'horloge, ...
	void SynchronizeClockAfterPause();	// Synchronise l'horloge du jeu avec l'horloge syst�me	

    DWORD GetTime();					// Renvoie l'heure du jeu
    inline DWORD GetDT();				// Renvoie l'intervalle de temps �coul� entre 2 updates
};

//--- Instance de la classe utilis�e par le jeu ---
extern TGameHistory *GameHistory;

////// Fonctions Externes //////////////////////////////////////////////////////
void InitPack_GameHistory(bool load = false);		// Initialise l'instance du jeu
void DisablePack_GameHistory();						// D�salloue tout
void UpdatePack_GameHistory();						// MaJ de GameHistory � chaque frame



#endif
