////////////////////////////////////////////////////////////////////////////////
//   UNITE : Animation All Sprite											  //
//	 => permet e relire les fichiers *.ina contenant les informations d'	  //
//	 d'animation des sprites												  //
//                                                                            //
//	 Begin 27/06/98															  //
////////////////////////////////////////////////////////////////////////////////

#include <vcl\system.hpp>  // pour les pointers NULL
#include <vcl\vcl.h>
#include <Stdio.h>


#include "Animall.h"
#include "fPrincipale.h"
#include "DDLibEditor.h"
#include "GameDebug.h"

// retourne la valeur affectée à l'état
int ValEtat(const char* phrase)
{
    if (strcmp(phrase,"MARCHE1")==0) return MARCHE1;
    if (strcmp(phrase,"ARRET1")==0) return ARRET1;
    if (strcmp(phrase,"PRENDCOUP1")==0) return PRENDCOUP1;
    if (strcmp(phrase,"TOURNER1")==0) return TOURNER1;
    if (strcmp(phrase,"TOURNER2")==0) return TOURNER2;
    if (strcmp(phrase,"DEMITOUR1")==0) return DEMITOUR1;
    if (strcmp(phrase,"DONNECOUP1")==0) return DONNECOUP1;
    if (strcmp(phrase,"MEURT1")==0) return MEURT1;
    if (strcmp(phrase,"MORT1")==0) return MORT1;
    if (strcmp(phrase,"COURSE1")==0) return COURSE1;
    if (strcmp(phrase,"STOPNET1")==0) return STOPNET1;
    if (strcmp(phrase,"NOSTATE")==0) return NOSTATE;
    if (strcmp(phrase,"STAND1")==0) return STAND1;
    if (strcmp(phrase,"COUPEPEE1")==0) return COUPEPEE1;
    if (strcmp(phrase,"MANGE1")==0) return MANGE1;
    if (strcmp(phrase,"FINIMANGER1")==0) return FINIMANGER1;
    if (strcmp(phrase,"BRISE1")==0) return BRISE1;
    if (strcmp(phrase,"BRISE2")==0) return BRISE2;
    if (strcmp(phrase,"CASSER1")==0) return CASSER1;
    if (strcmp(phrase,"CASSER2")==0) return CASSER2;
    if (strcmp(phrase,"TIRER1")==0) return TIRER1;
    if (strcmp(phrase,"SORTIR1")==0) return SORTIR1;
    if (strcmp(phrase,"SORTI1")==0) return SORTI1;
    if (strcmp(phrase,"REBUILD1")==0) return REBUILD1;
    if (strcmp(phrase,"NOSTATE")==0) return NOSTATE;
    if (strcmp(phrase,"TRAVAIL1")==0) return TRAVAIL1;
    if (strcmp(phrase,"TRAVAIL2")==0) return TRAVAIL2;
    if (strcmp(phrase,"COUPHACHE1")==0) return COUPHACHE1;
    if (strcmp(phrase,"SAUTE1")==0) return SAUTE1;
    if (strcmp(phrase,"RESSUSCITE1")==0) return RESSUSCITE1;
    // si arrive jusque là : erreur
    MessageBoxA(NULL,phrase,"Animall.cpp : ValEtat",1);
    return -1;
}
//----------------------------------------------------------------------------//


//=== Convertit le n° de l'Etat en chaîne de caractère =======================//
void EtatToString(int etat, char* texte)
{    switch(etat)
     { case MARCHE1      : strcpy(texte, "MARCHE1"); break;
       case ARRET1       : strcpy(texte, "ARRET1"); break;
       case PRENDCOUP1   : strcpy(texte, "PRENDCOUP1"); break;
       case TOURNER1     : strcpy(texte, "TOURNER1"); break;
       case TOURNER2     : strcpy(texte, "TOURNER2"); break;
       case DEMITOUR1    : strcpy(texte, "DEMITOUR1"); break;
       case DONNECOUP1   : strcpy(texte, "DONNECOUP1"); break;
       case MEURT1       : strcpy(texte, "MEURT1"); break;
       case MORT1        : strcpy(texte, "MORT1"); break;
       case COURSE1      : strcpy(texte, "COURSE1"); break;
       case STOPNET1     : strcpy(texte, "STOPNET1"); break;
       case NOSTATE      : strcpy(texte, "NOSTATE"); break;
       case STAND1       : strcpy(texte, "STAND1"); break;
       case COUPEPEE1    : strcpy(texte, "COUPEPEE1"); break;
       case MANGE1       : strcpy(texte, "MANGE1"); break;
       case FINIMANGER1  : strcpy(texte, "FINIMANGER1"); break;
       case BRISE1       : strcpy(texte, "BRISE1"); break;
       case BRISE2       : strcpy(texte, "BRISE2"); break;
       case CASSER1      : strcpy(texte, "CASSER1"); break;
       case CASSER2      : strcpy(texte, "CASSER2"); break;
       case REBUILD1     : strcpy(texte, "REBUILD1"); break;
       case TRAVAIL1     : strcpy(texte, "TRAVAIL1"); break;
       case TRAVAIL2     : strcpy(texte, "TRAVAIL2"); break;
       case COUPHACHE1   : strcpy(texte, "COUPHACHE1"); break;
       case SAUTE1	 : strcpy(texte, "SAUTE1"); break;
       case RESSUSCITE1	 : strcpy(texte, "RESSUSCITE1"); break;
       default :  strcpy(texte, "Inconnu"); break;
     }
}
//----------------------------------------------------------------------------//


// Tableau dynamique de pointers
//AnimSprite** AnimSpriteLib = NULL;

// Tableau dynamique de pointers;
// AnimTransition** AnimTransLib = NULL;

AnimSprite::AnimSprite(void)
{
    Nom[0] ='\n';
    Numero = -1;
    Type   =  0;
    NbSprites = 0;
    FirstSprite = -1;
    for (int i=0 ; i<8 ; i++) SprRepertoire[i] = NULL;
};

AnimTransition::AnimTransition(void)
{
    NumActCour = NumActFut = NumActInterm = -1;
    ImageA = ImageB = -1;
};

