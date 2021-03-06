////////////////////////////////////////////////////////////////////////////////
//  Unit�  "INILib.cpp"  : Librarie permettant de charger un .ini crypt� ou   //
//			 non et de travailler dessus directement en m�moire				  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef MICROSOFT
//using namespace std;
#endif


//-------------------------------------------------------------- fichiers inclus
#ifdef VISUALC
#include "StdAfx.h"         //--- Special for Visual C++ 6 MFC ---
#endif
#include <fstream.h>		// Flux de donn�es de la STL

#ifdef NEW_FILE_MANAGER
#include "FileVirtual.h"	// Fichiers virtuels (dans un .pak, en m�moire ou tout simplement sur le disk)
#endif // NEW_FILE_MANAGER
#include "INILib.h"         // son Header


//------------------------------------------------------------------ d�finitions
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
	// Se place au debut du fichier
	iCurseur=0;			
	iNumSection=0;

	// Charge le .ini en m�moire
	LoadFile(filename, useFileManager);

	// D�crypte le fichier si il est crypt�
}
//----------------------------------------------------------------------------//

TINILib::TINILib(char* buffer, int size)
{	pMem = NULL;
	iSize = 0;
	// Copie le tampon passe en entree
	LoadFileFromMem(buffer, size);
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TINILib::~TINILib()
{	if (pMem != NULL)
	{	delete [] pMem;
		pMem = NULL;
	}
}
//----------------------------------------------------------------------------//

//=== Charge un .ini en m�moire ==============================================//
bool TINILib::LoadFile(char* filename, bool useFileManager)
{	// Ouvre en lecture le fichier
	
	if (!useFileManager)
	{	ifstream	fichier(filename, ifstream::in);
		if (fichier.fail()) return false;
		
		// R�cup�re la taille du fichier et alloue la m�moire n�cessaire � son chargement
		fichier.seekg(0 ,ifstream::end);
		iSize = fichier.tellg();
		
		// Charge le fichier en ram
		pMem = new char[iSize];	
		fichier.seekg(0 ,ifstream::beg);
		fichier.read(pMem, iSize);
		
		// Referme le fichier et sort
		fichier.close();
	} 

#ifdef NEW_FILE_MANAGER
	else
	{	// On utilise le FileManager
		TFileVirtual *fichier = fopenFV(filename);
		if (fichier == NULL) return false;
		// R�cup�re la taille du fichier et alloue la m�moire n�cessaire � son chargement
		iSize = fichier->size();
		pMem = new char[iSize];	
		fichier->read(pMem, iSize);
		// Referme le fichier et sort
		fcloseFV(fichier);
	}
#endif // NEW_FILE_MANAGER
	
	return true;
}
//----------------------------------------------------------------------------//

//=== Charge un .ini en memoire directement a partir d'un buffer en memoire ==//
bool TINILib::LoadFileFromMem(char* buffer, int size)
{	iSize = size;
	pMem = new char[iSize];	
	strcpy(pMem, buffer);
	return true;
}
//----------------------------------------------------------------------------//

//=== Renvoie un pointeur sur le n� de la prochaine ligne rencontr�e =========//
// -> recherche � partir d'offset et renvoie le nb de caract�re la composant
int	 TINILib::GetNextLine(int &index,int offset)
{		
	if (offset >= iSize) return -1;		// Sort si l'offset > � la taille

	// Si ce n'est pas le 1er caract�re d'une ligne
	if ((offset != 0) && (pMem[offset-1] != '\n'))
	{	// on recherche de la prochaine ligne
		while ((pMem[offset]!='\n') && (offset < iSize)) offset++;
	
		// Saute le caract�re de fin de ligne
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

//=== Se place au debut de la section suivant cet @offset ===================//
// @param	offset	curseur ds le .ini a partir duquel on lance la recherche (input)
// @return	offset de debut de la section trouvee (-1 si on en n'a pas trouve)
int	TINILib::getNewSectionFrom(int offset)
{	int newoffset =0;
	int nboctets = 0;

	while ((offset < iSize) && (nboctets!=-1))
	{	nboctets = GetNextLine(newoffset, offset);
		if ((nboctets==-1) || (newoffset==-1) || (newoffset>=iSize)) return -1;
		offset = newoffset;				// d�but d'une ligne
		// On a trouv� un d�but de section
		if (pMem[offset] == '[')
		{	return offset;
		}
		offset++;	// Saute le 1er caract�re de d�but de ligne	
	}
	return -1;
}
//----------------------------------------------------------------------------//

//=== Renvoi le nom de la section demarrant a tel @offset ====================//
// @param	offset	curseur ds le .ini ou debute la section  -> caracter '[' (input)
// @param	name	nom de cette section (output)
void TINILib::getSectionName(int offset, char* name)
{		// Recupere le nom de la section
	int i=1;
	while ((pMem[offset+i]!=']') && (offset+i<iSize))
	{	name[i-1] = pMem[offset+i];
		i++;
	}
	if (offset+i>=iSize) // fin de section non trouvee
	{	name[0]='\0';
		return;
	}
	name[i-1] = '\0';
}
//----------------------------------------------------------------------------//

//=== Renvoie l'offset de la ligne suivant le nom de la section recherch�e ===//
int TINILib::SearchSection(char* sectionname)
{	static char section[MAX_SECTION_NAME];
	int offset = 0;		// On part du d�but du fichier
	int newoffset;

	while (((offset=getNewSectionFrom(offset)) != -1) && (offset<iSize))
	{
		// Recupere le nom de la section
		getSectionName(offset, section);

		// V�rifie si c'est la section que l'on recherchait
		if (strcmp(sectionname, section)==0)	
		{	GetNextLine(newoffset, offset+1);
			return newoffset;
		}
		offset++;	// Saute le 1er caract�re de d�but de ligne	
	}
	return -1;
}
//----------------------------------------------------------------------------//

//=== Indique si la ligne situ�e � l'offset 'offset' correspond au profile recherch� 
// ->  -1  : d�but d'une nouvelle section ou fin du fichier
//	    1  : profilename trouv�
//      0  : pas cette ligne
int TINILib::SearchProfile(char *profilename, int offset)
{	
	char car;
	int i = 0;
	char profile[MAX_PROFILE_NAME];

	// Parcours la ligne � partir d'offset et recherche le profilename voulu
	do
	{	car = pMem[offset];
		// Saute les espaces et les tabs de d�but et de fin de ligne
		if ((car == ' ') || (car == '\t')) offset++;
		else 
		// Saute les commentaires
		if (car == ';') return 0; 
		else
		// Nouvelle section
		if (car == '[') return -1;	
		else
		// Si c'est un �gal, on v�rifie si c'est le profilname que l'on recherche, sinon on cherche la fin de la ligne
		if (car == '=')
		{	if (i==0) return false;
			profile[i] = '\0';
			return (int) (strcmp(profile, profilename) == 0);
		}
		else
		// R�cup�re les caract�res du profilename pottentiel
		{	profile[i++]=car;
			offset++;
		}
	}
	while ((car!='\n') && (offset<iSize));

	if (offset>=iSize) return -1;	// Fin du fichier
	return 0;						// Fin de ligne
}
//----------------------------------------------------------------------------//

//=== Renvoie la cha�ne de caract�re suivant le '=' ==========================//
int	TINILib::GetStringValue(int offset, char* resultname)
{	int nbOctets = 0;
	char car;

	// Parcours la ligne jusqu'au signe '='
	while (pMem[offset++]!='=');

	// Se positionne au d�but de la cha�ne de carat�res suivant le '='
	do 
	{	car = pMem[offset++];
	} while ((car==' ') || (car=='\t'));
	offset--;

	// Copie de la cha�ne trouv�e jusqu'au caract. de fin de ligne
	do
	{	car = pMem[offset++];
		resultname[nbOctets++] = car;
	}
	while ((car != '\n') && (offset<iSize));	// fin de ligne ou fin de fichier
	resultname[nbOctets-1] = '\0';

	// Rajouter au cas o� l'on est ouvert le fichier .ini en mode binaire
	if (resultname[nbOctets-2] == 13) resultname[nbOctets-2] = '\0';

	// Suppression des espaces en fin de ligne
	TrimRight(resultname);
	
	return nbOctets;
}
//----------------------------------------------------------------------------//


//=== Renvoie une cha�ne suivant le "profilename = " =========================//
// ->  -1 : si pas trouv�
//      n : nb de car. contenus dans resultname
int TINILib::GetStringProfile(char* sectionname, char* profilename, char* resultname)
{	int nbcar = 0;	// Nb de caract�res pr�sents ds la string renvoy�e
	int offset;
	
	
	// Recheche tout d'abord la ligne suivant la section recherch�e
	offset = SearchSection(sectionname);
	if (offset==-1) return -1;		// Section introuvable

	// Puis recherche pour chaque ligne suivante le profilename recherch�
	int res = 0;
	while ((offset < iSize) && (res == 0) && (offset != -1))
	{	res = SearchProfile(profilename, offset);
		if (res == -1) return -1;	// fin de fichier ou  section suivante
		
		if (res == 1)
		{	return GetStringValue(offset, resultname);
		}
		
		// Sinon on passe � la ligne suivante
		offset = GetNextLine(offset);
	}
	
	return nbcar;
}
//----------------------------------------------------------------------------//

//=== Supprime espaces et tabulations de fin de cha�ne =======================//
void TINILib::TrimRight(char* str)
{	// Recherche le caract�re de fin de cha�ne
	int end = 0;
	while (str[end]!='\0') end++;
	end--;

	// Puis remonte la cha�ne en supprimant espaces et tabulations
	while ((end>=0) && ((str[end]==' ') || (str[end]=='\t'))) end--;
	str[++end] = '\0';
}
//----------------------------------------------------------------------------//


//=== Renvoie l'entier suivant le "profilename = " ===========================//
int TINILib::GetIntProfile(char* sectionname, char* profilename, int idefault)
{	// R�cup�re la cha�ne de caract�re suivant le sous-nom de section
	char str[128];
	int hr = GetStringProfile(sectionname, profilename, str);
	if (hr==-1) return idefault;

	// Convertion de la cha�ne en entier
	int res = atoi(str);
	return res;
}
//----------------------------------------------------------------------------//

//=== Renvoie le nb flottant suivant le "profilename = " =====================//
double TINILib::GetFloatProfile(char* sectionname, char* profilename, double fdefault)
{	// R�cup�re la cha�ne de caract�re suivant le sous-nom de section
	char str[128];
	int hr = GetStringProfile(sectionname, profilename, str);
	if (hr==-1) return fdefault;

	// Convertion de la cha�ne en entier
	double res = atof(str);
	return res;
}
//----------------------------------------------------------------------------//

//= Renvoie le script suivant le "profilename = " d�limit� par des accolades =//
// ->  -1 : si pas trouv�
//      n : nb de car. contenus dans resultscript
int TINILib::GetScriptProfile(char* sectionname, char* profilename, char* resultscript)
{	int nbcar = 0;	// Nb de caract�res pr�sents ds la string renvoy�e
	int offset;
	
	// Recheche tout d'abord la ligne suivant la section recherch�e
	offset = SearchSection(sectionname);
	if (offset==-1) return -1;		// Section introuvable

	// Puis recherche pour chaque ligne suivante le profilename recherch�
	int res = 0;
	while ((offset < iSize) && (res == 0) && (offset != -1))
	{	res = SearchProfile(profilename, offset);
		if (res == -1) return -1;	// fin de fichier ou  section suivante
		
		if (res == 1)
		{	int nbOctets = 0;
			char car;

			// Parcours la ligne jusqu'au signe '='
			while (pMem[offset++]!='=');

			// Se positionne sur l'accolade ouverte de d�but de script '{'
			do 
			{	car = pMem[offset++];
			} while (car!='{');

			// Supprime les espaces et les tabs
			while ((pMem[offset]==' ') || (pMem[offset]=='\t')) offset++;
			
			// Copie de la cha�ne trouv�e jusqu'� l'accolade ferm�e '}'
			// supprime les caract�res de fin et les retour � la ligne 0x0D et 0x0A
			do
			{	car = pMem[offset++];
				
				// Si l'on est arriv� au bout du fichier, on quitte
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
		
		// Sinon on passe � la ligne suivante
		offset = GetNextLine(offset);
	}
	
	return nbcar;
}
//----------------------------------------------------------------------------//

//=== Renvoie une string se trouvant au label de tel section =================//
// @param sectioname	nom de section dans le .ini (input)
// @param profilename	label dans cette section (input)
// @param resultname	nom recherche (output)
// @param defaultname	nom par defaut renvoye si on n'a pas trouve la section courespondante (input)
// @return nb de car. contenus dans resultscript (-1 si on a pris la valeur par defaut)
int TINILib::GetStringProfile(char* sectionname, char* profilename, char* resultname, char *defaultname)
{	int res = GetStringProfile(sectionname, profilename, resultname);
	if (res==-1)
	{	strcpy(resultname, defaultname);
	}
	return res;
}
//----------------------------------------------------------------------------//

//=== Se replace au debut du fichier =========================================//
void TINILib::begin()
{	iCurseur=0;			
	iNumSection=0;
}
//----------------------------------------------------------------------------//

//=== passe a la prochaine section ===========================================//
// @param	section		nom de la section trouvee (output)
// @return	offset de la ligne suivant le nom de la section recherch�e
//			ou -1 si il n'y a plus de section
int	TINILib::getNextSection(char* section)
{	int offset = iCurseur;	// On part du curseur courant du fichier

	// On se place au debut de la section suivante
	offset = getNewSectionFrom(offset);
	if ((offset==-1) || (offset>=iSize)) goto error; 
	
	// Recupere le nom de cette section
	getSectionName(offset, section);
	if (strcmp(section,"")==0) goto error;

	// Se place sur la ligne suivante
	GetNextLine(iCurseur, offset+1);
	return iCurseur;

  error:
	section[0]='\0';
	return -1;
}

// Fin de la classe TINILIB ////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Fonctions ANNEXES                                                          //
////////////////////////////////////////////////////////////////////////////////








