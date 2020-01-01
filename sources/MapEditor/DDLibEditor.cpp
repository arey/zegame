////////////////////////////////////////////////////////////////////////////////
//  DDLibEditor.cpp : Librairie utilisant des fonctions de DirectDraw		  //
//  				  prolongement de DDlib.cpp en windowed-mode              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include "DDLib.h"
#include "DDLibEditor.h"
#include "consts.h"


// Déclarations des données utilisées par l'éditeur
float ViewZoom = 1.0;		// Zoom sur la vue du jeu

// DirectScreen View Editor : fenêtre représentant la vue du jeu
TDirectScreen_Editor  *DS_VE = NULL;
// DirectScreen Map Editor : fenêtre représentant la vue de dessus
TDirectScreen_Editor  *DS_ME = NULL;



////////////////////////////////////////////////////////////////////////////////
// Fonctions Objet : TDIRECTSCREEN_EDITOR                                            //
////////////////////////////////////////////////////////////////////////////////

// Constructeur --------------------------------------------------------------//
TDirectScreen_Editor::TDirectScreen_Editor(HWND Hand)
{
    // Initialisation des variables
    pDD = NULL;
    pDDSPrimary = NULL;
    pDDSBack = NULL;

    DDHandle = Hand;
}
//----------------------------------------------------------------------------//

// Destructeur ---------------------------------------------------------------//
TDirectScreen_Editor::~TDirectScreen_Editor(void)
{
    if (pDD!=NULL) pDD->RestoreDisplayMode();
    if (pDDSPrimary!=NULL) delete pDDSPrimary;
    if (pDDSBack!=NULL) delete pDDSBack;
    pDD->Release();
}
//----------------------------------------------------------------------------//

