////////////////////////////////////////////////////////////////////////////////
//  Unité "FileFormat.h"													  //	                                                                            //
//	Description : Formats de différents fichiers							  //
//                                                                            //
//  Copyright (C) 1999 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com	                  //
//  Version      : 1.0		                                                  //
//  Creation     : 27/11/1999 		                                          //
//  Last upgrade : <none>	                                                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef FileFormatH
#define FileFormatH

//---------------------------------------------------------------------- Include

//----------------------------------------------------------------------- Define
#define BYTE	unsigned char
#define WORD	unsigned short int
#define DWORD   unsigned long
#define LONG	int

//-------------------------------------------------------------------------- BMP


struct FFBitmapFileHeader	// 14 Octets
{
  WORD	  bfPadding1;		// NE PAS LIRE CE MOT
  WORD    bfType;			// 2 caractères identifiants le bmp 'BM' - Windows 3.1x, 98, NT | 'BA' - OS/2 Bitmap Array | 'IC' - OS/2 Icon ... 
  DWORD   bfSize;			// Taille totale de l'image
  WORD    bfReserved1;		// 2 mots réservés
  WORD    bfReserved2;
  DWORD   bfOffBits;		// Offset de début des données
};



struct FFBitmapInfoHeader	// 40 Octets
{
  DWORD  biSize;			// Longueur du BitmapinfoHeader utilisé pour décrire les couleurs, la compression, ... 3 valeurs possibles : 28h Win98 - 0Ch OS/2 1.x - F0h OS/2 2.x
  LONG   biWidth;			// Largeur de l'image en pixels
  LONG   biHeight;			// Hauteur de l'image en pixels
  WORD   biPlanes;			// Nb de plan de bits
  WORD   biBitCount;		// Nb de bits par couleurs : 1, 4, 8, 16, 24, 32
  DWORD  biCompression;		// Compression : 0 BI_RGB - 1 BI_RLE4 - 2 BI_RLE8 - 3 BI_BITFIELDS
  DWORD  biSizeImage;		// Taille des données (à arrondir au double mot suivant)
  LONG   biXPelsPerMeter;	// Résolution en X, exprimée en pixels / mètre
  LONG   biYPelsPerMeter;	// Résolution en Y, exprimée en pixels / mètre
  DWORD  biClrUsed;			// Nb de couleurs utilisées par l'image (ex: 256 pour 8bits/pixel)
  DWORD  biClrImportant;	// Nb de couleurs importantes
};


#endif	// FileFormatH
