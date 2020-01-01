////////////////////////////////////////////////////////////////////////////////
//  Unité  "D3DLib.cpp"  : Librarie regroupant diverses fonctions pour        //
//                         Direct3D créées intialement pour gérer des effets  //
//                         de transparences et de lumières.                   //
//  Copyright (C) 1998 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com                      //
//                                                                            //
//  Version      : beta 0.01                                                  //
//  Creation     : 10/04/1999                                                 //
//  Last upgrade : 10/04/1999                                                 //
//                                                                            //
// Rmq : utilise la librairie "d3dframe.lib" (Direct3D Immediate Mode helper  //
//       library from Microsoft)                                              //
//                                                                            //  
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------ définitions
#define D3D_OVERLOADS       // Permet de surcharger certains opérateurs et constructeurs Direct3D

//-------------------------------------------------------------- fichiers inclus
#include "ZeNain.h"			// Fichier commun à tout le projet
#include "GameDebug.h"		// Librairie maison de debuggage
#include "SpritesLib.h"		// Accès aux listes de SpriteTile3D
#pragma hdrstop
#include "D3DUtil.h"        // outils du SDK Direct3D permettant la manipulation de matrices
#include "D3DEnum.h"        // Fonctions énumérants les devices, modes graphiques et drivers existants
#include "D3DLib.h"         // son Header

//------------------------------------------------------ variables et constantes
// Objet principal gérant une fenêtre Direct3D à l'écran
TD3DScreen *D3DS = NULL;
// Objet principal gérant les effets spéciaux du jeu
TD3DSpecialEffect *D3D_SE = NULL;
// Utilisation de Direct3D ou émulation logicielle
bool SCREEN_DIRECT3D = true;	

// Supprime les warning liés aux convertions int -> float
#ifdef VISUALC
#pragma warning(disable : 4244)  
#endif