// A utiliser lorsque l'on ouvre une nouvelle fenêtre, -----------------------//
//		par exemple aprés un changement de résolution
void  TDirectScreen_Editor::InitDDraw(int ScrWidth, int ScrHeight)
{
    HRESULT hr=0;   // gère les erreurs

    Screen_Width = ScrWidth;
    Screen_Height = ScrHeight;
    Screen_Bits = EditorBits;

    SCREEN_WIDTH =  ScrWidth;
    SCREEN_HEIGHT = ScrHeight;

   /* if (FAILED(CoInitialize(NULL))) asm int 3;
    // Crée l'objet COM DirectDraw
   LPDIRECTDRAW lpDD=NULL;
    hr = DirectDrawCreate(NULL,&lpDD,NULL);
    if FAILED(hr) TraceErrorDD(hr,99);
    hr = lpDD->QueryInterface(IID_IDirectDraw4,(LPVOID*) &pDD);
    if FAILED(hr) TraceErrorDD(hr,99);
    lpDD->Release();    */

    if (FAILED(CoInitialize(NULL))) asm int 3;
    hr = CoCreateInstance(CLSID_DirectDraw,
    	NULL, CLSCTX_ALL,
        IID_IDirectDraw4,
        (void**) &pDD);
    if(FAILED(hr)) asm int 3;
    hr = IDirectDraw4_Initialize(pDD, NULL);
    if(FAILED(hr)) asm int 3;

    // Défini un niveau de coopérationon exclusif afin de pouvoir changer le mode d'affichage
    hr = pDD->SetCooperativeLevel(DDHandle,DDSCL_NORMAL);
    if (FAILED(hr)) TraceErrorDD(hr,99);  // s'arrête si windows ne veut coopérer
    // Change la résolution en SCREEN_WIDTH x SCREEN_HEIGHT x SCREEN_BITS  full-screen mode
    hr = pDD->SetDisplayMode(EditorWidth, EditorHeight, EditorBits,0,0);
    if (FAILED(hr)) TraceErrorDD(hr,99);  // ne peut pas non plus continuer


    LPDIRECTDRAWSURFACE4 lpSurf;
    // Crée la Surface Primaire
    pDDSPrimary = new DDSurface(Screen_Width,Screen_Height);
    memset(&pDDSPrimary->DDSurfDesc,0,sizeof(pDDSPrimary->DDSurfDesc));  // met tout à zéro
    pDDSPrimary->DDSurfDesc.dwSize = sizeof(pDDSPrimary->DDSurfDesc);    // taille de sa propre strucutre
    pDDSPrimary->DDSurfDesc.dwFlags = DDSD_CAPS;
    pDDSPrimary->DDSurfDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    pDD->CreateSurface(&pDDSPrimary->DDSurfDesc,&lpSurf,NULL);
    if FAILED(hr) TraceErrorDD(hr,99);
    hr = lpSurf->QueryInterface(IID_IDirectDrawSurface4, (LPVOID*) &pDDSPrimary->pDDSurf);
    if FAILED(hr) TraceErrorDD(hr,99);

    // Lui attache une surface secondaire
    pDDSBack    = new DDSurface(Screen_Width,Screen_Height);
    memset(&pDDSBack->DDSurfDesc,0,sizeof(pDDSBack->DDSurfDesc));  // met tout à zéro
    pDDSBack->DDSurfDesc.dwSize   = sizeof(pDDSBack->DDSurfDesc);    // taille de sa propre strucutre
    pDDSBack->DDSurfDesc.dwFlags  = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
    pDDSBack->DDSurfDesc.dwWidth  = ScrWidth;
    pDDSBack->DDSurfDesc.dwHeight = ScrHeight;
    pDDSBack->DDSurfDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
    lpSurf = NULL;
    pDD->CreateSurface(&pDDSBack->DDSurfDesc,&lpSurf,NULL);
    if FAILED(hr) TraceErrorDD(hr,99);
    if (lpSurf == NULL) Application->Terminate();
    hr = lpSurf->QueryInterface(IID_IDirectDrawSurface3, (LPVOID*) &pDDSBack->pDDSurf);
    if FAILED(hr) TraceErrorDD(hr,99);
    lpSurf->Release();

    // en Windowed-mode, on ne peut pas attacher la surface arrière au 1er plan

    // Défini le Clipper de la fenêtre
    hr = pDD->CreateClipper(0,&pDDClipper,NULL);
    if (FAILED(hr)) TraceErrorDD(hr,99);
    hr = pDDClipper->SetHWnd(0,DDHandle);
    if (FAILED(hr)) TraceErrorDD(hr,99);
    hr = pDDSPrimary->pDDSurf->SetClipper(pDDClipper);
    if (FAILED(hr)) TraceErrorDD(hr,99);
};
//----------------------------------------------------------------------------//

// Flip les surfaces Principales/Backbuffer ----------------------------------//
HRESULT TDirectScreen_Editor::FlipSurfaces()
{
	HRESULT hr;
    RECT rcSrc;
    RECT rcDest;
    POINT p;
    // On s'assure d'abord que la surface ne soit pas perdue
    if (pDDSPrimary->CheckSurfaces())
     {
     	p.x = p.y = 0;
        ::ClientToScreen(DDHandle,&p);
        SetRect(&rcSrc,0,0,Screen_Width,Screen_Height);
        SetRect(&rcDest,p.x,p.y,p.x+Screen_Width * ViewZoom,p.y + Screen_Height * ViewZoom);
        hr = pDDSPrimary->pDDSurf->Blt(&rcDest,pDDSBack->pDDSurf,&rcSrc,DDBLT_WAIT,NULL);
        if (FAILED(hr)) TraceErrorDD(hr,1);
        return 1; // Success
     };
    return 0; // Faillure
};
//----------------------------------------------------------------------------//

// Affiche un pixel sur la surface arrière ===================================//
void TDirectScreen_Editor::PutBackPixel(int X, int Y, unsigned int Color)
{
    WORD *Bitmap = (WORD*) pDDSBack->DDSurfDesc.lpSurface;
    switch (Screen_Bits)
     {
        case 16 : Bitmap[Y * ScreenViewX + X]= (WORD) Color; break;
        default : MessageBoxA(NULL," Mettez vous en mode graphiqe 16 bits","Changement de mode",0); return;
     };

};

// Affiche du texte sur la surface arrière ===================================//
void TDirectScreen_Editor::PutBackText(int X, int Y, unsigned int Color, LPCTSTR Texte)
{  pDDSBack->TextSurf(X, Y, Color, Texte);
};


