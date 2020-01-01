////////////////////////////////////////////////////////////////////////////////
//   UNITE : Déclarations des paramètres tentant de simuler un monde réel     //
//           le plus réaliste possible.                                       //
//                                                                            //
//	 Begin 26/01/99							      //								  //
////////////////////////////////////////////////////////////////////////////////
#include "WorldSimulation.h"

// Largeur en mètres d'un carré d'une case de la grille de la carte en vue de dessus
const double sWorldModel::SQR_LENGTH = 0.60;
// Longueur parcourue par un homme aprés avoir fait un pas en marche
const double sWorldModel::PAS_HOMME_MARCHE = 0.85;
const double sWorldModel::PAS_HOMME_COURSE = 0.85*3;

// Valeurs utilisées par les Conversions :
const double sWorldModel::PixPerMeter = 38.89;             // Nombre de pixels par mètre
const double sWorldModel::MeterPerPixel = 1 / PixPerMeter; // Nomdre de mètres par pixel

// Instance utilisé par le jeu
sWorldModel WorldModel;

