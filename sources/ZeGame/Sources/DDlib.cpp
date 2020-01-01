////////////////////////////////////////////////////////////////////////////////
//  Unité  "DDLib.cpp"  : Library des principales fonctions de DirectDraw     //
//  Copyright (C) 1998 Antoine Rey    arey@insa-lyon.fr                       //
//                                    antrey@club-internet.fr                 //
//                                                                            //
//  Version      : beta 0.65                                                  //
//  Creation     : 21/05/1998 17h45                                           //
//  Last upgrade : 31/08/1998                                                 //
//                                                                            //
//  Comment :  Destiné à la programmation de DirectDraw en FULLSCREEN MODE    //
//                                                                            //
//  Sources :                                                                 //
//    -  des parties de codes proviennent du bouqin Delphi,                   //
//    -  d'autres du 'Game Programming with Direct X' by David Joffe          //
//          http://www.geocities.com/SoHo/Lofts/2018/dj3.html                 //
//    - voir aussi doc http://www.pcisys.net/~omar/direct2.html               //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Rmq : - Problème de conversion couleur 24 bits en 16 bits   "RGBto16"      //
//		 - nouveau header  "DDlib.h" d'accompagnement						  //
////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------- include
#include "ZeNain.h"			// Fichier commun à tout le projet
#include "D3DLib.h"         // sprites 3D
#include "DDutil.h"         // Routines for loading bitmaps & palettes  IDirectDrawSurface4
#include "ImageUtil.h"		// Routines maison pour charger les bitmaps dans des surfaces DirectDraw
#include "PixelFormat.h"	// Structures definissant les divers formats de pixels que l'on peut rencontrer
#include "FileVirtual.h"	// Fichiers virtuels (dans un .pak, en mémoire ou tout simplement sur le disk)
#include "GameDebug.h"
#include <cmath>            // for sqrt() and pow()
#ifdef BORLAND
#include "DirectLib.h"
#include <vcl\Classes.hpp>
#include <stdio.h>
#endif
#include "consts.h"
#include "DDlib.h"			// Header 
#pragma hdrstop

//#define DDLIB_DEBUG

#define BYTE	unsigned char
#define LONG	int
#define WORD	unsigned short int
#define DWORD   unsigned long


//------------------------------------------------------------------------ define
// Déclaration des variables, classes et autres structures
// Les Constantes de MON programme
#define FULLSCREEN			FALSE
#define HARDWARE			TRUE
#define MODEX				FALSE
#define HARDWARECLIPPING	FALSE

//** DONNEES TEMPORAIRES *******************************************************
// Données géométriques d'un trapèze
const int ISO_HEIGHT_2 	     = 21-1,
		  ISO_WIDTH_2  		 = 39-1,
    	  ISO_HEIGHT_HALF_2  = 10,
    	  ISO_WIDTH_HALF_2   = 19,
          ISO_DENOMINATEUR_2 = 2 * ISO_HEIGHT_HALF_2 * ISO_WIDTH_HALF_2;
// Type de tracé du trapèze
const int TrapTYPE_SIMPLE_2  = 0,
	  TrapTYPE_DOUBLE_2  = 1;
//******************************************************************************

TDirectScreen  *DS = NULL;
// par défaut
int SCREEN_WIDTH = 0,SCREEN_HEIGHT = 0, SCREEN_BITS   = 16;
bool SCREEN_FULLSCREEN = FULLSCREEN;

////////////////////////////////////////////////////////////////////////////////
// Fonctions & Déclarations annexes                                                          //
////////////////////////////////////////////////////////////////////////////////

// Permet de coder une couleur sur 24 bits (8:8:8) ---------------------------//
typedef struct
{   // tous codés sur 8 bits  => max de 255
    unsigned long Blue;
    unsigned long Green;
    unsigned long Red;
} tTrueColor;


 // Convertit une couleur (R,G,B) sur 32*3bits au format 16 bits 5:6:5
    unsigned long RGB16(unsigned long Red,unsigned long Green,unsigned long Blue);
    unsigned long RGB16(tHighColor Coul);

 // Convertit une couleur (R,G,B) 24 bits (8:8:8) en (R,G,B) 16 bits (5:6:5)
    tHighColor RGB32to16(unsigned long Red,unsigned long Green,unsigned long Blue);
    tHighColor RGB32to16(tTrueColor Coul);

 // S'occupe d'interrompre le programme si problèmes
    void MessageError(char *from,char *Erreur,int Gravite);
    void TraceErrorDD(HRESULT hErr,int Gravite);

 // Fonctions externes --------------------------------------------------------//
extern "C"
{       // DDUtil2.h
        extern HRESULT DDReLoadBitmap2(IDirectDrawSurface3 *pdds, LPCSTR szBitmap);
}




////////////////////////////////////////////////////////////////////////////////
// Fonctions Objet : DDSURFACE                                                //
////////////////////////////////////////////////////////////////////////////////

// Constructeur vide ---------------------------------------------------------//
DDSurface::DDSurface(void)
{  };

DDSurface::DDSurface(int Width, int Height)
{
  pDDSurf = NULL;
  SurfDC  = NULL;
  SurfFont = NULL;
  PixWidth = Width;
  PixHeight = Height;

// Assigne les différents paramètres de la surface
  SrcRect.top = DestRect.top = 0;
  SrcRect.left = DestRect.left = 0;
  SrcRect.bottom = DestRect.bottom = Height;
  SrcRect.right = DestRect.right = Width;
  ColKey1=ColKey2=0;
  isAllocateTexture = false;
  isTextureDirect3D = false;
};

// Constructeur --------------------------------------------------------------//
DDSurface::DDSurface(TDirectScreen *pScreen, int Width, int Height,SURFACEMEM surfMem)
{
    pDDSurf = NULL;
    SurfDC  = NULL;
    PixWidth = Width;
    PixHeight = Height;
    SurfFont = NULL;
    isAllocateTexture = false;
    isTextureDirect3D = false;

    AllocateMemory(pScreen,surfMem);

    // Assigne les différents paramètres de la surface
    SrcRect.top = DestRect.top = 0;
    SrcRect.left = DestRect.left = 0;
    SrcRect.bottom = DestRect.bottom = Height;
    SrcRect.right = DestRect.right = Width;

    ColKey1=ColKey2=0;
}
//----------------------------------------------------------------------------//


// Destructeur ---------------------------------------------------------------//
DDSurface::~DDSurface()
{
    if (pDDSurf!=NULL) pDDSurf->Release();
    pDDSurf=NULL;
};

//=== Allloue de la mémoire pour la DirectDrawSurface ========================//
//-- 1ère version pour jeu --
void DDSurface::AllocateMemory(TDirectScreen *pScreen, SURFACEMEM surfMem)
{   HRESULT hr;
    memset(&DDSurfDesc,0,sizeof(DDSurfDesc));  // met tout à zéro
    DDSurfDesc.dwSize=sizeof(DDSurfDesc);      // taille de sa propre structure
    DDSurfDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    switch (surfMem)
    { case (VIDEO_MEMORY)   : DDSurfDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY; break;
      case (SYSTEM_MEMORY)  : DDSurfDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY; break;
      case (TEXTURE_MEMORY) :
        {  // Lorsque la surface va être utilisée comme texture par Direct3D
           DDSurfDesc.ddsCaps.dwCaps = DDSCAPS_TEXTURE/* | DDSCAPS_ALLOCONLOAD*/;
		   DDSurfDesc.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
           isAllocateTexture = false;
           isTextureDirect3D = true;
        }
    };

    DDSurfDesc.dwWidth = PixWidth;
    DDSurfDesc.dwHeight = PixHeight;

    // Crée la surface IDirectDrawSurface
    // <=>  hr = pScreen->pDD->CreateSurface(&DDSurfDesc,(IDirectDrawSurface **) &pDDSurf,NULL);
    LPDIRECTDRAWSURFACE4 lpSurf;
    hr = pScreen->pDD->CreateSurface(&DDSurfDesc,&lpSurf,NULL);
    if (hr==DDERR_OUTOFVIDEOMEMORY)
    { // Il ne doit pas y avoir de problèmes ici pour l'allocation des textures
      DDSurfDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
      hr = pScreen->pDD->CreateSurface(&DDSurfDesc,&lpSurf,NULL);
    };
    if FAILED(hr) 
	{	TraceErrorDD(hr,99);
		_asm int 3;
	}
    hr = lpSurf->QueryInterface(IID_IDirectDrawSurface4, (LPVOID*) &pDDSurf);
    if FAILED(hr) TraceErrorDD(hr,99);
    lpSurf->Release();
}
//-- 2nde version pour éditeur --
void DDSurface::AllocateMemory(LPDIRECTDRAW4 *pDD2, SURFACEMEM surfMem)
{   HRESULT hr;
    memset(&DDSurfDesc,0,sizeof(DDSurfDesc));  // met tout à zéro
    DDSurfDesc.dwSize=sizeof(DDSurfDesc);      // taille de sa propre structure
    DDSurfDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    switch (surfMem)
    {
      case (VIDEO_MEMORY)  : DDSurfDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY; break;
      case (SYSTEM_MEMORY) : DDSurfDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY; break;
      case (TEXTURE_MEMORY) :
        {  // Lorsque la surface va être utilisée comme texture par Direct3D
           DDSurfDesc.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_ALLOCONLOAD;
           isAllocateTexture = false;
           isTextureDirect3D = true;
        }
    };
    DDSurfDesc.dwWidth = PixWidth;
    DDSurfDesc.dwHeight = PixHeight;

    // Crée la surface IDirectDrawSurface
    // <=>  hr = pScreen->pDD->CreateSurface(&DDSurfDesc,(IDirectDrawSurface **) &pDDSurf,NULL);
    LPDIRECTDRAWSURFACE4 lpSurf;
    hr = (*pDD2)->CreateSurface(&DDSurfDesc,&lpSurf,NULL);
    if (hr==DDERR_OUTOFVIDEOMEMORY)
      {     // Il ne doit pas y avoir de problèmes ici pour l'allocation des textures
            DDSurfDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
            hr = (*pDD2)->CreateSurface(&DDSurfDesc,&lpSurf,NULL);
      };
    if FAILED(hr) TraceErrorDD(hr,99);

    hr = lpSurf->QueryInterface(IID_IDirectDrawSurface4, (LPVOID*) &pDDSurf);
    if FAILED(hr) TraceErrorDD(hr,99);
    lpSurf->Release();
}
//----------------------------------------------------------------------------//

// Vérifie l'état de la surface ----------------------------------------------//
//   Procédure restaurant les surfaces perdues dans certaines circonstances à
// cause du GDI de Windows. Elle regarde si la mémoire associée aux surfaces
// est perdue et la récupère si nécessaire.
BOOL DDSurface::CheckSurfaces()
{	
	HRESULT hr;
	if (pDDSurf!=NULL)
	{	switch(pDDSurf->IsLost())
        {
          case DD_OK : return TRUE;  // it's all right ...
          // Si elle est perdue, on la restaure gentillement
          case DDERR_SURFACELOST : hr = pDDSurf->Restore(); _asm int 3; return FALSE;
          // le matériel est occupé et que l'on devra répéter la requête ultérieurement
          case DDERR_WASSTILLDRAWING : return FALSE;
          // Erreur non définie ? peut-être arrêter l'exécution ?
          default : return FALSE;
        }
	}
	return TRUE;
}
//----------------------------------------------------------------------------//

