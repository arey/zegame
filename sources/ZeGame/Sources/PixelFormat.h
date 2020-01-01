////////////////////////////////////////////////////////////////////////////////
//  Unité "PixelFormat.h"													  //	                                                                            //
//	Description : Recense des informations sur les differents formats d'un	  //
//                pixel : 8,1624 ou 32 bits									  //
//                                                                            //
//  Copyright (C) 2000 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com	                  //
//  Version      : 1.0		                                                  //
//  Creation     : 25/08/2000 		                                          //
//  Last upgrade : <none>	                                                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef PixelFormatH
#define PixelFormatH

//---------------------------------------------------------------------- Include

//----------------------------------------------------------------------- Define
#define BYTE	unsigned char
#define WORD	unsigned short int
#define DWORD   unsigned long
#define LONG	int

#define PIXEL_FORMAT_8b		 8
#define PIXEL_FORMAT_16b	16
#define PIXEL_FORMAT_24b	24
#define PIXEL_FORMAT_32b	32


#define PIXEL_FORMAT_INDEXED_COLOR		1
#define	PIXEL_FORMAT_NON_INDEXED_COLOR	2


class TPixelFormat
{ public:
	DWORD	dwNbBitPerPixel;		// Nombre de bits par pixels : 8, 16, 24 ou 32 bits
	DWORD	dwColorMode;			// Indexed color or not
	
	TPixelFormat():
		dwNbBitPerPixel(0) { }
	TPixelFormat(DWORD nbBitPerPixel):
		dwNbBitPerPixel(nbBitPerPixel){	}
	TPixelFormat(DWORD nbBitPerPixel, DWORD colorMode):
		dwNbBitPerPixel(nbBitPerPixel), dwColorMode(colorMode){	}
};

class TPixelFormatIndexedColor : public TPixelFormat
{ public:
	// Default constructor
	TPixelFormatIndexedColor() { }
	TPixelFormatIndexedColor(int nbBitPerPixel):
		TPixelFormat(nbBitPerPixel, PIXEL_FORMAT_INDEXED_COLOR) {  }
};

class TPixelFormatNonIndexedColor : public TPixelFormat
{ public:
	DWORD	dwNbRedBit;
	DWORD	dwNbGreenBit;
	DWORD	dwNbBlueBit;
	DWORD	dwRedBitMask;
	DWORD	dwGreenBitMask;
	DWORD	dwBlueBitMask;
	DWORD	dwShiftRed;
	DWORD	dwShiftGreen;
	DWORD	dwShiftBlue;
  
  public:
	// Default constructor
	TPixelFormatNonIndexedColor():
		dwNbRedBit(0), dwNbGreenBit(0), dwNbBlueBit(0) { }
	TPixelFormatNonIndexedColor(DWORD nbBitPerPixel):
		TPixelFormat(nbBitPerPixel, PIXEL_FORMAT_NON_INDEXED_COLOR),
		dwNbRedBit(0), dwNbGreenBit(0), dwNbBlueBit(0) { }
};


#undef BYTE
#undef WORD
#undef DWORD
#endif	// PixelFormatH
