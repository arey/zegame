////////////////////////////////////////////////////////////////////////////////
//  Unité "PakFile.h"													      //	                                                                            //
//	Description : Classe TPakFile décrivant la composition d'un gros fichier  //
//                ".zgp" regroupant plein de fichiers de divers formats.      //
//                                                                            //
//  Copyright (C) 1999 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com	                  //
//  Version      : beta 0.01                                                  //
//  Creation     : 06/12/1999 		                                          //
//  Last upgrade : <none>	                                                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef PakFileH
#define PakFileH

//---------------------------------------------------------------------- Include
#ifdef VISUALC
#pragma warning(disable : 4100 4663 4018)
#endif
#include <fstream.h>    // Gestionnaire de fichiers du C++
#include <vector>		// Tableaux dynamiques de la STL 

//----------------------------------------------------------------------- Define
#define ZGP_FILE_MAX_CHAR			128
#define	ZGP_FILE_VERSION			1.1
#define ZGP_FILE_CRYPT_NONE			  0
#define ZGP_FILE_CRYPT_HEADER_128	  1

#define ZGP_MAX_FILTER		 10
#define ZGP_MAX_EXTENSION	 5


//----------------------------------------------------------------------- Struct

// === Recherche récursive des fichiers à inclure dans le .zgp ===
struct sZgpSearchFolder
{	char sFilterIn[ZGP_MAX_EXTENSION][ZGP_MAX_FILTER];
	char sFilterOut[ZGP_MAX_EXTENSION][ZGP_MAX_FILTER];
	int	 iNbFilterIn; 
	int	 iNbFilterOut;
};

// === En-Tête du fichier .zpg ===
struct sZgpHeader
{	char  ZgpSignature[4];	// Signature du fichier ZGP
   	float fZgpVersion;		// Version du fichier
    int   iCryptVersion;	// Comment le fichier a été crypté 
    int   iZgpSize;			// Taille du fichier .zgp
	int	  iDataOffset;		// Adresse du début des données
    int   iNbFile;			// Nb de fichiers contenus ds les data
};

// === Fichiers contenus dans le .zgp ===
struct sZgfFile
{	char sZgfName[ZGP_FILE_MAX_CHAR];		// Sous-Rep + Nom du fichier .zgf
	int	 iZgfSize;							// Taille de ce fichier (en octets)
    int	 iZgfOffset;						// Offset du début de ce fichier dans le .zgp
	int	 iFileType;							// Type du fichier : sur disk ou en mémoire
};

// === Fichiers virtuels contenus dans la bibliothèque ===
struct sVirtualFile : public sZgfFile
{	char sZgpName[ZGP_FILE_MAX_CHAR];		// Sous-Rep + Nom du fichier .zgp
	unsigned char* pData;					// Pointe sur les data du .zgp
	int iDataOffset;						// Offset à partir duquel ce fichier se trouve ds le buffer 
	int iZgpIndex;							// Index du PakFile
};


// TPAKFILE ////////////////////////////////////////////////////////////////////
class TPakFile
{   //--------------------------------------------------- Attributs de la classe
  private:
    sZgpHeader ZgpHeader;				// En-tête du fichier .zgp
	std::vector<sZgfFile> vZgfFiles;	// Liste des fichiers contenus dans le .zgp
	int iNbZgfFile;
    
	unsigned char*	ZgpData;			// Data du fichier	(pas souvent utilisées)
	int iDataSize;						// Taille des data du .zgp
	bool bInMemory;						// Vrai si le fichier est chargé en mémoire

	char ZgpName[ZGP_FILE_MAX_CHAR];	// Chemin + nom du fichier .pak
	ifstream	*ZgpFile;				// Fichier ouvert

	friend class TFileManager;			// Déclaration d'une classe amie

 	//---------------------------------------------------- Méthodes de la classe
  private:
	bool AddZgfFile(char* name);		// Ajoute un fichier .zgf dans le gros fichier	
	int GetExtensions(char* temp, sZgpSearchFolder &search, bool bIn = true);	// Récupère les extensions de fichiers
	void AddFolders(char* path, sZgpSearchFolder &search, bool recurs);			// Ajoute le contenu d'un répertoire au .zgp
	bool IsGoodFile(char *name, sZgpSearchFolder &search);						// Renvoi vrai si le fichier doit être inclu dans le .zgp
	void GetExension(char *name, char* extens);									// Renvoi l'extension d'un fichier
	
  public:
  	TPakFile();     				// Constructeur par défaut
    ~TPakFile();					// Destructeur
    void PakWrite(char* name);		// Sauve le fichier .zgp
	void CreateZgp(char* iniName);	// Crée le big fichier
	bool OpenPakFile(char* name);	// Ouvre un .zgp
	void LoadPakInMemory();			// Charge les data du .pak en mémoire
	void UnLoadPakFromMemory();		// Libère les données chargées en mémoire
    void Clear();					// Vide le .pak
};
////////////////////////////// FIN de TPAKFILE /////////////////////////////////



//----------------------------------------- Instance unique de la classe externe

//----------------------------------------------------------- Fonctions externes



#endif	// PakFileH
