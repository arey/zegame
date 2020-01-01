////////////////////////////////////////////////////////////////////////////////
//  Unité  "DSMp3.cpp" : Permet de lire des .mp3							  //				
//					     Cette clase derive de la classe abstraite TDSoundFile//  
//                                                                            // 
//	Pour decoder du mpeg layer 3, j'utilise la tres fameuse mpg123 mpglib	  //
//	(www.mpg123.org) qui a donne lieu a des lecteurs mp3 tel kjoofol.		  //	
//																			  //
//  Copyright (C) 2000 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com                      //
//					                                                          //
//  Version      : beta 0.01                                                  //
//  Creation     : 22/07/1999                                                 //
//  Last upgrade :															  //
////////////////////////////////////////////////////////////////////////////////


// --------------------------------------------------------------------- include
#include "ZeNain.h"			// Fichier commun a toute les classes de ZeGame
#include "DSLib.h"			// Librairie DirectSound
#include "FileVirtual.h"	// Fichiers virtuels
#include "DSMp3.h"			// Son Header


// ------------------------------------------------------------------- variables


////////////////////////////////////////////////////////////////////////////////
//	Classe TDSMP3															  //
////////////////////////////////////////////////////////////////////////////////

const int TDSMp3::soundBufferLength = 400000;
const int TDSMp3::frequencies[9] = { 44100, 48000, 32000, 22050, 24000, 16000 , 11025 , 12000 , 8000 };		// Table des frequences possibles
const int TDSMp3::bitrates[15] = { 0,32,40,48, 56, 64, 80, 96,112,128,160,192,224,256,320 };				// Table des bit rates possibles 

const int TDSMp3::IN_BUF_SIZE = 8192;		// Lecture du fichier mp3 par increment de IN_BUF_SIZE octets
const int TDSMp3::OUT_BUF_SIZE = 8192;

//===	Constructeur par défaut ================================================//
TDSMp3::TDSMp3():
	TDSSoundFile()	// Constructeur de la classe parent
{	pWaveFormat = new WAVEFORMATEX();
	compbuf = NULL;
	compbufpos = 0;
	compbuflen = 0;
}
//----------------------------------------------------------------------------//


//=== Destructeur ============================================================//
TDSMp3::~TDSMp3()
{	// ferme le fichier .mp3 ouvert
	// ...
	
	stop();	// Arrete de jouer ce son
}
//----------------------------------------------------------------------------//


//=== Ouvre un fichier wav et se prépare à le lire ===========================//
bool TDSMp3::Open(char* soundPathName, char* name)
{	TDSSoundFile::Open(soundPathName, name);

	if (compbuf != NULL)
	{	delete compbuf; 	
		compbuf = NULL;
	}
	compbufpos = 0;
	compbuflen = 0;

	// Initialize the MP3 decoder
	InitMP3(&mp3Infos);

	// Ouvre le fichier mp3 pouvant être inclu dans un .pak ou être en mémoire
 	if (hSoundFV!=NULL) delete hSoundFV;
	hSoundFV = fopenFV(soundPathName);
	if (hSoundFV == NULL) return false;	// Erreur à l'ouverture du fichier
	
	// Lit la 1ere frame du mp3 afin de recuperer ses caracteristiques
	// et de decoder un 1er buffer
	int status = decopeMP3Trame();
	if(status == MP3_ERR) return false;

	// Ici, normalement, on aura normalement decode dans @compbuf environ 92160 octets 
	// suivants du mp3 (MP3_NEED_MORE status)
	
	// Description du format du mp3
	pWaveFormat = new WAVEFORMATEX();
	pWaveFormat->wFormatTag = WAVE_FORMAT_PCM;						// only tag valid with DirectSound
	pWaveFormat->nChannels = (unsigned short) mp3Infos.fr.stereo;	// nombre de channels (1 pour mono & 2 pour stereo)
	pWaveFormat->nSamplesPerSec = frequencies[mp3Infos.fr.sampling_frequency];	// Frequence en Hertz (44100, 22050 ou encore 48000)
	pWaveFormat->wBitsPerSample = 16;								// nombre de bits par sample (generalement 8 ou 16 bits)
	pWaveFormat->nBlockAlign =  (unsigned short) (pWaveFormat->nChannels*(pWaveFormat->wBitsPerSample/8));
	pWaveFormat->nAvgBytesPerSec =  pWaveFormat->nSamplesPerSec*pWaveFormat->nBlockAlign;

	// Alloue le DirectSound Buffer de 2 fois la taille de la plus grande frame que l'on 
	// pourrait decoder dans ce mp3. On instancie aussi les timers.
	if (compbuflen > soundBufferLength/2) return false;
	HRESULT hr = createSoundBuffer(soundBufferLength);
	if FAILED(hr) return false;	 

    return true;
}
//----------------------------------------------------------------------------//

