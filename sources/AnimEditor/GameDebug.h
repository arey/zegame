////////////////////////////////////////////////////////////////////////////////
//  Unit�  "GameDebug.h"  : classe GameDebug utilis� pour d�bugger en mode    //
//							plein �cran.									  //
//  Copyright (C) 1999 Antoine Rey    antoine.rey@insa-lyon.fr				  //
//                                                                            //
//  Version      : 1.00                                                  	  //
//  Creation     : 12/06/1999                                                 //
//  Last upgrade : 12/06/1999                                                 //
////////////////////////////////////////////////////////////////////////////////


#ifndef GameDebugH
#define GameDebugH

#include <fstream.h>          // Debuggage dans des fichiers
#include "DDLib.h"            // Debuggage � l'�cran
#include "PersoLib.h"         // Infos sur les persos � debugger

class GameDebug
{
private:
      ofstream  fout;                        // fichier de sortie
      char*     filename;                    // nom du fichier ds lequel on va tracer

      GameDebug();                           // Constructeur par d�faut interdit
public:
      GameDebug(char* fichier);              // Constructeur obligatoire
      ~GameDebug();                          // Destructeur
      void TRACE(char* fstring, ...);        // Permet d'�crire une cha�ne de caract�re format�e ds le fichier

      // Partie utilis�e pour le debuggage � l'�cran
      void STRINGF(int posX, int posY, unsigned int color, char* fstring, ...);    // Affiche une cha�ne format�e � l'�cran en (posX,posY)
      void TRACE_Perso(Personnage* perso);                                         // Affiche les principales infos d'un perso

};

//--- Instance de cette classe utilis�e ds le jeu ---
extern GameDebug *ZeGameDebug;

//--- Macros utilis�es lors du debuggage ---
#define TRACEDBG  ZeGameDebug->TRACE
#define TRACESCR  ZeGameDebug->STRINGF
#define DEBUGPANEL_PERSO(value)       ZeGameDebug->TRACE_Perso(value)


//---------------------------------------------------------------------------
#endif