//=== Charge tout le fichier *.ina ===========================================//
// => fichier texte d'animations des PERSOS
void LoadFileIna(const char* InaNameTresShort,AnimSprite*** pASL, AnimTransition*** pATL,int* pNbAnim,int* pNbTrans,SpriteTilePersoLib **STLPerso, Character* Individu)
{
    // Chemin complet
    AnsiString InaName = CheminApplication + cPathPersos + InaNameTresShort + "\\" + InaNameTresShort +".ina";

	char InaNameCommon[512];
    #ifdef CBUILDER
	strcpy(InaNameCommon, InaName.c_str());
	#else
	strcpy(InaNameCommon, LPCSTR(InaName));
	#endif // CBUILDER

    FILE *f;
    int ComptLine = 1;  	// Compte le nombre de ligne
    int MaxLine = 512;		// Nb maximal de caractères / ligne
    char BufLine[512];		// Buffer de lecture d'une ligne du fichier
    int Etape = 0;			// Etape du fichier

    int NbAnim;				// Nombre d'animations ou de transitions ou de ...
    int NumAnim;			// Curseur pour se repérer dans la Liste

    // Pointers à renvoyés à la fin de la fonction à l'appelante
    AnimSprite **ASL;
    AnimTransition **ATL;

    // Ouvre le fichier
    if ((f = fopen(InaName.c_str(),"r"))==NULL)
       {   MessageBoxA(NULL,"FICHIER INEXISTANT :",InaName.c_str(),1);
           return;
       }
       
    // Scrute le fichier ligne par ligne
    while (fgets(BufLine, MaxLine, f) != NULL)  // tant que le fichier n'est pas fini
    {
   		if (BufLine[0]=='\n') goto _FinDeLigne; 				  		// Ligne vide
        if ((BufLine[0]=='/') && (BufLine[1]=='/')) goto _FinDeLigne;	// Commentaires

        // Changement d'étape
        if ((Etape!=1) || (Etape!=2) || (Etape!=3))
         if (BufLine[0]=='#')
          {
            char Entete[128];
            int i=1, curseur;
			// Lit les caractères et les ranges ds Entete jusqu'à tabulation
            while (BufLine[i]!='\t') { Entete[i-1]=BufLine[i]; i++; };
            Entete[i-1]='\0';

            // Passe à l'étape souhaitée
            if (strcmp(Entete,"anim")==0) Etape = 1;
            else if (strcmp(Entete,"transition")==0) Etape = 2;
            else if (strcmp(Entete,"frame")==0) Etape = 3;
            else if (strcmp(Entete,"frameSuperposee")==0) Etape = 4;

            // Puis récupère le nombre d'anim, de transition ou de ...
            curseur = ++i; i=0;
            while (BufLine[curseur]!='\n') { Entete[i] = BufLine[curseur]; i++; curseur++; };
            Entete[i]='\0';
            NbAnim = atoi(Entete);     // conversion chaîne de caractères -> integer
            NumAnim = 0;

            // Alloue de la mémoire suivant Etape & NbAnim
            // Alloue la mémoire nécessaire vers le tableau
            switch (Etape)  {
            	case  1 :      //  #anim
                    if ((ASL = (AnimSprite**) malloc(sizeof(AnimSprite*) * NbAnim)) == NULL)
         	           MessageBoxA(NULL,"Problème de mémoire","LoadFileIna",1);
                    *pNbAnim = NbAnim;
                    break;
                case  2 :     //  #transition
                  if ((ATL = (AnimTransition**) malloc(sizeof(AnimTransition*) * NbAnim)) == NULL)
         	          MessageBoxA(NULL,"Problème de mémoire","LoadFileIna",1);
                  *pNbTrans = NbAnim;
                   break;
                case  3 :     //  #frame
                   (*STLPerso)->nbFrames = NbAnim;
                   (*STLPerso)->fileInaBmpName = new char* [NbAnim];
                   break;
                case 4 :      // #frameSuperposee
                    Individu->nbObjetFrames = NbAnim;
                    Individu->STLFrameSuperp->fileInaBmpName = new char* [NbAnim];
                   break;
            };

        	// passe à la ligne suivante
            goto _FinDeLigne;
          };

		// Si l'on arrive jusque là, c'est que l'on est arrivé à une ligne importante de l'étape
        switch (Etape)
        {
        	case 1 :  //  #anim
             {  // variables utilisées
                int c=0, i=2;	  // compteurs
             	char texte[128];  // buffer
                AnimSprite* AS;

                // crée le pointer vers l'objet et l'affecte à la liste
                AS = new AnimSprite();
                ASL[NumAnim] = AS;

                // choppe le nom de l'animation
                if ((BufLine[0]=='N') && (BufLine[1]==':'))
                  while (BufLine[i]!='\t') texte[c++]=BufLine[i++];
                else MessageBoxA(NULL,"Erreur ds le fichier à la ligne n°"+ComptLine,InaName.c_str(),1);
                texte[c]='\0';
                // l'affecte à AS et traîte qq infos
                strcpy(AS->Nom,texte);
                AS->Numero = ValEtat(texte);

                // choppe le type de l'animation
                i++; c=0;
                if ((BufLine[i]=='T') && (BufLine[i+1]==':'))
                  { i+=2; while (BufLine[i]!='\t') texte[c++]=BufLine[i++]; }
                else MessageBoxA(NULL,"Erreur ds le fichier à la ligne n° ?",InaName.c_str(),1);
                texte[c]='\0';
                AS->Type = atoi(texte); // l'affecte à AS;

                // choppe le nombre d'image par orientation
                i++; c=0;
                if ((BufLine[i]=='C') && (BufLine[i+1]==':'))
                  { i+=2; while (BufLine[i]!='\t') texte[c++]=BufLine[i++]; }
                else MessageBoxA(NULL,"Erreur ds le fichier à la ligne n° ?",InaName.c_str(),1);
                texte[c]='\0';
                AS->NbSprites = atoi(texte); // l'affecte à AS;

                // suivant le type d'animation, choppe le n° d'image de départ ou la suite des n° d'images
                switch (AS->Type) {
                    case 1 :  // type 1
                    {	i++; c=0;
                    	if ((BufLine[i]=='I') && (BufLine[i+1]==':'))
                         { i+=2; while (BufLine[i]!='\n') texte[c++]=BufLine[i++]; }
                        else MessageBoxA(NULL,"Erreur ds le fichier à la ligne n° ?",InaName.c_str(),1);
                        texte[c]='\0';
                        AS->FirstSprite = atoi(texte);  // n°image de départ;
                    }; break;
                    case 2 :  // type 2
                    {   // Crée le tableau dynamique ou se trouveront les n° lus
                        for (int j=0; j<8; j++)
                           AS->SprRepertoire[j] =(int*) malloc(sizeof(int)*AS->NbSprites);

                        // puis lit les données
                        i+=1;
                        if (!((BufLine[i]=='I') && (BufLine[i+1]==':'))) MessageBoxA(NULL,"Erreur ds le fichier à la ligne n° ?",InaName.c_str(),1);
                        i+=2; // pointe sur le prochaine chiffre à lire
                        for (int j=0; j<8; j++) // choppe les n°images pour chaque orientation
                         	{
                               for (int n=0; n<AS->NbSprites ; n++)
                                 { c=0;
                                  while ((BufLine[i]!=',') && (BufLine[i]!=';') && (BufLine[i]!='\n'))
                                       texte[c++]=BufLine[i++];
                                  texte[c]='\0';
                                  AS->SprRepertoire[j][n] = atoi(texte);
                                  i++;
                                 };
                            }
                    }; break;
					 // Type 3 : même chose ke le type 1 sauf qu'ici il n'y a qu'1 orientation de caméra
					case 3 : 
                    {	i++; c=0;
                    	if ((BufLine[i]=='I') && (BufLine[i+1]==':'))
                        { i+=2; while ((BufLine[i]!='\n') && (BufLine[i]!='\t')) texte[c++]=BufLine[i++]; }
                        else {
							#ifdef CBUILDER
							MessageBoxA(NULL,"Erreur ds le fichier à la ligne n° ?",InaNameCommon,1);
							#else
							MessageBoxA(NULL,"Erreur ds le fichier à la ligne n° ?",LPCSTR(InaNameCommon),1);
							#endif
						}
                        texte[c]='\0';
                        AS->FirstSprite = atoi(texte);  // n°image de départ;
                    } break;

					// Type 4 : même chose ke pour le type 4 mais avec
					case 4 :  
                    {  	// Crée le tableau dynamique ou se trouveront les n° lus
						// mais pour une seule position de caméra et non 8
                        AS->SprRepertoire[0] =(int*) malloc(sizeof(int)*AS->NbSprites);

                        // puis lit les données
                        i+=1;
						if (!((BufLine[i]=='I') && (BufLine[i+1]==':'))) 
						{	
							#ifdef CBUILDER
							MessageBoxA(NULL,"Erreur ds le fichier à la ligne n° ?",InaNameCommon,1);
							#else
							MessageBoxA(NULL,"Erreur ds le fichier à la ligne n° ?",LPCSTR(InaNameCommon),1);
							#endif
						}
                        i+=2; // pointe sur le prochaine chiffre à lire
                        // choppe les n°images pour 1 seule orientation
                        for (int n=0; n<AS->NbSprites ; n++)
                        { c=0;
                          while ((BufLine[i]!=',') && (BufLine[i]!=';') && ((BufLine[i]!='\n') && (BufLine[i]!='\t')))
                               texte[c++]=BufLine[i++];
                          texte[c]='\0';
                          AS->SprRepertoire[0][n] = atoi(texte);
                          i++;
                        };
                    } break;
	            }

				// [New 10/02/00] : Vitesse d'animation
				if (BufLine[i] == '\t')
				{	i++; c=0;
					if ((BufLine[i]=='S') && (BufLine[i+1]==':'))
						{ i+=2; while (BufLine[i]!='\n') texte[c++]=BufLine[i++]; }
					else MessageBoxA(NULL,"Erreur ds le fichier à la ligne n° ?", InaNameCommon, 1);
					texte[c]='\0';
					AS->fAnimSpeed = (float) atof(texte);
				}

             	NumAnim++;
             } break;

       		case 2 : //  #transition
            {   // variables utilisées
                int c=0, i=2;	 	  // compteurs
             	char texte[128];      // buffer
                AnimTransition* AT;

                // réserve la mémoirez nécessaire
                AT = new AnimTransition();
                ATL[NumAnim] = AT;

                // choppe le nom de l'état en cours
                if ((BufLine[0]=='P') && (BufLine[1]==':'))
                  while (BufLine[i]!='\t') texte[c++]=BufLine[i++];
                else MessageBoxA(NULL,"Erreur ds le fichier à la ligne n°"+ComptLine,InaName.c_str(),1);
                texte[c]='\0';
                AT->NumActCour = ValEtat(texte);

                // choppe le nom du futur état
                i++; c=0;
                if ((BufLine[i]=='F') && (BufLine[i+1]==':'))
                  { i+=2; while (BufLine[i]!='\t') texte[c++]=BufLine[i++]; }
                else MessageBoxA(NULL,"Erreur ds le fichier à la ligne n° ?",InaName.c_str(),1);
                texte[c]='\0';
                AT->NumActFut = ValEtat(texte);

                // choppe le n°image A
                i++; c=0;
                if ((BufLine[i]=='A') && (BufLine[i+1]==':'))
                  { i+=2; while (BufLine[i]!='\t') texte[c++]=BufLine[i++]; }
                else MessageBoxA(NULL,"Erreur ds le fichier à la ligne n° ?",InaName.c_str(),1);
                texte[c]='\0';
                if (strcmp("n",texte)==0) AT->ImageA = -1;
                else AT->ImageA = atoi(texte);

                // choppe le n°image B
                i++; c=0;
                if ((BufLine[i]=='B') && (BufLine[i+1]==':'))
                  { i+=2; while (BufLine[i]!='\t') texte[c++]=BufLine[i++]; }
                else MessageBoxA(NULL,"Erreur ds le fichier à la ligne n° ?",InaName.c_str(),1);
                texte[c]='\0';
                if (strcmp("n",texte)==0) AT->ImageB = -1;
                else AT->ImageB = atoi(texte);

                // choppe le nom de l'état intermédiaire
                i++; c=0;
                if ((BufLine[i]=='I') && (BufLine[i+1]==':'))
                  { i+=2; while (BufLine[i]!='\n') texte[c++]=BufLine[i++]; }
                else MessageBoxA(NULL,"Erreur ds le fichier à la ligne n° ?",InaName.c_str(),1);
                texte[c]='\0';
                AT->NumActInterm = ValEtat(texte);

                NumAnim++;
            }; break;

            case 3 :  //  #frame
             {  // variables utilisées
                int c=0, i=2;	  // compteurs
             	char texte[128];  // buffer

                // 1) choppe le nom du bitmap et du spi qui sont identiques
                if ((BufLine[0]=='F') && (BufLine[1]==':'))
                  while (BufLine[i]!='\n') texte[c++]=BufLine[i++];
                else MessageBoxA(NULL,"Erreur ds le fichier à la ligne n°"+ComptLine,InaName.c_str(),1);
                texte[c]='\0';
                // l'affecte à AS et traîte qq infos
                AnsiString BmpSpiName = texte;
                AnsiString filePathBmp =  InaNameTresShort;
                filePathBmp = cPathPersos + filePathBmp + "\\" + texte + ".bmp";
                AnsiString filePathSpi =  InaNameTresShort;
                filePathSpi = cPathPersos + filePathSpi + "\\" + texte + ".spi";

                // 2) Vérifie si ce nom existe bien dans le fichier des IDBmpFrame
                if (!IDBmpFrame->IsCle(BmpSpiName))
                { MessageBoxA(NULL,"Nom de frame *.bmp inexistant à la ligne n° ?",InaName.c_str(),1);
                }

                // 3) Récupère le n°ID associé à cette frame
                int numIDFrame = IDBmpFrame->NumID(BmpSpiName);

                SFL->AddFrame(&DS_VA->pDD,*STLPerso,filePathBmp.c_str(),filePathSpi.c_str(),numIDFrame);

                // Ajoute le nom contenu dans "texte" à la lib
                (*STLPerso)->fileInaBmpName[NumAnim] = new char [strlen(texte)+1];
                strcpy(texte,(*STLPerso)->fileInaBmpName[NumAnim]);

             	NumAnim++;
             }; break;

             case 4 :  //  #frameSuperposee
             {  // variables utilisées
                int c=0, i=2;	  // compteurs
             	char texte[128];  // buffer

                // 1.a) choppe le nom du bitmap et du spi qui sont identiques de la frame superposée
                if ((BufLine[0]=='S') && (BufLine[1]==':'))
                  while (BufLine[i]!='\t') texte[c++]=BufLine[i++];
                else MessageBoxA(NULL,"Erreur ds frameSuperposée à la ligne n°"+ComptLine,InaName.c_str(),1);
                texte[c]='\0';
                // l'affecte à AS et traîte qq infos
                AnsiString BmpSpiName = texte;
                AnsiString filePathBmp =  InaNameTresShort;
                filePathBmp = cPathPersos + filePathBmp + "\\" + texte + ".bmp";
                AnsiString filePathSpi =  InaNameTresShort;
                filePathSpi = cPathPersos + filePathSpi + "\\" + texte + ".spi";

                // 1.b) Vérifie si ce nom existe bien dans le fichier des IDBmpFrame
                if (!IDBmpFrame->IsCle(BmpSpiName))
                   MessageBoxA(NULL,"Nom de frame inexistant à la ligne n° ?",InaName.c_str(),1);
                // 1.c) Récupère le n°ID associé à cette frame
                int numIDFrameSuperposee = IDBmpFrame->NumID(BmpSpiName);

                // 2.a) Récupère le nom de la frame existante sur laquelle doit s'appliquer les sprites de cette frame
                i++; c=0;
                if ((BufLine[i]=='F') && (BufLine[i+1]==':'))
                  { i+=2; while (BufLine[i]!='\t') texte[c++]=BufLine[i++]; }
                else MessageBoxA(NULL,"Erreur ds frameSuperposée à la ligne n° ?",InaName.c_str(),1);
                texte[c]='\0';
                AnsiString BmpFrameOriginal = texte;

                // 2.b) Vérifie si ce nom existe bien dans le fichier des IDBmpFrame
                if (!IDBmpFrame->IsCle(BmpFrameOriginal))
                {  MessageBoxA(NULL,"Numéro ou nom de frame Existant inexistant à la ligne n° ?",InaName.c_str(),1);
                }

                // 2.c) Récupère le n°ID associé à cette frame
                int numIDFrameOriginal = IDBmpFrame->NumID(BmpFrameOriginal);

                // 3.a) Récupère le nom du type d'objet correspondant à cette frame superposée
                i++; c=0;
                if ((BufLine[i]=='T') && (BufLine[i+1]==':'))
                  { i+=2; while (BufLine[i]!='\t') texte[c++]=BufLine[i++]; }
                else MessageBoxA(NULL,"Erreur ds frameSuperposée à la ligne n° ?",InaName.c_str(),1);
                texte[c]='\0';
                AnsiString typeObjSprite = texte;

                // 3.b) Vérifie si ce nom existe bien dans le fichier des cIDTypeObjSpriteFilename
                if (!IDTypeObjSprite->IsCle(typeObjSprite))
                   MessageBoxA(NULL,"Numéro ou nom de type d'Objet inexistant à la ligne n° ?",InaName.c_str(),1);

                // 3.c) Récupère le n°ID associé à ce type d'objet
                int numIDTypeObjSprite = IDTypeObjSprite->NumID(typeObjSprite);

                // 4.a) Récupère le nom du membre humain surlequel doit se positionner cette frame
                i++; c=0;
                if ((BufLine[i]=='M') && (BufLine[i+1]==':'))
                  { i+=2; while (BufLine[i]!='\t') texte[c++]=BufLine[i++]; }
                else MessageBoxA(NULL,"Erreur ds frameSuperposée à la ligne n° ?",InaName.c_str(),1);
                texte[c]='\0';
                AnsiString typeMembre = texte;

                // 4.b) Vérifie si ce nom existe bien dans le fichier des cIDPersosMembresFilename
                if (!IDMembres->IsCle(typeMembre))
                   MessageBoxA(NULL,"Numéro ou nom de type d'Objet inexistant à la ligne n° ?",InaName.c_str(),1);

                // 4.c) Récupère le n°ID associé à ce membre
                int numIDMembres = IDMembres->NumID(typeMembre);

                // 5.a) Récupère l'indication suivante :
                //        - MEM si le bmp doit toujours être chargé en mémoire
                //        - DD  si le bmp peut être laissé sur Disque Dur qd on ne s'en sert pas 
                i++; c=0;
                if ((BufLine[i]=='E') && (BufLine[i+1]==':'))
                  { i+=2; while (BufLine[i]!='\n') texte[c++]=BufLine[i++]; }
                else MessageBoxA(NULL,"Erreur ds frameSuperposée à la ligne n° ?",InaName.c_str(),1);
                texte[c]='\0';
                AnsiString typeLoadMemory = texte;

                // 5.b) Transforme cette indication en booléen
                bool loadBmpInMem;
                if (typeLoadMemory == "DD") loadBmpInMem = false;
                else if (typeLoadMemory == "MEM") loadBmpInMem = true;
                else MessageBoxA(NULL,"Type de mémoire (MEM ou DD) invalide à la ligne n° ?",InaName.c_str(),1);

                //*** 5) Complète les infos du Personnage ***
                // a) Ajoute un couple numObjet-numFrameSuperposee-numFrameOriginal perso
                Individu->ObjetFrame[numIDMembres].Add(numIDTypeObjSprite,numIDFrameSuperposee,numIDFrameOriginal);

                // b) Ajoute la frame bmp à la liste sans le charger mais en chargeant le .spi
                Individu->FrameSuperpList->AddFrame(&DS_VA->pDD, Individu->STLFrameSuperp,filePathBmp.c_str(),filePathSpi.c_str(),numIDFrameSuperposee,loadBmpInMem);

                // c) Ajoute le nom contenu dans "texte" à la lib
                Individu->STLFrameSuperp->fileInaBmpName[NumAnim] = new char [strlen(texte)+1];
                strcpy(texte,Individu->STLFrameSuperp->fileInaBmpName[NumAnim]);

                NumAnim++;
             }; break;


       };

      _FinDeLigne:
    	ComptLine++;
    };
	fclose(f);
//    if (ATL[5]->ImageB==1) Application->Terminate();
    *pATL = ATL;
    *pASL = ASL;
};

