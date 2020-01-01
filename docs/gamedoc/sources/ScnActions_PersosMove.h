////////////////////////////////////////////////////////////////////////////////
//  Unité "ScnActions_PersosMoveH.h"										  //
//                                                                            //
//	Description : Cette classe derive de la classe TAction et permet de		  //
//				  deplacer un personnage de l'endroit ou il se trouve		  //
//				  actuellement a un poit quelconque de la map.				  //
//                                                                            //
//  Copyright (C) 1999 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@ifrance.com		                  //
//  Version      : beta 0.20                                                  //
//  Creation     : 14/07/1999 		                                          //
//  Last upgrade : <none>	                                                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef ScnActions_PersosMoveH
#define ScnActions_PersosMoveH

//----------------------------------------------------------------------- Define
#ifdef CBUILDER
#define NULL	0
#endif

//------------------------------------------------------------------------- Enum
enum 
{	ACTION_PERSOSMOVE_DESTINATION_XY,
	ACTION_PERSOSMOVE_OFFSET,
	ACTION_PERSOSMOVE_DESTINATION_UV
};

//---------------------------------------------------------------------- Classes
class GameObject;		// Classe mère de toues les objets du jeu
class TScriptManager;	// Structure de suivit des scripts
class TRunningScript;	// Structure complétée par l'ExecuteAction de la ScriptLine 


// TACTION_PERSOS_MOVE /////////////////////////////////////////////////////////
// Fait avancer un personnage en											  //	
//  - soit au point (fPosX,fPosY) de la map,								  //
//	- soit d'un déplacement de (fDepX,fDepY) à partir de sa position courante //

class TAction_PersosMove : public TAction
{	int iPersoID;			// N° du perso à déplacer (0 indique que c'est celui qui a exécuté le script et -1 que c'est celui du RunningScript)
	int	iMoveType;			// Type de déplacement
	union
	{	double	fPosX;		// Coordonnée X où le Perso doit se déplacer
		int		iPosU;		//		"	  U			"				"
		double	fDepX;		// Offset de déplacement en X
	};
	union
	{	double	fPosY;		// Coordonnée Y où le Perso doit se déplacer
		int		iPosV;		//		"	  V			"				"
		double	fDepY;		// Offset de déplacement en Y
	};
	int	iRadius;			// Rayon servant à faire du random

  public:
  	TAction_PersosMove(int moveType, double posX, double posY, int persoID = 0, int radius=0);  // 1er Constructeur
	TAction_PersosMove(int moveType, int posU, int posC, int persoID = 0, int radius=0);		// 2nd Constructeur
    ~TAction_PersosMove();											// Destructeur
    // Change l'état d'animation du décors 
	void PlayAction(TRunningScript &RunningAction, GameObject *pGO = NULL);
};
///////////////////// Fin de TACTION_PERSOS_MOVE ///////////////////////////////



#endif	// ScnActions_PersosMoveH