////////////////////////////////////////////////////////////////////////////////
// Classe TD3DSCREEN                                                          //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur ===========================================================//
TD3DScreen::TD3DScreen(HWND i_Hand) :
  TDirectScreen(i_Hand)         // Fait tout d'abord appel au constructeur de sa classe maître
{
   pD3D = NULL;
   pD3DDevice = NULL;
   pD3DViewport = NULL;
   pD3DMaterial = NULL;
   pD3DViewportRect = NULL;
   supportAlphaBlend = false;
   supportAlphaTest = false;
   Hardware3D = true;
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TD3DScreen::~TD3DScreen()
{	// Libère les objets D3D exclusivement utilisés par cette classe
	CleanD3D();
	// Libère ensuite les objets DD de la lcasse de base
	TDirectScreen::~TDirectScreen();
}
//----------------------------------------------------------------------------//

//=== Initialise les divers objets Direct3D ==================================//
void TD3DScreen::InitD3D()
{   HRESULT hr;
    // 1) Crée l'objet IDirect3DX en demandant une interface avec l'objet IDirectDrawX
    if (pD3D == NULL)
	{	// On ne crée cet objet que s'il n'existait déjà pas
		hr = pDD->QueryInterface(IID_IDirect3D3,(LPVOID*) &pD3D);
		if FAILED(hr) TraceErrorD3D(hr,99);
	}

    // 2) Vérifie que l'on est au moins en mode graphiquee 16 bits indispensable à l'alphablending
    DDSURFACEDESC2 ddsd;
    ddsd.dwSize = sizeof(DDSURFACEDESC2);
    pDD->GetDisplayMode(&ddsd);
    if(ddsd.ddpfPixelFormat.dwRGBBitCount <= 8)
    {  MessageError("Direct3D Erreur","Mode graphique incompatible avec cette application 16, 24 ou 32 bits",99);
    }

    // 3) Trouve un device compatible avec cet ordi et sa carte 3D
    // 3.1) Cherche tout d'abord à utiliser le hardware 3D
    D3DFINDDEVICESEARCH devSearch;
    D3DFINDDEVICERESULT devResult;
    ZeroMemory(&devSearch, sizeof(devSearch));
    devSearch.dwSize = sizeof(devSearch);
    ZeroMemory(&devResult, sizeof(devResult));
    devResult.dwSize = sizeof(devResult);
    devSearch.dwFlags =   D3DFDS_HARDWARE| D3DFDS_ALPHACMPCAPS      // Détermine le device d'aprés la variable bHarware
                          | D3DFDS_DSTBLENDCAPS | D3DFDS_SRCBLENDCAPS
                          | D3DFDS_TRIANGLES | D3DFDS_TEXTURECAPS;
    devSearch.dpcPrimCaps.dwSize = sizeof(devSearch.dpcPrimCaps);
    devSearch.dpcPrimCaps.dwDestBlendCaps = D3DPBLENDCAPS_INVSRCALPHA;
    devSearch.dpcPrimCaps.dwSrcBlendCaps = D3DPBLENDCAPS_SRCALPHA;
    devSearch.dpcPrimCaps.dwAlphaCmpCaps = D3DPCMPCAPS_GREATEREQUAL;
    devSearch.dpcPrimCaps.dwTextureCaps = D3DPTEXTURECAPS_TRANSPARENCY | D3DPTEXTURECAPS_ALPHA;
    devSearch.bHardware = Hardware3D;         // vrai -> lorsque le carte 3D supporte l'effet, on la fait bosser. Sinon, on l'émule en software
    hr = pD3D->FindDevice(&devSearch, &devResult);
    // 3.2) Si çà ne marche pas, on essaye en émulation software
    if FAILED(hr)
    {
      #ifdef D3DLIB_DEBUG
      MessageError("Information Direct3D","Passage en émulation logicielle !",1);
      #endif
      Hardware3D = false;
      devSearch.bHardware = Hardware3D;
      hr = pD3D->FindDevice(&devSearch, &devResult);
      if FAILED(hr) MessageError("Direct3D Erreur","CPU & carte vidéo même pas assez performants pour émuler l'alpha-blending !",2);
    }

    // 4) Crée le D3D device d'aprés celui que l'on a trouvé
    hr = pD3D->CreateDevice(devResult.guid, pDDSBack->pDDSurf, &pD3DDevice, NULL);
    if FAILED(hr) TraceErrorD3D(hr,99);

    // 5) Crée la ViewPort : détermine comment la caméra est positionnée et comment la scène 3D va s'afficher sur l'écran.
    D3DVIEWPORT2 vp;
    ZeroMemory(&vp, sizeof(vp));
    vp.dwSize = sizeof(vp);
    vp.dwX = vp.dwY = 0;            // Bords supérieurs gauches de la fenêtre de rendu 3D
    vp.dwWidth  = Screen_Width;
    vp.dwHeight = Screen_Height;
    // Viewport de (-1.0,+1.0) sur les X, de (-1.0,+1.0) sur les Y et de (0.0,1.0à sur les Z
    vp.dvClipX      = -1.0f;
    vp.dvClipWidth  =  2.0f;
    vp.dvClipY      =  1.0f;
    vp.dvClipHeight =  2.0f;
    vp.dvMinZ       =  0.0f;
    vp.dvMaxZ       =  1.0f;
    hr = pD3D->CreateViewport( &pD3DViewport, NULL );    // Crée le Viewport
    if FAILED(hr) TraceErrorD3D(hr,99);
    hr = pD3DDevice->AddViewport(pD3DViewport);          // Associe le Viewport au device
    if FAILED(hr) TraceErrorD3D(hr,99);
    hr = pD3DViewport->SetViewport2(&vp);                // Passe les paramètres souhaités au Viewport
    if FAILED(hr) TraceErrorD3D(hr,99);
    hr = pD3DDevice->SetCurrentViewport(pD3DViewport);   // Associe ce nouveau Viewport au device
    if FAILED(hr) TraceErrorD3D(hr,99);

    // Initialise le rectangle délimitant le viewport
    pD3DViewportRect = new D3DRECT();
    pD3DViewportRect->x1 = pD3DViewportRect->y1 = 0;
    pD3DViewportRect->x2 = Screen_Width;
    pD3DViewportRect->y2 = Screen_Height;

    // 6) Crée l'objet Material : gère les lumières et les effets de réflections
    hr = pD3D->CreateMaterial(&pD3DMaterial, NULL);
    if FAILED(hr) TraceErrorD3D(hr,99);
    D3DMATERIAL        matiere;
    // Initialise les propriétés du matériel
    ZeroMemory(&matiere, sizeof(matiere));
    matiere.dwSize   = sizeof(matiere);
    // Paramètres pour la réflection diffuse
    matiere.dcvDiffuse.r = (D3DVALUE)0.5;
    matiere.dcvDiffuse.g = (D3DVALUE)0.0;
    matiere.dcvDiffuse.b = (D3DVALUE)0.5;
    matiere.dcvDiffuse.a = (D3DVALUE)1.0;
    // Paramètres pour la réflection ambiante
    matiere.dcvAmbient.r = (D3DVALUE)0.5;
    matiere.dcvAmbient.g = (D3DVALUE)0.0;
    matiere.dcvAmbient.b = (D3DVALUE)0.5;
    matiere.dcvAmbient.a = (D3DVALUE)1.0;
    // Autres paramètres : couleur et sharpness des pecular highlights
    matiere.dcvSpecular.r = (D3DVALUE)1.0;
    matiere.dcvSpecular.g = (D3DVALUE)1.0;
    matiere.dcvSpecular.b = (D3DVALUE)1.0;
    matiere.dcvSpecular.a = (D3DVALUE)1.0;
    matiere.dvPower = (float) 50.0;
    // Si l'on est en ramp emulation, on utilisera 16 entrées de color map
    matiere.dwRampSize = 16;
    // Ce matériel devient le matériel courant
    hr = pD3DMaterial->SetMaterial(&matiere);
    if FAILED(hr) TraceErrorD3D(hr,99);
    // Sélectionne et valide un matériel pour le calcul de la scène
    D3DMATERIALHANDLE  hmatiere;
    hr = pD3DMaterial->GetHandle(pD3DDevice, &hmatiere);    // Récupère son handle
    if FAILED(hr) TraceErrorD3D(hr,99);
    hr = pD3DDevice->SetLightState(D3DLIGHTSTATE_MATERIAL, hmatiere);
    if FAILED(hr) TraceErrorD3D(hr,99);

    // 7) Construction des matrices de transformations
    D3DVECTOR vPointVue   = D3DVECTOR(0.0f, 0.0f, -20.0f);  // positionné sur l'axe des Z
    D3DVECTOR vRegardVers = D3DVECTOR(0.0f, 0.0f, 0.0f);    // regarde l'origine du repère
    D3DVECTOR vUpVec      = D3DVECTOR(0.0f, 0.0f, 1.0f);
    // Matrice du monde qui contôle ma position et l'orientation des polygones dans l'espace 3D.
    D3DMATRIX matWorld;
    D3DUtil_SetIdentityMatrix(matWorld);
    pD3DDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matWorld );
    // Matrice de la vue qui définie la position et l'orientation de la caméra
    D3DMATRIX matView;
    D3DUtil_SetViewMatrix(matView, vPointVue, vRegardVers, vUpVec);
    pD3DDevice->SetTransform( D3DTRANSFORMSTATE_VIEW, &matView);
    // Matrice de projection qui définie comment la scène 3D va être projetée sur la surface 2D
    D3DMATRIX matProj;
    D3DUtil_SetProjectionMatrix(matProj, 1.57f, 1.0f, 1.0f, 100.0f);
    pD3DDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj);

    // 8) Initialise divers états du monde 3D
    // Lumière ambiante = white ambient light
    hr = pD3DDevice->SetLightState(D3DLIGHTSTATE_AMBIENT, 0xffffffff);
    if FAILED(hr) TraceErrorD3D(hr,99);
    // Rendu 3D
    hr = pD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
    hr = pD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
    hr = pD3DDevice->SetRenderState(D3DRENDERSTATE_DITHERENABLE, false);      // Invalide le dithering
    hr = pD3DDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, true);
    if FAILED(hr) TraceErrorD3D(hr,99);
    // Etat des textures
    hr = pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    hr = pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    hr = pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    hr = pD3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_LINEAR);
