////////////////////////////////////////////////////////////////////////////////
//   UNITE : Déclarations des paramètres tentant de simuler un monde réel     //
//           le plus réaliste possible.                                       //
//                                                                            //
//	 Begin 26/01/99							      //								  //
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
      static const double MeterPerPixel; // Nomdre de mètres par pixel


      // Méthodes de convertions en tout genre
      // .....................................
};
extern sWorldModel WorldModel;
//----------------------------------------------------------------------------//



#endif
