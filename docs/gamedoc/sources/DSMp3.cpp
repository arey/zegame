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

const int TDSMp3::soundBufferLength = 300000;

//===	Constructeur par défaut ================================================//
TDSMp3::TDSMp3():
	TDSSoundFile()	// Constructeur de la classe parent
{	pWaveFormat = new WAVEFORMATEX();
}
//----------------------------------------------------------------------------//


//=== Destructeur ============================================================//
TDSMp3::~TDSMp3()
{	// ferme le fichier .mp3 ouvert
	// ...
	TDSSoundFile::~TDSSoundFile();	// Destructeur de la classe parent
}
//----------------------------------------------------------------------------//

#define IN_BUF_SIZE 8192
#define OUT_BUF_SIZE 8192

//=== Ouvre un fichier wav et se prépare à le lire ===========================//
bool TDSMp3::Open(char* soundPathName, char* name)
{	TDSSoundFile::Open(soundPathName, name);

	// Initialize the MP3 decoder
	InitMP3(&mp3Infos);

	// Ouvre le fichier mp3 pouvant être inclu dans un .pak ou être en mémoire
	if (hSoundFV==NULL) delete hSoundFV;
	hSoundFV = fopenFV(soundPathName);
	if (hSoundFV == NULL) return false;	// Erreur à l'ouverture du fichier
	
	int mp3Size2Read = hSoundFV->size();		// de l'ordre de 4Mo
	int sectionSize2Read = soundBufferLength/2;	// dans les 150ko
	char inbuf[IN_BUF_SIZE], outbuf[OUT_BUF_SIZE];
	char *compbuf=NULL;
	int compbufpos = 0, compbuflen = 0;
	int inlen, outlen, status = MP3_OK;

	// Lit la 1ere frame du mp3 afin de recuperer ses caracteristiques
	// et de decoder un 1er buffer

	// Calcul combien d'octets je peux encore lire
	inlen = (mp3Size2Read >= IN_BUF_SIZE) ? IN_BUF_SIZE : mp3Size2Read;
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
	
	// Alloue le DirectSound Buffer de 2 fois la taille de la frame que l'on vient de decoder
	HRESULT hr = createSoundBuffer(outlen * 2);
	if FAILED(hr) return false;	 

    return true;
}
//----------------------------------------------------------------------------//


//=== Remplit le buffer audio avec un morceau de wav =========================//
bool TDSMp3::StreamToBuffer(DWORD dwPos)
{	
	return true;
}
//----------------------------------------------------------------------------//


