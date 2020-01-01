////////////////////////////////////////////////////////////////////////////////
//  Unit�  "D3DLib.cpp"  : Librarie regroupant diverses fonctions pour        //
//                         Direct3D cr��es intialement pour g�rer des effets  //
//                         de transparences et de lumi�res.                   //
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

//------------------------------------------------------------------ d�finitions
#define D3D_OVERLOADS       // Permet de surcharger certains op�rateurs et constructeurs Direct3D

//-------------------------------------------------------------- fichiers inclus
#include "ZeNain.h"			// Fichier commun � tout le projet
#include "GameDebug.h"		// Librairie maison de debuggage
#include "SpritesLib.h"		// Acc�s aux listes de SpriteTile3D
#pragma hdrstop
#include "D3DUtil.h"        // outils du SDK Direct3D permettant la manipulation de matrices
#include "D3DEnum.h"        // Fonctions �num�rants les devices, modes graphiques et drivers existants
#include "D3DLib.h"         // son Header

//------------------------------------------------------ variables et constantes
// Objet principal g�rant une fen�tre Direct3D � l'�cran
TD3DScreen *D3DS = NULL;
// Objet principal g�rant les effets sp�ciaux du jeu
TD3DSpecialEffect *D3D_SE = NULL;
// Utilisation de Direct3D ou �mulation logicielle
bool SCREEN_DIRECT3D = true;	

// Supprime les warning li�s aux convertions int -> float
#ifdef VISUALC
#pragma warning(disable : 4244)  
#endif

////////////////////////////////////////////////////////////////////////////////
// Classe TD3DSCREEN                                                          //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur ===========================================================//
TD3DScreen::TD3DScreen(HWND i_Hand) :
  TDirectScreen(i_Hand)         // Fait tout d'abord appel au constructeur de sa classe ma�tre
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
{	// Lib�re les objets D3D exclusivement utilis�s par cette classe
	CleanD3D();
	// Lib�re ensuite les objets DD de la lcasse de base
	TDirectScreen::~TDirectScreen();
}
//----------------------------------------------------------------------------//

