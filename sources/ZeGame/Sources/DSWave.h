////////////////////////////////////////////////////////////////////////////////
//  Unité  "DSWave.h" : Permet de lire un .wav								  //				
//						Cette clase derive de la classe abstraite TDSoundFile //  
//                                                                            //  
//  Copyright (C) 1998 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com                      //
//					                                                          //
//  Version      : beta 0.01                                                  //
//  Creation     : 10/04/1999                                                 //
//  Last upgrade : 10/04/1999                                                 //
////////////////////////////////////////////////////////////////////////////////

#ifndef DSWaveH
#define DSWaveH

#ifdef MICROSOFT
#pragma warning(disable : 4100 4663 4018 4786 4201)  
#endif

// --------------------------------------------------------------------- Include
#include <objbase.h>		
#pragma warning(disable : 4201)  
#include "DSLib.h"			// Librairie maison DirectSound
#include <vector>			// librairie STL de gestion de tableaux dynamiques

class TFileVirtual;


// TDSWAVE /////////////////////////////////////////////////////////////////////
//	Classe s'occupant de jouer des fichiers sonores .wav   					  //
class TDSWave : public TDSSoundFile
{// ----------------------------------------------------------------- Attributs
	MMCKINFO                    mckInfoData;			// Chunk info 
	MMCKINFO					mckInfoParent;			// Parent chunk info
	int							iDataWavSize;			// Taille des données du .wav à lire

 // ----------------------------------------------------------------- Methodes
  public:
	TDSWave();							// Constructeur par défaut
	virtual ~TDSWave();					// Destructeur

	// Lit une partie du fichier sonore dans le buffer audio
	bool readSoundFile(DWORD dwBytes1,		// Nb d'octets à lire
					   BYTE *lpvPtr1,		// Destination data audio memory  
					   UINT *cbBytesRead);	// Actual bytes read

	bool closeSoundFile();				// Ferme le fichier sonore

	bool Open(char* soundPathName,  char* name = NULL);	// Ouvre un fichier wav et se prépare à le lire
};
////////////////////////////// Fin de TDSWAVE /////////////////////////////////


#endif // DSWaveH