//    hr = pD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);     // Filtre et lisse les textures, mais pose problème lors de l'utilisation de la keycolor car il reste des pixels roses	
    if FAILED(hr) TraceErrorD3D(hr,99);

    // 9) Verifie les capacités du hardware pour l'alphablending et l'alphatesting
    devDescHEL.dwSize = sizeof(devDescHEL);
    devDescHAL.dwSize = sizeof(devDescHAL);
    pD3DDevice->GetCaps(&devDescHAL, &devDescHEL);
    D3DDEVICEDESC *pDesc = (devDescHAL.dwFlags) ? &devDescHAL : &devDescHEL;
    // Vérifie la possibilité de faire de l'alpha-blending
    if (    (pDesc->dpcTriCaps.dwDestBlendCaps & D3DPBLENDCAPS_INVSRCALPHA)
         && (pDesc->dpcTriCaps.dwSrcBlendCaps & D3DPBLENDCAPS_SRCALPHA))
    { supportAlphaBlend = true; }
    else MessageError("Direct3D Problem","Votre carte vidéo ne supporte l'alpha-blending en HAL",2);
    // Vérifie la possibiltité de faire de l'alpha-testing
    if (pDesc->dpcTriCaps.dwAlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL)
    { supportAlphaTest = true; }
    else MessageError("Direct3D Problem","Votre carte vidéo ne supporte l'alpha-testing en HAL",2);

}
//----------------------------------------------------------------------------//

