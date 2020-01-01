////////////////////////////////////////////////////////////////////////////////
//  Unité "MemLib.h" :  Interface des classes TMemMutexLib,                   //
//                                                                            //
//	Description :  Gère divers processus de communications tel que les boîtes //
//				   aux lettres, les sémpahores ou la mémoire partagée	      //
//                                                                            //
//  Copyright (C) 1999 Antoine Rey    antoine.rey@insa-lyon.fr                //
//																			  //
//  Version      : beta 0.02                                                  //
//  Creation     : 09/11/1999 		                                          //
//  Last upgrade : <none>	                                                  //
////////////////////////////////////////////////////////////////////////////////


#ifndef MemLibH
#define MemLibH

#ifdef MICROSOFT
#pragma warning(disable : 4100 4663 4018 4786)  
#endif

//---------------------------------------------------------------------- Include
#include <map>          // Librairie STL de gestion de dictionnaires à clé unique


// ------------------------------------------------------------ Classes externes

// ---------------------------------------------------------------------- Define

// -------------------------------------------------- Classes définies plus loin



// TMEMMUTEXLIB ////////////////////////////////////////////////////////////////
// Sémaphore de synchronisation												  //	 
class TMemMutexLib
{ //--------------------------------------------------- Attributs de la classe	
  private:	
	std::map<int, int>	mMutex;		// Bibliothèques de sémaphores <N°ID, Nb Jetons>
	int iNextID;

  //---------------------------------------------------- Méthodes de la classe
  public:
	TMemMutexLib();					// Constructeur par défaut
	~TMemMutexLib();				// Destructeur

	int NewMutex(int val=1);		// Crée un nouveau Mutex
	void DelMutex(int ind);			// Supprime un Mutex
	// Récupère le nb de jetons contenu dans le Mutex
	inline int GetValMutex(int ind)	{ return mMutex[ind]; }	
	// Libère un Mutex
	inline void FreeMutex(int ind) { mMutex[ind]=0; }
};
/////////////////////////// Fin de TMemMutexLib //////////////////////////////


//----------------------------------------------------------- Variables externes
extern TMemMutexLib* MemMutexLib;	// Librairie gérant les Mutex

//------------------------------------------------------------ Fonctions annexes
void InitGame_MemLib();				// Initialise les uniques instances de management de mémoire
void CloseGame_MemLib();			// Referme proprement ces outils de communications


#endif	// MemLibH
