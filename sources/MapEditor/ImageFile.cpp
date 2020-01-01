////////////////////////////////////////////////////////////////////////////////
//  Unité "ImageDDFile.cpp"													  //	                                                                            //
//	Description : Classes d'images utilisées directement par DirectDraw		  //
//                                                                            //
//  Copyright (C) 1999 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com	                  //
//  Version      : beta 0.01                                                  //
//  Creation     : 01/12/1999 		                                          //
//  Last upgrade : <none>	                                                  //
////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------- Include
#include <memory.h>			// pour le 'memset'
#include "FileFormat.h"		// Regroupe les formats de différents fichiers
#include "ColorFormat.h"	// Gestion des couleurs
#include "FileVirtual.h"	// Fichiers virtuels (dans un .pak, en mémoire ou tout simplement sur le disk)
#include "ImageDDFile.h"	// Images spécialisées dans des surfaces DirectDraw
#include "ImageFile.h"		// Header du fichier - Classes d'Images de tout format


#define BYTE	unsigned char
#define LONG	int
#define WORD	unsigned short int
#define DWORD   unsigned int

////////////////////////////////////////////////////////////////////////////////
// Classe TSTDIMAGE                                                           //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur ===========================================================//
TStdImage::TStdImage():
	lpData(NULL), lpPal(NULL), iImageType(TYPE_IMAGE_UNKNOW)
{	
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TStdImage::~TStdImage()
{ 
}
//----------------------------------------------------------------------------//

//=== Renvoie un pointeur sur les données ====================================//
void* TStdImage::GetDataAddr()
{	return lpData;
}
//----------------------------------------------------------------------------//

//=== Renvoie un pointeur sur la palette =====================================//
void* TStdImage::GetPalAddr()
{	return lpPal;
}
//----------------------------------------------------------------------------//
////////////////////// Fin de la classe TSTDIMAGE //////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Classe TIMAGE8B                                                            //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur ===========================================================//
TImage8b::TImage8b():
	TStdImage()	
{	iBitPerPix = IMAGE_BPP_8;
}
//----------------------------------------------------------------------------//

//=== 2nd Constructeur =======================================================//
TImage8b::TImage8b(int w, int h):
	TStdImage()	
{	iBitPerPix = IMAGE_BPP_8;
	InitImage(w, h);
}
//----------------------------------------------------------------------------//

//=== Alloue l'espace nécessaire aux données de l'image ======================//
void TImage8b::InitImage(int w, int h)
{	iWidth = w;
	iBpW = w;
	iHeight = h;
	lpPal = new DWORD[256];
	lpData = new BYTE[w*h];
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TImage8b::~TImage8b()
{	delete lpData;	lpData = NULL;
	delete lpPal;	lpPal = NULL;
}
//----------------------------------------------------------------------------//

//=== Copy cette image dans une surface directX ==============================//
//----------------------------------------------------------------------------//

//=== Copy cette image 8 bits dans une image 16 bits =========================//
bool TImage8b::Copy(TImage16b* image)
{	DWORD offSrc=0, offDst=0;
	BYTE* dataSrc = (BYTE*) lpData;
	WORD* dataDst = (WORD*) image->GetDataAddr();

	for (int j=0 ; j < iHeight ; j++)
	{	offSrc = j * iBpW;
		offDst = j * (image->GetPitch()>>1);
		for (int i=0 ; i < iWidth ; i++)
		{	dataDst[offDst++] = ColorConv8to16(dataSrc[offSrc++], (DWORD*) lpPal);
		}		
	}
	return true;
}
//----------------------------------------------------------------------------//

////////////////////// Fin de la classe TIMAGE8B ///////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Classe TIMAGE16B                                                           //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur ===========================================================//
TImage16b::TImage16b():
	TStdImage()	
{	iBitPerPix = IMAGE_BPP_16;
}
//----------------------------------------------------------------------------//

//=== 2nd Constructeur =======================================================//
TImage16b::TImage16b(int w, int h):
	TStdImage()	
{	iBitPerPix = IMAGE_BPP_16;
	iWidth = w;
	iBpW = w*2;
	iHeight = h;
	lpData = new BYTE[w*h*2];
}
//----------------------------------------------------------------------------//

//=== 3ieme Constructeur : copie l'image se trouvant dans l'image 256c =======//
TImage16b::TImage16b(TImage8b* image):
	TStdImage()	
{	iBitPerPix = IMAGE_BPP_16;
	InitImage(image->GetWidth(), image->GetHeight());
	image->Copy(this);
}
//----------------------------------------------------------------------------//

//=== Alloue l'espace nécessaire aux données de l'image ======================//
void TImage16b::InitImage(int w, int h)
{	iWidth = w;
	iBpW = w*2;
	iHeight = h;
	lpData = new BYTE[w*h*2];
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TImage16b::~TImage16b()
{	delete lpData;	lpData = NULL;
}
//----------------------------------------------------------------------------//
////////////////////// Fin de la classe TIMAGE16B //////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Classe TIMAGE24B                                                           //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur ===========================================================//
TImage24b::TImage24b():
	TStdImage()	
{	iBitPerPix = IMAGE_BPP_24;
}
//----------------------------------------------------------------------------//

//=== 2nd Constructeur =======================================================//
TImage24b::TImage24b(int w, int h):
	TStdImage()	
{	iBitPerPix = IMAGE_BPP_24;
	iWidth = w;
	iBpW = w*3;
	iHeight = h;
	lpData = new BYTE[w*h*3];
}
//----------------------------------------------------------------------------//

//=== Alloue l'espace nécessaire aux données de l'image ======================//
void TImage24b::InitImage(int w, int h)
{	iWidth = w;
	iBpW = w*3;
	iHeight = h;
	lpData = new BYTE[w*h*3];
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TImage24b::~TImage24b()
{	delete lpData;	lpData = NULL;
}
//----------------------------------------------------------------------------//

//=== Copy cette image 24 bits dans une image 16 bits ========================//
bool TImage24b::Copy(TImage16b* image)
{	DWORD offSrc=0, offDst=0;
	BYTE* dataSrc = (BYTE*) lpData;
	WORD* dataDst = (WORD*) image->GetDataAddr();

	for (int j=0 ; j < iHeight ; j++)
	{	offSrc = j * iBpW;
		offDst = j * (image->GetPitch()>>1);
		for (int i=0 ; i < iWidth ; i++)
		{	dataDst[offDst++] = ColorConv24to16(dataSrc[offSrc+2], dataSrc[offSrc+1], dataSrc[offSrc]);
			offSrc += 3;
		}		
	}
	return true;
}
//----------------------------------------------------------------------------//

////////////////////// Fin de la classe TIMAGE24B //////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Classe TIMAGELOADER                                                        //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur ===========================================================//
TImageLoader::TImageLoader()
{	iMaxHeaderSize = IMAGE_HEADER_SIZE_BMP;
}
//----------------------------------------------------------------------------//


//=== Destructeur ============================================================//
TImageLoader::~TImageLoader()
{ 
}
//----------------------------------------------------------------------------//

//=== Renvoie le type de l'image si il l'a reconnu
int TImageLoader::GetRecognizeHeader(BYTE* data)
{	// Vérifie si c'est une image BMP (en-tête de 2 octets)
	if ((data[0]=='B') && (data[1]=='M')) return TYPE_IMAGE_BMP;
	
	return TYPE_IMAGE_UNKNOW;	// Format de l'image inconnu
}
//----------------------------------------------------------------------------//

//=== Crée et charge une image et renvoie son ptr ============================//
TStdImage* TImageLoader::LoadAnyImage(TFileVirtual *fp)
{	TStdImage*	image = NULL;
	
	// Lit l'en-tête du fichier pour déterminer de quel format de fichier on a affaire
	BYTE fileHeader[IMAGE_HEADER_SIZE_BMP];
	fp->read(fileHeader, iMaxHeaderSize);
	fp->seekg(-iMaxHeaderSize, ios::cur);
	int formatImage = GetRecognizeHeader(fileHeader);

	switch (formatImage)
	{	// Image BMP de Microsoft
		case (TYPE_IMAGE_BMP):
				image = LoadImageBmp(fp);				
			break;
		default: return NULL;
	}

	return image;
}
//----------------------------------------------------------------------------//

//=== Charge le BMP en mémoire ===============================================//
TStdImage* TImageLoader::LoadImageBmp(TFileVirtual *fp)
{	TStdImage*	image = NULL;
	int cur = fp->tellg();
	
	// Lecture des en-têtes du Bmp
	FFBitmapFileHeader file;
	FFBitmapInfoHeader info;
	fp->read((char*) &file.bfType, 14);
	fp->read((char*) &info, sizeof(info));
	if (fp->bad()) return NULL;

	// Image 256 couleurs
	if ((info.biBitCount==8) && (info.biPlanes==1))
	{	image = new TImage8b(info.biWidth, info.biHeight);
		image->SetImageType(TYPE_IMAGE_BMP);

		// Lecture de la palette
		info.biClrUsed =  (info.biClrUsed>0) ? info.biClrUsed : 256;
		fp->read((char*) image->lpPal, info.biClrUsed*sizeof(DWORD));

		// Image non compressée
		if (info.biCompression==0)
		{	// On se place au début des data
			fp->seek(cur+file.bfOffBits);
			
			// Chaque ligne est allignée sur 4 octets -> calcul du padding
			BYTE paddBuf[4];
			int padding = (file.bfSize-54-info.biClrUsed*4) / info.biHeight - info.biWidth;
			
			// On lit chaque ligne une à une
			for (int i=0 ; i<info.biHeight ; i++)  
			{	fp->read((char*) image->lpData + (info.biHeight-i-1)*info.biWidth, info.biWidth);
				if (fp->fail()) return NULL;
				// On lit les données qui ne servent à rien
				if (padding)
				{	fp->read(paddBuf, padding);
				}
			}
			return image;
		}
		// Compression RLE4
		else if (info.biCompression == 1)	
		{
			bool done = false;
			int iDataCompt = 0;
			int passCompt = 0;
			BYTE *data = (BYTE*) image->lpData;
			int x=0, y= image->iHeight-1;

			while (!done)
			{
			  BYTE pair[2];
			  fp->read(pair, 2);
			  iDataCompt+=2;
			  passCompt++;
			  if (*pair==0)				// Car. spécial
			  {         
				if (pair[1]==0)			// Fin de la ligne
				{
				  x=0;
				  y--;
				  data= (BYTE*) image->lpData+ y*image->iWidth;
				} 
				else if (pair[1]==1)	// Fin du Bmp
				{  done = true;
				}
				else if (pair[1]==2)	// goto xy
				{
				  fp->read(pair,2);
				  x+=pair[0];

				  y-=pair[1];
				  data = (BYTE*) image->lpData + y*image->iWidth + x;                   
				} else					// On continue sur des données
				{             
				  fp->read(data, pair[1]);
				  iDataCompt+=pair[1];
     
				  if (pair[1] & 1) { fp->read(pair,1); iDataCompt++; }
				  data+=pair[1];
				  x+=pair[1];
				}
			  } else					// Même couleur
			  {	
				memset(data, pair[1], pair[0]);
				data += pair[0];
				x += pair[0];
			  }
			}
			return image;
		} else
		{	// Format inconnu
			delete image;
			return NULL;
		}
	} else
	// Image 16M couleurs (24bits)
	if ((info.biBitCount==24) && (info.biPlanes==1))
	{	image = new TImage24b(info.biWidth, info.biHeight);
		image->SetImageType(TYPE_IMAGE_BMP);

		// On se place au début des data
		fp->seek(cur+file.bfOffBits);
		
		// Chaque ligne est allignée sur 4 octets -> calcul du padding
		BYTE paddBuf[4];
		int padding = (file.bfSize-54-info.biClrUsed*4) / info.biHeight - info.biWidth*3;
		
		// On lit chaque ligne une à une
		for (int i=0 ; i < info.biHeight ; i++)  
		{	fp->read((char*) image->lpData + (info.biHeight-i-1)*info.biWidth*3, info.biWidth*3);
			if (fp->fail()) return NULL;
			// On lit les données qui ne servent à rien
			if (padding)
			{	fp->read(paddBuf, padding);
			}
		}
		return image;
	}

	return image;
}
//----------------------------------------------------------------------------//


////////////////////// Fin de la classe TIMAGELOADER ///////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Fonctions ANNEXES														  //	
////////////////////////////////////////////////////////////////////////////////

