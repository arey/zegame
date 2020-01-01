////////////////////////////////////////////////////////////////////////////////
// IDMap.cpp : permet de g�rer la gestion de tous les IDENTIFICATEURS du jeu, //
//			   utilis�s dans les divers �diteurs.							  //
////////////////////////////////////////////////////////////////////////////////


//--- inclusuions -----------------------------------------------------------
#include <vcl\system.hpp>  // pour la manipulation des fichiers
#include "IDMap.h"		   // interface de la classe

using namespace std;

//---------------------------------------------------------------------------
//=== Permet de mettre dans le dictionnaire un fichier contenant les noms ====//
//=== des identificateurs et leur num�ro associ�						  ====//
void IDMap::LireFileID(AnsiString IDfichier)
{
    FILE *f;
    int ComptLine = 1;  	// Compte le nombre de ligne lue
	const int MaxLine = 1024;		// Nb maximal de caract�res / ligne
    char BufLine[MaxLine];	// Buffer de lecture d'une ligne du fichier

    // Ouvre le fichier
    if ((f = fopen(IDfichier.c_str(),"r"))==NULL)
           MessageBoxA(NULL,"FICHIER MANQUANT :",IDfichier.c_str(),1);

    // Scrute le fichier ligne par ligne
	while (fgets(BufLine, MaxLine, f) != NULL)  // tant que le fichier n'est pas fini
     {
        AnsiString cle;             // cle de la map sous forme de cha�nes
        AnsiString sattribut;	// attribut de la map
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
              { AnsiString ligne = "Ligne : " + ComptLine;
                MessageBoxA(NULL,"fichier Identificateur corrompu",ligne.c_str(),1);
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
        attribut = sattribut.ToInt();

        // Ajoute � la map une paire <AnsiString,int>
        AjoutID(cle, attribut);

       _FinDeLigne:						// Ligne suivante
        ComptLine++;                    // une ligne de plus
    };
fclose(f);
};
//----------------------------------------------------------------------------//

//== Ajoute un �l�ment � la map ==============================================//
// => Si la clef n'existe pas !
void IDMap::AjoutID(AnsiString cle, int attribut)
{
    MapID.insert(make_pair(cle, attribut));
}
//----------------------------------------------------------------------------//

//== Ajout d'un �l�ment � la map =============================================//
// => en �crasant l'attribut des clefs d�j� existante
void IDMap::EcraseID(AnsiString cle, int attribut)
{
    MapID[cle] = attribut;
}
//----------------------------------------------------------------------------//

// Recherche si la clef est d�j� utilis�e dans la map ========================//
bool IDMap::IsCle(AnsiString cle)
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

// Recherche si la paire (AnsiString,int) existe dans la map =================//
bool IDMap::IsCouple(AnsiString &cle, int element)
{   Iterat t = MapID.find(cle);
    return ((*t).second == element);
}
//----------------------------------------------------------------------------//

// Retourne le num�ro d'ID correspondant � la clef ===========================//
int IDMap::NumID(AnsiString &cle)
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
void IDMap::AjoutClef(AnsiString cle)
{
    int attrib = MaxAttribut() + 1;
    AjoutID(cle, attrib);
}
//----------------------------------------------------------------------------//

// Cr�ation d'une paire � partir d'un n� donn� et d'une cle de base
bool IDMap::CreateBasedNumID(AnsiString cleDefaut, int attribut)
{
    // l'attribut est d�fini
    int num = attribut;
    if (IsElement(num)) return false;	    // v�rifie que l'�l�ment n'existe d�j� pas

    // Composition de la cle � partir de la cle de base : cle_num
    AnsiString cle = cleDefaut + "_";
    AnsiString numString = num;
    cle += numString.c_str();
    // Regarde si la cle n'existe dej� pas
    if (IsCle(cle)) return false;

    AjoutID(cle, num);		// l'ajoute enfin au dico

    return true;
}
//----------------------------------------------------------------------------//

// Cr�ation d'une paire uniquement � partir d'un nom par d�faut
void IDMap::CreateBasedID(AnsiString cleDefaut)
{
    // Nouvel attribut
    int attrib = MaxAttribut() + 1;

    // D�termination d'une cle non existante
    int num = 1;
    AnsiString cle;
    AnsiString numString;
    do
     {
    	cle = cleDefaut + "_";
    	numString = num;
        // Rajoute des z�ros pour qu'il y est 3 chiffres
        for (int i=numString.Length() ; i < 3 ; i++) cle += "0"; 
    	cle += numString.c_str();
        num++;
      }
    while (IsCle(cle));

    AjoutID(cle, attrib);	// et l'ajoute � la map
}


//=== Sauvegarde la librairie dans un fichier *.id ===========================//
void IDMap::SauveFileID(AnsiString IDfichier)
{
	FILE *f;

    // Ouvre le fichier binaire en �criture
    if ((f = fopen(IDfichier.c_str(),"wb"))==NULL)
           MessageBoxA(NULL,"FICHIER MANQUANT :",IDfichier.c_str(),1);

	// �crit chaque paire du dico dans ce fichier dans l'ordre des clefs
    for (Iterat iter = MapID.begin(); iter!=MapID.end() ; iter++)
    {
       // Pr�pare les donn�es � �crire
       AnsiString cle = (*iter).first;
       int typ = (*iter).second;
       AnsiString t= typ;

       // �crit la cl�
       fwrite(cle.c_str(),cle.Length(), 1, f);

       // place quelques espaces entre les 2 valeurs
       // => essaye de le placer au 20i�me caract�re
	   if (cle.Length() < 20)
         for (int i=0 ; i <= 20 - cle.Length()-t.Length(); i++) fwrite(" ",sizeof(char), 1, f);
       else fwrite("  ",sizeof("  "), 1, f);  // sinon place qq espaces


       // �crit le type
       fwrite(t.c_str(),t.Length(), 1, f);

       // retourne chariot et � la ligne
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
void IDMap::SupprID(AnsiString cle)
{    MapID.erase(cle);
}
//----------------------------------------------------------------------------//

//=== Renvoie la cle de n� element ===========================================//
//*** Contrat : le n� doit absolument exister ***
AnsiString IDMap::CleID(int element)
{   // scrute toute la map
    for (Iterat iter = MapID.begin(); iter!=MapID.end() ; iter++)
    {   int typ = (*iter).second;
	if (typ == element) return (*iter).first;
    }                     
    return "";     // Si pas sorti avant => ERROR
}
//----------------------------------------------------------------------------//

