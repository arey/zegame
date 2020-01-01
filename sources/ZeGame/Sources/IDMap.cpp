////////////////////////////////////////////////////////////////////////////////
// IDMap.cpp : permet de g�rer la gestion de tous les IDENTIFICATEURS du jeu, //
//	  		   utilis�s dans les divers �diteurs.							  //
//                                                                            //
// Version : 1.2															  //			
//  - 22/07/99 adaptation de la librairie pour Visual C++					  //
//	- 21/12/99 encodage et lecture � partir des fichiers virtuels			  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------- include 
#include "StdAfx.h"         //--- Special for Visual C++ 6 MFC ---

#ifndef MICROSOFT
#include <vcl\system.hpp>   // pour la manipulation des fichiers
#else
#pragma warning(disable : 4100 4663 4018 4786)  
#endif
#ifdef NEW_FILE_MANAGER
#include "FileVirtual.h"	// Fichiers virtuels (dans un .pak, en m�moire ou tout simplement sur le disk)
#endif // NEW_FILE_MANAGER
#include "IDMap.h"          // interface de la classe

using namespace std;

//---------------------------------------------------------------------------
//=== Permet de mettre dans le dictionnaire un fichier contenant les noms ====//
//=== des identificateurs et leur num�ro associ�						  ====//
void IDMap::LireFileID(CString IDfichier)
{
    int ComptLine = 1;  		// Compte le nombre de ligne lue
	const int MaxLine = 1024;	// Nb maximal de caract�res / ligne
    char BufLine[MaxLine];		// Buffer de lecture d'une ligne du fichier

    // Ouvre le fichier
	#ifndef NEW_FILE_MANAGER
	FILE *f;
    if ((f = fopen(LPCSTR(IDfichier),"r"))==NULL)
	{	MessageBoxA(NULL,"FICHIER MANQUANT :",LPCSTR(IDfichier),1);
	}      
	#else
	TFileVirtual *f = fopenFV((char*) LPCSTR(IDfichier));
	if (f == NULL) MessageBoxA(NULL,"FICHIER MANQUANT :",LPCSTR(IDfichier),1);
	#endif // NEW_FILE_MANAGER

    // Scrute le fichier ligne par ligne
	#ifndef NEW_FILE_MANAGER
	while (fgets(BufLine, MaxLine, f) != NULL)  // tant que le fichier n'est pas fini
	#else
	while (fgetsFV(BufLine, MaxLine, f) != NULL)
	#endif // NEW_FILE_MANAGER
    {
        CString cle;             // cle de la map sous forme de cha�nes
        CString sattribut;	// attribut de la map
        int attribut;

        int iter = 0;			// d�but de ligne

	  	if (BufLine[0]=='\n') goto _FinDeLigne; 				  		// Ligne vide
    	if ((BufLine[0]=='/') && (BufLine[1]=='/')) goto _FinDeLigne;	// Commentaires
        // Pour chaque ligne, on va lire : l'entier & la cha�ne de caract�res s�par�s
        // par des espaces

        // saute les espaces de d�but de ligne
        while ((BufLine[iter]==' ') || (BufLine[iter]=='\t'))  iter++;

        // On lit le nom de l'identificateur
        while ((BufLine[iter]!=' ') && (BufLine[iter]!='\n') && (BufLine[iter]!='\t'))
         {
           if (BufLine[iter]=='\n')
              { CString ligne = "Ligne : " + ComptLine;
                MessageBoxA(NULL,"fichier Identificateur corrompu",LPCSTR(ligne),1);
              }
           char a = BufLine[iter];
		   cle += a;    // ajout d'un caract�re
		   iter++;		// un caract�re de plus
         };
        // saute les espaces de d�but de ligne
        while ((BufLine[iter]==' ') || (BufLine[iter]=='\t'))  iter++;

        // On lit le chiffre
        while ((BufLine[iter]!=' ') && (BufLine[iter]!='\n') && (BufLine[iter]!='\t'))
         {
           char a = BufLine[iter];
		   sattribut += a;    // ajout d'un caract�re
		   iter++;		      // un caract�re de plus
         };

        // transforme la cha�ne de caract�re en entier
//        MessageBoxA(NULL,"Ligne n�",cle.c_str(),0);
//        MessageBoxA(NULL,"Ligne n�",sattribut.c_str(),0);
        attribut = atoi(LPCSTR(sattribut));

        // Ajoute � la map une paire <CString,int>
        AjoutID(cle, attribut);

       _FinDeLigne:						// Ligne suivante
        ComptLine++;                    // une ligne de plus
    };
	
	#ifndef NEW_FILE_MANAGER
	fclose(f);
	#else
	fcloseFV(f);
	#endif // NEW_FILE_MANAGER
};
//----------------------------------------------------------------------------//

//== Ajoute un �l�ment � la map ==============================================//
// => Si la clef n'existe pas !
void IDMap::AjoutID(CString cle, int attribut)
{
    MapID.insert(make_pair(cle, attribut));
}
//----------------------------------------------------------------------------//

//== Ajout d'un �l�ment � la map =============================================//
// => en �crasant l'attribut des clefs d�j� existante
void IDMap::EcraseID(CString cle, int attribut)
{
    MapID[cle] = attribut;
}
//----------------------------------------------------------------------------//

// Recherche si la clef est d�j� utilis�e dans la map ========================//
bool IDMap::IsCle(CString cle)
{
    Iterat t = MapID.find(cle);
    return (t != MapID.end());		// vrai si la clef a �t� trouv�e
}
//----------------------------------------------------------------------------//

