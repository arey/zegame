////////////////////////////////////////////////////////////////////////////////
//  Unité  "GestFile.cpp"  : S'occupe de mentionner tous les fichiers du jeu  //
//  Copyright (C) 1998 Antoine Rey    arey@insa-lyon.fr                       //
//                                    antrey@club-internet.fr                 //
//                                                                            //
//  Version      :  0.80                                                      //
//  Creation     : 22/06/1998                                                 //
//  Last upgrade : 20/06/1998                                                 //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------- include
#ifdef CBUILDER
#include <Stdio.h>
#include <vcl\vcl.h>
#include "DirectLib.h"
#endif
#include "ZeNain.h"
#include "consts.h"
#include "Animall.h"
#include "DDLib.h"
#include "GameDebug.h"
#include "ImDecors2.h"
#include "GestFile.h"

//------------------------------------------------------------------------ const
const char *AllFiles[] =
    {"Hero\\HeroMar1.bmp", "Hero\\HeroMar1.spi",
     "Hero\\HeroMar2.bmp","Hero\\HeroMar2.spi",
     "Hero\\HeroPlG1.bmp","Hero\\HeroPlG1.spi",
     "Hero\\Hero.ina",
     "Ogre\\OgreMar1.bmp", "Ogre\\OgreMar1.spi",
     "Ogre\\OgrePlg1.bmp", "Ogre\\OgrePlg1.spi",
     "Ogre\\OgreFit1.bmp", "Ogre\\OgreFit1.spi",
     "Ogre\\OgreDed1.bmp", "Ogre\\OgreDed1.spi",
     "Ogre\\Ogre.ina", };


// Regarde si tous les fichiers nécessaires existent -------------------------//
void VerifAllFiles()
{
    FILE *f;
    for (int i=0 ; i<NbFiles ; i++)
      {
        AnsiString path = CheminApplication + cPathPersos + AllFiles[i];
		#ifdef CBUILDER
		if ((f = fopen(path.c_str(),"rb"))==NULL)
           MessageBoxA(NULL,path.c_str(),"FICHIER MANQUANT :",1);
		#else
		if ((f = fopen(LPCSTR(path),"rb"))==NULL)
           MessageBoxA(NULL, LPCSTR(path), "FICHIER MANQUANT :",1);
		#endif
        else fclose(f);
      }
};
//----------------------------------------------------------------------------//

SpriteTilePersoLib  *STLHero = NULL;			// Liste officielle des sprites du Héro
SpriteTilePersoLib  *STLOgre = NULL;			// Liste officielle des sprites de l'Ogre
// Liste officielle de tous les types de personnage du jeu
SpriteTilePersoLib *STLModelePersos[NbCharacters];