//=== Initialise les divers objets Direct3D ==================================//
void TD3DScreen::InitD3D()
{   HRESULT hr;
    // 1) Cr�e l'objet IDirect3DX en demandant une interface avec l'objet IDirectDrawX
    if (pD3D == NULL)
	{	// On ne cr�e cet objet que s'il n'existait d�j� pas
		hr = pDD->QueryInterface(IID_IDirect3D3,(LPVOID*) &pD3D);
		if FAILED(hr) TraceErrorD3D(hr,99);
	}

    // 2) V�rifie que l'on est au moins en mode graphiquee 16 bits indispensable � l'alphablending
    DDSURFACEDESC2 ddsd;
    ddsd.dwSize = sizeof(DDSURFACEDESC2);
    pDD->GetDisplayMode(&ddsd);
    if(ddsd.ddpfPixelFormat.dwRGBBitCount <= 8)
    {  MessageError("Direct3D Erreur","Mode graphique incompatible avec cette application 16, 24 ou 32 bits",99);
    }

    // 3) Trouve un device compatible avec cet ordi et sa carte 3D
    // 3.1) Cherche tout d'abord � utiliser le hardware 3D
    D3DFINDDEVICESEARCH devSearch;
    D3DFINDDEVICERESULT devResult;
    ZeroMemory(&devSearch, sizeof(devSearch));
    devSearch.dwSize = sizeof(devSearch);
    ZeroMemory(&devResult, sizeof(devResult));
    devResult.dwSize = sizeof(devResult);
    devSearch.dwFlags =   D3DFDS_HARDWARE| D3DFDS_ALPHACMPCAPS      // D�termine le device d'apr�s la variable bHarware
                          | D3DFDS_DSTBLENDCAPS | D3DFDS_SRCBLENDCAPS
                          | D3DFDS_TRIANGLES | D3DFDS_TEXTURECAPS;
    devSearch.dpcPrimCaps.dwSize = sizeof(devSearch.dpcPrimCaps);
    devSearch.dpcPrimCaps.dwDestBlendCaps = D3DPBLENDCAPS_INVSRCALPHA;
    devSearch.dpcPrimCaps.dwSrcBlendCaps = D3DPBLENDCAPS_SRCALPHA;
    devSearch.dpcPrimCaps.dwAlphaCmpCaps = D3DPCMPCAPS_GREATEREQUAL;
    devSearch.dpcPrimCaps.dwTextureCaps = D3DPTEXTURECAPS_TRANSPARENCY | D3DPTEXTURECAPS_ALPHA;
    devSearch.bHardware = Hardware3D;         // vrai -> lorsque le carte 3D supporte l'effet, on la fait bosser. Sinon, on l'�mule en software
    hr = pD3D->FindDevice(&devSearch, &devResult);
    // 3.2) Si �� ne marche pas, on essaye en �mulation software
    if FAILED(hr)
    {
      #ifdef D3DLIB_DEBUG
      MessageError("Information Direct3D","Passage en �mulation logicielle !",1);
      #endif
      Hardware3D = false;
      devSearch.bHardware = Hardware3D;
      hr = pD3D->FindDevice(&devSearch, &devResult);
      if FAILED(hr) MessageError("Direct3D Erreur","CPU & carte vid�o m�me pas assez performants pour �muler l'alpha-blending !",2);
    }

    // 4) Cr�e le D3D device d'apr�s celui que l'on a trouv�
    hr = pD3D->CreateDevice(devResult.guid, pDDSBack->pDDSurf, &pD3DDevice, NULL);
    if FAILED(hr) TraceErrorD3D(hr,99);

    // 5) Cr�e la ViewPort : d�termine comment la cam�ra est positionn�e et comment la sc�ne 3D va s'afficher sur l'�cran.
    D3DVIEWPORT2 vp;
    ZeroMemory(&vp, sizeof(vp));
    vp.dwSize = sizeof(vp);
    vp.dwX = vp.dwY = 0;            // Bords sup�rieurs gauches de la fen�tre de rendu 3D
    vp.dwWidth  = Screen_Width;
    vp.dwHeight = Screen_Height;
    // Viewport de (-1.0,+1.0) sur les X, de (-1.0,+1.0) sur les Y et de (0.0,1.0� sur les Z
    vp.dvClipX      = -1.0f;
    vp.dvClipWidth  =  2.0f;
    vp.dvClipY      =  1.0f;
    vp.dvClipHeight =  2.0f;
    vp.dvMinZ       =  0.0f;
    vp.dvMaxZ       =  1.0f;
    hr = pD3D->CreateViewport( &pD3DViewport, NULL );    // Cr�e le Viewport
    if FAILED(hr) TraceErrorD3D(hr,99);
    hr = pD3DDevice->AddViewport(pD3DViewport);          // Associe le Viewport au device
    if FAILED(hr) TraceErrorD3D(hr,99);
    hr = pD3DViewport->SetViewport2(&vp);                // Passe les param�tres souhait�s au Viewport
    if FAILED(hr) TraceErrorD3D(hr,99);
    hr = pD3DDevice->SetCurrentViewport(pD3DViewport);   // Associe ce nouveau Viewport au device
    if FAILED(hr) TraceErrorD3D(hr,99);

    // Initialise le rectangle d�limitant le viewport
    pD3DViewportRect = new D3DRECT();
    pD3DViewportRect->x1 = pD3DViewportRect->y1 = 0;
    pD3DViewportRect->x2 = Screen_Width;
    pD3DViewportRect->y2 = Screen_Height;

    // 6) Cr�e l'objet Material : g�re les lumi�res et les effets de r�flections
    hr = pD3D->CreateMaterial(&pD3DMaterial, NULL);
    if FAILED(hr) TraceErrorD3D(hr,99);
    D3DMATERIAL        matiere;
    // Initialise les propri�t�s du mat�riel
    ZeroMemory(&matiere, sizeof(matiere));
    matiere.dwSize   = sizeof(matiere);
    // Param�tres pour la r�flection diffuse
    matiere.dcvDiffuse.r = (D3DVALUE)0.5;
    matiere.dcvDiffuse.g = (D3DVALUE)0.0;
    matiere.dcvDiffuse.b = (D3DVALUE)0.5;
    matiere.dcvDiffuse.a = (D3DVALUE)1.0;
    // Param�tres pour la r�flection ambiante
    matiere.dcvAmbient.r = (D3DVALUE)0.5;
    matiere.dcvAmbient.g = (D3DVALUE)0.0;
    matiere.dcvAmbient.b = (D3DVALUE)0.5;
    matiere.dcvAmbient.a = (D3DVALUE)1.0;
    // Autres param�tres : couleur et sharpness des pecular highlights
    matiere.dcvSpecular.r = (D3DVALUE)1.0;
    matiere.dcvSpecular.g = (D3DVALUE)1.0;
    matiere.dcvSpecular.b = (D3DVALUE)1.0;
    matiere.dcvSpecular.a = (D3DVALUE)1.0;
    matiere.dvPower = (float) 50.0;
    // Si l'on est en ramp emulation, on utilisera 16 entr�es de color map
    matiere.dwRampSize = 16;
    // Ce mat�riel devient le mat�riel courant
    hr = pD3DMaterial->SetMaterial(&matiere);
    if FAILED(hr) TraceErrorD3D(hr,99);
    // S�lectionne et valide un mat�riel pour le calcul de la sc�ne
    D3DMATERIALHANDLE  hmatiere;
    hr = pD3DMaterial->GetHandle(pD3DDevice, &hmatiere);    // R�cup�re son handle
    if FAILED(hr) TraceErrorD3D(hr,99);
    hr = pD3DDevice->SetLightState(D3DLIGHTSTATE_MATERIAL, hmatiere);
    if FAILED(hr) TraceErrorD3D(hr,99);

    // 7) Construction des matrices de transformations
    D3DVECTOR vPointVue   = D3DVECTOR(0.0f, 0.0f, -20.0f);  // positionn� sur l'axe des Z
    D3DVECTOR vRegardVers = D3DVECTOR(0.0f, 0.0f, 0.0f);    // regarde l'origine du rep�re
    D3DVECTOR vUpVec      = D3DVECTOR(0.0f, 0.0f, 1.0f);
    // Matrice du monde qui cont�le ma position et l'orientation des polygones dans l'espace 3D.
    D3DMATRIX matWorld;
    D3DUtil_SetIdentityMatrix(matWorld);
    pD3DDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matWorld );
    // Matrice de la vue qui d�finie la position et l'orientation de la cam�ra
    D3DMATRIX matView;
    D3DUtil_SetViewMatrix(matView, vPointVue, vRegardVers, vUpVec);
    pD3DDevice->SetTransform( D3DTRANSFORMSTATE_VIEW, &matView);
    // Matrice de projection qui d�finie comment la sc�ne 3D va �tre projet�e sur la surface 2D
    D3DMATRIX matProj;
    D3DUtil_SetProjectionMatrix(matProj, 1.57f, 1.0f, 1.0f, 100.0f);
    pD3DDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj);

    // 8) Initialise divers �tats du monde 3D
    // Lumi�re ambiante = white ambient light
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
//    hr = pD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);     // Filtre et lisse les textures, mais pose probl�me lors de l'utilisation de la keycolor car il reste des pixels roses	
    if FAILED(hr) TraceErrorD3D(hr,99);

    // 9) Verifie les capacit�s du hardware pour l'alphablending et l'alphatesting
    devDescHEL.dwSize = sizeof(devDescHEL);
    devDescHAL.dwSize = sizeof(devDescHAL);
    pD3DDevice->GetCaps(&devDescHAL, &devDescHEL);
    D3DDEVICEDESC *pDesc = (devDescHAL.dwFlags) ? &devDescHAL : &devDescHEL;
    // V�rifie la possibilit� de faire de l'alpha-blending
    if (    (pDesc->dpcTriCaps.dwDestBlendCaps & D3DPBLENDCAPS_INVSRCALPHA)
         && (pDesc->dpcTriCaps.dwSrcBlendCaps & D3DPBLENDCAPS_SRCALPHA))
    { supportAlphaBlend = true; }
    else MessageError("Direct3D Problem","Votre carte vid�o ne supporte l'alpha-blending en HAL",2);
    // V�rifie la possibiltit� de faire de l'alpha-testing
    if (pDesc->dpcTriCaps.dwAlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL)
    { supportAlphaTest = true; }
    else MessageError("Direct3D Problem","Votre carte vid�o ne supporte l'alpha-testing en HAL",2);

}
//----------------------------------------------------------------------------//

