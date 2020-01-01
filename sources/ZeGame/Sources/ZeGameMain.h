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
void InitPack_StartGame();		// à lancer au démarrage du jeu
void InitPack_StartLevel();		// à lancer afin de démarrer dans un niveau
void ClosePack_EndGame();		// à lancer avant de quitter l'application
void ClosePack_EndLevel();		// à lancer lors de la fin d'un niveau
void MainLoop();				// boucle principale du jeu exécutée en permancance
void SpecialKeys();				// interface homme-clavier spécifique à l'interface
void AfficheHelp();				// affichage de l'aide
void ToggleFullScreenMode();	// chgt de mode vidéo
void GamePaused();				// pause en plein jeu 	
void InitPak_Ini();				// récupere les paramètres de config ds zegame.ini



#endif // ZeGameMainH
