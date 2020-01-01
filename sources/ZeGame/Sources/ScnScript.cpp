////////////////////////////////////////////////////////////////////////////////
//  Unit� "ScnScript.cpp" : Impl�mentation des classes ScriptLine, TScript et //
//							GameScriptList									  //										
//                                                                            //
//	Description :  ces classes permettent de lire, stocker et ex�cuter un     //
//				   script sc�naris� d'un objet, d'un d�cors ou d'un perso.	  //
//                                                                            //
//  Copyright (C) 1999 Antoine Rey    arey@insa-lyon.fr                       //
//                                    antrey@club-internet.fr                 //
//  Version      : beta 0.62                                                  //
//  Creation     : 16/07/1999 		                                          //
//  Last upgrade : 08/09/2000                                                 //
////////////////////////////////////////////////////////////////////////////////


#ifdef MICROSOFT
#pragma warning(disable : 4786 4100)  
#endif

//---------------------------------------------------------------------- Include
#ifdef MAPEDITOR
#include "Editor.h"
#endif

#pragma hdrstop
#include "ZeNain.h"         //--- Special for Visual C++ 6 MFC ---
#include "ScnActions.h"		// Actions que peut ex�cuter un script
#include "ScnCauses.h"		// Conditions n�cessaires au d�clenchement des actions 
#include "ScnEvents.h"		// Ev�nements pouvant �tre d�clench� au cours du jeu par � peu pr�s tout
#include "GameObject.h"		// Classe m�re de tous les d�cors/persos/objets du jeu
#include "INILib.h"         // Permet de charger et de travailler sur un .ini
#include "IDMap.h"			// Gestion des identificateurs du jeu
#include "Consts.h"			// Constantes du jeu
#include "GameDebug.h"		// Libraire maison de debuggage
#include "RandomType.h"		// Types aleatoires
#include "ScnActions_PersosMove.h"	// Deplacement d'un perso
#include "ScnActions_Sound.h"		// Gestiom des sons
#include "ScnScript.h"		// Son Header

//----------------------------------------------------------- Variables externes
std::IDMap *IDGamePersos = NULL;		// Map des ID des personnages joueur/pnj/streum �voluants ds le jeu
std::IDMap *IDGameDecors = NULL;		//		"	  des DecorsObjets plac�s dans les divers niveaux du jeu
GameScriptList* GameScripts = NULL;		// Librairie incorporant tous les scripts du jeu

//----------------------------------------------------------------------- define
#define MAX_LINE_CHAR			512
#define MAX_SCRIPTLINE_CHAR	   2048
#ifdef CBUILDER
#define CheminApplication	CheminEditor
#endif

