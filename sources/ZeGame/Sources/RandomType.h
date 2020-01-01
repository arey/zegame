////////////////////////////////////////////////////////////////////////////////
//  Unité "RandomType.h" : interface de la classe RandomType	              //
//                                                                            //
//	Description : random class that is a sort of type						  //
//                                                                            //
//  Copyright (C) 2000 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@ifrance.com		                  //
//  Version      : beta 0.01                                                  //
//  Creation     : 06/09/2000 		                                          //
//  Last upgrade : <none>                                                     //
////////////////////////////////////////////////////////////////////////////////

#ifndef RandomTypeH
#define RandomTypeH

//---------------------------------------------------------------------- Include
#include <math.h>

//----------------------------------------------------------------- Define & enum

////////////////////////////////////////////////////////////////////////////////
// Classe RANDOMTYPE
class RandomType
{ // ---------------------------------------------------------- Attributs privés
  protected:
	int m_iMinNumber;		// Borne minimale de l'intervalle dans lequel sera tire un nombre aleatoire
	int m_iMaxNumber;		// Borne maximale de ce meme intervalle (inclus)
	int m_iInterval;		// Nombre de positions dans l'intervalle
	
	// ------------------------------------------------------ Méthodes publiques
  public:
	//--- Constructeurs & Destructeurs ---
	RandomType(int value):
		m_iMinNumber(value),m_iMaxNumber(value), m_iInterval(1) { }	
	RandomType(int min, int max):				
		m_iMinNumber(min),m_iMaxNumber(max) 
		{ m_iInterval = max-min+1; }
		virtual ~RandomType() { };						// Destructeur

	inline int random() 
		{ return (m_iInterval==0) ?  m_iMinNumber : m_iMinNumber + rand()%(m_iInterval); }
};
//====================== Fin de la classe RANDOMTYPE ==========================//


////////////////////////////////////////////////////////////////////////////////
// Classe RANDOMFLOATTYPE
class RandomFloatType
{ // --------------------------------------------------------- Attributs priivés
  protected:
	float m_fMinNumber;		// Borne minimale de l'intervalle dans lequel sera tire un nombre aleatoire
	float m_fMaxNumber;		// Borne maximale de ce meme intervalle (inclus)
	int	  m_iInterval;		// Largeur de l'intervalle
	float m_fStep;			// Longeur d'un increment
	
	// -------------------------------------------------------- Méthodes publiques
  public:
	//--- Constructeurs & Destructeurs ---
	RandomFloatType(float value):
		m_fMinNumber(value),m_fMaxNumber(value), m_iInterval(0) { }	
	RandomFloatType(float min, float max, float step):				
		m_fMinNumber(min),m_fMaxNumber(max), m_fStep(step) 
		{ m_iInterval = ((int) ((max-min)*10/step)) / 10; }	// x10/10 en attendant le round
		virtual ~RandomFloatType() { };						// Destructeur

	inline float random() 
		{ return (m_iInterval==0) ?  m_fMinNumber : m_fMinNumber + (rand()%m_iInterval)*m_fStep; }
};
//==================== Fin de la classe RANDOMFLOATTYPE ========================//




//------------------------------------------------------------- Fonctions annexes
#endif	// RandomTypeH


