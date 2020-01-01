////////////////////////////////////////////////////////////////////////////////
//  Unité  "GameDebug.h"  : classe GameDebug utilisé pour débugger en mode    //
//							plein écran.									  //
//  Copyright (C) 1999 Antoine Rey    antoine.rey@insa-lyon.fr				  //
//                                                                            //
//  Version      : 1.00                                                  	  //
//  Creation     : 12/06/1999                                                 //
//  Last upgrade : 12/06/1999                                                 //
////////////////////////////////////////////////////////////////////////////////


#ifndef GameDebugH
#define GameDebugH

#include <fstream.h>          // Debuggage dans des fichiers
#include "DDLib.h"            // Debuggage à l'écran
#include "PersoLib.h"         // Infos sur les persos à debugger

class GameDebug
{
private:
      ofstream  fout;                        // fichier de sortie
      char*     filename;                    // nom du fichier ds lequel on va tracer

      GameDebug();                           // Constructeur par défaut interdit
public:
      GameDebug(char* fichier);              // Constructeur obligatoire
      ~GameDebug();                          // Destructeur
      void TRACE(char* fstring, ...);        // Permet d'écrire une chaîne de caractère formatée ds le fichier

      // Partie utilisée pour le debuggage à l'écran
      void STRINGF(int posX, int posY, unsigned int color, char* fstring, ...);    // Affiche une chaîne formatée à l'écran en (posX,posY)
      void TRACE_Perso(Personnage* perso);                                         // Affiche les principales infos d'un perso

};

//--- Instance de cette classe utilisée ds le jeu ---
extern GameDebug *ZeGameDebug;

//--- Macros utilisées lors du debuggage ---
#define TRACEDBG  ZeGameDebug->TRACE
#define TRACESCR  ZeGameDebug->STRINGF
#define DEBUGPANEL_PERSO(value)       ZeGameDebug->TRACE_Perso(value)


//---------------------------------------------------------------------------
#endif