////////////////////////////////////////////////////////////////////////////////
// Classe SCRIPTLINE                                                          //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur ===========================================================//
ScriptLine::ScriptLine()
{	// Defini les options par defaut : re-entrance
	iFlags = 0;
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
ScriptLine::~ScriptLine()
{
    // D�salloue toutes les actions et les causes de la ligne de script et vide le tableau
    if (!vActions.empty())
    {	for (unsigned int i=0 ; i < vActions.size() ; i++)
    	{	delete vActions[i];
        }
		vActions.clear();
    }
	if (!vCauses.empty())
    {	for (unsigned int i=0 ; i < vCauses.size() ; i++)
    	{	delete vCauses[i];
        }
		vCauses.clear();
    }
}
//----------------------------------------------------------------------------//

//=== Facteur d�clencheur d'une ligne de script ==============================//
int ScriptLine::GetTrigger()
{	return iTrigger;
}
//----------------------------------------------------------------------------//

//=== Ajoute le facteur d�clencheur d'une ligne de script ====================//
void ScriptLine::SetTrigger(int trig)
{	iTrigger = trig;
}
//----------------------------------------------------------------------------//

//=== Ajoute une action � la liste d'apr�s son type ==========================//
bool ScriptLine::AddAction(int typeAction, ...)
{	va_list  pParam;					// Pointeur sur les param�tres
	va_start(pParam, typeAction);		// adresse du 1er param�tre optionnel
	int numParam = va_arg(pParam, int);	// Nb de param�tres transmis
	//numParam = numParam;
	TAction *action = NULL;
	char* paramString;
	AnsiString sParam1, sParam2;
	int etatID, iParam1, iParam2, iParam3;
	double fParam1, fParam2;
	float fParam3;
	bool bParam1;
	bool endSynchro;
	
	// Suivant le type de cause, on alloue une instance d�riv�e de la classe TCause
	switch (typeAction)
	{	
		// Ajout d'un Memory au GameObject
		case (ACTION_TYPE_GO_NEWMEMORY) :
			paramString = va_arg(pParam, char*);  // N�ID du Memory
			sParam1 = paramString;
			iParam1 = IDMemory->NumID(sParam1);
			if (numParam == 2)
			{	paramString = va_arg(pParam, char*);  // N�ID du Perso
				sParam1 = paramString;
				iParam2 = IDGamePersos->NumID(sParam1);
				action = new TAction_GONewMemory(iParam1,iParam2);
			} else
			{	action = new TAction_GONewMemory(iParam1, 0);
			}
			break;
		// Supprime un Memory au GameObject
		case (ACTION_TYPE_GO_DELMEMORY) :
			paramString = va_arg(pParam, char*);  // N�ID du Memory
			sParam1 = paramString;
			iParam1 = IDMemory->NumID(sParam1);
			if (numParam == 2)
			{	paramString = va_arg(pParam, char*);  // N�ID du Perso
				sParam1 = paramString;
				iParam2 = IDGamePersos->NumID(sParam1);
				action = new TAction_GODelMemory(iParam1,iParam2);
			} else
			{	action = new TAction_GODelMemory(iParam1, 0);
			}
			break;
		// Associe un script sc�naritique � un GameObject
		case (ACTION_TYPE_GO_SETSCRIPT) :
			paramString = va_arg(pParam, char*);  // N�ID du Script
			sParam1 = paramString;
			iParam1 = GameScripts->GetScriptMap()->NumID(sParam1);
			
			// Si il y'a un 2i�me param�tre, c'est qu'on nous a fournit 
			// soit le n�ID du GO qui doit accepter ce script, soit un pr�fixe
			iParam2 = 0;	// par d�faut, c'est le GO poss�dant ce script qui se voit associer un autre script
			if (numParam == 2)
			{	paramString = va_arg(pParam, char*);  
				// Prefix indiquant qu'il faudra prendre le n�ID du GO pr�c�dent
				if (strcmp(paramString, "perso") == 0)
				{	iParam2 = -1;	 
				} else
				{	// N�ID du GO qui se verra assigner le scipt
					sParam2 = paramString;
					if (IDGamePersos->IsCle(sParam2))
					{	iParam2 = IDGamePersos->NumID(sParam2);
					} else
					if (IDGameDecors->IsCle(sParam2))
					{	iParam2 = IDGameDecors->NumID(sParam2);
					}
				}				
			}

			action = new TAction_GOSetScript(iParam1, iParam2);
			break;
		// Fait appara�tre/dispara�tre le GameObject de l'�cran
		case (ACTION_TYPE_GO_VISIBLE) :
			paramString = va_arg(pParam, char*);  // GO Visible ou non
			if (  (strcmp(paramString, "true")==0) || (strcmp(paramString, "TRUE")==0)
				|| (strcmp(paramString, "True")==0) )
			{	bParam1 = true;		
			} else
			{	bParam1 = false;
			}
			
			// Si il y'a un 2i�me param�tre, c'est qu'on nous a fournit 
			// soit le n�ID du GO qui doit accepter ce script, soit un pr�fixe
			iParam1 = 0;	// par d�faut, c'est le GO poss�dant ce script qui se voit associer un autre script
			if (numParam == 2)
			{	paramString = va_arg(pParam, char*);  
				// Prefix indiquant qu'il faudra prendre le n�ID du GO pr�c�dent
				if (strcmp(paramString, "perso") == 0)
				{	iParam1 = -1;	 
				} else
				{	// N�ID du GO qui se verra assigner le scipt
					sParam1 = paramString;
					if (IDGamePersos->IsCle(sParam1))
					{	iParam2 = IDGamePersos->NumID(sParam1);
					} else
					if (IDGameDecors->IsCle(sParam1))
					{	iParam1 = IDGameDecors->NumID(sParam1);
					}
				}				
			}
			action = new TAction_GOSetVisible(bParam1, iParam1);
			break;


		// Cr�ation d'un personnage non joueur
		case (ACTION_TYPE_PERSO_CREATE):
			paramString = va_arg(pParam, char*);	// Type de personnages : OGRE, NAIN ...
			sParam1 = paramString;
			iParam1 = IDPatronPersos->NumID(sParam1);
			switch (numParam)
			{	case 1:
					// Le PNJ appara�trat � l'endroit o� se trouvait l'objet qui a d�clench� ce script
					action = new TAction_PersosCreate(iParam1);
					break;
				case 3:
					// R�cup�re les coordonn�es o� l'on devra placer le personnage
					paramString = va_arg(pParam, char*);	
					fParam1 = atof(paramString);
					paramString = va_arg(pParam, char*);	
					fParam2 = atof(paramString);
					action = new TAction_PersosCreate(iParam1, fParam1, fParam2, TAction_PersosCreate::PERSOCREATE_ABSOLUE);
					break;
				case 4:
					// Le PNJ appara�trat � l'endroit o� se trouvait l'objet qui a d�clench� ce script
					// � plus ou moins pr�s un petit d�placement
					// Rmq : le 4i�me param ne sert pas pour l'instant -> juste pour diff�rencier cette Action avec la n�3
					paramString = va_arg(pParam, char*);	
					fParam1 = atof(paramString);
					paramString = va_arg(pParam, char*);	
					fParam2 = atof(paramString);
					// 4i�me param�tre bidon
					paramString = va_arg(pParam, char*);
					action = new TAction_PersosCreate(iParam1, fParam1, fParam2, TAction_PersosCreate::PERSOCREATE_OFFSET);
					break;
				case 5:
					// Le PNJ appara�trat � l'endroit o� se trouvait l'objet qui a d�clench� ce script
					// � plus ou moins pr�s un petit d�placement
					// Rmq : le 4i�me param ne sert pas pour l'instant -> juste pour diff�rencier cette Action avec la n�3
					// Le perso est important et a son propre ID
					paramString = va_arg(pParam, char*);	
					fParam1 = atof(paramString);
					paramString = va_arg(pParam, char*);	
					fParam2 = atof(paramString);
					// 4i�me param�tre bidon
					paramString = va_arg(pParam, char*);
					if (strcmp(paramString, "offset")==0)
					{	iParam2 = TAction_PersosCreate::PERSOCREATE_OFFSET;
					} else
					{	iParam2 = TAction_PersosCreate::PERSOCREATE_ABSOLUE;
					}
					// N�ID du perso
					paramString = va_arg(pParam, char*);
					sParam1 = paramString;
					iParam3 = IDGamePersos->NumID(sParam1);
					action = new TAction_PersosCreate(iParam1, fParam1, fParam2, iParam2, iParam3);
					break;
			}
			break;
		// Supression d'un personnage
		case (ACTION_TYPE_PERSO_DELETE) :
			paramString = va_arg(pParam, char*);  // N�ID du Memory
			if (strcmp(paramString, "himself") == 0)
			{	iParam1 = 0;
			} else
			{	sParam1 = paramString;
				iParam1 = IDGamePersos->NumID(sParam1);
			}
			action = new TAction_PersosDelete(iParam1);
			break;

		// D�placement d'un personnage en un point (X,Y) ou d'une distance (X,Y)
		case (ACTION_TYPE_PERSO_MOVETO):
		case (ACTION_TYPE_PERSO_MOVEFROM):
			paramString = va_arg(pParam, char*);  // D�placement X
			fParam1 = atof(paramString);
			paramString = va_arg(pParam, char*);  // D�placement Y
			fParam2 = atof(paramString);

			// Si il y'a un 4i�me param�tre, c'est qu'il y'a un random dans le tas
			iParam2 = 0;
			if (numParam == 4)
			{	paramString = va_arg(pParam, char*);
				iParam2 = atoi(paramString);
			}
			
			// Par d�faut, c'est le perso poss�dant ce script qui doit se d�placer
			iParam1 = 0;
			// Si il y'a un 3i�me param�tre, c'est qu'on nous a fournit 
			// soit le n�ID du perso devant se d�placer, soit un pr�fixe
			if (numParam >= 3)
			{	paramString = va_arg(pParam, char*);  
				// Prefix indiquant qu'il faudra prendre le n�ID du perso pr�c�dent
				if (strcmp(paramString, "perso") == 0)
				{	iParam1 = -1;	 
				} else
				{	// N�ID du perso � d�placer 
					sParam1 = paramString;
					iParam1 = IDGamePersos->NumID(sParam1);
				}				
			}
			
			switch (typeAction)
			{	case (ACTION_TYPE_PERSO_MOVETO):
					action = new TAction_PersosMove(ACTION_PERSOSMOVE_DESTINATION_XY, fParam1, fParam2, iParam1, iParam2);
					break;
				case (ACTION_TYPE_PERSO_MOVEFROM):
					action = new TAction_PersosMove(ACTION_PERSOSMOVE_OFFSET, fParam1, fParam2, iParam1);
					break;
			}			
			break;

		// Joue une animation superpos�e � l'objet du d�cors
		case (ACTION_TYPE_DECORS_PLAYSUPANIM) :
			paramString = va_arg(pParam, char*);  // N�ID de l'anim superpos�e
			sParam1 = paramString;
			iParam1 = IDAnimation->NumID(sParam1);
			action = new TAction_DecorsPlaySupAnim(0, iParam1);
			break;
		
		// Ajoute un �tat touch� par un perso/objet
		// [ACF] il manque 1 param�tre et c'est pas vraiment au point cette histoire d'anim
		case (ACTION_TYPE_DECORS_FORCENEWSTATE):
		case (ACTION_TYPE_DECORS_NEWSTATE):
			if (numParam == 1)
			{	// Etat du d�cors poss�dant ce script
				paramString = va_arg(pParam, char*);
				sParam1 = paramString;
				etatID = IDAnimation->NumID(sParam1);
				if (typeAction == ACTION_TYPE_DECORS_NEWSTATE)
				{	action = new TAction_DecorsNewState(-1, etatID);
				} else
				{	action = new TAction_DecorsForceNewState(-1, etatID);
				}
			} else	// if (numParam == 2)
			{	// Etat d'un autre GameObject
				// - N� de ce GameObject
				paramString = va_arg(pParam, char*);
				sParam1 = paramString;
				iParam2 = IDGamePersos->NumID(sParam1);
				// - N� de l'anim
				paramString = va_arg(pParam, char*);
				sParam1 = paramString;
				etatID = IDAnimation->NumID(sParam1);
				if (typeAction == ACTION_TYPE_DECORS_NEWSTATE)
				{	action = new TAction_DecorsNewState(iParam2, etatID);
				} else
				{	action = new TAction_DecorsForceNewState(iParam2, etatID);
				}
			}
			break;

		// Ajout d'un Memory au Niveau
		case (ACTION_TYPE_SCENARIO_NEWLEVELMEMORY) :
			paramString = va_arg(pParam, char*);  // N�ID du Memory
			sParam1 = paramString;
			iParam1 = IDMemory->NumID(sParam1);
			action = new TAction_ScenarioNewLevelMemory(iParam1);
			break;
		// Supprime une Memoire au Level
		case (ACTION_TYPE_SCENARIO_DELLEVELMEMORY) :
			paramString = va_arg(pParam, char*);  // N�ID du Memory
			sParam1 = paramString;
			iParam1 = IDMemory->NumID(sParam1);
			action = new TAction_ScenarioDelLevelMemory(iParam1);
			break;
		// --- Son ---
		case (ACTION_TYPE_SOUND_PLAY) :
			// 1er param obligatoire : ID du bruitage
			paramString = va_arg(pParam, char*);
			sParam1 = paramString;
			iParam1 = IDSoundEffect->NumID(sParam1);
			switch (numParam)
			{	case 1 : 
					action = new TAction_SoundPlay(iParam1);
					break;
				case 2 :
					// 2nd parametre (optionnel) : volume sonore en dB
					paramString = va_arg(pParam, char*);
					// Peut etre un nombre aleatoire
					if (isRandomParameter(paramString))
					{	RandomFloatType* rft = getRandomFloatType(paramString);
						if (rft==NULL)
						{	TRACEDBG("ScnScript.cpp - AddScriptLine failed at ACTION_TYPE_SOUND_PLAY : %s\n",paramString);
							return false;
						}
						action = new TAction_SoundPlay(iParam1, rft);
						} else
					{	fParam3 = (float) atof(paramString);
						action = new TAction_SoundPlay(iParam1, fParam3);
					}
					break;
			}
			break;

		// --- Synchro ---
		// Attends N ms avant de reprendre l'ex�cution des action
		case (ACTION_TYPE_SYNCHRO_SLEEP) :
			paramString = va_arg(pParam, char*);
			// Peut etre un nombre aleatoire
			if (isRandomParameter(paramString))
			{	RandomType* rt = getRandomType(paramString);
				if (rt==NULL)
				{	TRACEDBG("ScnScript.cpp - AddScriptLine failed at ACTION_TYPE_SYNCHRO_SLEEP : %s\n",paramString);
					return false;
				}
				action = new TAction_Sleep(rt);
			} else
			{	iParam1 = atoi(paramString);
				action = new TAction_Sleep(iParam1);
			}
			break;
		
		// Se synchronise sur la fin de l'action n�iParam1
		case (ACTION_TYPE_SYNCHRO_WAITEND) :
			paramString = va_arg(pParam, char*);
			iParam1 = atoi(paramString);
			action = new TAction_WaitEnd(iParam1);
			break;
		
		// Condition non valide
		default:
			return false;	
	}
	
	// V�rifie si l'on n'aura pas besoin de se synchroniser sur la fin de cette action par la suite
	endSynchro = va_arg(pParam, bool);
	action->SetEndSynchro(endSynchro);
	
	// On ajoute cette action � la suite de la liste
	vActions.push_back(action);

	va_end(pParam);		// Fin d'�valuation : NULL ds pParam
	return true;
}
//----------------------------------------------------------------------------//

//=== Ajoute une cause � la liste d'apr�s son type ===========================//
bool ScriptLine::AddCause(int typeCause, ...)
{	va_list  pParam;                // Pointeur sur les param�tres
	va_start(pParam, typeCause);    // adresse du 1er param�tre optionnel
	TCause *cause = NULL;
	char* paramString;
	
	int thingID, iParam1, iParam2;
	AnsiString sParam1, sParam2;

	
	// Suivant le type de cause, on alloue une instance d�riv�e de la classe TCause
	switch (typeCause)
	{	
		// Objet/d�cors touch� par un perso/objet
		case (CAUSE_TYPE_THING_STRIKE_BY) :
			paramString = va_arg(pParam, char*);	
			// ID du Objet/Perso/D�cors originaire du coup
			thingID = 0; //??? paramString ????
			cause = new TCause_StrikeBy(thingID);
			break;

		// Objet/Perso/D�cors a �t� frapp� avec tel arme/objet
		case (CAUSE_TYPE_THING_STRIKE_WITH) :
			paramString = va_arg(pParam, char*);	
			// ID de l'Arme/Objet avec lequel on a frapp�
			thingID = 0; //??? paramString ????
			cause = new TCause_StrikeWith(thingID);
			break;	

		// Objet/Perso/D�cors connait tel chose
		case (CAUSE_TYPE_THING_MEMORY_IN) :
			paramString = va_arg(pParam, char*);
			sParam1 = paramString;
			iParam1 = IDMemory->NumID(sParam1);	// ID de la M�moire
			cause = new TCause_MemoryIn(iParam1);
			break;	
		// Objet/Perso/D�cors ne connait pas tel chose
		case (CAUSE_TYPE_THING_MEMORY_NO) :
			paramString = va_arg(pParam, char*);
			sParam1 = paramString;
			iParam1 = IDMemory->NumID(sParam1);	// ID de la M�moire � ne pas avoir
			cause = new TCause_MemoryNo(iParam1);
			break;	
		
		// ================= Sc�nario =================
		// Level connait tel chose
		case (CAUSE_TYPE_LEVEL_MEMORY_IN) :
			paramString = va_arg(pParam, char*);
			sParam1 = paramString;
			iParam1 = IDMemory->NumID(sParam1);	// ID de la M�moire
			cause = new TCause_LevelMemoryIn(iParam1);
			break;	
		// Level ne connait pas tel chose
		case (CAUSE_TYPE_LEVEL_MEMORY_NO) :
			paramString = va_arg(pParam, char*);
			sParam1 = paramString;
			iParam1 = IDMemory->NumID(sParam1);	// ID de la M�moire � ne pas avoir
			cause = new TCause_LevelMemoryNo(iParam1);
			break;
		
		// ================ Personnages ================
		// Personnage est � l'int�rieur ou � l'ext�rieur de telle zone
		case (CAUSE_TYPE_PERSO_ZONE_OUT):
		case (CAUSE_TYPE_PERSO_ZONE_IN) :
			paramString = va_arg(pParam, char*);
			sParam1 = paramString;
			iParam1 = IDGamePersos->NumID(sParam1);	// N� du Personnage
			paramString = va_arg(pParam, char*);	
			sParam2 = paramString;
			iParam2 = IDMapZones->NumID(sParam2);	// N� de la Zone
			if (typeCause==CAUSE_TYPE_PERSO_ZONE_IN) 
			{	cause = new TCause_CharactZoneIn(iParam1, iParam2);
			} else
			{	cause = new TCause_CharactZoneOut(iParam1, iParam2);
			}
			break;
		// Hero � l'int�rieur ou � l'ext�rieur de telle zone
		case (CAUSE_TYPE_HERO_ZONE_OUT):
		case (CAUSE_TYPE_HERO_ZONE_IN) :
			paramString = va_arg(pParam, char*);	
			sParam1 = paramString;
			iParam1 = IDMapZones->NumID(sParam1);	// N� de la Zone
			if (typeCause==CAUSE_TYPE_HERO_ZONE_IN) 
			{	cause = new TCause_HeroZoneIn(iParam1);
			} else
			{	cause = new TCause_HeroZoneOut(iParam1);
			}
			break;

		// ================== Synchro ==================
		// Tire un nombre al�atoire compris entre 0 et (iParam1 - 1)
		case (CAUSE_TYPE_RANDOM) :
			paramString = va_arg(pParam, char*);
			iParam1 = atoi(paramString);			
			cause = new TCause_Random(iParam1);
			break;
		
		// Attends iParam1 ms avant de d�clencer l'�v�nement
		case (CAUSE_TYPE_TIMER) :
			paramString = va_arg(pParam, char*);
			iParam1 = atoi(paramString);			
			cause = new TCause_Timer(iParam1);
			break;
		
		// Condition non valide
		default:
			return false;	
	}
	
	// On ajoute cette condition � la suite de la liste
	vCauses.push_back(cause);

	va_end(pParam);		// Fin d'�valuation : NULL ds pParam
	return true;
}
//----------------------------------------------------------------------------//


//=== Compile une cha�ne de car. en une liste d'actions encod�es =============//
int ScriptLine::EncodeActions(char *actions)
{	int cur = 0;
	int maxLong = strlen(actions);
	char car;
	int i = 0;
	bool findAction = false;
	static char keyword[MAX_LENGTH_KEYWORD];
	int ActionType = ACTION_TYPE_NONE; 
	static char parameter[MAX_SCRIPT_FUNCTION_PARAMETER][MAX_LENGTH_KEYWORD];
	int numParam = 0;
	int nbActions = 1;
	bool endSynchro;
	int numPrefix = 0;
	
	// Tant que l'on n'est pas arriv� � la fin du script
	while (cur <= maxLong)
	{	// Saute tous les caract�res non interpr�tables
		while ((isInvalidChar(car = actions[cur])) && (car!='\0')) cur++;
		// V�rifie si l'on n'est pas arriv� � la fin de la cha�ne "actions"
		if (cur >= maxLong)
		{	if (!findAction) return SCRIPT_ERROR_NOTHING;
			else			return SCRIPT_NOERROR;
		}

		// R�cup�re un mot-clef en s'arr�tant lorsque l'on trouve une parenth�se, un '.' ou un blanc
		i = 0;
		do
		{	car = keyword[i++] = actions[cur++];
		}
		while ((!isInvalidChar(car)) /*&& (car!='.')*/ && (car!='(') && (car!='&') && (car!='\0') && (i<MAX_LENGTH_KEYWORD));
		keyword[i-1] = '\0';

		// V�rifie si il n'y a pas un pr�fixe devant le mot-cl� et l'enl�ve au cas o�
		numPrefix = GetActionPrefix(keyword);
		
		// V�rifie si c'est un mot-cl� et r�cup�re le SCRIPT_ACTION_ associ�
		ActionType = GetActionType(keyword);
		if (ActionType == -1) return SCRIPT_ERROR_NOTANACTION;

		// Si c'est une parenth�se, on r�cup�re le ou les param�tres de cette fonction
		if (car == '(')
		{	findAction = true;
			numParam = getParameters(actions, parameter, cur); 

			// V�rifie si il n'y aura pas besoin de se synchroniser par la suite
			endSynchro =  (actions[cur+1] == '!');
			if (endSynchro) cur++;

			// Si l'on a trouv� un pr�fix, doit donc rajouter un param�tre
			if (numPrefix != ACTION_PREFIXE_NONE)
			{	strcpy(parameter[numParam++], GetActionPrefixName(numPrefix));
			}
		
			// On ajoute une condition � la liste
			// -> pour l'instant on peut passer de 0 � 4 param�tres
			switch (numParam)
			{	case  0 : AddAction(ActionType, 0, endSynchro); break;
				case  1 : AddAction(ActionType, numParam, parameter[0], endSynchro); break;
				case  2 : AddAction(ActionType, numParam, parameter[0], parameter[1], endSynchro); break;
				case  3 : AddAction(ActionType, numParam, parameter[0], parameter[1], parameter[2], endSynchro); break;
				case  4 : AddAction(ActionType, numParam, parameter[0], parameter[1], parameter[2], parameter[3], endSynchro); break;
				case  5 : AddAction(ActionType, numParam, parameter[0], parameter[1], parameter[2], parameter[3], parameter[4], endSynchro); break;
				case  6 : AddAction(ActionType, numParam, parameter[0], parameter[1], parameter[2], parameter[3], parameter[4], parameter[5], endSynchro); break;
				default : AddAction(ActionType, -1); break;
			}

			cur++;		// on saute le car. ')'
		}

		// Saute tous les caract�res non interpr�tables
		while ((isInvalidChar(car = actions[cur])) && (car!='\0')) cur++;
		if ((cur >= maxLong) && (!findAction)) return SCRIPT_ERROR_NOTHING;

		// Nouvelle action trouv�e
		if (car == '&')
		{	nbActions++;
			cur++;		// on saute le car. '&'
		}
		
	}
	
	return SCRIPT_NOERROR;
}
//----------------------------------------------------------------------------//

//=== Compile une cha�ne de car. en des conditions � r�unir ==================//
int ScriptLine::EncodeCauses(char *causes)
{	int cur = 0;
	int maxLong = strlen(causes);
	char car;
	int i = 0;
	bool findCause = false;
	static char keyword[MAX_LENGTH_KEYWORD];
	int CauseType = CAUSE_TYPE_NONE; 
	static char parameter[MAX_SCRIPT_FUNCTION_PARAMETER][MAX_LENGTH_KEYWORD];
	int numParam = 0;
	int nbCauses = 1;

	// Tant que l'on n'est pas arriv� � la fin du script
	while (cur <= maxLong)
	{
		// Saute tous les caract�res non interpr�tables
		while ((isInvalidChar(car = causes[cur])) && (car!='\0')) cur++;
		// V�rifie si l'on n'est pas arriv� � la fin de la cha�ne "causes"
		if (cur >= maxLong)
		{	if (!findCause) return SCRIPT_ERROR_NOTHING;
			else			return SCRIPT_NOERROR;
		}

		// R�cup�re un mot-clef en s'arr�tant lorsque l'on trouve une parenth�se, un '.' ou un blanc
		i = 0;
		do
		{	car = keyword[i++] = causes[cur++];
		}
		while ((!isInvalidChar(car)) && (car!='.') && (car!='(') && (car!='&') && (car!='\0') && (i<MAX_LENGTH_KEYWORD));
		keyword[i-1] = '\0';

		// V�rifie si c'est un mot-cl� et r�cup�re le SCRIPT_CAUSE_ associ�
		CauseType = GetCauseType(keyword);
		if (CauseType == -1) return SCRIPT_ERROR_NOTACAUSE;

		// Si c'est une parenth�se, on r�cup�re le ou les param�tres de cette fonction
		if (car == '(')
		{	findCause = true;
			numParam = i = 0;
			while 	((car = causes[cur]) != ')')
			{	// chgt de param�tre
				if (car==',')	
				{	parameter[numParam][i] = '\0';
					numParam++;		
					i = 0;
					cur++;
				}
				// car. du param�tre
				if ((!isInvalidChar(car)) && (car!=','))
				{	parameter[numParam][i++] = car;
				}

				cur++;		// car. suivant
			}
			parameter[numParam++][i] = '\0';

			// On ajoute une condition � la liste
			// -> pour l'instant on peut passer de 0 � 4 param�tres
			switch (numParam)
			{	case  0 : AddCause(CauseType); break;
				case  1 : AddCause(CauseType, parameter[0]); break;
				case  2 : AddCause(CauseType, parameter[0], parameter[1]); break;
				case  3 : AddCause(CauseType, parameter[0], parameter[1], parameter[2]); break;
				case  4 : AddCause(CauseType, parameter[0], parameter[1], parameter[2], parameter[3]); break;
				default :  AddCause(CauseType); break;
			}

			cur++;		// on saute le car. ')'
		}

		// Saute tous les caract�res non interpr�tables
		while ((isInvalidChar(car = causes[cur])) && (car!='\0')) cur++;
		if ((cur >= maxLong) && (!findCause)) return SCRIPT_ERROR_NOTHING;

		// Nouvelle condition trouv�e
		if (car == '&')
		{	nbCauses++;
			cur++;		// on saute le car. '&'
		}

	}
	
	return SCRIPT_NOERROR;
}
//----------------------------------------------------------------------------//

//=== Renvoie vrai si c'est un espace, une tab, un car. de fin de ligne ... ==//
inline bool ScriptLine::isInvalidChar(char car)
{	return ((car ==' ') || (car=='\n') || (car=='\t') || (car=='0x0D'));
}
//----------------------------------------------------------------------------//

//=== Vrai si toutes les conditions sont v�rifi�es ===========================//
bool ScriptLine::isAllCausesRealized(GameObject* pGO, TEvent* pEvent)
{	
	USE_PARAM(pGO);
	USE_PARAM(pEvent);
	
	for (int i=0 ; i < vCauses.size(); i++)
	{	if (!vCauses[i]->isCauseRealized(pGO->GeScriptManager())) return false;
	}
	return true;
}
//----------------------------------------------------------------------------//


//=== Lance telle action (n�iPosition) et r�cup�re les valeurs d'attentes ====//
void ScriptLine::ExecuteAction(int iPos, TRunningScript &RunningAction, GameObject* pGO)
{	vActions[iPos]->PlayAction(RunningAction, pGO);
}
//----------------------------------------------------------------------------//

//=== Get parameters of a function ===========================================//
//	@param	str			string parameters to parse (input)
//	@param	curseur		index where to begin (input/output)
//	@param	parameters	parameters (output)
//	@return	the number of parameters
int ScriptLine::getParameters(char *str, char parameter[][MAX_LENGTH_KEYWORD], int &curseur)
{	int numParam=0, i=0;
	char car;
	while 	((car = str[curseur]) != ')')
	{	// chgt de param�tre
		if (car==',')	
		{	parameter[numParam][i] = '\0';
			numParam++;		
			i = 0;
			//cur++;
		}
		// car. du param�tre
		if ((!isInvalidChar(car)) && (car!=',')) parameter[numParam][i++] = car;

		curseur++;		// car. suivant
	}
	parameter[numParam++][i] = '\0';
	return numParam;
}
//----------------------------------------------------------------------------//

//=== Indique si c'est un nombre aleatoire ===================================//
bool ScriptLine::isRandomParameter(char* param)
{	int cur=0;
	static char temp[16];
	// Saute tous les espaces et les caracteres de tabulation
	while ((param[cur]==' ') || (param[cur]=='\t')) cur++;

	while ((param[cur]!='\0') && (cur<16))
	{	temp[cur] = param[cur];
		if (temp[cur]=='[')
		{	temp[cur]='\0';
			if ((strcmp(temp, "rand")==0) || (strcmp(temp, "random")==0))
			{	return true;
			}
		}
		cur ++;		
	}
	return false;
}
//----------------------------------------------------------------------------//

//=== Renvoi le nombre aleatoire construit a partir de nombres entiers ======//
RandomType*	ScriptLine::getRandomType(char* param)
{	int cur=0, i=0;
	int min, max;
	static char temp[16];
	// On se positionne au debut de l'intervalle
	while ((param[cur]!='[') && (param[cur]!='\0')) cur++;
	if (param[cur]=='\0') return NULL;
	cur++;
	
	// On recupere les 2 bornes de l'intervalle du random
	while ((param[cur]!=';') && (param[cur]!='\0') && (i<16))
	{	temp[i++] = param[cur++];
	}
	if ((param[cur]=='\0') || (i>=16)) return NULL;
	temp[i]='\0';
	min = atoi(temp);
	i=0;
	cur++;
	while ((param[cur]!=')') && (param[cur]!='\0') && (i<16))
	{	temp[i++] = param[cur++];
	}
	if (i>=16) return NULL;
	temp[i]='\0';
	max = atoi(temp);
	return new RandomType(min, max);
}
//----------------------------------------------------------------------------//

//=== Renvoi le nombre aleatoire construit a partir de nombres reels =========//
RandomFloatType* ScriptLine::getRandomFloatType(char* param)
{	int cur=0, i=0;
	float min, max, step;
	static char temp[16];
	// On se positionne au debut de l'intervalle
	while ((param[cur]!='[') && (param[cur]!='\0')) cur++;
	if (param[cur]=='\0') return NULL;
	cur++;
	
	// On recupere les 2 bornes de l'intervalle du random
	while ((param[cur]!=';') && (param[cur]!='\0') && (i<16))
	{	temp[i++] = param[cur++];
	}
	if ((param[cur]=='\0') || (i>=16)) return NULL;
	temp[i]='\0';
	min = (float) atof(temp);
	i=0;
	cur++;
	while ((param[cur]!=';') && (param[cur]!='\0') && (i<16))
	{	temp[i++] = param[cur++];
	}
	if ((param[cur]=='\0') || (i>=16)) return NULL;
	temp[i]='\0';
	max = (float) atof(temp);

	// et recupere le pas
	i=0;
	cur++;
	while ((param[cur]!=')') && (param[cur]!='\0') && (i<16))
	{	temp[i++] = param[cur++];
	}
	if (i>=16) return NULL;
	temp[i]='\0';
	step = (float) atof(temp);
	return new RandomFloatType(min, max, step);
}
//----------------------------------------------------------------------------//

//=== Indique si meme ligne de script peut etre executee plusieurs fois en ===//
// meme temps par un meme objet
inline bool ScriptLine::isReentrance()
{	return ((iFlags & SCRIPTLINE_FLAG_NO_REENTRANCE) != SCRIPTLINE_FLAG_NO_REENTRANCE);
}
//----------------------------------------------------------------------------//

//=== Rajoute une option =====================================================//
inline void ScriptLine::setFlag(int flag)
{	iFlags |= flag;
}
//----------------------------------------------------------------------------//

//=== Parse les options d'une ligne de script ================================//
int ScriptLine::setOptions(char* options)
{	int cur=0, i, optionType;
	char car;
	static char keyword[MAX_LENGTH_KEYWORD];
	bool findOption = false;

	// Tant que l'on n'est pas arriv� � la fin des options
	while (options[cur]!='\0')
	{	car = options[cur];
		// Saute tous les caract�res non interpr�tables
		while ((isInvalidChar(car)) && (car!='\0')) cur++;
		// V�rifie si l'on n'est pas arriv� � la fin des options
		if ((car=='\0') && (!findOption)) return SCRIPT_ERROR_NOTHING;
		
		// R�cup�re une option en s'arr�tant lorsque l'on trouve un  ou un blanc ou un car. de fin de ligne
		i = 0;
		do
		{	car = keyword[i++] = options[cur++];
		}
		while ((!isInvalidChar(car)) && (car!='|') && (car!='\0') && (i<MAX_LENGTH_KEYWORD));
		keyword[i-1] = '\0';

		// V�rifie si c'est bien un mot-cl� et r�cup�re le SCRIPTLINE_OPTION_ associ�
		optionType = GetScriptLineOption(keyword);
		if (optionType == -1) return SCRIPT_ERROR_NOTANOPTION;
		findOption = true;
		switch (optionType)
		{	case SCRIPTLINE_OPTION_NO_REENTRANCE :
				setFlag(SCRIPTLINE_FLAG_NO_REENTRANCE);
				break;
		}

		// Saute tous les caract�res non interpr�tables
		while ((isInvalidChar(car = options[cur])) && (car!='\0')) cur++;
		if ((car=='\0') && (!findOption)) return SCRIPT_ERROR_NOTHING;

		// Nouvelle optiom trouv�e
		if (car == '|')
		{	cur++;		// on saute le car. '|'
		}
	}	
	return SCRIPT_NOERROR;	
}
//----------------------------------------------------------------------------//

//////////////////////////// Fin de SCRIPTLINE /////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Classe TRUNNINGACTION							                          //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur  ==========================================================//
TRunningScript::TRunningScript(int numTrig, int numLine, int pos, int finalTimer, int nbact):
	iNumTrigger(numTrig),iNumScriptLine(numLine),iPosition(pos),iFinalTimer(finalTimer),
	iNbActions(nbact), bWaiting(false), iLastGoID(-1), iActionMutexIndex(0)
{	if (iNbActions != 0)
	{	iMutexID = new int[iNbActions];
		iActionMutex = new int[iNbActions];
	} else
	{	iMutexID = NULL;
		iActionMutex = NULL;
	}
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TRunningScript::~TRunningScript()
{	delete iMutexID;
	delete iActionMutex;
}
//----------------------------------------------------------------------------//

////////////////////////// Fin de TRUNNINGACTION ///////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Classe TSCRIPTMANAGER							                          //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur par d�faut ================================================//
TScriptManager::TScriptManager()
{
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TScriptManager::~TScriptManager()
{	// Vide la map des Timers
	mTimer.clear();
}
//----------------------------------------------------------------------------//

//=== Renvoie vrai si il a d�j� un �l�ment associ� ===========================//
bool TScriptManager::IsTimer(int iID)
{	std::map<int, int>::iterator t = mTimer.find(iID);
    return (t != mTimer.end());		// vrai si la clef a �t� trouv�e
}
//----------------------------------------------------------------------------//

//=== Renvoie la valeur tu timer associ� au script n�iID =====================//
int TScriptManager::GetTimer(int iID)
{	return mTimer[iID];
}
//----------------------------------------------------------------------------//

//=== Associe un n� de script � un timer =====================================//
void TScriptManager::SetNewTimer(int iID, int iTimer)
{	mTimer.insert(std::make_pair(iID, iTimer));
}
//----------------------------------------------------------------------------//

//=== MaJ de la valeur du futur timer ========================================//
void TScriptManager::SetTimer(int iID, int iTimer)
{	mTimer[iID] = iTimer;
}
//----------------------------------------------------------------------------//

//=== Ajout d'un Script � la liste des scripts en cours d'ex�cution ==========//
void TScriptManager::AddRunningActionsScript(int iNumTrigger, int iNumScriptLine, int iNbActions)
{	TRunningScript *running = new TRunningScript(iNumTrigger, iNumScriptLine, 0, -1, iNbActions);
	vRunningScripts.push_back(running); 								   	
}
//----------------------------------------------------------------------------//

//=== Indique si cette ScripLine est en cours d'execution ====================//
bool TScriptManager::isScriptLineRunning(int line)
{	if (!isScriptRunning()) return false;
	std::list<TRunningScript*>::iterator iter = vRunningScripts.begin(); 
	for (; iter != vRunningScripts.end() ; ++iter)
	{	if ((*iter)->iNumScriptLine == line) return true;		
	}
	return false;
}
//----------------------------------------------------------------------------//


////////////////////////////// Fin de TSCRIPTMANAGER ///////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Classe TSCRIPT									                          //
////////////////////////////////////////////////////////////////////////////////

//=== Destructeur ============================================================//
TScript::TScript(int zgsID)
{	iScriptID = zgsID;
	iScriptType = 0;
	Name[0] = '\0';
	// Defini les options par defaut : re-entrance
	iFlags = 0;
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TScript::~TScript()
{
    // D�salloue tous les lignes de script et vide le tableau
    for (int i=0 ; i < SCN_TRIGGER_MAX ; i++)
    {  if (!vScript[i].empty())
       {	for (unsigned int j=0 ; j < vScript[i].size() ; j++)
           {	delete vScript[i][j];
           }
           vScript[i].clear();
       }
    }
}
//----------------------------------------------------------------------------//

//=== Charge le script � partir d'un fichier texte .zgs ======================//
bool TScript::LoadScript(char *scriptname)
{   
	char infos[MAX_LINE_CHAR]; 
	char temp[MAX_LINE_CHAR];

	// 1) Ouvre et charge le .zgs en m�moire
    TINILib ini(scriptname, true);
	
    // 2) R�cup�re les infos d'en-t�te du script
    // - Nom du script
	ini.GetStringProfile("HEADER", "ScriptName", Name);
	// - Type de script : personnalis� ou mod�le de script
	ini.GetStringProfile("HEADER", "ScriptType", infos);
	if (    (strcmp(infos, "PERSONNALISE") == 0)
		 ||	(strcmp(infos, "CUSTOMIZED") == 0))
	{	iScriptType = SCRIPT_TYPE_PERSONNALISE;
	} else 
	if (    (strcmp(infos, "MODELE") == 0)
		 ||	(strcmp(infos, "TEMPLATE") == 0))
	{	iScriptType = SCRIPT_TYPE_MODELE;
	}

	// - Nom du mod�le de script dont il d�rive
	ini.GetStringProfile("HEADER", "ScriptPatron", infos);
	if (strcmp(infos,"<none>") != 0)
	{	// On charge le script p�re
		// ...
	}

	// - Options parametrants ce script
	infos[0]='\0';
	ini.GetStringProfile("HEADER", "ScriptOptions", infos);
	if (infos[0]!='\0')
	{	setOptions(infos);
	}

	// 3) Encode chaque ligne de script
	int numScriptLigne = 0;
	bool endOfScript = false;
	do
	{	itoa(numScriptLigne, temp, 10);
		strcpy(infos,"SCRIPT_LINE_");
		strcat(infos,temp);
		endOfScript = (ini.SearchSection(infos) == -1);
		// Si l'on a trouv� un bloc-ligne de script, on r�cup�re son �v�nement 
		// d�clencheur, puis on encode les conditions et les actions associ�es
		if (!endOfScript)
		{	AddScriptLine(ini, infos);
		}
		numScriptLigne++;
	} while (!endOfScript);
    
    return true;
}
//----------------------------------------------------------------------------//

//=== Ajoute une ligne de script =============================================//
bool TScript::AddScriptLine(TINILib &script, char* scriptLine)
{	static char temp[MAX_LINE_CHAR];
	static char sConditions[MAX_SCRIPTLINE_CHAR];
	static char sActions[MAX_SCRIPTLINE_CHAR];
	int index = -1;
	ScriptLine* line = NULL;
	
	script.GetStringProfile(scriptLine, "Trigger", temp);
	if ((index = GetIndexTrigger(temp)) != -1)
	{	
		// R�cup�re les scripts de conditions
		if (script.GetScriptProfile(scriptLine, "Conditions", sConditions) == -1) return false;
		if (script.GetScriptProfile(scriptLine, "Actions", sActions) == -1) return false;
		
		// Cr�e le bloc-ligne de script et compile les conditions et les actions
		line = new ScriptLine();
		if (line->EncodeCauses(sConditions) != SCRIPT_NOERROR) return false;
		if (line->EncodeActions(sActions) != SCRIPT_NOERROR) return false;

		// Recupere les diverses options parametrant cette ligne de script
		temp[0]='\0';
		script.GetStringProfile(scriptLine, "Options", temp);
		if (temp[0]!='\0')
		{	line->setOptions(temp);
		}

		// Ajoute au script (d'apr�s son trigger) cette ligne de script encod�e
		vScript[index].push_back(line);
		return true;
	}
	
	return false;
}
//----------------------------------------------------------------------------//

//=== Ex�cution Ex�cution des scripts r�pondants � l'�v�nement iTrigger ======//
void TScript::LaunchScript(int iTrigger, GameObject* pGO, TEvent* pEvent)
{	// Si le script n'est pas re-entrant et est deja en cours d'execution, ce
	// n'est pas la peine d'aller plus loin
	if ( (!isReentrance()) &&  pGO->isScriptRunning()) return;
	
	ScriptLine* line;
	// Si toutes les conditions sont remplies, on ex�cute les actions
	for (int numScriptLine=0 ; numScriptLine < vScript[iTrigger].size() ; numScriptLine++) 
	{	line = vScript[iTrigger][numScriptLine];
		// Si la ligne du script est re-entrante (on peut l'executer en parallele)
		// ou si elle n'est pas deja en cours d'execution
		if (    (line->isReentrance())
			 || (!pGO->isScriptLineRunning(numScriptLine))   )
		{	if (line->isAllCausesRealized(pGO, pEvent))
			{	// [ATTENTION MODIF] vScript[iTrigger][i]->ExecuteActions(pGO, pEvents);
				pGO->AddRunningActionsScript(iTrigger, numScriptLine, pEvent, line->GetNbActions());
			}
		}
	}
}
//----------------------------------------------------------------------------//

//=== Indique si un script peut etre executee plusieurs fois en meme temps ===// 
//	  par un meme objet
inline bool TScript::isReentrance()
{	return ((iFlags & SCRIPT_FLAG_NO_REENTRANCE) != SCRIPT_FLAG_NO_REENTRANCE);
}
//----------------------------------------------------------------------------//

//=== Rajoute une option au script ===========================================//
inline void TScript::setFlag(int flag)
{	iFlags |= flag;
}
//----------------------------------------------------------------------------//

//=== Parse les options d'une ligne de script ================================//
int TScript::setOptions(char* options)
{	int cur=0, i, optionType;
	char car;
	static char keyword[MAX_LENGTH_KEYWORD];
	bool findOption = false;

	// Tant que l'on n'est pas arriv� � la fin des options
	while (options[cur]!='\0')
	{	car = options[cur];
		// Saute tous les caract�res non interpr�tables
	while ((ScriptLine::isInvalidChar(car)) && (car!='\0')) cur++;
		// V�rifie si l'on n'est pas arriv� � la fin des options
		if ((car=='\0') && (!findOption)) return SCRIPT_ERROR_NOTHING;
		
		// R�cup�re une option en s'arr�tant lorsque l'on trouve un  ou un blanc ou un car. de fin de ligne
		i = 0;
		do
		{	car = keyword[i++] = options[cur++];
		}
		while ((!ScriptLine::isInvalidChar(car)) && (car!='|') && (car!='\0') && (i<MAX_LENGTH_KEYWORD));
		keyword[i-1] = '\0';

		// V�rifie si c'est bien un mot-cl� et r�cup�re le SCRIPT_OPTION_ associ�
		optionType = GetScriptOption(keyword);
		if (optionType == -1) return SCRIPT_ERROR_NOTANOPTION;
		findOption = true;
		switch (optionType)
		{	case SCRIPT_OPTION_NO_REENTRANCE :
				setFlag(SCRIPT_FLAG_NO_REENTRANCE);
				break;
		}

		// Saute tous les caract�res non interpr�tables
		while ((ScriptLine::isInvalidChar(car = options[cur])) && (car!='\0')) cur++;
		if ((car=='\0') && (!findOption)) return SCRIPT_ERROR_NOTHING;

		// Nouvelle optiom trouv�e
		if (car == '|')
		{	cur++;		// on saute le car. '|'
		}
	}	
	return SCRIPT_NOERROR;	
}
//----------------------------------------------------------------------------//



///////////////////////////// Fin de TSCRIPT ///////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
// Classe GAMESCRIPTLIST                                                      //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur par d�faut ================================================//
GameScriptList::GameScriptList()
{	TScript*	script = NULL;
	AnsiString name;
	char zgsFilename[128];
	int zgsID;
	
	GameScripts = this;

	// Charge la librairies des identifiants des scripts ainsi le nom des fichiers .zgs
	IDGameScripts = new std::IDMap();
    #ifndef NEW_FILE_MANAGER
	#ifndef MAPEDITOR
	IDGameScripts->LireFileID(CheminApplication + cIDGameScriptsFilename);
    #else
    IDGameScripts->LireFileID(CheminEditor + cIDGameScriptsFilename);
    #endif // MAPEDITOR
	#else
	IDGameScripts->LireFileID(cIDGameScriptsFilename);
	#endif // NEW_FILE_MANAGER

	// Lit les fichiers de scripts .zgs
	for (std::IDMap::Iterat iter = IDGameScripts->MapID.begin(); iter != IDGameScripts->MapID.end(); iter++)
    {	zgsID = (*iter).second;
		name = (*iter).first + ".zfs";
		name = "World\\" + name;
		#ifndef NEW_FILE_MANAGER
		name = CheminApplication + name;
		#endif
		#ifdef CBUILDER
		strcpy(zgsFilename, name.c_str());
		#else
		strcpy(zgsFilename, LPCSTR(name));
		#endif
		script = new TScript(zgsID);
		script->LoadScript(zgsFilename);
		ScriptsList.insert(std::make_pair(zgsID, script));
    }
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
GameScriptList::~GameScriptList()
{
	// Vide le dictionnaire de tous ses scripts
    for (iter = ScriptsList.begin(); iter != ScriptsList.end(); iter++)
    {	delete (*iter).second;
    }
    ScriptsList.clear();
}
//----------------------------------------------------------------------------//

//=== R�cup�re le dico des ID des scripts du jeu =============================//
std::IDMap*  GameScriptList::GetScriptMap()
{	return IDGameScripts;
}
//----------------------------------------------------------------------------//

//=== Renvoie un pointeur vers le script n�iID ===============================//
TScript* GameScriptList::GetScript(int iID)
{	return ScriptsList[iID];
}
//----------------------------------------------------------------------------//

////////////////////////// Fin de GAMESCRIPTLIST ///////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Fonctions ANNEXES														  //
////////////////////////////////////////////////////////////////////////////////

//=== Initialise la liste de scripts du jeu ==================================//
void InitGame_GameScriptList()
{	// Charge les librairies des identifiants
	// a] Personnages PNJ/Hero/Monstres ayant leur propre personnalit�
    IDGamePersos = new std::IDMap();
    #ifndef NEW_FILE_MANAGER
	IDGamePersos->LireFileID(CheminApplication + cIDGamePersosFilename);
	#else
	IDGamePersos->LireFileID(cIDGamePersosFilename);
	#endif // NEW_FILE_MANAGER
	// b] D�cors sc�naris�s 
	IDGameDecors = new std::IDMap();
	#ifndef NEW_FILE_MANAGER
	IDGameDecors->LireFileID(CheminApplication + cIDGameDecorsFilename);
	#else
	IDGameDecors->LireFileID(cIDGameDecorsFilename);
	#endif // NEW_FILE_MANAGER

	// Alloue la liste globale des scripts du jeu
	GameScripts = new GameScriptList();
}
//----------------------------------------------------------------------------//

//=== Referme proprement la liste des scripts ================================//
void CloseGame_GameScriptList()
{	// D�salloue les librairies des identifiants
	// a] D�cors sc�naris�s 
	delete IDGameDecors; IDGameDecors = NULL;
	// b] Personnages PNJ/Hero/Monstres ayant leur propre personnalit�
    delete IDGamePersos; IDGamePersos = NULL;
    	
	// D�salloue la liste globale des scripts du jeu
	delete GameScripts;	GameScripts = NULL;
}
//----------------------------------------------------------------------------//