//=== Restaure une surface perdue ============================================//
bool DDSurface::Restaure()
{  return (!FAILED(pDDSurf->Restore()));
}
//----------------------------------------------------------------------------//

// Copie la surface dans une autre -------------------------------------------//
void DDSurface::Draw(DDSurface* pDDSurfDest)
{
    HRESULT hr;
    hr = pDDSurfDest->pDDSurf->Blt(&DestRect,pDDSurf,&SrcRect,DDBLT_WAIT | DDBLT_KEYSRC,NULL);
    // peut-être une surface perdue ? qui sait ?
    if FAILED(hr) if (!CheckSurfaces()) TraceErrorDD(hr,99);
}
//----------------------------------------------------------------------------//

// Copie la surface dans une autre -------------------------------------------//
void DDSurface::DrawNoKey(DDSurface* pDDSurfDest)
{
    HRESULT hr;
    hr = pDDSurfDest->pDDSurf->Blt(&DestRect,pDDSurf,&SrcRect,DDBLT_WAIT,NULL);
    // peut-être une surface perdue ? qui sait ?
    if FAILED(hr) if (!CheckSurfaces()) TraceErrorDD(hr,99);
}
//----------------------------------------------------------------------------//

void DDSurface::DrawNoKeyXY(DDSurface* pDDSurfDest, int X, int Y)
{
    HRESULT hr;
    RECT temp;
    temp = DestRect;
    DestRect.top+=Y;
    DestRect.bottom+=Y;
    DestRect.right+=X;
    DestRect.left+=X;

    hr = pDDSurfDest->pDDSurf->Blt(&DestRect,pDDSurf,&SrcRect,DDBLT_WAIT,NULL);
    DestRect = temp;
    // peut-être une surface perdue ? qui sait ?
    if FAILED(hr) if (!CheckSurfaces()) TraceErrorDD(hr,99);
}
//----------------------------------------------------------------------------//


// Copie + rapidement encore (10%) -------------------------------------------//
void DDSurface::DrawFast(DDSurface* pDDSurfDest,int PosX,int PosY)
{
    HRESULT hr;
    hr = pDDSurfDest->pDDSurf->BltFast(PosX,PosY,pDDSurf,&SrcRect,DDBLTFAST_WAIT);
    if FAILED(hr) if (!CheckSurfaces()) TraceErrorDD(hr,99);
}
//----------------------------------------------------------------------------//

// Copie + rapidement encore (10%) + Color Key--------------------------------//
void DDSurface::DrawFastCK(DDSurface* pDDSurfDest,int PosX,int PosY)
{
    HRESULT hr;
    hr = pDDSurfDest->pDDSurf->BltFast(PosX,PosY,pDDSurf,&SrcRect,DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY );
    if FAILED(hr) if (!CheckSurfaces()) TraceErrorDD(hr,99);
}
//----------------------------------------------------------------------------//


// Obtient un pointer vers la surface ----------------------------------------//
// => à utiliser lorsque l'on veut manipuler DDSurfDesc.lpSurface
void DDSurface::Lock()
{
    HRESULT hr;
    hr = pDDSurf->Lock(NULL,&DDSurfDesc,DDLOCK_WAIT,NULL);
    if FAILED(hr) TraceErrorDD(hr,99);
}
//----------------------------------------------------------------------------//


// Obtient un pointer vers la surface ----------------------------------------//
// => à utiliser lorsque l'on veut manipuler DDSurfDesc.lpSurface
void DDSurface::Lock2()
{   HRESULT hr;
    ZeroMemory(&DDSurfDesc, sizeof(DDSurfDesc));
    DDSurfDesc.dwSize = sizeof(DDSurfDesc);
    hr = pDDSurf->Lock(NULL, &DDSurfDesc, DDLOCK_SURFACEMEMORYPTR|DDLOCK_WAIT, NULL);
    if FAILED(hr) TraceErrorDD(hr,99);
}
//----------------------------------------------------------------------------//

// Avertit DirectX lorsque les manips avec la surface sont finies ------------//
void DDSurface::UnLock()
{
    HRESULT hr;
    // hr = pDDSurf->Unlock(DDSurfDesc.lpSurface);
    hr = pDDSurf->Unlock(NULL);
    if FAILED(hr) TraceErrorDD(hr,99);
}
//----------------------------------------------------------------------------//

// Affiche un pixel ----------------------------------------------------------//
//   la variable Col doit être au format du pixel 8,16,24 ou 32 bits
void DDSurface::PutPixel(int X,int Y,unsigned int Col)
{	// Vérifie qd même si les points ne sortent pas de la surface
    if ((X<0) || (Y<0) || (X>=PixWidth) || (Y>=PixHeight)) return;

    static DWORD *Map32 = (DWORD*) DDSurfDesc.lpSurface;
    static WORD  *Map16 = (WORD*)  DDSurfDesc.lpSurface;
    static BYTE  *Map8  = (BYTE*)  DDSurfDesc.lpSurface;
    switch (DS->Screen_Bits)
    {
        case 32 : Map32[Y*DDSurfDesc.lPitch/4+X] = (WORD) Col; break;
        case 16 : Map16[Y*DDSurfDesc.lPitch/2+X] = (WORD) Col; break;
        case  8 : Map8[Y*DDSurfDesc.lPitch+X] = (BYTE) Col; break;
    }
}
//----------------------------------------------------------------------------//

// Choppe un pixel -----------------------------------------------------------//
// renvoit la valeur trouvée sans la convertir
unsigned int DDSurface::GetPixel(int X,int Y)
{
    static WORD *Map16 = (WORD*) DDSurfDesc.lpSurface;
    static BYTE *Map8  = (BYTE*) DDSurfDesc.lpSurface;
    switch (DS->Screen_Bits)
    {
        case 16 : return Map16[Y*DDSurfDesc.lPitch+X];
        case  8 : return Map8[Y*DDSurfDesc.lPitch+X];
    }
    return 0;  // Si inconnu
}
//----------------------------------------------------------------------------//

// Intervalle de couleur non prise en compte par le blitting -----------------//
// c1 et c2 peuvent tout aussi bien représenter des indices de palettes (8 bits)
// ou une valeur (R,V,B) codée sur 16 ou 24 bits
void DDSurface::ColorKey(DWORD c1,DWORD c2)
{
    DDCOLORKEY ddCK;

    ColKey1=c1; ColKey2=c2;
    ddCK.dwColorSpaceLowValue  = c1;
    ddCK.dwColorSpaceHighValue = c2;
    pDDSurf->SetColorKey(DDCKEY_SRCBLT,&ddCK);

};
//----------------------------------------------------------------------------//

// Affiche un texte sur la surface en (X,Y) ----------------------------------//
void DDSurface::TextSurf(int X,int Y,COLORREF Col,LPCTSTR Texte)
{
    HRESULT hr;
    hr = pDDSurf->GetDC(&SurfDC);
    if FAILED(hr) TraceErrorDD(hr,99);

    SetBkMode(SurfDC,TRANSPARENT);
    SetTextColor(SurfDC,Col);
    TextOut(SurfDC,X,Y,Texte,lstrlen(Texte));

    pDDSurf->ReleaseDC(SurfDC);
}
//----------------------------------------------------------------------------//

// Change la police de caractère de la surface -------------------------------//
void DDSurface::ChangeFont(const char* Nom,int Width,int Height,int Attributes)
{
    SurfFont = CreateFont(Height,Width,0,0,Attributes,FALSE,FALSE,FALSE,
        ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,NONANTIALIASED_QUALITY,
        VARIABLE_PITCH,Nom);

    SelectObject(SurfDC,SurfFont);
}
//----------------------------------------------------------------------------//

// Charge un BITMAP ----------------------------------------------------------//
void DDSurface::LoadBitmap2Surf(const char* BitmapName)
{
	if (BitmapName==NULL) return;
    if (DDReLoadBitmap(pDDSurf,BitmapName)!=DD_OK) return;
};
//----------------------------------------------------------------------------//

// Efface l'écran d'une couleur spécifique -----------------------------------//
void DDSurface::Fill(unsigned long ColFill)
{
    DDBLTFX ddBltFx;

    ddBltFx.dwSize = sizeof(DDBLTFX);
    ddBltFx.dwFillColor = ColFill;
    HRESULT hr = pDDSurf->Blt(NULL,NULL,NULL,DDBLT_WAIT | DDBLT_COLORFILL,&ddBltFx);
    if FAILED(hr) if (!CheckSurfaces()) TraceErrorDD(hr,99);

}
//----------------------------------------------------------------------------//

// échange 2 integers  (utilisé par l'algo de lignes)
void Swap(int *a, int*b)
{   int Temp = *a;
    *a = *b;
    *b = Temp;
}


//== Trace une ligne sur l'écran par interpolation ===========================//
void DDSurface::Line(int X1, int Y1, int X2, int Y2, int Col)
{
	double xStep, yStep, X, Y;
    int xLength, yLength, xCount, yCount;
    xLength = abs(X2-X1);  yLength = abs(Y2-Y1);

    Lock2();		// lock la surface pdt qu'on travaille dessus
    if (xLength > yLength)
     {
     	if (X1>X2) { Swap(&X1, &X2); Swap(&Y1, &Y2);  }
        yStep = (double) (Y2 - Y1) / (double) (X2 - X1);
        Y = Y1;
        for (xCount = X1; xCount <= X2; xCount++)
        {
            if ((xCount>0) && (xCount<PixWidth) && (Y>0) && (Y<PixHeight))
                PutPixel(xCount,(int) Y, Col);
            Y += yStep;
         };
      }
     else
      {
        if (Y1>Y2) { Swap(&X1, &X2); Swap(&Y1, &Y2);  }
        xStep = (double) (X2 - X1) / (double) (Y2 - Y1);
        X = X1;
        for (yCount = Y1; yCount <= Y2; yCount++)
        {
            if ((yCount>0) && (yCount<PixHeight) && (X>0) && (X<PixWidth))
        		PutPixel((int) X, yCount, Col);
            X += xStep;
         };
      };
    UnLock();
};
//----------------------------------------------------------------------------//

//=== Trace une croix voyante sur la surface =================================//
void DDSurface::PutCroix(int X1, int Y1, int largeur, int longueur, int Col)
{   for (int i= - largeur/2 ; i <= +largeur/2 ; i++)
    {  Line(X1+i, Y1-longueur/2, X1+i, Y1+longueur/2, Col);	// Barre verticale
       Line(X1-longueur/2, Y1-i, X1+longueur/2, Y1+i, Col);	// Barre horizontale
    }
}
//----------------------------------------------------------------------------//

//=== Get back the pixedl format of this surface =============================//
int	DDSurface::getPixelFormat(LPDDPIXELFORMAT lpDDPixelFormat)
{	DDSURFACEDESC2 ddsd;
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_PIXELFORMAT;
	HRESULT hr = pDDSurf->GetSurfaceDesc(&ddsd);
	if (hr != DD_OK) return (int) hr;
	*lpDDPixelFormat = ddsd.ddpfPixelFormat;
	return 0;
	//return (int)pDDSurf->GetPixelFormat(lpDDPixelFormat);
}
//----------------------------------------------------------------------------//


