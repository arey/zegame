#ifdef CBUILDER
#include <vcl\system.hpp>	// pour les pointers NULL
#include <vcl\vcl.h>
#include "DirectLib.h"		// Header principal du jeu
#elif VISUALC
#include <cmath>            // for sqrt() and pow()
#endif

#include "ZeNain.h"			// Fichier commun à tout le projet
#include "consts.h"			// Constantes globales à tout le projet
#include "DDlib.h"			// Routines DirectDraw et gestionnaire de sprites des persos et objets
#include "D3DLib.h"			// Routines Direct3D principalement utilisée pour la transluence des textures
#include "DDLibAsm.h"           // Fonctions DirectDraw écrites en assembleur
#include "Animall.h"		// Gestion des sprites & animations des persos & objets
#ifdef NEW_FILE_MANAGER
#include "FileVirtual.h"	// Fichiers virtuels (dans un .pak, en mémoire ou tout simplement sur le disk)
#endif // NEW_FILE_MANAGER
#pragma hdrstop         	// Accéleration par préchargement des headers
#include "GameDebug.h"		// Libraire maison de debuggage
#include "SpritesLib.h"


bool GetLabel(char* label, char* texte, FILE *f)
{   int i=0, c=0;
	int MaxLine = 512;	    // Nb maximal de caractères / ligne
    char BufLine[512];	    // Buffer de lecture d'une ligne du fichier

    // Récupère la ligne courante
    if (fgets(BufLine, MaxLine, f) == NULL)  return false;	// fin du fichier

    // Saute les espaces et les tabulations du début de ligne
    while ((BufLine[i]==' ') || (BufLine[i]=='\t')) i++;

    // Récupère le label
    while ((BufLine[i]!=' ') && (BufLine[i]!='\t') && (BufLine[i]!='='))
    {	texte[c++]=BufLine[i++]; }
    texte[c]='\0';
    if (strcmp(texte, label) != 0) return false;	// ce n'est pas la bonne ligne

    // Saute les espaces, les tabs et le signe égal
    while ((BufLine[i]==' ') || (BufLine[i]=='\t') || (BufLine[i]=='=')) i++;

    // Récupère le champs demandé
    c=0;
    while ((BufLine[i]!=' ') && (BufLine[i]!='\t') && (BufLine[i]!='\n'))
    {	texte[c++]=BufLine[i++]; }
    texte[c]='\0';

    return true;
}
bool GetLabel(char* label, char* texte, TFileVirtual *f)
{   int i=0, c=0;
	int MaxLine = 512;	    // Nb maximal de caractères / ligne
    char BufLine[512];	    // Buffer de lecture d'une ligne du fichier

    // Récupère la ligne courante
    if (fgetsFV(BufLine, MaxLine, f) == NULL)  return false;	// fin du fichier

    // Saute les espaces et les tabulations du début de ligne
    while ((BufLine[i]==' ') || (BufLine[i]=='\t')) i++;

    // Récupère le label
    while ((BufLine[i]!=' ') && (BufLine[i]!='\t') && (BufLine[i]!='='))
    {	texte[c++]=BufLine[i++]; }
    texte[c]='\0';
    if (strcmp(texte, label) != 0) return false;	// ce n'est pas la bonne ligne

    // Saute les espaces, les tabs et le signe égal
    while ((BufLine[i]==' ') || (BufLine[i]=='\t') || (BufLine[i]=='=')) i++;

    // Récupère le champs demandé
    c=0;
    while ((BufLine[i]!=' ') && (BufLine[i]!='\t') && (BufLine[i]!='\n'))
    {	texte[c++]=BufLine[i++]; }
    texte[c]='\0';

    return true;
}




// Liste officielle des sprites ne faisant en particulier ni partie des objets, ni des persos
// mais pouvant être utilisés comme sprites superposés par plusieurs persos ou objets :
//  fummée, sang qui gicle, étincelles, explosions ...
SpriteTileGameLib *STGL = NULL;

