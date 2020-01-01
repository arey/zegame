////////////////////////////////////////////////////////////////////////////////
//  Unité "ColorFormat.h"													  //	                                                                            //
//	Description : Fonctions de conversion entre les différents types de coul. //
//                                                                            //
//  Copyright (C) 1999 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com	                  //
//  Version      : 1.0		                                                  //
//  Creation     : 28/11/1999 		                                          //
//  Last upgrade : <none>	                                                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef ColorFormatH
#define ColorFormatH

//---------------------------------------------------------------------- Include
//#include "windef.h"

//----------------------------------------------------------------------- Define
#define BYTE	unsigned char
#define LONG	int
#define WORD	unsigned short int
#define DWORD   unsigned int



inline WORD ColorConv32to16(DWORD color)
{
	DWORD r = (color>>16) & 0xFF;
	DWORD g = (color>>8) & 0xFF;
	DWORD b = (color>>0) & 0xFF;
	r >>= 3;
	g >>= 2;
	b >>= 3;
	return (WORD) (r<<11 | g<<5 | b);
}


inline WORD ColorConv32to15(DWORD color)
{
  DWORD r=(color>>(16+3))   & (1 | 2 | 4 | 8 | 16);
  DWORD g=(color>>(8+3))    & (1 | 2 | 4 | 8 | 16);
  DWORD b=(color>>(0+3))    & (1 | 2 | 4 | 8 | 16);
  return (WORD) ((r<<10) | (g<<5) | b);
}


inline DWORD ColorConvRGBto32(BYTE r, BYTE g, BYTE b)
{ return (r<<16) | (g<<8) | b;
}

inline DWORD ColorConv8to24(BYTE ind, BYTE* pal)
{	return ((DWORD) pal[ind*4]);
}

inline WORD ColorConv8to16(BYTE ind, DWORD* pal)
{	DWORD color = pal[ind];
	DWORD r = (color>>16) & 0xFF;
	DWORD g = (color>>8) & 0xFF;
	DWORD b = (color>>0) & 0xFF;
	r >>= 3;
	g >>= 2;
	b >>= 3;	
	return (WORD) (r<<11 | g<<5 | b);	
}

inline WORD ColorConv24to16(BYTE r, BYTE g, BYTE b)
{	r >>= 3;
	g >>= 2;
	b >>= 3;	
	return (WORD) (r<<11 | g<<5 | b);	
}

#undef BYTE
#undef WORD
#undef DWORD
#undef LONG
#endif	// ColorFormatH
