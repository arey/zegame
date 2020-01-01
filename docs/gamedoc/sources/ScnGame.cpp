////////////////////////////////////////////////////////////////////////////////
//  Unit� "ScnGame.cpp" impl�mentation de la classe TGameScenario			  //
//                                                                            //
//	Description : Regroupe tout un ensemble de fonctions li�es au d�roulement //
//                des sc�narii du jeu					                      //
//                                                                            //
//  Copyright (C) 2000 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com	                  //
//  Version      : beta 0.01                                                  //
//  Creation     : 09/04/2000 		                                          //
//  Last upgrade : <none>	                                                  //
////////////////////////////////////////////////////////////////////////////////


//---------------------------------------------------------------------- Include
#include "ZeNain.h"         //--- Special for Visual C++ 6 MFC ---
#ifndef MICROSOFT			// Unit� principale du projet
#ifdef MAPEDITOR
#include "umapView.h"
#else   
#include "DirectLib.h"
#endif  // MapEditor
#endif  // Microsoft
#pragma hdrstop
#include "GameObject.h"		// Gestion des objets du jeu
#include "GameObjectList.h"	// Liste des GameObject du jeu
#include "TimeClock.h"		// Lib. de gestion temporelle du jeu
#include "ScnGame.h"		// Header du fichier


// ---------------------------------------------------------- variables externes
TGameScenario*  GameScenario = NULL;	// Instance unique de la classe


////////////////////////////////////////////////////////////////////////////////
// Classe TGAMESCENARIO                                                       //
////////////////////////////////////////////////////////////////////////////////


//=== Constructeur par d�faut ================================================//
TGameScenario::TGameScenario()
{	// Ajoute des associations n�Personnage/n�Script afin de pouvoir faire des
	// associations dynamiques lors de la cr�ation de personnages
	AddPersoWithScript(100, 8);		// level_New_NAIN_FENANT  / UnWorkerDwarf
	AddPersoWithScript(101, 9);		// level_New_NAIN_BOSSEUR / FullWorkerDwarf
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TGameScenario::~TGameScenario()
{	// Vide le dictionnaire contenant des associations n�Personnage/n�Script
	mPersoScript.clear();
}
//----------------------------------------------------------------------------//

//=== Ajoute un couple � mPersoScript ========================================//
void TGameScenario::AddPersoWithScript(int perso, int script)
{	mPersoScript.insert(std::make_pair(perso, script));
}
//----------------------------------------------------------------------------//

//=== Supprime la paire du dictionnaire ======================================//
void TGameScenario::ErasePersoWithScript(int perso)
{    mPersoScript.erase(perso);
}
//----------------------------------------------------------------------------//

//=== Recherche si le personnage existe dans la map ==========================//
bool TGameScenario::ExistPersoWithScript(int perso)
{   std::map<int, int>::iterator t = mPersoScript.find(perso);
    return (t!=mPersoScript.end());
}
//----------------------------------------------------------------------------//

//=== Renvoi le n� de script associ� au personnage (ou -1 si y'en a pas) =====//
int TGameScenario::GetNumScriptOfPerso(int perso)
{	if (!ExistPersoWithScript(perso)) return -1;
	return mPersoScript[perso];
}
//----------------------------------------------------------------------------//

//=== Indique si 'memo' fait parti de la m�moire interne du niveau ===========//
bool TGameScenario::isInLevelMemory(int memo)
{	for (unsigned int i=0; i < vLevelMemory.size() ; i++)
	{ if (vLevelMemory[i] == memo) return true;
    }
    return false;
}
//----------------------------------------------------------------------------//

//=== Rajoute un �l�ment de m�moire ==========================================//
void TGameScenario::AddLevelMemory(int memo)
{	vLevelMemory.push_back(memo);
}
//----------------------------------------------------------------------------//

//=== Perte de m�moire =======================================================//
void TGameScenario:: DelLevelMemory(int memo)
{	std::vector<int>::iterator iter;  // it�rateur sur le tab. dynamique d'entier
	for (iter = vLevelMemory.begin() ; iter != vLevelMemory.end() ; iter++)
    {   if (*iter == memo)
    	{	vLevelMemory.erase(iter);
        	return;
		}
    }
}
//----------------------------------------------------------------------------//
//////////////////// Fin de la classe TGAMESCENARIO ////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Fonctions ANNEXES														  //	
////////////////////////////////////////////////////////////////////////////////

//=== Initialisation de GameScenario =========================================//
void InitGame_GameScenario()
{	GameScenario = new TGameScenario();
}
//----------------------------------------------------------------------------//

//=== Fermeture de GameScenario ==============================================//
void CloseGame_GameScenario()
{	delete GameScenario;
	GameScenario = NULL;
}
//----------------------------------------------------------------------------//