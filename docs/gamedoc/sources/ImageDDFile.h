////////////////////////////////////////////////////////////////////////////////
//  Unité "ImageDDFile.h"													  //	                                                                            //
//	Description : Classes d'images utilisées directement par DirectDraw		  //
//                                                                            //
//  Copyright (C) 1999 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com	                  //
//  Version      : beta 0.01                                                  //
//  Creation     : 01/12/1999 		                                          //
//  Last upgrade : <none>	                                                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef ImageDDFileH
#define ImageDDFileH

//---------------------------------------------------------------------- Include
#ifdef VISUALC
#pragma warning(disable : 4100 4663 4018)  
#endif
#include "ImageFile.h"	// Classes d'Images
#include "DDLib.h"		// Librairie maison DirectDraw

class TFileVirtual;

//----------------------------------------------------------------------- Define

// TIMAGEDIRECTX ///////////////////////////////////////////////////////////////
//##ModelId=38A4889B026C
class TImageDirectX : public TImage16b, public DDSurface
{   //--------------------------------------------------- Attributs de la classe
	
 	//---------------------------------------------------- Méthodes de la classe
  public:
	//##ModelId=38A4889B0275
	TImageDirectX();						// Constructeur par défaut
	//##ModelId=38A4889B02A8
	TImageDirectX(int w, int h);			// 2nd Constructeur
	//##ModelId=38A4889B02AB
	TImageDirectX(TFileVirtual *file);		// 3ieme Constructeur
	//##ModelId=38A4889B0272
	virtual void InitImage(int w, int h);	// Alloue l'espace nécessaire aux données de l'image
	//##ModelId=38A4889B0271
    virtual ~TImageDirectX();				// Destructeur
	//##ModelId=38A4889B026F
	virtual bool Load(TImage8b* image);		// Copie une image 256c dans la surface DirectX
};
////////////////////////// FIN de TIMAGEDIRECTX ////////////////////////////////



//----------------------------------------- Instance unique de la classe externe

//----------------------------------------------------------- Fonctions externes



#endif	// ImageDDFileH
