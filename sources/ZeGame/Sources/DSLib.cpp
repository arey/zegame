////////////////////////////////////////////////////////////////////////////////
//  Unité  "DSLib.cpp" : Librarie regroupant diverses fonctions de gestion	  //
//							  	 sonore et musicale sous DirectSound.		  //				
//                                                                            //  
//  Copyright (C) 1998 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com                      //
//					                                                          //
//  Version      : beta 0.01                                                  //
//  Creation     : 10/04/1999                                                 //
//  Last upgrade : 10/04/1999                                                 //
////////////////////////////////////////////////////////////////////////////////


// --------------------------------------------------------------------- include
#include "ZeNain.h"
#include "winuser.h"		// Lib de gestion de messages Win32
#include "mmsystem.h" 		// Windows Multimedia API's
#include "wave.h"			// Librairie microsoft maitrisant le format .wav
#include "INILib.h"			// Lib de gestion des fichiers .ini
#include "FileVirtual.h"	// Fichiers virtuels
#include "MemLib.h"			// Lib. regroupant divers canaux de communication entre modules
#include "SoundUtil.h"		// Librairie maison de lecture des .wav
#include "DSWave.h"			// Classe sachant lire des fichiers .wav
#include "DSMp3.h"			// Classe sachant lire des fichiers .mp3
#include "DSLib.h"			// Header


// ------------------------------------------------------------------- variables
TDirectSound	*DSGame = NULL;  // Seul objet DirectSound du jeu
TDSMuzikLib	*DSMuzikLib = NULL;  // Seul instance du jeu de la classe TDSMuzikLib

// ---------------------------------------------------------------------- define
#define DSBCAPS_CTRLDEFAULT         0x000000E0		// disparu ds DirectX 8


////////////////////////////////////////////////////////////////////////////////
//	Classe TDIRECTSOUND														  //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur par défaut ================================================//
TDirectSound::TDirectSound()
{	pDS = NULL;
	pPrimaryBuffer = NULL;
	bPause = true;
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TDirectSound::~TDirectSound()
{	bPause = true;
	if (pPrimaryBuffer != NULL) pPrimaryBuffer->Release();
	if (pDS != NULL) pDS->Release(); 
}
//----------------------------------------------------------------------------//

//=== Initialise DirectSound =================================================//
bool TDirectSound::InitDSound(HWND hWnd)
{	HRESULT hr;
	DSBUFFERDESC desc;

	hr = DirectSoundCreate(NULL, &pDS, NULL);							// Créée l'objet COM DirectSound
	if FAILED(hr) return false;
	
	// [DEBUG] hr = pDS->SetCooperativeLevel(hWnd, DSSCL_EXCLUSIVE);	// Met en place le mode de coopération avec windows : les applications en arrière-plan sont muettes
    hr = pDS->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);
	if FAILED(hr) return false;

	
	// Crée le buffer sonore principal
	ZeroMemory(&desc, sizeof(DSBUFFERDESC));
	desc.dwSize = sizeof(DSBUFFERDESC);
	// [DEBUG] desc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;
    desc.dwFlags = DSBCAPS_PRIMARYBUFFER;
	hr = pDS->CreateSoundBuffer(&desc, &pPrimaryBuffer, NULL);
	if FAILED(hr)	return false;

	
	// Défini par défaut le format du buffer principal :
	//	44100Hz, 16bits, stéréo
	WAVEFORMATEX wfx;
	memset(&wfx, 0, sizeof(WAVEFORMATEX)); 
	wfx.wFormatTag = WAVE_FORMAT_PCM;		// Etiquette de format
	wfx.nChannels = 2;						// Nb de canaux
	wfx.nSamplesPerSec = 44100;				// Taux d'échantillonage
	wfx.wBitsPerSample = 16;				// Nb de bits / échantillon
	wfx.nBlockAlign = (unsigned short) (wfx.wBitsPerSample / 8 * wfx.nChannels);
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign; 
	hr = pPrimaryBuffer->SetFormat(&wfx); 
	if FAILED(hr) return false;

	bPause = false;
	return true;		// Initialisation completed
}

//=== Fait une ch'tite pause =================================================//
void TDirectSound::SetPause(bool p)
{	bPause = p;
}
//----------------------------------------------------------------------------//