////////////////////////////////////////////////////////////////////////////////
// Fonctions Objet : TDIRECTSCREEN                                            //
////////////////////////////////////////////////////////////////////////////////


// Constructeur --------------------------------------------------------------//
TDirectScreen::TDirectScreen(HWND Handle)
{
    pDD = NULL;
    pDDSPrimary = NULL;
    pDDSBack = NULL;
    pDDPal = NULL;
    pDDClipper = NULL;
    pDDGammaControl = NULL;

    FullScreen = SCREEN_FULLSCREEN;
    ModeX = MODEX;
    Hardware = HARDWARE;
    HardwareClipping = HARDWARECLIPPING;
    Screen_Height = SCREEN_HEIGHT;
    Screen_Width =  SCREEN_WIDTH;
    Screen_Bits = SCREEN_BITS;

    DDHandle = Handle;
	GetWindowRect(DDHandle, &g_rcWindow);

    RedCoeff = GreenCoeff = BlueCoeff = 1.0; // Coefficients servant à calculer les composantes de la table
	pixelFormat = NULL;
}
//----------------------------------------------------------------------------//

//  A utiliser lorsque l'on ouvre une session sous DirectDraw ----------------//
void TDirectScreen::InitDDraw()
{
    HRESULT					hr;		// Résultat des fonctions COM DirectX	
	DDSURFACEDESC2			ddSD;	// Structure permettant la description de la surface souhaitée
	LPDIRECTDRAW			lpDD;	// Ptr sur un objet COM DirectDraw	
	LPDIRECTDRAWSURFACE4	lpSurf; // Surface DirectDraw
    
	// Crée l'objet COM DirectDraw
    // <=> if (FAILED(hr=DirectDrawCreate(NULL,(IDirectDraw **) &pDD,NULL))) TraceErrorDD(hr,99);
    hr = DirectDrawCreate(NULL,&lpDD,NULL);
    if FAILED(hr) TraceErrorDD(hr,99);

    hr = lpDD->QueryInterface(IID_IDirectDraw4,(LPVOID*) &pDD);
    if FAILED(hr) TraceErrorDD(hr,99);
    lpDD->Release();

	
    // Défini un niveau de coopération exclusif afin de pouvoir changer le mode d'affichage
    if (FullScreen)
    {
        if (!ModeX)
		{	
			hr = pDD->SetCooperativeLevel(DDHandle,DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT);
		}
        else
		{	hr = pDD->SetCooperativeLevel(DDHandle,DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX | DDSCL_ALLOWREBOOT);
		}
        if (FAILED(hr)) TraceErrorDD(hr,99);  // s'arrête si windows ne veut coopérer

		// Change la résolution en SCREEN_WIDTH x SCREEN_HEIGHT x SCREEN_BITS  full-screen mode
        hr = pDD->SetDisplayMode(Screen_Width, Screen_Height, Screen_Bits, 0, 0);
        if (FAILED(hr)) TraceErrorDD(hr,99);  // ne peut pas non plus continuer

		// Récupère les dimensions de la fenêtre du jeu
    	SetRect(&g_rcViewport, 0, 0, Screen_Width, Screen_Height);
    	memcpy(&g_rcScreen, &g_rcViewport, sizeof(RECT));

		// Crée une surface complexe permettant le basculement entre 2 pages
		memset(&ddSD,0,sizeof(ddSD));  // met tout à zéro
		ddSD.dwSize = sizeof(ddSD);    // taille de sa propre strucutre
		ddSD.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		if (Hardware) ddSD.ddsCaps.dwCaps = DDSCAPS_COMPLEX | DDSCAPS_FLIP | DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE;
		else  ddSD.ddsCaps.dwCaps = DDSCAPS_COMPLEX | DDSCAPS_FLIP | DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE/* | DDSCAPS_SYSTEMMEMORY*/;
		ddSD.dwBackBufferCount = 1;

		// Création des 2 tampons ds la surface complex
		// Crée la surface IDirectDrawSurface
		// <=>  hr = pScreen->pDD->CreateSurface(&DDSurfDesc,(IDirectDrawSurface **) &pDDSurf,NULL);
		pDDSPrimary = new DDSurface(Screen_Width,Screen_Height);
		pDDSBack    = new DDSurface(Screen_Width,Screen_Height);
		 // <=> hr = pDD->CreateSurface(&ddSD,(IDirectDrawSurface **) &pDDSPrimary->pDDSurf,NULL);
		hr = pDD->CreateSurface(&ddSD,&lpSurf,NULL);
		if (FAILED(hr)) TraceErrorDD(hr,99);
		hr = lpSurf->QueryInterface(IID_IDirectDrawSurface4, (LPVOID*)&pDDSPrimary->pDDSurf);
		if (FAILED(hr)) TraceErrorDD(hr,99);
		lpSurf->Release();

		ddSD.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER | DDSCAPS_3DDEVICE;
		hr = pDDSPrimary->pDDSurf->GetAttachedSurface(&ddSD.ddsCaps,&pDDSBack->pDDSurf);
		if (FAILED(hr)) TraceErrorDD(hr,99);
    }
    else  // Windowed-mode : on ne change pas de résolution et l'on reste en coopération normale avec Windows
    {
		hr = pDD->SetCooperativeLevel(DDHandle,DDSCL_NORMAL);
		if (FAILED(hr)) TraceErrorDD(hr,99);  // s'arrête si windows ne veut coopérer (le méchant !)

		// Récupère les dimensions de la fenêtre du jeu ainsi que son positionnement à l'écran
    	GetClientRect(DDHandle, &g_rcViewport);
    	GetClientRect(DDHandle, &g_rcScreen);
    	ClientToScreen(DDHandle, (POINT*)&g_rcScreen.left);
    	ClientToScreen(DDHandle, (POINT*)&g_rcScreen.right);

		// Préparation des futures surfaces principales & secondaires
		pDDSPrimary = new DDSurface(Screen_Width,Screen_Height);
		pDDSBack    = new DDSurface(Screen_Width,Screen_Height);

		// Création de la surface principale
        ZeroMemory(&ddSD,sizeof(ddSD));
        ddSD.dwSize = sizeof(ddSD);
        ddSD.dwFlags = DDSD_CAPS;
        ddSD.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE;
        hr = pDD->CreateSurface(&ddSD, &lpSurf, NULL);
		if (FAILED(hr)) TraceErrorDD(hr,99);

		// Création d'un objet de clipping pour la fenêtre windows
		hr = pDD->CreateClipper(0, &pDDClipper, NULL);
		if (FAILED(hr))	{  TraceErrorDD(hr,99); pDDClipper = NULL; }

		// On associe le clipper à la fenêtre du jeu
        pDDClipper->SetHWnd(0, DDHandle);
        lpSurf->SetClipper(pDDClipper);
        pDDClipper->Release();
        pDDClipper = NULL;
        
		// Cette surface deviendra donc la surface principale de ZeGame
		hr = lpSurf->QueryInterface(IID_IDirectDrawSurface4, (LPVOID*)&pDDSPrimary->pDDSurf);
		if (FAILED(hr)) TraceErrorDD(hr,99);
		lpSurf->Release();

		// Création de la surface secondaire 
		ddSD.dwFlags        = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
        ddSD.dwWidth        = Screen_Width;
        ddSD.dwHeight       = Screen_Height;
        ddSD.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;
        hr = pDD->CreateSurface(&ddSD, &pDDSBack->pDDSurf, NULL);
    }

	// Recupere des informations sur le format des pixels de la surface principale
	setPixelFormat(pDDSPrimary);
	

    // S'occupe du clipping Hardware de la surface complexe si voulu
    if (HardwareClipping)
     {
    	// Crée le cadre de clipping
    	hr = pDD->CreateClipper(0,&pDDClipper,NULL);
		if (FAILED(hr))	{  TraceErrorDD(hr,99); pDDClipper = NULL; }
    	// Assigne le clipper à la fenêtre
    	hr = pDDClipper->SetHWnd(0, DDHandle);
    	if (FAILED(hr))  // on abandonne le clipping !
    	{  	pDDClipper->Release();
			pDDClipper = NULL;
        	TraceErrorDD(hr,99);
			_asm int 3;
    	}
    	// zone de clipping de la surface principale
    	hr = pDDSPrimary->pDDSurf->SetClipper(pDDClipper);
    	if (FAILED(hr))
    	{   pDDClipper->Release();
			pDDClipper = NULL;	
			TraceErrorDD(hr,99);  
			_asm int 3;
		}
 		// zone de clipping de la surface secondaire
    	hr = pDDSBack->pDDSurf->SetClipper(pDDClipper);
    	if (FAILED(hr))
    	{   // plus besoin du clipper
			pDDClipper->Release();
			pDDClipper = NULL;
			TraceErrorDD(hr,99);  
			_asm int 3;
		}
    		
		
     };  // fin du clipping harware

    // Initialise le Gamma Control sur la surface principale
    hr = pDDSPrimary->pDDSurf->QueryInterface(IID_IDirectDrawGammaControl ,(LPVOID*) &pDDGammaControl);
    if FAILED(hr) TraceErrorDD(hr,99);

}
//----------------------------------------------------------------------------//

// Destructeur ---------------------------------------------------------------//
TDirectScreen::~TDirectScreen(void)
{
    if (pDD == NULL) return;
    
	// Restaure la résolution courante
	pDD->RestoreDisplayMode();

	// Libère les 2 surfaces DirectDraw
	if (pDDSBack!=NULL) { delete pDDSBack; pDDSBack = NULL; };
	if (pDDSPrimary!=NULL)  { delete pDDSPrimary; pDDSPrimary = NULL; };
	
	// Libère la palette
	if (pDDPal!=NULL) { pDDPal->Release(); pDDPal = NULL; };

	// Libère le clipper
	if (pDDClipper!=NULL) { pDDClipper->Release(); pDDClipper = NULL; };

	// Désalloue les objets liés au GammaControl
	if (pDDGammaControl!=NULL)
	{	pDDGammaControl->Release();	
		pDDGammaControl = NULL; 
	}	

	// Relâche l'objet DirectDraw
	pDD->Release(); pDD = NULL;
}
//----------------------------------------------------------------------------//

// Charge un Bitmap ds le Back Buffer ----------------------------------------//
BOOL TDirectScreen::LoadBitmap(const char* BitmapName)
{	
	#ifndef NEW_FILE_MANAGER
    if (BitmapName==NULL) return FALSE;
    if (DDReLoadBitmap(pDDSBack->pDDSurf,BitmapName)!=DD_OK) return FALSE;
	#else
	TFileVirtual *fichier = fopenFV((char*) BitmapName);
	// [A REVOIR] Ici, on est pour l'instant obligé de créé une surface directdraw
	// et de la copier dans la surface secondaire car je ne sais pas directement 
	// charger une image en mémoire vidéo
	IDirectDrawSurface4* surface = DDLoadAnyBitmapFromFile(DS->pDD, fichier, 0, 0);
	DS->pDDSBack->pDDSurf->Blt(NULL, surface, NULL, DDBLT_WAIT, NULL);
	surface->Release();
	fcloseFV(fichier);
	#endif // NEW_FILE_MANAGER
	
	return TRUE;
}
//----------------------------------------------------------------------------//

