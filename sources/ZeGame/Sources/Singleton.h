////////////////////////////////////////////////////////////////////////////////
//  Unité "Singleton.h" : interface et implementation de la classe generique  //
//						  Singleton (template)								  //
//                                                                            //
//	Description : Patron dont on peut faire deriver chaque classe singleton	  //
//				  devant etre un Singleton.									  //
//                                                                            //
//																			  //
//  Copyright (C) 2000 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@ifrance.com		                  //
//																			  //
//	Ref : cette classe a ete reconcue a partir de l'article de Scott Bilas	  //
//		  present dans "Game Programming Gems" (chapitre 1.3)				  //
//																			  //
//  Version      : 1.00														  //
//  Creation     : 03/09/2000 		                                          //
//  Last upgrade : <none>                                                     //
////////////////////////////////////////////////////////////////////////////////

#ifndef SingletonH
#define SingletonH

//---------------------------------------------------------------------- Include

//----------------------------------------------------------------- Define & enum

////////////////////////////////////////////////////////////////////////////////
// Classe SINGLETON															  //
template <typename S>
class Singleton
{ // --------------------------------------------------------- Attributs priivés
  protected:
	static S* ms_Singleton;			// Ptr sur l'unique instance de la classe singleton
	
	// -------------------------------------------------------- Méthodes publiques
  public:
  	//--- Constructeurs & Destructeurs ---
	Singleton();					// Constructeur par defaut
	virtual ~Singleton();			// Destructeur
	
	//--- Accesseurs sur le singleton ---
	static S& getSingleton();		// Recupere une reference sur le singleton
	static S* getSingletonPtr();	// Recupere un pointeur sur le singleton
};
//======================= Fin de la classe SINGLETON ==========================//


//--------------------------------------------------------------------- Variables 
template <typename S> S* Singleton <S>::ms_Singleton = NULL;

//---------------------------------------------------------------------- Methodes 

//=== Constructeur par defaut =================================================//
template <typename S> Singleton <S>::Singleton()
{	// Calcul de l'adresse relative de la classe derivee de Singleton qui peut
	// deriver d'autres classe (par heritage multiple) et dont le ptr this serait
	// dans ce cas different du this de Singleton :
	// calcul de la difference entre les 2 this de Singleton<S> et de S
	int offset = (int) (S*)1 - (int)(Singleton <S>*)(S*)1;
	ms_Singleton = (S*)((int)this + offset);
}
//-----------------------------------------------------------------------------//

//=== Destructeur =============================================================//
template <typename S> Singleton <S>::~Singleton()
{	ms_Singleton = NULL;
}
//-----------------------------------------------------------------------------//

//=== Recupere une reference sur le singleton =================================//
template <typename S> S& Singleton<S>::getSingleton()
{	return *ms_Singleton;
}
//-----------------------------------------------------------------------------//

//=== Recupere un pointeur sur le singleton ===================================//
template <typename S> S* Singleton<S>::getSingletonPtr()
{	return ms_Singleton;
}
//-----------------------------------------------------------------------------//


//------------------------------------------------------------- Fonctions annexes


#endif	// SingletonH

