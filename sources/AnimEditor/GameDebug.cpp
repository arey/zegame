////////////////////////////////////////////////////////////////////////////////
//  Unit�  "GameDebug.cpp"  : Impl�mentation de la classe GameDebug utilis�   //
//                            pour d�bugger en mode plein �cran.    		  //
//  Copyright (C) 1999 Antoine Rey    antoine.rey@insa-lyon.fr				  //
//                                                                            //
//  Version      : 1.00                                                  	  //
//  Creation     : 12/06/1999                                                 //
//  Last upgrade : 12/06/1999                                                 //
////////////////////////////////////////////////////////////////////////////////

#include "GameDebug.h"		// Header

GameDebug *ZeGameDebug = NULL;  // Instance de cette classe utilis�e ds le jeu 

//=== TRACE ds fichier une cha�ne de caract�res format�s � la printf =========//
//  format : %d, %f
void GameDebug::TRACE(char* fstring, ...)
{    va_list  pParam;                // Pointeur sur les param�tres
     int entier;                     // Types de param�tres que l'on peut recevoir : entier, flottant,
     float flottant;
     bool   boolean;
     char*  chaine;
     va_start(pParam, fstring);      // adresse du 1er param�tre optionnel

     // On parcours toute la cha�ne en recherchant les formats %?
     while (*fstring!='\0')         // tant que l'on n'a pas rencontr� le caract�re '\0' de fin de cha�ne
     {
        if (*fstring != '%')        // Si c'est un caract�re 'normal' on l'�crit tel quel ds le fichier de sortie
        {  fout << *fstring;
        } else
        {  switch (*++fstring)      // Le caract�re suivant nous donne le format du param�tre
           {  case 'd' :            // On a affaire � un entier
                   entier = va_arg(pParam, int);
                   fout << entier;
                   break;
              case 'f' :            // Nb � virgules flottantes
                   flottant = va_arg(pParam,float);
                   fout << flottant;
                   break;
              case 'b' : 			// Bool�en
              	   boolean = va_arg(pParam,bool);
                   if (boolean) fout << "vrai";
                   else         fout << "faux";
                   break;
              case 's' :                        // String
                   chaine = va_arg(pParam,char*);
                   fout << chaine;
                   break;
              default : fout << *fstring;	// caract�re inconnu
           }
        }
        fstring++;					// Caract�re suivant
     }
     va_end(pParam);				// Fin d'�valuation : NULL ds pParam
}
//----------------------------------------------------------------------------//

//=== Constructeur obligatoire ===============================================//
GameDebug::GameDebug(char* fichier)
{  // Copie locale de la cha�ne
   filename = new char[strlen(fichier)+1];
   strcpy(filename, fichier);

   fout.open(filename, ios::out);         // Cr�e et ouvre le fichier en �criture
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
GameDebug::~GameDebug()
{  fout.close();         // ferme le fichier
   delete [] filename;   // lib�re la m�moire allou�e pour le nom de fichier
}
//----------------------------------------------------------------------------//


//=== Affiche une cha�ne format�e � l'�cran en (posX,posY) ===================//
void GameDebug::STRINGF(int posX, int posY, unsigned int color, char* fstring, ...)
{    va_list  pParam;                // Pointeur sur les param�tres
     int entier;                     // Types de param�tres que l'on peut recevoir : entier, flottant,
     double flottant;
     bool   boolean;
     char*  cstring;
     va_start(pParam, fstring);      // adresse du 1er param�tre optionnel
     static char Texte[512];         // Buffer de caract�res que l'on va afficher � l'�cran
     int indice = 0;                 // Indice de d�placement dans le buffer
     char convert[64];               // Buffer de caract�res servant aux conversions

     // On parcours toute la cha�ne en recherchant les formats %?
     while (*fstring!='\0')         // tant que l'on n'a pas rencontr� le caract�re '\0' de fin de cha�ne
     {
        if (*fstring != '%')        // Si c'est un caract�re 'normal' on l'�crit tel quel ds le fichier de sortie
        {  Texte[indice++] = (char) *fstring;
        } else
        {  switch (*++fstring)      // Le caract�re suivant nous donne le format du param�tre
           {  case 'd' :            // On a affaire � un entier
                   entier = va_arg(pParam, int);
                   itoa(entier, convert, 10);
                   Texte[indice] = '\0';
                   strcat(Texte, convert);
                   indice = strlen(Texte);
                   break;
              case 'f' :            // Nb � virgules flottantes
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
              case 'b' : 			//  Bool�en
              	   boolean = va_arg(pParam,bool);
                   Texte[indice] = '\0';
                   if (boolean)
                   { strcat(Texte,"vrai");
                   } else
                   { strcat(Texte,"faux");
                   }
                   indice = strlen(Texte);
                   break;
              default : fout << *fstring;	// caract�re inconnu
           }
        }
        fstring++;				// Caract�re suivant
     }
     Texte[indice] = '\0';      // Caracr�re de fin de cha�ne
     va_end(pParam);		// Fin d'�valuation : NULL ds pParam

     if ((DS==NULL) || (DS->pDDSBack==NULL)) return;
     DS->pDDSBack->TextSurf(posX, posY, color, Texte);
}

//=== Affiche les principales infos d'un perso ===============================//
void GameDebug::TRACE_Perso(Personnage* perso)
{    static char mot[128];
     EtatToString(perso->peEtat, mot);
     STRINGF(15, 40, RGBto16(250,10,10), "peEtat : %s  n� %d ", mot, perso->peEtat);
     TRACEDBG("peEtat : %s", mot);
     EtatToString(perso->peFutEtat, mot);
     TRACEDBG("     peFutEtat : %s", mot);
     STRINGF(15, 55, RGBto16(250,10,10), "peFutEtat : %s  n� %d", mot, perso->peFutEtat);
     EtatToString(perso->peOldEtat, mot);
     TRACEDBG("     peOldEtat : %s\n", mot);
     STRINGF(15, 70, RGBto16(250,10,10), "peOldEtat : %s  n� %d", mot, perso->peOldEtat);
     STRINGF(15, 85, RGBto16(250,10,10), "peDirection : %d", perso->peDirection);
     STRINGF(15,100, RGBto16(250,10,10), "peFutDirection : %d", perso->peFutDirection);
     STRINGF(15,115, RGBto16(250,10,10), "peNumSprite : %d", perso->peNumSprite);

}
