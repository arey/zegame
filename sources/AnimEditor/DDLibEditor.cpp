////////////////////////////////////////////////////////////////////////////////
//  DDLibEditor.cpp : Librairie utilisant des fonctions de DirectDraw		  //
//  				  prolongement de DDlib.cpp en windowed-mode              //
//                                                                            //
//  Last uprade : 29/11/1998												  //
////////////////////////////////////////////////////////////////////////////////
//#define INITGUID
#include "DDLib.h"
#include "DDLibEditor.h"
#include "animall.h"
#include "consts.h"


// Déclarations des données utilisées par l'éditeur
// DirectScreen View Editor : fenêtre représentant la vue du jeu
TDirectScreen_Editor  *DS_VE = NULL;
// DirectScreen Map Editor : fenêtre représentant la vue de dessus
TDirectScreen_Editor  *DS_ME = NULL;
// DirectScreen View Editor : fenêtre représentant la vue d'une animation
TDirectScreen_Editor  *DS_VA = NULL;


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

    // Crée l'objet COM DirectDraw
    /*LPDIRECTDRAW lpDD;
    hr = DirectDrawCreate(NULL,&lpDD,NULL);
    if FAILED(hr) TraceErrorDD(hr,99);
    hr = lpDD->QueryInterface(IID_IDirectDraw4,(LPVOID*) &pDD);
    if FAILED(hr) TraceErrorDD(hr,99);
    lpDD->Release();*/
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
 //   hr = pDD->SetDisplayMode(EditorWidth, EditorHeight, EditorBits,0,0);
 //   if (FAILED(hr)) TraceErrorDD(hr,99);  // ne peut pas non plus continuer


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
    pDD->CreateSurface(&pDDSBack->DDSurfDesc,&lpSurf,NULL);
    if FAILED(hr) TraceErrorDD(hr,99);
    hr = lpSurf->QueryInterface(IID_IDirectDrawSurface4, (LPVOID*) &pDDSBack->pDDSurf);
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
        SetRect(&rcDest,p.x,p.y,p.x+Screen_Width,p.y + Screen_Height);
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
        case 16 : Bitmap[Y *  pDDSBack->DDSurfDesc.lPitch+ X]= (WORD) Color; break;
        default : MessageBoxA(NULL," Mettez vous en mode graphiqe 16 bits","Changement de mode",0); return;
     };

};

// Efface l'écran d'une couleur spécifique -----------------------------------//
void TDirectScreen_Editor::Fill(DWORD ColFill)
{
    DDBLTFX ddBltFx;

    ddBltFx.dwSize = sizeof(DDBLTFX);
    ddBltFx.dwFillColor = ColFill;
    pDDSBack->pDDSurf->Blt(NULL,NULL,NULL,DDBLT_WAIT | DDBLT_COLORFILL,&ddBltFx);
}
//----------------------------------------------------------------------------//


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

//== Trace une ligne sur l'écran par interpolation ===========================//
// -> Utilise 'PutCarre' à la place de 'PutBackPixel'
void TDirectScreen_Editor::BackLine2(int x1, int y1, int x2, int y2, int Col)
{   int d, dx, dy, aincr, bincr, xincr, yincr, x, y;
    if ( abs(x2-x1) < abs(y2-y1) )    //  {Parcours : par axe X ou Y ? }
    {                               //             { par l'axe des Y }
	if ( y1 > y2 )                //          { y1 sup‚rieur … y2? }
	  {
	    Swap( &x1, &x2 );      //                { Oui, ‚change X1 et X2, }
	    Swap( &y1, &y2 );      //                              { Y1 et Y2 }
	  }

	if ( x2 > x1 )
	  xincr = 1;         		// { Fixe le pas horizontal }
	    else
	     xincr = -1;

	dy = y2 - y1;
	dx = abs( x2-x1 );
	d  = 2 * dx - dy;
	aincr = 2 * (dx - dy);
	bincr = 2 * dx;
	x = x1;
	y = y1;
        PutCarre(x, y, 1, Col);
	for (y=y1+1; y<=y2 ; y++)     //            { Parcourt l'axe des Y }
	  {
	    if ( d >= 0 )
		{
		  x+=xincr;
		  d+=aincr;
		}
	    else d+=bincr;
            PutCarre(x, y, 1, Col);

	  }
    }
  else                                //               { par l'axe des X }
    {
	if ( x1 > x2 )                  //     { x1 plus grand que x2? }
	  {
	    Swap( &x1, &x2 );        //               { Oui, ‚change X1 et X2 }
	    Swap( &y1, &y2 );        //                            { Y1 et Y2 }
	  }

	if ( y2 > y1 )
	  yincr = 1;            			// { Fixe le pas vertical }
	     else
		yincr = -1;

	dx = x2 - x1;
	dy = abs( y2-y1 );
	d  = 2 * dy - dx;
	aincr = 2 * (dy - dx);
	bincr = 2 * dy;
	x = x1;
	y = y1;
        PutCarre(x, y, 1, Col);
	for (x=x1+1; x<=x2 ; x++)       //             { Parcourt l'axe des X }
	  {
	    if ( d >= 0 )
		{
		  y+=yincr;
		  d+=aincr;
		}
	    else
		d+=bincr;
            PutCarre(x, y, 1, Col);
	 }
    }

}
//----------------------------------------------------------------------------//