//--- Charge les frames contenant les sprites --------------------------------//
void LoadFrames()
{
   //=====================  HERO  ============================================//
   STLHero = new SpriteTilePersoLib;
   STLModelePersos[HERO1] = STLHero;
   //=== Charge les sprites et les infos sur les Modeles de personnages ======//
   // -> fichiers bmp, ina et mdl
   // Et Charge le contenu du fichier .mdl contenant toutes les informations concernant les
   // tiles de l'anim : zone au sol, pts divers, ...
   AnsiString chem;
   //*** Hero1 ***
   ListeCharacters[HERO1] = new Character(HERO1);
   ListeCharacters[HERO1]->LoadIna("Hero",&STLHero);
   STLHero->CreateSprIndex();
   ListeCharacters[HERO1]->STLFrameSuperp->CreateSprIndex();
   #ifndef NEW_FILE_MANAGER
   chem = CheminApplication+cPathPersos+"Hero\\Hero.mdl";
   #else
   chem = cPathPersos+"Hero\\Hero.mdl";
   #endif // NEW_FILE_MANAGER
   #ifdef CBUILDER
   ListeCharacters[HERO1]->LoadMdl(chem.c_str());
   #else
   ListeCharacters[HERO1]->LoadMdl(LPCSTR(chem));
   #endif
   
   //=====================  OGRE  ============================================//
   STLOgre = new SpriteTilePersoLib;
   STLModelePersos[OGRE1] = STLOgre;
   //*** Ogre1 ***
   ListeCharacters[OGRE1] = new Character(OGRE1);
   ListeCharacters[OGRE1]->LoadIna("Ogre",&STLOgre);
   STLOgre->CreateSprIndex();
   ListeCharacters[OGRE1]->STLFrameSuperp->CreateSprIndex();
   #ifndef NEW_FILE_MANAGER
   chem = CheminApplication+cPathPersos+"Ogre\\Ogre.mdl";
   #else
   chem = cPathPersos+"Ogre\\Ogre.mdl";
   #endif // NEW_FILE_MANAGER
   #ifdef CBUILDER
   ListeCharacters[OGRE1]->LoadMdl(chem.c_str());
   #else
   ListeCharacters[OGRE1]->LoadMdl(LPCSTR(chem));
   #endif
   
	//=====================  NAIN RENEGAT ====================================//
   SpriteTilePersoLib* STLperso = new SpriteTilePersoLib;
   STLModelePersos[NAINRENEGAT1] = STLperso;
   //*** Ogre1 ***
   ListeCharacters[NAINRENEGAT1] = new Character(NAINRENEGAT1, Character::CHARACTER_STATUE);
   ListeCharacters[NAINRENEGAT1]->LoadIna("NainReneg",&STLperso);
   STLperso->CreateSprIndex();
   ListeCharacters[NAINRENEGAT1]->STLFrameSuperp->CreateSprIndex();
   #ifndef NEW_FILE_MANAGER
   chem = CheminApplication+cPathPersos+"NainReneg\\NainReneg.mdl";
   #else
   chem = cPathPersos+"NainReneg\\NainReneg.mdl";
   #endif // NEW_FILE_MANAGER
   #ifdef CBUILDER
   ListeCharacters[NAINRENEGAT1]->LoadMdl(chem.c_str());
   #else
   ListeCharacters[NAINRENEGAT1]->LoadMdl(LPCSTR(chem));
   #endif


   //=============================  MOINE ====================================//
   STLperso = new SpriteTilePersoLib;
   STLModelePersos[MOINE1] = STLperso;
   ListeCharacters[MOINE1] = new Character(MOINE1, Character::CHARACTER_STATUE);
   ListeCharacters[MOINE1]->LoadIna("Moine",&STLperso);
   STLperso->CreateSprIndex();
   ListeCharacters[MOINE1]->STLFrameSuperp->CreateSprIndex();
   #ifndef NEW_FILE_MANAGER
   chem = CheminApplication+cPathPersos+"Moine\\Moine.mdl";
   #else
   chem = cPathPersos+"Moine\\Moine.mdl";
   #endif // NEW_FILE_MANAGER
   #ifdef CBUILDER
   ListeCharacters[MOINE1]->LoadMdl(chem.c_str());
   #else
   ListeCharacters[MOINE1]->LoadMdl(LPCSTR(chem));
   #endif


   //=============================  NAIN NORMAL ==============================//
   STLperso = new SpriteTilePersoLib;
   STLModelePersos[NAIN1] = STLperso;
   ListeCharacters[NAIN1] = new Character(NAIN1, Character::CHARACTER_STATUE);
   ListeCharacters[NAIN1]->LoadIna("Nain",&STLperso);
   STLperso->CreateSprIndex();
   ListeCharacters[NAIN1]->STLFrameSuperp->CreateSprIndex();
   #ifndef NEW_FILE_MANAGER
   chem = CheminApplication+cPathPersos+"Nain\\Nain.mdl";
   #else
   chem = cPathPersos+"Nain\\Nain.mdl";
   #endif // NEW_FILE_MANAGER
   #ifdef CBUILDER
   ListeCharacters[NAIN1]->LoadMdl(chem.c_str());
   #else
   ListeCharacters[NAIN1]->LoadMdl(LPCSTR(chem));
   #endif

   
   //=== Charge le sprites et les infos concernant les Objets du Décors ======//
   // Alloue la place pour stocker N objets/décors
   NbMaxTileDecors=IDObjets->MaxAttribut()+1;
   ListeObjetsDecors = new TAnimTileDecors* [NbMaxTileDecors];
   // Puis pour chaque objets/décors, charge leurs anims
   for (int i=0 ; i < NbMaxTileDecors; i++)
   {  if (IDObjets->IsElement(i))
      {	 ListeObjetsDecors[i] = new TAnimTileDecors(i);
         AnsiString shortFilename = IDObjets->CleID(i);
		 ListeObjetsDecors[i]->LoadIna(shortFilename);
         ListeObjetsDecors[i]->STLObjets->CreateSprIndex();
         AnsiString chem = cPathObjets+shortFilename+".mdl";
		 #ifndef NEW_FILE_MANAGER
		 chem = CheminApplication + chem;
		 #endif
		 #ifdef CBUILDER
		 ListeObjetsDecors[i]->LoadMdl(chem.c_str());
 		 #else
		 ListeObjetsDecors[i]->LoadMdl(LPCSTR(chem));
		 #endif
      } else
      {  ListeObjetsDecors[i] = NULL; }  // numéro sauté
   }
};
//----------------------------------------------------------------------------//

//=== Décharge les frames contenant les sprites ==============================//
void GestFile_UnloadFrames()
{	// Supprime les modèles de personnages
	for (int i=0 ; i < NbCharacters; i++)
	{	delete STLModelePersos[i];
		STLModelePersos[i] = NULL;
	}
}
//----------------------------------------------------------------------------//