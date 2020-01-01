////////////////////////////////////////////////////////////////////////////////
// IDMap.cpp : permet de gérer la gestion de tous les IDENTIFICATEURS du jeu, //
//			   utilisés dans les divers éditeurs.							  //
////////////////////////////////////////////////////////////////////////////////


//--- inclusuions -----------------------------------------------------------
#include <vcl\system.hpp>  // pour la manipulation des fichiers
#include "IDMap.h"		   // interface de la classe

using namespace std;

//---------------------------------------------------------------------------
//=== Permet de mettre dans le dictionnaire un fichier contenant les noms ====//
//=== des identificateurs et leur numéro associé						  ====//
void IDMap::LireFileID(AnsiString IDfichier)
{
    FILE *f;
    int ComptLine = 1;  	// Compte le nombre de ligne lue
	const int MaxLine = 1024;		// Nb maximal de caractères / ligne
    char BufLine[MaxLine];	// Buffer de lecture d'une ligne du fichier

    // Ouvre le fichier
    if ((f = fopen(IDfichier.c_str(),"r"))==NULL)
           MessageBoxA(NULL,"FICHIER MANQUANT :",IDfichier.c_str(),1);

    // Scrute le fichier ligne par ligne
	while (fgets(BufLine, MaxLine, f) != NULL)  // tant que le fichier n'est pas fini
     {
        AnsiString cle;             // cle de la map sous forme de chaînes
        AnsiString sattribut;	// attribut de la map
        int attribut;

        int iter = 0;			// début de ligne

	  	if (BufLine[0]=='\n') goto _FinDeLigne; 				  		// Ligne vide
    	if ((BufLine[0]=='/') && (BufLine[1]=='/')) goto _FinDeLigne;	// Commentaires
        // Pour chaque ligne, on va lire : l'entier & la chaîne de caractères séparés
        // par des espaces

        // saute les espaces de début de ligne
        while ((BufLine[iter]==' ') || (BufLine[iter]=='\t'))  iter++;

        // On lit le nom de l'identificateur
        while ((BufLine[iter]!=' ') && (BufLine[iter]!='\n') && (BufLine[iter]!='\t'))
         {
           if (BufLine[iter]=='\n')
              { AnsiString ligne = "Ligne : " + ComptLine;
                MessageBoxA(NULL,"fichier Identificateur corrompu",ligne.c_str(),1);
              }
           char a = BufLine[iter];
		   cle += a;    // ajout d'un caractère
		   iter++;		// un caractère de plus
         };
        // saute les espaces de début de ligne
        while ((BufLine[iter]==' ') || (BufLine[iter]=='\t'))  iter++;

        // On lit le chiffre
        while ((BufLine[iter]!=' ') && (BufLine[iter]!='\n') && (BufLine[iter]!='\t'))
         {
           char a = BufLine[iter];
		   sattribut += a;    // ajout d'un caractère
		   iter++;		      // un caractère de plus
         };

        // transforme la chaîne de caractère en entier
//        MessageBoxA(NULL,"Ligne n°",cle.c_str(),0);
//        MessageBoxA(NULL,"Ligne n°",sattribut.c_str(),0);
        attribut = sattribut.ToInt();

        // Ajoute à la map une paire <AnsiString,int>
        AjoutID(cle, attribut);

       _FinDeLigne:						// Ligne suivante
        ComptLine++;                    // une ligne de plus
    };
fclose(f);
};
//----------------------------------------------------------------------------//

//== Ajoute un élément à la map ==============================================//
// => Si la clef n'existe pas !
void IDMap::AjoutID(AnsiString cle, int attribut)
{
    MapID.insert(make_pair(cle, attribut));
}
//----------------------------------------------------------------------------//

//== Ajout d'un élément à la map =============================================//
// => en écrasant l'attribut des clefs déjà existante
void IDMap::EcraseID(AnsiString cle, int attribut)
{
    MapID[cle] = attribut;
}
//----------------------------------------------------------------------------//

// Recherche si la clef est déjà utilisée dans la map ========================//
bool IDMap::IsCle(AnsiString cle)
{
    Iterat t = MapID.find(cle);
    return (t != MapID.end());		// vrai si la clef a été trouvée
}
//----------------------------------------------------------------------------//

// Recherche si l'élément "int" existe dans la map ===========================//
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

// Recherche si la paire (AnsiString,int) existe dans la map =================//
bool IDMap::IsCouple(AnsiString &cle, int element)
{   Iterat t = MapID.find(cle);
    return ((*t).second == element);
}
//----------------------------------------------------------------------------//

// Retourne le numéro d'ID correspondant à la clef ===========================//
int IDMap::NumID(AnsiString &cle)
{  Iterat t = MapID.find(cle);
   return (*t).second;
}
//----------------------------------------------------------------------------//

