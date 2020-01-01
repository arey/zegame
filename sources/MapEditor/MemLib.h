////////////////////////////////////////////////////////////////////////////////
//  Unit� "MemLib.h" :  Interface des classes TMemMutexLib,                   //
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


#ifndef MemLibH
#define MemLibH

#ifdef MICROSOFT
#pragma warning(disable : 4100 4663 4018 4786)  
#endif

//---------------------------------------------------------------------- Include
#include <map>          // Librairie STL de gestion de dictionnaires � cl� unique


// ------------------------------------------------------------ Classes externes

// ---------------------------------------------------------------------- Define

// -------------------------------------------------- Classes d�finies plus loin



// TMEMMUTEXLIB ////////////////////////////////////////////////////////////////
// S�maphore de synchronisation												  //	 
class TMemMutexLib
{ //--------------------------------------------------- Attributs de la classe	
  private:	
	std::map<int, int>	mMutex;		// Biblioth�ques de s�maphores <N�ID, Nb Jetons>
	int iNextID;

  //---------------------------------------------------- M�thodes de la classe
  public:
	//##ModelId=38A488AA007E
	TMemMutexLib();					// Constructeur par d�faut
	//##ModelId=38A488AA007D
	~TMemMutexLib();				// Destructeur

	//##ModelId=38A488AA007B
	int NewMutex(int val=1);		// Cr�e un nouveau Mutex
	//##ModelId=38A488AA0079
	void DelMutex(int ind);			// Supprime un Mutex
	// R�cup�re le nb de jetons contenu dans le Mutex
	//##ModelId=38A488AA004E
	inline int GetValMutex(int ind)	{ return mMutex[ind]; }	
	// Lib�re un Mutex
	//##ModelId=38A488AA004C
	inline void FreeMutex(int ind) { mMutex[ind]=0; }
};
/////////////////////////// Fin de TMemMutexLib //////////////////////////////


//----------------------------------------------------------- Variables externes
extern TMemMutexLib* MemMutexLib;	// Librairie g�rant les Mutex

//------------------------------------------------------------ Fonctions annexes
void InitGame_MemLib();				// Initialise les uniques instances de management de m�moire
void CloseGame_MemLib();			// Referme proprement ces outils de communications


#endif	// MemLibH
