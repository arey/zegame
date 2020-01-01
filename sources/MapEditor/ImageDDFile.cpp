////////////////////////////////////////////////////////////////////////////////
//  Unité "ImageFile.cpp"													  //	                                                                            //
//	Description : Classes d'images de différents formats				      //
//                                                                            //
//  Copyright (C) 1999 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com	                  //
//  Version      : beta 0.01                                                  //
//  Creation     : 27/11/1999 		                                          //
//  Last upgrade : <none>	                                                  //
////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------- Include
#include <memory.h>			// pour le 'memset'
#include "FileFormat.h"		// Regroupe les formats de différents fichiers
#include "ColorFormat.h"	// Gestion des couleurs
#include "ImageFile.h"		// Classes d'Images
#ifdef MAPEDITOR
#include "DDLibEditor.h"
#include "Editor.h"
#endif
#include "ImageDDFile.h"	// Header du fichier


//----------------------------------------------------------------------- Define
#define BYTE	unsigned char
#define LONG	int
#define WORD	unsigned short int
#define DWORD   unsigned int



////////////////////////////////////////////////////////////////////////////////
// Classe TIMAGEDIRECTX                                                       //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur ===========================================================//
TImageDirectX::TImageDirectX():
	TImage16b()	
{	iBitPerPix = IMAGE_BPP_16;
}
//----------------------------------------------------------------------------//

//=== 2nd Constructeur =======================================================//
TImageDirectX::TImageDirectX(int w, int h):
	TImage16b(),		// Constructeurs des classes mères
	DDSurface(w, h)
{	iBitPerPix = IMAGE_BPP_DIRECTX;
	iWidth = w;
	iBpW = -1;
	iHeight = h;
	lpData = NULL;
	lpPal = NULL;
}
//----------------------------------------------------------------------------//

//=== Alloue l'espace nécessaire aux données de l'image ======================//
void TImageDirectX::InitImage(int w, int h)
{	iWidth = w;
	iBpW = w*2;
	iHeight = h;
	lpData = new BYTE[w*h*2];
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TImageDirectX::~TImageDirectX()
{ 
}
//----------------------------------------------------------------------------//

//=== 3ieme Constructeur =====================================================//
TImageDirectX::TImageDirectX(TFileVirtual *file):
	TImage16b()		// Constructeur d'une des 2 classes mères
{	iBitPerPix = IMAGE_BPP_DIRECTX;
	
	// Lit l'image sur le disque et la convertie en 16bits
	TStdImage *imageLoad = TImageLoader::LoadImageBmp(file);
	InitImage(imageLoad->GetWidth(), imageLoad->GetHeight());
	switch (imageLoad->GetBitPerPix())
	{	case 8: 
			((TImage8b*) imageLoad)->Copy(this);
			break;
	}
	delete imageLoad;
	
	// Initialise la description de la surface
	memset(&DDSurfDesc,0,sizeof(DDSurfDesc));  // met tout à zéro
    memset(&DDSurfDesc.ddpfPixelFormat, 0, sizeof(DDPIXELFORMAT));
	DDSurfDesc.dwSize=sizeof(DDSurfDesc);      // taille de sa propre structure
	DDSurfDesc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_LPSURFACE |
				         DDSD_PITCH | DDSD_PIXELFORMAT |DDSD_CAPS;
	DDSurfDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
    DDSurfDesc.dwWidth = iWidth;
    DDSurfDesc.dwHeight= iHeight;
    DDSurfDesc.lPitch  = 2 * iWidth;
    DDSurfDesc.lpSurface = lpData;
 
    // Format des pixels : 16-bit RGB (5-6-5).
    DDSurfDesc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
    DDSurfDesc.ddpfPixelFormat.dwFlags= DDPF_RGB;
    DDSurfDesc.ddpfPixelFormat.dwRGBBitCount = 16;
    DDSurfDesc.ddpfPixelFormat.dwRBitMask    = 0xF800;
    DDSurfDesc.ddpfPixelFormat.dwGBitMask    = 0x07E0;
    DDSurfDesc.ddpfPixelFormat.dwBBitMask    = 0x001F;

	// Crée la surface DirectDraw
    #ifdef MAPEDITOR
	HRESULT hr = DS_VE->pDD->CreateSurface(&DDSurfDesc, &pDDSurf, NULL);
    #else
	HRESULT hr = DS->pDD->CreateSurface(&DDSurfDesc, &pDDSurf, NULL);    
    #endif
	if FAILED(hr) TraceErrorDD(hr,99);

	SrcRect.top = DestRect.top = 0;
    SrcRect.left = DestRect.left = 0;
    PixWidth = SrcRect.bottom = DestRect.bottom = iHeight;
    PixHeight = SrcRect.right = DestRect.right = iWidth;
    ColKey1=ColKey2=0;
}
//----------------------------------------------------------------------------//

//=== Copie une image 256c dans la surface DirectX ===========================//
bool TImageDirectX::Load(TImage8b* image)
{	Lock2();
	
	DWORD offSrc=0, offDst=0;
	WORD* dataDst = (WORD*) DDSurfDesc.lpSurface;
	BYTE* dataSrc = (BYTE*) image->GetDataAddr();

	for (int i=0 ; i < iHeight ; i++)
	{   int j;
        for (j=0 ; j < iHeight ; j++)
		{	dataDst[offDst++] = ColorConv8to16(dataSrc[offSrc++], (DWORD*) image->GetPalAddr());
		}
		offSrc = j * iBpW;
		offDst = j * (iBpW>>2);
	}
	UnLock();
	return true;
}
//----------------------------------------------------------------------------//
////////////////////// Fin de la classe TIMAGEDIRECTX //////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Fonctions ANNEXES														  //	
////////////////////////////////////////////////////////////////////////////////