//=== Recherche si l'utilisations de Direct3D est possible ===================//
/*void TD3DScreen::SearchDevice(DWORD flags)
{   HRESULT hr;
    // 1) Enum�re les devices Direct3D possibles, en lui passant en param�tre les crit�res
    // sur lesquels il va accepter ou rejeter tel device d'apr�s les capacit�s que souhaitent l'appli.
	// PROBLEME avec DirectX 7.0
	if (flags & D3DLIB_ALPHABLENDING)
    { hr = D3DEnum_EnumerateDevices( ConfirmDevice_AphaBlending );
      if FAILED(hr) MessageError("Direct3D Erreur","Votre carte 3D n'est pas assez performante pour cette application !",2);
    }
}*/
//----------------------------------------------------------------------------//

//=== Indique d'o� provient l'erreur d�tect�e par Direct3D ===================//
void TD3DScreen::TraceErrorD3D(HRESULT hErr,int Gravite)
{   AnsiString Erreur;
    if (pD3D!=NULL) pDD->FlipToGDISurface();  // Revient en mode fen�tr�
    switch (hErr)
    {   case D3D_OK                  : Erreur = "D3D_OK : �trange ..."; break;
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

//=== D�salloue tous les objets et toute la m�moire ==========================//
void TD3DScreen::CleanD3D()
{  // Lib�re les objets D3D exclusivement utilis�s par cette classe
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

//=== Fonction � appeler avant le rendu d'une sc�ne 3D =======================//
void TD3DScreen::InitRenderScene3D()
{ HRESULT hr;
  // Efface le contenu du Viewport
  pD3DViewport->Clear2(1UL, pD3DViewportRect, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                       0x20202020, 1.0f, 0L);
  // On peut commencer � afficher la sc�ne 2D & 3D
  hr = pD3DDevice->BeginScene();
  if FAILED(hr) TraceErrorD3D(hr,99);
}
//----------------------------------------------------------------------------//

//=== A appeler apr�s avoir rendu la sc�ne 3D ================================//
void TD3DScreen::CloseRenderScene3D()
{  pD3DDevice->EndScene();
}
//----------------------------------------------------------------------------//

//=== Param�trage du rendu 3D ================================================//
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
   // Lib�re certains objets D3D exclusivement utilis�s par cette classe
   if (pD3DViewportRect) delete pD3DViewportRect;
   if (pD3DMaterial)     pD3DMaterial->Release();
   if (pD3DViewport)     pD3DViewport->Release();
   if (pD3DDevice)       pD3DDevice->Release();
   pD3DViewportRect = NULL;
   pD3DMaterial = NULL;
   pD3DViewport = NULL;
   pD3DDevice = NULL;
	
	// Changement de mode-�cran de tout ce qui est relatif � la 3D
	TDirectScreen::ToggleFullScreenMode();

	// Puis on r�initialise les divers objets Direct3D
	InitD3D();	
}
//----------------------------------------------------------------------------//


// Fin de la classe TD3DScreen /////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Classe SPRITE3DTILE                                                        //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur par d�faut ================================================//
Sprite3DTile::Sprite3DTile():
    SpriteTile()     // appel du constructeur de la classe ma�tre
{ ScrX1 = ScrY1 = ScrX2 = ScrY2 = 0;
  pDevice  = NULL;
  pTexture = NULL;
  TypeSprite = SPRITE_TILE_3D;
}
//----------------------------------------------------------------------------//

//=== Informations pour son rendu 3D =========================================//
void Sprite3DTile::Create3DSprite(LPDIRECT3DDEVICE3 pDev, D3DCOLOR i_col, D3DCOLOR i_spec, float tu1, float tv1, float tu2, float tv2, DDSurface &pFrame)
{  HRESULT hr;
   // Infos sur l'intensit� des couleurs
   cColor = i_col;
   cSpecular = i_spec;
   // Coordonn�es du sprite3D sur la texture
   TexU1 = tu1; TexV1 = tv1;
   TexU2 = tu2; TexV2 = tv2;
   // S�lectionne le device utilis� par le rendu 3D
   pDevice = pDev;
   pDDSurface = &pFrame;
   // Rafraichit le tableau des points sans tenir compte de la position des sprites
   RefreshVertices();
   // Obtient une interface vers une texture
   // Interface ce sprite � sa texture
   hr = pFrame.pDDSurf->QueryInterface(IID_IDirect3DTexture2, (void**)&pTexture);
   if FAILED(hr) D3DS->TraceErrorD3D(hr,1);

}
//----------------------------------------------------------------------------//

//=== Recalcule les vertices d'apr�s la position du sprite sur l'�cran =======//
void Sprite3DTile::RefreshVertices()
{ vSpriteVertices[0] = D3DTLVERTEX( D3DVECTOR(ScrX1, ScrY1, 0), 1, cColor, cSpecular, TexU1, TexV1);
  vSpriteVertices[1] = D3DTLVERTEX( D3DVECTOR(ScrX2, ScrY1, 0), 1, cColor, cSpecular, TexU2, TexV1);
  vSpriteVertices[2] = D3DTLVERTEX( D3DVECTOR(ScrX1, ScrY2, 0), 1, cColor, cSpecular, TexU1, TexV2);
  vSpriteVertices[3] = D3DTLVERTEX( D3DVECTOR(ScrX2, ScrY2, 0), 1, cColor, cSpecular, TexU2, TexV2);
}
//----------------------------------------------------------------------------//

//=== Rafra�chit seulement la position du sprite 3D ==========================//
void Sprite3DTile::RefreshPosition()
{ vSpriteVertices[0].sx = (float) ScrX1; vSpriteVertices[0].sy = (float) ScrY1;
  vSpriteVertices[1].sx = (float) ScrX2; vSpriteVertices[1].sy = (float) ScrY1;
  vSpriteVertices[2].sx = (float) ScrX1; vSpriteVertices[2].sy = (float) ScrY2;
  vSpriteVertices[3].sx = (float) ScrX2; vSpriteVertices[3].sy = (float) ScrY2;
}
//----------------------------------------------------------------------------//

//=== Positionne le sprite sur l'�cran =======================================//
// -> le sprite conserve sa taille d'origine
void Sprite3DTile::SetScreenCoord(int x, int y)
{  ScrX1 = x;
   ScrY1 = y;
   ScrX2 = x + Width;
   ScrY2 = y + Height;
   RefreshPosition();
}
// -> redimensionn� pour l'occasion
void Sprite3DTile::SetScreenCoord(int x1, int y1, int x2, int y2)
{  ScrX1 = x1; ScrY1 = y1; ScrX2 = x2; ScrY2 = y2;
   RefreshPosition();
}
// -> zoom�
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

//=== Red�fini les couleurs d'un point de la texture =========================//
void Sprite3DTile::SetVertexColor(int numVertex, D3DCOLOR col, D3DCOLOR spec)
{ vSpriteVertices[numVertex].color = col;
  vSpriteVertices[numVertex].color = spec;
}
//----------------------------------------------------------------------------//

//=== Affiche un sprite 3D � l'�cran =========================================//
void Sprite3DTile::SpriteRender()
{ HRESULT hr;
  // Archive les param�tres du rendu 3D concernant les z-comparaisons
  DWORD oldZState;
  hr = pDevice->GetRenderState(D3DRENDERSTATE_ZFUNC, &oldZState);
  if FAILED(hr) _asm int 3;
  // Tests de comparaisons sur les Z toujours fait
  hr = pDevice->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_ALWAYS);
  if FAILED(hr) _asm int 3;

  // V�rifie si la texture est en m�moire
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

  // Restaure les param�tres de rendu 3D sauvegard�s
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
	// Interface ce sprite � une texture
	hr = pDDSurface->pDDSurf->QueryInterface(IID_IDirect3DTexture2, (void**)&pTexture);
	if FAILED(hr) D3DS->TraceErrorD3D(hr,1);
}
//----------------------------------------------------------------------------//

// Fin de la classe SPRITE3DTILE ///////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Classe TD3DSPECIALEFFECT                                                   //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeutr par d��fait ==============================================//
TD3DSpecialEffect::TD3DSpecialEffect()
{ transition = false;
  colDay.SetColor(0x0);    // Couleur transparente
  sePresHour = seFutHour = HOUR_NONE;
  pDevice3D = NULL; 
};
//----------------------------------------------------------------------------//

//=== Cr�e le filtre en lui passant tous les param�tres requis ===============//
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

//=== Change de p�riode journali�re brusquement ==============================//
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

//=== Change de p�riode en fading ============================================//
void TD3DSpecialEffect::ChangeState(eHOUR state)
{  // V�rifie que l'on ne soit pas d�j� � cette p�riode
   if (sePresHour == state) return;

   // Calcule les diff�rents delta � parcourir avant d'atteindre la couleur d�sir�e
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

//=== Met � jour des couleurs du filtre ======================================//
void TD3DSpecialEffect::RefreshColorFilter()
{ vScreenFilter[0].color = actualColor.GetColor();
  vScreenFilter[1].color = actualColor.GetColor();
  vScreenFilter[2].color = actualColor.GetColor();
  vScreenFilter[3].color = actualColor.GetColor();
}
//----------------------------------------------------------------------------//

//=== Update le filtre �cran =================================================//
void TD3DSpecialEffect::UpdateSE()
{
   //*** En attendant de g�rer convenablement le jour et la nuit *************//
   if ((!transition) && ((rand() % 1000)==499))
   { if (sePresHour==HOUR_DAY) ChangeState(HOUR_NIGHT);
     else ChangeState(HOUR_DAY);
   }
   //*************************************************************************//

   // Sort si il n'y a rien � faire
   if (!transition) return;

   // Actualise la couleur du filtre courant
   actualColor.red   += deltaColor.red;
   actualColor.green += deltaColor.green;
   actualColor.blue  += deltaColor.blue;
   actualColor.alpha += deltaColor.alpha;
   RefreshColorFilter();

   // V�rifie que l'on ne soit pas arriv� � la couleur recherch�e
   if (actualColor.IsSimilaire(nextColor))
   {  sePresHour = seFutHour;
      transition = false;
   }
}
//----------------------------------------------------------------------------//

//=== Applique le filtre � l'�cran ===========================================//
void TD3DSpecialEffect::RenderFilter(bool backup)
{
  // Regarde si c'est la peine d'afficher le filtre
  if (actualColor.alpha ==0) return; 

  DWORD oldAlphablendState=0, oldSrcblendState=0, oldDestblendState=0;
  // Archive les param�tres du rendu 3D concernant l'aplha-blending
  if (backup)
  {  pDevice3D->GetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, &oldAlphablendState);
     pDevice3D->GetRenderState(D3DRENDERSTATE_SRCBLEND, &oldSrcblendState);
     pDevice3D->GetRenderState(D3DRENDERSTATE_DESTBLEND, &oldDestblendState);
  }

  // Param�tres d'Alpha-blending du filtre
  pDevice3D->SetRenderState(D3DRENDERSTATE_SRCBLEND,  D3DBLEND_SRCALPHA  );
  //pDevice3D->SetRenderState(D3DRENDERSTATE_SRCBLEND,  D3DBLEND_ONE);
  pDevice3D->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
  pDevice3D->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, true);
  pDevice3D->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, vScreenFilter, 4, NULL);

  // Restaure les param�tres d'alpha-blending du rendu 3D
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

//=== Appel� lors de l'intialisation, cette fonction v�rifie que le device ===//
// poss�de un minimum des capacit�s souhait�es.
HRESULT ConfirmDevice_AphaBlending(DDCAPS* pddDriverCaps, D3DDEVICEDESC* pd3dDeviceDesc)
{   // R�cup�re un triangle caps (Hardware ou software) et v�rifie l'alpha-blending
    // Get triangle caps (Hardware or software) and check for alpha blending
    LPD3DPRIMCAPS pdpc = &pd3dDeviceDesc->dpcTriCaps;
   if((pdpc->dwSrcBlendCaps & pdpc->dwDestBlendCaps & D3DBLEND_ONE) == 0)
   { return E_FAIL; }

   return S_OK;
}
//----------------------------------------------------------------------------//







