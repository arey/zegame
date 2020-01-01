////////////////////////////////////////////////////////////////////////////////
//  Unité  "GameDebug.cpp"  : Implémentation de la classe GameDebug utilisé   //
//                            pour débugger en mode plein écran.    		  //
//  Copyright (C) 1999 Antoine Rey    antoine.rey@insa-lyon.fr				  //
//                                                                            //
//  Version      : 1.00                                                  	  //
//  Creation     : 12/06/1999                                                 //
//  Last upgrade : 12/06/1999                                                 //
////////////////////////////////////////////////////////////////////////////////

#include "GameDebug.h"		// Header

GameDebug *ZeGameDebug = NULL;  // Instance de cette classe utilisée ds le jeu 

//=== TRACE ds fichier une chaîne de caractères formatés à la printf =========//
//  format : %d, %f
void GameDebug::TRACE(char* fstring, ...)
{    va_list  pParam;                // Pointeur sur les paramètres
     int entier;                     // Types de paramètres que l'on peut recevoir : entier, flottant,
     float flottant;
     bool   boolean;
     char*  chaine;
     va_start(pParam, fstring);      // adresse du 1er paramètre optionnel

     // On parcours toute la chaîne en recherchant les formats %?
     while (*fstring!='\0')         // tant que l'on n'a pas rencontré le caractère '\0' de fin de chaîne
     {
        if (*fstring != '%')        // Si c'est un caractère 'normal' on l'écrit tel quel ds le fichier de sortie
        {  fout << *fstring;
        } else
        {  switch (*++fstring)      // Le caractère suivant nous donne le format du paramètre
           {  case 'd' :            // On a affaire à un entier
                   entier = va_arg(pParam, int);
                   fout << entier;
                   break;
              case 'f' :            // Nb à virgules flottantes
                   flottant = va_arg(pParam,float);
                   fout << flottant;
                   break;
              case 'b' : 			// Booléen
              	   boolean = va_arg(pParam,bool);
                   if (boolean) fout << "vrai";
                   else         fout << "faux";
                   break;
              case 's' :                        // String
                   chaine = va_arg(pParam,char*);
                   fout << chaine;
                   break;
              default : fout << *fstring;	// caractère inconnu
           }
        }
        fstring++;					// Caractère suivant
     }
     va_end(pParam);				// Fin d'évaluation : NULL ds pParam
}
//----------------------------------------------------------------------------//

//=== Constructeur obligatoire ===============================================//
GameDebug::GameDebug(char* fichier)
{  // Copie locale de la chaîne
   filename = new char[strlen(fichier)+1];
   strcpy(filename, fichier);

   fout.open(filename, ios::out);         // Crée et ouvre le fichier en écriture
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
GameDebug::~GameDebug()
{  fout.close();         // ferme le fichier
   delete [] filename;   // libère la mémoire allouée pour le nom de fichier
}
//----------------------------------------------------------------------------//


//=== Affiche une chaîne formatée à l'écran en (posX,posY) ===================//
void GameDebug::STRINGF(int posX, int posY, unsigned int color, char* fstring, ...)
{    va_list  pParam;                // Pointeur sur les paramètres
     int entier;                     // Types de paramètres que l'on peut recevoir : entier, flottant,
     double flottant;
     bool   boolean;
     char*  cstring;
     va_start(pParam, fstring);      // adresse du 1er paramètre optionnel
     static char Texte[512];         // Buffer de caractères que l'on va afficher à l'écran
     int indice = 0;                 // Indice de déplacement dans le buffer
     char convert[64];               // Buffer de caractères servant aux conversions

     // On parcours toute la chaîne en recherchant les formats %?
     while (*fstring!='\0')         // tant que l'on n'a pas rencontré le caractère '\0' de fin de chaîne
     {
        if (*fstring != '%')        // Si c'est un caractère 'normal' on l'écrit tel quel ds le fichier de sortie
        {  Texte[indice++] = (char) *fstring;
        } else
        {  switch (*++fstring)      // Le caractère suivant nous donne le format du paramètre
           {  case 'd' :            // On a affaire à un entier
                   entier = va_arg(pParam, int);
                   itoa(entier, convert, 10);
                   Texte[indice] = '\0';
                   strcat(Texte, convert);
                   indice = strlen(Texte);
                   break;
              case 'f' :            // Nb à virgules flottantes
                   flottant = va_arg(pParam,double);
                   gcvt(flottant, 6, convert);
                   Texte[indice] = '\0';
                   strcat(Texte, convert);
                   indice = strlen(Texte);
                   break;
              case 's' : 			// String
              	   cstring = va_arg(pParam,char*);
                   Texte[indice] = '\0';
                   strcat(Texte, cstring);
                   indice = strlen(Texte);
                   break;
              case 'b' : 			//  Booléen
              	   boolean = va_arg(pParam,bool);
                   Texte[indice] = '\0';
                   if (boolean)
                   { strcat(Texte,"vrai");
                   } else
                   { strcat(Texte,"faux");
                   }
                   indice = strlen(Texte);
                   break;
              default : fout << *fstring;	// caractère inconnu
           }
        }
        fstring++;				// Caractère suivant
     }
     Texte[indice] = '\0';      // Caracrère de fin de chaîne
     va_end(pParam);		// Fin d'évaluation : NULL ds pParam

     if ((DS==NULL) || (DS->pDDSBack==NULL)) return;
     DS->pDDSBack->TextSurf(posX, posY, color, Texte);
}

//=== Affiche les principales infos d'un perso ===============================//
void GameDebug::TRACE_Perso(Personnage* perso)
{    static char mot[128];
     EtatToString(perso->peEtat, mot);
     STRINGF(15, 40, RGBto16(250,10,10), "peEtat : %s  n° %d ", mot, perso->peEtat);
     TRACEDBG("peEtat : %s", mot);
     EtatToString(perso->peFutEtat, mot);
     TRACEDBG("     peFutEtat : %s", mot);
     STRINGF(15, 55, RGBto16(250,10,10), "peFutEtat : %s  n° %d", mot, perso->peFutEtat);
     EtatToString(perso->peOldEtat, mot);
     TRACEDBG("     peOldEtat : %s\n", mot);
     STRINGF(15, 70, RGBto16(250,10,10), "peOldEtat : %s  n° %d", mot, perso->peOldEtat);
     STRINGF(15, 85, RGBto16(250,10,10), "peDirection : %d", perso->peDirection);
     STRINGF(15,100, RGBto16(250,10,10), "peFutDirection : %d", perso->peFutDirection);
     STRINGF(15,115, RGBto16(250,10,10), "peNumSprite : %d", perso->peNumSprite);

}