// Charge la Palette d'un bmp ------------------------------------------------//
BOOL TDirectScreen::LoadPalette(const char* BitmapName)
{
    if (BitmapName==NULL) return FALSE;
    pDDPal = DDLoadPalette(pDD,BitmapName);
    if (pDDPal==NULL) return FALSE;
    pDDSPrimary->pDDSurf->SetPalette(pDDPal);
    return TRUE;
}
//----------------------------------------------------------------------------//

// Flip les surfaces Principales/Backbuffer ----------------------------------//
HRESULT TDirectScreen::FlipSurfaces()
{
    HRESULT hr = 0;
    
    if (pDDSPrimary->CheckSurfaces())  // s'assure que les surfaces principales et secondaires ne soient pas perdues
    {
      if (!(FullScreen))
      { 
		  // On Blit les 2 surfaces en windowed-mode
        /*
		RECT rcSrc;    // variables utilisées en windowed-mode;
		RECT rcDest;
		POINT p;
		p.x = p.y = 0; // regarde où la fenêtre se trouve sur la surface primaire
        ClientToScreen(DDHandle,&p);
        GetClientRect(DDHandle,&rcDest);
        OffsetRect(&rcSrc,p.x,p.y);
        SetRect(&rcSrc,0,0,Screen_Width,Screen_Width);
        hr = pDDSPrimary->pDDSurf->Blt(&rcDest,pDDSBack->pDDSurf,&rcSrc,DDBLT_WAIT,NULL);*/
		hr = pDDSPrimary->pDDSurf->Blt(&g_rcScreen, pDDSBack->pDDSurf, &g_rcViewport, DDBLT_WAIT, NULL);
        if FAILED(hr) if (!pDDSPrimary->CheckSurfaces()) TraceErrorDD(hr,99);
      }
      else
      { 
		// Bascule automatiquement sur la surface d'arrière-plan (synchronisé)
        hr = pDDSPrimary->pDDSurf->Flip(NULL, DDFLIP_WAIT);
      }
      // Faut-il appeler TraceErrorDD sachant qu'il ne fera rien
      //    if (FAILED(hr)) TraceErrorDD(hr,1);
      //   => NON (for the moment)
    }
    else { };
    return hr;
 }
//----------------------------------------------------------------------------//

// Efface l'écran d'une couleur spécifique -----------------------------------//
void TDirectScreen::Fill(DWORD ColFill)
{
    DDBLTFX ddBltFx;

    ddBltFx.dwSize = sizeof(DDBLTFX);
    ddBltFx.dwFillColor = ColFill;
    HRESULT hr = pDDSBack->pDDSurf->Blt(NULL,NULL,NULL,DDBLT_WAIT | DDBLT_COLORFILL,&ddBltFx);
    if FAILED(hr) if (!pDDSBack->CheckSurfaces()) TraceErrorDD(hr,99);
}
//----------------------------------------------------------------------------//


//== Trace une ligne sur l'écran par interpolation ===========================//
void TDirectScreen::BackLine(int X1, int Y1, int X2, int Y2, int Col)
{
    double xStep, yStep, X, Y;
    int xLength, yLength, xCount, yCount;
    xLength = abs(X2-X1);  yLength = abs(Y2-Y1);

    pDDSBack->Lock2();
    if (xLength > yLength)
     {
     	if (X1>X2) { Swap(&X1, &X2); Swap(&Y1, &Y2);  }
        yStep = (double) (Y2 - Y1) / (double) (X2 - X1);
        Y = Y1;
        for (xCount = X1; xCount <= X2; xCount++)
        {
            if ((xCount>0) && (xCount < Screen_Width) && (Y>0) && (Y < Screen_Height))
                DS->pDDSBack->PutPixel(xCount,(int) Y, Col);
            Y += yStep;
         };
      }
     else
      {
        if (Y1>Y2) { Swap(&X1, &X2); Swap(&Y1, &Y2);  }
        xStep = (double) (X2 - X1) / (double) (Y2 - Y1);
        X = X1;
        for (yCount = Y1; yCount <= Y2; yCount++)
        {
            if ((yCount>0) && (yCount<Screen_Height) && (X>0) && (X<Screen_Width))
        		DS->pDDSBack->PutPixel((int) X, yCount, Col);
            X += xStep;
         };
      };
    pDDSBack->UnLock();
};
//----------------------------------------------------------------------------//

//== Trace un trapèze d'aprés les données ISO ================================//
void TDirectScreen::BackISOTrapeze(int X, int Y, int Col, int Type)
{
         int X2 = X-ISO_WIDTH_HALF_2;
         int Y2 = Y+ISO_HEIGHT_HALF_2;
         BackLine(X, Y, X2, Y2, Col);
         if (Type==TrapTYPE_DOUBLE_2) BackLine(X, Y+1, X2, Y2+1, Col);
         X = X2; Y = Y2;
         X2+=ISO_WIDTH_HALF_2;
         Y2+=ISO_HEIGHT_HALF_2;
         BackLine(X, Y, X2, Y2, Col);
         if (Type==TrapTYPE_DOUBLE_2) BackLine(X, Y+1, X2, Y2+1, Col);
         X = X2; Y = Y2;
         X2+=ISO_WIDTH_HALF_2;
         Y2-=ISO_HEIGHT_HALF_2;
         BackLine(X, Y, X2, Y2, Col);
         if (Type==TrapTYPE_DOUBLE_2) BackLine(X, Y+1, X2, Y2+1, Col);
         X = X2; Y = Y2;
         X2-=ISO_WIDTH_HALF_2;
         Y2-=ISO_HEIGHT_HALF_2;
         BackLine(X, Y, X2, Y2, Col);
         if (Type==TrapTYPE_DOUBLE_2) BackLine(X, Y+1, X2, Y2+1, Col);
};
//----------------------------------------------------------------------------//

//=== Modifie la table des couleurs ==========================================//
void TDirectScreen::ChangeGammaRamp()
{   // Applique la Gamma table à l'écran
    pDDGammaControl->SetGammaRamp(0,&lpGammaRamp);
}
//----------------------------------------------------------------------------//

//=== Assigne les coefficients à appliquer aux composantes de couleur ========//
void TDirectScreen::SetGammaRamp(double& Rcoeff, double& Vcoeff, double& Bcoeff)
  { RedCoeff = Rcoeff;
    BlueCoeff = Bcoeff;
    GreenCoeff = Vcoeff;

    // Remplissage de la table
    unsigned int normal, ramp;
    for (int i=0 ; i<256 ; i++)
    {   normal = i*256;
        ramp = (unsigned int) (normal * RedCoeff);
        if (ramp < 65535) lpGammaRamp.red[i]=(WORD) ramp; else lpGammaRamp.red[i] = 65535;
        ramp = (unsigned int) (normal * GreenCoeff);
        if (ramp < 65535) lpGammaRamp.green[i]=(WORD) ramp; else lpGammaRamp.green[i] = 65535;
        ramp = (unsigned int) (normal * BlueCoeff);
        if (ramp < 65535) lpGammaRamp.blue[i]=(WORD) ramp; else lpGammaRamp.blue[i] = 65535;
     }
}
//----------------------------------------------------------------------------//

//=== MaJ de la taille et de la position de la fenêtre =======================//
void TDirectScreen::UpdateWindowPos()
{	GetWindowRect(DDHandle, &g_rcWindow);
    GetClientRect(DDHandle, &g_rcViewport);
    GetClientRect(DDHandle, &g_rcScreen);
    ClientToScreen(DDHandle, (POINT*)&g_rcScreen.left);
    ClientToScreen(DDHandle, (POINT*)&g_rcScreen.right);
}
//----------------------------------------------------------------------------//

//=== Permutation du mode FullScreen <-> Windowed ============================//
void TDirectScreen::ToggleFullScreenMode()
{	
	// Change de Screen-Mode et sauvegarde la position de la fenêtre courante
	if (!FullScreen)
	{	GetWindowRect(DDHandle, &g_rcWindow);
	}
	FullScreen = !FullScreen;

	// On redevient coopératif avec windows
	if (pDD == NULL) return;
	pDD->SetCooperativeLevel(DDHandle, DDSCL_NORMAL);

	
	// Libère les 2 surfaces DirectDraw
	if (pDDSBack!=NULL) { delete pDDSBack; pDDSBack = NULL; };
	if (pDDSPrimary!=NULL)  { delete pDDSPrimary; pDDSPrimary = NULL; };

	// Désalloue les objets liés au GammaControl
	if (pDDGammaControl!=NULL)
	{	pDDGammaControl->Release();	
		pDDGammaControl = NULL; 
	}
	
	// Libère la palette
	//if (pDDPal!=NULL) { pDDPal->Release(); pDDPal = NULL; };
	// Libère le clipper
	//if (pDDClipper!=NULL) { pDDClipper->Release(); pDDClipper = NULL; };

    // Si l'on était en plein écran, on restaure la taille et la position de la fenêtre windows
	if (!FullScreen)
    {	SetWindowPos(DDHandle, HWND_NOTOPMOST, g_rcWindow.left, g_rcWindow.top,
                     (g_rcWindow.right - g_rcWindow.left), 
                     (g_rcWindow.bottom - g_rcWindow.top), SWP_SHOWWINDOW );
    }

	// On réinitialise les surfaces primaires et secondaires
	InitDDraw();		
}
//----------------------------------------------------------------------------//

//=== Set @pixelFormat attribute of the main surfaces of the screen ==========//
void TDirectScreen::setPixelFormat(DDSurface *surf)
{	if (pixelFormat!=NULL)
	{	delete pixelFormat;
		pixelFormat = NULL;
	}

	DDPIXELFORMAT ddPixelFormat;
	surf->getPixelFormat(&ddPixelFormat);
	switch (ddPixelFormat.dwRGBBitCount)
	{	case 8:
			pixelFormat = new TPixelFormatIndexedColor(8);
			break;
		case 16:
		case 24:
		case 32:
			pixelFormat = new TPixelFormatNonIndexedColor(ddPixelFormat.dwRGBBitCount);
			break;
		case 4:
		default:	// Undefined color or not supported color mode
			return;
	}

	//TPixelFormatIndexedColor	*indexed = NULL;
	TPixelFormatNonIndexedColor *nonIndexed = NULL;
	unsigned int maxValue = (unsigned int) pow(2, ddPixelFormat.dwRGBBitCount);
	switch (pixelFormat->dwColorMode)
	{	case PIXEL_FORMAT_INDEXED_COLOR:
			break;
		case PIXEL_FORMAT_NON_INDEXED_COLOR:
			nonIndexed = (TPixelFormatNonIndexedColor*) pixelFormat;
			// Recupere les bitmasks de chacune des 3 composantes
			nonIndexed->dwRedBitMask = ddPixelFormat.dwRBitMask;
			nonIndexed->dwGreenBitMask = ddPixelFormat.dwGBitMask;
			nonIndexed->dwBlueBitMask = ddPixelFormat.dwBBitMask;
			// Puis calcul le nombre de bits pour chaque composante
			for (unsigned int n=1 ; n < maxValue ; n<<=1)
			{	if (ddPixelFormat.dwRBitMask & n) nonIndexed->dwNbRedBit++;
				else if (ddPixelFormat.dwGBitMask & n) nonIndexed->dwNbGreenBit++;
				else if (ddPixelFormat.dwBBitMask & n) nonIndexed->dwNbBlueBit++;
			}
			// Puis on en deduit la position de chaque composante dans le DWORD
			nonIndexed->dwShiftRed = nonIndexed->dwNbGreenBit + nonIndexed->dwNbBlueBit;
			nonIndexed->dwShiftGreen = nonIndexed->dwNbBlueBit;
			nonIndexed->dwShiftBlue = 0;
			break;
	}


}
//----------------------------------------------------------------------------//

