////////////////////////////////////////////////////////////////////////////////
//  Unité "ImageUtil.cpp"													  //	                                                                            //
//	Description : Librairie regroupant diverses fonctions de manipulations	  //
//				  d'images													  //	
//                                                                            //
//  Copyright (C) 1999 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com	                  //
//  Version      : beta 0.01                                                  //
//  Creation     : 29/11/1999 		                                          //
//  Last upgrade : <none>	                                                  //
////////////////////////////////////////////////////////////////////////////////

// --------------------------------------------------------------------- Include
#pragma warning(disable : 4100)

#include "ColorFormat.h"	// Fonctions de convertion entre les différents formats de couleurs
#include "ImageFile.h"		// Différents formats d'images
#include "FileVirtual.h"	// Fichiers virtuels (dans un .pak, en mémoire ou tout simplement sur le disk)
#include "ImageUtil.h"		// Header du fichier
#ifdef MAPEDITOR
#include "DDLibEditor.h"
#endif

// ----------------------------------------------------------- Fonctions annexes


//=== Charge une image d'un fichier dans une surface DirectDraw ==============//
IDirectDrawSurface4*  DDLoadAnyBitmapFromFile(IDirectDraw4 * pdd, TFileVirtual* file, int dx, int dy)
{	DDSURFACEDESC2          ddsd;
    IDirectDrawSurface4    *pdds  = NULL;
	TStdImage			   *image = NULL;

	// On essaye de charger un fichier en mémoire
	TImageLoader load;
	image = load.LoadAnyImage(file);
	if (image == NULL) return NULL;	
		
	// On crée une surface pour cette image
	ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
    ddsd.dwWidth = image->GetWidth();
    ddsd.dwHeight = image->GetHeight();
	if (pdd->CreateSurface(&ddsd, &pdds, NULL) != DD_OK) return NULL;
	
	// On copie l'image chargée dans la surface DirectDraw
#ifdef MAPEDITOR
  	switch (DS_VE->Screen_Bits)
#else
	switch (DS->Screen_Bits)
#endif    
	{	case 16:
			switch (image->GetBitPerPix())
			{	case 8  : CopyImage2Surface16b((TImage8b*) image, pdds); 
						  delete (TImage8b*) image;
						  break;
				case 24 : CopyImage2Surface16b((TImage24b*) image, pdds); 
						  delete (TImage24b*) image; 
						  break;
			}
			break;

		default:
			delete image;
			pdds->Release();
			return NULL;
	}

	// On détruit l'image crée et l'on rend la main
	// delete image; [BUG] ca planté à ce niveau donc je les détruis + haut
	return pdds;
}

IDirectDrawSurface4* DDLoadAnyBitmapFromFile(IDirectDraw4 * pdd, char* name, int dx, int dy)
{	IDirectDrawSurface4* pds = NULL;
	TFileVirtual *file = fopenFV(name);
	pds = DDLoadAnyBitmapFromFile(pdd, file, dx, dy);
	fcloseFV(file);
	return pds;
}
//----------------------------------------------------------------------------//


//=== Recharge une image dans une surface DirectDraw déjà existante ==========//
HRESULT DDReLoadAnyBitmapFromFile(IDirectDrawSurface4 *pdds, TFileVirtual* file)
{   TStdImage			   *image = NULL;
	
	// On essaye de recharger le fichier en mémoire
	TImageLoader load;
	image = load.LoadAnyImage(file);
	if (image == NULL) return -1;	

	// On copie l'image chargée dans la surface DirectDraw	
#ifdef MAPEDITOR
  	switch (DS_VE->Screen_Bits)
#else
	switch (DS->Screen_Bits)
#endif    

	{	case 16:
			switch (image->GetBitPerPix())
			{	case 8 : CopyImage2Surface16b((TImage8b*) image, pdds); break;
			}
			break;

		default:
			delete image;
			return -1;
	}
	// On détruit l'image crée et l'on rend la main
	delete image;
    return 0;
}

HRESULT DDReLoadAnyBitmapFromFile(IDirectDrawSurface4 *pdds, char* name)
{	TFileVirtual *file = fopenFV(name);
	HRESULT hr = DDReLoadAnyBitmapFromFile(pdds, file);
	fcloseFV(file);
	return hr;
}
//----------------------------------------------------------------------------//


//============================================================================//
void CopyImage2Surface16b(TImage8b* image, IDirectDrawSurface4 *surface)
{	HRESULT hr;
	// Vérouille les 2 surfaces
	DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
	hr = surface->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR /*| DDLOCK_WAIT | DDLOCK_WRITEONLY*/, NULL);
	if (hr == DDERR_SURFACEBUSY) return;
		
	int surfPitch = ddsd.lPitch;
	int srcPitch = image->GetPitch();
	WORD* dataDst = (WORD*) ddsd.lpSurface;
	BYTE* dataSrc = (BYTE*) image->GetDataAddr();
	DWORD offSrc=0, offDst=0;	

	for (int j=0 ; j < ddsd.dwHeight ; j++)
	{	offSrc = j * srcPitch;
		offDst = j * (surfPitch>>1);
		for (int i=0 ; i < ddsd.dwWidth ; i++)
		{	dataDst[offDst++] = ColorConv8to16(dataSrc[offSrc++], (unsigned int*) image->GetPalAddr());
		}
	}

	hr = surface->Unlock(NULL);
}
//----------------------------------------------------------------------------//

//============================================================================//
void CopyImage2Surface16b(TImage24b* image, IDirectDrawSurface4 *surface)
{	HRESULT hr;
	// Vérouille les 2 surfaces
	DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
	hr = surface->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR| DDLOCK_WAIT | DDLOCK_WRITEONLY, NULL);
	if (hr == DDERR_SURFACEBUSY) return;
		
	int surfPitch = ddsd.lPitch;
	int srcPitch = image->GetPitch();
	WORD* dataDst = (WORD*) ddsd.lpSurface;
	BYTE* dataSrc = (BYTE*) image->GetDataAddr();
	DWORD offSrc=0, offDst=0;	

	for (int j=0 ; j < ddsd.dwHeight ; j++)
	{	offSrc = j * srcPitch;
		offDst = j * (surfPitch>>1);
		for (int i=0 ; i < ddsd.dwWidth ; i++)
		{	dataDst[offDst++] = ColorConv24to16(dataSrc[offSrc+2], dataSrc[offSrc+1], dataSrc[offSrc]);
			offSrc += 3;
		}
	}

	hr = surface->Unlock(NULL);
}
//----------------------------------------------------------------------------//

