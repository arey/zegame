////////////////////////////////////////////////////////////////////////////////
// Unité DDLibAsm.cpp : Regroupe les fonctions graphiques écrites en          //
//                      assembleurs.                                          //
////////////////////////////////////////////////////////////////////////////////

#ifdef CBUILDER
#pragma inline			// utilisation d'assembleur
#endif
#include "ZeNain.h"		// Fichier commun à tout le projet
#include "DDLibAsm.h"   // son propre header

// SpriteDrawShadow ////////////////////////////////////////////////////////////
// Dessine le sprite n° ??? dans la surface ???                               //
// en utilisant la transparence  et la couleur d'ombrage                      //
void SpriteDrawShadow(int ScX,int ScY,int NumSprite,DDSurface *pSurface,SpriteTileLib* STLperso)
{ 
    if ((NumSprite<0) || (STLperso==NULL) || (STLperso->SL_List==NULL)) return;  // vérifications
    if (NumSprite>=STLperso->NbSprites) return;

    // Remonte jusqu'au bon sprite de la SpriteListe
    SpriteTile *Sprite;
    Sprite = STLperso->SprIndex[NumSprite];

    // Blit du sprite à la main
    // Lock les surfaces
    pSurface->Lock2();          // surface de destination
    Sprite->pFrame->Lock2();    // surface source
    // => pour une résolution 16 bits
    static WORD *MapDest16 = (WORD*) pSurface->DDSurfDesc.lpSurface;
    static WORD *MapSrc16  = (WORD*) Sprite->pFrame->DDSurfDesc.lpSurface;
    switch (DS->Screen_Bits)
    { case 16 :
       { if(pSurface->DDSurfDesc.lpSurface && Sprite->pFrame->DDSurfDesc.lpSurface)
            { unsigned int DestPitch = pSurface->DDSurfDesc.lPitch,
                           SrcPitch  = Sprite->pFrame->DDSurfDesc.lPitch,
                           DestPtr   = (unsigned int) pSurface->DDSurfDesc.lpSurface,
                           SrcPtr    = (unsigned int) Sprite->pFrame->DDSurfDesc.lpSurface;

                   SrcPtr += Sprite->SrcRect.left*2 + Sprite->SrcRect.top*Sprite->pFrame->DDSurfDesc.lPitch;
                   int Height = Sprite->Height;
                   int Width  = Sprite->Width;
                   WORD TRANSPARENCECOLOR16 = (WORD) RGBto16((unsigned long) Sprite->cfRGB);
                   WORD TRANSLUENCECOLOR16  = (WORD) RGBto16((unsigned long) Sprite->coRGB);
				   #ifdef CBUILDER
				   asm{
				   #else
				   _asm{
				   #endif
                            mov		ESI, SrcPtr
                            mov		EDI, ScY
                            imul	EDI, DestPitch
                            add		EDI, ScX                // Add*2 car 2 octets par point
                            add		EDI, ScX
                            add		EDI, DestPtr
                            mov		ECX, Height
                       Loop1a:
                            push	ECX
                            push	ESI
                            push	EDI
                            mov		ECX, Width
                       Loop1b:
                       	    mov		ax, [esi]
			    cmp		ax, TRANSPARENCECOLOR16
			    je		Skip2
                            cmp         ax, TRANSLUENCECOLOR16
                            jne         SkipNotShadow
                            mov         AX,[EDI]
                            shr         AX,1
                            and         AX,0111101111101111b
                            mov         [EDI],AX
                            jmp Skip2
                       SkipNotShadow:
			    mov		[edi], ax
		       Skip2:
	                    add		esi, 2
			    add		edi, 2
			    loop	Loop1b
                            pop		EDI
                            pop		ESI
                            add		EDI, DestPitch
                            pop		ECX
                            add		ESI, SrcPitch
                            loop	Loop1a
                    }

             }
       }; break;
    }
    // Unlock les surfaces
    Sprite->pFrame->UnLock();    // surface source
    pSurface->UnLock();          // surface de destination
//    if (!pSurface->CheckSurfaces()) TraceErrorDD(hr,99);
//    if (!Sprite->pFrame->CheckSurfaces()) TraceErrorDD(hr,99);

};
////////////////////////////////////////////////////////////////////////////////


// SpriteDrawShadowClip/////////////////////////////////////////////////////////
// Dessine le sprite n° ? dans la surface ??? en gérant le clipping à la main //
// en utilisant la transparence  et la couleur d'ombrage                      //
void SpriteDrawShadowClip(int ScX,int ScY,int NumSprite,DDSurface *pSurface,SpriteTileLib* STLperso)
{
    if ((NumSprite<0) || (STLperso==NULL) || (STLperso->SL_List==NULL)) return;  // vérifications
    if (NumSprite>=STLperso->NbSprites) return;

    // Remonte jusqu'au bon sprite de la SpriteListe
    SpriteTile *Sprite;
    Sprite = STLperso->SprIndex[NumSprite];

    SpriteDrawShadowClip(ScX, ScY, pSurface, Sprite);
}
////////////////////////////////////////////////////////////////////////////////

// SpriteDrawShadowClip/////////////////////////////////////////////////////////
// Dessine le sprite n° ? dans la surface ??? en gérant le clipping à la main //
// en utilisant la transparence  et la couleur d'ombrage                      //
void SpriteDrawShadowClip(int ScX,int ScY,DDSurface *pSurface,SpriteTile* Sprite)
{   // Prépare le clipping
    int depSpriteX = Sprite->SrcRect.left;
    int depSpriteY = Sprite->SrcRect.top;
    int sprHeight  = Sprite->Height;
    int sprWidth   = Sprite->Width;

    // élimine cache les sprites en dehors de l'écran
    if ((ScX >= pSurface->PixWidth) || (ScY >= pSurface->PixHeight)) return;
    if ((ScX+sprWidth-1 < 0) || (ScY+sprHeight-1 < 0)) return;

    if (ScX < 0) {sprWidth += ScX; depSpriteX -= ScX; ScX = 0; }
    if (ScY < 0) {sprHeight += ScY; depSpriteY -= ScY; ScY = 0; }
    if (ScX+sprWidth >= pSurface->PixWidth)
     { //depSpriteX += ScX+sprWidth-pSurface->PixWidth;
       sprWidth -= ScX+sprWidth-pSurface->PixWidth; }
    if (ScY+sprHeight >= pSurface->PixHeight)
     { //depSpriteY += ScY+sprHeight-pSurface->PixHeight;
       sprHeight -= ScY+sprHeight-pSurface->PixHeight; }
		
    // Blit du sprite à la main
    // Lock les surfaces
    pSurface->Lock2();          // surface de destination
    Sprite->pFrame->Lock2();    // surface source
    // => pour une résolution 16 bits
    static WORD *MapDest16 = (WORD*) pSurface->DDSurfDesc.lpSurface;
    static WORD *MapSrc16  = (WORD*) Sprite->pFrame->DDSurfDesc.lpSurface;
    switch (DS->Screen_Bits)
    { case 16 :
       { if(pSurface->DDSurfDesc.lpSurface && Sprite->pFrame->DDSurfDesc.lpSurface)
            { unsigned int DestPitch = pSurface->DDSurfDesc.lPitch,
                           SrcPitch  = Sprite->pFrame->DDSurfDesc.lPitch,
                           DestPtr   = (unsigned int) pSurface->DDSurfDesc.lpSurface,
                           SrcPtr    = (unsigned int) Sprite->pFrame->DDSurfDesc.lpSurface;

                   SrcPtr += depSpriteX*2 + depSpriteY*Sprite->pFrame->DDSurfDesc.lPitch;
                   int Height = sprHeight;
                   int Width  = sprWidth;		
				   Height--;	// [BUG POTENTIEL] 16/01/2000
				   if (Height <= 0) return;	// Ajustement du 13/02/2000
                   WORD TRANSPARENCECOLOR16 = (WORD) RGBto16((unsigned long) Sprite->cfRGB);
                   WORD TRANSLUENCECOLOR16  = (WORD) RGBto16((unsigned long) Sprite->coRGB);
                   #ifdef CBUILDER
				   asm{
				   #else
				   _asm{
				   #endif
                            mov		ESI, SrcPtr
                            mov		EDI, ScY
                            imul	EDI, DestPitch
                            add		EDI, ScX                // Add*2 car 2 octets par point
                            add		EDI, ScX
                            add		EDI, DestPtr
                            mov		ECX, Height
                       Loop10a:
                            push	ECX
                            push	ESI
                            push	EDI
                            mov		ECX, Width
                       Loop10b:
                       	    mov		ax, [esi]
			    cmp		ax, TRANSPARENCECOLOR16
			    je		Skip3
                            cmp         ax, TRANSLUENCECOLOR16
                            jne         SkipNotShadow2
                            mov         AX,[EDI]
                            shr         AX,1
                            and         AX,0111101111101111b
                            mov         [EDI],AX
                            jmp Skip3
                       SkipNotShadow2:
			    mov		[edi], ax
		       Skip3:
	                    add		esi, 2
			    add		edi, 2
			    loop	Loop10b
                            pop		EDI
                            pop		ESI
                            add		EDI, DestPitch
                            pop		ECX
                            add		ESI, SrcPitch
                            loop	Loop10a
                    }

             }
       }; break;
    }
    // Unlock les surfaces
    Sprite->pFrame->UnLock();    // surface source
    pSurface->UnLock();          // surface de destination
};
////////////////////////////////////////////////////////////////////////////////

//=== Affiche un sprite déjà clippé de la pSrcSurface vers pSurface ==========// 
void SpriteDrawShadowClip(DDSurface *pSurface, DDSurface *pSrcSurface)
{	// Clipping déjà préparé dans la surface source pSrcSurface
    int depSpriteX = pSrcSurface->SrcRect.left;
    int depSpriteY = pSrcSurface->SrcRect.top;
    int sprHeight  = pSrcSurface->SrcRect.bottom - pSrcSurface->SrcRect.top;
    int sprWidth   = pSrcSurface->SrcRect.right - pSrcSurface->SrcRect.left;
    int ScX = pSrcSurface->DestRect.left;
    int ScY = pSrcSurface->DestRect.top;

    // élimine cache les sprites en dehors de l'écran
    if ((ScX >= pSurface->PixWidth) || (ScY >= pSurface->PixHeight)) return;
    if ((ScX+sprWidth-1 < 0) || (ScY+sprHeight-1 < 0)) return;

    // Blit du sprite à la main
    // Lock les surfaces
    pSurface->Lock2();          // surface de destination
    pSrcSurface->Lock2();    	// surface source
    // => pour une résolution 16 bits
    static WORD *MapDest16 = (WORD*) pSurface->DDSurfDesc.lpSurface;
    static WORD *MapSrc16  = (WORD*) pSrcSurface->DDSurfDesc.lpSurface;
    switch (DS->Screen_Bits)
    { case 16 :
       { if(pSurface->DDSurfDesc.lpSurface && pSrcSurface->DDSurfDesc.lpSurface)
            { unsigned int DestPitch = pSurface->DDSurfDesc.lPitch,
                           SrcPitch  = pSrcSurface->DDSurfDesc.lPitch,
                           DestPtr   = (unsigned int) pSurface->DDSurfDesc.lpSurface,
                           SrcPtr    = (unsigned int) pSrcSurface->DDSurfDesc.lpSurface;

                   SrcPtr += depSpriteX*2 + depSpriteY*pSrcSurface->DDSurfDesc.lPitch;
                   int Height = sprHeight;
                   int Width  = sprWidth;
                   WORD TRANSPARENCECOLOR16 = (WORD) RGBto16((unsigned long) DDLIB_COLOR_TRANSPARENT);
                   WORD TRANSLUENCECOLOR16  = (WORD) RGBto16((unsigned long) DDLIB_COLOR_TRANSLUENT);
                   #ifdef CBUILDER
				   asm{
				   #else
				   _asm{
				   #endif
                            mov		ESI, SrcPtr
                            mov		EDI, ScY
                            imul	EDI, DestPitch
                            add		EDI, ScX                // Add*2 car 2 octets par point
                            add		EDI, ScX
                            add		EDI, DestPtr
                            mov		ECX, Height
                       Loop20a:
                            push	ECX
                            push	ESI
                            push	EDI
                            mov		ECX, Width
                       Loop20b:
                       	    mov		ax, [esi]
			    cmp		ax, TRANSPARENCECOLOR16
			    je		Skip23
                            cmp         ax, TRANSLUENCECOLOR16
                            jne         SkipNotShadow22
                            mov         AX,[EDI]
                            shr         AX,1
                            and         AX,0111101111101111b
                            mov         [EDI],AX
                            jmp Skip23
                       SkipNotShadow22:
			    mov		[edi], ax
		       Skip23:
	                    add		esi, 2
			    add		edi, 2
			    loop	Loop20b
                            pop		EDI
                            pop		ESI
                            add		EDI, DestPitch
                            pop		ECX
                            add		ESI, SrcPitch
                            loop	Loop20a
                    }

             }
       }; break;
    }
    // Unlock les surfaces
    pSrcSurface->UnLock();    	// surface source
    pSurface->UnLock();         // surface de destination
}
//----------------------------------------------------------------------------//



//=== Applique un filtre bleuté simulant la nuit à tout l'écran ==============//
// -> en le modifiant un peu, on ne pourrait appliquer ce filtre qu'à une
//    partie de l'écran permettant ainsi  d'exclure, par exemple, l'inventaire.
void NightFilter()
{   // Lock la surface sur laquelle on va appliquer le filtre de nuit
    DS->pDDSBack->Lock2();
    unsigned int DestPitch = DS->pDDSBack->DDSurfDesc.lPitch;

    switch (DS->Screen_Bits)
    { case 16 :
       { if(DS->pDDSBack->DDSurfDesc.lpSurface)
         { unsigned int DestPtr   = (unsigned int) DS->pDDSBack->DDSurfDesc.lpSurface;

           unsigned int height = DS->pDDSBack->PixHeight;
           unsigned int width  = DS->pDDSBack->PixWidth;
           #ifdef CBUILDER
				   asm{
				   #else
				   _asm{
				   #endif     
					mov		EDI, DestPtr
                    mov		ECX, height
               Loop2a:
                    push	ECX
                    push	EDI
                    mov		ECX, width
               Loop2b:
                    mov		AX,[EDI]
                    mov         BX,AX
                    and         BX,1111111111100000b    // R,V
                    and         AX,0000000000011111b    // B
                    shr         BX,1
                    and         BX,0111101111100000b
                    add         AX,BX
                    mov         [EDI],AX
                    add		EDI, 2
                    loop	Loop2b

                    pop		EDI
                    add		EDI,DestPitch
                    pop		ECX
                    loop	Loop2a
              }
           }
       }; break;
    }

    // Unlock la surface nuitée
    DS->pDDSBack->UnLock();
//    if (!DS->pDDSBack->CheckSurfaces()) TraceErrorDD(hr,99);
}
//----------------------------------------------------------------------------//


//=== Affiche un sprite en (ScX,ScY) avec de la transluence alpha ============//
// paramètres : 
//  - alpha	=  compris entre 0 et 255 et sera réajusté suivant le nb de bits par pixel
//  - (ScX, ScY) = point supérieur gauche
void SpriteDrawTransluenceClip(int ScX,int ScY,DDSurface *pSurface,SpriteTile* Sprite, int alpha)
{	// Prépare le clipping
    int depSpriteX = Sprite->SrcRect.left;
    int depSpriteY = Sprite->SrcRect.top;
    int sprHeight  = Sprite->Height;
    int sprWidth   = Sprite->Width;

    // élimine cache les sprites en dehors de l'écran
    if ((ScX >= pSurface->PixWidth) || (ScY >= pSurface->PixHeight)) return;
    if ((ScX+sprWidth-1 < 0) || (ScY+sprHeight-1 < 0)) return;

    if (ScX < 0) {sprWidth += ScX; depSpriteX -= ScX; ScX = 0; }
    if (ScY < 0) {sprHeight += ScY; depSpriteY -= ScY; ScY = 0; }
    if (ScX+sprWidth >= pSurface->PixWidth)
     { //depSpriteX += ScX+sprWidth-pSurface->PixWidth;
       sprWidth -= ScX+sprWidth-pSurface->PixWidth; }
    if (ScY+sprHeight >= pSurface->PixHeight)
     { //depSpriteY += ScY+sprHeight-pSurface->PixHeight;
       sprHeight -= ScY+sprHeight-pSurface->PixHeight; }

    // Vérouille les 2 surfaces DirectDraw et récupère leur pointeur
    pSurface->Lock2();          // surface de destination
    Sprite->pFrame->Lock2();    // surface source
	
	// Récupère les pointeurs sur les surfaces DirectDraw
	BYTE *pMapDest = (BYTE*) pSurface->DDSurfDesc.lpSurface;
	BYTE *pMapSrc  = (BYTE*) Sprite->pFrame->DDSurfDesc.lpSurface;
	
	// Pitch des surfaces
	int DestPitch = pSurface->DDSurfDesc.lPitch;
    int SrcPitch  = Sprite->pFrame->DDSurfDesc.lPitch;

	// A PARTIR D'ICI, NE MARCHE QU'EN 16 BITS
	if (DS->Screen_Bits != 16) return;
	
	// Préparation : début de transfert de données
	pMapDest += ScX*2 + ScY*DestPitch;
	pMapSrc += depSpriteX*2 + depSpriteY*SrcPitch;

	const int RMask = 0xF800;	// 1111100000000000
	const int VMask = 0x07E0;	// 0000011111100000
	const int BMask = 0x001F;	// 0000000000011111
	WORD TRANSPARENCECOLOR16 = (WORD) RGBto16((unsigned long) Sprite->cfRGB);

	int invAlpha = 255 - alpha;

	// Codé en asm
	#ifdef CBUILDER
	asm
	#else
	_asm
	#endif     
	{		mov		ESI, pMapSrc
			mov		EDI, pMapDest
			mov		ECX, sprHeight

		LoopLine:
			// Parcours chaque ligne
			push	ECX		// Nb de lignes à tracer
            push	ESI		// Adr du buffer source au début de la ligne
            push	EDI		//	"		"	 destination
			mov		ECX, sprWidth
		LoopPixel:
			// composante Rouge
			mov		AX, [ESI]
			mov		BX, [EDI]
			cmp		AX, TRANSPARENCECOLOR16
			je		PixelNotDraw

			and		EAX, RMask
			and		EBX, RMask
			imul	EAX, alpha
			imul	EBX, invAlpha
			add		EAX, EBX
			sar		EAX, 8
			and		EAX, RMask
			// composante Vert
			mov		DX, [ESI]
			mov		BX, [EDI]
			and		EDX, VMask
			and		EBX, VMask
			imul	EDX, alpha
			imul	EBX, invAlpha
			add		EDX, EBX
			sar		EDX, 8
			and		EDX, VMask
			add		EAX, EDX
			// composante Bleu
			mov		DX, [ESI]
			mov		BX, [EDI]
			and		EDX, BMask
			and		EBX, BMask
			imul	EDX, alpha
			imul	EBX, invAlpha
			add		EDX, EBX
			sar		EDX, 8
			and		EDX, BMask
			add		EAX, EDX
			
			mov		[EDI], AX

		PixelNotDraw:
			add		EDI, 2
			add		ESI, 2

			loop	LoopPixel
			// Ici, on est arrivé au bout de la ligne et l'on passe à la suivante
            pop		EDI
            pop		ESI
            add		EDI, DestPitch
            pop		ECX
            add		ESI, SrcPitch
			loop	LoopLine
			

	}
	// Unlock les surfaces
    Sprite->pFrame->UnLock();    // surface source
    pSurface->UnLock();          // surface de destination

}
//----------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////////////////



/*
First you mask off each color component of the source and destination pixels, multiply the source component by the Alpha value, multiply the destination component by the Inverse Alpha value, add the two, divide the result by the maximum Alpha value (in this case 256, which can be accomplished by a bit shift of 8 to the right), bit-and with the component mask again, and finally bit-or the three color components back together to produce the output pixel value. (Note, using an unsigned char Alpha mask only allows an alpha of up to 255, so even if the alpha channel is all 255, the colors of the source image will be ever so slightly reduced from their full intensity.) 
RMask, GMask, and BMask are assumed to be the bit-masks for that color channel, usually devined from DirectDraw's DDPixelFormat, or a fixed set of masks for Windows DIBs. source and dest are pointers to the source and destination pixels, and alpha is an unsigned char pointer to the alpha channel pixel (which may be fixed for the whole blit). This should work for 16-bit unsigned short or 32-bit unsigned long source and dest pixels. 


	int ialpha = 256 - *alpha; //inverse alpha
	*dest = (RMask & (((*source & RMask) * *alpha +
	           (*dest & RMask) * ialpha) >>8)) |
	        (GMask & (((*source & GMask) * *alpha +
	           (*dest & GMask) * ialpha) >>8)) |
	        (BMask & (((*source & BMask) * *alpha +
	           (*dest & BMask) * ialpha) >>8));


*/