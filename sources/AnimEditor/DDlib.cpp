////////////////////////////////////////////////////////////////////////////////
//  Unit�  "DDLib.cpp"  : Library des principales fonctions de DirectDraw     //
//  Copyright (C) 1998 Antoine Rey    arey@insa-lyon.fr                       //
//                                    antrey@club-internet.fr                 //
//                                                                            //
//  Version      : beta 0.65                                                  //
//  Creation     : 21/05/1998 17h45                                           //
//  Last upgrade : 31/08/1998                                                 //
//                                                                            //
//  Comment :  Destin� � la programmation de DirectDraw en FULLSCREEN MODE    //
//                                                                            //
//  Sources :                                                                 //
//    -  des parties de codes proviennent du bouqin Delphi,                   //
//    -  d'autres du 'Game Programming with Direct X' by David Joffe          //
//          http://www.geocities.com/SoHo/Lofts/2018/dj3.html                 //
//    - voir aussi doc http://www.pcisys.net/~omar/direct2.html               //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Rmq : - Probl�me de conversion couleur 24 bits en 16 bits   "RGBto16"      //
//		 - nouveau header  "DDlib.h" d'accompagnement						  //
////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// Unit�s � inclure
//#define INITGUID           // Pour pouvoir toucher aux IID_IDirectDrawSurface4

//#include "DDraw.h"        // Bibliohot�que Microsoft DirectDraw
#include <stdio.h>
#include "DDlib.h"
#include "D3DLib.h"         // sprites 3D
#include "DDutil.h"         // Routines for loading bitmaps & palettes  IDirectDrawSurface4
#include "fPrincipale.h"
#include <vcl\Classes.hpp>
//#include "consts.h"

// pragma en tout genre
#pragma hdrstop         // Impos� par C++ Builder
//#define DDLIB_DEBUG


//------------------------------------------------------------------------------
// D�claration des variables, classes et autres structures
// Les Constantes de MON programme
#define FULLSCREEN TRUE
#define HARDWARE TRUE
#define MODEX FALSE
#define HARDWARECLIPPING FALSE

//** DONNEES TEMPORAIRES *******************************************************
// Donn�es g�om�triques d'un trap�ze
const int ISO_HEIGHT_2 	    = 21-1,
	  ISO_WIDTH_2  	    = 39-1,
    	  ISO_HEIGHT_HALF_2 = 10,
    	  ISO_WIDTH_HALF_2  = 19,
          ISO_DENOMINATEUR_2 = 2 * ISO_HEIGHT_HALF_2 * ISO_WIDTH_HALF_2;
// Type de trac� du trap�ze
const int TrapTYPE_SIMPLE_2  = 0,
	  TrapTYPE_DOUBLE_2  = 1;
//******************************************************************************

SpriteFrameList *SFL;  // Liste officielle de la gestion des frames
TDirectScreen  *DS = NULL;
// par d�faut
int SCREEN_WIDTH = 0,SCREEN_HEIGHT = 0, SCREEN_BITS   = 16;

////////////////////////////////////////////////////////////////////////////////
// Fonctions & D�clarations annexes                                                          //
////////////////////////////////////////////////////////////////////////////////