// Renvoi le plus grand attribut trouvé ds la map ============================//
int IDMap::MaxAttribut()
{
    int max = 0;
    for (Iterat iter = MapID.begin(); iter!=MapID.end() ; iter++)
    {   int typ = (*iter).second;
		if (typ > max) max = typ;
    }
    return max;
};

// Ajout une clef à la map en déterminant lui-même l'attribut ================//
void IDMap::AjoutClef(AnsiString cle)
{
    int attrib = MaxAttribut() + 1;
    AjoutID(cle, attrib);
}
//----------------------------------------------------------------------------//

// Création d'une paire à partir d'un n° donné et d'une cle de base
bool IDMap::CreateBasedNumID(AnsiString cleDefaut, int attribut)
{
    // l'attribut est défini
    int num = attribut;
    if (IsElement(num)) return false;	    // vérifie que l'élément n'existe déjà pas

    // Composition de la cle à partir de la cle de base : cle_num
    AnsiString cle = cleDefaut + "_";
    AnsiString numString = num;
    cle += numString.c_str();
    // Regarde si la cle n'existe dejà pas
    if (IsCle(cle)) return false;

    AjoutID(cle, num);		// l'ajoute enfin au dico

    return true;
}
//----------------------------------------------------------------------------//

// Création d'une paire uniquement à partir d'un nom par défaut
void IDMap::CreateBasedID(AnsiString cleDefaut)
{
    // Nouvel attribut
    int attrib = MaxAttribut() + 1;

    // Détermination d'une cle non existante
    int num = 1;
    AnsiString cle;
    AnsiString numString;
    do
     {
    	cle = cleDefaut + "_";
    	numString = num;
        // Rajoute des zéros pour qu'il y est 3 chiffres
        for (int i=numString.Length() ; i < 3 ; i++) cle += "0"; 
    	cle += numString.c_str();
        num++;
      }
    while (IsCle(cle));

    AjoutID(cle, attrib);	// et l'ajoute à la map
}


//=== Sauvegarde la librairie dans un fichier *.id ===========================//
void IDMap::SauveFileID(AnsiString IDfichier)
{
	FILE *f;

    // Ouvre le fichier binaire en écriture
    if ((f = fopen(IDfichier.c_str(),"wb"))==NULL)
           MessageBoxA(NULL,"FICHIER MANQUANT :",IDfichier.c_str(),1);

	// écrit chaque paire du dico dans ce fichier dans l'ordre des clefs
    for (Iterat iter = MapID.begin(); iter!=MapID.end() ; iter++)
    {
       // Prépare les données à écrire
       AnsiString cle = (*iter).first;
       int typ = (*iter).second;
       AnsiString t= typ;

       // écrit la clé
       fwrite(cle.c_str(),cle.Length(), 1, f);

       // place quelques espaces entre les 2 valeurs
       // => essaye de le placer au 20ième caractère
	   if (cle.Length() < 20)
         for (int i=0 ; i <= 20 - cle.Length()-t.Length(); i++) fwrite(" ",sizeof(char), 1, f);
       else fwrite("  ",sizeof("  "), 1, f);  // sinon place qq espaces


       // écrit le type
       fwrite(t.c_str(),t.Length(), 1, f);

       // retourne chariot et à la ligne
       char RC = 13;
       fwrite(&RC, sizeof(char), 1, f);
       fwrite("\n", sizeof(char), 1, f);
    }
	fclose(f);
}
//----------------------------------------------------------------------------//

//=== Change le nom de l'identificateur par un autre =========================//
bool IDMap::ChangeClef(AnsiString src, AnsiString dest)
{
	// Vérifie si le nom de la source existe bien
    if (!IsCle(src)) return false;

    // Vérifie si le nouveau nom n'est pas déjà utilisé
    if (IsCle(dest)) return false;

    // On peut intervertir les 2 noms
    AjoutID(dest, MapID[src]);		// ajoute le nouveau nom
    SupprID(src);					// supprime l'ancien

    return true;
}
//----------------------------------------------------------------------------//

//=== Supprime la paire du dictionnaire ======================================//
void IDMap::SupprID(AnsiString cle)
{    MapID.erase(cle);
}
//----------------------------------------------------------------------------//

//=== Renvoie la cle de n° element ===========================================//
//*** Contrat : le n° doit absolument exister ***
AnsiString IDMap::CleID(int element)
{   // scrute toute la map
    for (Iterat iter = MapID.begin(); iter!=MapID.end() ; iter++)
    {   int typ = (*iter).second;
	if (typ == element) return (*iter).first;
    }                     
    return "";     // Si pas sorti avant => ERROR
}
//----------------------------------------------------------------------------//

