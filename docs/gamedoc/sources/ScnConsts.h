////////////////////////////////////////////////////////////////////////////////
//  Unit� "ScnConsts.h"  :  regroupe toutes les constantes et les definitions //
//							propre � la gestion de sc�narios.				  //
////////////////////////////////////////////////////////////////////////////////
#ifndef ScnConstsH
#define ScnConstsH

//----------------------------------------------------------------------- Define
#define LANGUAGE_FRANGLAIS		0
#define LANGUAGE_ENGLISH		1
#define NB_SCRIPT_LANGUAGE		2

#define MAX_LENGTH_KEYWORD	  196		// Nb car. max. pour les mots-cl�s entre parenth�se (HERO, PORTE_SUD, ...)
#define MAX_LENGTH_PREFIX	   32		// Nb car. max. d'un pr�fixe pr�c�dent une action ou une cause


//------------------------------------------------------------------------- Enum

//=== Trigger d�clenchant une ligne de script ===
enum
{	SCN_TRIGGER_FIRST = 0,
	SCN_TRIGGER_UNKNOW = SCN_TRIGGER_FIRST,	
	SCN_TRIGGER_EVERY_TIME,
	SCN_TRIGGER_SCENARIO,
	SCN_TRIGGER_SCRIPT_FIRST,
		SCN_TRIGGER_PLAYER_UTILISER = SCN_TRIGGER_SCRIPT_FIRST,
		SCN_TRIGGER_PLAYER_FRAPPER,
		SCN_TRIGGER_PERSO_DSZONE,	// D�clench� � chaque fois qu'un personnage est dans telle zone
		SCN_TRIGGER_PERSO_ZONEIN,	// D�clench� une seule fois lorsqu'un personnage rentre dans une nouvelle zone
		SCN_TRIGGER_PERSO_ZONEOUT,	// D�clench� une seule fois lorsqu'un personnage sort d'une zone dans laquelle il se trouvait
		SCN_TRIGGER_METEO,
		SCN_TRIGGER_TIME,
		SCN_TRIGGER_PNJ_DEAD,
	SCN_TRIGGER_SCRIPT_LAST = SCN_TRIGGER_PNJ_DEAD,
	SCN_TRIGGER_LAST = SCN_TRIGGER_SCRIPT_LAST,
	SCN_TRIGGER_MAX
};

const char* const  sTriggerType[SCN_TRIGGER_MAX] =
{   "UNKNOW",
	"EVERY_TIME",
	"SCENARIO",
    "PLAYER_UTILISER",
    "PLAYER_FRAPPER",
	"PERSO_DSZONE",
	"PERSO_ZONEIN",
	"PERSO_ZONEOUT",
    "METEO",
    "TIME",
    "PNJ_DEAD"	
};

//=== TAction : Type de l'action ====
enum
{	
	FIRST_ACTION_TYPE					=   0,
	ACTION_TYPE_NONE					= FIRST_ACTION_TYPE,
	// Concerne tous les types de GameObject
		ACTION_TYPE_GO_NEWMEMORY,
		ACTION_TYPE_GO_DELMEMORY,
		ACTION_TYPE_GO_SETSCRIPT,
		ACTION_TYPE_GO_VISIBLE,
	// Personnages : h�ro, pnj, monstres
		ACTION_TYPE_PERSO_MOVETO,
		ACTION_TYPE_PERSO_MOVEFROM,
		ACTION_TYPE_PERSO_CREATE,
		ACTION_TYPE_PERSO_DELETE,
	// Objets : arme, outil
		ACTION_TYPE_OBJET_UTILISER,		
	// D�cors : arbre, grue, table, porte 
		ACTION_TYPE_DECORS_FRAPPER,
		ACTION_TYPE_DECORS_UTILISER,
		ACTION_TYPE_DECORS_PLAYANIM,
		ACTION_TYPE_DECORS_PLAYSUPANIM, 	  
		ACTION_TYPE_DECORS_NEWSTATE,
		ACTION_TYPE_DECORS_FORCENEWSTATE,
	// Sc�nario : Niveau, jeu ...
		ACTION_TYPE_SCENARIO_NEWLEVELMEMORY,
		ACTION_TYPE_SCENARIO_DELLEVELMEMORY,
	// Son
		ACTION_TYPE_SOUND_PLAY,
	// Synchro
		ACTION_TYPE_SYNCHRO_SLEEP,			// Attends N ms
		ACTION_TYPE_SYNCHRO_WAITEND,		// Synchronisation sur une action pr�c�dente
	LAST_ACTION_YPE						= ACTION_TYPE_SYNCHRO_WAITEND,
	MAX_ACTION_TYPE
};

