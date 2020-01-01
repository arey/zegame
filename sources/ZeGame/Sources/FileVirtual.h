////////////////////////////////////////////////////////////////////////////////
//  Unité "FileVirtual.h"													  //	                                                                            //
//	Description : Classe TFileVirtual représentant un fichier virtuel pouvant //
//                être stocké aussi bien dans un gros fichier qu'en mémoire.   //
//                                                                            //
//  Copyright (C) 1999 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com	                  //
//  Version      : beta 0.01                                                  //
//  Creation     : 02/12/1999 		                                          //
//  Last upgrade : <none>	                                                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef FileVirtualH
#define FileVirtualH

// --------------------------------------------------------------------- Include
#ifdef VISUALC
#pragma warning(disable : 4100 4663 4018)  
#endif
#include <fstream.h>    // Gestionnaire de fichiers du C++
#include <stdio.h>		// Gestionnaire de fichiers du C

// ---------------------------------------------------------------------- Define

// ------------------------------------------------------------------------ Enum
enum 
{	FILE_VIRTUAL_NONE = 0,
	FILE_VIRTUAL_DISK = 1,
	FILE_VIRTUAL_RAM = 2
};
	

// TFILEVIRTUAL ////////////////////////////////////////////////////////////////
class TFileVirtual
{   //--------------------------------------------------- Attributs de la classe
  protected:	
	ifstream	*diskFile;			// Fichier dans lequel se trouve le fichier virtuel
	unsigned char	*ramFile;		// Adresse du buffer dans lequel se trouve le fichier virtuel
	int				iStartIndex;	// Début du fichier dans le diskFile ou le ramFile
	int				iType;			// Type de fichiers : sur disk ou en mémoire
	int				iSize;			// Taille du fichier
	int				iCursor;		// Curseur de lecture du fichier (<=> Offset)
  public:
	enum { beg=0, cur, end };
	
 	//---------------------------------------------------- Méthodes de la classe
  public:
	TFileVirtual();									// Constructeur par défaut
	virtual ~TFileVirtual();						// Destructeur
	virtual int read(void *buffer, int size) = 0;	// Lecture de 'size' octets du fichier dans buffer
	virtual int seek (int offset) = 0;				// Se positionne à tel offset dans le fichier
	virtual void seekg(int delta, int dir) = 0;		// Se déplace de delta octets par rapport à la direction
	inline int size() { return iSize; }				// Renvoi la taille du fichier
	inline int tellg() { return iCursor; }			// Renvoi la position courante où se trouve le curseur
	virtual inline bool bad() { return false; }		// Indique si il y'a eu un problème de lecture du fichier
	virtual inline bool fail() { return false; }	// Indique si il y'a eu un problème de lecture du fichier
	virtual int read32() = 0;						// Lit un double-mot de 32 bits
	virtual int read16() = 0;						// Lit un mot de 16 bits
	virtual int read8() = 0;						// Lit un octet de 8 bits
	virtual char *fgetsFV(char *buffer,int size)=0; // Lit une ligne entière
	inline ifstream* Getifstream() { return diskFile; }	// Renvoi le fichier ifstream*
	inline int GetType() { return iType; }				// Renvoi le type de fichier : disk ou ram
	inline bool eof() { return iCursor>=iSize; }		// Vrai lorsque l'on est arrivé au bout du fichier
	inline int getSizeToEnd() { return iSize - iCursor; }	// Renvoi la taille restant a lire
};
/////////////////////////// FIN de TFILEVIRTUAL ////////////////////////////////


// TDISKFILEVIRTUAL ////////////////////////////////////////////////////////////
class TDiskFileVirtual : public TFileVirtual
{   //--------------------------------------------------- Attributs de la classe
	FILE			*iodiskFile;		// Fichiers de type FILE*	
	char			ioFileName[128];	// Nom du fichier dans lequel il se trouve
	
	//---------------------------------------------------- Méthodes de la classe
  protected: 
	int Start();										// Se place au début du fichier virtuel
  
  public:
	TDiskFileVirtual(char *file, int start, int s);		// Constructeur
	~TDiskFileVirtual();								// Destructeur
	int read(void *buffer, int size);					// Lecture de 'size' octets du fichier dans buffer
	int seek (int offset);								// Se positionne à tel offset dans le fichier		
	void seekg(int delta, int dir);						// Se déplace de delta octets par rapport à la direction
	bool OpenIOFile();									// Ouvre le fichier FILE*
	inline FILE* GetioFile() { return iodiskFile; }		// Renvoie le fichier FILE*
	int read32();										// Lit un double-mot de 32 bits
	int read16();										// Lit un mot de 16 bits
	int read8();										// Lit un octet de 8 bits
	char *fgetsFV(char *buffer,int size);				// Lit une ligne entière
};
/////////////////////// FIN de TDISKFILEVIRTUAL ////////////////////////////////

/// TRAMFILEVIRTUAL ////////////////////////////////////////////////////////////
class TRamFileVirtual : public TFileVirtual
{   //--------------------------------------------------- Attributs de la classe
	
	
 	//---------------------------------------------------- Méthodes de la classe
  public:
	TRamFileVirtual(unsigned char* file, int start, int s); // Constructeur	
	~TRamFileVirtual();										// Destructeur
	int read(void *buffer, int size);						// Lecture de 'size' octets du fichier dans buffer
	int seek (int offset);									// Se positionne à tel offset dans le fichier	
	void seekg(int delta, int dir);							// Se déplace de delta octets par rapport à la direction
	int read32();											// Lit un double-mot de 32 bits
	int read16();											// Lit un mot de 16 bits
	int read8();											// Lit un octet de 8 bits
	char *fgetsFV(char *buffer,int size);					// Lit une ligne entière
};
//////////////////////// FIN de TRAMFILEVIRTUAL ////////////////////////////////

//----------------------------------------- Instance unique de la classe externe

//----------------------------------------------------------- Fonctions externes
TFileVirtual*	fopenFV(char* name);						// Ouvre un fichier virtuel
bool fcloseFV(TFileVirtual* file);							// Ferme un fichier virtuel
int  freadFV(void *buffer, int size, TFileVirtual* file);	// Lit le contenu d'un fichier virtuel dans un buffer
char *fgetsFV(char *buffer, int size, TFileVirtual* file);	// Lit une ligne complète


#endif	// FileVirtualH
