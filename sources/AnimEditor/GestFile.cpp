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
#include <Stdio.h>
#include <vcl\vcl.h>

#include "GestFile.h"
#include "consts.h"
#include "fPrincipale.h"
#include "Animall.h"
#include "DDLib.h"

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
        if ((f = fopen(path.c_str(),"rb"))==NULL)
           MessageBoxA(NULL,path.c_str(),"FICHIER MANQUANT :",1);
        else fclose(f);
      }
};
//----------------------------------------------------------------------------//

SpriteTilePersoLib  *STLHero;  // Liste officielle des sprites du Héro
SpriteTilePersoLib  *STLOgre;  // Liste officielle des sprites de l'Ogre
// Liste officielle de tous les types de personnage du jeu
SpriteTilePersoLib *STLModelePersos[NbCharacters];

//--- Charge les frames contenant les sprites --------------------------------//
void LoadFrames()
{
/*   //=====================  HERO  ============================================//
   STLHero = new SpriteTilePersoLib;

   //=====================  OGRE  ============================================//
   STLOgre = new SpriteTilePersoLib;

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
   chem = CheminApplication+cPathPersos+"Hero\\Hero.mdl";
   ListeCharacters[HERO1]->LoadMdl(chem.c_str());
   //*** Ogre1 ***
   ListeCharacters[OGRE1] = new Character(OGRE1);
   ListeCharacters[OGRE1]->LoadIna("Ogre",&STLOgre);
   STLOgre->CreateSprIndex();
   ListeCharacters[OGRE1]->STLFrameSuperp->CreateSprIndex();
   chem = CheminApplication+cPathPersos+"Ogre\\Ogre.mdl";
   ListeCharacters[OGRE1]->LoadMdl(chem.c_str());

   //=== Charge le sprites et les infos concernant les Objets du Décors ======//
   // Alloue la place pour stocker N objets/décors
   NbMaxTileDecors=IDObjets->MaxAttribut()+1;
   ListeObjetsDecors = new TAnimTileDecors* [NbMaxTileDecors];
   // Puis pour chaque objets/décors, charge leurs anims
   for (int i=0 ; i < NbMaxTileDecors; i++)
   {  if (IDObjets->IsElement(i))
      {  ListeObjetsDecors[i] = new TAnimTileDecors();
         ListeObjetsDecors[i]->LoadIna(IDObjets->CleID(i));
         ListeObjetsDecors[i]->STLObjets->CreateSprIndex();
      } else
      {  ListeObjetsDecors[i] = NULL; }  // numéro sauté
   }*/
};
//----------------------------------------------------------------------------//
