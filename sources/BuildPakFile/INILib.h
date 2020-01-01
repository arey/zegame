////////////////////////////////////////////////////////////////////////////////
//  Unit�  "INILib.h"  : Librarie permettant de charger un .ini crypt� ou non //
//			 et de travailler dessus directement en m�moire       //
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
{	char*	pMem;				// Zone de m�moire o� est charg� le .ini
	int		iSize;				// Taille du fichier (et donc de pMem)


//private:	
public:
	TINILib();					// Constructeur par d�faut interdit
	int	 GetNextLine(int &index,int offset);	// Renvoie un pointeur sur le n� de la prochaine ligne rencontr�e � partir d'offset et renvoie le nb de caract�re la composant
	int  GetNextLine(int offset);				// Renvoie l'offset de la ligne suivante
	int  SearchSection(char* sectionname);		// Renvoie l'offset de la ligne suivant le nom de la section recherch�e
	int  SearchProfile(char *profilename, int offset);	// Indique si la ligne situ�e � l'offset 'offset' correspond au profile recherch�
	int	 GetStringValue(int offset, char* resultname);	// Renvoie la cha�ne de caract�re suivant le '='
	void TrimRight(char* str);							// Supprime espaces et tabulations de fin de cha�ne

public:
	TINILib(char* filename);	// Constructeur � utiliser
	virtual ~TINILib();			// Destructeur par d�faut
	
	bool LoadFile(char* filename);			// Charge un .ini en m�moire
	int  GetStringProfile(char* sectionname, char* profilename, char* resultname);	// Renvoie une cha�ne suivant le "profilename = "		
	int  GetIntProfile(char* sectionname, char* profilename, int idefault);			// Renvoie l'entier suivant le "profilename = "		
	double GetFloatProfile(char* sectionname, char* profilename, double fdefault);	// Renvoie le nb flottant suivant le "profilename = "		

};
////////////////////////////////////////////////////////////////////////////////




//----------------------------------------------------------------------------//
#endif   // INILibH