// Recherche si l'�l�ment "int" existe dans la map ===========================//
bool IDMap::IsElement(int element)
{   // scrute toute la map
    bool trouve = false;
    for (Iterat iter = MapID.begin(); (iter!=MapID.end()) && !trouve; iter++)
    {   int typ = (*iter).second;
		trouve = (typ == element);
    }
    return trouve;
};
//----------------------------------------------------------------------------//

// Recherche si la paire (CString,int) existe dans la map =================//
bool IDMap::IsCouple(CString &cle, int element)
{   Iterat t = MapID.find(cle);
    return ((*t).second == element);
}
//----------------------------------------------------------------------------//

// Retourne le num�ro d'ID correspondant � la clef ===========================//
int IDMap::NumID(CString &cle)
{  Iterat t = MapID.find(cle);
   return (*t).second;
}
//----------------------------------------------------------------------------//

// Renvoi le plus grand attribut trouv� ds la map ============================//
int IDMap::MaxAttribut()
{
    int max = 0;
    for (Iterat iter = MapID.begin(); iter!=MapID.end() ; iter++)
    {   int typ = (*iter).second;
		if (typ > max) max = typ;
    }
    return max;
};

// Ajout une clef � la map en d�terminant lui-m�me l'attribut ================//
void IDMap::AjoutClef(CString cle)
{
    int attrib = MaxAttribut() + 1;
    AjoutID(cle, attrib);
}
//----------------------------------------------------------------------------//

// Cr�ation d'une paire � partir d'un n� donn� et d'une cle de base
bool IDMap::CreateBasedNumID(CString cleDefaut, int attribut)
{
    // l'attribut est d�fini
    int num = attribut;
    if (IsElement(num)) return false;	    // v�rifie que l'�l�ment n'existe d�j� pas

    // Composition de la cle � partir de la cle de base : cle_num
    CString cle = cleDefaut + "_";
    char cNumString[512];
    itoa(num, cNumString, 10);
    CString numString = cNumString;
    cle += numString;
    // Regarde si la cle n'existe dej� pas
    if (IsCle(cle)) return false;

    AjoutID(cle, num);		// l'ajoute enfin au dico

    return true;
}
//----------------------------------------------------------------------------//

// Cr�ation d'une paire uniquement � partir d'un nom par d�faut
void IDMap::CreateBasedID(CString cleDefaut)
{
    // Nouvel attribut
    int attrib = MaxAttribut() + 1;

    // D�termination d'une cle non existante
    int num = 1;
    CString cle;
    CString numString;
    char cNumString[512];
    do
     {
    	cle = cleDefaut + "_";
        itoa(num, cNumString, 10);
    	numString = cNumString;
        // Rajoute des z�ros pour qu'il y est 3 chiffres
        for (int i=numString.GetLength() ; i < 3 ; i++) cle += "0"; 
    	cle += numString;
        num++;
      }
    while (IsCle(cle));

    AjoutID(cle, attrib);	// et l'ajoute � la map
}


//=== Sauvegarde la librairie dans un fichier *.id ===========================//
void IDMap::SauveFileID(CString IDfichier)
{
	FILE *f;

    // Ouvre le fichier binaire en �criture
    if ((f = fopen(LPCSTR(IDfichier),"wb"))==NULL)
           MessageBoxA(NULL,"FICHIER MANQUANT :",LPCSTR(IDfichier),1);

	// �crit chaque paire du dico dans ce fichier dans l'ordre des clefs
    for (Iterat iter = MapID.begin(); iter!=MapID.end() ; iter++)
    {
       // Pr�pare les donn�es � �crire
       CString cle = (*iter).first;
       int typ = (*iter).second;
       char ctypString[512];
       itoa(typ, ctypString, 10);
       CString t= ctypString;

       // �crit la cl�
       fwrite(LPCSTR(cle),cle.GetLength(), 1, f);

       // place quelques espaces entre les 2 valeurs
       // => essaye de le placer au 20i�me caract�re
	   if (cle.GetLength() < 20)
         for (int i=0 ; i <= 20 - cle.GetLength()-t.GetLength(); i++) fwrite(" ",sizeof(char), 1, f);
       else fwrite("  ",sizeof("  "), 1, f);  // sinon place qq espaces


       // �crit le type
       fwrite(LPCSTR(t),t.GetLength(), 1, f);

       // retourne chariot et � la ligne
       char RC = 13;
       fwrite(&RC, sizeof(char), 1, f);
       fwrite("\n", sizeof(char), 1, f);
    }
	fclose(f);
}
//----------------------------------------------------------------------------//

//=== Change le nom de l'identificateur par un autre =========================//
bool IDMap::ChangeClef(CString src, CString dest)
{
	// V�rifie si le nom de la source existe bien
    if (!IsCle(src)) return false;

    // V�rifie si le nouveau nom n'est pas d�j� utilis�
    if (IsCle(dest)) return false;

    // On peut intervertir les 2 noms
    AjoutID(dest, MapID[src]);		// ajoute le nouveau nom
    SupprID(src);					// supprime l'ancien

    return true;
}
//----------------------------------------------------------------------------//

//=== Supprime la paire du dictionnaire ======================================//
void IDMap::SupprID(CString cle)
{    MapID.erase(cle);
}
//----------------------------------------------------------------------------//

//=== Renvoie la cle de n� element ===========================================//
//*** Contrat : le n� doit absolument exister ***
CString IDMap::CleID(int element)
{   // scrute toute la map
    for (Iterat iter = MapID.begin(); iter!=MapID.end() ; iter++)
    {   int typ = (*iter).second;
	if (typ == element) return (*iter).first;
    }                     
    return "";     // Si pas sorti avant => ERROR
}
//----------------------------------------------------------------------------//

//=== Constructeur par d�faut ================================================//
IDMap::IDMap()
{
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
IDMap::~IDMap()
{	MapID.clear();
}
//----------------------------------------------------------------------------//