//=== Charge tout le fichier *.ina ===========================================//
// => fichier texte d'animations des DECORS
void SpriteTileGameLib::LoadFileSpriteIna(const char* InaNameTresShort)
{
    // Chemin complet du 'Anims\InaNameTresShort.ina'
    char InaName[512];
	
	#ifndef NEW_FILE_MANAGER
	#ifdef CBUILDER
	strcpy(InaName, CheminApplication.c_str());
	#else
	strcpy(InaName, LPCSTR(CheminApplication));
	#endif // CBUILDER
	strcat(InaName, "Anims\\");
	strcat(InaName, InaNameTresShort);
	strcat(InaName, ".ina");
    FILE *f;
	// Ouvre le fichier
	if ((f = fopen(InaName,"r"))==NULL)
    {  MessageBoxA(NULL,"SpriteTileGameLib::LoadFileSpriteIna - FICHIER MANQUANT :",InaName,1);
       return;
    }
	#else // NEW_FILE_MANAGER
	TFileVirtual *f = NULL;
	strcpy(InaName, "Anims\\");
	strcat(InaName, InaNameTresShort);
	strcat(InaName, ".ina");
	// Ouvre le fichier
	f = fopenFV(InaName);
	if (f==NULL)	 { MessageBoxA(NULL,"SpriteTileGameLib::LoadFileSpriteIna - FICHIER MANQUANT :",InaName,1); return; }
	#endif // NEW_FILE_MANAGER


    int ComptLine = 1;      // Compte le nombre de ligne
    int MaxLine = 512;	    // Nb maximal de caractères / ligne
    char BufLine[512];	    // Buffer de lecture d'une ligne du fichier
    int Etape = 0;	     	// Etape du fichier
    int NbAnim;		     	// Nombre d'animations ou de ...
    int NumAnim = 0;     	// Curseur pour se repérer dans la Liste

    

   TRACEDBG("--- Début du chargement ---\n");

    // Scrute le fichier ligne par ligne
    #ifndef NEW_FILE_MANAGER
	while (fgets(BufLine, MaxLine, f) != NULL)  // tant que le fichier n'est pas fini
	#else
	while (fgetsFV(BufLine, MaxLine, f) != NULL)  // tant que le fichier n'est pas fini
	#endif  // NEW_FILE_MANAGER
    {
   		if (BufLine[0]=='\n') goto _FinDeLigne; 		 	  			// Ligne vide
        if ((BufLine[0]=='/') && (BufLine[1]=='/')) goto _FinDeLigne;	// Commentaires

        // Changement d'étape
        if (BufLine[0]=='#')
        {
            char Entete[128];
            int i=1, curseur;
	    // Lit les caractères et les ranges ds Entete jusqu'à tabulation
            while ((BufLine[i]!='\t') && (BufLine[i]!=' ') && (BufLine[i]!='\n'))
            { Entete[i-1]=BufLine[i]; i++;
            }
            Entete[i-1]='\0';


            // Passe à l'étape souhaitée
            if (strcmp(Entete,"frame")==0) Etape = 3;
            if (strcmp(Entete,"SimpleAnims")==0) Etape = 1;

            // Puis récupère le nombre de frames ou d'anims
            curseur = ++i; i=0;
            while ((BufLine[curseur]!='\n') && (BufLine[curseur]!=' ') && (BufLine[curseur]!='\n'))
            { Entete[i] = BufLine[curseur]; i++; curseur++;
            };
            Entete[i]='\0';
            NbAnim = atoi(Entete);     // conversion chaîne de caractères -> integer
            NumAnim = 0;

            // Alloue de la mémoire suivant Etape & NbAnim
            // Alloue la mémoire nécessaire vers le tableau
            switch (Etape)
            {  case  1 :      //  #SimpleAnims
		    		//if ((ASL = (AnimSprite**) malloc(sizeof(AnimSprite*) * NbAnim)) == NULL)
         	        //   MessageBoxA(NULL,"Problème de mémoire","LoadFileSpriteIna",1);
                //--DEBUGY--    pNbAnim = NbAnim;
                    break;
                case  3 :     //  #frame
                 	nbFrames = NbAnim;
                    fileInaBmpName = NULL;
                	//fileInaBmpName = new char* [NbAnim];
                   break;
            }

        	// passe à la ligne suivante
            goto _FinDeLigne;
          }

        // Si l'on arrive jusque là, c'est que l'on est arrivé à une ligne importante de l'étape
        switch (Etape)
        { //===--  #FRAME --====
          case 3 :
           {
           	  // variables utilisées
              int c=0, i=2;	  // compteurs
              char texte[128];  // buffer
              unsigned int cColor    = 0xEEFFFFFF;		// Composantes RGBA du sprite
              unsigned int cSpecular = 0x00000001;
              int iStartIndex = 0;						// index du 1er sprite de la frame
              int iTypeSprite;							// Type de sprite appartenant à la frame : 2D ou 3D
              bool is3DFrame = false;

              // 1) Récupère le nom du bitmap et du spi qui sont identiques
              if ((BufLine[0]=='F') && (BufLine[1]==':'))
                while ((BufLine[i]!=' ') && (BufLine[i]!='\t')) texte[c++]=BufLine[i++];
              else MessageBoxA(NULL,"Erreur ds le fichier à la ligne n°"+ComptLine,InaName,1);
              texte[c]='\0';
              TRACEDBG("Nom de la frame : %s\n",texte);
              // l'affecte à AS et traîte qq infos
	          char BmpSpiName[512];
			  strcpy(BmpSpiName, texte);
              char filePathBmp[512];
			  strcpy(filePathBmp, "Anims\\");
              strcat(filePathBmp, texte);
			  strcat(filePathBmp, ".bmp");
              char filePathSpi[512];
			  strcpy(filePathSpi, "Anims\\");
			  strcat(filePathSpi, texte );
			  strcat(filePathSpi, ".spi");

              // 2) Vérifie si ce nom existe bien dans le fichier des IDBmpFrame
              if (!IDBmpFrame->IsCle(BmpSpiName))
                 MessageBoxA(NULL,"Nom de frame *.bmp inexistant à la ligne n° ?",InaName,1);

              // 3) Récupère le n°ID associé à cette frame
			  AnsiString BmpSpiName2 = BmpSpiName; 	
              int numIDFrame = IDBmpFrame->NumID(BmpSpiName2);
              TRACEDBG("NumIDFrame : %d\n",numIDFrame);
              TRACEDBG("Nom du bmp : %s\n",filePathBmp);

              // 4) Récupère l'offset de départ (iStartIndex) du 1er sprite de cette frame
              i++; c=0;
              if ((BufLine[i]=='D') && (BufLine[i+1]==':'))
              { i+=2;
                while ((BufLine[i]!='\t') && (BufLine[i]!=' ')) texte[c++]=BufLine[i++];
              } else MessageBoxA(NULL,"Erreur ds le fichier à la ligne n° ?",InaName,1);
              texte[c]='\0';
              iStartIndex = atoi(texte);
              TRACEDBG("StartIndex : %d\n", iStartIndex);

              // 5)  Récupère le type de sprite auxquel on a affaire dans cette frame : 2D ou 3D
              i++; c=0;
              if ((BufLine[i]=='T') && (BufLine[i+1]==':'))
              { i+=2;  while ((BufLine[i]!='\t') && (BufLine[i]!=' ') && (BufLine[i]!='\n')) texte[c++]=BufLine[i++];
              } else MessageBoxA(NULL,"Erreur ds le fichier à la ligne n° ?",InaName,1);
              texte[c]='\0';
              if (strcmp("3D",texte) == 0)
              {	iTypeSprite = SPRITE_TILE_3D; is3DFrame = true;
              } else
              { iTypeSprite = SPRITE_TILE_2D; is3DFrame = false;
              }
              TRACEDBG("iTypeSprite : %d  /  texte : %s\n", iTypeSprite, texte);

              // 6) Si on a affaire a un sprite3D, on choppe les composantes color et specular
              if (iTypeSprite == SPRITE_TILE_3D)
              {  // cColor
                 i++; c=0;
              	 if ((BufLine[i]=='C') && (BufLine[i+1]==':'))
                 { i+=2;  while ((BufLine[i]!='\t') && (BufLine[i]!=' ') && (BufLine[i]!='\n')) texte[c++]=BufLine[i++];
              	 } else MessageBoxA(NULL,"Erreur ds le fichier à la ligne n° ?",InaName,1);
              	texte[c]='\0';
                cColor = hexatoi(texte);
                TRACEDBG("cColor : %d\n",cColor);
                // cSpecular
                 i++; c=0;
              	 if ((BufLine[i]=='S') && (BufLine[i+1]==':'))
                 { i+=2;  while ((BufLine[i]!='\t') && (BufLine[i]!=' ') && (BufLine[i]!='\n')) texte[c++]=BufLine[i++];
              	 } else MessageBoxA(NULL,"Erreur ds le fichier à la ligne n° ?",InaName,1);
              	texte[c]='\0';
                cSpecular = hexatoi(texte);
                TRACEDBG("cSpecular : %d\n",cSpecular);
              }

              // 7) Ajoute la frame à la banque de frame du jeu [PAR DEFAUT : en mémoire]
              SpriteFrame *New_Frame = SFL->AddFrame(DS,NULL,filePathBmp, filePathSpi, numIDFrame, true, is3DFrame);
              New_Frame->iStartSpriteIndex = iStartIndex;
              // 8) Ajoute les sprites inclues dans cette frame
			  AddFrameSprites(filePathSpi, New_Frame, iStartIndex, cColor, cSpecular);

              NumAnim++;
           } break;

           //===--  #SimpleAnims --===
           case 1 :
           {  // variables utilisées
              int c=0, i=1;	   		// compteurs
              char texte[128];   	// buffer
              AnimSprite* AS;       // Nouveal AnimSprite

              AS = new AnimSprite();	// Alloue une instance d'AnimSprite

              // 1) Récupère le nom de l'animation
              if (BufLine[0]=='[')
              {  while (BufLine[i]!=']') texte[c++]=BufLine[i++]; }
              else MessageBoxA(NULL,"Erreur ds le fichier : ",InaName,1);
              texte[c]='\0';
              strcpy(AS->Nom,texte);
              char AnimName[512];
			  strcpy(AnimName, texte);
              TRACEDBG("Nom de l'anim : %s\n", texte);

              // 2) Vérifie si ce nom existe bien dans le fichier des IDBmpFrame
              if (!IDAnimation->IsCle(AnimName))
              {	 AnsiString error = "Nom de l'anim inexistant : ";
				 error+= AnimName;
                 MessageBoxA(NULL,error, InaName,1);
              }
              // 3) Récupère le n°ID associé à cette frame
			  AnsiString sAnimName = AnimName;
              int numIDAnim = IDAnimation->NumID(sAnimName);
              TRACEDBG("N° de l'anim : %d\n",numIDAnim);

              // 4) Récupère le nombre d'image par orientation
			  if (!GetLabel("NbSprites", texte, f))
              {  AnsiString error = "Champs 'NbSprites' inexistant !";
                 MessageBoxA(NULL,error, InaName,1);
              }
              TRACEDBG("NbSprites : %s\n",texte);
              AS->NbSprites = atoi(texte); 			// l'affecte à l'AnimSprite;

              // 5) Récupère le type de l'animation
	          if (!GetLabel("Type", texte, f))
              {  AnsiString error = "Champs 'Type' inexistant !";
                 MessageBoxA(NULL,error, InaName,1);
              }
              TRACEDBG("Type : %s\n",texte);
              AS->Type = atoi(texte);				// l'affecte à l'AnimSprite;

              // 6) suivant le type d'animation, choppe le n° d'image de départ ou la suite des n° d'images
              switch (AS->Type)
              {
                  case 3 :  // 1 seule orientation de caméra (et non 8)
                  { // Récupère le label derrière "Index = "
                  	if (!GetLabel("Index", BufLine, f))
              		{  AnsiString error = "Champs 'Index' inexistant !";
                 	   MessageBoxA(NULL, error, InaName, 1);
              		}
                    // Choppe le nom de la frame ds lequel se situe ce sprite
                    c = 0; i = 1;
                    if (BufLine[0]=='@')
                    {	while ((BufLine[i]!=',') && (BufLine[i]!=' ') && (BufLine[i]!='\n') && (BufLine[i]!='\t'))
                    	{ texte[c++]=BufLine[i++];
                    	}
                    } else MessageBoxA(NULL,"Syntaxe de frame incorrecte",InaName,1);
                    AnsiString Frame = texte;
              		if (!IDBmpFrame->IsCle(Frame)) 	MessageBoxA(NULL,"Nom de frame inexistant ?",InaName,1);
					int numIDFrame = IDBmpFrame->NumID(Frame); // Récupère le n°ID associé à cette frame
                    TRACEDBG("NumIDFrame : %d\n", numIDFrame);

                    // Saute les virgules, espaces et tabs
                    while ((BufLine[i]==',') || (BufLine[i]==' ') || (BufLine[i]=='\t')) i++;

                    // A partir de l'ID de la frame, récupère l'offset de départ de l'anim
                    c=0;
                    while ((BufLine[i]!=',') && (BufLine[i]!=' ') && (BufLine[i]!='\n') && (BufLine[i]!='\t'))
                    { texte[c++]=BufLine[i++];
                    }
                    int offset = atoi(texte);

					SpriteFrame *frame = SFL->GetFrame(numIDFrame);
                    if (frame==NULL)	MessageBoxA(NULL,"Frame inexistante",InaName,1);
                    offset += frame->iStartSpriteIndex;
                    TRACEDBG("Offset : %d\n", offset);
                    AS->FirstSprite = offset;  // n°image de départ;
                  } break;
              }

              // 7) Regarde si ce sont des sprites 2D ou 3D
              if (!GetLabel("Direct3D", texte, f))
              {  AnsiString error = "Champs 'Direct3D' inexistant !";
                 MessageBoxA(NULL, error, InaName, 1);
              }
              TRACEDBG("Direct3D : %s\n",texte);
              bool sprite3D = false;
              if (strcmp(texte,"YES")==0) sprite3D = true;

              // 8) Si c'est un sprite 3D, on récupère les composantes alpha et specular
              if (sprite3D)
              { unsigned int cColor    = 0xEEFFFFFF;		// Composantes RGBA du sprite
              	unsigned int cSpecular = 0x00000001;
              	// ColorAlpha
              	if (!GetLabel("ColorAlpha", texte, f))
                {  AnsiString error = "Champs 'ColorAlpha' inexistant !";
                   MessageBoxA(NULL, error, InaName, 1);
                }
                TRACEDBG("ColorAlpha : %s\n",texte);
                if (strcmp(texte,"default")!=0)
                {	cColor = hexatoi(texte);
                }
                // ColorSpecular
              	if (!GetLabel("ColorSpecular", texte, f))
                {  AnsiString error = "Champs 'ColorSpecular' inexistant !";
                   MessageBoxA(NULL, error, InaName, 1);
                }
                TRACEDBG("ColorSpecular : %s\n",texte);
                if (strcmp(texte,"default")!=0)
                {	cSpecular = hexatoi(texte);
                }
              }

              // Ajoute cette anim au dico
              AnimsMap.insert(std::make_pair(numIDAnim, AS));

              NumAnim++;
           }; break;

        }
      _FinDeLigne:
      ComptLine++;
    }

	// On referme gentillement le fichier
	#ifndef NEW_FILE_MANAGER
	fclose(f);
	#else
	fcloseFV(f);
	#endif // NEW_FILE_MANAGER

//--DEBUGY--    *pATL = ATL;
//--DEBUGY--    *pASL = ASL;
}

