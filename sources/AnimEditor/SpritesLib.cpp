#include <vcl\system.hpp>  // pour les pointers NULL
#include <vcl\vcl.h>

#include "DDlibEditor.h"	// Routines DirectDraw et gestionnaire de sprites des persos et objets
#include "D3DLib.h"			// Routines Direct3D principalement utilis�e pour la transluence des textures
#include "Animall.h"		// Gestion des sprites & animations des persos & objets
#include "fPrincipale.h"		// Header principal du jeu
#pragma hdrstop         	// Acc�leration par pr�chargement des headers
#include "SpritesLib.h"
#include "GameDebug.h"


bool GetLabel(char* label, char* texte, FILE *f)
{   int i=0, c=0;
	int MaxLine = 512;	    // Nb maximal de caract�res / ligne
    char BufLine[512];	    // Buffer de lecture d'une ligne du fichier

    // R�cup�re la ligne courante
    if (fgets(BufLine, MaxLine, f) == NULL)  return false;	// fin du fichier

    // Saute les espaces et les tabulations du d�but de ligne
    while ((BufLine[i]==' ') || (BufLine[i]=='\t')) i++;

    // R�cup�re le label
    while ((BufLine[i]!=' ') && (BufLine[i]!='\t') && (BufLine[i]!='='))
    {	texte[c++]=BufLine[i++]; }
    texte[c]='\0';
    if (strcmp(texte, label) != 0) return false;	// ce n'est pas la bonne ligne

    // Saute les espaces, les tabs et le signe �gal
    while ((BufLine[i]==' ') || (BufLine[i]=='\t') || (BufLine[i]=='=')) i++;

    // R�cup�re le champs demand�
    c=0;
    while ((BufLine[i]!=' ') && (BufLine[i]!='\t') && (BufLine[i]!='\n'))
    {	texte[c++]=BufLine[i++]; }
    texte[c]='\0';

    return true;
}



// Liste officielle des sprites ne faisant en particulier ni partie des objets, ni des persos
// mais pouvant �tre utilis�s comme sprites superpos�s par plusieurs persos ou objets :
//  fumm�e, sang qui gicle, �tincelles, explosions ...
SpriteTileGameLib *STGL = NULL;

