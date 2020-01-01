////////////////////////////////////////////////////////////////////////////////
//  File		:	ZeGameMain.h											  //
//                                                                            //
//	Description :   Fonctions principales du jeu (inits, shut down, ...)	  //
//                                                                            //
//  Copyright (C) 1998 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@ifrance.com		                  //
////////////////////////////////////////////////////////////////////////////////


#ifndef ZeGameMainH
#define ZeGameMainH

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// -------------------------------------------------------------------- fonctions
void MainZeGame();				// fonction Main() du jeu
void CloseZeGame();				// quitte proprement le jeu
void InitPack_StartGame();		// � lancer au d�marrage du jeu
void InitPack_StartLevel();		// � lancer afin de d�marrer dans un niveau
void ClosePack_EndGame();		// � lancer avant de quitter l'application
void ClosePack_EndLevel();		// � lancer lors de la fin d'un niveau
void MainLoop();				// boucle principale du jeu ex�cut�e en permancance
void SpecialKeys();				// interface homme-clavier sp�cifique � l'interface
void AfficheHelp();				// affichage de l'aide
void ToggleFullScreenMode();	// chgt de mode vid�o
void GamePaused();				// pause en plein jeu 	
void InitPak_Ini();				// r�cupere les param�tres de config ds zegame.ini



#endif // ZeGameMainH
