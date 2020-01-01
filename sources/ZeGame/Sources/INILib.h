////////////////////////////////////////////////////////////////////////////////
//  Unité  "INILib.h"  : Librarie permettant de charger un .ini crypté ou non //
//			 et de travailler dessus directement en mémoire					  //
//                                                                            //
//  Copyright (C) 1999 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com                      //
//                                                                            //
//  Version       :  v1.20	                                                  //
//	Last modified :	03/03/2001												  //
//	Changes :																  //
//		* 1.2 : chargement d'un .ini directement a partir d'un tampon memoire //
//														                      //
////////////////////////////////////////////////////////////////////////////////
#ifndef INILibH
#define INILibH


// TINILIB /////////////////////////////////////////////////////////////////////
// Permet de charger un .ini en ram et fournit des outils pour le manipuler   //
class TINILib
{	char*	pMem;				// Zone de mémoire où est chargé le .ini
	int		iSize;				// Taille du fichier (et donc de pMem)
	
	// Parametres utilises lorsque l'on parcours le .ini a l'aveuglette
	int		iCurseur;			// Pointe sur le caractere courant
	int		iNumSection;		// Numero de la section en cours


private:	
	int	getNewSectionFrom(int offset);				// Se place au debut de la section suivant cet @offset
	void getSectionName(int offset, char* name);	// Renvoi le nom de la section demarrant a tel @offset

public:
	TINILib();					// Constructeur par défaut interdit
	int	 GetNextLine(int &index,int offset);	// Renvoie un pointeur sur le n° de la prochaine ligne rencontrée à partir d'offset et renvoie le nb de caractère la composant
	int  GetNextLine(int offset);				// Renvoie l'offset de la ligne suivante
	int  SearchProfile(char *profilename, int offset);	// Indique si la ligne située à l'offset 'offset' correspond au profile recherché
	int	 GetStringValue(int offset, char* resultname);	// Renvoie la chaîne de caractère suivant le '='
	void TrimRight(char* str);							// Supprime espaces et tabulations de fin de chaîne

public:
	TINILib(char* filename, bool useFileManager = false);		// Constructeur à utiliser pour charger un .ini a partir d'un fichier
	TINILib(char* buffer, int size);							//		"											"	   "  buffer en memoire
	virtual ~TINILib();											// Destructeur par défaut
	
	bool LoadFile(char* filename, bool useFileManager = false);	// Charge un .ini en mémoire
	bool LoadFileFromMem(char* buffer, int size);
	int  SearchSection(char* sectionname);						// Renvoie l'offset de la ligne suivant le nom de la section recherchée
	int  GetStringProfile(char* sectionname, char* profilename, char* resultname);	// Renvoie une chaîne suivant le "profilename = "		
	int  GetStringProfile(char* sectionname, char* profilename, char* resultname, char *defaultname);	
	int  GetIntProfile(char* sectionname, char* profilename, int idefault);			// Renvoie l'entier suivant le "profilename = "		
	int  GetScriptProfile(char* sectionname, char* profilename, char* resultscript);// Renvoie le script suivant le "profilename = " délimité par des accolades
	double GetFloatProfile(char* sectionname, char* profilename, double fdefault);	// Renvoie le nb flottant suivant le "profilename = "		

	// Fonctions a utiliser pour parcourir le ini
	void begin();							// se remet au debut du fichier					
	int	 getNextSection(char* section);		// passe a la prochaine sectio (-1 si y'en a plus)

};
////////////////////////////////////////////////////////////////////////////////




//----------------------------------------------------------------------------//
#endif   // INILibH
