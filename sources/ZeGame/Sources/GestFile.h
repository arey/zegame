////////////////////////////////////////////////////////////////////////////////
//  Unité  "GestFile.h"  : S'occupe de mentionner tous les fichiers du jeu    //
//  Copyright (C) 1998 Antoine Rey    arey@insa-lyon.fr                       //
//                                    antrey@club-internet.fr                 //
//                                                                            //
//  Version      :  0.80                                                      //
//  Creation     : 22/06/1998                                                 //
//  Last upgrade : 20/06/1998                                                 //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef GestFileH
#define GestFileH

#include "DDLib.h"
#include "PersoLib.h"

// Enumère tous les fichiers que le jeu recquière
const int NbFiles = 16;
extern const char *AllFiles[];


// Regarde si tous les fichiers nécessaires existent -------------------------//
void VerifAllFiles();
// Charge les frames contenant les sprites -----------------------------------//
void LoadFrames();
// Décharge les frames contenant les sprites ---------------------------------//
void GestFile_UnloadFrames();

extern SpriteTilePersoLib  *STLHero;		 // Liste officielle des sprites du Héro
extern SpriteTilePersoLib  *STLOgre;		 // Liste officielle des sprites de l'Ogre
// Liste officielle de tous les types de personnage du jeu
extern SpriteTilePersoLib *STLModelePersos[NbCharacters];
#endif