//=== Renvoie vrai si l'on est en pause ======================================//
bool TDirectSound::IsPaused()
{	return bPause;
}
//----------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////////////////
//	Classe TDSSoundFile 													  //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur par défaut ================================================//
TDSSoundFile::TDSSoundFile()
{	hSoundFV = NULL;
    pSoundNotify = NULL;
    pSoundBuffer = NULL;
	soundName[0] = '\0';
	soundPathName[0] = '\0';
	bPlaying = false;
	bPaused = false;
	iEndSoundSynchro = -1;	// Pas de synchronisation a la fin du son
}
//----------------------------------------------------------------------------//

//=== Arrete de jouer ce son =================================================//
void TDSSoundFile::stop()
{	// disable le gestionnaire d'évènements 
	if (pSoundNotify) pSoundNotify->Release();	
	// ferme le sound buffer
	if (pSoundBuffer)
	{	pSoundBuffer->Stop();	// mieux vaut 2 fois qu'une 
		pSoundBuffer->Release();
	}
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TDSSoundFile::~TDSSoundFile()
{	//stop();	// Arret le son
}
//----------------------------------------------------------------------------//


//=== Ventile les evenements windows concernant le son =======================//
void TDSSoundFile::DSWinMain()
{	
	// Supension de l'éxécution du programme jusqu'à ce qu'il se produise :
	//  - soit l'un des évènements  de hEvents est déclenché,
	//  - soit un message est placé dans la file
	DWORD dwEvt = MsgWaitForMultipleObjects(
						NUMEVENTS,				// Nb d'évènements
						hEvent,					// Emplacement des handles
						FALSE,					// Ne les attends pas tous
						0,						// Durée d'attente (INFINITE plante)
						QS_ALLINPUT);			// Chaque message est un évènement

	//   => renvoie ds 'dwEvt' un indice du tableau des évènements en sachant
	//   que WAIT_OBJET_0 désigne le 1er élément
	dwEvt -= WAIT_OBJECT_0;

	// Si l'évènement a été défini par le tampon de capture sonore, on doit le traiter
	if ( dwEvt < NUMEVENTS)
	{	StreamToBuffer(dwEvt);		// Copie des données wav vers le buffer audio
	} 
}
//----------------------------------------------------------------------------//


//=== Vrai si l'on est en-train de jouer le .wav =============================//
bool TDSSoundFile::isPlaying()
{	return bPlaying;
}
//----------------------------------------------------------------------------//

//=== Revient au début de la musique =========================================//
void TDSSoundFile::Beginning()
{	// [A REVOIR] Pas au point dut tout -> Milia2000
	// => on rouvre le fichier et on perd donc du temps
	Open(soundPathName, soundName);
}
//----------------------------------------------------------------------------//

//=== Fait une p'tite pause ==================================================//
void TDSSoundFile::SetPause(bool pause)
{	if (pause)
	{	pSoundBuffer->Stop();
	} else
	{	pSoundBuffer->Play(0, 0, DSBPLAY_LOOPING);
	}
	bPaused = pause;
}
//----------------------------------------------------------------------------//

//=== Joue le .wav en boucle ou non ==========================================//
void TDSSoundFile::Play()
{	bPlaying = true;
	bPaused = false;
	dwStopNextTime = 0xFFFF;
	StreamToBuffer(NUMEVENTS-1);
	pSoundBuffer->SetCurrentPosition(0);
	pSoundBuffer->Play(0, 0, DSBPLAY_LOOPING);
}
//----------------------------------------------------------------------------//

//=== Ouvre un fichier sonore et se prépare à le lire ========================//
bool TDSSoundFile::Open(char* soundPathName, char* name)
{	if (name!=NULL)
	{	strcpy(soundName, name);
	} else
	{	soundName[0]='\0';
	}
	if (soundPathName!=NULL)
	{	strcpy(this->soundPathName, soundPathName);
	} else
	{	this->soundPathName[0]='\0';
	}

	    
    if (pSoundNotify != NULL)
    {   pSoundNotify->Release();
        pSoundNotify = NULL;
    } 
    if (pSoundBuffer != NULL)
    {   pSoundBuffer->Release();
        pSoundBuffer = NULL;
    }
    return true;
}
//----------------------------------------------------------------------------//


//=== Cree un DirectSound buffer de @length octets ===========================//
// Et prepare les call back functions
HRESULT TDSSoundFile::createSoundBuffer(int length)
{	// On crée un buffer secondaire au même format que le fichier .wav que l'on veut lire
	// -> c'est un streaming buffer qui évolue en permance
	memset(&desc, 0, sizeof(DSBUFFERDESC)); 	
	desc.dwSize = sizeof(DSBUFFERDESC);     
	desc.dwFlags = 
            DSBCAPS_GETCURRENTPOSITION2		// Toujours une bonne idée
            | DSBCAPS_GLOBALFOCUS			// Permet le background playing
            | DSBCAPS_CTRLPOSITIONNOTIFY	// Needed for notification 
			| DSBCAPS_CTRLDEFAULT;			// Default control options : DSBCAPS_CTRLPAN, DSBCAPS_CTRLVOLUME, and DSBCAPS_CTRLFREQUENCY flags
	// La taille  du buffer est arbitraire mais doit être au moins de 2 secondes pour que l'on puisse écrire des données dedans pdt qu'il est lu
	desc.dwBufferBytes = length;  
	desc.lpwfxFormat = pWaveFormat;
	HRESULT hr = DSGame->pDS->CreateSoundBuffer(&desc, &pSoundBuffer, NULL);
	if FAILED(hr) return hr;

	// Etant donné que l'on a affaire à un streaming-buffer, on va devoir être averti lorsque la 
	// position courante du player audio aura atteint certaines positions : ce sera temps de
	// remplir le flux audio avec les prochaines données audio. 
	// Ici, on va être averti au début et au milieu du buffer; sinon, on sera averti à intervalle 
	// régulier du buffer.
	
	
	// Création des NUMEVENTS évènements ds 'hEvent'
	for (int i = 0; i < NUMEVENTS; i++)
	{	hEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (NULL == hEvent[i]) return -1;
	}

	// Associe une certaine position du buffer à un handler
	int offset = 0;
	for (i = 0; i < NUMEVENTS; i++)
	{	EventPos[i].dwOffset = offset;
		EventPos[i].hEventNotify = hEvent[i];
		offset += desc.dwBufferBytes / NUMEVENTS;
    }

	// Interface le buffer secondaire avec le gestionnaire d'évènement
	hr = IDirectSoundBuffer_QueryInterface(pSoundBuffer, IID_IDirectSoundNotify, (VOID **)&pSoundNotify);
    if FAILED(hr) return hr; 
 
    // Lui confie les évènements souhaités	
	hr = IDirectSoundNotify_SetNotificationPositions(pSoundNotify, NUMEVENTS, EventPos);
    if FAILED(hr)
	{	IDirectSoundNotify_Release(pSoundNotify);
        return hr;
    }
	return 0;
}
//----------------------------------------------------------------------------//

//=== Remplit le buffer audio avec un morceau de musique =====================//
bool TDSSoundFile::StreamToBuffer(DWORD dwPos)
{	DWORD	dwStartOfs;    
	VOID	*lpvPtr1, *lpvPtr2;
    DWORD	dwBytes1, dwBytes2;    
	UINT    cbBytesRead;
	LONG lNumToWrite;

	// dwStopNextTime est un flag indiquant si la fin du fichier a été atteint
	//   - si <> 0  : il n'y a plus de données à transférer
	//   - si == index : la play-position courante est revenue où elle était qd la fin du fichier a été joué

	// Si tout le .wav a été joué, on stop le DirectSoundBuffer
	if (dwStopNextTime == dwPos)
	{	pSoundBuffer->Stop();
		dwStopNextTime = 0xFFFF;
		bPlaying = false;

		// Si une synchronisation a ete demandee, on previent que c'est la fin du fichier 
		if (iEndSoundSynchro != -1) 
		{	MemMutexLib->FreeMutex(iEndSoundSynchro);
			iEndSoundSynchro = -1;
		}
		return true;
	}

	
	// On détermine l'offset à partir duquel on va copier les datas
	if (dwPos == 0)        
	{	dwStartOfs = EventPos[NUMEVENTS - 1].dwOffset;   
	} else
	{   dwStartOfs = EventPos[dwPos-1].dwOffset;
	}

	
	// Calcul de la taille du buffer à copier
	lNumToWrite = (LONG) EventPos[dwPos].dwOffset - dwStartOfs;
    if (lNumToWrite < 0) lNumToWrite += desc.dwBufferBytes;


	// On va locker la portion de buffer où les datas seront écrites
	IDirectSoundBuffer_Lock(pSoundBuffer,
                     dwStartOfs,       // Offset à partir duquel commence le lock
                     lNumToWrite,      // Nb d'octets à locker
                     &lpvPtr1,         // Address of lock start
                     &dwBytes1,        // Count of bytes locked
                     &lpvPtr2,         // Address of wrap around
                     &dwBytes2,        // Count of wrap around bytes
                     0);               // Flags
	
	// Il n'y a plus rien n'a chargé ds le SoundBuffer mais on le laisse joué jusqu'à la fin du .wav
	if (dwStopNextTime != 0xFFFF)
	{	// On peut quand meme rajouter un peu de blanc, car sinon, si le prochain
		// evenement est en retard d'une micro seconde, on peut entendre un petit "pet"
		FillMemory((PBYTE)lpvPtr1, 
				lNumToWrite, 
                (desc.lpwfxFormat->wBitsPerSample==8) ? 128 : 0);
	} else
	{	
		// Transfert du fichier dans la memoire audio
		readSoundFile(dwBytes1,            // Nb d'octets à lire
					 (BYTE *) lpvPtr1,     // Destination   
					 &cbBytesRead);        // Actual bytes read

		// Now, on détermine si la fin du fichier sonore a été atteint 
		// -> si oui, on le ferme et on écrit du silence ds le reste du segment audio
		if (cbBytesRead < dwBytes1)        
		{
			closeSoundFile();
			FillMemory((PBYTE)lpvPtr1 + cbBytesRead, 
					dwBytes1 - cbBytesRead, 
					(desc.lpwfxFormat->wBitsPerSample==8) ? 128 : 0);
			dwStopNextTime = dwPos;
		}
	}

	// Pour finir, on dévérouille le buffer et l'on revient à la boucle de messages
    IDirectSoundBuffer_Unlock(pSoundBuffer, lpvPtr1, dwBytes1, lpvPtr2, dwBytes2);

	return true;
}
//----------------------------------------------------------------------------//

//=== Regle le volume du buffer sonore =======================================//
// Le volume est exprime en decibels/100 et s'etend de 0 a -10000 (directsound ne
// gere pas l'amplication)
// *      0	: volume sonore d'origine
// * -10000	: silence radio
// *   -100 : 50% du volume de depart
bool TDSSoundFile::setVolume(int volume)				
{	// Corrige l'intervalle
	if (volume < -10000) volume = -10000;
	if (volume > 0) volume = 0;
	// Set the new volume rate
	if (DS_OK != pSoundBuffer->SetVolume(volume))
	{	return false;
	}
	return true;
}
//----------------------------------------------------------------------------//

//=== Augmente ou diminue le volume courant de dtVol/100 de dB ===============//
bool TDSSoundFile::increaseVolume(int dtVol)
{	return setVolume(getVolume() + dtVol);
}
//----------------------------------------------------------------------------//


//=== Indique le volume courant du buffer sonore ============================//
// volume sonore exprime en 100ieme de dB (0 a -10000)
// renvoie 0xFFFF si le volume n'a pas pu etre lu
int TDSSoundFile::getVolume()
{	long volume = 0xFFFF;
	if (DS_OK != pSoundBuffer->GetVolume(&volume))	return 0xFFFF;
	return volume;
}
//----------------------------------------------------------------------------//

//=== Volume relatif entre l'enceinte gauche et droite (-100dB a +100dB) =====//
bool TDSSoundFile::setPan(int pan)
{	if ((pan < -10000) || (pan > 10000)) return false;
	if (DS_OK != pSoundBuffer->SetPan(pan))
	{	return false;
	}
	return true;
}

//=== Renvoie le volume relatif entre les 2 enceintes ========================//
int TDSSoundFile::getPan()
{	long pan;
	if (DS_OK != pSoundBuffer->GetPan(&pan)) return 0;
	return pan;
}
//----------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////////////////
//	Classe TDSMUZIKLIB														  //
////////////////////////////////////////////////////////////////////////////////

extern AnsiString CheminApplication;
extern const char cPathSounds[];
extern const char cPathMuzik[];
extern const char cIniZeGame[];

//=== Constructeur par défaut ================================================//
TDSMuzikLib::TDSMuzikLib(bool musik)
{	int		nbMuzik;
	char	Zikpath[512]; 
	char	ZikpathName[512];
	char	ZikName[128];
	char	ZikNumber[8];
	TDSSoundFile* pSoundFile = NULL;

	iCurrentMuzik = -1;
	bMusikGame = musik;
	
	if (!bMusikGame) return;

	// Récupère les fichiers wav d'ambiance musicale
	TINILib	ini((char*) cIniZeGame);
	nbMuzik = ini.GetIntProfile("Audio", "NbMusic", 0);
	if (nbMuzik>0)
	{	strcpy(Zikpath, LPCSTR(CheminApplication));
		for (int i=1 ; i <= nbMuzik ; i++)
		{	// Récupère le nom du .wav suivant
			strcpy(ZikName, "MusicFile");
			itoa(i, ZikNumber, 10);
			strcat(ZikName, ZikNumber);		
			ini.GetStringProfile("Audio", ZikName, ZikName);
			#ifndef NEW_FILE_MANAGER
			strcpy(ZikpathName, Zikpath);
			strcat(ZikpathName, cPathMuzik);
			#else
			strcpy(ZikpathName, cPathMuzik);
			#endif // NEW_FILE_MANAGER
			strcat(ZikpathName, ZikName);
			// Prépare le fichier à être joué et l'ajoute à la liste des musiques
			pSoundFile = AllocateSoundFile(ZikName);
			if (pSoundFile != NULL)
			{	pSoundFile->Open(ZikpathName, ZikName);
				vSoundMuzik.push_back(pSoundFile);
			}
		}
	}
}
//----------------------------------------------------------------------------//


//=== Destructeur ============================================================//
TDSMuzikLib::~TDSMuzikLib()
{	for (int i=0; i < vSoundMuzik.size() ; i++)
	{	delete vSoundMuzik[i];
	}
	vSoundMuzik.clear();
}
//----------------------------------------------------------------------------//

//=== Renvoi une instance d'un fichier sonore suivant son extension ==========//
// Gere : .wav, 
TDSSoundFile* TDSMuzikLib::AllocateSoundFile(char* filename)
{	char extension[10];
	int i =0, j=0;
	bool parseext = false;
	// Recupere l'extension du fichier sonore
	while ((filename[i]!='\0') && (j<10))
	{	if (parseext)
		{	extension[j++] = filename[i];
		}
		else if (filename[i]=='.') parseext = true;
		i++;
	}
	if ((j<1) || (j>=10)) return NULL;
	extension[j] = '\0';

	if (strcmp(extension, "wav")==0) return new TDSWave();
	else if (strcmp(extension, "WAV")==0) return new TDSWave();
	else if (strcmp(extension, "mp3")==0) return new TDSMp3();
	else if (strcmp(extension, "MP3")==0) return new TDSMp3();
	
	return NULL;
}

//=== S'occupe de mettre à jour le SoundBuffer ===============================//
void TDSMuzikLib::MuzikManage()
{	// on sort si on est pas en train de jouer les musiques d'ambiance
	if (!bMusikGame) return;

	if (iCurrentMuzik != -1)
	{	// Fichier wav en cours
		if (vSoundMuzik[iCurrentMuzik]->isPlaying())
		{	vSoundMuzik[iCurrentMuzik]->DSWinMain();
		} else
		// Si il est fini, on joue le suivant
		{	iCurrentMuzik++;
			if (iCurrentMuzik >= vSoundMuzik.size()) iCurrentMuzik = 0;
			vSoundMuzik[iCurrentMuzik]->Beginning();
			vSoundMuzik[iCurrentMuzik]->Play();
		}
	} else if (vSoundMuzik.size()!=0)
	{	// On joue la 1ère Musique
		iCurrentMuzik = 0;
		vSoundMuzik[iCurrentMuzik]->Play();	
	}
}
//----------------------------------------------------------------------------//

//=== Stoppe toutes les musiques =============================================//
void TDSMuzikLib::SetPause(bool pause)
{	if (DSGame == NULL) return;
	// Vérifie si l'on n'est pas déjà ds l'état souhaité
	if (DSGame->IsPaused() == pause) return;

	if (iCurrentMuzik != -1)
	{	DSGame->SetPause(pause);
		vSoundMuzik[iCurrentMuzik]->SetPause(pause);		
	}
}
//----------------------------------------------------------------------------//


/////////////////////// Fin de la classe TDSMuzikLib ///////////////////////////



///////////////////////////////////////////////////////////////////////////////
//							Fonctions EXTERNES								 //
///////////////////////////////////////////////////////////////////////////////


//=== Initialize tout ce qui se rapporte au son =============================//
void InitGame_DirectSound(HWND hWnd)
{	char sSound[16];
	TINILib	ini((char*) cIniZeGame);
	ini.GetStringProfile("Audio", "Sound", sSound);
	bool bMusik = (strcmp(sSound, "Yes") == 0) ;
	if (bMusik)
	{	DSGame = new TDirectSound();
		DSGame->InitDSound(hWnd);
	}
	DSMuzikLib = new TDSMuzikLib(bMusik);	
}
//---------------------------------------------------------------------------//

//=== Initialize tout ce qui se rapporte au son =============================//
void CloseGame_DirectSound()
{	delete DSMuzikLib; DSMuzikLib = NULL;
	delete DSGame; DSGame = NULL;
}
//---------------------------------------------------------------------------//

