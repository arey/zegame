////////////////////////////////////////////////////////////////////////////////
//  Unit�  "DSWave.cpp":Permet de lire un .wav								  //				
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

//===	Constructeur par d�faut ================================================//
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

//=== Ouvre un fichier wav et se pr�pare � le lire ===========================//
bool TDSWave::Open(char* soundPathName, char* name)
{	HRESULT hr;

	TDSSoundFile::Open(soundPathName, name);
	
    // Close any open file and release interfaces
    #ifdef NEW_FILE_MANAGER
	WaveCloseReadFileFV(hSoundFV, &pWaveFormat);
	#else
	WaveCloseReadFile(&hWaveFile, &pWaveFormat);  // Function in Wave.c
	#endif 

	// Ouvre un fichier .wav, r�cup�re son handle, v�rifie si c'est un fichier RIFF 
	// et obtiens les infos concernant son format et le pr�pare � �tre jou�
	#ifdef NEW_FILE_MANAGER
	iDataWavSize = WaveOpenFileFV(soundPathName, hSoundFV, &pWaveFormat, &mckInfoData);	// et on se place au d�but des donn�es
	#else
	hr = WaveOpenFile(WavePathName, &hWaveFile, &pWaveFormat, &mckInfoParent);
	if FAILED(iDataWavSize) return false;
	#endif  // NEW_FILE_MANAGER

	// Se place au d�but du morceau des donn�es, lit les donn�e du 'chunk' dans mckInfoData pr�pare le fichier � �tre lu en flot de donn�es
    #ifndef NEW_FILE_MANAGER
	// On se place au d�but des donn�es
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

	// dwStopNextTime est un flag indiquant si la fin du fichier a �t� atteint
	//   - si <> 0  : il n'y a plus de donn�es � transf�rer
	//   - si == index : la play-position courante est revenue o� elle �tait qd la fin du fichier a �t� jou�

	// Si tout le .wav a �t� jou�, on stop le DirectSoundBuffer
	if (dwStopNextTime == dwPos)
	{	pSoundBuffer->Stop();
		dwStopNextTime = 0xFFFF;
		bPlaying = false;
		return true;
	}

	// Il n'y a plus rien n'a charg� ds le SoundBuffer mais on le laisse jou� jusqu'� la fin du .wav
	if (dwStopNextTime != 0xFFFF)
	{	return true;
	}
	
	// On d�termine l'offset � partir duquel on va copier les datas
	if (dwPos == 0)        
	{	dwStartOfs = EventPos[NUMEVENTS - 1].dwOffset;   
	} else
	{   dwStartOfs = EventPos[dwPos-1].dwOffset;
	}

	
	// Calcul de la taille du buffer � copier
	lNumToWrite = (LONG) EventPos[dwPos].dwOffset - dwStartOfs;
    if (lNumToWrite < 0) lNumToWrite += desc.dwBufferBytes;


	// On va locker la portion de buffer o� les datas seront �crites
	IDirectSoundBuffer_Lock(pSoundBuffer,
                     dwStartOfs,       // Offset � partir duquel commence le lock
                     lNumToWrite,      // Nb d'octets � locker
                     &lpvPtr1,         // Address of lock start
                     &dwBytes1,        // Count of bytes locked
                     &lpvPtr2,         // Address of wrap around
                     &dwBytes2,        // Count of wrap around bytes
                     0);               // Flags
	

	// Transfert du .wav dans la m�moire audio
    #ifdef NEW_FILE_MANAGER
	WaveReadFileFV(hSoundFV,            // Handle du fichier wav
                 dwBytes1,             // Nb d'octets � lire
                 (BYTE *) lpvPtr1,     // Destination   
                 &mckInfoData,         // File chunk info 
                 &cbBytesRead);        // Actual bytes read
	#else
	WaveReadFile(hWaveFile,            // Handle du fichier wav
                 dwBytes1,             // Nb d'octets � lire
                 (BYTE *) lpvPtr1,     // Destination   
                 &mckInfoData,         // File chunk info 
                 &cbBytesRead);        // Actual bytes read
	#endif // NEW_FILE_MANAGER

	// Now, on d�termine si la fin du fichier wav a �t� atteint 
	// -> si oui, on le ferme et on �crit du silence ds le reste du segment audio
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

	// Pour finir, on d�v�rouille le buffer et l'on revient � la boucle de messages
    IDirectSoundBuffer_Unlock(pSoundBuffer, lpvPtr1, dwBytes1, lpvPtr2, dwBytes2);

	return true;
}
//----------------------------------------------------------------------------//