// Permet de coder une couleur sur 24 bits (8:8:8) ---------------------------//
typedef struct
{   // tous cod�s sur 8 bits  => max de 255
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

 // S'occupe d'interrompre le programme si probl�mes
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

// Assigne les diff�rents param�tres de la surface
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

    // Assigne les diff�rents param�tres de la surface
    SrcRect.top = DestRect.top = 0;
    SrcRect.left = DestRect.left = 0;
    SrcRect.bottom = DestRect.bottom = Height;
    SrcRect.right = DestRect.right = Width;

    ColKey1=ColKey2=0;
}
DDSurface::DDSurface(LPDIRECTDRAW4 *pDD4, int Width, int Height,SURFACEMEM surfMem)
{
    pDDSurf = NULL;
    SurfDC  = NULL;
    PixWidth = Width;
    PixHeight = Height;
    SurfFont = NULL;
    isAllocateTexture = false;
    isTextureDirect3D = false;

    AllocateMemory(pDD4,surfMem);

    // Assigne les diff�rents param�tres de la surface
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

//=== Allloue de la m�moire pour la DirectDrawSurface ========================//
//-- 1�re version pour jeu --
void DDSurface::AllocateMemory(TDirectScreen *pScreen, SURFACEMEM surfMem)
{   HRESULT hr;
    memset(&DDSurfDesc,0,sizeof(DDSurfDesc));  // met tout � z�ro
    DDSurfDesc.dwSize=sizeof(DDSurfDesc);      // taille de sa propre structure
    DDSurfDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    switch (surfMem)
    { case (VIDEO_MEMORY)   : DDSurfDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY; break;
      case (SYSTEM_MEMORY)  : DDSurfDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY; break;
      case (TEXTURE_MEMORY) :
        {  // Lorsque la surface va �tre utilis�e comme texture par Direct3D
           DDSurfDesc.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_ALLOCONLOAD;
           isAllocateTexture = false;
           isTextureDirect3D = true;
        }
    };

    DDSurfDesc.dwWidth = PixWidth;
    DDSurfDesc.dwHeight = PixHeight;

    // Cr�e la surface IDirectDrawSurface
    // <=>  hr = pScreen->pDD->CreateSurface(&DDSurfDesc,(IDirectDrawSurface **) &pDDSurf,NULL);
    LPDIRECTDRAWSURFACE4 lpSurf;
    hr = pScreen->pDD->CreateSurface(&DDSurfDesc,&lpSurf,NULL);
    if (hr==DDERR_OUTOFVIDEOMEMORY)
    { // Il ne doit pas y avoir de probl�mes ici pour l'allocation des textures
      DDSurfDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
      hr = pScreen->pDD->CreateSurface(&DDSurfDesc,&lpSurf,NULL);
    };
    if FAILED(hr) TraceErrorDD(hr,99);
    hr = lpSurf->QueryInterface(IID_IDirectDrawSurface4, (LPVOID*) &pDDSurf);
    if FAILED(hr) TraceErrorDD(hr,99);
    lpSurf->Release();
}
//-- 2nde version pour �diteur --
void DDSurface::AllocateMemory(LPDIRECTDRAW4 *pDD2, SURFACEMEM surfMem)
{   HRESULT hr;
    memset(&DDSurfDesc,0,sizeof(DDSurfDesc));  // met tout � z�ro
    DDSurfDesc.dwSize=sizeof(DDSurfDesc);      // taille de sa propre structure
    DDSurfDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    switch (surfMem)
    {
      case (VIDEO_MEMORY)  : DDSurfDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY; break;
      case (SYSTEM_MEMORY) : DDSurfDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY; break;
      case (TEXTURE_MEMORY) :
        {  // Lorsque la surface va �tre utilis�e comme texture par Direct3D
           DDSurfDesc.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_ALLOCONLOAD;
           isAllocateTexture = false;
           isTextureDirect3D = true;
        }
    }
    DDSurfDesc.dwWidth = PixWidth;
    DDSurfDesc.dwHeight = PixHeight;

    // Cr�e la surface IDirectDrawSurface
    // <=>  hr = pScreen->pDD->CreateSurface(&DDSurfDesc,(IDirectDrawSurface **) &pDDSurf,NULL);
    LPDIRECTDRAWSURFACE4 lpSurf;
    hr = (*pDD2)->CreateSurface(&DDSurfDesc,&lpSurf,NULL);
    if (hr==DDERR_OUTOFVIDEOMEMORY)
    {     // Il ne doit pas y avoir de probl�mes ici pour l'allocation des textures
          DDSurfDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
          hr = (*pDD2)->CreateSurface(&DDSurfDesc,&lpSurf,NULL);
    }
    if FAILED(hr) TraceErrorDD(hr,99);

    hr = lpSurf->QueryInterface(IID_IDirectDrawSurface4, (LPVOID*) &pDDSurf);
    if FAILED(hr) TraceErrorDD(hr,99);
    lpSurf->Release();                                                       
}
//----------------------------------------------------------------------------//

// V�rifie l'�tat de la surface ----------------------------------------------//
//   Proc�dure restaurant les surfaces perdues dans certaines circonstances �
// cause du GDI de Windows. Elle regarde si la m�moire associ�e aux surfaces
// est perdue et la r�cup�re si n�cessaire.
BOOL DDSurface::CheckSurfaces()
{

       if (pDDSurf!=NULL)
        switch(pDDSurf->IsLost())
         {
          case DD_OK : return TRUE;  // it's all right ...
          // Si elle est perdue, on la restaure gentillement
          case DDERR_SURFACELOST : pDDSurf->Restore();return FALSE;
          // le mat�riel est occup� et que l'on devra r�p�ter la requ�te ult�rieurement
          case DDERR_WASSTILLDRAWING : return FALSE;
          // Erreur non d�finie ? peut-�tre arr�ter l'ex�cution ?
          default : return FALSE;
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
    // peut-�tre une surface perdue ? qui sait ?
    if FAILED(hr) if (!CheckSurfaces()) TraceErrorDD(hr,99);
}
//----------------------------------------------------------------------------//

// Copie la surface dans une autre -------------------------------------------//
void DDSurface::DrawNoKey(DDSurface* pDDSurfDest)
{
    HRESULT hr;
    hr = pDDSurfDest->pDDSurf->Blt(&DestRect,pDDSurf,&SrcRect,DDBLT_WAIT,NULL);
    // peut-�tre une surface perdue ? qui sait ?
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
    // peut-�tre une surface perdue ? qui sait ?
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
// => � utiliser lorsque l'on veut manipuler DDSurfDesc.lpSurface
void DDSurface::Lock()
{
    HRESULT hr;
    hr = pDDSurf->Lock(NULL,&DDSurfDesc,DDLOCK_WAIT,NULL);
    if FAILED(hr) TraceErrorDD(hr,99);
}
//----------------------------------------------------------------------------//


// Obtient un pointer vers la surface ----------------------------------------//
// => � utiliser lorsque l'on veut manipuler DDSurfDesc.lpSurface
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
//   la variable Col doit �tre au format du pixel 8,16,24 ou 32 bits
void DDSurface::PutPixel(int X,int Y,unsigned int Col)
{
    static WORD *Map16 = (WORD*) DDSurfDesc.lpSurface;
    static BYTE *Map8  = (BYTE*) DDSurfDesc.lpSurface;
    switch (DS->Screen_Bits)
    {
        case 16 : Map16[Y*DDSurfDesc.lPitch/2+X] = (WORD) Col; break;
        case  8 : Map8[Y*DDSurfDesc.lPitch+X] = (BYTE) Col; break;
    }
}
//----------------------------------------------------------------------------//

// Choppe un pixel -----------------------------------------------------------//
// renvoit la valeur trouv�e sans la convertir
unsigned int DDSurface::GetPixel(int X,int Y)
{
    static WORD *Map16 = (WORD*) DDSurfDesc.lpSurface;
    static BYTE *Map8  = (BYTE*) DDSurfDesc.lpSurface;
    switch (DS->Screen_Bits)
    {
        case 16 : return Map16[Y*DDSurfDesc.lPitch+X];
        case  8 : return Map8[Y*DDSurfDesc.lPitch+X];
    }
    return -1;  // Si inconnu
}
//----------------------------------------------------------------------------//

// Intervalle de couleur non prise en compte par le blitting -----------------//
// c1 et c2 peuvent tout aussi bien repr�senter des indices de palettes (8 bits)
// ou une valeur (R,V,B) cod�e sur 16 ou 24 bits
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

// Change la police de caract�re de la surface -------------------------------//
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

// Efface l'�cran d'une couleur sp�cifique -----------------------------------//
void DDSurface::Fill(DWORD ColFill)
{
    DDBLTFX ddBltFx;

    ddBltFx.dwSize = sizeof(DDBLTFX);
    ddBltFx.dwFillColor = ColFill;
    HRESULT hr = pDDSurf->Blt(NULL,NULL,NULL,DDBLT_WAIT | DDBLT_COLORFILL,&ddBltFx);
    if FAILED(hr) if (!CheckSurfaces()) TraceErrorDD(hr,99);

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

    FullScreen = FULLSCREEN;
    ModeX = MODEX;
    Hardware = HARDWARE;
    HardwareClipping = HARDWARECLIPPING;
    Screen_Height = SCREEN_HEIGHT;
    Screen_Width =  SCREEN_WIDTH;
    Screen_Bits = SCREEN_BITS;

    DDHandle = Handle;

    RedCoeff = GreenCoeff = BlueCoeff = 1.0; // Coefficients servant � calculer les composantes de la table
}
//----------------------------------------------------------------------------//

//  A utiliser lorsque l'on ouvre une session sous DirectDraw ----------------//
void TDirectScreen::InitDDraw()
{
    HRESULT hr;

    // Cr�e l'objet COM DirectDraw
    LPDIRECTDRAW lpDD;
    // <=> if (FAILED(hr=DirectDrawCreate(NULL,(IDirectDraw **) &pDD,NULL))) TraceErrorDD(hr,99);
    hr = DirectDrawCreate(NULL,&lpDD,NULL);
    if FAILED(hr) TraceErrorDD(hr,99);

    hr = lpDD->QueryInterface(IID_IDirectDraw4,(LPVOID*) &pDD);
    if FAILED(hr) TraceErrorDD(hr,99);
    lpDD->Release();


    // D�fini un niveau de coop�ration exclusif afin de pouvoir changer le mode d'affichage
    if (FullScreen)
     {
        if (!ModeX)
           hr = pDD->SetCooperativeLevel(DDHandle,DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT);
        else
           hr = pDD->SetCooperativeLevel(DDHandle,DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX | DDSCL_ALLOWREBOOT);
        if (FAILED(hr)) TraceErrorDD(hr,99);  // s'arr�te si windows ne veut coop�rer

        // Change la r�solution en SCREEN_WIDTH x SCREEN_HEIGHT x SCREEN_BITS  full-screen mode
        hr = pDD->SetDisplayMode(Screen_Width,Screen_Height,Screen_Bits,0,0);
        if (FAILED(hr)) TraceErrorDD(hr,99);  // ne peut pas non plus continuer
       }
    else  // Windowed-mode : on ne change pas de r�solution
        {
          hr = pDD->SetCooperativeLevel(DDHandle,DDSCL_NORMAL);
          if (FAILED(hr)) TraceErrorDD(hr,99);  // s'arr�te si windows ne veut coop�rer
        }
// Cr�e une surface complexe permettant le basculement entre 2 pages
    DDSURFACEDESC2 ddSD;     // Une structure permettant la description de la surface souhait�e

    memset(&ddSD,0,sizeof(ddSD));  // met tout � z�ro
    ddSD.dwSize = sizeof(ddSD);    // taille de sa propre strucutre
    ddSD.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
    if (Hardware) ddSD.ddsCaps.dwCaps = DDSCAPS_COMPLEX | DDSCAPS_FLIP | DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE;
    else  ddSD.ddsCaps.dwCaps = DDSCAPS_COMPLEX | DDSCAPS_FLIP | DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE/* | DDSCAPS_SYSTEMMEMORY*/;
    ddSD.dwBackBufferCount = 1;

    // Cr�ation des 2 tampons ds la surface complex
    // Cr�e la surface IDirectDrawSurface
 // <=>  hr = pScreen->pDD->CreateSurface(&DDSurfDesc,(IDirectDrawSurface **) &pDDSurf,NULL);
    pDDSPrimary = new DDSurface(Screen_Width,Screen_Height);
    pDDSBack    = new DDSurface(Screen_Width,Screen_Height);
     // <=> hr = pDD->CreateSurface(&ddSD,(IDirectDrawSurface **) &pDDSPrimary->pDDSurf,NULL);
    LPDIRECTDRAWSURFACE4 lpSurf;
    hr = pDD->CreateSurface(&ddSD,&lpSurf,NULL);
    if (FAILED(hr)) TraceErrorDD(hr,99);
    hr = lpSurf->QueryInterface(IID_IDirectDrawSurface4, (LPVOID*)&pDDSPrimary->pDDSurf);
    if (FAILED(hr)) TraceErrorDD(hr,99);
    lpSurf->Release();

    ddSD.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER | DDSCAPS_3DDEVICE;
    hr = pDDSPrimary->pDDSurf->GetAttachedSurface(&ddSD.ddsCaps,&pDDSBack->pDDSurf);
    if (FAILED(hr)) TraceErrorDD(hr,99);

    // S'occupe du clipping Hardware de la surface complexe si voulu
    if (HardwareClipping)
     {
    	// Cr�e le cadre de clipping
    	hr = pDD->CreateClipper(0,&pDDClipper,NULL);
    	if (FAILED(hr))	TraceErrorDD(hr,99);
    	// Assigne le clipper � la fen�tre
    	hr = pDDClipper->SetHWnd(0, DDHandle);
    	if (FAILED(hr))  // on abandonne le clipping !
    	{
        	pDDClipper->Release();
        	TraceErrorDD(hr,99);
    	}
    	// zone de clipping de la surface principale
    	hr = pDDSPrimary->pDDSurf->SetClipper(pDDClipper);
    	if (FAILED(hr))
    	{   pDDClipper->Release();
			TraceErrorDD(hr,99);  }
 		// zone de clipping de la surface secondaire
    	hr = pDDSBack->pDDSurf->SetClipper(pDDClipper);
    	if (FAILED(hr))
    	{   pDDClipper->Release();
			TraceErrorDD(hr,99);  }
    	// plus besoin du clipper
		pDDClipper->Release();
     };  // fin du clipping harware

    // Initialise le Gamma Control sur la surface principale
    hr = pDDSPrimary->pDDSurf->QueryInterface(IID_IDirectDrawGammaControl ,(LPVOID*) &pDDGammaControl);
    if FAILED(hr) TraceErrorDD(hr,99);

}
//----------------------------------------------------------------------------//

// Destructeur ---------------------------------------------------------------//
TDirectScreen::~TDirectScreen(void)
{
    if (pDD!=NULL)
    {
       pDD->RestoreDisplayMode();

       if (pDDSPrimary!=NULL)  { delete pDDSPrimary; pDDSPrimary = NULL; };
       if (pDDSBack!=NULL) { delete pDDSBack; pDDSBack = NULL; };
       if (pDDPal!=NULL) { pDDPal->Release(); pDDPal = NULL; };

       pDD->Release(); pDD = NULL;
    }
}
//----------------------------------------------------------------------------//

// Charge un Bitmap ds le Back Buffer ----------------------------------------//
BOOL TDirectScreen::LoadBitmap(const char* BitmapName)
{
    if (BitmapName==NULL) return FALSE;
    if (DDReLoadBitmap(pDDSBack->pDDSurf,BitmapName)!=DD_OK) return FALSE;
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
    HRESULT hr;
    RECT rcSrc;    // variables utilis�es en windowed-mode;
    RECT rcDest;
    POINT p;

    if (pDDSPrimary->CheckSurfaces())  // s'assure que les surfaces ne soient pas perdues
     {
      if (!(FullScreen))
      { // On Blit les 2 surfaces en windowed-mode
        p.x = p.y = 0; // regarde o� la fen�tre se trouve sur la surface primaire
        ClientToScreen(DDHandle,&p);
        GetClientRect(DDHandle,&rcDest);
        OffsetRect(&rcSrc,p.x,p.y);
        SetRect(&rcSrc,0,0,Screen_Width,Screen_Width);
        hr = pDDSPrimary->pDDSurf->Blt(&rcDest,pDDSBack->pDDSurf,&rcSrc,DDBLT_WAIT,NULL);
        if FAILED(hr) if (!pDDSPrimary->CheckSurfaces()) TraceErrorDD(hr,99);
      }
      else
      { // Bascule automatiquement sur la surface d'arri�re-plan (synchronis�)
        hr = pDDSPrimary->pDDSurf->Flip(NULL,DDFLIP_WAIT);
      }
      // Faut-il appeler TraceErrorDD sachant qu'il ne fera rien
      //    if (FAILED(hr)) TraceErrorDD(hr,1);
      //   => NON (for the moment)
     }
    else { };
    return hr;
 }
//----------------------------------------------------------------------------//

// Efface l'�cran d'une couleur sp�cifique -----------------------------------//
void TDirectScreen::Fill(DWORD ColFill)
{
    DDBLTFX ddBltFx;

    ddBltFx.dwSize = sizeof(DDBLTFX);
    ddBltFx.dwFillColor = ColFill;
    HRESULT hr = pDDSBack->pDDSurf->Blt(NULL,NULL,NULL,DDBLT_WAIT | DDBLT_COLORFILL,&ddBltFx);
    if FAILED(hr) if (!pDDSBack->CheckSurfaces()) TraceErrorDD(hr,99);
}
//----------------------------------------------------------------------------//

void Swap(int *a, int*b)  // �change 2 integers  (utilis� par l'algo de lignes)
{
	int Temp = *a;
    *a = *b;
    *b = Temp;
};

//== Trace une ligne sur l'�cran par interpolation ===========================//
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

//== Trace un trap�ze d'apr�s les donn�es ISO ================================//
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
{   // Applique la Gamma table � l'�cran
    pDDGammaControl->SetGammaRamp(0,&lpGammaRamp);
}
//----------------------------------------------------------------------------//

//=== Assigne les coefficients � appliquer aux composantes de couleur ========//
void TDirectScreen::SetGammaRamp(double& Rcoeff, double& Vcoeff, double& Bcoeff)
  { RedCoeff = Rcoeff;
    BlueCoeff = Bcoeff;
    GreenCoeff = Vcoeff;

    // Remplissage de la table
    unsigned int normal, ramp;
    for (int i=0 ; i<256 ; i++)
    {   normal = i*256;
        ramp = normal * RedCoeff;
        if (ramp < 65535) lpGammaRamp.red[i]=(WORD) ramp; else lpGammaRamp.red[i] = 65535;
        ramp = normal * GreenCoeff;
        if (ramp < 65535) lpGammaRamp.green[i]=(WORD) ramp; else lpGammaRamp.green[i] = 65535;
        ramp = normal * BlueCoeff;
        if (ramp < 65535) lpGammaRamp.blue[i]=(WORD) ramp; else lpGammaRamp.blue[i] = 65535;
     }
}
//----------------------------------------------------------------------------//


////////////////////////////////////////////////////////////////////////////////
// Fonctions & D�clarations annexes                                                          //
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
unsigned long RGB16(tHighColor Coul) // m�me chose en utilisant tHichColor
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
{  // ATTENTION = apprement ce n'est pas l'exacte m�thode pour convertir les
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

/* Proc�dure utilis�e lorsque le programme a plant�
   => revient sous windows et affiche le message
    - le param�tre gravit� permet de savoir si l'on doit oui ou non refermer l'application
        0  : Aucune information sur le type d'erreur occasionn�e
        1  : Erreur passag�re => n'affiche rien et laisse le programme continuer;
        2  : Laisse � l'utilisateur le libre de choix de continuer <=> pas trop grave
        99 : Erreur critique => le programme doit s'arr�ter
*/
void MessageError(char *from,char *Erreur,int Gravite)
{
    #ifdef DDLIB_DEBUG
    switch (Gravite)
    {
      case 0 : return;   //  ????
      case 1 : return;  // Ca passe ...
      case 2 : if (MessageBoxA(NULL,Erreur,from,1)==IDYES) return;
               else Application->Terminate(); break; // laisse le choix

      // Erreur critique : message et sort !
      case 99 : MessageBoxA(NULL,Erreur,from,1); Application->Terminate(); break;
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
       	case DDERR_ALREADYINITIALIZED  : Erreur="L'objet est d�j� initialis�"; break;
        case DDERR_CANNOTATTACHSURFACE : Erreur="La surface ne peut �tre attach�e � la surface d�sir�e"; break;
        case DDERR_CANNOTDETACHSURFACE : Erreur="La surface ne peut �tre d�sattach�e de la surface d�sir�e"; break;

    	default : Erreur="Erreur inconnue !"; break;
    }
  #ifdef DDLIB_DEBUG
  MessageError("Erreur DirectDraw",Erreur.c_str(),Gravite);
  Application->Terminate();
  #endif
}






//============================================================================//
//                             GESTION DES SPRITES                            //
//============================================================================//




//========================== Fonctions =======================================//

////////////////////////////////////////////////////////////////////////////////
// Fonction Objet : SPRITEFRAME                                               //
////////////////////////////////////////////////////////////////////////////////
SpriteFrame::SpriteFrame(TDirectScreen *pScreen,int Width,int Height, SURFACEMEM surfMem)
:DDSurface(pScreen,Width,Height, surfMem)
{   NumFrame=0;
    NbSprites=0;
    SF_Suiv = SF_Prec = NULL;
    IDFrame = 0;   // D�fini � z�ro si pas donn�
    BmpAlwaysIntoMemory = true;
    BmpLoadIntoMemory = false;
    Semaphore = 0;
 	iStartSpriteIndex = -1;
}
SpriteFrame::SpriteFrame(LPDIRECTDRAW4 *pDD4,int Width,int Height, SURFACEMEM surfMem)
:DDSurface(pDD4,Width,Height, surfMem)
{   NumFrame=0;
    NbSprites=0;
    SF_Suiv = SF_Prec = NULL;
    IDFrame = 0;   // D�fini � z�ro si pas donn�
    BmpAlwaysIntoMemory = true;
    BmpLoadIntoMemory = false;
    Semaphore = 0;
	iStartSpriteIndex = -1;    
}
//----------------------------------------------------------------------------//

//=== Constructeur n'allouant pas la m�moire pour IDirectDrawSurface =========//
SpriteFrame::SpriteFrame(int Width,int Height)
: DDSurface(Width,Height)
{  NumFrame=0;
   NbSprites=0;
   SF_Suiv = SF_Prec = NULL;
   IDFrame = 0;   // D�fini � z�ro si pas donn�
   BmpAlwaysIntoMemory = true;
   BmpLoadIntoMemory = false;
   Semaphore = 0;
  iStartSpriteIndex = -1;   
}
//----------------------------------------------------------------------------//

void SpriteFrame::CreateFrame(TDirectScreen *Screen,void* STLperso,const char* BitmapName,const char* SpiName,int NbFrame,SpriteFrame *New_Frame, int i_IDFrame,bool BmpToMem)
{   FILE *f;
    FrInfo SpiInfo;
    FrInfo* Spi = &SpiInfo;

    // Assigne son num�ro d'identification � la frame
    IDFrame = i_IDFrame;

    // V�rifie l'�tat des noms des fichiers
    if ((BitmapName==NULL) || (SpiName==NULL))
            MessageError("SpriteFrame Constructor","Noms de fichiers non valides",99);

    // Charge le SPI
    AnsiString pathSpi = CheminApplication + SpiName;
    if ((f = fopen(pathSpi.c_str(),"rb"))==NULL) MessageError("Sprite Constructor","N'arrive pas � ouvrir le fichier",99);
    fread(Spi,sizeof(SpiInfo),1,f);

    // Informations de SpriteFrame
    strcpy(Description,SpiInfo.Description);
    NbSprites = SpiInfo.NbSprites;
    CoulFond = SpiInfo.CoulFond;
    CoulOmbr = SpiInfo.CoulOmbr;
    cfRGB = SpiInfo.cfRGB;
    coRGB = SpiInfo.coRGB;

    // Charge le Bitmap (si demand�)
    AnsiString pathBmp = CheminApplication + BitmapName;
    if (BmpToMem)
    {  pDDSurf = DDLoadBitmap(Screen->pDD,pathBmp.c_str(),PixWidth,PixHeight);
       BmpLoadIntoMemory = true;                // bmp charg� en m�moire
       BmpAlwaysIntoMemory = true;
       // Couleur transparente suivant la r�solution
       switch (SCREEN_BITS)
       { case  8 : ColorKey(CoulFond,CoulFond);break;
         case 16 : ColorKey(RGBto16(cfRGB),RGBto16(cfRGB)); break;
         case 24 : ColorKey(cfRGB,cfRGB);break;
         default : ColorKey(0,0);  // Noir ou 1�re couleur de la palette
       }

    } else
    {  BmpLoadIntoMemory = false;               // bmp non charg� en m�moire
       BmpAlwaysIntoMemory = false;
       BitmapPathName = new char [strlen(pathBmp.c_str())+1];
       strcpy(BitmapPathName,pathBmp.c_str());  // conserve le nom du bitmap
    }

    // Informations de DDSurface
    DestRect.top = DestRect.left = SrcRect.top = SrcRect.left = 0;
    DestRect.bottom = SrcRect.bottom = PixHeight;
    DestRect.right = SrcRect.right = PixWidth;

    //--- On charge
    if (STLperso!=NULL)
    {	// --- S'occupe de r�partir les sprites ds leur librairie ---
    	SprInfo SI;
    	SprInfo* SInfo = &SI;
    	for (int compt=0; compt<NbSprites; compt++)
    	{  fread(SInfo,sizeof(SprInfo),1,f);
           ((SpriteTileLib*) STLperso)->AddSprite(SI,CoulFond,CoulOmbr,cfRGB,coRGB,NbFrame,New_Frame);
    	}
    }
}

void SpriteFrame::CreateFrame(LPDIRECTDRAW4 *pDD4,void* STLperso,const char* BitmapName,const char* SpiName,int NbFrame,SpriteFrame *New_Frame, int i_IDFrame,bool BmpToMem)
{   FILE *f;
    FrInfo SpiInfo;
    FrInfo* Spi = &SpiInfo;

    // Assigne son num�ro d'identification � la frame
    IDFrame = i_IDFrame;

    // V�rifie l'�tat des noms des fichiers
    if ((BitmapName==NULL) || (SpiName==NULL))
            MessageError("SpriteFrame Constructor","Noms de fichiers non valides",99);

    // Charge le SPI
    AnsiString pathSpi = CheminApplication + SpiName;
    if ((f = fopen(pathSpi.c_str(),"rb"))==NULL) MessageError("Sprite Constructor","N'arrive pas � ouvrir le fichier",99);
    fread(Spi,sizeof(SpiInfo),1,f);

    // Informations de SpriteFrame
    strcpy(Description,SpiInfo.Description);
    NbSprites = SpiInfo.NbSprites;
    CoulFond = SpiInfo.CoulFond;
    CoulOmbr = SpiInfo.CoulOmbr;
    cfRGB = SpiInfo.cfRGB;
    coRGB = SpiInfo.coRGB;

    // Charge le Bitmap (si demand�)
    AnsiString pathBmp = CheminApplication + BitmapName;
    if (BmpToMem)
    {  pDDSurf = DDLoadBitmap(*pDD4,pathBmp.c_str(),PixWidth,PixHeight);
       BmpLoadIntoMemory = true;                // bmp charg� en m�moire
       BmpAlwaysIntoMemory = true;
       // Couleur transparente suivant la r�solution
       switch (SCREEN_BITS)
       { case  8 : ColorKey(CoulFond,CoulFond);break;
         case 16 : ColorKey(RGBto16(cfRGB),RGBto16(cfRGB)); break;
         case 24 : ColorKey(cfRGB,cfRGB);break;
         default : ColorKey(0,0);  // Noir ou 1�re couleur de la palette
       }

    } else
    {  BmpLoadIntoMemory = false;               // bmp non charg� en m�moire
       BmpAlwaysIntoMemory = false;
       BitmapPathName = new char [strlen(pathBmp.c_str())+1];
       strcpy(BitmapPathName,pathBmp.c_str());  // conserve le nom du bitmap
    }

    // Informations de DDSurface
    DestRect.top = DestRect.left = SrcRect.top = SrcRect.left = 0;
    DestRect.bottom = SrcRect.bottom = PixHeight;
    DestRect.right = SrcRect.right = PixWidth;

    // --- S'occupe de r�partir les sprites ds leur librairie ---
    SprInfo SI;
    SprInfo* SInfo = &SI;

    //--- On charge
    if (STLperso!=NULL)
    {	// --- S'occupe de r�partir les sprites ds leur librairie ---
    	SprInfo SI;
    	SprInfo* SInfo = &SI;
    	for (int compt=0; compt<NbSprites; compt++)
    	{  fread(SInfo,sizeof(SprInfo),1,f);
           ((SpriteTileLib*) STLperso)->AddSprite(SI,CoulFond,CoulOmbr,cfRGB,coRGB,NbFrame,New_Frame);
    	}
    }
}

////////////////////////////////////////////////////////////////////////////////

//=== Charge le bmp dans la IDirectDrawSurface ===============================//
void SpriteFrame::ReserveMemoryAndLoadBmp(LPDIRECTDRAW4 *pDD2)
{ if (!BmpLoadIntoMemory)  // V�rifie quand m�me si le bmp n'est pas d�j� charg�
  {  // Alloue la m�moire n�cessaire avant de charger le bitmap
     AllocateMemory(pDD2,VIDEO_MEMORY);
     // Charge le bmp
     pDDSurf = DDLoadBitmap(*pDD2,BitmapPathName,PixWidth,PixHeight);

     // Couleur transparente suivant la r�solution
       switch (SCREEN_BITS)
       { case  8 : ColorKey(CoulFond,CoulFond);break;
         case 16 : ColorKey(RGBto16(cfRGB),RGBto16(cfRGB)); break;
         case 24 : ColorKey(cfRGB,cfRGB);break;
         default : ColorKey(0,0);  // Noir ou 1�re couleur de la palette
       }
     BmpLoadIntoMemory = true;   // Bmp charg�
  }
}
//----------------------------------------------------------------------------//

//=== D�salloue la m�moire utilis�e par le bmp ===============================//
void SpriteFrame::DesallocateMemory(LPDIRECTDRAW4 *pDD2)
{ if (BmpLoadIntoMemory)         // V�rifie qu'un bmp est bien charg� en m�moire
  { pDDSurf->Release();          // Lib�re la DirectDrawSurface
    pDDSurf=NULL;
    BmpLoadIntoMemory = false;   // M�moire lib�r�e
  }
}
//----------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
// Fonction Objet : SPRITEFRAMELIST                                           //
////////////////////////////////////////////////////////////////////////////////

SpriteFrameList::SpriteFrameList(void)      // Constructeur
{
    SF_List = NULL;                                // Pointe encore sur rien;
    NbFrame = 0;                                   // Encore aucune frame de charg�e
}

// Ajoute une frame � la liste
//  => le num�ro est souvent mis � -1 <=> n� donn� automatiquement � la suite de la liste
SpriteFrame*  SpriteFrameList::AddFrame(TDirectScreen *Screen,void* STLperso,const char* BitmapName,
                                const char* SpiName, int i_IDFrame,bool BmpToMem, bool texture)
{
    SpriteFrame *New_Node;
    // R�cup�re la dimension de la frame contenue ds le BMP � l'offset 18 et 22
    FILE *f;
    unsigned short BmpWidth,BmpHeight;
    char temp[18];
    AnsiString path = CheminApplication + BitmapName;
    if ((f = fopen(path.c_str(),"rb"))==NULL) MessageError("SpriteFrameList::AddFrame",path.c_str(),99);
    fread(temp,18,1,f); fread(&BmpWidth,2,1,f);
    fread(temp,02,1,f); fread(&BmpHeight,2,1,f);
    fclose(f);

    // Si on doit charger le bmp en m�moire
    if (BmpToMem)
    { // Cr�e une nouvelle Frame de la taille requise ( dans l'�diteur seulement )
      if (!texture) New_Node = new SpriteFrame(Screen,BmpWidth,BmpHeight,VIDEO_MEMORY);
      else          New_Node = new SpriteFrame(Screen,BmpWidth,BmpHeight,TEXTURE_MEMORY);
    } else
    { New_Node = new SpriteFrame(BmpWidth,BmpHeight);
    };
    if (New_Node == NULL) MessageError("Probl�me de m�moire","SpriteFrameList::AddFrame",99);
    New_Node->CreateFrame(Screen,STLperso,BitmapName,SpiName,NbFrame,New_Node,i_IDFrame,BmpToMem);

    // Ajoute cette derni�re � la liste
    New_Node->SF_Suiv = SF_List;
    New_Node->SF_Prec = NULL;
    if (SF_List!=NULL) SF_List->SF_Prec = New_Node;
    SF_List = New_Node;

    // Change quelques param�tres;
    SF_List->NumFrame = NbFrame;  // n� de frame suivante
    NbFrame++;

    return New_Node;
};
////////////////////////////////////////////////////////////////////////////////

// AddFrame pour les Editeurs
//  => le num�ro est souvent mis � -1 <=> n� donn� automatiquement � la suite de la liste
SpriteFrame*  SpriteFrameList::AddFrame(LPDIRECTDRAW4  *pDD4,void* STLperso,const char* BitmapName,
                                const char* SpiName, int i_IDFrame,bool BmpToMem, bool texture)
{
    SpriteFrame *New_Node;
   // R�cup�re la dimension de la frame contenue ds le BMP � l'offset 18 et 22
    FILE *f;
    unsigned short BmpWidth,BmpHeight;
    char temp[18];
    AnsiString path = CheminApplication + BitmapName;
    if ((f = fopen(path.c_str(),"rb"))==NULL) MessageError("SpriteFrameList::AddFrame",path.c_str(),99);
    fread(temp,18,1,f); fread(&BmpWidth,2,1,f);
    fread(temp,02,1,f); fread(&BmpHeight,2,1,f);
    fclose(f);

    // Si on doit charger le bmp en m�moire
    if (BmpToMem)
    { // Cr�e une nouvelle Frame de la taille requise ( dans l'�diteur seulement )
      if (!texture) New_Node = new SpriteFrame(pDD4,BmpWidth,BmpHeight,VIDEO_MEMORY);
      else         New_Node = new SpriteFrame(pDD4,BmpWidth,BmpHeight,TEXTURE_MEMORY);
    } else
    { New_Node = new SpriteFrame(BmpWidth,BmpHeight);
    };
    if (New_Node == NULL) MessageError("Probl�me de m�moire","SpriteFrameList::AddFrame",99);
    New_Node->CreateFrame(pDD4,STLperso,BitmapName,SpiName,NbFrame,New_Node,i_IDFrame,BmpToMem);

    // Ajoute cette derni�re � la liste
    New_Node->SF_Suiv = SF_List;
    New_Node->SF_Prec = NULL;
    if (SF_List!=NULL) SF_List->SF_Prec = New_Node;
    SF_List = New_Node;

    // Change quelques param�tres;
    SF_List->NumFrame = NbFrame;  // n� de frame suivante
    NbFrame++;

    return New_Node;
};
//----------------------------------------------------------------------------//

//=== Renvoie un ptr sur la frame ayant cet ID ===============================//
SpriteFrame* SpriteFrameList::GetFrame(int numIDFrame)
{   SpriteFrame* frame = SF_List;
    if ((frame!=NULL) && (frame->IDFrame!=numIDFrame)) frame = frame->SF_Suiv;
    return frame;
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
    NumFrame = NumSprite = 0;
    TypeSprite = SPRITE_TILE_2D;
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
};

// Rajoute un sprite de PERSONNAGE � la liste
// classe d�riv�e SPRITETILEPERSOLIB 
void SpriteTilePersoLib::AddSprite(SprInfo InfSprite,int CF,int CO,unsigned int RGBcf,unsigned int RGBco
                              ,int NumFr,SpriteFrame *pFr)
{
    SpriteTile *New_Node;

    // Cr�e un nouveau Sprite
    New_Node = new SpriteTilePerso();
    #ifdef DDLIB_DEBUG
    if (New_Node == NULL)
    { MessageError("Probl�me de m�moire","SpriteLib::AddSprite",99);
     Application->Terminate();
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

    // Ajoute ce sprite � la liste
    New_Node->Suiv = SL_List;
    New_Node->Prec = NULL;
    if (SL_List!=NULL) SL_List->Prec = New_Node;
    SL_List = New_Node;

    NbSprites++; // Augmente le compteur de la liste
};

// Rajoute un sprite NORMAL � la liste
void SpriteTileLib::AddSprite(SprInfo InfSprite,int CF,int CO,unsigned int RGBcf,unsigned int RGBco
                              ,int NumFr,SpriteFrame *pFr)
{
    SpriteTile *New_Node;

    // Cr�e un nouveau Sprite 3D ou 2D
    if (pFr->isTextureDirect3D)
    { New_Node = new Sprite3DTile();
      ((Sprite3DTile*) New_Node)->Create3DSprite(D3DS->Get3DDevice(), 0x00001234,0x00001234, 0, 0, 1, 1, *pFr); 
    }
    else
    { New_Node = new SpriteTile();
    }

    #ifdef DDLIB_DEBUG
    if (New_Node == NULL)
    {  MessageError("Probl�me de m�moire","SpriteLib::AddSprite",99);
       Application->Terminate();
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

    // Ajoute ce sprite � la liste
    New_Node->Suiv = SL_List;
    New_Node->Prec = NULL;
    if (SL_List!=NULL) SL_List->Prec = New_Node;
    SL_List = New_Node;

    NbSprites++; // Augmente le compteur de la liste
};

// Cr�e un tableau dynamique de pointers sur des sprites accessible par le n� du sprite
void SpriteTileLib::CreateSprIndex(void)
{
    // V�rifie si SprIndex est d�j� utilis�
    if (SprIndex!=NULL)  free(SprIndex); // Ne peut lib�rer la m�moire => � n'utiliser qu'1 fois

    // Alloue la m�moire n�cessaire vers le tableau
    if ((SprIndex = (SpriteTile**) malloc(sizeof(SpriteTile*) * NbSprites)) == NULL)
    		MessageError("Probl�me de m�moire","SpriteTileLib::CreateSprIndex",99);

    // Y range les adresses des sprites
    SpriteTile* Sprite;
    Sprite = SL_List; // dernier sprite
    for (int i = NbSprites-1  ;  i >=0  ; i--)
       {
       	  SprIndex[i]=Sprite;
          Sprite = (SpriteTile*) Sprite->Suiv;
       };
};

////////////////////////////////////////////////////////////////////////////////

//======================= Fonctions annexes ==================================//

// SpriteDrawFast //////////////////////////////////////////////////////////////
// Dessine le sprite n� ??? dans la surface ??? en utilisant la transparence  //
void SpriteDrawFast(int ScX,int ScY,int NumSprite,DDSurface *pSurface,SpriteTileLib* STLperso)
{
    if ((NumSprite<0) || (STLperso==NULL) || (STLperso->SL_List==NULL)) return;  // v�rifications
    if (NumSprite>=STLperso->NbSprites) return;

    // Remonte jusqu'au bon sprite de la SpriteListe
    SpriteTile *Sprite;
    HRESULT hr;
    // Sans "SprIndex", j'utilisais �� :
    // Sprite=STL->SL_List; for (int i=1; i < STL->NbSprites-NumSprite ; i++) Sprite=Sprite->Suiv;
    Sprite = STLperso->SprIndex[NumSprite];
    hr = pSurface->pDDSurf->BltFast(ScX,ScY,Sprite->pFrame->pDDSurf,&(Sprite->SrcRect),DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
    if FAILED(hr) if (!pSurface->CheckSurfaces()) TraceErrorDD(hr,99);
};
////////////////////////////////////////////////////////////////////////////////

// Sprite3DDraw ////////////////////////////////////////////////////////////////
void Sprite3DDraw(int ScX,int ScY,int NumSprite,DDSurface *pSurface,SpriteTileLib* STLperso)
{ if ((NumSprite<0) || (STLperso==NULL) || (STLperso->SL_List==NULL)) return;  // v�rifications
    if (NumSprite>=STLperso->NbSprites) return;
    // Remonte jusqu'au bon sprite de la SpriteListe
    Sprite3DTile *Sprite3D;
    HRESULT hr;
    Sprite3D = (Sprite3DTile*) STLperso->SprIndex[NumSprite];
    Sprite3D->SpriteRender(ScX, ScY);
    //hr = pSurface->pDDSurf->BltFast(ScX,ScY,Sprite->pFrame->pDDSurf,&(Sprite->SrcRect),DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
    //if FAILED(hr) if (!pSurface->CheckSurfaces()) TraceErrorDD(hr,99);
}
////////////////////////////////////////////////////////////////////////////////

// SpriteDraw //////////////////////////////////////////////////////////////////
void SpriteDraw(int ScX,int ScY,int NumSprite,DDSurface *pSurface,SpriteTileLib* STLperso)
{
    if ((NumSprite<0) || (STLperso==NULL) || (STLperso->SL_List==NULL)) return;  // v�rifications
    if (NumSprite>=STLperso->NbSprites) return;

    // Remonte jusqu'au bon sprite de la SpriteListe
    SpriteTile *Sprite;
    HRESULT hr;
    // Sans "SprIndex", j'utilisais �� :
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
float CGeneralSpriteInfos::Version = 1.00;               // n� de version

//=== Constructeur de la classe ----------------------------------------------//
CGeneralSpriteInfos::CGeneralSpriteInfos()
{

};
//----------------------------------------------------------------------------//

//=== Sauvegarde les informations contenue de cette structure ================//
void CGeneralSpriteInfos::SaveToFile(FILE &fichier)
{  // Sauve le rectangle de la zone au sol
   ZoneSol.SaveToFile(fichier);
   // Sauve ke centre de gravit� du perso projet� au sol
   GravSol.SaveToFile(fichier);
   // Sauve la liste des sprites superposables
   ListAnimSuperp.SaveToFile(fichier);
   // Sauve le rectangle de la zone aux pieds
   ZonePied.SaveToFile(fichier);
   // Sauve les donn�es tampons
   for (int i=0 ; i < 10 ; i++) fwrite(&temp[i],sizeof(int),1,&fichier);}
//----------------------------------------------------------------------------//

//=== Charge les informations contenue de cette structure ====================//
void CGeneralSpriteInfos::LoadFromFile(FILE &fichier)
{  // Charge le rectangle de la zone au sol
   ZoneSol.LoadFromFile(fichier);
   // Charge le centre de gravit� du perso projet� au sol
   GravSol.LoadFromFile(fichier);
   // Charge la liste des sprites superposables
   ListAnimSuperp.LoadFromFile(fichier);
   // Charge le rectangle de la zone aux pieds
   ZonePied.LoadFromFile(fichier);
   // Charge les donn�es tampons
   for (int i=0 ; i < 10 ; i++) fread(&temp[i],sizeof(int),1,&fichier);
}
//----------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////////////////

// struct ZONERECT /////////////////////////////////////////////////////////////
//=== Sauvegarde les 4 coordonn�es des 2 points du rectangle =================//
void ZoneRect::SaveToFile(FILE &fichier)
{  fwrite(&x1,sizeof(x1), 1, &fichier);
   fwrite(&y1,sizeof(y1), 1, &fichier);
   fwrite(&x2,sizeof(x2), 1, &fichier);
   fwrite(&y2,sizeof(y2), 1, &fichier);
}
//----------------------------------------------------------------------------//

//=== Charge les 4 coordonn�es des 2 points du rectangle =====================//
void ZoneRect::LoadFromFile(FILE &fichier)
{  fread(&x1,sizeof(x1), 1, &fichier);
   fread(&y1,sizeof(y1), 1, &fichier);
   fread(&x2,sizeof(x2), 1, &fichier);
   fread(&y2,sizeof(y2), 1, &fichier);

   // Remplit le champs Enabled
   if ((x1!=-1) || (y1!=-1) || (x2!=-1) || (y2!=-1)) Enabled = true;
}
//----------------------------------------------------------------------------//
////////////////////////////////////////////////////////////////////////////////

// struct ZONEORIENTRECT ///////////////////////////////////////////////////////
//=== Sauvegarde les 4 coordonn�es des 2 points du rectangle =================//
void ZoneOrientRect::SaveToFile(FILE &fichier)
{  fwrite(&x1,sizeof(x1), 1, &fichier); fwrite(&y1,sizeof(y1), 1, &fichier);
   fwrite(&x2,sizeof(x2), 1, &fichier); fwrite(&y2,sizeof(y2), 1, &fichier);
   fwrite(&x3,sizeof(x3), 1, &fichier); fwrite(&y3,sizeof(y3), 1, &fichier);
   fwrite(&x4,sizeof(x4), 1, &fichier); fwrite(&y4,sizeof(y4), 1, &fichier);
}
//----------------------------------------------------------------------------//

//=== Charge les 4 coordonn�es des 2 points du rectangle =====================//
void ZoneOrientRect::LoadFromFile(FILE &fichier)
{  fread(&x1,sizeof(x1), 1, &fichier); fread(&y1,sizeof(y1), 1, &fichier);
   fread(&x2,sizeof(x2), 1, &fichier); fread(&y2,sizeof(y2), 1, &fichier);
   fread(&x3,sizeof(x3), 1, &fichier); fread(&y3,sizeof(y3), 1, &fichier);
   fread(&x4,sizeof(x4), 1, &fichier); fread(&y4,sizeof(y4), 1, &fichier);

   // Remplit le champs Enabled
   if (   (x1!=-1) || (y1!=-1) || (x2!=-1) || (y2!=-1)
       || (x3!=-1) || (y3!=-1) || (x4!=-1) || (y4!=-1) ) Enabled = true;
}
//----------------------------------------------------------------------------//
////////////////////////////////////////////////////////////////////////////////


// struct ZONEPOINT ////////////////////////////////////////////////////////////
//=== Sauvegarde les 2 coordonn�es ===========================================//
void ZonePoint::SaveToFile(FILE &fichier)
{  fwrite(&x,sizeof(x), 1, &fichier);
   fwrite(&y,sizeof(y), 1, &fichier);
}
//----------------------------------------------------------------------------//

//=== Charge les 2 coordonn�es du point ======================================//
void ZonePoint::LoadFromFile(FILE &fichier)
{  fread(&x,sizeof(x), 1, &fichier);
   fread(&y,sizeof(y), 1, &fichier);

//   Remplit le champs Enabled
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

//=== Sauvegarde le tableau d'anim superpos�es + un entier d'en-t�te =========//
void ZoneAnimSuperp::SaveToFile(FILE &fichier)
{  // En-t�te : entier (int) indiquant le nombre d'�l�ment du vecteur
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

//=== Charge le tableau d'anims superpos�es ==================================//
void ZoneAnimSuperp::LoadFromFile(FILE &fichier)
{  // Lit l'en-t�te
   int nbElem=0;
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
//----------------------------------------------------------------------------//

//=== Renvoie vrai si le sAnimSuperp correspondant � ce n� existe ============//
bool ZoneAnimSuperp::Existe(int num_ID)
{ for (int i=0; i < TabAnimSuperp.size() ; i++)
  { if (TabAnimSuperp[i].numIDTypeObjSprite == num_ID) return true;
  }
  return false;
}
//----------------------------------------------------------------------------//

//=== Renvoie les coords du sprite de n�num_ID ===============================//
void ZoneAnimSuperp::GetCoord(int num_ID, int &xs,int &ys)
{ for (int i=0; i < TabAnimSuperp.size() ; i++)
  { if (TabAnimSuperp[i].numIDTypeObjSprite == num_ID)
    { xs = TabAnimSuperp[i].x;
      ys = TabAnimSuperp[i].y;
      return;
    }
  }  // Si on arrive ici, c'est que le sprite n'existe pas
}
//----------------------------------------------------------------------------//


//=== Renvoie les coords du sprite de n�num_ID ===============================//
void ZoneAnimSuperp::SetCoord(int num_ID, int &xs,int &ys)
{ for (int i=0; i < TabAnimSuperp.size() ; i++)
  { if (TabAnimSuperp[i].numIDTypeObjSprite == num_ID)
    { TabAnimSuperp[i].x = xs;
      TabAnimSuperp[i].y = ys;
      return;
    }
  }  // Si on arrive ici, c'est que le sprite n'existe pas
}
//----------------------------------------------------------------------------//

//=== Renvoie le n�ID du sprite superposable correspondant au n�ID du type d'objet
int ZoneAnimSuperp::GetIDS(int num_IDT)
{ for (int i=0; i < TabAnimSuperp.size() ; i++)
  { if (TabAnimSuperp[i].numIDTypeObjSprite == num_IDT)
    { return TabAnimSuperp[i].numIDSpriteSuperpose;
    }
  }
  return 0;
}
//----------------------------------------------------------------------------//


////////////////////////////////////////////////////////////////////////////////

