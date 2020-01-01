////////////////////////////////////////////////////////////////////////////////
//   UNITE : D�clarations des param�tres tentant de simuler un monde r�el     //
//           le plus r�aliste possible.                                       //
//                                                                            //
//	 Begin 26/01/99							      //								  //
////////////////////////////////////////////////////////////////////////////////
#include "WorldSimulation.h"

// Largeur en m�tres d'un carr� d'une case de la grille de la carte en vue de dessus
const double sWorldModel::SQR_LENGTH = 0.60;
// Longueur parcourue par un homme apr�s avoir fait un pas en marche
const double sWorldModel::PAS_HOMME_MARCHE = 0.85;
const double sWorldModel::PAS_HOMME_COURSE = 0.85*3;

// Valeurs utilis�es par les Conversions :
const double sWorldModel::PixPerMeter = 38.89;             // Nombre de pixels par m�tre
const double sWorldModel::MeterPerPixel = 1 / PixPerMeter; // Nomdre de m�tres par pixel

// Instance utilis� par le jeu
sWorldModel WorldModel;

