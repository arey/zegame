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

//---------------------------------------------------------------------- Include
#include <fstream.h>          // Debuggage dans des fichiers
#include "DDLib.h"            // Debuggage � l'�cran

//------------------------------------------------------------ Classes utilis�es
class Personnage;			 // Infos sur les persos � debugger venant de "PersoLib.h"


//##ModelId=38A488B8026C
class GameDebug
{
private:
	//##ModelId=38A488B802AB
      ofstream  fout;                        // fichier de sortie
	//##ModelId=38A488B802A8
      char*     filename;                    // nom du fichier ds lequel on va tracer

	//##ModelId=38A488B802BA
      GameDebug();                           // Constructeur par d�faut interdit
public:
	//##ModelId=38A488B802BB
      GameDebug(char* fichier);              // Constructeur obligatoire
	//##ModelId=38A488B802B9
      ~GameDebug();                          // Destructeur
	//##ModelId=38A488B802B6
      void TRACER(char* fstring, ...);       // Permet d'�crire une cha�ne de caract�re format�e ds le fichier

      // Partie utilis�e pour le debuggage � l'�cran
	//##ModelId=38A488B802B0
      void STRINGF(int posX, int posY, unsigned int color, char* fstring, ...);    // Affiche une cha�ne format�e � l'�cran en (posX,posY)
	//##ModelId=38A488B802AE
      void TRACE_Perso(Personnage* perso);                                         // Affiche les principales infos d'un perso

};

//--- Instance de cette classe utilis�e ds le jeu ---
extern GameDebug *ZeGameDebug;

//--- Macros utilis�es lors du debuggage ---
#define TRACEDBG  ZeGameDebug->TRACER
#define TRACESCR  ZeGameDebug->STRINGF
#define DEBUGPANEL_PERSO(value)       ZeGameDebug->TRACE_Perso(value)


//---------------------------------------------------------------------------
#endif