//=== Recherche si l'utilisations de Direct3D est possible ===================//
/*void TD3DScreen::SearchDevice(DWORD flags)
{   HRESULT hr;
    // 1) Enumère les devices Direct3D possibles, en lui passant en paramètre les critères
    // sur lesquels il va accepter ou rejeter tel device d'aprés les capacités que souhaitent l'appli.
	// PROBLEME avec DirectX 7.0
	if (flags & D3DLIB_ALPHABLENDING)
    { hr = D3DEnum_EnumerateDevices( ConfirmDevice_AphaBlending );
      if FAILED(hr) MessageError("Direct3D Erreur","Votre carte 3D n'est pas assez performante pour cette application !",2);
    }
}*/
//----------------------------------------------------------------------------//

//=== Indique d'où provient l'erreur détectée par Direct3D ===================//
void TD3DScreen::TraceErrorD3D(HRESULT hErr,int Gravite)
{   AnsiString Erreur;
    if (pD3D!=NULL) pDD->FlipToGDISurface();  // Revient en mode fenêtré
    switch (hErr)
    {   case D3D_OK                  : Erreur = "D3D_OK : étrange ..."; break;
        case D3DERR_BADMAJORVERSION  : Erreur = "D3DERR_BADMAJORVERSION"; break;
        case D3DERR_BADMINORVERSION  : Erreur = "D3DERR_BADMINORVERSION"; break;
        case D3DERR_COLORKEYATTACHED : Erreur = "D3DERR_COLORKEYATTACHED"; break;
        case D3DERR_CONFLICTINGTEXTUREFILTER  : Erreur = "D3DERR_CONFLICTINGTEXTUREFILTER"; break;
        case D3DERR_CONFLICTINGTEXTUREPALETTE : Erreur = "D3DERR_CONFLICTINGTEXTUREPALETTE"; break;
        case D3DERR_CONFLICTINGRENDERSTATE    : Erreur = "D3DERR_CONFLICTINGRENDERSTATE"; break;
        case D3DERR_DEVICEAGGREGATED          : Erreur = "D3DERR_DEVICEAGGREGATED"; break;
        case D3DERR_EXECUTE_CLIPPED_FAILED    : Erreur = "D3DERR_EXECUTE_CLIPPED_FAILED"; break;
        case D3DERR_EXECUTE_CREATE_FAILED     : Erreur = "D3DERR_EXECUTE_CREATE_FAILED"; break;
        case D3DERR_EXECUTE_DESTROY_FAILED    : Erreur = "D3DERR_EXECUTE_DESTROY_FAILED"; break;
        case D3DERR_EXECUTE_FAILED            : Erreur = "D3DERR_EXECUTE_FAILED"; break;
        case D3DERR_EXECUTE_LOCK_FAILED       : Erreur = "D3DERR_EXECUTE_LOCK_FAILED"; break;
        case D3DERR_EXECUTE_LOCKED            : Erreur = "D3DERR_EXECUTE_LOCKED"; break;
        case D3DERR_EXECUTE_NOT_LOCKED        : Erreur = "D3DERR_EXECUTE_NOT_LOCKED"; break;
        case D3DERR_EXECUTE_UNLOCK_FAILED     : Erreur = "D3DERR_EXECUTE_UNLOCK_FAILED"; break;
        case D3DERR_INITFAILED                : Erreur = "D3DERR_INITFAILED"; break;
        case D3DERR_INBEGIN                   : Erreur = "D3DERR_INBEGIN"; break;
        case D3DERR_INVALID_DEVICE            : Erreur = "D3DERR_INVALID_DEVICE"; break;
        case D3DERR_INVALIDCURRENTVIEWPORT    : Erreur = "D3DERR_INVALIDCURRENTVIEWPORT"; break;
        case D3DERR_INVALIDMATRIX             : Erreur = "D3DERR_INVALIDMATRIX"; break;
        case D3DERR_INVALIDPALETTE            : Erreur = "D3DERR_INVALIDPALETTE"; break;

    	default : Erreur="Erreur inconnue !"; break;
    }
  #ifdef D3DLIB_DEBUG
  MessageError("Erreur Direct3D",Erreur.c_str(),Gravite);
  #endif
}
//----------------------------------------------------------------------------//