//= Transforme RGB(8:8:8) en RGB(5:6:5) ou RGB(5:5:5) suivant la carte video =//
WORD TDirectScreen::RGB24to16(DWORD col)
{   DWORD r = (col & 0x00FF0000) >> 16;
    DWORD g = (col & 0x0000FF00) >> 8;
    DWORD b = (col & 0x000000FF);
    return RGBto16(r,g,b);
}
//----------------------------------------------------------------------------//

//= Transforme R(8)G(8)B(8) en RGB(5:6:5) ou RGB(5:5:5) suivant la carte video//
WORD TDirectScreen::RGBto16(DWORD red, DWORD green,DWORD blue)
{	TPixelFormatNonIndexedColor* nonIndexed = (TPixelFormatNonIndexedColor*) pixelFormat;
	red >>= 3;
	green >>= (8 - nonIndexed->dwNbGreenBit);
	blue >>= 3;
	return (WORD) ((red << nonIndexed->dwShiftRed) | (green << nonIndexed->dwShiftGreen) | (blue << nonIndexed->dwShiftBlue));
}
//----------------------------------------------------------------------------//



////////////////////////////////////////////////////////////////////////////////
// Fonctions & Déclarations annexes                                                          //
////////////////////////////////////////////////////////////////////////////////

// Convertit une couleur (R,G,B) sur 32*3bits au format 16 bits 5:6:5 --------//
unsigned long RGB16(unsigned long Red,unsigned long Green,unsigned long Blue)
{
    static unsigned long Col;

    Col = Red;
    Col<<=6;
    Col |= Green;
    Col<<=5;
    Col |= Blue;
    return Col;
};
unsigned long RGB16(tHighColor Coul) // même chose en utilisant tHichColor
{
    static unsigned long Col;

    Col = Coul.Red;
    Col<<=6;
    Col |= Coul.Green;
    Col<<=5;
    Col |= Coul.Blue;
    return Col;
};

unsigned long RGB24(unsigned long Red,unsigned long Green,unsigned long Blue)
{
    static unsigned long col;
    col = Red;    col<<=8;
    col |= Green; col<<=8;
    col |= Blue;
    return col;
}

//----------------------------------------------------------------------------//

// Convertit une couleur (R,G,B) 24 bits (8:8:8) en (R,G,B) 16 bits (5:6:5)
tHighColor RGB24to16(unsigned long Red,unsigned long Green,unsigned long Blue)
{
    tHighColor Col;
    Col.Red   = Red >> 3;    // 5 bits
    Col.Green = Green >> 2;  // 6 bits
    Col.Blue  = Blue >> 3;    // 5 bits;
    return Col;
};
tHighColor RGB24to16(tTrueColor Coul)
{
    tHighColor Col;
    Col.Red   = Coul.Red >> 3;    // 5 bits
    Col.Green = Coul.Green >> 2;  // 6 bits
    Col.Blue  = Coul.Blue >> 3;   // 5 bits;
    return Col;
}//---------------------------------------------------------------------------//

unsigned long RGBto16(unsigned long Red,unsigned long Green,unsigned long Blue)
{  // ATTENTION = apprement ce n'est pas l'exacte méthode pour convertir les
   // composantes d'une couleur 24 bitsen 16 bits
   return RGB16(Red >>3, Green>>2, Blue>>3);
}

unsigned long RGBto16(unsigned long col) // Transforme (8:8:8) en (5:6:5)
{
    unsigned long r,g,b;
    r = col & 0x00FF0000; r>>=16;
    g = col & 0x0000FF00; g>>=8;
    b = col & 0x000000FF;
    return RGBto16(r,g,b);
}
//----------------------------------------------------------------------------//

/* Procédure utilisée lorsque le programme a planté
   => revient sous windows et affiche le message
    - le paramètre gravité permet de savoir si l'on doit oui ou non refermer l'application
        0  : Aucune information sur le type d'erreur occasionnée
        1  : Erreur passagère => n'affiche rien et laisse le programme continuer;
        2  : Laisse à l'utilisateur le libre de choix de continuer <=> pas trop grave
        99 : Erreur critique => le programme doit s'arrêter
*/
void MessageError(char *from,char *Erreur,int Gravite)
{
    #ifdef DDLIB_DEBUG
    switch (Gravite)
    {
      case 0 : return;   //  ????
      case 1 : return;  // Ca passe ...
      case 2 : if (MessageBoxA(NULL,Erreur,from,1)==IDYES) return;
			   else {
			   #ifdef CBUILDER
			   Application->Terminate();
			   #else
			   _asm int 3;
			   #endif
			   }  break;
      // Erreur critique : message et sort !
      case 99 :	MessageBoxA(NULL,Erreur,from,1); 
				#ifdef CBUILDER
				Application->Terminate();
				#else
				_asm int 3;
				#endif 
				break;
    }
    #endif
};

//----------------------------------------------------------------------------//
void TraceErrorDD(HRESULT hErr,int Gravite)    // Gestion des erreurs de DirectDraw
{
    AnsiString Erreur;

    if ((DS!=NULL) && (DS->pDD!=NULL)) DS->pDD->FlipToGDISurface();
	switch (hErr)
    {
       	case DDERR_ALREADYINITIALIZED	: Erreur="L'objet est déjà initialisé"; break;
        case DDERR_CANNOTATTACHSURFACE	: Erreur="La surface ne peut être attachée à la surface désirée"; break;
        case DDERR_CANNOTDETACHSURFACE	: Erreur="La surface ne peut être désattachée de la surface désirée"; break;
		case DDERR_WRONGMODE			: Erreur="Changement de mode vidéo !"; break;

    	default : Erreur="Erreur inconnue !"; break;
    }
  #ifdef DDLIB_DEBUG
  
  #ifdef CBUILDER
  MessageError("Erreur DirectDraw",Erreur.c_str(),Gravite);
  Application->Terminate();
  #else
  MessageError("Erreur DirectDraw", LPSTR(LPCSTR(Erreur)),Gravite);
  _asm int 3;
  #endif  
  #endif
}






//============================================================================//
//                             GESTION DES SPRITES                            //
//============================================================================//

//==============================  Structures & Variables =====================//


SpriteFrameList *SFL = NULL;  // Liste officielle de la gestion des frames



//========================== Fonctions =======================================//

