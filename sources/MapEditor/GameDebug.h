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

//---------------------------------------------------------------------- Include
#include <fstream.h>          // Debuggage dans des fichiers
#include "DDLib.h"            // Debuggage à l'écran

//------------------------------------------------------------ Classes utilisées
class Personnage;			 // Infos sur les persos à debugger venant de "PersoLib.h"


//##ModelId=38A488B8026C
class GameDebug
{
private:
	//##ModelId=38A488B802AB
      ofstream  fout;                        // fichier de sortie
	//##ModelId=38A488B802A8
      char*     filename;                    // nom du fichier ds lequel on va tracer

	//##ModelId=38A488B802BA
      GameDebug();                           // Constructeur par défaut interdit
public:
	//##ModelId=38A488B802BB
      GameDebug(char* fichier);              // Constructeur obligatoire
	//##ModelId=38A488B802B9
      ~GameDebug();                          // Destructeur
	//##ModelId=38A488B802B6
      void TRACER(char* fstring, ...);       // Permet d'écrire une chaîne de caractère formatée ds le fichier

      // Partie utilisée pour le debuggage à l'écran
	//##ModelId=38A488B802B0
      void STRINGF(int posX, int posY, unsigned int color, char* fstring, ...);    // Affiche une chaîne formatée à l'écran en (posX,posY)
	//##ModelId=38A488B802AE
      void TRACE_Perso(Personnage* perso);                                         // Affiche les principales infos d'un perso

};

//--- Instance de cette classe utilisée ds le jeu ---
extern GameDebug *ZeGameDebug;

//--- Macros utilisées lors du debuggage ---
#define TRACEDBG  ZeGameDebug->TRACER
#define TRACESCR  ZeGameDebug->STRINGF
#define DEBUGPANEL_PERSO(value)       ZeGameDebug->TRACE_Perso(value)


//---------------------------------------------------------------------------
#endif