//=== Désalloue tous les objets et toute la mémoire ==========================//
void TD3DScreen::CleanD3D()
{  // Libère les objets D3D exclusivement utilisés par cette classe
   if (pD3DViewportRect) delete pD3DViewportRect;
   if (pD3DMaterial)     pD3DMaterial->Release();
   if (pD3DViewport)     pD3DViewport->Release();
   if (pD3DDevice)       pD3DDevice->Release();
   if (pD3D)             pD3D->Release();
   D3DEnum_FreeResources();

   pD3DViewportRect = NULL;
   pD3DMaterial = NULL;
   pD3DViewport = NULL;
   pD3DDevice = NULL;
   pD3D = NULL;
   supportAlphaBlend = false;
   supportAlphaTest = false;
}
//----------------------------------------------------------------------------//

//=== Renvoie un pointeur sur le device3D de cet objet Direct3D ==============//
LPDIRECT3DDEVICE3 TD3DScreen::Get3DDevice()
{ return pD3DDevice;
}
//----------------------------------------------------------------------------//

//=== Fonction à appeler avant le rendu d'une scène 3D =======================//
void TD3DScreen::InitRenderScene3D()
{ HRESULT hr;
  // Efface le contenu du Viewport
  pD3DViewport->Clear2(1UL, pD3DViewportRect, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                       0x20202020, 1.0f, 0L);
  // On peut commencer à afficher la scène 2D & 3D
  hr = pD3DDevice->BeginScene();
  if FAILED(hr) TraceErrorD3D(hr,99);
}
//----------------------------------------------------------------------------//

//=== A appeler aprés avoir rendu la scène 3D ================================//
void TD3DScreen::CloseRenderScene3D()
{  pD3DDevice->EndScene();
}
//----------------------------------------------------------------------------//

//=== Paramétrage du rendu 3D ================================================//
void TD3DScreen::Set3DRenderState(bool alpha_blending, bool alpha_testing, bool specular)
{ // Alpha-blending
  if (alpha_blending)
  {  // FinalColor = TexelColor * SourceBlendFactor + PixelColor * DestBlendFactor
     pD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND,  D3DBLEND_SRCALPHA );
     pD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA );
     pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, true);
  } else
  {  pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, false);
  }
  // Alpha-testing
  if (alpha_testing)
  {  pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, true);
     pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHAREF,  0x08);
     pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHAFUNC, D3DCMP_GREATEREQUAL);
  } else
  {  pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, false);
  }
  // Specular light
  if (specular)
  {  pD3DDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, true);
  } else
  {  pD3DDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, false);
  }

  // Utilise la color-keyed transparency de la texture
  pD3DDevice->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE , true);
}
//----------------------------------------------------------------------------//

//=== Permutation du mode FullScreen <-> Windowed ============================//
void TD3DScreen::ToggleFullScreenMode()
{	
   // Libère certains objets D3D exclusivement utilisés par cette classe
   if (pD3DViewportRect) delete pD3DViewportRect;
   if (pD3DMaterial)     pD3DMaterial->Release();
   if (pD3DViewport)     pD3DViewport->Release();
   if (pD3DDevice)       pD3DDevice->Release();
   pD3DViewportRect = NULL;
   pD3DMaterial = NULL;
   pD3DViewport = NULL;
   pD3DDevice = NULL;
	
	// Changement de mode-écran de tout ce qui est relatif à la 3D
	TDirectScreen::ToggleFullScreenMode();

	// Puis on réinitialise les divers objets Direct3D
	InitD3D();	
}
//----------------------------------------------------------------------------//


// Fin de la classe TD3DScreen /////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Classe SPRITE3DTILE                                                        //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur par défaut ================================================//
Sprite3DTile::Sprite3DTile():
    SpriteTile()     // appel du constructeur de la classe maître
{ ScrX1 = ScrY1 = ScrX2 = ScrY2 = 0;
  pDevice  = NULL;
  pTexture = NULL;
  TypeSprite = SPRITE_TILE_3D;
}
//----------------------------------------------------------------------------//