// Efface l'écran d'une couleur spécifique -----------------------------------//
void TDirectScreen_Editor::Fill(DWORD ColFill)
{
    DDBLTFX ddBltFx;

    ddBltFx.dwSize = sizeof(DDBLTFX);
    ddBltFx.dwFillColor = ColFill;
    HRESULT hr = pDDSBack->pDDSurf->Blt(NULL,NULL,NULL,DDBLT_WAIT | DDBLT_COLORFILL,&ddBltFx);
}
//----------------------------------------------------------------------------//

// MAPEDITOR
void Swap(int *a, int*b)  // échange 2 integers  (utilisé par l'algo de lignes)
{
	int Temp = *a;
    *a = *b;
    *b = Temp;
};

//== Trace une ligne sur l'écran par interpolation ===========================//
void TDirectScreen_Editor::BackLine(int X1, int Y1, int X2, int Y2, int Col)
{
	double xStep, yStep, X, Y;
    int xLength, yLength, xCount, yCount;
    xLength = abs(X2-X1);  yLength = abs(Y2-Y1);

    pDDSBack->Lock();
    if (xLength > yLength)
     {
     	if (X1>X2) { Swap(&X1, &X2); Swap(&Y1, &Y2);  }
        yStep = (double) (Y2 - Y1) / (double) (X2 - X1);
        Y = Y1;
        for (xCount = X1; xCount <= X2; xCount++)
        {
            if ((xCount>0) && (xCount<ScreenViewX) && (Y>0) && (Y<ScreenViewY))
                PutBackPixel(xCount,(int) Y, Col);
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
            if ((yCount>0) && (yCount<ScreenViewY) && (X>0) && (X<ScreenViewX))
        		PutBackPixel((int) X, yCount, Col);
            X += xStep;
         };
      };
    pDDSBack->UnLock();
};
//----------------------------------------------------------------------------//

//== Trace un trapèze d'aprés les données ISO ================================//
void TDirectScreen_Editor::BackISOTrapeze(int X, int Y, int Col, int Type)
{
         int X2 = X-ISO_WIDTH_HALF;
         int Y2 = Y+ISO_HEIGHT_HALF;
         BackLine(X, Y, X2, Y2, Col);
         if (Type==TrapTYPE_DOUBLE) BackLine(X, Y+1, X2, Y2+1, Col);
         X = X2; Y = Y2;
         X2+=ISO_WIDTH_HALF;
         Y2+=ISO_HEIGHT_HALF;
         BackLine(X, Y, X2, Y2, Col);
         if (Type==TrapTYPE_DOUBLE) BackLine(X, Y+1, X2, Y2+1, Col);
         X = X2; Y = Y2;
         X2+=ISO_WIDTH_HALF;
         Y2-=ISO_HEIGHT_HALF;
         BackLine(X, Y, X2, Y2, Col);
         if (Type==TrapTYPE_DOUBLE) BackLine(X, Y+1, X2, Y2+1, Col);
         X = X2; Y = Y2;
         X2-=ISO_WIDTH_HALF;
         Y2-=ISO_HEIGHT_HALF;
         BackLine(X, Y, X2, Y2, Col);
         if (Type==TrapTYPE_DOUBLE) BackLine(X, Y+1, X2, Y2+1, Col);
};
//----------------------------------------------------------------------------//

//=== Cercle de centre (X,Y) et de rayon R ===================================//
void TDirectScreen_Editor::BackCircle(int X, int Y, int R, int Col)
{	float x = (float) -R;
    float y;
    float r2 = ((float) R) * R;
    while (x < 0)
    {	y = (float) sqrt(r2 - (x*x));
    	PutBackPixel(X + (int) x, (Y - (int) y), Col);
    	PutBackPixel(X - (int) x, (Y - (int) y), Col);
        PutBackPixel(X + (int) x, (Y + (int) y), Col);
        PutBackPixel(X - (int) x, (Y + (int) y), Col);
 		x += 0.1;
	}
}
//----------------------------------------------------------------------------//
