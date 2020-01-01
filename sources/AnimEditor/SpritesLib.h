////////////////////////////////////////////////////////////////////////////////
//  Unité "SpriteLib.h" : Library secondaire de gestion des sprites 	      //
//   	          	 s'appliquant par dessus des anims de persos & objets //
//  Copyright (C) 1998 Antoine Rey    arey@insa-lyon.fr                       //
//                                    antrey@club-internet.fr                 //
//  Version      : beta 0.10                                                  //
//  Creation     : 22/06/1999 21h14                                           //
//  Last upgrade : none                                                       //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#ifndef SpriteLib
#define SpriteLib

//----------------------------------------------------------------- Includes ---
#include "DDlib.h"		// Routines DirectDraw et gestionnaire de sprites des persos et objets
#include "Animall.h"	// Gestion des sprites & animations des persos & objets
#pragma hdrstop         // Accéleration par préchargement des headers
#include <map>			// Dictionnaires de la STL du C++


class SpriteTileGameLib : public SpriteTileLib
{
    typedef std::map<int,SpriteTile*> TSpriteMap;	// Dictionnaire associant un n° à un SpriteTile
	TSpriteMap  SpriteMap;

    typedef std::map<int,AnimSprite*> TAnimsMap;	// Dictionnaire associant un n° à un AnimSprite
    TAnimsMap	AnimsMap;

    void  AddFrameSprites(char *SpiFileName, SpriteFrame *pFr, int startIndex, unsigned int cColor, unsigned int cSpecular);
    void  AddOneSprite(SprInfo InfSprite,int CF,int CO,unsigned int RGBcf,unsigned int RGBco,
    			   unsigned int cColor, unsigned int cSpecular, SpriteFrame *pFr, int index);

  public:
	void LoadFileSpriteIna(const char* InaNameTresShort);	// Charge un .ina simplifié pour les anims de sprites "classiques"
    virtual ~SpriteTileGameLib();							// Destructeur
    void PlayAnim(int animID, int numSprite, int x, int y);	// Joue une anim en (x,y)

    SpriteTile*	GetSpriteTile(int numID);
    AnimSprite*	GetAnimSprite(int numID);

};

// Liste officielle des sprites ne faisant en particulier ni partie des objets, ni des persos
// mais pouvant être utilisés comme sprites superposés par plusieurs persos ou objets :
//  fummée, sang qui gicle, étincelles, explosions ...
extern SpriteTileGameLib *STGL;


//--- Convertit une chaîne de caractère au format hexa '0xA1EF' en un entier ---
unsigned int hexatoi(char* str);

#endif  // SpriteLib