//=== Ajoute une banque de sprites appartenant à une frame ===================//
void  SpriteTileGameLib::AddFrameSprites(char *SpiFileName, SpriteFrame *pFr, int startIndex, unsigned int cColor, unsigned int cSpecular)
{ 	FrInfo Spi;
	#ifndef NEW_FILE_MANAGER
	FILE *f;
	#else
	TFileVirtual *file = NULL;
	#endif	// NEW_FILE_MANAGER

    // Vérifie l'état des noms du fichier et du pointeur
    if ((SpiFileName==NULL) || (pFr==NULL))
            MessageError("SpritesLib::AddSprites","Noms de fichiers non valides",99);

    // Ouvre le .SPI et lit l'en-tête
	#ifndef NEW_FILE_MANAGER
	AnsiString pathSpi = CheminApplication + SpiFileName;
	#ifdef CBULDER
	if ((f = fopen(pathSpi.c_str(),"rb"))==NULL) 
	#else
	if ((f = fopen(LPCSTR(pathSpi),"rb"))==NULL) 
	#endif // CBULDER
	{	MessageError("SpritesLib::AddSprites","N'arrive pas à ouvrir le fichier",99);
	}
    fread(&Spi,sizeof(Spi),1,f);
	#else
	file = fopenFV(SpiFileName);
	if (file == NULL) MessageError("SpritesLib::AddSprites","N'arrive pas à ouvrir le fichier",99);
	file->read(&Spi, sizeof(Spi));
	#endif // NEW_FILE_MANAGER

	// S'occupe de répartir les sprites ds leur librairie
    SprInfo SI;
    int index = startIndex;
    for (int compt=0; compt < Spi.NbSprites; compt++)
    {  
		#ifndef NEW_FILE_MANAGER
		fread(&SI,sizeof(SprInfo),1,f);
		#else
		file->read(&SI, sizeof(SprInfo));
		#endif // NEW_FILE_MANAGER
		AddOneSprite(SI,Spi.CoulFond, Spi.CoulOmbr, Spi.cfRGB, Spi.coRGB, cColor, cSpecular, pFr, index);
		index++;
    }
	#ifndef NEW_FILE_MANAGER
	fclose(f);
	#else
	fcloseFV(file);
	#endif	// NEW_FILE_MANAGER
}
//----------------------------------------------------------------------------//

