////////////////////////////////////////////////////////////////////////////////
//  Unité  "DSMp3.h" : Permet de lire des .mp3								  //				
//					   Cette clase derive de la classe abstraite TDSoundFile  //  
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

#ifndef DSMp3H
#define DSMp3H

#ifdef MICROSOFT
#pragma warning(disable : 4100 4663 4018 4786 4201)  
#endif

// --------------------------------------------------------------------- Include
#include <objbase.h>		
#pragma warning(disable : 4201)  
#include "DSLib.h"			// Librairie maison DirectSound
#include <vector>			// librairie STL de gestion de tableaux dynamiques
#include "mpglib\mpg123.h"	// mpg123 library
extern "C"
{
#include "mpglib\mpglib.h"	// mpg123 library
}

class TFileVirtual;


// TDSMP3 /////////////////////////////////////////////////////////////////////
//	Classe s'occupant de lire, decoder et jouer des fichiers sonores mp3     //
class TDSMp3 : public TDSSoundFile
{// ----------------------------------------------------------------- Attributs
	mpstr mp3Infos;						// Structure contenant tout un tas d'infos sur le mp3 en cours		
	static const int soundBufferLength;	// Longueur pre-definie du DirectSound Buffer
	
	static const int frequencies[9];	// Table des frequences possibles = { 44100, 48000, 32000, 22050, 24000, 16000 , 11025 , 12000 , 8000 };
	static const int bitrates[15];		// Table des bit rates possibles  = { 0,32,40,48, 56, 64, 80, 96,112,128,160,192,224,256,320 };

	static const int IN_BUF_SIZE;		// Lecture du fichier mp3 par increment de IN_BUF_SIZE octets
	static const int OUT_BUF_SIZE;		//
	
	char *compbuf;						// Buffer a taille variable ou l'on va lire une trame du mp3
	int compbuflen;						// Taille courante du buffer (peut varier)
	int compbufpos;						// Position ds le buffer a partir de laquelle un bout on doit copier le reste du mp3	
	int compbufread;					// Nb d'octets du buffer que l'on a deja copie ds le DirectSound buffer 

 // ----------------------------------------------------------------- Methodes
  public:
	TDSMp3();							// Constructeur par défaut
	virtual ~TDSMp3();					// Destructeur

	// Lit une partie du fichier sonore dans le buffer audio
	bool readSoundFile(DWORD dwBytes1,		// Nb d'octets à lire
					   BYTE *lpvPtr1,		// Destination data audio memory  
					   UINT *cbBytesRead);	// Actual bytes read

	bool closeSoundFile();				// Ferme le fichier sonore

	bool Open(char* soundPathName,  char* name = NULL);	// Ouvre un fichier mp3 et se prépare à le lire
  private:
	  int decopeMP3Trame();
};
////////////////////////////// Fin de TDSMP3 ///////////////////////////////////


#endif // DSMp3H