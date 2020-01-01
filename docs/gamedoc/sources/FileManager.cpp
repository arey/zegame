////////////////////////////////////////////////////////////////////////////////
//  Unité "FileManager.cpp"													  //	                                                                            //
//	Description : Gère des fichiers virtuels présent dans un gros fichiers ou //
//                dans en mémoire.											  //
//                                                                            //
//  Copyright (C) 1999 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com	                  //
//  Version      : 1.02 	                                                  //
//  Creation     : 02/12/1999 		                                          //
//  Last upgrade : 10/12/1999                                                 //
////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------- Include
#include "INILib.h"			// Gestion des fichiers .ini
#include "FileManager.h"	// Header du fichier

//----------------------------------------------------------------------- Define

//-------------------------------------------------------------------- Variables
TFileManager *FileManager = NULL;	// Manager des fichiers du jeu


////////////////////////////////////////////////////////////////////////////////
// Classe TFILEMANAGER                                                        //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur par défaut ================================================//
TFileManager::TFileManager()
{	iLookPreference = FILEMANAGER_SEARCH_NONE;
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TFileManager::~TFileManager()
{	// On vide la liste des .pak
	for (int i=0; i < vPakFiles.size() ; i++)
	{	delete vPakFiles[i];
	}
	vPakFiles.clear();	
	// On vide le bibliothèque des fichiers virtuels
	mVirtualFiles.clear();
}
//----------------------------------------------------------------------------//

//=== Ajout d'un .pak au manager =============================================//
bool TFileManager::AddPakFile(char* name)
{	// Ouvre un .pak et l'ajoute à la liste
	TPakFile *pakFile = new TPakFile(); 
	vPakFiles.push_back(pakFile);
	bool crdu = pakFile->OpenPakFile(name);
	if (!crdu) return false;

	// Ajoute les infos contenues dans ce .pak à la liste
	AnsiString sName; 
	sVirtualFile virtualFile;
	int indice = vPakFiles.size() - 1;
	for (int i=0 ; i < pakFile->iNbZgfFile ; i++)
	{	sName = (LPCSTR) pakFile->vZgfFiles[i].sZgfName;		
		virtualFile.iZgfOffset = pakFile->vZgfFiles[i].iZgfOffset;
		virtualFile.iZgfSize = pakFile->vZgfFiles[i].iZgfSize;
		strcpy(virtualFile.sZgfName, pakFile->vZgfFiles[i].sZgfName);
		strcpy(virtualFile.sZgpName, name);
		virtualFile.iZgpIndex = indice;		
		if (pakFile->bInMemory)
		{	virtualFile.iFileType = 2;
			virtualFile.pData = pakFile->ZgpData;
			virtualFile.iDataOffset = virtualFile.iZgfOffset - pakFile->ZgpHeader.iDataOffset;
		}
		else
		{	virtualFile.iFileType = 1;
			virtualFile.pData = NULL;
		}
		mVirtualFiles.insert(std::make_pair(sName, virtualFile));
	}

	return true;
}
//----------------------------------------------------------------------------//

//=== Initialise le gestionnaire de fichiers à partir d'un .ini ==============//
int TFileManager::InitManager(char *iniName)
{	int nbPakFile = 0;
	char pakName[512];
	char sectionName[512];
	char zgpNumber[16];
	bool end;

	// Récupère les fichiers .zgp à charger dans le FileManager
	TINILib	ini(iniName);
	do
	{	// Récupère le nom du .zgp suivant
		strcpy(sectionName, "ZgpFile");
		itoa(nbPakFile+1, zgpNumber, 10);
		strcat(sectionName, zgpNumber);		
		end = ini.GetStringProfile("PakFile", sectionName, pakName) == -1;
		// Et l'ajoute au FileManager
		if (!end)
		{	nbPakFile++;
			AddPakFile(pakName);
		}
	} while (!end);

	// Recherche par où commencer à choisir lorsqu'on cherche un fichier
	ini.GetStringProfile("PakFile", "Search", sectionName);
	if (strcmp(sectionName, "PAK") == 0)
	{	iLookPreference = FILEMANAGER_SEARCH_PAK;
	} else 
	if (strcmp(sectionName, "FILES") == 0)
	{	iLookPreference = FILEMANAGER_SEARCH_FILES;
	}

	return nbPakFile;
}
//----------------------------------------------------------------------------//

//=== Renvoi la destruction du fichier que l'on recherche ====================//
sVirtualFile* TFileManager::GetFile(char* name)
{	return &mVirtualFiles[name];
}
//----------------------------------------------------------------------------//

//=== Renvoie l'attribut 'iLookPreference' ===================================//
int TFileManager::GetLookPreference()
{	return iLookPreference; 
}
//----------------------------------------------------------------------------//

//=== Recherche si le fichier 'name' se trouve dans un des .pak ==============//
// Renvoi 0 si il n'y est pas, 1 si il est sur disk et 2 si il est en mémoire
int TFileManager::IsInPakFiles(char* name)
{	std::map<AnsiString, sVirtualFile>::iterator iter; 
	iter = mVirtualFiles.find(name);
	if (iter == mVirtualFiles.end()) return 0;	// Pas trouvé
	return (*iter).second.iFileType;
}
//----------------------------------------------------------------------------//

/////////////////////// Fin de la classe TFILEMANAGER //////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Fonctions ANNEXES														  //	
////////////////////////////////////////////////////////////////////////////////

//=== Initialise le Manager des fichiers du jeu ==============================//
void InitGame_FileManager()
{	FileManager = new TFileManager();
	FileManager->InitManager("ZeGame.ini");
}
//----------------------------------------------------------------------------//

//=== Referme proprement le Manager des fichiers du jeu ======================//
void CloseGame_FileManager()
{	delete FileManager; 
	FileManager = NULL;
}
//----------------------------------------------------------------------------//