//=== Informations pour son rendu 3D =========================================//
void Sprite3DTile::Create3DSprite(LPDIRECT3DDEVICE3 pDev, D3DCOLOR i_col, D3DCOLOR i_spec, float tu1, float tv1, float tu2, float tv2, DDSurface &pFrame)
{  HRESULT hr;
   // Infos sur l'intensité des couleurs
   cColor = i_col;
   cSpecular = i_spec;
   // Coordonnées du sprite3D sur la texture
   TexU1 = tu1; TexV1 = tv1;
   TexU2 = tu2; TexV2 = tv2;
   // Sélectionne le device utilisé par le rendu 3D
   pDevice = pDev;
   pDDSurface = &pFrame;
   // Rafraichit le tableau des points sans tenir compte de la position des sprites
   RefreshVertices();
   // Obtient une interface vers une texture
   // Interface ce sprite à sa texture
   hr = pFrame.pDDSurf->QueryInterface(IID_IDirect3DTexture2, (void**)&pTexture);
   if FAILED(hr) D3DS->TraceErrorD3D(hr,1);

}
//----------------------------------------------------------------------------//

//=== Recalcule les vertices d'aprés la position du sprite sur l'écran =======//
void Sprite3DTile::RefreshVertices()
{ vSpriteVertices[0] = D3DTLVERTEX( D3DVECTOR(ScrX1, ScrY1, 0), 1, cColor, cSpecular, TexU1, TexV1);
  vSpriteVertices[1] = D3DTLVERTEX( D3DVECTOR(ScrX2, ScrY1, 0), 1, cColor, cSpecular, TexU2, TexV1);
  vSpriteVertices[2] = D3DTLVERTEX( D3DVECTOR(ScrX1, ScrY2, 0), 1, cColor, cSpecular, TexU1, TexV2);
  vSpriteVertices[3] = D3DTLVERTEX( D3DVECTOR(ScrX2, ScrY2, 0), 1, cColor, cSpecular, TexU2, TexV2);
}
//----------------------------------------------------------------------------//

//=== Rafraîchit seulement la position du sprite 3D ==========================//
void Sprite3DTile::RefreshPosition()
{ vSpriteVertices[0].sx = (float) ScrX1; vSpriteVertices[0].sy = (float) ScrY1;
  vSpriteVertices[1].sx = (float) ScrX2; vSpriteVertices[1].sy = (float) ScrY1;
  vSpriteVertices[2].sx = (float) ScrX1; vSpriteVertices[2].sy = (float) ScrY2;
  vSpriteVertices[3].sx = (float) ScrX2; vSpriteVertices[3].sy = (float) ScrY2;
}
//----------------------------------------------------------------------------//

//=== Positionne le sprite sur l'écran =======================================//
// -> le sprite conserve sa taille d'origine
void Sprite3DTile::SetScreenCoord(int x, int y)
{  ScrX1 = x;
   ScrY1 = y;
   ScrX2 = x + Width;
   ScrY2 = y + Height;
   RefreshPosition();
}
// -> redimensionné pour l'occasion
void Sprite3DTile::SetScreenCoord(int x1, int y1, int x2, int y2)
{  ScrX1 = x1; ScrY1 = y1; ScrX2 = x2; ScrY2 = y2;
   RefreshPosition();
}
// -> zoomé
void Sprite3DTile::SetScreenCoord(int x, int y, float zoom)
{  ScrX1 = x; ScrY1 = y;
   ScrX2 = x + Width*zoom;
   ScrY2 = y + Height*zoom;
   RefreshPosition();
}
//----------------------------------------------------------------------------//

//=== Positionne le sprite sur la texture ====================================//
void Sprite3DTile::SetTextureCoord(float u1, float v1, float u2, float v2)
{  TexU1 = u1; TexV1 = v1; TexU2 = u2; TexV2 = v2;
   RefreshVertices();
}
//----------------------------------------------------------------------------//

//=== Redéfini les couleurs d'un point de la texture =========================//
void Sprite3DTile::SetVertexColor(int numVertex, D3DCOLOR col, D3DCOLOR spec)
{ vSpriteVertices[numVertex].color = col;
  vSpriteVertices[numVertex].color = spec;
}
//----------------------------------------------------------------------------//

//=== Affiche un sprite 3D à l'écran =========================================//
void Sprite3DTile::SpriteRender()
{ HRESULT hr;
  // Archive les paramètres du rendu 3D concernant les z-comparaisons
  DWORD oldZState;
  hr = pDevice->GetRenderState(D3DRENDERSTATE_ZFUNC, &oldZState);
  if FAILED(hr) _asm int 3;
  // Tests de comparaisons sur les Z toujours fait
  hr = pDevice->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_ALWAYS);
  if FAILED(hr) _asm int 3;

  // Vérifie si la texture est en mémoire
  if (!pFrame->isAllocateTexture)
  { hr = pTexture->Load(pTexture);
    if FAILED(hr) D3DS->TraceErrorD3D(hr,99);
    pFrame->isAllocateTexture = true;
  }
  // Set the texture state's for single-texturing mode