//=== Charge tout le fichier *.ina ===========================================//
// => fichier texte d'animations des DECORS
void LoadFileInaDecors(const char* InaNameTresShort,AnimSprite*** pASL, AnimTransition*** pATL,int &pNbAnim,int &pNbTrans,SpriteTilePersoLib **STLObjets, TAnimTileDecors *elem)
{
    // Chemin complet du 'Objets\InaNameTresShort.ina'
    AnsiString InaName = CheminApplication + cPathObjets + InaNameTresShort +".ina";

	char InaNameCommon[512];
    #ifdef CBUILDER
	strcpy(InaNameCommon, InaName.c_str());
	#else
	strcpy(InaNameCommon, LPCSTR(InaName));
	#endif // CBUILDER

    FILE *f;
    int ComptLine = 1;       // Compte le nombre de ligne
    int MaxLine = 512;	     // Nb maximal de caractères / ligne
    char BufLine[512];	     // Buffer de lecture d'une ligne du fichier
    int Etape = 0;	     // Etape du fichier

    int NbAnim;		     // Nombre d'animations ou de transitions ou de ...
    int NumAnim;	     // Curseur pour se repérer dans la Liste

    // Pointers à renvoyés à la fin de la fonction à l'appelante
    AnimSprite **ASL;
    AnimTransition **ATL;

    // Ouvre le fichier
    if ((f = fopen(InaName.c_str(),"r"))==NULL)
     {	MessageBoxA(NULL,"LoadFileInaDecors - FICHIER MANQUANT :",InaName.c_str(),1);
      	return;
     }

    // Scrute le fichier ligne par ligne
    while (fgets(BufLine, MaxLine, f) != NULL)  // tant que le fichier n'est pas fini
    {
   	if (BufLine[0]=='\n') goto _FinDeLigne; 				  		// Ligne vide
        if ((BufLine[0]=='/') && (BufLine[1]=='/')) goto _FinDeLigne;	// Commentaires

        // Changement d'étape
        if ((Etape!=1) || (Etape!=2) || (Etape!=3))
         if (BufLine[0]=='#')
          {
            char Entete[128];
            int i=1, curseur;
	    // Lit les caractères et les ranges ds Entete jusqu'à tabulation
            while (BufLine[i]!='\t') { Entete[i-1]=BufLine[i]; i++; };
            Entete[i-1]='\0';


            // Passe à l'étape souhaitée
            if (strcmp(Entete,"anim")==0) Etape = 1;
            else if (strcmp(Entete,"transition")==0) Etape = 2;
            else if (strcmp(Entete,"frame")==0) Etape = 3;
            else if (strcmp(Entete,"frameSuperposee")==0) Etape = 4;
            else if (strcmp(Entete,"ClassikAnim")==0) Etape = 5;


            // Puis récupère le nombre d'anim, de transition ou de ...
            curseur = ++i; i=0;
            while (BufLine[curseur]!='\n') { Entete[i] = BufLine[curseur]; i++; curseur++; };
            Entete[i]='\0';
            NbAnim = atoi(Entete);     // conversion chaîne de caractères -> integer
            NumAnim = 0;

            // Alloue de la mémoire suivant Etape & NbAnim
            // Alloue la mémoire nécessaire vers le tableau
            switch (Etape)  {
            	case  1 :      //  #anim
		    if ((ASL = (AnimSprite**) malloc(sizeof(AnimSprite*) * NbAnim)) == NULL)
         	           MessageBoxA(NULL,"Problème de mémoire","LoadFileIna",1);
                    pNbAnim = NbAnim;
                    break;
                case  2 :     //  #transition
                  if ((ATL = (AnimTransition**) malloc(sizeof(AnimTransition*) * NbAnim)) == NULL)
         	          MessageBoxA(NULL,"Problème de mémoire","LoadFileIna",1);
                    pNbTrans = NbAnim;
                    break;
                case  3 :     //  #frame
                   (*STLObjets)->nbFrames = NbAnim;
                   (*STLObjets)->fileInaBmpName = new char* [NbAnim];
                   break;
                case 4 :      // #frameSuperposee
                    elem->nbObjetFrames = NbAnim;
                    if (NbAnim!=0)elem->STLFrameSuperp->fileInaBmpName = new char* [NbAnim];
                   break;
                 case  5 :     //  #ClassikAnim
                   elem->peNbClassikAnim = NbAnim;
                   if (NbAnim != 0)  elem->ClassikAnimInfos = new SClassikAnimInfos[NbAnim];
                   break;
            };

        	// passe à la ligne suivante
            goto _FinDeLigne;
          };

        // Si l'on arrive jusque là, c'est que l'on est arrivé à une ligne importante de l'étape
        switch (Etape)
        {
             case 1 :  //  #anim
             {  // variables utilisées
                int c=0, i=2;	   // compteurs
             	char texte[128];   // buffer
                AnimSprite* AS;

                // crée le pointer vers l'objet et l'affecte à la liste
                AS = new AnimSprite();
                ASL[NumAnim] = AS;

                // choppe le nom de l'animation
                if ((BufLine[0]=='N') && (BufLine[1]==':'))
                  while (BufLine[i]!='\t') texte[c++]=BufLine[i++];
                else MessageBoxA(NULL,"Erreur ds le fichier à la ligne n°"+ComptLine,InaName.c_str(),1);
                texte[c]='\0';
                // l'affecte à AS et traîte qq infos
                strcpy(AS->Nom,texte);
                AS->Numero = ValEtat(texte);

                // choppe le type de l'animation
                i++; c=0;
                if ((BufLine[i]=='T') && (BufLine[i+1]==':'))
                  { i+=2; while (BufLine[i]!='\t') texte[c++]=BufLine[i++]; }
                else MessageBoxA(NULL,"Erreur ds le fichier à la ligne n° ?",InaName.c_str(),1);
                texte[c]='\0';
                AS->Type = atoi(texte); // l'affecte à AS;

                // choppe le nombre d'image par orientation
                i++; c=0;
                if ((BufLine[i]=='C') && (BufLine[i+1]==':'))
                  { i+=2; while (BufLine[i]!='\t') texte[c++]=BufLine[i++]; }
                else MessageBoxA(NULL,"Erreur ds le fichier à la ligne n° ?",InaName.c_str(),1);
                texte[c]='\0';
                AS->NbSprites = atoi(texte); // l'affecte à AS;

                // suivant le type d'animation, choppe le n° d'image de départ ou la suite des n° d'images
                switch (AS->Type) {
                    // type 0 : on ne tient ni compte de C:, ni de I:
                    // => 0 indique qu'il faudra prendre l'image bitmap du sprite au repos
                    case 0 :
                    {	i++; c=0;
                        // On doit récupérer le nom du tile qui nous donne son n°
                    	if ((BufLine[i]=='T') && (BufLine[i+1]==':'))
                         { i+=2; while (BufLine[i]!='\n') texte[c++]=BufLine[i++]; }
                        else MessageBoxA(NULL,"Erreur ds le fichier à la ligne n° ?",InaName.c_str(),1);
                        texte[c]='\0';
                        // Convertit le nom du tile en n°
                        AnsiString t = texte;
                        AS->FirstSprite = IDTiles->NumID(t); // n°ID du tileDecors

                    /*    if (AS->Numero == ARRET1)
                        { elem->NoDecorsNumAssoc = AS->FirstSprite; // n°ID du TilDecors Principal
                        }*/

                        // Si c'est le Tile Principal représentant l'anim (<=>ARRET1) , on le signale
                        if (AS->Numero == ARRET1)
                        { // DULib n'est pas encore chargé à ce niveau là !
                          elem->MainTile = DULib->getTileDecors(AS->FirstSprite);
                        } 

                    }; break;
                    case 1 :  // type 1 : 8 positions de caméra
                    {	i++; c=0;
                    	if ((BufLine[i]=='I') && (BufLine[i+1]==':'))
                         { i+=2; while ((BufLine[i]!='\n') && (BufLine[i]!='\t')) texte[c++]=BufLine[i++]; }
                        else MessageBoxA(NULL,"Erreur ds le fichier à la ligne n° ?",InaName.c_str(),1);
                        texte[c]='\0';
                        AS->FirstSprite = atoi(texte);  // n°image de départ;
                    }; break;
                    case 3 :  // type 3 : même chose ke le type 1 sauf qu'ici il n'y a qu'1 orientation de caméra
                    {	i++; c=0;
                    	if ((BufLine[i]=='I') && (BufLine[i+1]==':'))
                         { i+=2; while ((BufLine[i]!='\n') && (BufLine[i]!='\t')) texte[c++]=BufLine[i++]; }
                        else MessageBoxA(NULL,"Erreur ds le fichier à la ligne n° ?",InaName.c_str(),1);
                        texte[c]='\0';
                        AS->FirstSprite = atoi(texte);  // n°image de départ;
                    }; break;

                    case 2 :  // type 2
                    {   // Crée le tableau dynamique ou se trouveront les n° lus
                        for (int j=0; j<8; j++)
                           AS->SprRepertoire[j] =(int*) malloc(sizeof(int)*AS->NbSprites);

                        // puis lit les données
                        i+=1;
                        if (!((BufLine[i]=='I') && (BufLine[i+1]==':'))) MessageBoxA(NULL,"Erreur ds le fichier à la ligne n° ?",InaName.c_str(),1);
                        i+=2; // pointe sur le prochaine chiffre à lire
                        for (int j=0; j<8; j++) // choppe les n°images pour chaque orientation
                            	{
                               for (int n=0; n<AS->NbSprites ; n++)
                                 { c=0;
                                  while ((BufLine[i]!=',') && (BufLine[i]!=';') && ((BufLine[i]!='\n') && (BufLine[i]!='\t')))
                                       texte[c++]=BufLine[i++];
                                  texte[c]='\0';
                                  AS->SprRepertoire[j][n] = atoi(texte);
                                  i++;
                                 };
                            }
                    } break;

                    case 4 :  // type 4 : même chose ke le type 2 sauf qu'ici il n'y a qu'1 orientation de caméra
                    {   // Crée le tableau dynamique ou se trouveront les n° lus
                        AS->SprRepertoire[0] =(int*) malloc(sizeof(int)*AS->NbSprites);

                        // puis lit les données
                        i+=1;
                        if (!((BufLine[i]=='I') && (BufLine[i+1]==':'))) MessageBoxA(NULL,"Erreur ds le fichier à la ligne n° ?",InaName.c_str(),1);
                        i+=2; // pointe sur le prochaine chiffre à lire
                        // choppe les n°images pour 1 seule orientation
                        for (int n=0; n<AS->NbSprites ; n++)
                        { c=0;
                          while ((BufLine[i]!=',') && (BufLine[i]!=';') && ((BufLine[i]!='\n') && (BufLine[i]!='\t')))
                               texte[c++]=BufLine[i++];
                          texte[c]='\0';
                          AS->SprRepertoire[0][n] = atoi(texte);
                          i++;
                        }
                    } break;
                }
   				// [New 10/02/00] : rapidité de l'animation
				if (BufLine[i] == '\t')
				{	i++; c=0;
					if ((BufLine[i]=='S') && (BufLine[i+1]==':'))
						{ i+=2; while (BufLine[i]!='\n') texte[c++]=BufLine[i++]; }
					else MessageBoxA(NULL,"Erreur ds le fichier à la ligne n° ?", InaNameCommon, 1);
					texte[c]='\0';
					AS->fAnimSpeed = (float) atof(texte);
				}

             	NumAnim++;
             } break;

       		case 2 : //  #transition
            {   // variables utilisées
                int c=0, i=2;	      // compteurs
             	char texte[128];      // buffer
                AnimTransition* AT;

                // réserve la mémoirez nécessaire
                AT = new AnimTransition();
                ATL[NumAnim] = AT;

                // choppe le nom de l'état en cours
                if ((BufLine[0]=='P') && (BufLine[1]==':'))
                  while (BufLine[i]!='\t') texte[c++]=BufLine[i++];
                else MessageBoxA(NULL,"Erreur ds le fichier à la ligne n°"+ComptLine,InaName.c_str(),1);
                texte[c]='\0';
                AT->NumActCour = ValEtat(texte);

                // choppe le nom du futur état
                i++; c=0;
                if ((BufLine[i]=='F') && (BufLine[i+1]==':'))
                  { i+=2; while (BufLine[i]!='\t') texte[c++]=BufLine[i++]; }
                else MessageBoxA(NULL,"Erreur ds le fichier à la ligne n° ?",InaName.c_str(),1);
                texte[c]='\0';
                AT->NumActFut = ValEtat(texte);

                // choppe le n°image A
                i++; c=0;
                if ((BufLine[i]=='A') && (BufLine[i+1]==':'))
                  { i+=2; while (BufLine[i]!='\t') texte[c++]=BufLine[i++]; }
                else MessageBoxA(NULL,"Erreur ds le fichier à la ligne n° ?",InaName.c_str(),1);
                texte[c]='\0';
                if (strcmp("n",texte)==0) AT->ImageA = -1;
                else AT->ImageA = atoi(texte);

                // choppe le n°image B
                i++; c=0;
                if ((BufLine[i]=='B') && (BufLine[i+1]==':'))
                  { i+=2; while (BufLine[i]!='\t') texte[c++]=BufLine[i++]; }
                else MessageBoxA(NULL,"Erreur ds le fichier à la ligne n° ?",InaName.c_str(),1);
                texte[c]='\0';
                if (strcmp("n",texte)==0) AT->ImageB = -1;
                else AT->ImageB = atoi(texte);

                // choppe le nom de l'état intermédiaire
                i++; c=0;
                if ((BufLine[i]=='I') && (BufLine[i+1]==':'))
                  { i+=2; while (BufLine[i]!='\n') texte[c++]=BufLine[i++]; }
                else MessageBoxA(NULL,"Erreur ds le fichier à la ligne n° ?",InaName.c_str(),1);
                texte[c]='\0';
                AT->NumActInterm = ValEtat(texte);

                NumAnim++;
            }; break;

            case 3 :  //  #frame
             {  // variables utilisées
                int c=0, i=2;	  // compteurs
             	char texte[128];  // buffer

                // 1) choppe le nom du bitmap et du spi qui sont identiques
                if ((BufLine[0]=='F') && (BufLine[1]==':'))
                  while (BufLine[i]!='\n') texte[c++]=BufLine[i++];
                else MessageBoxA(NULL,"Erreur ds le fichier à la ligne n°"+ComptLine,InaName.c_str(),1);
                texte[c]='\0';
                // l'affecte à AS et traîte qq infos
                AnsiString BmpSpiName = texte;
                AnsiString filePathBmp =  InaNameTresShort;
                filePathBmp = cPathObjets + texte + ".bmp";
                AnsiString filePathSpi =  InaNameTresShort;
                filePathSpi = cPathObjets + texte + ".spi";

                // 2) Vérifie si ce nom existe bien dans le fichier des IDBmpFrame
                if (!IDBmpFrame->IsCle(BmpSpiName))
                   MessageBoxA(NULL,"Nom de frame *.bmp inexistant à la ligne n° ?",InaName.c_str(),1);

                // 3) Récupère le n°ID associé à cette frame
                int numIDFrame = IDBmpFrame->NumID(BmpSpiName);

                SFL->AddFrame(&DS_VA->pDD,*STLObjets,filePathBmp.c_str(),filePathSpi.c_str(),numIDFrame);

                // Ajoute le nom contenu dans "texte" à la lib
                (*STLObjets)->fileInaBmpName[NumAnim] = new char [strlen(texte)+1];
                strcpy(texte,(*STLObjets)->fileInaBmpName[NumAnim]);

             	NumAnim++;
             }; break;

             case 4 :  //  #frameSuperposee
             {  // variables utilisées
                int c=0, i=2;	  // compteurs
             	char texte[128];  // buffer

                // 1.a) choppe le nom du bitmap et du spi qui sont identiques de la frame superposée
                if ((BufLine[0]=='S') && (BufLine[1]==':'))
                  while (BufLine[i]!='\t') texte[c++]=BufLine[i++];
                else MessageBoxA(NULL,"Erreur ds frameSuperposée à la ligne n°"+ComptLine,InaName.c_str(),1);
                texte[c]='\0';
                // l'affecte à AS et traîte qq infos
                AnsiString BmpSpiName = texte;
                AnsiString filePathBmp =  InaNameTresShort;
                filePathBmp = cPathObjets + texte + ".bmp";
                AnsiString filePathSpi =  InaNameTresShort;
                filePathSpi = cPathObjets + texte + ".spi";

                // 1.b) Vérifie si ce nom existe bien dans le fichier des IDBmpFrame
                if (!IDBmpFrame->IsCle(BmpSpiName))
                   MessageBoxA(NULL,"Nom de frame inexistant à la ligne n° ?",InaName.c_str(),1);
                // 1.c) Récupère le n°ID associé à cette frame
                int numIDFrameSuperposee = IDBmpFrame->NumID(BmpSpiName);

                // 2.a) Récupère le nom de la frame existante sur laquelle doit s'appliquer les sprites de cette frame
                i++; c=0;
                if ((BufLine[i]=='F') && (BufLine[i+1]==':'))
                  { i+=2; while (BufLine[i]!='\t') texte[c++]=BufLine[i++]; }
                else MessageBoxA(NULL,"Erreur ds frameSuperposée à la ligne n° ?",InaName.c_str(),1);
                texte[c]='\0';
                AnsiString BmpFrameOriginal = texte;

                // 2.b) Vérifie si ce nom existe bien dans le fichier des IDBmpFrame
                if (!IDBmpFrame->IsCle(BmpFrameOriginal))
                   MessageBoxA(NULL,"Numéro ou nom de frame Existant inexistant à la ligne n° ?",InaName.c_str(),1);

                // 2.c) Récupère le n°ID associé à cette frame
                int numIDFrameOriginal = IDBmpFrame->NumID(BmpFrameOriginal);

                // 3.a) Récupère le nom du type d'objet correspondant à cette frame superposée
                i++; c=0;
                if ((BufLine[i]=='T') && (BufLine[i+1]==':'))
                  { i+=2; while (BufLine[i]!='\t') texte[c++]=BufLine[i++]; }
                else MessageBoxA(NULL,"Erreur ds frameSuperposée à la ligne n° ?",InaName.c_str(),1);
                texte[c]='\0';
                AnsiString typeObjSprite = texte;

                // 3.b) Vérifie si ce nom existe bien dans le fichier des cIDTypeObjSpriteFilename
                if (!IDTypeObjSprite->IsCle(typeObjSprite))
                   MessageBoxA(NULL,"Numéro ou nom de type d'Objet inexistant à la ligne n° ?",InaName.c_str(),1);

                // 3.c) Récupère le n°ID associé à ce type d'objet
                int numIDTypeObjSprite = IDTypeObjSprite->NumID(typeObjSprite);

                // 4.a) Récupère le nom du membre humain surlequel doit se positionner cette frame
                i++; c=0;
                if ((BufLine[i]=='M') && (BufLine[i+1]==':'))
                  { i+=2; while (BufLine[i]!='\t') texte[c++]=BufLine[i++]; }
                else MessageBoxA(NULL,"Erreur ds frameSuperposée à la ligne n° ?",InaName.c_str(),1);
                texte[c]='\0';
                AnsiString typeMembre = texte;

                // 4.b) Vérifie si ce nom existe bien dans le fichier des cIDPersosMembresFilename
                if (!IDMembres->IsCle(typeMembre))
                   MessageBoxA(NULL,"Numéro ou nom de type d'Objet inexistant à la ligne n° ?",InaName.c_str(),1);

                // 4.c) Récupère le n°ID associé à ce membre
                int numIDMembres = IDMembres->NumID(typeMembre);

                // 5.a) Récupère l'indication suivante :
                //        - MEM si le bmp doit toujours être chargé en mémoire
                //        - DD  si le bmp peut être laissé sur Disque Dur qd on ne s'en sert pas
                i++; c=0;
                if ((BufLine[i]=='E') && (BufLine[i+1]==':'))
                  { i+=2; while (BufLine[i]!='\n') texte[c++]=BufLine[i++]; }
                else MessageBoxA(NULL,"Erreur ds frameSuperposée à la ligne n° ?",InaName.c_str(),1);
                texte[c]='\0';
                AnsiString typeLoadMemory = texte;

                // 5.b) Transforme cette indication en booléen
                bool loadBmpInMem;
                if (typeLoadMemory == "DD") loadBmpInMem = false;
                else if (typeLoadMemory == "MEM") loadBmpInMem = true;
                else MessageBoxA(NULL,"Type de mémoire (MEM ou DD) invalide à la ligne n° ?",InaName.c_str(),1);

                //*** 5) Complète les infos du Personnage ***
                // a) Ajoute un couple numObjet-numFrameSuperposee-numFrameOriginal perso
                elem->ObjetFrame[numIDMembres].Add(numIDTypeObjSprite,numIDFrameSuperposee,numIDFrameOriginal);

                // b) Ajoute la frame bmp à la liste sans le charger mais en chargeant le .spi
                elem->FrameSuperpList->AddFrame(&DS_VA->pDD, elem->STLFrameSuperp,filePathBmp.c_str(),filePathSpi.c_str(),numIDFrameSuperposee,loadBmpInMem);

                // c) Ajoute le nom contenu dans "texte" à la lib
                elem->STLFrameSuperp->fileInaBmpName[NumAnim] = new char [strlen(texte)+1];
                strcpy(texte,elem->STLFrameSuperp->fileInaBmpName[NumAnim]);

                NumAnim++;
             }; break;

               case 5 :  //  #ClassikAnim
             {  // variables utilisées
                int c=0, i=2;	  // compteurs
             	char texte[128];  // buffer

                // 1) choppe le nom de l'anim classique
                if ((BufLine[0]=='N') && (BufLine[1]==':'))
                  while ((BufLine[i]!=' ') && (BufLine[i]!='\t')) texte[c++]=BufLine[i++];
                else MessageBoxA(NULL,"ClassikAnim (1): Erreur ds le fichier",InaName.c_str(),1);
                texte[c]='\0';
                TRACEDBG("Nom de l'anim : %s\n",texte);

                // 2) Vérifie si cette anim existe bien dans le fichier des IDAnimations
                AnsiString AnimName = texte;
                if (!IDAnimation->IsCle(AnimName))
                   MessageBoxA(NULL,"ClassikAnim (2): Erreur ds le fichier",InaName.c_str(),1);

                // 3) Récupère le n°ID associé à cette anim
                int numIDAnim = IDAnimation->NumID(AnimName);
                TRACEDBG("numIDAnim : %d\n",numIDAnim);

                // 4) Recherche une structure libre dans 'elem->ClassikAnimInfos' et y stock ce n° d'anim
                int index=0;
				while (    (elem->ClassikAnimInfos[index].iAnimID!=-1)
                        && (index<elem->peNbClassikAnim))
                {	index++; }
                elem->ClassikAnimInfos[index].iAnimID =  numIDAnim;

                // 5) Récupère le n° de l'anim associée sur laquelle on applique cette anim superposable
                i++; c=0;
                if ((BufLine[i]=='A') && (BufLine[i+1]==':'))
                {	i+=2;
                	while ((BufLine[i]!='\t') && (BufLine[i]!='\n'))
                    {	texte[c++]=BufLine[i++];
                    }
                }
                else MessageBoxA(NULL,"ClassikAnim (5): Nom de l'anim associée inexistante",InaName.c_str(),1);
                texte[c]='\0';

                // 6) Regarde si cette anim superposable n'est pas destinée à toutes les anims principales de l'objet
                if (strcmp(texte,"[ALL]")==0)
                {	elem->ClassikAnimInfos[index].iMainAnimID = -1;
                } else
                {	// 7) Vérifie si cette anim existe bien dans le fichier des IDAnimations
                	AnimName = texte;
                	if (!IDAnimation->IsCle(AnimName))
                   		MessageBoxA(NULL,"ClassikAnim (7): Nom de l'anim associée inexistant ds la lib",InaName.c_str(),1);

                	// 8) Récupère le n°ID associé à cette anim et l'ajoute à la liste
                	int numIDAnimAssocie = IDAnimation->NumID(AnimName);
                	elem->ClassikAnimInfos[index].iMainAnimID =  numIDAnimAssocie;
                }

             	NumAnim++;
             }; break;

       };

      _FinDeLigne:
    	ComptLine++;
    };
	fclose(f);
//    if (ATL[5]->ImageB==1) Application->Terminate();
    *pATL = ATL;
    *pASL = ASL;
};





