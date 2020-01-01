////////////////////////////////////////////////////////////////////////////////
//  Unité "FileVirtual.cpp"													  //	                                                                            //
//	Description : Classe TFileVirtual représentant un fichier virtuel pouvant //
//                être stocké aussi bien dans un gros fichier qu'en mémoire.   //
//                                                                            //
//  Copyright (C) 1999 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com	                  //
//  Version      : beta 0.01                                                  //
//  Creation     : 02/12/1999 		                                          //
//  Last upgrade : <none>	                                                  //
////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------- Include
#include <memory.h>			// utilisation des fonctions : memcpy, ...
#include "FileManager.h"	// Gestionnaire de fichiers
#include "FileVirtual.h"	// Header du fichier

//----------------------------------------------------------------------- Define
#define BYTE	unsigned char
#define LONG	int
#define WORD	unsigned short int
#define DWORD   unsigned int



////////////////////////////////////////////////////////////////////////////////
// Classe TFILEVIRTUAL                                                        //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur par défaut ================================================//
TFileVirtual::TFileVirtual():
	diskFile(NULL), ramFile(NULL)
{	iStartIndex = 0;
	iCursor = 0;
	iType = FILE_VIRTUAL_NONE;
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TFileVirtual::~TFileVirtual()
{	}
//----------------------------------------------------------------------------//


/////////////////////// Fin de la classe TFILEVIRTUAL //////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Classe TDISKFILEVIRTUAL                                                    //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur ===========================================================//
TDiskFileVirtual::TDiskFileVirtual(char *file, int start, int s):
	TFileVirtual(), iodiskFile(NULL)
{	iStartIndex = start;
	iType = FILE_VIRTUAL_DISK;
	iSize = s;
	strcpy(ioFileName, file);
    #ifndef CBUILDER
	diskFile = new ifstream(file, ifstream::binary | ifstream::in | ios::nocreate, filebuf::sh_read);
    #else
	diskFile = new ifstream(file, ifstream::binary | ifstream::in);    
    #endif
	Start();		// Se place au début du fichier lors de sa création
}
//----------------------------------------------------------------------------//


//=== Destructeur ============================================================//
TDiskFileVirtual::~TDiskFileVirtual()
{	diskFile->close();
	delete diskFile;
	if (iodiskFile != NULL) fclose(iodiskFile);
}
//----------------------------------------------------------------------------//


//=== Lecture de 'size' octets du fichier dans buffer ========================//
// Contrat : ne vérifie pas si l'on est arrivé au bout du fichier
int TDiskFileVirtual::read(void *buffer, int size)
{	diskFile->read((char*) buffer, size);
	iCursor += size;
	return size;
}
//----------------------------------------------------------------------------//

//=== Se positionne à tel offset dans le fichier =============================//
int TDiskFileVirtual::seek(int offset)
{	iCursor = offset;
	diskFile->seekg(iStartIndex+iCursor);
	return 0;
}
//----------------------------------------------------------------------------//

//=== Se place au début du fichier virtuel ===================================//
int TDiskFileVirtual::Start()
{	return seek(0);
}
//----------------------------------------------------------------------------//

//=== Se déplace de delta octets par rapport à la direction ==================//
void TDiskFileVirtual::seekg(int delta, int dir)
{	// A partir ...
	switch (dir)
	{	// ... du Debut
		case beg : 
			diskFile->seekg(iStartIndex + delta);
			iCursor = delta;
			break;
		// ... de la Position Courante
		case cur : 
			diskFile->seekg(iStartIndex + iCursor + delta);
			iCursor += delta;
			break;
		// ... de la fin
		case end : 
			diskFile->seekg(iStartIndex + iSize + delta);
			iCursor = iSize + delta;
			break;
	}
}
//----------------------------------------------------------------------------//

//=== Ouvre le fichier FILE* =================================================//
bool TDiskFileVirtual::OpenIOFile()
{	if (iodiskFile != NULL) fclose(iodiskFile);
	iodiskFile = fopen(ioFileName, "rb");
	if (iodiskFile==NULL) return false;
	return true;
}
//----------------------------------------------------------------------------//

//=== Fonctions avancées de lecture ==========================================//
// - Lit un double-mot de 32 bits
int TDiskFileVirtual::read32()
{	int dw;
	diskFile->read((char*) &dw, 4);
	iCursor += 4;
	return dw;	
}
// - Lit un mot de 16 bits
int TDiskFileVirtual::read16()
{	short int w;
	diskFile->read((char*) &w, 2);
	iCursor += 2;
	return (int) w;
}
// - Lit un octet de 8 bits
int TDiskFileVirtual::read8()
{	int b = diskFile->get();
	iCursor ++;
	return b;
}
//----------------------------------------------------------------------------//

//=== Lit une ligne entière ==================================================//
char *TDiskFileVirtual::fgetsFV(char *buffer,int size)
{	// [A OPTIMISER] largement
	char car = '\0';
	int i=0;
	if (iCursor >= iSize) return NULL;
	while ((iCursor < iSize) && (car != 13) && (i<size))
	{	car = (char) diskFile->get();	
		iCursor++;
		buffer[i++] = car;
	}
	if (i<size) 
	{	buffer[i-1] = 10;	// retour chariot
		buffer[i] = '\0';	// car de fin de ligne
		diskFile->get();	// caractère 0x10 ('\n')
		iCursor++;
	}
	else buffer[size-1] = '\0';
	return buffer;
}
//----------------------------------------------------------------------------//

///////////////////// Fin de la classe TDISKFILEVIRTUAL ////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Classe TRAMFILEVIRTUAL                                                     //
////////////////////////////////////////////////////////////////////////////////

//=== Csonstructeur ==========================================================//
TRamFileVirtual::TRamFileVirtual(unsigned char* file, int start, int s):
	TFileVirtual()
{	iStartIndex = start;
	iType = FILE_VIRTUAL_RAM;
	ramFile = file;
	iSize = s;
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TRamFileVirtual::~TRamFileVirtual()
{	}
//----------------------------------------------------------------------------//

//=== Lecture de 'size' octets du fichier dans buffer ========================//
// Contrat : ne vérifie pas si l'on est arrivé au bout du fichier
int TRamFileVirtual::read(void *buffer, int size)
{	memcpy(buffer, &ramFile[iStartIndex+iCursor], size);
	iCursor += size;
	return size;
}
//----------------------------------------------------------------------------//

//=== Se positionne à tel offset dans le fichier =============================//
int TRamFileVirtual::seek(int offset)	
{	iCursor = offset;
	return 0;
}
//----------------------------------------------------------------------------//

//=== Se déplace de delta octets par rapport à la direction ==================//
void TRamFileVirtual::seekg(int delta, int dir)
{	// A partir ...
	switch (dir)
	{	// ... du Debut
		case beg : iCursor = delta;	break;
		// ... de la Position Courante
		case cur : iCursor += delta; break;
		// ... de la fin
		case end : iCursor = iSize + delta; break;
	}
}
//----------------------------------------------------------------------------//

//=== Fonctions avancées de lecture ==========================================//
// - Lit un double-mot de 32 bits
int TRamFileVirtual::read32()
{	int dw = (int) ramFile[iStartIndex+iCursor];
	iCursor += 4;
	return dw;	
}
// - Lit un mot de 16 bits
int TRamFileVirtual::read16()
{	short int w = (short int) ramFile[iStartIndex+iCursor];
	iCursor += 2;
	return (int) w;
}
// - Lit un octet de 8 bits
int TRamFileVirtual::read8()
{	char b = (char) ramFile[iStartIndex+iCursor];
	iCursor ++;
	return (int) b;
}
//----------------------------------------------------------------------------//

//=== Lit une ligne entière ==================================================//
char *TRamFileVirtual::fgetsFV(char *buffer,int size)
{	// [A OPTIMISER] largement
	char car = '\0';
	int i=0;
	if (iCursor >= iSize) return NULL;
	while ((iCursor < iSize) && (car != 13) && (i<size))
	{	car = (char) ramFile[iStartIndex + iCursor++];
		buffer[i++] = car;
	}
	if (i<size) 
	{	buffer[i-1] = 10;
		buffer[i] = '\0';
		iCursor++;		// on saute le caractère 0x10 ('\n')
	}
	else buffer[size-1] = '\0';
	return buffer;
}
//----------------------------------------------------------------------------//

///////////////////// Fin de la classe TRAMFILEVIRTUAL /////////////////////////



////////////////////////////////////////////////////////////////////////////////
// Fonctions ANNEXES														  //	
////////////////////////////////////////////////////////////////////////////////

//=== Ouvre un fichier virtuel ===============================================//
TFileVirtual* fopenFV(char* name)
{	TFileVirtual* fileVirtual = NULL;
	sVirtualFile* descFile = NULL;
	
	// Regarde par quoi commencer à rechercher
	if (FileManager->GetLookPreference() == FILEMANAGER_SEARCH_PAK)
	{	// On commence par chercher dans les .pak
		int where = FileManager->IsInPakFiles(name);
		// Si il est inclus dans l'un des .pak du jeu
		if (where > 0)
		{	// Récupère la description du fichier virtuel
			descFile = FileManager->GetFile(name);	
			
			// - Cas d'un fichier Disk
			if (descFile->iFileType == 1)	 
			{	// On ouvre le fichier .zgp et on se place au début du sous-fichier
				fileVirtual = new TDiskFileVirtual(descFile->sZgpName, 
													   descFile->iZgfOffset, 
													   descFile->iZgfSize);
			} else 
			// - Cas d'un fichier en mémoire
			if (descFile->iFileType == 2)
			{	fileVirtual = new TRamFileVirtual(descFile->pData, 
													   descFile->iDataOffset, 
													   descFile->iZgfSize);
			}
		} else
		{	// Il faut regarder si il est pas quand même sur le disk
            #ifdef CBUILDER
			ifstream existFile(name, ifstream::in);
            #else
			ifstream existFile(name, ifstream::in | ios::nocreate);
            #endif
			if (existFile.is_open())
			{	existFile.seekg(0 ,ifstream::end);
				int size = existFile.tellg();
				existFile.close();
				fileVirtual = new TDiskFileVirtual(name, 0, size);
			}
		}
	} else 
	if (FileManager->GetLookPreference() == FILEMANAGER_SEARCH_FILES)
	{	// On commence d'abord par le chercher sur le disk
        #ifdef CBUILDER
		ifstream existFile(name, ifstream::in);
        #else
		ifstream existFile(name, ifstream::in | ios::nocreate);
        #endif
		if (existFile.is_open())
		{	// Fichier présent sur le disque
			existFile.seekg(0 ,ifstream::end);
			int size = existFile.tellg();
			existFile.close();
			fileVirtual = new TDiskFileVirtual(name, 0, size);
		} else
		{	existFile.close();
			// On regarde si il n'est pas dans un .pak
			int where = FileManager->IsInPakFiles(name);
			// Si il est inclus dans l'un des .pak du jeu
			if (where > 0)
			{	// Récupère la description du fichier virtuel
				descFile = FileManager->GetFile(name);	
				
				// - Cas d'un fichier Disk
				if (descFile->iFileType == 1)	 
				{	// On ouvre le fichier .zgp et on se place au début du sous-fichier
					fileVirtual = new TDiskFileVirtual(descFile->sZgpName, 
													   descFile->iZgfOffset, 
													   descFile->iZgfSize);
				} else 
				// - Cas d'un fichier en mémoire
				if (descFile->iFileType == 2)
				{	fileVirtual = new TRamFileVirtual(descFile->pData, 
													  descFile->iDataOffset, 
													  descFile->iZgfSize);
				}
			}
		}
	}

	return fileVirtual;
}
//----------------------------------------------------------------------------//

//=== Ferme un fichier virtuel ===============================================//
bool fcloseFV(TFileVirtual* file)
{	if (file == NULL) return true;
	// On supprime tout simplement l'instance du fichier
	delete file;
	file = NULL;
	return true;
}
//----------------------------------------------------------------------------//

//=== Lit le contenu d'un fichier virtuel dans un buffer =====================//
int freadFV(void *buffer, int size, TFileVirtual* file)
{	return file->read(buffer, size);
}
//----------------------------------------------------------------------------//

//=== Lit une ligne complète =================================================//
char *fgetsFV(char *buffer, int size, TFileVirtual* file)
{	return file->fgetsFV(buffer, size);
}
//----------------------------------------------------------------------------//