/*    pDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
    pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
// Set any appropiate state
    pDevice->SetRenderState( D3DRENDERSTATE_ZENABLE, FALSE );
    pDevice->SetLightState(  D3DLIGHTSTATE_AMBIENT,  0xffffffff );
    pDevice->SetRenderState( D3DRENDERSTATE_TEXTUREPERSPECTIVE, TRUE );
    pDevice->SetRenderState( D3DRENDERSTATE_DITHERENABLE, TRUE );
    pDevice->SetRenderState( D3DRENDERSTATE_SPECULARENABLE, FALSE );*/

    hr = pDevice->SetTexture(0,pTexture);
	if FAILED(hr) _asm int 3;
    hr = pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, vSpriteVertices , 4, D3DDP_WAIT);
	if FAILED(hr) _asm int 3;

  // Restaure les paramètres de rendu 3D sauvegardés
  hr =  pDevice->SetRenderState(D3DRENDERSTATE_ZFUNC, oldZState);
  if FAILED(hr) _asm int 3;
}

void Sprite3DTile::SpriteRender(int x, int y)
{  SetScreenCoord(x, y);
   SpriteRender();
}
//----------------------------------------------------------------------------//

//=== Changement de device3D =================================================//
void Sprite3DTile::SetDirect3DDevice(LPDIRECT3DDEVICE3 pDev)
{	HRESULT hr;
	pDevice = pDev;
	// Interface ce sprite à une texture
	hr = pDDSurface->pDDSurf->QueryInterface(IID_IDirect3DTexture2, (void**)&pTexture);
	if FAILED(hr) D3DS->TraceErrorD3D(hr,1);
}
//----------------------------------------------------------------------------//

// Fin de la classe SPRITE3DTILE ///////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Classe TD3DSPECIALEFFECT                                                   //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeutr par dééfait ==============================================//
TD3DSpecialEffect::TD3DSpecialEffect()
{ transition = false;
  colDay.SetColor(0x0);    // Couleur transparente
  sePresHour = seFutHour = HOUR_NONE;
  pDevice3D = NULL; 
};
//----------------------------------------------------------------------------//

//=== Crée le filtre en lui passant tous les paramètres requis ===============//
void TD3DSpecialEffect::CreateFilter(DWORD night, DWORD fog, int x, int y, int w, int h, LPDIRECT3DDEVICE3 pDev)
{ colNight.SetColor(night);
  colFog.SetColor(fog);
  filX = x; filY = y;
  filWidth = w; filHeight = h;
  pDevice3D = pDev;
  vScreenFilter[0] = D3DTLVERTEX( D3DVECTOR(filX, filY, 0), 1, actualColor.GetColor(), 0x0, 0, 0);
  vScreenFilter[1] = D3DTLVERTEX( D3DVECTOR(filX+filWidth, filY, 0), 1, actualColor.GetColor(), 0x0, 0, 0);
  vScreenFilter[2] = D3DTLVERTEX( D3DVECTOR(filX, filY+filHeight, 0), 1, actualColor.GetColor(), 0x0, 0, 0);
  vScreenFilter[3] = D3DTLVERTEX( D3DVECTOR(filX+filWidth, filY+filHeight, 0), 1, actualColor.GetColor(), 0x0, 0, 0);
}
//----------------------------------------------------------------------------//

//=== Change de période journalière brusquement ==============================//
void TD3DSpecialEffect::SetState(eHOUR state)
{ seFutHour = state;
  sePresHour = state;
   switch (state)
   {  case HOUR_DAY   : actualColor = colDay;   break;
      case HOUR_NIGHT : actualColor = colNight; break;
      case HOUR_FOG   : actualColor = colFog;   break;
   }
  RefreshColorFilter();
  transition = false;
}
//----------------------------------------------------------------------------//

//=== Change de période en fading ============================================//
void TD3DSpecialEffect::ChangeState(eHOUR state)
{  // Vérifie que l'on ne soit pas déjà à cette période
   if (sePresHour == state) return;

   // Calcule les différents delta à parcourir avant d'atteindre la couleur désirée
   switch (state)
   {  case HOUR_DAY   : nextColor = colDay;   break;
      case HOUR_NIGHT : nextColor = colNight; break;
      case HOUR_FOG   : nextColor = colFog;   break;
   }
   deltaColor.red   = (nextColor.red   - actualColor.red)   / 100;
   deltaColor.blue  = (nextColor.blue  - actualColor.blue)  / 100;
   deltaColor.green = (nextColor.green - actualColor.green) / 100;
   deltaColor.alpha = (nextColor.alpha - actualColor.alpha) / 100;

   seFutHour = state;
   transition = true;
}
//----------------------------------------------------------------------------//

