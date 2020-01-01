////////////////////////////////////////////////////////////////////////////////
//  File		:	ZeGame.h												  //
//                                                                            //
//	Description :   Déclarations des paramètres tentant de simuler un monde   //
//		            réel le plus réaliste possible.							  //
//  Copyright (C) 1998 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@ifrance.com		                  //
//  Version      : beta 0.10                                                  //
//  Creation     : 26/01/1999												  //
////////////////////////////////////////////////////////////////////////////////

#ifndef WorldSimulationH
#define WorldSimulationH

//=== Données modélisant les paramètres du monde réel ========================//
const struct sWorldModel
{     // Largeur en mètres d'un carré d'une case de la grille de la carte en vue de dessus
      static const double SQR_LENGTH;

      // Longueur parcourue par un homme aprés avoir fait un pas en marche
      static const double PAS_HOMME_MARCHE;
      static const double PAS_HOMME_COURSE;

      // Valeurs utilisées par les Conversions :
      static const double PixPerMeter;             // Nombre de pixels par mètre
      static const double MeterPerPixel;		   // Nomdre de mètres par pixel


      // Méthodes de convertions en tout genre
      // .....................................

	  //--- Donnees liees aux bruitages sonores ---
	  // A partir de la source sonore, a chaque fois que l'on s'eloigne de cette 
	  // distance, le volume sonore diminue	de moitie (de SOUND_DEFAULTDBDECREASE dB)
	  static const double	SOUND_DEFAULTMINDISTANCE;	
	  // Distance par defaut a partir de laquelle on considere un son comme inaudible	
	  static const double	SOUND_DEFAULTMAXDISTANCE;
	  // Perte sonore (exprime en 100ieme de dB) a chaque fois que l'on s'eloigne de la cible
	  static const int		SOUND_DEFAULTDBDECREASE;
};
extern sWorldModel WorldModel;
//----------------------------------------------------------------------------//



#endif
