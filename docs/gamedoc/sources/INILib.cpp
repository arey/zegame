////////////////////////////////////////////////////////////////////////////////
//  Unité  "INILib.cpp"  : Librarie permettant de charger un .ini crypté ou   //
//			 non et de travailler dessus directement en mémoire				  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef MICROSOFT
//using namespace std;
#endif


//-------------------------------------------------------------- fichiers inclus
#ifdef VISUALC
#include "StdAfx.h"         //--- Special for Visual C++ 6 MFC ---
#endif
#include <fstream.h>		// Flux de données de la STL
#ifdef NEW_FILE_MANAGER
#include "FileVirtual.h"	// Fichiers virtuels (dans un .pak, en mémoire ou tout simplement sur le disk)
#endif // NEW_FILE_MANAGER
#include "INILib.h"         // son Header


//------------------------------------------------------------------ définitions
#define TRACEDBG	TRACE
#define MAX_SECTION_NAME	128
#define MAX_PROFILE_NAME	128


//------------------------------------------------------ variables et constantes



////////////////////////////////////////////////////////////////////////////////
// Classe TINILIB															  //                                            
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur ===========================================================//
TINILib::TINILib(char* filename, bool useFileManager)
{	pMem = NULL;
	iSize = 0;

	// Charge le .ini en mémoire
	LoadFile(filename, useFileManager);

	// Décrypte le fichier si il est crypté
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TINILib::~TINILib()
{	if (pMem != NULL)
	{	delete pMem;
		pMem = NULL;
	}
}
//----------------------------------------------------------------------------//

//=== Charge un .ini en mémoire ==============================================//
bool TINILib::LoadFile(char* filename, bool useFileManager)
{	// Ouvre en lecture le fichier
	
	if (!useFileManager)
	{	ifstream	fichier(filename, ifstream::in);
		if (fichier.fail()) return false;
		
		// Récupère la taille du fichier et alloue la mémoire nécessaire à son chargement
		fichier.seekg(0 ,ifstream::end);
		iSize = fichier.tellg();
		
		// Charge le fichier en ram
		pMem = new char[iSize];	
		fichier.seekg(0 ,ifstream::beg);
		fichier.read(pMem, iSize);
		
		// Referme le fichier et sort
		fichier.close();
	} else
	{	// On utilise le FileManager
		TFileVirtual *fichier = fopenFV(filename);
		if (fichier == NULL) return false;
		// Récupère la taille du fichier et alloue la mémoire nécessaire à son chargement
		iSize = fichier->size();
		pMem = new char[iSize];	
		fichier->read(pMem, iSize);
		// Referme le fichier et sort
		fcloseFV(fichier);
	}
	
	return true;
}
//----------------------------------------------------------------------------//


//=== Renvoie un pointeur sur le n° de la prochaine ligne rencontrée =========//
// -> recherche à partir d'offset et renvoie le nb de caractère la composant
int	 TINILib::GetNextLine(int &index,int offset)
{		
	if (offset >= iSize) return -1;		// Sort si l'offset > à la taille

	// Si ce n'est pas le 1er caractère d'une ligne
	if ((offset != 0) && (pMem[offset-1] != '\n'))
	{	// on recherche de la prochaine ligne
		while ((pMem[offset]!='\n') && (offset < iSize)) offset++;
	
		// Saute le caractère de fin de ligne
		offset++;
		if (offset >= iSize) return -1;
	}
	index = offset; 

	// Recherche la fin de la ligne
	int nbcar = 0;	// Nb de car. de la ligne
	while ((pMem[offset]!='\n') && (offset < iSize)) 
	{	offset++; 
		nbcar++;
	}	
	if (offset >= iSize) return -1;

	return nbcar;
}
//----------------------------------------------------------------------------//

//=== Renvoie l'offset de la ligne suivante ==================================//
int TINILib::GetNextLine(int offset)
{	char car;
	do
	{	car = pMem[offset++];
	}
	while ((car != '\n') && (offset < iSize))	;
	if (offset >= iSize) return -1;
	return offset;
}
//----------------------------------------------------------------------------//

//=== Renvoie l'offset de la ligne suivant le nom de la section recherchée ===//
int  TINILib::SearchSection(char* sectionname)
{	
	int offset = 0;		// On part du début du fichier
	int newoffset =0;	// 
	int nboctets = 0;
	char section[MAX_SECTION_NAME];

	while ((offset < iSize) && (nboctets!=-1))
	{	nboctets = GetNextLine(newoffset, offset);
		offset = newoffset;				// début d'une ligne
		
		if (pMem[offset] == '[')		// début de section
		{	int i=1;
			while (pMem[offset + i]!=']') 
			{	section[i-1] = pMem[offset + i];	i++;
			}
			section[i-1] = '\0';

			// Vérifie si c'est la section que l'on recherchait
			if (strcmp(sectionname, section)==0)	
			{	GetNextLine(newoffset, offset+1);
				return newoffset;
			}
		}
		offset++;	// Saute le 1er caractère de début de ligne	
	}

	return -1;
}
//----------------------------------------------------------------------------//

//=== Indique si la ligne située à l'offset 'offset' correspond au profile recherché 
// ->  -1  : début d'une nouvelle section ou fin du fichier
//	    1  : profilename trouvé
//      0  : pas cette ligne
int TINILib::SearchProfile(char *profilename, int offset)
{	
	char car;
	int i = 0;
	char profile[MAX_PROFILE_NAME];

	// Parcours la ligne à partir d'offset et recherche le profilename voulu
	do
	{	car = pMem[offset];
		// Saute les espaces et les tabs de début et de fin de ligne
		if ((car == ' ') || (car == '\t')) offset++;
		else 
		// Saute les commentaires
		if (car == ';') return 0; 
		else
		// Nouvelle section
		if (car == '[') return -1;	
		else
		// Si c'est un égal, on vérifie si c'est le profilname que l'on recherche, sinon on cherche la fin de la ligne
		if (car == '=')
		{	if (i==0) return false;
			profile[i] = '\0';
			return (int) (strcmp(profile, profilename) == 0);
		}
		else
		// Récupère les caractères du profilename pottentiel
		{	profile[i++]=car;
			offset++;
		}
	}
	while ((car!='\n') && (offset<iSize));

	if (offset>=iSize) return -1;	// Fin du fichier
	return 0;						// Fin de ligne
}
//----------------------------------------------------------------------------//

//=== Renvoie la chaîne de caractère suivant le '=' ==========================//
int	TINILib::GetStringValue(int offset, char* resultname)
{	int nbOctets = 0;
	char car;

	// Parcours la ligne jusqu'au signe '='
	while (pMem[offset++]!='=');

	// Se positionne au début de la chaîne de caratères suivant le '='
	do 
	{	car = pMem[offset++];
	} while ((car==' ') || (car=='\t'));
	offset--;

	// Copie de la chaîne trouvée jusqu'au caract. de fin de ligne
	do
	{	car = pMem[offset++];
		resultname[nbOctets++] = car;
	}
	while (car != '\n');
	resultname[nbOctets-1] = '\0';

	// Rajouter au cas où l'on est ouvert le fichier .ini en mode binaire
	if (resultname[nbOctets-2] == 13) resultname[nbOctets-2] = '\0';

	// Suppression des espaces en fin de ligne
	TrimRight(resultname);
	
	return nbOctets;
}
//----------------------------------------------------------------------------//


//=== Renvoie une chaîne suivant le "profilename = " =========================//
// ->  -1 : si pas trouvé
//      n : nb de car. contenus dans resultname
int TINILib::GetStringProfile(char* sectionname, char* profilename, char* resultname)
{	int nbcar = 0;	// Nb de caractères présents ds la string renvoyée
	int offset;
	
	
	// Recheche tout d'abord la ligne suivant la section recherchée
	offset = SearchSection(sectionname);
	if (offset==-1) return -1;		// Section introuvable

	// Puis recherche pour chaque ligne suivante le profilename recherché
	int res = 0;
	while ((offset < iSize) && (res == 0) && (offset != -1))
	{	res = SearchProfile(profilename, offset);
		if (res == -1) return -1;	// fin de fichier ou  section suivante
		
		if (res == 1)
		{	return GetStringValue(offset, resultname);
		}
		
		// Sinon on passe à la ligne suivante
		offset = GetNextLine(offset);
	}
	
	return nbcar;
}
//----------------------------------------------------------------------------//

//=== Supprime espaces et tabulations de fin de chaîne =======================//
void TINILib::TrimRight(char* str)
{	// Recherche le caractère de fin de chaîne
	int end = 0;
	while (str[end]!='\0') end++;
	end--;

	// Puis remonte la chaîne en supprimant espaces et tabulations
	while ((end>=0) && ((str[end]==' ') || (str[end]=='\t'))) end--;
	str[++end] = '\0';
}
//----------------------------------------------------------------------------//


//=== Renvoie l'entier suivant le "profilename = " ===========================//
int TINILib::GetIntProfile(char* sectionname, char* profilename, int idefault)
{	// Récupère la chaîne de caractère suivant le sous-nom de section
	char str[128];
	int hr = GetStringProfile(sectionname, profilename, str);
	if (hr==-1) return idefault;

	// Convertion de la chaîne en entier
	int res = atoi(str);
	return res;
}
//----------------------------------------------------------------------------//

//=== Renvoie le nb flottant suivant le "profilename = " =====================//
double TINILib::GetFloatProfile(char* sectionname, char* profilename, double fdefault)
{	// Récupère la chaîne de caractère suivant le sous-nom de section
	char str[128];
	int hr = GetStringProfile(sectionname, profilename, str);
	if (hr==-1) return fdefault;

	// Convertion de la chaîne en entier
	double res = atof(str);
	return res;
}
//----------------------------------------------------------------------------//

//= Renvoie le script suivant le "profilename = " délimité par des accolades =//
// ->  -1 : si pas trouvé
//      n : nb de car. contenus dans resultscript
int TINILib::GetScriptProfile(char* sectionname, char* profilename, char* resultscript)
{	int nbcar = 0;	// Nb de caractères présents ds la string renvoyée
	int offset;
	
	// Recheche tout d'abord la ligne suivant la section recherchée
	offset = SearchSection(sectionname);
	if (offset==-1) return -1;		// Section introuvable

	// Puis recherche pour chaque ligne suivante le profilename recherché
	int res = 0;
	while ((offset < iSize) && (res == 0) && (offset != -1))
	{	res = SearchProfile(profilename, offset);
		if (res == -1) return -1;	// fin de fichier ou  section suivante
		
		if (res == 1)
		{	int nbOctets = 0;
			char car;

			// Parcours la ligne jusqu'au signe '='
			while (pMem[offset++]!='=');

			// Se positionne sur l'accolade ouverte de début de script '{'
			do 
			{	car = pMem[offset++];
			} while (car!='{');

			// Supprime les espaces et les tabs
			while ((pMem[offset]==' ') || (pMem[offset]=='\t')) offset++;
			
			// Copie de la chaîne trouvée jusqu'à l'accolade fermée '}'
			// supprime les caractères de fin et les retour à la ligne 0x0D et 0x0A
			do
			{	car = pMem[offset++];
				
				// Si l'on est arrivé au bout du fichier, on quitte
				if ((car == '\0') || (offset>=iSize)) return -1;	

				if ((car!=0x0A) && (car!=0x0D))
				{	resultscript[nbOctets++] = car;
				}
			}
			while (car != '}');
			resultscript[nbOctets-1] = '\0';

			// Suppression des espaces en fin de ligne
			TrimRight(resultscript);
			
			return nbOctets;
		}
		
		// Sinon on passe à la ligne suivante
		offset = GetNextLine(offset);
	}
	
	return nbcar;
}
//----------------------------------------------------------------------------//

// Fin de la classe TINILIB ////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Fonctions ANNEXES                                                          //
////////////////////////////////////////////////////////////////////////////////








