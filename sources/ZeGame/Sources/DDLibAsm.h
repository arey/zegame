////////////////////////////////////////////////////////////////////////////////
// Unit� DDLibAsm.h : Regroupe les fonctions graphiques DirectDraw �crites en //
//                    assembleurs.                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef DDLibAsmH
#define DDLibAsmH

class DDSurface;
class SpriteTile;
class SpriteTileLib;


//--- Affiche un sprite � l'�cran en rendant translucide sa couleur d'ombrage ---
extern void SpriteDrawShadow(int ScX,int ScY,int NumSprite,DDSurface *pSurface,SpriteTileLib* STLperso);
//--- Affiche un sprite � l'�cran en rendant translucide sa couleur d'ombrage + clipping ---
extern void SpriteDrawShadowClip(int ScX,int ScY,int NumSprite,DDSurface *pSurface,SpriteTileLib* STLperso);
extern void SpriteDrawShadowClip(int ScX,int ScY,DDSurface *pSurface,SpriteTile* Sprite);
extern void SpriteDrawShadowClip(DDSurface *pSurface, DDSurface *pSrcSurface);
//--- Applique un filtre bleut� � l'�cran ---
extern void NightFilter();
//--- Affiche un sprite en (ScX,ScY) avec de la transluence alpha ---
extern void SpriteDrawTransluenceClip(int ScX,int ScY,DDSurface *pSurface,SpriteTile* Sprite, int alpha);

//---------------------------------------------------------------------------
#endif	// DDLibAsmH