//=== Met à jour des couleurs du filtre ======================================//
void TD3DSpecialEffect::RefreshColorFilter()
{ vScreenFilter[0].color = actualColor.GetColor();
  vScreenFilter[1].color = actualColor.GetColor();
  vScreenFilter[2].color = actualColor.GetColor();
  vScreenFilter[3].color = actualColor.GetColor();
}
//----------------------------------------------------------------------------//

//=== Update le filtre écran =================================================//
void TD3DSpecialEffect::UpdateSE()
{
   //*** En attendant de gérer convenablement le jour et la nuit *************//
   if ((!transition) && ((rand() % 1000)==499))
   { if (sePresHour==HOUR_DAY) ChangeState(HOUR_NIGHT);
     else ChangeState(HOUR_DAY);
   }
   //*************************************************************************//

   // Sort si il n'y a rien à faire
   if (!transition) return;

   // Actualise la couleur du filtre courant
   actualColor.red   += deltaColor.red;
   actualColor.green += deltaColor.green;
   actualColor.blue  += deltaColor.blue;
   actualColor.alpha += deltaColor.alpha;
   RefreshColorFilter();

   // Vérifie que l'on ne soit pas arrivé à la couleur recherchée
   if (actualColor.IsSimilaire(nextColor))
   {  sePresHour = seFutHour;
      transition = false;
   }
}
//----------------------------------------------------------------------------//

//=== Applique le filtre à l'écran ===========================================//
void TD3DSpecialEffect::RenderFilter(bool backup)
{
  // Regarde si c'est la peine d'afficher le filtre
  if (actualColor.alpha ==0) return; 

  DWORD oldAlphablendState=0, oldSrcblendState=0, oldDestblendState=0;
  // Archive les paramètres du rendu 3D concernant l'aplha-blending
  if (backup)
  {  pDevice3D->GetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, &oldAlphablendState);
     pDevice3D->GetRenderState(D3DRENDERSTATE_SRCBLEND, &oldSrcblendState);
     pDevice3D->GetRenderState(D3DRENDERSTATE_DESTBLEND, &oldDestblendState);
  }

  // Paramètres d'Alpha-blending du filtre
  pDevice3D->SetRenderState(D3DRENDERSTATE_SRCBLEND,  D3DBLEND_SRCALPHA  );
  //pDevice3D->SetRenderState(D3DRENDERSTATE_SRCBLEND,  D3DBLEND_ONE);
  pDevice3D->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
  pDevice3D->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, true);
  pDevice3D->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, vScreenFilter, 4, NULL);

  // Restaure les paramètres d'alpha-blending du rendu 3D
  if (backup)
  {  pDevice3D->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, oldAlphablendState);
     pDevice3D->SetRenderState(D3DRENDERSTATE_SRCBLEND, oldSrcblendState);
     pDevice3D->SetRenderState(D3DRENDERSTATE_DESTBLEND, oldDestblendState);
  }
}
//----------------------------------------------------------------------------//

//=== Chgt de device 3D ======================================================//
void TD3DSpecialEffect::SetDevice3D(LPDIRECT3DDEVICE3 pDev)
{	pDevice3D = pDev;
}
//----------------------------------------------------------------------------//

// Fin de la classe TD3DSPECIALEFFECT //////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Fonctions ANNEXES                                                          //
////////////////////////////////////////////////////////////////////////////////

//=== Appelé lors de l'intialisation, cette fonction vérifie que le device ===//
// possède un minimum des capacités souhaitées.
HRESULT ConfirmDevice_AphaBlending(DDCAPS* pddDriverCaps, D3DDEVICEDESC* pd3dDeviceDesc)
{   // Récupère un triangle caps (Hardware ou software) et vérifie l'alpha-blending
    // Get triangle caps (Hardware or software) and check for alpha blending
    LPD3DPRIMCAPS pdpc = &pd3dDeviceDesc->dpcTriCaps;
   if((pdpc->dwSrcBlendCaps & pdpc->dwDestBlendCaps & D3DBLEND_ONE) == 0)
   { return E_FAIL; }

   return S_OK;
}
//----------------------------------------------------------------------------//