////////////////////////////////////////////////////////////////////////////////
// Fonction Objet : SPRITEFRAME                                               //
////////////////////////////////////////////////////////////////////////////////
SpriteFrame::SpriteFrame(TDirectScreen *pScreen,int Width,int Height, SURFACEMEM surfMem)
:DDSurface(pScreen,Width,Height, surfMem)
{   NumFrame=0;
    NbSprites=0;
    SF_Suiv = SF_Prec = NULL;
    IDFrame = 0;   // Défini à zéro si pas donné
    BmpAlwaysIntoMemory = true;
    BmpLoadIntoMemory = false;
    Semaphore = 0;
    iStartSpriteIndex = -1;
}
//=== Constructeur n'allouant pas la mémoire pour IDirectDrawSurface =========//
SpriteFrame::SpriteFrame(int Width,int Height)
: DDSurface(Width,Height)
{  NumFrame=0;
   NbSprites=0;
   SF_Suiv = SF_Prec = NULL;
   IDFrame = 0;   // Défini à zéro si pas donné
   BmpAlwaysIntoMemory = true;
   BmpLoadIntoMemory = false;
   Semaphore = 0;
   iStartSpriteIndex = -1;   
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
SpriteFrame::~SpriteFrame()
{
}
//----------------------------------------------------------------------------//

void SpriteFrame::CreateFrame(TDirectScreen *Screen,void* STLperso,const char* BitmapName,const char* SpiName,int NbFrame,SpriteFrame *New_Frame, int i_IDFrame,bool BmpToMem)
{   
	#ifndef NEW_FILE_MANAGER
	FILE *f;
	#else
	TFileVirtual *file = NULL;
	#endif	// NEW_FILE_MANAGER
    FrInfo SpiInfo;
    FrInfo* Spi = &SpiInfo;

    // Assigne son numéro d'identification à la frame
    IDFrame = i_IDFrame;

    // Vérifie l'état des noms des fichiers
    if ((BitmapName==NULL) || (SpiName==NULL))
            MessageError("SpriteFrame Constructor","Noms de fichiers non valides",99);

    // Charge le SPI
	#ifdef NEW_FILE_MANAGER
	file = fopenFV((char*) SpiName);
	if (file == NULL) 
	{	MessageError("Sprite Constructor","N'arrive pas à ouvrir le fichier",99);
		TRACEDBG("SpriteFrame::CreateFrame - Impossible de lire le .spi");
	}
    freadFV(Spi,sizeof(SpiInfo), file);
	#else
	AnsiString pathSpi = CheminApplication + SpiName;
	#ifdef CBUILDER
	if ((f = fopen(pathSpi.c_str(),"rb"))==NULL) MessageError("Sprite Constructor","N'arrive pas à ouvrir le fichier",99);
	#elif VISUALC
	if ((f = fopen(LPCSTR(pathSpi),"rb"))==NULL) MessageError("Sprite Constructor","N'arrive pas à ouvrir le fichier",99);
	#endif
	#endif // NEW_FILE_MANAGER
 
    // Informations de SpriteFrame
    strcpy(Description,SpiInfo.Description);
    NbSprites = SpiInfo.NbSprites;
    CoulFond = SpiInfo.CoulFond;
    CoulOmbr = SpiInfo.CoulOmbr;
    cfRGB = SpiInfo.cfRGB;
    coRGB = SpiInfo.coRGB;
	TRACEDBG("SpriteFrame::CreateFrame - NbSprites = %d\n", NbSprites);

    // Conserve le chemin+nom du fichier bmp
	#ifdef NEW_FILE_MANAGER
	BitmapPathName = new char [strlen(BitmapName)+1];
	strcpy(BitmapPathName, BitmapName);  // conserve le nom du bitmap
	#else
	AnsiString pathBmp = CheminApplication + BitmapName;
	#ifdef CBUILDER
	BitmapPathName = new char [strlen(pathBmp.c_str())+1];
	strcpy(BitmapPathName,pathBmp.c_str());  // conserve le nom du bitmap
	#elif VISUALC
	BitmapPathName = new char [strlen(LPCSTR(pathBmp))+1];
	strcpy(BitmapPathName,LPCSTR(pathBmp));  // conserve le nom du bitmap
	#endif	
	#endif	// NEW_FILE_MANAGER

    // Charge le Bitmap (si demandé)
	if (BmpToMem)
    {  
	   #ifndef NEW_FILE_MANAGER
	   #ifdef CBUILDER
	   pDDSurf = DDLoadBitmap(Screen->pDD,pathBmp.c_str(),PixWidth,PixHeight);
	   #elif VISUALC
	   pDDSurf = DDLoadBitmap(Screen->pDD,LPCSTR(pathBmp),PixWidth,PixHeight);
	   #endif
	   #else
	   //pDDSurf = DDLoadAnyBitmapFromFile(Screen->pDD, (char*) BitmapName, PixWidth, PixHeight);
	   DDReLoadAnyBitmapFromFile(pDDSurf, (char*) BitmapName);	
	   #endif // NEW_FILE_MANAGER
	   BmpLoadIntoMemory = true;                // bmp chargé en mémoire
       BmpAlwaysIntoMemory = true;
       // Couleur transparente suivant la résolution
       switch (SCREEN_BITS)
       { case  8 : ColorKey(CoulFond,CoulFond);break;
         case 16 : ColorKey(RGBto16(cfRGB),RGBto16(cfRGB)); break;
         case 24 : ColorKey(cfRGB,cfRGB);break;
         default : ColorKey(0,0);  // Noir ou 1ère couleur de la palette
       }

    } else
    {  BmpLoadIntoMemory = false;               // bmp non chargé en mémoire
       BmpAlwaysIntoMemory = false;	  
    }

    // Informations de DDSurface
    DestRect.top = DestRect.left = SrcRect.top = SrcRect.left = 0;
    DestRect.bottom = SrcRect.bottom = PixHeight;
    DestRect.right = SrcRect.right = PixWidth;

    //--- On charge
    if (STLperso!=NULL)
    {	// --- S'occupe de répartir les sprites ds leur librairie ---
    	SprInfo SI;
    	SprInfo* SInfo = &SI;    	
		for (int compt=0; compt<NbSprites; compt++)
		{  
			#ifndef NEW_FILE_MANAGER
			fread(SInfo,sizeof(SprInfo),1,f);
			#else
			freadFV(SInfo,sizeof(SprInfo),file);
			#endif
			((SpriteTileLib*) STLperso)->AddSprite(SI,CoulFond,CoulOmbr,cfRGB,coRGB,NbFrame,New_Frame);
    	}
    }
}
////////////////////////////////////////////////////////////////////////////////


//=== Charge le bmp dans la IDirectDrawSurface ===============================//
void SpriteFrame::ReserveMemoryAndLoadBmp(LPDIRECTDRAW4 *pDD2)
{ if (!BmpLoadIntoMemory)  // Vérifie quand même si le bmp n'est pas déjà chargé
  {  // Alloue la mémoire nécessaire avant de charger le bitmap
     AllocateMemory(pDD2,VIDEO_MEMORY);
     // Charge le bmp
     pDDSurf = DDLoadBitmap(*pDD2,BitmapPathName,PixWidth,PixHeight);

     // Couleur transparente suivant la résolution
       switch (SCREEN_BITS)
       { case  8 : ColorKey(CoulFond,CoulFond);break;
         case 16 : ColorKey(RGBto16(cfRGB),RGBto16(cfRGB)); break;
         case 24 : ColorKey(cfRGB,cfRGB);break;
         default : ColorKey(0,0);  // Noir ou 1ère couleur de la palette
       }
     BmpLoadIntoMemory = true;   // Bmp chargé
  }
}
//----------------------------------------------------------------------------//

//=== Désalloue la mémoire utilisée par le bmp ===============================//
void SpriteFrame::DesallocateMemory(LPDIRECTDRAW4 *pDD2)
{ if (BmpLoadIntoMemory)         // Vérifie qu'un bmp est bien chargé en mémoire
  { pDDSurf->Release();          // Libère la DirectDrawSurface
    pDDSurf=NULL;
    BmpLoadIntoMemory = false;   // Mémoire libérée
  }
}
//----------------------------------------------------------------------------//

//=== Changement de Mode Vidéo ===============================================//
void SpriteFrame::ChangeVideoMode(LPDIRECTDRAW4 *pDD4)
{	// Libère la surface si un bmp est bien chargé en mémoire
	if (BmpLoadIntoMemory)
	{	pDDSurf->Release();          // Libère la DirectDrawSurface
		pDDSurf=NULL;
		BmpLoadIntoMemory = false;   // Mémoire libérée
	}

	// Alloue la mémoire nécessaire avant de charger le bitmap
	if (isTextureDirect3D)
	{	AllocateMemory(pDD4, TEXTURE_MEMORY);
	} else
	{	AllocateMemory(pDD4, VIDEO_MEMORY);
	}
	// Recharge le bmp
	DDReLoadBitmap(pDDSurf, BitmapPathName);

	// Couleur transparente suivant la résolution
	switch (SCREEN_BITS)
	{	case  8 : ColorKey(CoulFond,CoulFond);break;
		case 16 : ColorKey(RGBto16(cfRGB),RGBto16(cfRGB)); break;
		case 24 : ColorKey(cfRGB,cfRGB);break;
		default : ColorKey(0,0);  // Noir ou 1ère couleur de la palette
	}
	BmpLoadIntoMemory = true;   // Bmp chargé
}
//----------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
// Fonction Objet : SPRITEFRAMELIST                                           //
////////////////////////////////////////////////////////////////////////////////

SpriteFrameList::SpriteFrameList(void)      // Constructeur
{
    SF_List = NULL;                                // Pointe encore sur rien;
    NbFrame = 0;                                   // Encore aucune frame de chargée
}

// Ajoute une frame à la liste
//  => le numéro est souvent mis à -1 <=> n° donné automatiquement à la suite de la liste
SpriteFrame* SpriteFrameList::AddFrame(TDirectScreen *Screen,void* STLperso,const char* BitmapName,
                                const char* SpiName, int i_IDFrame,bool BmpToMem, bool texture)
{   SpriteFrame *New_Node;
    #ifndef NEW_FILE_MANAGER
	FILE *f;
	#else
	TFileVirtual *file;
	#endif	// NEW_FILE_MANAGER

	// Récupère la dimension de la frame contenue ds le BMP à l'offset 18 et 22
    unsigned short BmpWidth,BmpHeight;
    char temp[18];

	#ifndef NEW_FILE_MANAGER
    AnsiString path = CheminApplication + BitmapName;
	#ifdef CBUILDER
	if ((f = fopen(path.c_str(),"rb"))==NULL) MessageError("SpriteFrameList::AddFrame",path.c_str(),99);
	#elif VISUALC
	if ((f = fopen(LPCSTR(path),"rb"))==NULL) MessageError("SpriteFrameList::AddFrame",(char*) LPCSTR(path),99);
	#endif
    fread(temp,18,1,f); fread(&BmpWidth,2,1,f);
    fread(temp,02,1,f); fread(&BmpHeight,2,1,f);
    fclose(f);
	#else
	file = fopenFV((char*) BitmapName);
	if (file == NULL) MessageError("SpriteFrameList::AddFrame",(char*) BitmapName,99);
    freadFV(temp,18,file); freadFV(&BmpWidth,2,file);
    freadFV(temp,02,file); freadFV(&BmpHeight,2,file);
    fcloseFV(file);
	#endif	// NEW_FILE_MANAGER

    // Si on doit charger le bmp en mémoire
    if (BmpToMem)
    { // Crée une nouvelle Frame de la taille requise ( dans l'éditeur seulement )
      if (!texture) 
	  {	New_Node = new SpriteFrame(Screen,BmpWidth,BmpHeight,VIDEO_MEMORY);
	  } else
      { New_Node = new SpriteFrame(Screen,BmpWidth,BmpHeight,TEXTURE_MEMORY);
	  }
    } else
    { New_Node = new SpriteFrame(BmpWidth,BmpHeight);
    };
    if (New_Node == NULL) MessageError("Problème de mémoire","SpriteFrameList::AddFrame",99);

    New_Node->CreateFrame(Screen,STLperso,BitmapName,SpiName,NbFrame,New_Node,i_IDFrame,BmpToMem);

    // Ajoute cette dernière à la liste
    New_Node->SF_Suiv = SF_List;
    New_Node->SF_Prec = NULL;
    if (SF_List!=NULL) SF_List->SF_Prec = New_Node;
    SF_List = New_Node;

    // Change quelques paramètres;
    SF_List->NumFrame = NbFrame;  // n° de frame suivante
    NbFrame++;

    return New_Node;
}
//----------------------------------------------------------------------------//

//=== Renvoie un ptr sur la frame ayant cet ID ===============================//
SpriteFrame* SpriteFrameList::GetFrame(int numIDFrame)
{   SpriteFrame* frame = SF_List;
    if ((frame!=NULL) && (frame->IDFrame!=numIDFrame)) frame = frame->SF_Suiv;
    return frame;
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
SpriteFrameList::~SpriteFrameList(void)
{	// Supprime toutes les frames de la liste
	Clear();
}
//----------------------------------------------------------------------------//

//=== Supprime toutes les SpriteFrame de la liste ============================//
SpriteFrameList::Clear()
{	SpriteFrame* frame = NULL;
	while (SF_List != NULL)
	{	frame = SF_List;
		SF_List = frame->SF_Suiv;
		delete frame;
		NbFrame--;
	}
}
//----------------------------------------------------------------------------//

//=== Fonction appelée après un chgt de Mode Vidéo ===========================//
void SpriteFrameList::ChangeSpriteFrameVideoMode(LPDIRECTDRAW4 *pDD4)
{	// Test les surfaces DirectDraw de la liste, et (si il le faut)
	// recrée les surfaces et recharge les bitmaps
	HRESULT hr;
	SpriteFrame* frame = SF_List;
	while (frame != NULL)
	{
		hr = frame->pDDSurf->IsLost();
		if (hr == DDERR_SURFACELOST)
		{	hr = frame->pDDSurf->Restore();
		}

		if (hr == DDERR_WRONGMODE)
		{	// Libère la surface si un bmp est bien chargé en mémoire et recharge le bmp
			frame->ChangeVideoMode(pDD4);
		}
			
		frame = frame->SF_Suiv;		// SpriteFrame suivante
	}
}
//----------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Fonction Objet : SPRITETILE                                                //
////////////////////////////////////////////////////////////////////////////////

SpriteTile::SpriteTile(void)      // Constructeur
{
    Suiv = Prec = NULL;
    pFrame = NULL;
    TypeSprite = SPRITE_TILE_2D;
    NumFrame = NumSprite = 0;
}

// Affiche le sprite ds la surface choisie
void SpriteTile::DrawFast(int ScrX,int ScrY,DDSurface* pSurface)
{
    HRESULT hr;
    hr = pSurface->pDDSurf->BltFast(ScrX,ScrY,pFrame->pDDSurf,&SrcRect,DDBLTFAST_WAIT);
    if FAILED(hr) if (!pSurface->CheckSurfaces()) TraceErrorDD(hr,99);
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Fonction Objet : SPRITETILEPERSO                                           //
////////////////////////////////////////////////////////////////////////////////

SpriteTilePerso::SpriteTilePerso()    // Constructeur
{   GSprInf = new CGeneralSpriteInfos();   // Alloue la place requise
} 
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Fonction Objet : SPRITELIB                                                 //
////////////////////////////////////////////////////////////////////////////////

SpriteTileLib::SpriteTileLib(void)      // Constructeur
{
    SL_List = NULL;
    NbSprites = 0;
    SprIndex = NULL;
    nbFrames = 0;
    fileInaBmpName = NULL;
}

//=== Destructeur ============================================================//
SpriteTileLib::~SpriteTileLib()
{	// Vide toute la liste de sprites
	SpriteTile *sprite;
	while (SL_List!=NULL)
	{	sprite = SL_List;
		SL_List = SL_List->Suiv;
		delete sprite;
	}
}
//----------------------------------------------------------------------------//

// Rajoute un sprite de PERSONNAGE à la liste
// classe dérivée SPRITETILEPERSOLIB 
void SpriteTilePersoLib::AddSprite(SprInfo InfSprite,int CF,int CO,unsigned int RGBcf,unsigned int RGBco
                              ,int NumFr,SpriteFrame *pFr)
{
    SpriteTile *New_Node;

    // Crée un nouveau Sprite
    New_Node = new SpriteTilePerso();
    #ifdef DDLIB_DEBUG
    if (New_Node == NULL)
    { MessageError("Problème de mémoire","SpriteLib::AddSprite",99);
 	  #ifdef CBUILDER
	  Application->Terminate();
	  #else
	  _asm int 3;
	  #endif	  
    } 
    #endif
    // Lui injecte toutes les infos
    New_Node->CoordX = InfSprite.CoordX;
    New_Node->CoordY = InfSprite.CoordY;
    New_Node->Width  = InfSprite.Width;
    New_Node->Height = InfSprite.Height;
    New_Node->GravX  = InfSprite.GravX;
    New_Node->GravY  = InfSprite.GravY;
    New_Node->CoulFond = CF;
    New_Node->CoulOmbr = CO;
    New_Node->cfRGB = RGBcf;
    New_Node->coRGB = RGBco;
    New_Node->NumFrame = NumFr;
    New_Node->pFrame = pFr;

    New_Node->NumSprite = NbSprites;

    New_Node->SrcRect.left = InfSprite.CoordX;
    New_Node->SrcRect.top = InfSprite.CoordY;
    New_Node->SrcRect.right = InfSprite.CoordX+InfSprite.Width;
    New_Node->SrcRect.bottom = InfSprite.CoordY+InfSprite.Height;

    // Ajoute ce sprite à la liste
    New_Node->Suiv = SL_List;
    New_Node->Prec = NULL;
    if (SL_List!=NULL) SL_List->Prec = New_Node;
    SL_List = New_Node;

    NbSprites++; // Augmente le compteur de la liste
};

// Rajoute un sprite NORMAL à la liste
void SpriteTileLib::AddSprite(SprInfo InfSprite,int CF,int CO,unsigned int RGBcf,unsigned int RGBco
                              ,int NumFr,SpriteFrame *pFr)
{
    SpriteTile *New_Node;
                                             
    // Crée un nouveau Sprite 3D ou 2D
    if (pFr->isTextureDirect3D)
    { New_Node = new Sprite3DTile();
      ((Sprite3DTile*) New_Node)->Create3DSprite(D3DS->Get3DDevice(), 0x00001234,0x00001234, 0, 0, 1, 1, *pFr);
    }
    else
    { New_Node = new SpriteTile();
    }

    #ifdef DDLIB_DEBUG
    if (New_Node == NULL)
    {  MessageError("Problème de mémoire","SpriteLib::AddSprite",99);
       #ifdef CBUILDER
	   Application->Terminate();
	   #else
	   _asm int 3;
	   #endif
    }
    #endif
    // Lui injecte toutes les infos
    New_Node->CoordX = InfSprite.CoordX;
    New_Node->CoordY = InfSprite.CoordY;
    New_Node->Width  = InfSprite.Width;
    New_Node->Height = InfSprite.Height;
    New_Node->GravX  = InfSprite.GravX;
    New_Node->GravY  = InfSprite.GravY;
    New_Node->CoulFond = CF;
    New_Node->CoulOmbr = CO;
    New_Node->cfRGB = RGBcf;
    New_Node->coRGB = RGBco;
    New_Node->NumFrame = NumFr;
    New_Node->pFrame = pFr;

    New_Node->NumSprite = NbSprites;

    New_Node->SrcRect.left = InfSprite.CoordX;
    New_Node->SrcRect.top = InfSprite.CoordY;
    New_Node->SrcRect.right = InfSprite.CoordX+InfSprite.Width;
    New_Node->SrcRect.bottom = InfSprite.CoordY+InfSprite.Height;

    // Ajoute ce sprite à la liste
    New_Node->Suiv = SL_List;
    New_Node->Prec = NULL;
    if (SL_List!=NULL) SL_List->Prec = New_Node;
    SL_List = New_Node;

    NbSprites++; // Augmente le compteur de la liste
};

// Crée un tableau dynamique de pointers sur des sprites accessible par le n° du sprite
void SpriteTileLib::CreateSprIndex(void)
{
    // Vérifie si SprIndex est déjà utilisé
    if (SprIndex!=NULL)  free(SprIndex); // Ne peut libèrer la mémoire => à n'utiliser qu'1 fois

    // Alloue la mémoire nécessaire vers le tableau
    if ((SprIndex = (SpriteTile**) malloc(sizeof(SpriteTile*) * NbSprites)) == NULL)
    		MessageError("Problème de mémoire","SpriteTileLib::CreateSprIndex",99);
	TRACEDBG("SpriteTileLib::CreateSprIndex -  NbSprites = %d\n",NbSprites);
	for (int j=0 ; j < NbSprites ; j++) SprIndex[j]=NULL;
    // Y range les adresses des sprites
    SpriteTile* Sprite;
    Sprite = SL_List; // dernier sprite
    for (int i = NbSprites-1  ;  i >=0  ; i--)
       {
       	  SprIndex[i]=Sprite;
          Sprite = (SpriteTile*) Sprite->Suiv;
       }
}
//----------------------------------------------------------------------------//

//=== Changement du device3D de toute la liste ===============================//
void SpriteTileLib::ChangeD3DDevice(IDirect3DDevice3* device3D)
{	for (int i=0 ; i < vSprite3DList.size() ; i++)
	{	((Sprite3DTile*) vSprite3DList[i])->SetDirect3DDevice(device3D);
	}
}
//----------------------------------------------------------------------------//

//=== Ajoute un sprite 3D à la liste vSprite3DList ===========================//
void SpriteTileLib::AddSpriteTile3D(SpriteTile* sprite3D)
{	vSprite3DList.push_back(sprite3D);
}
//----------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////////////////

//======================= Fonctions annexes ==================================//

// SpriteDrawFast //////////////////////////////////////////////////////////////
// Dessine le sprite n° ??? dans la surface ??? en utilisant la transparence  //
void SpriteDrawFast(int ScX,int ScY,int NumSprite,DDSurface *pSurface,SpriteTileLib* STLperso)
{
    if ((NumSprite<0) || (STLperso==NULL) || (STLperso->SL_List==NULL)) return;  // vérifications
    if (NumSprite>=STLperso->NbSprites) return;

    // Remonte jusqu'au bon sprite de la SpriteListe
    SpriteTile *Sprite;
    HRESULT hr;
    // Sans "SprIndex", j'utilisais çà :
    // Sprite=STL->SL_List; for (int i=1; i < STL->NbSprites-NumSprite ; i++) Sprite=Sprite->Suiv;
    Sprite = STLperso->SprIndex[NumSprite];
    hr = pSurface->pDDSurf->BltFast(ScX,ScY,Sprite->pFrame->pDDSurf,&(Sprite->SrcRect),DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
    if FAILED(hr) if (!pSurface->CheckSurfaces()) TraceErrorDD(hr,99);
};
////////////////////////////////////////////////////////////////////////////////

// Sprite3DDraw ////////////////////////////////////////////////////////////////
void Sprite3DDraw(int ScX,int ScY,int NumSprite,DDSurface *pSurface,SpriteTileLib* STLperso)
{ if ((NumSprite<0) || (STLperso==NULL) || (STLperso->SL_List==NULL)) return;  // vérifications
    if (NumSprite>=STLperso->NbSprites) return;
    // Remonte jusqu'au bon sprite de la SpriteListe
    Sprite3DTile *Sprite3D;
    Sprite3D = (Sprite3DTile*) STLperso->SprIndex[NumSprite];
    Sprite3D->SpriteRender(ScX, ScY);
    //HRESULT hr = pSurface->pDDSurf->BltFast(ScX,ScY,Sprite->pFrame->pDDSurf,&(Sprite->SrcRect),DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
    //if FAILED(hr) if (!pSurface->CheckSurfaces()) TraceErrorDD(hr,99);
}
////////////////////////////////////////////////////////////////////////////////

// SpriteDraw //////////////////////////////////////////////////////////////////
void SpriteDraw(int ScX,int ScY,int NumSprite,DDSurface *pSurface,SpriteTileLib* STLperso)
{
    if ((NumSprite<0) || (STLperso==NULL) || (STLperso->SL_List==NULL)) return;  // vérifications
    if (NumSprite>=STLperso->NbSprites) return;

    // Remonte jusqu'au bon sprite de la SpriteListe
    SpriteTile *Sprite;
    HRESULT hr;
    // Sans "SprIndex", j'utilisais çà :
    // Sprite=STL->SL_List; for (int i=1; i < STL->NbSprites-NumSprite ; i++) Sprite=Sprite->Suiv;
    Sprite = STLperso->SprIndex[NumSprite];
    RECT DestRect;
    DestRect.top = ScY; DestRect.left = ScX;
    DestRect.bottom = ScY + Sprite->SrcRect.bottom - Sprite->SrcRect.top;
    DestRect.right  = ScX + Sprite->SrcRect.right - Sprite->SrcRect.left;
    hr = pSurface->pDDSurf->Blt(&DestRect,Sprite->pFrame->pDDSurf,&(Sprite->SrcRect),DDBLT_WAIT | DDBLT_KEYSRC , NULL);
    if FAILED(hr) if (!pSurface->CheckSurfaces()) TraceErrorDD(hr,99);
};
////////////////////////////////////////////////////////////////////////////////


int GravX(int NumSprite,SpriteTilePersoLib* STLperso)
{   SpriteTile *Sprite = STLperso->SprIndex[NumSprite];
    return -(Sprite->GravX);
};
int GravY(int NumSprite,SpriteTilePersoLib* STLperso)
{   SpriteTile *Sprite = STLperso->SprIndex[NumSprite];
/*  Sprite=STLperso->SL_List;
    for (int i=1; i < STLperso->NbSprites-NumSprite ; i++) Sprite=Sprite->Suiv; */
    return -(Sprite->GravY);
};

// CGENERALSPRITEINFOS /////////////////////////////////////////////////////////
float CGeneralSpriteInfos::Version = 1.00;               // n° de version

//=== Constructeur de la classe ----------------------------------------------//
CGeneralSpriteInfos::CGeneralSpriteInfos()
{

};
//----------------------------------------------------------------------------//

//=== Sauvegarde les informations contenue de cette structure ================//
void CGeneralSpriteInfos::SaveToFile(FILE &fichier)
{  // Sauve le rectangle de la zone au sol
   ZoneSol.SaveToFile(fichier);
   // Sauve ke centre de gravité du perso projeté au sol
   GravSol.SaveToFile(fichier);
   // Sauve la liste des sprites superposables
   ListAnimSuperp.SaveToFile(fichier);
   // Sauve le rectangle de la zone aux pieds
   ZonePied.SaveToFile(fichier);

   // Sauve les données tampons
   for (int i=0 ; i < 10 ; i++) fwrite(&temp[i],sizeof(int),1,&fichier);}
//----------------------------------------------------------------------------//

//=== Charge les informations contenue de cette structure ====================//
void CGeneralSpriteInfos::LoadFromFile(FILE &fichier)
{  // Charge le rectangle de la zone au sol
   ZoneSol.LoadFromFile(fichier);
   // Charge le centre de gravité du perso projeté au sol
   GravSol.LoadFromFile(fichier);
   // Charge la liste des sprites superposables
   ListAnimSuperp.LoadFromFile(fichier);
   // Charge le rectangle de la zone aux pieds
   ZonePied.LoadFromFile(fichier);

   // Charge les données tampons
   for (int i=0 ; i < 10 ; i++) fread(&temp[i],sizeof(int),1,&fichier);
}
void CGeneralSpriteInfos::LoadFromFile(TFileVirtual &fichier)
{  // Charge le rectangle de la zone au sol
   ZoneSol.LoadFromFile(fichier);
   // Charge le centre de gravité du perso projeté au sol
   GravSol.LoadFromFile(fichier);
   // Charge la liste des sprites superposables
   ListAnimSuperp.LoadFromFile(fichier);
   // Charge le rectangle de la zone aux pieds
   ZonePied.LoadFromFile(fichier);
   // Charge les données tampons
   for (int i=0 ; i < 10 ; i++) fichier.read(&temp[i],sizeof(int));
}
//----------------------------------------------------------------------------//


////////////////////////////////////////////////////////////////////////////////

// struct ZONERECT /////////////////////////////////////////////////////////////
//=== Sauvegarde les 4 coordonnées des 2 points du rectangle =================//
void ZoneRect::SaveToFile(FILE &fichier)
{  fwrite(&x1,sizeof(x1), 1, &fichier);
   fwrite(&y1,sizeof(y1), 1, &fichier);
   fwrite(&x2,sizeof(x2), 1, &fichier);
   fwrite(&y2,sizeof(y2), 1, &fichier);
}
//----------------------------------------------------------------------------//

//=== Charge les 4 coordonnées des 2 points du rectangle =====================//
void ZoneRect::LoadFromFile(FILE &fichier)
{  fread(&x1,sizeof(x1), 1, &fichier);
   fread(&y1,sizeof(y1), 1, &fichier);
   fread(&x2,sizeof(x2), 1, &fichier);
   fread(&y2,sizeof(y2), 1, &fichier);

   // Remplit le champs Enabled
   if ((x1!=-1) || (y1!=-1) || (x2!=-1) || (y2!=-1)) Enabled = true;
}
void ZoneRect::LoadFromFile(TFileVirtual &fichier)
{	fichier.read(&x1,sizeof(x1));
	fichier.read(&y1,sizeof(y1));
	fichier.read(&x2,sizeof(x2));
	fichier.read(&y2,sizeof(y2));
	// Remplit le champs Enabled
	if ((x1!=-1) || (y1!=-1) || (x2!=-1) || (y2!=-1)) Enabled = true;
}
//----------------------------------------------------------------------------//
////////////////////////////////////////////////////////////////////////////////

// struct ZONEORIENTRECT ///////////////////////////////////////////////////////
//=== Sauvegarde les 4 coordonnées des 2 points du rectangle =================//
void ZoneOrientRect::SaveToFile(FILE &fichier)
{  fwrite(&x1,sizeof(x1), 1, &fichier); fwrite(&y1,sizeof(y1), 1, &fichier);
   fwrite(&x2,sizeof(x2), 1, &fichier); fwrite(&y2,sizeof(y2), 1, &fichier);
   fwrite(&x3,sizeof(x3), 1, &fichier); fwrite(&y3,sizeof(y3), 1, &fichier);
   fwrite(&x4,sizeof(x4), 1, &fichier); fwrite(&y4,sizeof(y4), 1, &fichier);
}
//----------------------------------------------------------------------------//

//=== Charge les 4 coordonnées des 2 points du rectangle =====================//
void ZoneOrientRect::LoadFromFile(FILE &fichier)
{  fread(&x1,sizeof(x1), 1, &fichier); fread(&y1,sizeof(y1), 1, &fichier);
   fread(&x2,sizeof(x2), 1, &fichier); fread(&y2,sizeof(y2), 1, &fichier);
   fread(&x3,sizeof(x3), 1, &fichier); fread(&y3,sizeof(y3), 1, &fichier);
   fread(&x4,sizeof(x4), 1, &fichier); fread(&y4,sizeof(y4), 1, &fichier);

   // Remplit le champs Enabled
   if (   (x1!=-1) || (y1!=-1) || (x2!=-1) || (y2!=-1)
       || (x3!=-1) || (y3!=-1) || (x4!=-1) || (y4!=-1) ) Enabled = true;
}
void ZoneOrientRect::LoadFromFile(TFileVirtual &fichier)
{	fichier.read(&x1,sizeof(x1)); fichier.read(&y1,sizeof(y1));
	fichier.read(&x2,sizeof(x2)); fichier.read(&y2,sizeof(y2));
	fichier.read(&x3,sizeof(x3)); fichier.read(&y3,sizeof(y3));
	fichier.read(&x4,sizeof(x4)); fichier.read(&y4,sizeof(y4));
	// Remplit le champs Enabled
	if (   (x1!=-1) || (y1!=-1) || (x2!=-1) || (y2!=-1)
        || (x3!=-1) || (y3!=-1) || (x4!=-1) || (y4!=-1) ) Enabled = true;
}

//----------------------------------------------------------------------------//
////////////////////////////////////////////////////////////////////////////////

// struct ZONEPOINT ////////////////////////////////////////////////////////////
//=== Sauvegarde les 2 coordonnées ===========================================//
void ZonePoint::SaveToFile(FILE &fichier)
{  fwrite(&x,sizeof(x), 1, &fichier);
   fwrite(&y,sizeof(y), 1, &fichier);
}
//----------------------------------------------------------------------------//

//=== Charge les 2 coordonnées du point ======================================//
void ZonePoint::LoadFromFile(FILE &fichier)
{  fread(&x,sizeof(x), 1, &fichier);
   fread(&y,sizeof(y), 1, &fichier);
   // Remplit le champs Enabled
   if ((x!=-1) || (y!=-1)) Enabled = true;
}
void ZonePoint::LoadFromFile(TFileVirtual &fichier)
{	fichier.read(&x, sizeof(x));
	fichier.read(&y, sizeof(y));
	// Remplit le champs Enabled
	if ((x!=-1) || (y!=-1)) Enabled = true;
}
//----------------------------------------------------------------------------//
////////////////////////////////////////////////////////////////////////////////


// Classe ZONEANIMSUPERP ///////////////////////////////////////////////////////

//=== Ajoute un sprite superposable au vecteur ===============================//
void ZoneAnimSuperp::Add(int &i_x, int &i_y, int &i_numIDT, int &i_numIDS)
{  TabAnimSuperp.push_back(sanimsuperp(i_x,i_y,i_numIDT,i_numIDS));
}
//----------------------------------------------------------------------------//

//=== Sauvegarde le tableau d'anim superposées + un entier d'en-tête =========//
void ZoneAnimSuperp::SaveToFile(FILE &fichier)
{  // En-tête : entier (int) indiquant le nombre d'élément du vecteur
   int nbElem = TabAnimSuperp.size();
   fwrite(&nbElem,sizeof(nbElem), 1, &fichier);
   // Tout le vecteur
   for (int i=0; i < nbElem; i++)
   { fwrite(&TabAnimSuperp[i].x,sizeof(TabAnimSuperp[i].x), 1, &fichier);
     fwrite(&TabAnimSuperp[i].y,sizeof(TabAnimSuperp[i].y), 1, &fichier);
     fwrite(&TabAnimSuperp[i].numIDTypeObjSprite,sizeof(TabAnimSuperp[i].numIDTypeObjSprite), 1, &fichier);
     fwrite(&TabAnimSuperp[i].numIDSpriteSuperpose,sizeof(TabAnimSuperp[i].numIDSpriteSuperpose), 1, &fichier);
   }
}
//----------------------------------------------------------------------------//

//=== Charge le tableau d'anims superposées ==================================//
void ZoneAnimSuperp::LoadFromFile(FILE &fichier)
{  // Lit l'en-tête
   int nbElem;
   fread(&nbElem,sizeof(nbElem), 1, &fichier);
   // Puis tout le vecteur
   int x, y, n, m;
   for (int i=0 ; i<nbElem; i++)
   {  // Lit 3 entiers
      fread(&x,sizeof(x), 1, &fichier);
      fread(&y,sizeof(y), 1, &fichier);
      fread(&n,sizeof(n), 1, &fichier);
      fread(&m,sizeof(m), 1, &fichier);
      Add(x,y,n,m);  // Puis les ajoute au vecteur
   }
}
void ZoneAnimSuperp::LoadFromFile(TFileVirtual &fichier)
{  // Lit l'en-tête
   int nbElem;
   fichier.read(&nbElem, sizeof(nbElem));
   // Puis tout le vecteur
   int x, y, n, m;
   for (int i=0 ; i<nbElem; i++)
   {  // Lit 3 entiers
      fichier.read(&x, sizeof(x));
	  fichier.read(&y, sizeof(y));
	  fichier.read(&n, sizeof(n));
	  fichier.read(&m, sizeof(m));
      Add(x,y,n,m);  // Puis les ajoute au vecteur
   }
}
//----------------------------------------------------------------------------//

//=== Renvoie vrai si le sAnimSuperp correspondant à ce n° existe ============//
bool ZoneAnimSuperp::Existe(int num_ID)
{ for (unsigned int i=0; i < TabAnimSuperp.size() ; i++)
  { if (TabAnimSuperp[i].numIDTypeObjSprite == num_ID) return true;
  }
  return false;
}
//----------------------------------------------------------------------------//

//=== Renvoie les coords du sprite de n°num_ID ===============================//
void ZoneAnimSuperp::GetCoord(int num_ID, int &xs,int &ys)
{ for (unsigned int i=0; i < TabAnimSuperp.size() ; i++)
  { if (TabAnimSuperp[i].numIDTypeObjSprite == num_ID)
    { xs = TabAnimSuperp[i].x;
      ys = TabAnimSuperp[i].y;
      return;
    }
  }  // Si on arrive ici, c'est que le sprite n'existe pas
}
//----------------------------------------------------------------------------//


//=== Renvoie les coords du sprite de n°num_ID ===============================//
void ZoneAnimSuperp::SetCoord(int num_ID, int &xs,int &ys)
{ for (unsigned int i=0; i < TabAnimSuperp.size() ; i++)
  { if (TabAnimSuperp[i].numIDTypeObjSprite == num_ID)
    { TabAnimSuperp[i].x = xs;
      TabAnimSuperp[i].y = ys;
      return;
    }
  }  // Si on arrive ici, c'est que le sprite n'existe pas
}
//----------------------------------------------------------------------------//

//=== Renvoie le n°ID du sprite superposable correspondant au n°ID du type d'objet
int ZoneAnimSuperp::GetIDS(int num_IDT)
{ for (unsigned int i=0; i < TabAnimSuperp.size() ; i++)
  { if (TabAnimSuperp[i].numIDTypeObjSprite == num_IDT)
    { return TabAnimSuperp[i].numIDSpriteSuperpose;
    }
  }
  return 0;
}
//----------------------------------------------------------------------------//


////////////////////////////////////////////////////////////////////////////////

