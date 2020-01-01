////////////////////////////////////////////////////////////////////////////////
//   UNITE : D�clarations des param�tres tentant de simuler un monde r�el     //
//           le plus r�aliste possible.                                       //
//                                                                            //
//	 Begin 26/01/99							      //								  //
////////////////////////////////////////////////////////////////////////////////
#ifndef WorldSimulationH
#define WorldSimulationH

//=== Donn�es mod�lisant les param�tres du monde r�el ========================//
const struct sWorldModel
{     // Largeur en m�tres d'un carr� d'une case de la grille de la carte en vue de dessus
      static const double SQR_LENGTH;

      // Longueur parcourue par un homme apr�s avoir fait un pas en marche
      static const double PAS_HOMME_MARCHE;
      static const double PAS_HOMME_COURSE;

      // Valeurs utilis�es par les Conversions :
      static const double PixPerMeter;             // Nombre de pixels par m�tre
      static const double MeterPerPixel; // Nomdre de m�tres par pixel


      // M�thodes de convertions en tout genre
      // .....................................
};
extern sWorldModel WorldModel;
//----------------------------------------------------------------------------//



#endif