//=== Crée et ajoute un sprite au dictionnaire ===============================//
void SpriteTileGameLib::AddOneSprite(SprInfo InfSprite,int CF,int CO,unsigned int RGBcf,unsigned int RGBco,
									  unsigned int cColor, unsigned int cSpecular, SpriteFrame *pFr, int index)
{
	SpriteTile *New_Node;

    // Crée un nouveau Sprite 3D ou 2D
    if (pFr->isTextureDirect3D)
    {   // Calcul des cordonnées de texture d'apès la position du sprite sur sa frame
        //  -> comprises entre [0..1]
        float posX1, posY1, posX2, posY2;
        posX1 = ((float) InfSprite.CoordX) / pFr->PixWidth;
        posY1 = ((float) InfSprite.CoordY) / pFr->PixHeight;
        posX2 = posX1 + ((float) InfSprite.Width) / pFr->PixWidth;
        posY2 = posY1 + ((float) InfSprite.Height) / pFr->PixHeight;

    	New_Node = new Sprite3DTile();
		AddSpriteTile3D(New_Node);
    	((Sprite3DTile*) New_Node)->Create3DSprite(D3DS->Get3DDevice(), cColor, cSpecular, posX1, posY1, posX2, posY2, *pFr);
    }
    else
    {	New_Node = new SpriteTile();
    }

    // Lui injecte toutes les infos
    New_Node->CoordX = InfSprite.CoordX;		// Positions
    New_Node->CoordY = InfSprite.CoordY;
    New_Node->Width  = InfSprite.Width;			// Dimension
    New_Node->Height = InfSprite.Height;
    New_Node->GravX  = InfSprite.GravX;			// Centre de "gravité"
    New_Node->GravY  = InfSprite.GravY;
    New_Node->CoulFond = CF;					// Index ds la palette 256c de la couleur de fond et d'ombre
    New_Node->CoulOmbr = CO;
    New_Node->cfRGB = RGBcf;					// Composantes des couleurs de fond et d'ombre
    New_Node->coRGB = RGBco;
    New_Node->NumFrame = pFr->NumFrame;			// N° et pointer sur sa frame
    New_Node->pFrame = pFr;
    New_Node->NumSprite = index;
    New_Node->SrcRect.left = InfSprite.CoordX;	// Rectangle source
    New_Node->SrcRect.top = InfSprite.CoordY;
    New_Node->SrcRect.right = InfSprite.CoordX+InfSprite.Width;
    New_Node->SrcRect.bottom = InfSprite.CoordY+InfSprite.Height;
	New_Node->AlphaBlend = (unsigned char) (cColor >> 24);

    // Ajoute tout simplement ce sprite au dictionnaire
    SpriteMap.insert(std::make_pair(index, New_Node));
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
SpriteTileGameLib::~SpriteTileGameLib()
{	// Désalloue tous les sprites de la librairie
	for (TSpriteMap::iterator iter = SpriteMap.begin(); iter!=SpriteMap.end() ; iter++)
    {   SpriteTile*  sprite = (*iter).second;
		delete sprite;
    }
    // Puis vide le dictionnaire
    SpriteMap.clear();

    // Désalloue toutes les anims de la librairie et vide le dico
	for (TAnimsMap::iterator iter2 = AnimsMap.begin(); iter2!=AnimsMap.end() ; iter2++)
    {   AnimSprite*  anim = (*iter2).second;
		delete anim;
    }
    // Puis vide le dictionnaire
    AnimsMap.clear();

}
//----------------------------------------------------------------------------//

//=== Joue une anim en (x,y) =================================================//
void SpriteTileGameLib::PlayAnim(int animID, int numSprite, int x, int y)
{	AnimSprite* anim = AnimsMap[animID];
 	if (anim==NULL) TRACEDBG("Animation n° %d non trouvée dans SpriteTileGameLib::PlayAnim\n", animID);
	int spriteID = anim->FirstSprite + numSprite;
    SpriteTile* sprite = SpriteMap[spriteID];
    if (sprite->TypeSprite == SPRITE_TILE_3D)
    {	if (SCREEN_DIRECT3D)
		{	// Utilisation de Direct3D
			((Sprite3DTile*) sprite)->SpriteRender(x, y);
		} else
		{	// Fonction d'alpha-blending émulée par voie logicielle
			SpriteDrawTransluenceClip(x, y, DS->pDDSBack, sprite, sprite->AlphaBlend);
		}
    } else
    {

    }
}

void SpriteTileGameLib::PlayAnim(Sprite2Play &spr)
{	PlayAnim(spr.animID, spr.numSprite, spr.x, spr.y);
}
//----------------------------------------------------------------------------//

//=== Renvoie un pointeur vers le sprite n°numID =============================//
SpriteTile*	SpriteTileGameLib::GetSpriteTile(int numID)
{	return SpriteMap[numID];
}
//----------------------------------------------------------------------------//

//=== Renvoie un pointeur vers l'animation n°numID ===========================//
AnimSprite*	SpriteTileGameLib::GetAnimSprite(int numID)
{	return AnimsMap[numID];
}
//----------------------------------------------------------------------------//


//--- Convertit une chaîne de caractère exprimée en hexa "0xF0E123" en entier //
unsigned int hexatoi(char* str)
{	int mul2 = 0;
	unsigned int number = 0;
    for (int i = strlen(str)-1 ; i >= 0 ; i--)
    {	char car = str[i];
        int num;
    	if ((car>='0') && (car<='9'))
        {  num = int(car - '0');
        } else
        if ((car>='A') && (car<='F'))
        {  num = 10 + int(car - 'A');
        } else
        if ((car=='x') || (car=='X')) return number;
        else return 0;  // erreur
        number += (unsigned int) (num * pow(16, mul2));
        mul2++;
    }
    return 0;		// on n'a pas rencontré le caractère 'x' ou 'X' exprimant que l'on a affaire a un nb en héxa
}
