////////////////////////////////////////////////////////////////////////////////
//  Unité "ImageUtil.h"														  //	                                                                            //
//	Description : Librairie regroupant diverses fonctions de manipulations	  //
//				  d'images													  //	
//                                                                            //
//  Copyright (C) 1999 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com	                  //
//  Version      : beta 0.01                                                  //
//  Creation     : 29/11/1999 		                                          //
//  Last upgrade : <none>	                                                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef ImageUtilH
#define ImageUtilH

//---------------------------------------------------------------------- Include
#ifdef MICROSOFT
#pragma warning(disable : 4100 4663 4018)  
#endif
#include <fstream.h>    // Gestionnaire de fichiers de la STL
#include "DDLib.h"		// Ma lib DirectDraw

//----------------------------------------------------------------------- Define


//------------------------------------------------------------ Fonctions annexes
class TFileVirtual;
class TImage8b;
class TImage24b;

// Charge une image d'un fichier dans une surface DirectDraw
IDirectDrawSurface4* DDLoadAnyBitmapFromFile(IDirectDraw4 * pdd, TFileVirtual* file, int dx=-1, int dy=-1);
IDirectDrawSurface4* DDLoadAnyBitmapFromFile(IDirectDraw4 * pdd, char* name, int dx=-1, int dy=-1);
// Recharge une image dans une surface DirectDraw déjà existante
HRESULT DDReLoadAnyBitmapFromFile(IDirectDrawSurface4 *pdds, TFileVirtual* file);
HRESULT DDReLoadAnyBitmapFromFile(IDirectDrawSurface4 *pdds, char* name);


void CopyImage2Surface16b(TImage8b* image, IDirectDrawSurface4 *surface);
void CopyImage2Surface16b(TImage24b* image, IDirectDrawSurface4 *surface);




#endif	// ImageUtilH
