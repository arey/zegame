////////////////////////////////////////////////////////////////////////////////
//  Unité  "DSWave.cpp":Permet de lire un .wav								  //				
//						Cette clase derive de la classe abstraite TDSoundFile //  
//                                                                            //  
//  Copyright (C) 1998 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com                      //
//					                                                          //
//  Version      : beta 0.01                                                  //
//  Creation     : 10/04/1999                                                 //
//  Last upgrade : 10/04/1999                                                 //
////////////////////////////////////////////////////////////////////////////////


// --------------------------------------------------------------------- include
#include "ZeNain.h"			// Fichier commun a toute les classes de ZeGame
#include "DSLib.h"			// Librairie DirectSound
#include "DSWave.h"			// Header
#include "wave.h"			// Librairie microsoft maitrisant le format .wav
#include "winuser.h"		// Lib de gestion de messages Win32
#include "FileVirtual.h"	// Fichiers virtuels
#include "SoundUtil.h"		// Librairie maison de lecture de .wav



// ------------------------------------------------------------------- variables


////////////////////////////////////////////////////////////////////////////////
//	Classe TDSWAVE																					//
////////////////////////////////////////////////////////////////////////////////

//===	Constructeur par défaut ================================================//
TDSWave::TDSWave():
	TDSSoundFile()	// Constructeur de la classe parent
{	pWaveFormat = NULL;
}
//----------------------------------------------------------------------------//


//=== Destructeur ============================================================//
TDSWave::~TDSWave()
{	// ferme le fichier sonore ouvert (si ce n'est deja fait)
	#ifdef NEW_FILE_MANAGER
	WaveCloseReadFileFV(hSoundFV, &pWaveFormat);
	#else
	WaveCloseReadFile(&hWaveFile, &pWaveFormat);			
	#endif
	
	stop();	// Arrete de jouer ce son
}
//----------------------------------------------------------------------------//

//=== Ouvre un fichier wav et se prépare à le lire ===========================//
bool TDSWave::Open(char* soundPathName, char* name)
{	HRESULT hr;

	TDSSoundFile::Open(soundPathName, name);
	
    // Close any open file and release interfaces
    #ifdef NEW_FILE_MANAGER
	WaveCloseReadFileFV(hSoundFV, &pWaveFormat);
	#else
	WaveCloseReadFile(&hWaveFile, &pWaveFormat);  // Function in Wave.c
	#endif 

	// Ouvre un fichier .wav, récupère son handle, vérifie si c'est un fichier RIFF 
	// et obtiens les infos concernant son format et le prépare à être joué
	#ifdef NEW_FILE_MANAGER
	iDataWavSize = WaveOpenFileFV(soundPathName, hSoundFV, &pWaveFormat, &mckInfoData);	// et on se place au début des données
	if (hSoundFV == NULL) _asm int 3; 
	#else
	hr = WaveOpenFile(WavePathName, &hWaveFile, &pWaveFormat, &mckInfoParent);
	if FAILED(iDataWavSize) return false;
	#endif  // NEW_FILE_MANAGER

	// Se place au début du morceau des données, lit les donnée du 'chunk' dans mckInfoData prépare le fichier à être lu en flot de données
    #ifndef NEW_FILE_MANAGER
	// On se place au début des données
	hr = WaveStartDataRead(&hWaveFile, &mckInfoData, &mckInfoParent);
	if FAILED(hr) return false;
	#endif

	// Cree un DirectSound buffer de l'ordre de 352800 octets
	hr = createSoundBuffer(pWaveFormat->nAvgBytesPerSec * 2);
	if FAILED(hr)	 
	{	closeSoundFile();
		return false;     
	}

    return true;
}
//----------------------------------------------------------------------------//


//=== Lit une partie du fichier sonore dans le buffer audio =================//
bool TDSWave::readSoundFile(DWORD dwBytes1,	// Nb d'octets à lire
					   BYTE *lpvPtr1,		// Destination data audio memory  
					   UINT *cbBytesRead)	// Actual bytes read
{	if (hSoundFV==NULL)
	{	*cbBytesRead = 0;
		return 0;
	}
	
	// Transfert du .wav dans la mémoire audio
    #ifdef NEW_FILE_MANAGER
	WaveReadFileFV(hSoundFV,           // Handle du fichier wav
                 dwBytes1,             // Nb d'octets à lire
                 (BYTE *) lpvPtr1,     // Destination   
                 &mckInfoData,         // File chunk info 
                 cbBytesRead);         // Actual bytes read
	#else
	WaveReadFile(hWaveFile,            // Handle du fichier wav
                 dwBytes1,             // Nb d'octets à lire
                 (BYTE *) lpvPtr1,     // Destination   
                 &mckInfoData,         // File chunk info 
                 cbBytesRead);         // Actual bytes read
	#endif // NEW_FILE_MANAGER
	return true;
}
//----------------------------------------------------------------------------//

//=== Ferme le fichier sonore ================================================//
bool TDSWave::closeSoundFile()
{	
	#ifdef NEW_FILE_MANAGER
	WaveCloseReadFileFV(hSoundFV, &pWaveFormat);
	#else
	WaveCloseReadFile(&hWaveFile, &pWaveFormat);
	#endif
	return true;
}			
//----------------------------------------------------------------------------//