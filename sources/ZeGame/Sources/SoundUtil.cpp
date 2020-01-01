////////////////////////////////////////////////////////////////////////////////
//  Unit� "SoundUtil.cpp"													  //	                                                                            //
//	Description : Librairie regroupant diverses fonctions de manipulations	  //
//				  des fichiers sonores (.wav)								  //
//                                                                            //
//  Copyright (C) 1999 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com	                  //
//  Version      : beta 0.01                                                  //
//  Creation     : 13/12/1999 		                                          //
//  Last upgrade : <none>	                                                  //
////////////////////////////////////////////////////////////////////////////////

// --------------------------------------------------------------------- Include
#pragma warning(disable : 4100)

#ifdef VISUALC
#pragma warning(disable : 4115)  
#endif

/* PROTOTYPES */
#include <windows.h>
#include <mmsystem.h>
#include "wave.h"			// fonctions windows de manipulations de .wav
#include "FileVirtual.h"	// Fichiers virtuels (dans un .pak, en m�moire ou tout simplement sur le disk)
#include "SoundUtil.h"		// Header du fichier
#include "debug.h"
#include "windowsx.h"
#include "windef.h"

#ifdef VISUALC
#pragma warning(disable : 4127 4115 4213 4100 4706)  
#endif

////////////////////////////////////////////////////////////////////////////////
// Fonctions bas�es sur wave.c de microsoft int�grant la gestion de fichiers  //
// virtuels.																  //
////////////////////////////////////////////////////////////////////////////////

//=== Convertit une signature de 4 caract�res en entier 32bits ===============//
inline int ChunktoID(char a, char b, char c, char d)
{  return  ((((int)d)<<24) | (((int)c)<<16) | (((int)b)<<8) | ((int)a) );
}
//---------------------------------------------------------------------------//

//=== Ouvre un fichier wav potentiellement inclu dans un fichier virtuel =====//
int WaveOpenFileFV(
	char *szFileName,                               // (IN)	  - nom du fichier .wav � jouer
	TFileVirtual* &fileFV,							// (OUT)  - handle du fichier .wav ouvert
	WAVEFORMATEX **ppwfxInfo,                       // (OUT)  - pointeur sur la structure WaveFormatEx
	MMCKINFO *pckInRIFF                             // (OUT)  - informations sur le wav actuellement lu
			)

{	HMMIO           hmmioIn;
	PCMWAVEFORMAT   pcmWaveFormat;  // Temp PCM structure to load in.       
	WORD            cbExtraAlloc;   // Extra bytes for waveformatex 
	int             nError;         // Return value.


	// Initialization...
	*ppwfxInfo = NULL;
	nError = 0;
	hmmioIn = NULL;

	delete fileFV; fileFV=NULL;
	// Ouvre le fichier .wav pouvant �tre inclu dans un .pak ou �tre en m�moire
	fileFV = fopenFV(szFileName);
	if (fileFV == NULL) _asm int 3;
	if (fileFV == NULL) return -1;	// Erreur � l'ouverture du fichier
	

	// Recherche de la signature 'RIFF' (Format windows de fichiers multi-m�dia)
	if (fileFV->read32() != ChunktoID('R','I','F','F')) return -2;	// Erreur de signature

	// R�cup�re la longueur des donn�es restantes � lire apr�s ce fichier
	int lgBeforeEnd = fileFV->read32();

	// Regarde si l'on est bien arriv� � la signature WAVE
	if (fileFV->read32() != ChunktoID('W','A','V','E')) return -2;

	// Lit Le Format Chunk et sort
	int chunk_type = fileFV->read32();
	int data = fileFV->read32();
	data += fileFV->tellg();
	if (chunk_type != ChunktoID('f','m','t',' ')) return -2;		// Erreur de signature
	
	// Lit les infos sur le .wav
	fileFV->read(&pcmWaveFormat, sizeof(PCMWAVEFORMAT));
	if (pcmWaveFormat.wf.wFormatTag != WAVE_FORMAT_PCM) return -3;	// .wav non au format PCM

	// On alloue la waveformatex structure
	cbExtraAlloc = 0;
	*ppwfxInfo = (WAVEFORMATEX*) GlobalAlloc(GMEM_FIXED, sizeof(WAVEFORMATEX)+cbExtraAlloc);
	// On copie les bytes de la structure pcm vers la structure waveformatex
	memcpy(*ppwfxInfo, &pcmWaveFormat, sizeof(pcmWaveFormat));
	(*ppwfxInfo)->cbSize = 0;

	// On se place au d�but des donn�es, apr�s la signature data
	fileFV->seek(data);
	if (fileFV->read32() != ChunktoID('d','a','t','a')) return -2;		// Erreur de signature
	lgBeforeEnd = fileFV->read32();
	pckInRIFF->cksize = lgBeforeEnd;
	return lgBeforeEnd;
}
//---------------------------------------------------------------------------//

//=== Lit les donn�es d'un fichier wav ======================================//
int WaveReadFileFV(
		TFileVirtual *fileFV,                   // IN   - handle du fichier wav TVirtualFile*
		UINT cbRead,                            // IN   - nb. octets � lire
		BYTE *pbDest,                           // IN   - buffer de destination (DirectSoundBuffer)
		MMCKINFO *pckIn,                        // IN.  - informations sur le wav actuellement lu
		UINT *cbActualRead                      // OUT. - nb r�el d'octets lus
		
		)
{	if (fileFV==NULL)
	{	*cbActualRead = 0;
		return 0;
	}

	// Regarde si l'on est pas arriv� au bout du fichier .wav
	UINT cbDataIn = cbRead;
	if (cbDataIn > pckIn->cksize) cbDataIn = pckIn->cksize;       
	pckIn->cksize -= cbDataIn;

	// Copie les quelques octets de data du .wav dans le soundbuffer
	fileFV->read(pbDest, cbDataIn);
	*cbActualRead = cbDataIn;
	return(0);
}
//---------------------------------------------------------------------------//

//=== Referme un fichier wav ouvert par WaveOpenReadFileFV ==================//
int WaveCloseReadFileFV(
			TFileVirtual *&fileFV,         // IN   - handle du fichier wav TVirtualFile*
			WAVEFORMATEX **ppwfxSrc        // IN   - pointeur sur la structure WaveFormatEx
			)
{	// Lib�re le buffer contenant les infos sur le wav
	if (*ppwfxSrc != NULL)
	{	GlobalFree(*ppwfxSrc);
		*ppwfxSrc = NULL;
	}
	
	// Referme le fichier .wav
	if (fileFV != NULL)
	{	delete fileFV;
		fileFV = NULL;
	}
	return(0);
}
//---------------------------------------------------------------------------//