////////////////////////////////////////////////////////////////////////////////
//  Unité "SoundUtil.h"														  //	                                                                            //
//                                                                            //
//	Description : Librairie regroupant diverses fonctions de manipulations	  //
//				  des fichiers sonores (.wav)								  //
//                                                                            //
//  Copyright (C) 1999 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com	                  //
//  Version      : beta 0.01                                                  //
//  Creation     : 13/12/1999 		                                          //
//  Last upgrade : <none>	                                                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef SoundUtilH
#define SoundUtilH

//---------------------------------------------------------------------- Include
#ifdef MICROSOFT
#pragma warning(disable : 4100 4663 4018)  
#endif
#include "wave.h"			// Librairie microsoft maitrisant le format .wav
#include "mmsystem.h" 		// Windows Multimedia API's

//----------------------------------------------------------------------- Define
class TFileVirtual;


// Fonctions basées sur wave.c de microsoft intégrant la gestion de fichiers virtuels.	
int WaveOpenFileFV(char *szFileName, TFileVirtual* &fileFV, WAVEFORMATEX **ppwfxInfo, MMCKINFO *pckInRIFF); 
int WaveReadFileFV(TFileVirtual *fileFV, UINT cbRead, BYTE *pbDest,	MMCKINFO *pckIn, UINT *cbActualRead);
int WaveCloseReadFileFV(TFileVirtual *&fileFV, WAVEFORMATEX **);

#endif	// SoundUtilH