const char* const  sActionType[MAX_ACTION_TYPE][NB_SCRIPT_LANGUAGE] =
{   
	{ "Rien",			"Nothing"		},
	{ "Apprends",		"NewMemory"		},
	{ "Oublie",			"DelMemory"		},
	{ "AttribueScript",	"SetScript"		},
	{ "Visible",		"Visible"		},
	{ "DeplacerEn",		"MoveTo"		},
	{ "DeplacerDe",		"MoveFrom"		}, 	
	{ "CreerPerso",		"CreateNPC"		},
	{ "SupprPerso",		"DeleteNPC"		},
	{ "Utiliser",		"Use"			},
	{ "Frapper",		"Heat"			},
	{ "Utiliser",		"Use"			},
	{ "LancerAnim",		"PlayAnim"		},
	{ "JouerAnimSup",	"PlaySupAnim"	},
	{ "NouvelEtat",		"NewState"		},
	{ "ForceEtat",		"ForceNewState" },
	{ "NivApprends",	"NewLevelMemory"},
	{ "NivOublie",		"DelLevelMemory"},
	{ "JouerSon",		"PlaySound"		},
	{ "Dormir",			"Sleep"			},
	{ "AttendFin",		"WaitEnd"		}
};


//=== TAction : Pr�fixe pouvant pr�c�d� un nom d'action ====
enum
{	
	FIRST_ACTION_PREFIXE				=   0,
	ACTION_PREFIXE_NONE					= FIRST_ACTION_PREFIXE,
		ACTION_PREFIXE_PERSO,
	LAST_ACTION_PREFIXE					= ACTION_PREFIXE_PERSO,
	MAX_ACTION_PREFIXE
};

const char* const sActionPrefixe[MAX_ACTION_PREFIXE][NB_SCRIPT_LANGUAGE] =
{	{ "rien",			"none"			},
	{ "perso",			"caract"		}
};



//=== TCause : type de la cause ===
enum
{	FIRST_CAUSE_TYPE					=   0,
	CAUSE_TYPE_NONE						= FIRST_CAUSE_TYPE,
		CAUSE_TYPE_THING_STRIKE_BY,
		CAUSE_TYPE_THING_STRIKE_WITH,
		CAUSE_TYPE_THING_MEMORY_IN,
		CAUSE_TYPE_THING_MEMORY_NO,
	
		CAUSE_TYPE_LEVEL_MEMORY_IN,	
		CAUSE_TYPE_LEVEL_MEMORY_NO,

		CAUSE_TYPE_PERSO_ZONE_IN,		// Personnage pr�sent dans telle zone
		CAUSE_TYPE_HERO_ZONE_IN,		// Hero			"		"	 "	  "		
		CAUSE_TYPE_PERSO_ZONE_OUT,		// Personnage hors de telle zone
		CAUSE_TYPE_HERO_ZONE_OUT,		// Hero			"		"	  "

		CAUSE_TYPE_RANDOM,
		CAUSE_TYPE_TIMER,
	LAST_CAUSE_TYPE						= CAUSE_TYPE_TIMER,
	MAX_CAUSE_TYPE
};

const char* const  sCauseType[MAX_CAUSE_TYPE][NB_SCRIPT_LANGUAGE] =
{   
    {"Rien",			"Nothing"				},
	{"FrapperPar",		"StrikeBy"				},
	{"FrapperAvec",		"StrikeWith"			},
	{"Connait",			"Know"					},
	{"NeSaisPas",		"DontKnow"				},
	{"NivConnait",		"LevelKnown"			},
	{"NivNeSaisPas",	"LevelUnKnown"			},
	{"PersoDsZone",		"CharacterIntoZone"		},
	{"HeroDsZone",		"HeroIntoZone"			}, 
	{"PersoHorsZone",	"CharacterOutsideZone"	},
	{"HeroHorsZone",	"HeroOutsideZone"		},
	{"Random",			"Rand"					},
	{"ToutesLesNms",	"Timer"					}	
};


//-------------------------------------------------------------- Fonctions annexes

int GetIndexTrigger(char* triggertype);		// Renvoie le n� correspondant au nom du trigger pass� en param�tre
int GetActionType(char* action);			// Renvoie le n� de l'action correspondant � cette cha�ne
int GetCauseType(char *cause);				// Renvoie le n� de la cause correspondant � cette cha�ne
int GetActionPrefix(char* action);			// Renvoie le n� du prefix d'une action  et renvoie l'action
char* GetActionPrefixName(int numPrefix);	// Renvoie le nom du prefix n�numPrefix (en anglais)




#endif