//== Trace un trapèze d'aprés les données ISO ================================//
void TDirectScreen_Editor::BackISOTrapeze(int X, int Y, int Col, int Type)
{        int X2 = X-ISO_WIDTH_HALF;
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

//== Trace un trapèze d'aprés les données ISO en tenant compte du Zoom =======//
void TDirectScreen_Editor::BackZoomISOTrapeze(int X, int Y, int Col, int Type, int Z)
{        int X2 = X-ISO_WIDTH_HALF*Z;
         int Y2 = Y+ISO_HEIGHT_HALF*Z;
         BackLine2(X, Y, X2, Y2, Col);
         if (Type==TrapTYPE_DOUBLE) BackLine2(X, Y+1, X2, Y2+1, Col);
         X = X2; Y = Y2;
         X2+=ISO_WIDTH_HALF*Z;
         Y2+=ISO_HEIGHT_HALF*Z;
         BackLine2(X, Y, X2, Y2, Col);
         if (Type==TrapTYPE_DOUBLE) BackLine2(X, Y+1, X2, Y2+1, Col);
         X = X2; Y = Y2;
         X2+=ISO_WIDTH_HALF*Z;
         Y2-=ISO_HEIGHT_HALF*Z;
         BackLine2(X, Y, X2, Y2, Col);
         if (Type==TrapTYPE_DOUBLE) BackLine2(X, Y+1, X2, Y2+1, Col);
         X = X2; Y = Y2;
         X2-=ISO_WIDTH_HALF*Z;
         Y2-=ISO_HEIGHT_HALF*Z;
         BackLine2(X, Y, X2, Y2, Col);
         if (Type==TrapTYPE_DOUBLE) BackLine2(X, Y+1, X2, Y2+1, Col);
}
//----------------------------------------------------------------------------//

//=== Affiche un carré tout plein ============================================//
// X et Y sont les coordonnées du point supérieur gauche
// => DECONNE : une ligne sur deux ?
void TDirectScreen_Editor::PutPoint(int X, int Y, int zoom, unsigned int Color)
{
  pDDSBack->Lock();
  for (int i=X; i < X+zoom ; i++)
        for (int j=Y; j < Y+zoom ; j++)
          { if ((i>=0) && (j>=0) && (i<Screen_Width) && (j<Screen_Height))
              PutBackPixel(i, j, Color);
          }
  pDDSBack->UnLock();
};
//----------------------------------------------------------------------------//

//=== Affiche un carré tout plein ============================================//
// X et Y sont les coordonnées du point supérieur gauche
void TDirectScreen_Editor::PutCarre(int X, int Y, int longueur, unsigned int Color)
{
    DDBLTFX ddBltFx;
    ddBltFx.dwSize = sizeof(DDBLTFX);
    ddBltFx.dwFillColor = Color;
    RECT rectDest;
    rectDest.left = X; rectDest.top = Y;
    rectDest.right = X+longueur; rectDest.bottom = Y+longueur;
    pDDSBack->pDDSurf->Blt(&rectDest,NULL,NULL,DDBLT_WAIT | DDBLT_COLORFILL,&ddBltFx);
};
//----------------------------------------------------------------------------//

//=== Affiche une croix ======================================================//
void TDirectScreen_Editor::PutCroix(int X, int Y, int longueur, unsigned int Color)
{   DS_VA->PutCarre(X-longueur,Y,longueur,Color); // à gauche
    DS_VA->PutCarre(X+longueur,Y,longueur,Color); // à droite
    DS_VA->PutCarre(X,Y-longueur,longueur,Color); // en haut
    DS_VA->PutCarre(X,Y+longueur,longueur,Color); // en bas
    DS_VA->PutCarre(X,Y,longueur,Color); // Affiche un carré vert au milieu
}
//----------------------------------------------------------------------------//

//=== Affiche un Rectangle d'une certaine épaisseur ==========================//
void TDirectScreen_Editor::PutRectangle(int x1, int y1, int x2, int y2, int epaisseur, unsigned int Color)
{   // Préparation
    DDBLTFX ddBltFx;
    ddBltFx.dwSize = sizeof(DDBLTFX);
    ddBltFx.dwFillColor = Color;
    RECT rectDest;

    // ordonne les coordonnées avant pour que le 1er point soit en haut à gauche
    if (x1 > x2) { int t=x1; x1=x2; x2=t; }
    if (y1 > y2) { int t=y1; y1=y2; y2=t; }

    // Ligne verticale à partir du point (x1,y1)
    rectDest.left = x1; rectDest.top = y1;
    rectDest.right = x1+epaisseur; rectDest.bottom = y2;
    pDDSBack->pDDSurf->Blt(&rectDest,NULL,NULL,DDBLT_WAIT | DDBLT_COLORFILL,&ddBltFx);
    // Ligne verticale à partir du point (x2,y1)
    rectDest.left = x2; rectDest.top = y1;
    rectDest.right = x2+epaisseur; rectDest.bottom = y2;
    pDDSBack->pDDSurf->Blt(&rectDest,NULL,NULL,DDBLT_WAIT | DDBLT_COLORFILL,&ddBltFx);
    // Ligne horizontale à partir du point (x1,y1)
    rectDest.left = x1; rectDest.top = y1;
    rectDest.right = x2; rectDest.bottom = y1+epaisseur;
    pDDSBack->pDDSurf->Blt(&rectDest,NULL,NULL,DDBLT_WAIT | DDBLT_COLORFILL,&ddBltFx);
    // Ligne horizontale à partir du point (x1,y2)
    rectDest.left = x1; rectDest.top = y2;
    rectDest.right = x2+epaisseur; rectDest.bottom = y2+epaisseur;
    pDDSBack->pDDSurf->Blt(&rectDest,NULL,NULL,DDBLT_WAIT | DDBLT_COLORFILL,&ddBltFx);
};
//----------------------------------------------------------------------------//

//=== Affiche un Rectangle d'une certaine épaisseur avec clipping ============//
void TDirectScreen_Editor::PutRectangleClip(int x1, int y1, int x2, int y2, int epaisseur, unsigned int Color)
{   // Préparation
    DDBLTFX ddBltFx;
    ddBltFx.dwSize = sizeof(DDBLTFX);
    ddBltFx.dwFillColor = Color;
    RECT rectDest;

    // ordonne les coordonnées avant pour que le 1er point soit en haut à gauche
    if (x1 > x2) { int t=x1; x1=x2; x2=t; }
    if (y1 > y2) { int t=y1; y1=y2; y2=t; }

    // Points temporaires utilisés par le clipping
    int px1, px2, py1, py2;
    // Ligne verticale à partir du point (x1,y1)
    if (y1<0) py1 = 0; else py1 = y1;
    if (y2>=Screen_Height) py2 = Screen_Height-1; else py2 = y2;
    px1 = x1;
    rectDest.left = px1; rectDest.top = py1;
    rectDest.right = px1+epaisseur; rectDest.bottom = py2;
    pDDSBack->pDDSurf->Blt(&rectDest,NULL,NULL,DDBLT_WAIT | DDBLT_COLORFILL,&ddBltFx);
    // Ligne verticale à partir du point (x2,y1)
    if (y1<0) py1 = 0; else py1 = y1;
    if (y2>=Screen_Height) py2 = Screen_Height-1; else py2 = y2;
    px2 = x2;
    rectDest.left = px2; rectDest.top = py1;
    rectDest.right = px2+epaisseur; rectDest.bottom = py2;
    pDDSBack->pDDSurf->Blt(&rectDest,NULL,NULL,DDBLT_WAIT | DDBLT_COLORFILL,&ddBltFx);
    // Ligne horizontale à partir du point (x1,y1)
    if (x1<0) px1 = 0; else px1 = x1;
    if (x2>=Screen_Width) px2 = Screen_Width-1; else px2 = x2;
    py1 = y1;
    rectDest.left = px1; rectDest.top = py1;
    rectDest.right = px2; rectDest.bottom = py1+epaisseur;
    pDDSBack->pDDSurf->Blt(&rectDest,NULL,NULL,DDBLT_WAIT | DDBLT_COLORFILL,&ddBltFx);
    // Ligne horizontale à partir du point (x1,y2)
    if (x1<0) px1 = 0; else px1 = x1;
    if (x2+epaisseur>=Screen_Width) px2 = Screen_Width-1-epaisseur; else px2 = x2;
    py2 = y2;
    rectDest.left = px1; rectDest.top = py2;
    rectDest.right = px2+epaisseur; rectDest.bottom = py2+epaisseur;
    pDDSBack->pDDSurf->Blt(&rectDest,NULL,NULL,DDBLT_WAIT | DDBLT_COLORFILL,&ddBltFx);
}
//----------------------------------------------------------------------------//


//=== Affiche un rectangle orienté avec clipping =============================//
// Suivant 8 directions : 0 <=> Ouest, 1 <=> N-O, ..., 7 <=> Sud-Ouest
void TDirectScreen_Editor::PutRectangleOrienteClip(int x1, int y1, int x2, int y2, int epaisseur, unsigned int Color, int orientation)
{   // Préparation
    DDBLTFX ddBltFx;
    ddBltFx.dwSize = sizeof(DDBLTFX);
    ddBltFx.dwFillColor = Color;
    RECT rectDest;

    // Détermine le type de rectangle à tracer
    enum eGenreRect { RECT_DROIT, RECT_DIAG_NE, RECT_DIAG_SE, RECT_NONE };
    eGenreRect genreRect = RECT_NONE;
    switch (orientation)
    { case 0 :  // Ouest - Sud - Est - Nord
      case 2 :
      case 4 :
      case 6 : genreRect = RECT_DROIT; break;
      case 3 : // Sud-Est - Nord-Ouest
      case 7 : genreRect = RECT_DIAG_SE; break;
      case 1 : // Sud-Ouest - Nord-Est
      case 5 : genreRect = RECT_DIAG_NE; break;
    }

    // ordonne les coordonnées avant pour que le 1er point soit en haut à gauche
    switch (genreRect)
    { case (RECT_DROIT) :
      case (RECT_DIAG_NE) :
      case (RECT_DIAG_SE) :
           if (y1 > y2) { int t=y1; y1=y2; y2=t; }
           break;
      case (RECT_NONE) :  MessageBoxA(NULL,"Problem ds TDirectScreen_Editor::PutRectangleOrienteClip","Orientation inconnue",0); break;
    }

    // Cas du rectangle en diagonale
    if ((genreRect==RECT_DIAG_NE) || (genreRect==RECT_DIAG_SE))
    {  // Décalage pouvant prendre une moyenne de 0.5
       float decalage = abs( (abs(y2-y1) - abs(x2-x1)) / 2);

       // Calcule des coords des 2 autres points
       int x3, y3, x4, y4;
       if (x1 < x2) // genreRect==RECT_DIAG_NE) Pt(1) en haut à gauche et Pt(2) en bas à droite
       { x3 = x2 + int(decalage);
         y3 = y2 - int(decalage);
         x4 = x1 - int(decalage);
         y4 = y1 + int(decalage);
       } else // (genreRect==RECT_DIAG_SE) Pt(1) en haut à droite et Pt(2) en bas à gauche
       { x3 = x1 + int(decalage);
         y3 = y1 + int(decalage);
         x4 = x2 - int(decalage);
         y4 = y2 - int(decalage);
       }

       // Trace 4 droites de la bonne épaisseur
       for (int i=0; i < epaisseur ; i++)
       { BackLine2(x1-i, y1+i, x3-i, y3+i, Color);
         BackLine2(x3+i, y3+i, x2+i, y2+i, Color);
         BackLine2(x2-i, y2+i, x4-i, y4+i, Color);
         BackLine2(x4+i, y4+i, x1+i, y1+i, Color);
       }

       // Marque aussi les 4 points du rectangle
       PutCarre(x1, y1, epaisseur, Color);
       PutCarre(x2, y2, epaisseur, Color);
       PutCarre(x3, y3, epaisseur, Color);
       PutCarre(x4, y4, epaisseur, Color);
    }
    else
    {  // Cas du rectangle classique
       PutRectangleClip(x1, y1, x2, y2, epaisseur, Color);
    }
}
//----------------------------------------------------------------------------//
