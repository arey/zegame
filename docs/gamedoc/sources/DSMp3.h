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

 // ----------------------------------------------------------------- Methodes
  public:
	TDSMp3();							// Constructeur par défaut
	virtual ~TDSMp3();					// Destructeur

	bool Open(char* soundPathName,  char* name = NULL);	// Ouvre un fichier mp3 et se prépare à le lire
	bool StreamToBuffer(DWORD dwPos);	// Fonction remplissant le buffer audio à partir de la position dwPos

};
////////////////////////////// Fin de TDSMP3 ///////////////////////////////////


#endif // DSMp3H