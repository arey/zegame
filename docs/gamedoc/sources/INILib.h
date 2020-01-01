////////////////////////////////////////////////////////////////////////////////
//  Unité  "INILib.h"  : Librarie permettant de charger un .ini crypté ou non //
//			 et de travailler dessus directement en mémoire       //
//                                                                            //
//  Copyright (C) 1999 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com                      //
//                                                                            //
//  Version      : beta 0.01                                                  //
//  Creation     :                                                	      //
//  Last upgrade :                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#ifndef INILibH
#define INILibH


// TINILIB /////////////////////////////////////////////////////////////////////
// Permet de charger un .ini en ram et fournit des outils pour le manipuler   //
class TINILib
{	char*	pMem;				// Zone de mémoire où est chargé le .ini
	int		iSize;				// Taille du fichier (et donc de pMem)


//private:	
public:
	TINILib();					// Constructeur par défaut interdit
	int	 GetNextLine(int &index,int offset);	// Renvoie un pointeur sur le n° de la prochaine ligne rencontrée à partir d'offset et renvoie le nb de caractère la composant
	int  GetNextLine(int offset);				// Renvoie l'offset de la ligne suivante
	int  SearchProfile(char *profilename, int offset);	// Indique si la ligne située à l'offset 'offset' correspond au profile recherché
	int	 GetStringValue(int offset, char* resultname);	// Renvoie la chaîne de caractère suivant le '='
	void TrimRight(char* str);							// Supprime espaces et tabulations de fin de chaîne

public:
	TINILib(char* filename, bool useFileManager = false);		// Constructeur à utiliser
	virtual ~TINILib();											// Destructeur par défaut
	
	bool LoadFile(char* filename, bool useFileManager = false);	// Charge un .ini en mémoire
	int  SearchSection(char* sectionname);	// Renvoie l'offset de la ligne suivant le nom de la section recherchée
	int  GetStringProfile(char* sectionname, char* profilename, char* resultname);	// Renvoie une chaîne suivant le "profilename = "		
	int  GetIntProfile(char* sectionname, char* profilename, int idefault);			// Renvoie l'entier suivant le "profilename = "		
	int  GetScriptProfile(char* sectionname, char* profilename, char* resultscript);// Renvoie le script suivant le "profilename = " délimité par des accolades
	double GetFloatProfile(char* sectionname, char* profilename, double fdefault);	// Renvoie le nb flottant suivant le "profilename = "		
};
////////////////////////////////////////////////////////////////////////////////




//----------------------------------------------------------------------------//
#endif   // INILibH
