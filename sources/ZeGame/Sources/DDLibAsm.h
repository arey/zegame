////////////////////////////////////////////////////////////////////////////////
// Unité DDLibAsm.h : Regroupe les fonctions graphiques DirectDraw écrites en //
//                    assembleurs.                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef DDLibAsmH
#define DDLibAsmH

class DDSurface;
class SpriteTile;
class SpriteTileLib;


//--- Affiche un sprite à l'écran en rendant translucide sa couleur d'ombrage ---
extern void SpriteDrawShadow(int ScX,int ScY,int NumSprite,DDSurface *pSurface,SpriteTileLib* STLperso);
//--- Affiche un sprite à l'écran en rendant translucide sa couleur d'ombrage + clipping ---
extern void SpriteDrawShadowClip(int ScX,int ScY,int NumSprite,DDSurface *pSurface,SpriteTileLib* STLperso);
extern void SpriteDrawShadowClip(int ScX,int ScY,DDSurface *pSurface,SpriteTile* Sprite);
extern void SpriteDrawShadowClip(DDSurface *pSurface, DDSurface *pSrcSurface);
//--- Applique un filtre bleuté à l'écran ---
extern void NightFilter();
//--- Affiche un sprite en (ScX,ScY) avec de la transluence alpha ---
extern void SpriteDrawTransluenceClip(int ScX,int ScY,DDSurface *pSurface,SpriteTile* Sprite, int alpha);

//---------------------------------------------------------------------------
#endif	// DDLibAsmH