//=== Charge tout le fichier *.ina ===========================================//
// => fichier texte d'animations des DECORS
void SpriteTileGameLib::LoadFileSpriteIna(const char* InaNameTresShort)
{
    // Chemin complet du 'Anims\InaNameTresShort.ina'
    AnsiString InaName = CheminApplication + "Anims\\" + InaNameTresShort +".ina";

    FILE *f;
    int ComptLine = 1;      // Compte le nombre de ligne
    int MaxLine = 512;	    // Nb maximal de caract�res / ligne
    char BufLine[512];	    // Buffer de lecture d'une ligne du fichier
    int Etape = 0;	     	// Etape du fichier
    int NbAnim;		     	// Nombre d'animations ou de ...
    int NumAnim;	     	// Curseur pour se rep�rer dans la Liste

    // Pointers � renvoy�s � la fin de la fonction � l'appelante
    AnimSprite **ASL;

    // Ouvre le fichier
    if ((f = fopen(InaName.c_str(),"r"))==NULL)
    {  MessageBoxA(NULL,"SpriteTileGameLib::LoadFileSpriteIna - FICHIER MANQUANT :",InaName.c_str(),1);
       return;
    }

   TRACEDBG("--- D�but du chargement ---\n");

    // Scrute le fichier ligne par ligne
    while (fgets(BufLine, MaxLine, f) != NULL)  // tant que le fichier n'est pas fini
    {
   		if (BufLine[0]=='\n') goto _FinDeLigne; 		 	  			// Ligne vide
        if ((BufLine[0]=='/') && (BufLine[1]=='/')) goto _FinDeLigne;	// Commentaires

        // Changement d'�tape
        if (BufLine[0]=='#')
        {
            char Entete[128];
            int i=1, curseur;
	    // Lit les caract�res et les ranges ds Entete jusqu'� tabulation
            while ((BufLine[i]!='\t') && (BufLine[i]!=' ') && (BufLine[i]!='\n'))
            { Entete[i-1]=BufLine[i]; i++;
            }
            Entete[i-1]='\0';


            // Passe � l'�tape souhait�e
            if (strcmp(Entete,"frame")==0) Etape = 3;
            if (strcmp(Entete,"SimpleAnims")==0) Etape = 1;

            // Puis r�cup�re le nombre de frames ou d'anims
            curseur = ++i; i=0;
            while ((BufLine[curseur]!='\n') && (BufLine[curseur]!=' ') && (BufLine[curseur]!='\n'))
            { Entete[i] = BufLine[curseur]; i++; curseur++;
            };
            Entete[i]='\0';
            NbAnim = atoi(Entete);     // conversion cha�ne de caract�res -> integer
            NumAnim = 0;


            // Alloue de la m�moire suivant Etape & NbAnim
            // Alloue la m�moire n�cessaire vers le tableau
            switch (Etape)
            {  case  1 :      //  #SimpleAnims
		    		//if ((ASL = (AnimSprite**) malloc(sizeof(AnimSprite*) * NbAnim)) == NULL)
         	        //   MessageBoxA(NULL,"Probl�me de m�moire","LoadFileSpriteIna",1);
                //--DEBUGY--    pNbAnim = NbAnim;
                    break;
                case  3 :     //  #frame
                 	nbFrames = NbAnim;
                    fileInaBmpName = NULL;
                	//fileInaBmpName = new char* [NbAnim];
                   break;
            }

        	// passe � la ligne suivante
            goto _FinDeLigne;
          }

        // Si l'on arrive jusque l�, c'est que l'on est arriv� � une ligne importante de l'�tape
        switch (Etape)
        { //===--  #FRAME --====
          case 3 :
           {
           	  // variables utilis�es
              int c=0, i=2;	  // compteurs
              char texte[128];  // buffer
              unsigned int cColor    = 0xEEFFFFFF;		// Composantes RGBA du sprite
              unsigned int cSpecular = 0x00000001;
              int iStartIndex = 0;						// index du 1er sprite de la frame
              int iTypeSprite;							// Type de sprite appartenant � la frame : 2D ou 3D
              bool is3DFrame = false;

              // 1) R�cup�re le nom du bitmap et du spi qui sont identiques
              if ((BufLine[0]=='F') && (BufLine[1]==':'))
                while ((BufLine[i]!=' ') && (BufLine[i]!='\t')) texte[c++]=BufLine[i++];
              else MessageBoxA(NULL,"Erreur ds le fichier � la ligne n�"+ComptLine,InaName.c_str(),1);
              texte[c]='\0';
              TRACEDBG("Nom de la frame : %s\n",texte);
              // l'affecte � AS et tra�te qq infos
	          AnsiString BmpSpiName = texte;
              AnsiString filePathBmp = "Anims\\";
              filePathBmp =  filePathBmp + texte + ".bmp";
              AnsiString filePathSpi = "Anims\\";
              filePathSpi = filePathSpi + texte + ".spi";

              // 2) V�rifie si ce nom existe bien dans le fichier des IDBmpFrame
              if (!IDBmpFrame->IsCle(BmpSpiName))
                 MessageBoxA(NULL,"Nom de frame *.bmp inexistant � la ligne n� ?",InaName.c_str(),1);

              // 3) R�cup�re le n�ID associ� � cette frame
              int numIDFrame = IDBmpFrame->NumID(BmpSpiName);
              TRACEDBG("NumIDFrame : %d\n",numIDFrame);
              TRACEDBG("Nom du bmp : %s\n",filePathBmp.c_str());

              // 4) R�cup�re l'offset de d�part (iStartIndex) du 1er sprite de cette frame
              i++; c=0;
              if ((BufLine[i]=='D') && (BufLine[i+1]==':'))
              { i+=2;
                while ((BufLine[i]!='\t') && (BufLine[i]!=' ')) texte[c++]=BufLine[i++];
              } else MessageBoxA(NULL,"Erreur ds le fichier � la ligne n� ?",InaName.c_str(),1);
              texte[c]='\0';
              iStartIndex = atoi(texte);
              TRACEDBG("StartIndex : %d\n", iStartIndex);

              // 5)  R�cup�re le type de sprite auxquel on a affaire dans cette frame : 2D ou 3D
              i++; c=0;
              if ((BufLine[i]=='T') && (BufLine[i+1]==':'))
              { i+=2;  while ((BufLine[i]!='\t') && (BufLine[i]!=' ') && (BufLine[i]!='\n')) texte[c++]=BufLine[i++];
              } else MessageBoxA(NULL,"Erreur ds le fichier � la ligne n� ?",InaName.c_str(),1);
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
              	 } else MessageBoxA(NULL,"Erreur ds le fichier � la ligne n� ?",InaName.c_str(),1);
              	texte[c]='\0';
                cColor = hexatoi(texte);
                TRACEDBG("cColor : %d\n",cColor);
                // cSpecular
                 i++; c=0;
              	 if ((BufLine[i]=='S') && (BufLine[i+1]==':'))
                 { i+=2;  while ((BufLine[i]!='\t') && (BufLine[i]!=' ') && (BufLine[i]!='\n')) texte[c++]=BufLine[i++];
              	 } else MessageBoxA(NULL,"Erreur ds le fichier � la ligne n� ?",InaName.c_str(),1);
              	texte[c]='\0';
                cSpecular = hexatoi(texte);
                TRACEDBG("cSpecular : %d\n",cSpecular);
              }

              // 7) Ajoute la frame � la banque de frame du jeu [PAR DEFAUT : en m�moire]
//*** SPECIAL EDITOR ***
// -> Direct3D non g�gr�
			  is3DFrame = false;
//**********************
              SpriteFrame *New_Frame = SFL->AddFrame(&DS_VA->pDD,NULL,filePathBmp.c_str(),filePathSpi.c_str(),numIDFrame, true, is3DFrame);
              New_Frame->iStartSpriteIndex = iStartIndex;
              // 8) Ajoute les sprites inclues dans cette frame
			  AddFrameSprites(filePathSpi.c_str(), New_Frame, iStartIndex, cColor, cSpecular);

              NumAnim++;
           } break;

           //===--  #SimpleAnims --===
           case 1 :
           {  // variables utilis�es
              int c=0, i=1;	   		// compteurs
              char texte[128];   	// buffer
              AnimSprite* AS;       // Nouveal AnimSprite

              AS = new AnimSprite();	// Alloue une instance d'AnimSprite

              // 1) R�cup�re le nom de l'animation
              if (BufLine[0]=='[')
              {  while (BufLine[i]!=']') texte[c++]=BufLine[i++]; }
              else MessageBoxA(NULL,"Erreur ds le fichier : ",InaName.c_str(),1);
              texte[c]='\0';
              strcpy(AS->Nom,texte);
              AnsiString AnimName = texte;
              TRACEDBG("Nom de l'anim : %s\n", texte);

              // 2) V�rifie si ce nom existe bien dans le fichier des IDBmpFrame
              if (!IDAnimation->IsCle(AnimName))
              {	 AnsiString error = "Nom de l'anim inexistant : " + AnimName;
                 MessageBoxA(NULL,error.c_str(),InaName.c_str(),1);
              }
              // 3) R�cup�re le n�ID associ� � cette frame
              int numIDAnim = IDAnimation->NumID(AnimName);
              TRACEDBG("N� de l'anim : %d\n",numIDAnim);

              // 4) R�cup�re le nombre d'image par orientation
              if (!GetLabel("NbSprites", texte, f))
              {  AnsiString error = "Champs 'NbSprites' inexistant !";
                 MessageBoxA(NULL,error.c_str(),InaName.c_str(),1);
              }
              TRACEDBG("NbSprites : %s\n",texte);
              AS->NbSprites = atoi(texte); 			// l'affecte � l'AnimSprite;

              // 5) R�cup�re le type de l'animation
	          if (!GetLabel("Type", texte, f))
              {  AnsiString error = "Champs 'Type' inexistant !";
                 MessageBoxA(NULL,error.c_str(),InaName.c_str(),1);
              }
              TRACEDBG("Type : %s\n",texte);
              AS->Type = atoi(texte);				// l'affecte � l'AnimSprite;

              // 6) suivant le type d'animation, choppe le n� d'image de d�part ou la suite des n� d'images
              switch (AS->Type)
              {
                  case 3 :  // 1 seule orientation de cam�ra (et non 8)
                  { // R�cup�re le label derri�re "Index = "
                  	if (!GetLabel("Index", BufLine, f))
              		{  AnsiString error = "Champs 'Index' inexistant !";
                 	   MessageBoxA(NULL,error.c_str(),InaName.c_str(),1);
              		}
                    // Choppe le nom de la frame ds lequel se situe ce sprite
                    c = 0; i = 1;
                    if (BufLine[0]=='@')
                    {	while ((BufLine[i]!=',') && (BufLine[i]!=' ') && (BufLine[i]!='\n') && (BufLine[i]!='\t'))
                    	{ texte[c++]=BufLine[i++];
                    	}
                    } else MessageBoxA(NULL,"Syntaxe de frame incorrecte",InaName.c_str(),1);
                    AnsiString Frame = texte;
              		if (!IDBmpFrame->IsCle(Frame)) 	MessageBoxA(NULL,"Nom de frame inexistant ?",InaName.c_str(),1);
					int numIDFrame = IDBmpFrame->NumID(Frame); // R�cup�re le n�ID associ� � cette frame
                    TRACEDBG("NumIDFrame : %d\n", numIDFrame);

                    // Saute les virgules, espaces et tabs
                    while ((BufLine[i]==',') || (BufLine[i]==' ') || (BufLine[i]=='\t')) i++;

                    // A partir de l'ID de la frame, r�cup�re l'offset de d�part de l'anim
                    c=0;
                    while ((BufLine[i]!=',') && (BufLine[i]!=' ') && (BufLine[i]!='\n') && (BufLine[i]!='\t'))
                    { texte[c++]=BufLine[i++];
                    }
                    int offset = atoi(texte);

   	  				SpriteFrame *frame = SFL->GetFrame(numIDFrame);
                    if (frame==NULL)
                    {   AnsiString Erreur = numIDFrame;
                    	MessageBoxA(NULL,Erreur.c_str(),InaName.c_str(),1);
                    }
                    offset += frame->iStartSpriteIndex;
                    TRACEDBG("Offset : %d\n", offset);
                    AS->FirstSprite = offset;  // n�image de d�part;
                  } break;
              }

              // 7) Regarde si ce sont des sprites 2D ou 3D
              if (!GetLabel("Direct3D", texte, f))
              {  AnsiString error = "Champs 'Direct3D' inexistant !";
                 MessageBoxA(NULL,error.c_str(),InaName.c_str(),1);
              }
              TRACEDBG("Direct3D : %s\n",texte);
              bool sprite3D = false;
              if (strcmp(texte,"YES")==0) sprite3D = true;

              // 8) Si c'est un sprite 3D, on r�cup�re les composantes alpha et specular
              if (sprite3D)
              { unsigned int cColor    = 0xEEFFFFFF;		// Composantes RGBA du sprite
              	unsigned int cSpecular = 0x00000001;
              	// ColorAlpha
              	if (!GetLabel("ColorAlpha", texte, f))
                {  AnsiString error = "Champs 'ColorAlpha' inexistant !";
                   MessageBoxA(NULL,error.c_str(),InaName.c_str(),1);
                }
                TRACEDBG("ColorAlpha : %s\n",texte);
                if (strcmp(texte,"default")!=0)
                {	cColor = hexatoi(texte);
                }
                // ColorSpecular
              	if (!GetLabel("ColorSpecular", texte, f))
                {  AnsiString error = "Champs 'ColorSpecular' inexistant !";
                   MessageBoxA(NULL,error.c_str(),InaName.c_str(),1);
                }
                TRACEDBG("ColorSpecular : %s\n",texte);
                if (strcmp(texte,"default")!=0)
                {	cSpecular = hexatoi(texte);
                }
              }

              // Ajoute cette anim au dico
              AnimsMap.insert(make_pair(numIDAnim, AS));

              NumAnim++;
           }; break;

        }
      _FinDeLigne:
      ComptLine++;
    }

	fclose(f);		// On referme gentillement le fichier

