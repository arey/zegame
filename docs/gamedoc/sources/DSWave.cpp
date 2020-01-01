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
{	// ferme le fichier .wav ouvert
	#ifdef NEW_FILE_MANAGER
	WaveCloseReadFileFV(hSoundFV, &pWaveFormat);
	#else
	WaveCloseReadFile(&hWaveFile, &pWaveFormat);			
	#endif
	TDSSoundFile::~TDSSoundFile();	// Destructeur de la classe parent
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
	{  
		#ifdef NEW_FILE_MANAGER
		WaveCloseReadFileFV(hSoundFV, &pWaveFormat);
		#else
		WaveCloseReadFile(&hWaveFile, &pWaveFormat);        
		#endif
		return false;     
	} 

    return true;
}
//----------------------------------------------------------------------------//


//=== Remplit le buffer audio avec un morceau de wav =========================//
bool TDSWave::StreamToBuffer(DWORD dwPos)
{	DWORD	dwStartOfs;    
	VOID	*lpvPtr1, *lpvPtr2;
    DWORD	dwBytes1, dwBytes2;    
	UINT    cbBytesRead;
    static DWORD	dwStopNextTime = 0xFFFF; 	
	LONG lNumToWrite;

	// dwStopNextTime est un flag indiquant si la fin du fichier a été atteint
	//   - si <> 0  : il n'y a plus de données à transférer
	//   - si == index : la play-position courante est revenue où elle était qd la fin du fichier a été joué

	// Si tout le .wav a été joué, on stop le DirectSoundBuffer
	if (dwStopNextTime == dwPos)
	{	pSoundBuffer->Stop();
		dwStopNextTime = 0xFFFF;
		bPlaying = false;
		return true;
	}

	// Il n'y a plus rien n'a chargé ds le SoundBuffer mais on le laisse joué jusqu'à la fin du .wav
	if (dwStopNextTime != 0xFFFF)
	{	return true;
	}
	
	// On détermine l'offset à partir duquel on va copier les datas
	if (dwPos == 0)        
	{	dwStartOfs = EventPos[NUMEVENTS - 1].dwOffset;   
	} else
	{   dwStartOfs = EventPos[dwPos-1].dwOffset;
	}

	
	// Calcul de la taille du buffer à copier
	lNumToWrite = (LONG) EventPos[dwPos].dwOffset - dwStartOfs;
    if (lNumToWrite < 0) lNumToWrite += desc.dwBufferBytes;


	// On va locker la portion de buffer où les datas seront écrites
	IDirectSoundBuffer_Lock(pSoundBuffer,
                     dwStartOfs,       // Offset à partir duquel commence le lock
                     lNumToWrite,      // Nb d'octets à locker
                     &lpvPtr1,         // Address of lock start
                     &dwBytes1,        // Count of bytes locked
                     &lpvPtr2,         // Address of wrap around
                     &dwBytes2,        // Count of wrap around bytes
                     0);               // Flags
	

	// Transfert du .wav dans la mémoire audio
    #ifdef NEW_FILE_MANAGER
	WaveReadFileFV(hSoundFV,            // Handle du fichier wav
                 dwBytes1,             // Nb d'octets à lire
                 (BYTE *) lpvPtr1,     // Destination   
                 &mckInfoData,         // File chunk info 
                 &cbBytesRead);        // Actual bytes read
	#else
	WaveReadFile(hWaveFile,            // Handle du fichier wav
                 dwBytes1,             // Nb d'octets à lire
                 (BYTE *) lpvPtr1,     // Destination   
                 &mckInfoData,         // File chunk info 
                 &cbBytesRead);        // Actual bytes read
	#endif // NEW_FILE_MANAGER

	// Now, on détermine si la fin du fichier wav a été atteint 
	// -> si oui, on le ferme et on écrit du silence ds le reste du segment audio
    if (cbBytesRead < dwBytes1)        
    {
        #ifdef NEW_FILE_MANAGER
		WaveCloseReadFileFV(hSoundFV, &pWaveFormat);
		#else
		WaveCloseReadFile(&hWaveFile, &pWaveFormat);
		#endif
        FillMemory((PBYTE)lpvPtr1 + cbBytesRead, 
				dwBytes1 - cbBytesRead, 
                (desc.lpwfxFormat->wBitsPerSample==8) ? 128 : 0);
        dwStopNextTime = dwPos;
    }

	// Pour finir, on dévérouille le buffer et l'on revient à la boucle de messages
    IDirectSoundBuffer_Unlock(pSoundBuffer, lpvPtr1, dwBytes1, lpvPtr2, dwBytes2);

	return true;
}
//----------------------------------------------------------------------------//



