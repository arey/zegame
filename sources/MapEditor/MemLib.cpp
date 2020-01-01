////////////////////////////////////////////////////////////////////////////////
//  Unit� "MemLib.cpp" :  Impl�mentation des classes TMemMutexLib			  //
//                                                                            //
//	Description :  G�re divers processus de communications tel que les bo�tes //
//				   aux lettres, les s�mpahores ou la m�moire partag�e	      //
//                                                                            //
//  Copyright (C) 1999 Antoine Rey    antoine.rey@insa-lyon.fr                //
//																			  //
//  Version      : beta 0.02                                                  //
//  Creation     : 09/11/1999 		                                          //
//  Last upgrade : <none>	                                                  //
////////////////////////////////////////////////////////////////////////////////



#ifdef MICROSOFT
#pragma warning(disable : 4786 4100)  
#endif

//---------------------------------------------------------------------- Include
#ifdef MAPEDITOR
#include "Editor.h"
#endif
#pragma hdrstop
#include "ZeNain.h"         //--- Special for Visual C++ 6 MFC ---
#include "Consts.h"			// Constantes du jeu
#include "MemLib.h"			// Son Header

//----------------------------------------------------------------------- define

//----------------------------------------------------------- Variables externes
TMemMutexLib* MemMutexLib = NULL;	// Librairie g�rant les Mutex


////////////////////////////////////////////////////////////////////////////////
// Classe TMEMMUTEXLIB								                          //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur par d�faut ================================================//
TMemMutexLib::TMemMutexLib()
{	iNextID = 1;
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TMemMutexLib::~TMemMutexLib()
{	// Vide la biblioth. des Mutex
	mMutex.clear();
}
//----------------------------------------------------------------------------//

//=== Cr�e un nouveau Mutex ==================================================//
int TMemMutexLib::NewMutex(int val)
{	mMutex[iNextID++] = val;
	return iNextID-1;
}
//----------------------------------------------------------------------------//

//=== Supprime un Mutex ======================================================//
inline void TMemMutexLib::DelMutex(int ind)
{	mMutex.erase(ind);
}
//----------------------------------------------------------------------------//

////////////////////////// Fin de TMEMMUTEXLIB /////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Fonctions ANNEXES														  //
////////////////////////////////////////////////////////////////////////////////

//=== Initialise les uniques instances de management de m�moire ==============//
void InitGame_MemLib()
{	// Alloue le gestionnaire de Mutex
	MemMutexLib = new TMemMutexLib();	
}
//----------------------------------------------------------------------------//

//=== Referme proprement ces outils de communications ========================//
void CloseGame_MemLib()
{	// D�salloue le gestionnaire de Mutex
	delete MemMutexLib;  MemMutexLib = NULL;
}
//----------------------------------------------------------------------------//
