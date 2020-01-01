////////////////////////////////////////////////////////////////////////////////
//  Unité "PakFile.cpp" 												      //	                                                                            //
//	Description : Classe TPakFile décrivant la composition d'un gros fichier  //
//                ".zgp" regroupant plein de fichiers de divers formats.      //
//                                                                            //
//  Copyright (C) 1999 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com	                  //
//  Version      : beta 0.01                                                  //
//  Creation     : 06/12/1999 		                                          //
//  Last upgrade : <none>	                                                  //
////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------- Include
#include "ZeNain.h"		// Pour utiliser les manipulations de fichiers de windows
#include "IniLib.h"		// Gestion des fichiers .ini
#include "PakFile.h"	// Header de la classe

//----------------------------------------------------------------------- Define
#define BYTE	unsigned char
#define LONG	int
#define WORD	unsigned short int
#define DWORD   unsigned long


////////////////////////////////////////////////////////////////////////////////
// Classe TPAKFILE                                                            //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur par défaut ================================================//
TPakFile::TPakFile()
{   // Prépare l'en-tête du fichier
	strcpy(ZgpHeader.ZgpSignature, "ZGP");
	ZgpHeader.fZgpVersion = (float) ZGP_FILE_VERSION;
	ZgpHeader.iCryptVersion = ZGP_FILE_CRYPT_HEADER_128;
	ZgpHeader.iZgpSize = 0;
    ZgpHeader.iDataOffset = sizeof(sZgpHeader) + 0;
    ZgpHeader.iNbFile = 0;
    // Aucune donnée pour l'instant
    ZgpData = NULL;
	iDataSize = 0;
	ZgpFile = NULL;
	bInMemory = false;
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TPakFile::~TPakFile()
{	Clear();
}
//----------------------------------------------------------------------------//

//=== Vide le .pak ===========================================================//
void TPakFile::Clear()
{	delete ZgpData; ZgpData = NULL;
	if (ZgpFile!=NULL) ZgpFile->close();
	delete ZgpFile; ZgpFile = NULL;
	vZgfFiles.clear();
}
//----------------------------------------------------------------------------//

//=== Nom du fichier .zgp en sortie ==========================================//
void TPakFile::PakWrite(char* name)
{	int i;
	ofstream file(name, ofstream::binary);

	// 1) Ecriture de l'en-tête du fichier
    ZgpHeader.iNbFile = vZgfFiles.size();
    ZgpHeader.iDataOffset = sizeof(sZgpHeader) + ZgpHeader.iNbFile*sizeof(sZgfFile);
    ZgpHeader.iZgpSize = ZgpHeader.iDataOffset;
    for (i=0; i < ZgpHeader.iNbFile ; i++)
    {	ZgpHeader.iZgpSize += vZgfFiles[i].iZgfSize;
    }
	file.write((char*) &ZgpHeader, sizeof(ZgpHeader));

    // 2) Ecriture du Space-File
    int offset = ZgpHeader.iDataOffset;
    unsigned char* buffer = new unsigned char[sizeof(sZgfFile)];	// Sert uniquement pour décypter le header
    for (i=0; i < ZgpHeader.iNbFile ; i++)
    {	vZgfFiles[i].iZgfOffset = offset;
    	offset += vZgfFiles[i].iZgfSize;

        // Suivant l'algo de cryptage, on encode les données
        if (ZgpHeader.iCryptVersion == ZGP_FILE_CRYPT_NONE)
        {	file.write((char*) &vZgfFiles[i], sizeof(sZgfFile));
        }
	    else if (ZgpHeader.iCryptVersion == ZGP_FILE_CRYPT_HEADER_128)
        {	memcpy((char*) buffer, (char*) &vZgfFiles[i], sizeof(sZgfFile));
            for (int i=0 ; i < sizeof(sZgfFile) ; i++)
            {	buffer[i] += (unsigned char) 128;
            }
	        file.write((char*) buffer, sizeof(sZgfFile));
        }
    }
    delete buffer; buffer = NULL;

    // 3) Ecriture des données
    for (i=0; i < ZgpHeader.iNbFile ; i++)
    {	// Ouvre le fichier à insérer dans le .pak
		ifstream fileIn(vZgfFiles[i].sZgfName, ifstream::binary);
		// Le lit dans un buffer
		char *buffer = new char[vZgfFiles[i].iZgfSize];
		fileIn.read(buffer, vZgfFiles[i].iZgfSize);
		// Et copie ce buffer à la suite du .pak
		file.write(buffer, vZgfFiles[i].iZgfSize);
		delete buffer;
    }

	file.close();	// Referme le .pak
}
//----------------------------------------------------------------------------//

//=== Ajoute un fichier .zgf dans le gros fichier ============================//
bool TPakFile::AddZgfFile(char* name)
{	// Vérifie si le fichier existe
	#ifdef VISUALC
	ifstream file(name, ofstream::binary | ofstream::in | ios::nocreate);
    #else
    ifstream file(name, ofstream::binary | ofstream::in);
    #endif
	if (!file.good()) return false;
	

	// Récupère la taille du fichier
	file.seekg(0, ifstream::end);
	int size = file.tellg();

	// Ajout d'un .Zgf à la liste
	vZgfFiles.push_back(sZgfFile());
	iNbZgfFile = vZgfFiles.size();
	memset(vZgfFiles[iNbZgfFile-1].sZgfName, 0, sizeof(vZgfFiles[iNbZgfFile-1]));
	strcpy(vZgfFiles[iNbZgfFile-1].sZgfName, name);
	vZgfFiles[iNbZgfFile-1].iZgfSize = size;
	
	ZgpHeader.iNbFile ++;
	iDataSize += size;				// Maj de la longueur des data
	file.close();
	return true;
}
//----------------------------------------------------------------------------//

//=== Crée le big fichier .zgp ===============================================//
void TPakFile::CreateZgp(char* iniName)
{	
	char infos[512];
	char temp[512];
	
	// 1) Ouvre .zgp
	TINILib ini(iniName);

	// 2) Récupère les infos d'en-tête du .ini    
	double version = ini.GetFloatProfile("HEADER", "Version", 0);
    ZgpHeader.fZgpVersion = (float) version;
    int cryptage = ini.GetIntProfile("HEADER", "Cryptage", 0);
    ZgpHeader.iCryptVersion = cryptage;

	// 3) Lit chaque filtre
	int numFiltre = 0;
	bool endOfScript = false;
	do
	{	itoa(numFiltre, temp, 10);
		strcpy(infos,"FILTER_");
		strcat(infos,temp);
		endOfScript = (ini.SearchSection(infos) == -1);

		// Regarde si l'on a affaire à un fichier ou à un répertoire
		if (!endOfScript)
		{	ini.GetStringProfile(infos, "Type", temp);
			if (strcmp(temp, "FILE") == 0)
			{	ini.GetStringProfile(infos, "File", temp);
				AddZgfFile(temp);
			} else
			if (strcmp(temp, "FOLDER") == 0)
			{	sZgpSearchFolder search;

				// - on regarde si la recherche doit se faire récursivement ou non
				bool recursif = false;
				ini.GetStringProfile(infos, "Recursive", temp);
				if (strcmp(temp, "Yes") == 0) recursif = true;

				// - récupère les extensions des fichiers à inclure du .zgp
              	ini.GetStringProfile(infos, "FilterIN", temp);
				search.iNbFilterIn = GetExtensions(temp, search, true);

				// - récupère les extensions des fichiers à exclure du .zgp
                temp[0] = '\0';
				ini.GetStringProfile(infos, "FilterOUT", temp);
				search.iNbFilterOut = GetExtensions(temp, search, false);
				
				ini.GetStringProfile(infos, "Folder", temp);
				AddFolders(temp, search, recursif);
			}
		}
		numFiltre++;
	} while (!endOfScript);
}
//----------------------------------------------------------------------------//

//=== Récupère les extensions de fichiers ====================================//
int TPakFile::GetExtensions(char* temp, sZgpSearchFolder &search, bool bIn)
{	int nbExtens = 0;
	int cursor = 0;
	int i = 0;
	char c;
	while ((c=temp[i++]) != '\0')
	{
		if (c == ',')
		{	if (bIn) search.sFilterIn[nbExtens][cursor] = '\0';
			else search.sFilterOut[nbExtens][cursor] = '\0';
			nbExtens++;
			cursor = 0;
		}
		else if ( (c!='\t') && (c!=' '))
		{	if (bIn) search.sFilterIn[nbExtens][cursor++] = c;
			else	 search.sFilterOut[nbExtens][cursor++] = c;
			if (temp[i] == '\0')
			{	if (bIn) search.sFilterIn[nbExtens][cursor] = '\0';
				else search.sFilterOut[nbExtens][cursor] = '\0';
				nbExtens++;
			}
		}
	}	

	if (bIn) search.iNbFilterIn = nbExtens;
	else	 search.iNbFilterOut = nbExtens;
	return nbExtens;
}
//----------------------------------------------------------------------------//

//=== Ajoute le contenu d'un répertoire au .zgp =============================//
// Fonction récursive pouvant parcourir toute l'arborescence du rep. de départ
void TPakFile::AddFolders(char* path, sZgpSearchFolder &search, bool recurs)
{	WIN32_FIND_DATA data ;
	BOOL res;
	HANDLE handle;

	// Récupère localement le chemin dee travail
	char chemCourant[512];
	strcpy(chemCourant, path);

	// Recherche tous les sous-répertoires de ce chemin (si on est en recursif)
	if (recurs)
	{
		char searchRep[512];
		strcpy(searchRep, chemCourant);
		strcat(searchRep, "*.");
		handle = FindFirstFile(searchRep, &data);
		if (handle != INVALID_HANDLE_VALUE)
		{	do
			{	// Vérifie si c'est bien un répertoire
				if ((strcmp(data.cFileName,".")!=0) && (strcmp(data.cFileName,"..")!=0))
				{	char chemComplete[512];
					strcpy(chemComplete, chemCourant);
					strcat(chemComplete, data.cFileName);
					int rep = GetFileAttributes(chemComplete);
					if (rep==FILE_ATTRIBUTE_DIRECTORY)
					{  // sous-répertoire trouvé que l'on va de nouveau inspecter
						strcat(chemComplete, "\\");
						AddFolders(chemComplete, search, true);
					}
				}
				res = FindNextFile(handle,&data); 
			}
			while (res);
		}
	}

	// Charge ensuite tous les fichiers de ce répertoire
	char searchAll[512];
	char fileName[512];
	strcpy(searchAll, chemCourant);
	strcat(searchAll, "*.*");
	handle = FindFirstFile(searchAll, &data);
	if (handle != INVALID_HANDLE_VALUE)
	{	do
		{	if ((strcmp(data.cFileName,".")!=0) && (strcmp(data.cFileName,"..")!=0))
			{	if (IsGoodFile(data.cFileName, search))
				{	// Initialise le chemin + nom du fichier et l'ajoute au .zgp
					strcpy(fileName, chemCourant);
					strcat(fileName,data.cFileName) ;
					AddZgfFile(fileName);
				}
			}
						
			res = FindNextFile(handle,&data) ;
		}
		while (res == TRUE) ;
		FindClose(handle) ;
	}

}
//----------------------------------------------------------------------------//


//=== Renvoi vrai si le fichier doit être inclu dans le .zgp =================//
bool TPakFile::IsGoodFile(char *name, sZgpSearchFolder &search)
{	bool inclu = true;
	int i;
	
	// Extrait l'extension du fichier
	char extend[ZGP_MAX_EXTENSION];
	GetExension(name, extend);
	
	// Vérifie si l'extension du fichier n'est pas à exclure
	char extend2[ZGP_MAX_EXTENSION];
	char *extendMin;
	for (i=0; i < search.iNbFilterOut ; i++)
	{	strcpy(extend2, &search.sFilterOut[i][0]);
		extendMin = strlwr(extend2);	// conversion en minuscule
		if (strcmp(extend, extendMin) == 0) return false;
	}

	// Vérifie si le fichier peut-être inclus
	inclu = false;
	for (i=0; i < search.iNbFilterIn ; i++)
	{	strcpy(extend2, &search.sFilterIn[i][0]);
		extendMin = strlwr(extend2);	// conversion en minuscule
		if (strcmp(extend, extendMin) == 0) return true;
		else if (strcmp("*", extendMin) == 0) return true;
	}


	return inclu;
}
//----------------------------------------------------------------------------//

//=== Renvoi l'extension d'un fichier ========================================//
void TPakFile::GetExension(char *name, char *extens)
{	int posPoint=-1;
	
	// Recherche du dernier caractère '.' de la chaîne
	for (unsigned int i=0 ; i < strlen(name) ; i++) 
	{	if (name[i]=='.') posPoint = i;
	}
	
	// Puis complète l'extension
	if (posPoint==-1) extens[0] ='\0'; 
	strcpy(extens, &name[posPoint+1]);
	extens = strlwr(extens);	// conversion en minuscule
}
//----------------------------------------------------------------------------//

//=== Ouvre un .zgp ==========================================================//
bool TPakFile::OpenPakFile(char* name)
{	// Récupère les infos concernant ce fichier
	// - son Nom
	strcpy(ZgpName, name);
	// - son Handle
    #ifdef VISUALC
	ZgpFile = new ifstream(name, ifstream::binary | ifstream::in | ios::nocreate, filebuf::sh_read);
    #else
   	ZgpFile = new ifstream(name, ifstream::binary | ifstream::in);
    #endif
	if (ZgpFile->bad()) return false;

	// - Lecture de l'en-tête du fichier
    ZgpFile->read((char*) &ZgpHeader, sizeof(ZgpHeader));
    iNbZgfFile = ZgpHeader.iNbFile;
	// - Lecture du Space-File
    unsigned char* buffer = new unsigned char[sizeof(sZgfFile)];	// Sert uniquement pour décypter le header
    for (int i=0; i < ZgpHeader.iNbFile ; i++)
    {   vZgfFiles.push_back(sZgfFile());
        // Lit le Space-File et le décrypte si il y'a besoin
        if (ZgpHeader.iCryptVersion == ZGP_FILE_CRYPT_NONE)
        {   // Pas de cryptage -> lecture directe
        	ZgpFile->read((char*) &vZgfFiles[i], sizeof(sZgfFile));
        } else
        if (ZgpHeader.iCryptVersion == ZGP_FILE_CRYPT_HEADER_128)
        {   // Cryptage -> inversion des caractères en leur rajoutant +128
        	ZgpFile->read((char*) buffer, sizeof(sZgfFile));
            for (int j=0 ; j < sizeof(sZgfFile) ; j++)
            {	buffer[j] += (unsigned char) 128;
            }
            memcpy((char*) &vZgfFiles[i], (char*) buffer, sizeof(sZgfFile));
        }
		iDataSize += vZgfFiles[i].iZgfSize;
    }
    delete buffer; buffer = NULL;

	ZgpFile->seekg(0);	// On se replace au début du fichier
//	LoadPakInMemory();
	return true;
}
//----------------------------------------------------------------------------//

//=== Charge les data du .pak en mémoire =====================================//
void TPakFile::LoadPakInMemory()
{	// Désalloue les données si y'en avait de chargées
	if (ZgpData!=NULL) delete ZgpData;
	// Puis on alloue la taille nécessaire au chargement des fichiers
	ZgpData = new unsigned char[iDataSize];
	// Et l'on charge les données
	ZgpFile->seekg(ZgpHeader.iDataOffset);
	ZgpFile->read(ZgpData, iDataSize);
	bInMemory = true;
}
//----------------------------------------------------------------------------//

//=== Libère les données chargées en mémoire =================================//
void TPakFile::UnLoadPakFromMemory()
{	// Désalloue les données si y'en avait de chargées
	if (ZgpData!=NULL) delete ZgpData;
	bInMemory = false;
}
//----------------------------------------------------------------------------//


///////////////////////// Fin de la classe TPAKFILE ////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Fonctions ANNEXES														  //
////////////////////////////////////////////////////////////////////////////////

