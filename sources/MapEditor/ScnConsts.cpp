#include "ZeNain.h"
#include "ScnConsts.h"      // Son Header
#ifdef CBUILDER
#include <stdio.h>
#include <string.h>
#endif  // CBUILDER


//=== Renvoie le n° correspondant au nom du trigger passé en paramètre ===
int GetIndexTrigger(char* triggertype)
{	for (int i=0 ; i < SCN_TRIGGER_MAX ; i++)
	{	if (strcmp(triggertype, sTriggerType[i]) == 0)
		{ return i;
		}
	}
	return -1;
}

//=== Renvoie le n° de l'action correspondant à cette chaîne ===
int GetActionType(char* action)
{	for (int i=FIRST_ACTION_TYPE ; i < MAX_ACTION_TYPE ; i++)
	{	for (int j=0 ; j < NB_SCRIPT_LANGUAGE ; j++)
		{	if (strcmp(sActionType[i][j], action) == 0) 
			{	return i;
			}
		}
	}
	return -1;
}

//=== Renvoie le n° de la cause correspondant à cette chaîne ===
int GetCauseType(char *cause)
{	for (int i=FIRST_CAUSE_TYPE ; i < MAX_CAUSE_TYPE ; i++)
	{	for (int j=0 ; j < NB_SCRIPT_LANGUAGE ; j++)
		{	if (strcmp(sCauseType[i][j], cause) == 0) 
			{	return i;
			}
		}
	}
	return -1;
}


//=== Renvoie le n° du prefix d'une action ===
//	  ainsi que le bon mot-clé
int GetActionPrefix(char* action)
{	char prefix[MAX_LENGTH_PREFIX];
	char keyword[MAX_LENGTH_KEYWORD];
	int actionLength = strlen(action);
	int keywordLength;

	for (int u=0 ; u < actionLength ; u++)
	{	if (action[u] == '.')
		{	memcpy(prefix, action, u);	
			prefix[u++] = '\0';
			keywordLength = actionLength - u + 1;
			memcpy(keyword, &action[u], keywordLength);
			keyword[keywordLength] = '\0';
			strcpy(action, keyword);

			for (int i=FIRST_ACTION_PREFIXE ; i < MAX_ACTION_PREFIXE ; i++)
			{	for (int j=0 ; j < NB_SCRIPT_LANGUAGE ; j++)
				{	if (strcmp(sActionPrefixe[i][j], prefix) == 0) 
					{	return i;
					}
				}
			}
		}
	}	
	return ACTION_PREFIXE_NONE;
}

//=== Renvoie le nom du prefix n°numPrefix (en anglais) ===
char* GetActionPrefixName(int numPrefix)
{	return (char*) sActionPrefixe[numPrefix][LANGUAGE_FRANGLAIS];
}