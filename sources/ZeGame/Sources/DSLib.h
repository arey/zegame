////////////////////////////////////////////////////////////////////////////////
//  Unité  "DSLib.h"  : Librarie regroupant diverses fonctions de gestion	  //
//								sonore et musicale sous DirectSound.		  //				
//                                                                            //  
//  Copyright (C) 1998 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com                      //
//					                                                          //
//  Version      : beta 0.01                                                  //
//  Creation     : 10/04/1999                                                 //
//  Last upgrade : 10/04/1999                                                 //
////////////////////////////////////////////////////////////////////////////////

#ifndef DSLibH
#define DSLibH

#ifdef MICROSOFT
#pragma warning(disable : 4100 4663 4018 4786 4201)  
#endif

// --------------------------------------------------------------------- Include
#include <objbase.h>		
#pragma warning(disable : 4201)  
#include "mmsystem.h" 		// Windows Multimedia API's
#include "dsound.h"			// Librairie SDK de DirectSound
#include <vector>			// librairie STL de gestion de tableaux dynamiques

// ---------------------------------------------------------------------- Define
#define NUMEVENTS	2		// Nb d'avertissent par buffer : 2 évènements par lecture du buffer -> un au début & un au milieu


// ----------------------------------------------------------------------- Class
class TFileVirtual;


// TDIRECTSOUND ////////////////////////////////////////////////////////////////
// Classe initialisation l'environnement sonore								  //
class TDirectSound  
{
  // ------------------------------------------------------------------ Méthodes
  public:
	// Constructeurs & Destructeurs
	TDirectSound();
	virtual ~TDirectSound();

	bool InitDSound(HWND hWnd);					// Initialise tout
	void SetPause(bool p = true);				// Fait une ch'tite pause
	bool IsPaused();							// Vrai si l'on est en pause

  // ----------------------------------------------------------------- Attributs
  public:
	LPDIRECTSOUND			pDS;				// Objet DirectSound d'architecture COM 
	LPDIRECTSOUNDBUFFER		pPrimaryBuffer;		// Buffer sonore principal
	bool					bPause;				// Indique si l'on doit jouer la zik
};

// Seul objet DirectSound du jeu
extern TDirectSound	*DSGame;

// TDSSOUNDFILE ////////////////////////////////////////////////////////////////
//	Classe abstraite s'occupant de jouer des fichiers sonores .wav ou .mp3 	  //
//	quel que soit leur nature
class TDSSoundFile
{ public:	
	TFileVirtual				*hSoundFV;				// 2nd Handle sur le fichier musical
  protected:
	WAVEFORMATEX                *pWaveFormat;			// Infos sur le format du fichier
	LPDIRECTSOUNDBUFFER			pSoundBuffer;			// Secondary SoundBuffer du fichier .wav lu
	DSBPOSITIONNOTIFY           EventPos[NUMEVENTS];	// Position des évènements ds le buffer audio
	HANDLE                      hEvent[NUMEVENTS];		// Handle vers les différents évènements
	LPDIRECTSOUNDNOTIFY         pSoundNotify;			// Mécanisme de gestion des évènements
	DSBUFFERDESC				desc;					// Description du Secondary Sound Buffer alloué
	
	char						soundName[96];			// Nom du fichier .wav ou .mp3
	char						soundPathName[96];		// Chemin du fichier .wav ou .mp3
	bool						bPlaying;				// Vrai si notre fichier est entrain d'être joué ou non
	DWORD						dwStopNextTime;			// Lorsque cette variable est a 0xFFFF, on pourra arreter de jouer ce son la prochaine fois que l'on passe ds DSWinMain
	bool						bPaused;				// Vrai quand le SoundBuffer est en pause

	int							iEndSoundSynchro;		// indique si l'on doit prévenir par mutex quand le son est fini

  // ----------------------------------------------------------------- Attributs	
  public:
	TDSSoundFile();								// Constructeur par défaut
	virtual ~TDSSoundFile();					// Destructeur
	
	// Lit une partie du fichier sonore dans le buffer audio
	virtual bool readSoundFile(DWORD dwBytes1,			// Nb d'octets à lire
							   BYTE *lpvPtr1,			// Destination data audio memory  
							   UINT *cbBytesRead) = 0;	// Actual bytes read

	virtual bool closeSoundFile() = 0;			// Ferme le fichier sonore 			
	

	virtual bool Open(char* soundPathName,  char* name = NULL);	// Ouvre un fichier sonore et se prépare à le lire
	virtual void SetPause(bool pause = true);	// Fait une p'tite pause	
	virtual void Play();						// Joue le .wav
	
	void DSWinMain();							// Méthode à inclure dans WinMain afin de gérer les messages
	bool isPlaying();							// Vrai si l'on est en-train de jouer le .wav
	void Beginning();							// Revient au début de la musique
	bool StreamToBuffer(DWORD dwPos);			// Fonction remplissant le buffer audio à partir de la position dwPos
	bool setVolume(int volume);					// Regle le volume du son : volume est exprime en decibels (0=son original et -10000=silence (sachant que -100=50% volume de depart))
	bool increaseVolume(int dtVol);				// Augmente ou diminue le volume courant de dtVol/100 de dB
	int	getVolume();							// Renvoie le volume sonore courant du directsound buffer (65535 si ca a plante)			
	bool setPan(int pan);						// Volume relatif entre l'enceinte gauche et droite (-100dB a +100dB)
	int getPan();								// Renvoie le volume relatif entre les 2 enceintes

	//--- inline functions ---
	inline bool isPaused() { return bPaused; }	// Renvoie vrai quand le buffer audio est en pause
	inline void setEndSoundSynchro(int mutex) { iEndSoundSynchro = mutex; }
  
  protected:
	HRESULT createSoundBuffer(int length);		// Cree un DirectSound buffer de @length octets
	void stop();								// Arrete de jouer ce son

};



// TDSMUZIKLIB ////////////////////////////////////////////////////////////////
//	Librairie des musiques & des bruitages du jeu                           // 
class TDSMuzikLib
{	std::vector<TDSSoundFile*> vSoundMuzik;	// Liste des musiques du jeu
	int	iCurrentMuzik;						// Index de la musique jouée actuellement
	bool bMusikGame;						// Faut-il jouer la musique

  public:
	TDSMuzikLib(bool musik=true);			// Constructeur par défaut
	virtual ~TDSMuzikLib();					// Destructeur

	void MuzikManage();						// S'occupe de mettre à jour le SoundBuffer
	void SetPause(bool pause = true);		// Stoppe toutes les musiques
	static TDSSoundFile* AllocateSoundFile(char* filename); // Renvoi une instance d'un fichier sonore suivant son extension
};
////////////////////////////// Fin de TDSMUZIKLIB ////////////////////////////

// Seul instance du jeu de la classe TDSMuzikLib
extern TDSMuzikLib *DSMuzikLib;


// --------------------------------------------------------- Fonctions externes
void InitGame_DirectSound(HWND hWnd);	// Initialize tout ce qui se rapporte au son
void CloseGame_DirectSound();			// Initialize tout ce qui se rapporte au son

#endif
