////////////////////////////////////////////////////////////////////////////////
//  Unité "FileManager.h"													  //	                                                                            //
//	Description : Gère des fichiers virtuels présent dans un gros fichiers ou //
//                dans en mémoire.											  //
//                                                                            //
//  Copyright (C) 1999 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com	                  //
//  Version      : 1.02 	                                                  //
//  Creation     : 02/12/1999 		                                          //
//  Last upgrade : 10/12/1999                                                 //
////////////////////////////////////////////////////////////////////////////////


#ifndef FileManagerH
#define FileManagerH

//---------------------------------------------------------------------- Include
#ifdef VISUALC
#pragma warning(disable : 4100 4663 4018 4786 4512)  
#endif
#include <fstream.h>    // Gestionnaire de fichiers du C++
#include <map>			// Bibliothèque de la STL
#include "ZeNain.h"		// Commun au projet
#include "PakFile.h"	// Gestion des fichiers zgp
#include <vcl.h>		// AnsiString
//----------------------------------------------------------------------- Define


//------------------------------------------------------------------------- enum
enum
{	FILEMANAGER_SEARCH_NONE = 0,
	FILEMANAGER_SEARCH_PAK,
	FILEMANAGER_SEARCH_FILES
};

// TFILEMANAGER ////////////////////////////////////////////////////////////////
class TFileManager
{   //--------------------------------------------------- Attributs de la classe
	std::map<AnsiString, sVirtualFile>  mVirtualFiles;		// Fichiers contenus dans des .pak	<FileName, PakFileIndex> 
	std::vector<TPakFile*> vPakFiles;						// Liste des multiples .pak pouvant être gérés par le jeu
	int iLookPreference;									// Endroit à privilégier pour la recherche des fichiers

 	//---------------------------------------------------- Méthodes de la classe
  public:
	 TFileManager();				// Constructeur par défaut
	~TFileManager();				// Destructeur
	bool AddPakFile(char* name);	// Ajout d'un .pak au manager
	int InitManager(char *iniName);	// Initialise le gestionnaire de fichiers à partir d'un .ini
	int IsInPakFiles(char* name);	// Recherche si le fichier 'name' se trouve dans un des .pak
	int GetLookPreference();		// Renvoie l'attribut 'iLookPreference'
	sVirtualFile* GetFile(char* name);	// Renvoi la destruction du fichier que l'on recherche
};
/////////////////////////// FIN de TFILEMANAGER ////////////////////////////////

//----------------------------------------- Instance unique de la classe externe
extern TFileManager *FileManager;	// Manager des fichiers du jeu

//----------------------------------------------------------- Fonctions externes
void InitGame_FileManager();		// Initialise le Manager des fichiers du jeu
void CloseGame_FileManager();		// Referme proprement le Manager des fichiers du jeu


#endif	// FileManagerH