//--DEBUGY--    *pATL = ATL;
//--DEBUGY--    *pASL = ASL;
}

//=== Ajoute une banque de sprites appartenant � une frame ===================//
void  SpriteTileGameLib::AddFrameSprites(char *SpiFileName, SpriteFrame *pFr, int startIndex, unsigned int cColor, unsigned int cSpecular)
{ 	FILE *f;
    FrInfo Spi;

    // V�rifie l'�tat des noms du fichier et du pointeur
    if ((SpiFileName==NULL) || (pFr==NULL))
            MessageError("SpritesLib::AddSprites","Noms de fichiers non valides",99);

    // Ouvre le .SPI et lit l'en-t�te
    AnsiString pathSpi = CheminApplication + SpiFileName;
    if ((f = fopen(pathSpi.c_str(),"rb"))==NULL) MessageError("SpritesLib::AddSprites","N'arrive pas � ouvrir le fichier",99);
    fread(&Spi,sizeof(Spi),1,f);

	// S'occupe de r�partir les sprites ds leur librairie
    SprInfo SI;
    int index = startIndex;
    for (int compt=0; compt < Spi.NbSprites; compt++)
    {  fread(&SI,sizeof(SprInfo),1,f);
       AddOneSprite(SI,Spi.CoulFond, Spi.CoulOmbr, Spi.cfRGB, Spi.coRGB, cColor, cSpecular, pFr, index);
       index++;
    }

    fclose(f);
}
//----------------------------------------------------------------------------//