//=== Lit une partie du fichier sonore dans le buffer audio =================//
bool TDSMp3::readSoundFile(DWORD dwBytes1,	// Nb d'octets à lire
					   BYTE *lpvPtr1,		// Destination data audio memory  
					   UINT *cbBytesRead)	// Actual bytes read
{	if (dwBytes1 < 0) return false;
	int status = MP3_OK;

	*cbBytesRead = 0;	// Aucun octet lu
	
	// On regarde tout d'abord si dans @compbuf, il n'y a pas des donnees audios
	// non encore copies dans le DirectSound buffer
	do {
		int octetsReady = compbufpos - compbufread;	// Nb d'octets non encore utilise ds @compbuf
		int octets2Read = dwBytes1 - *cbBytesRead;
		if (octetsReady > octets2Read)
		{	// Si il y'a plus d'octets ds @compbuf non encore utilises que ce dont
			// on a besoin de copier dans le DirectSound buffer, on n'aura pas besoin
			// de lire le reste du mp3
			memcpy(lpvPtr1+ *cbBytesRead, compbuf+compbufread, octets2Read);
			compbufread += octets2Read;
			*cbBytesRead += octets2Read;
			return true;
		} else 
		if (octetsReady > 0)
		{	// On utilise pleinement les donnees restantes de @compbuf
			memcpy(lpvPtr1 + *cbBytesRead, compbuf+compbufread, octetsReady);
			compbufread += octetsReady;
			*cbBytesRead += octetsReady;
			if (!hSoundFV->eof())
			{	status = decopeMP3Trame();	// status == NEED_MORE
			} else
			{	// On avait deja fini de lire le fichier
				// -> remarque ds le cas typique d'un tout petit mp3
				return false;	
			}
		}
	}
	// Ensuite, si besoin est, on continue de decoder une ou plusieurs trames du mp3
	// dans le DirectSound buffer jusqu'a ce qu'on y est copie dwBytes1 ou que l'on
	// est atteint la fin du fichier
	while ((*cbBytesRead < dwBytes1) && (status != MP3_ERR));
	return true;
}
//----------------------------------------------------------------------------//

//=== Ferme le fichier sonore ================================================//
bool TDSMp3::closeSoundFile()
{	if (compbuf != NULL)
	{	delete compbuf; 	
		compbuf = NULL;
	}
	fcloseFV(hSoundFV);
	hSoundFV = NULL;
	return true;
}			
//----------------------------------------------------------------------------//

//=== Decode dans @vompbuf la trame suivante du mp3 ==========================//
int TDSMp3::decopeMP3Trame()
{	static char inbuf[IN_BUF_SIZE], outbuf[OUT_BUF_SIZE];
	int inlen, outlen, status = MP3_OK;
	
	int mp3Size2Read = hSoundFV->size();	// de l'ordre de 4Mo
	// int mp3Size2Read = hSoundFV->getSizeToEnd(); [BUG?] 
	compbufpos = 0;							// reinitialise @compbuf
	compbufread = 0;

	// Calcul combien d'octets je peux encore lire
	inlen = (mp3Size2Read >= IN_BUF_SIZE) ? IN_BUF_SIZE : mp3Size2Read;	// utile pour les tout petits mp3
	hSoundFV->read(inbuf, inlen);

	// Recupere les meta-donnees du mp3 (frequences, bitrate, ...) et la taille exacte des donnees audio lues dans outbuf
	status = decodeMP3(&mp3Infos, inbuf, inlen, outbuf, OUT_BUF_SIZE, &outlen);

	// Decode dans @compbuf la trame suivante du mp3
	while(status == MP3_OK)
	{	
		// Check buffer size
		if ((compbufpos + outlen) > compbuflen || compbuf == NULL)
		{	// Recupere la taille des donnees audios de la frame courante restant a lire
			compbuflen = compbufpos+outlen;
			// et realloue la taille du buffer dans lequel on va lire @compbuflen octets du mp3
			compbuf = (char *) realloc(compbuf, compbuflen);
		}
		// Copy les @outlen octets que l'on vient de decoder dans le buffer @compbuf
		memcpy(compbuf+compbufpos, outbuf, outlen);
		compbufpos+=outlen;

		// A chaque passage, on decode outlen=4608 octets du mp3 que l'on concatene
		// dans compbuf
		status = decodeMP3(&mp3Infos, NULL, 0, outbuf, OUT_BUF_SIZE, &outlen);
	}
	compbufread = 0;	// Aucun octet de @compbuf deja copie ds le DirectSound buffer
	return status;
}
//----------------------------------------------------------------------------//