//=== Cr�e et ajoute un sprite au dictionnaire ===============================//
void SpriteTileGameLib::AddOneSprite(SprInfo InfSprite,int CF,int CO,unsigned int RGBcf,unsigned int RGBco,
									  unsigned int cColor, unsigned int cSpecular, SpriteFrame *pFr, int index)
{
	SpriteTile *New_Node;

    // Cr�e un nouveau Sprite 3D ou 2D
    if (pFr->isTextureDirect3D)
    {   // Calcul des cordonn�es de texture d'ap�s la position du sprite sur sa frame
        //  -> comprises entre [0..1]
        float posX1, posY1, posX2, posY2;
        posX1 = ((float) InfSprite.CoordX) / pFr->PixWidth;
        posY1 = ((float) InfSprite.CoordY) / pFr->PixHeight;
        posX2 = posX1 + ((float) InfSprite.Width) / pFr->PixWidth;
        posY2 = posY1 + ((float) InfSprite.Height) / pFr->PixHeight;

    	New_Node = new Sprite3DTile();
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
    New_Node->GravX  = InfSprite.GravX;			// Centre de "gravit�"
    New_Node->GravY  = InfSprite.GravY;
    New_Node->CoulFond = CF;					// Index ds la palette 256c de la couleur de fond et d'ombre
    New_Node->CoulOmbr = CO;
    New_Node->cfRGB = RGBcf;					// Composantes des couleurs de fond et d'ombre
    New_Node->coRGB = RGBco;
    New_Node->NumFrame = pFr->NumFrame;			// N� et pointer sur sa frame
    New_Node->pFrame = pFr;
    New_Node->NumSprite = index;
    New_Node->SrcRect.left = InfSprite.CoordX;	// Rectangle source
    New_Node->SrcRect.top = InfSprite.CoordY;
    New_Node->SrcRect.right = InfSprite.CoordX+InfSprite.Width;
    New_Node->SrcRect.bottom = InfSprite.CoordY+InfSprite.Height;

    // Ajoute tout simplement ce sprite au dictionnaire
    SpriteMap.insert(make_pair(index, New_Node));
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
SpriteTileGameLib::~SpriteTileGameLib()
{	// D�salloue tous les sprites de la librairie
	for (TSpriteMap::iterator iter = SpriteMap.begin(); iter!=SpriteMap.end() ; iter++)
    {   SpriteTile*  sprite = (*iter).second;
		delete sprite;
    }
    // Puis vide le dictionnaire
    SpriteMap.clear();

    // D�salloue toutes les anims de la librairie et vide le dico
	for (TAnimsMap::iterator iter = AnimsMap.begin(); iter!=AnimsMap.end() ; iter++)
    {   AnimSprite*  anim = (*iter).second;
		delete anim;
    }
    // Puis vide le dictionnaire
    AnimsMap.clear();

}
//----------------------------------------------------------------------------//

//=== Joue une anim en (x,y) =================================================//
void SpriteTileGameLib::PlayAnim(int animID, int numSprite, int x, int y)
{	AnimSprite* anim = AnimsMap[animID];
 	if (anim==NULL) TRACEDBG("Animation n� %d non trouv�e dans SpriteTileGameLib::PlayAnim\n", animID);
	int spriteID = anim->FirstSprite + numSprite;
    TRACEDBG("SpriteID : %d\n", spriteID);
    SpriteTile* sprite = SpriteMap[spriteID];
    if (sprite->TypeSprite == SPRITE_TILE_3D)
    {	((Sprite3DTile*) sprite)->SpriteRender(x, y);
    	TRACEDBG("Sprite3D\n");
    } else
    {

    }
}
//----------------------------------------------------------------------------//

SpriteTile*	SpriteTileGameLib::GetSpriteTile(int numID)
{	return SpriteMap[numID];
}

AnimSprite*	SpriteTileGameLib::GetAnimSprite(int numID)
{	return AnimsMap[numID];
}



//--- Convertit une cha�ne de caract�re exprim�e en hexa "0xF0E123" en entier //
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
        number += num * pow(16, mul2);
        mul2++;
    }
    return 0;		// on n'a pas rencontr� le caract�re 'x' ou 'X' exprimant que l'on a affaire a un nb en h�xa
}
