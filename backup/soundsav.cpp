// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  sound.cpp - Music and sound module                   *
// *************************************************************************

#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <msacm.h>
#include <stdio.h>
#include <dsound.h>
#include <math.h>

#include "revenant.h"
#include "mainwnd.h"
#include "resource.h"
#include "parse.h"
#include "file.h"
#include "sound.h"
#include "wavedata.h"

// *****************************
// * CD Sound System Functions *
// *****************************

#define DISTFACTOR (1.0f / (float)UNITSPERMETER)

static bool isopen;
static long cdstartticks;
static long cdtracklen;
static long cdtrackpos;

void CDOpen()
{
    char err[120];
    DWORD res = mciSendString("open cdaudio shareable", NULL, 0, 0L);
    mciGetErrorString(res, err, 120);

    isopen = TRUE;
}

void CDClose()
{
    char err[120];
    DWORD res = mciSendString("close cdaudio", NULL, 0, 0L);
    mciGetErrorString(res, err, 120);

    isopen = FALSE;
}

void CDPlayTrack(int track)
{
    char err[120];
    if (!SoundSystemOn)
        return;

    if (!isopen)
        CDOpen();

    char ret[40];
    char buf[40];

    DWORD res = mciSendString("stop cdaudio", NULL, 0, 0L);
    mciGetErrorString(res, err, 120);

    res = mciSendString("set cdaudio time format ms", NULL, 0, 0L);
    mciGetErrorString(res, err, 120);

    wsprintf(buf, "status cdaudio position track %d", track);
    res = mciSendString(buf, ret, 39, 0L);
    mciGetErrorString(res, err, 120);

    cdtrackpos = atol(ret);

    wsprintf(buf, "status cdaudio length track %d", track);
    res = mciSendString(buf, ret, 39, 0L);
    mciGetErrorString(res, err, 120);

    cdtracklen = atol(ret);

    wsprintf(buf, "play cdaudio from %ld to %ld", cdtrackpos, cdtrackpos + cdtracklen - 1);

    cdstartticks = GetTickCount();

    res = mciSendString(buf, NULL, 0, 0L);
    mciGetErrorString(res, err, 120);
}

DWORD CDTrackLength(int track)
{
    if (!SoundSystemOn)
        return 0;

    if (!isopen)
        CDOpen();

    char ret[40];
    char buf[40];

    mciSendString("set cdaudio time format ms", NULL, 0, 0L);

    wsprintf(buf, "status cdaudio length track %d", track);
    mciSendString(buf, ret, 39, 0L);

    return atol(ret);
}



void CDPlayRandomTrack()
{
    if (!SoundSystemOn)
        return;

    if (!isopen)
        CDOpen();

    char ret[40];
    int first = 1;
    int last;

    mciSendString("status cdaudio number of tracks", ret, 39, 0L);
    last = atoi(ret);

    mciSendString("status cdaudio type track 1", ret, 39, 0L);
    if (!stricmp(ret, "audio"))
        first = 1;
    else
        first = 2;

  // Check if this is our CD
/*  char buf[40];
    mciSendString("set cdaudio time format ms", NULL, 0, 0L);
    wsprintf(buf, "status cdaudio length track %d", 2);
    mciSendString(buf, ret, 39, 0L);
    long tracklen = atol(ret);
    if (tracklen > 1000L && tracklen < 1000L)
        first = 2;
*/

    static int played[20];
    int numplayed = min(20, last - first);
    int play;

    BOOL wasplayed;
    do
    {
        wasplayed = FALSE;
        play = random(first, last);
        for (int c = 0; c < numplayed; c++)
        {
            if (play == played[c])
            {
                wasplayed = TRUE;
                break;
            }
        }
    } while (wasplayed);

    CDPlayTrack(play);

    for (int c = 0; c < numplayed - 1; c++)
        played[c] = played[c + 1];
    played[numplayed - 1] = play;
}

void CDStop()
{
    if (!isopen)
        CDOpen();

    if (!SoundSystemOn)
        return;

    mciSendString("stop cdaudio", NULL, 0, 0L);
}

BOOL CDPlaying()
{
    if (!isopen)
        CDOpen();

    if (cdtracklen == 0)
        return FALSE;

    if (GetTickCount() - cdstartticks < (DWORD)cdtracklen)
        return TRUE;

    return FALSE;
}

void CDSetVolume(WORD volume)
{
    if (!isopen)
        CDOpen();

    int devs = auxGetNumDevs();

    AUXCAPS caps;

    for (int c = 0; c < devs; c++)
    {
        auxGetDevCaps(c, &caps, sizeof(AUXCAPS));
        if (caps.wTechnology == AUXCAPS_CDAUDIO)
        {
            auxSetVolume(c, MAKELONG(volume,volume));
            return;
        }
    }
}

// *****************
// * Sound Effects *
// *****************

struct l
{
    short s[64];
};

// Loads a wave data object and returns a pointer to it
BOOL LoadWave(char *filename, LPWAVEFORMATEX &format, DWORD &size, LPBYTE &data)
{

    HMMIO hmmio = mmioOpen(filename, NULL, MMIO_READ | MMIO_ALLOCBUF);
    if (!hmmio)
        return FALSE;

    MMCKINFO mmckchunkinfo;
    mmckchunkinfo.fccType = mmioFOURCC('W', 'A', 'V', 'E');
    if (mmioDescend(hmmio, (LPMMCKINFO)&mmckchunkinfo, NULL, MMIO_FINDRIFF))
    {
        mmioClose(hmmio, 0);
        return FALSE;
    }

    MMCKINFO subchunk;
    subchunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
    if (mmioDescend(hmmio, &subchunk, &mmckchunkinfo, MMIO_FINDCHUNK))
    {
        mmioClose(hmmio, 0);
        return FALSE;
    }

  //read the fmt chunk
    DWORD dwfmtsize = subchunk.cksize;
    format = (LPWAVEFORMATEX)new BYTE[max(dwfmtsize, sizeof(WAVEFORMATEX))];
    memset(format, 0, dwfmtsize);
    if (mmioRead(hmmio, (HPSTR)format, dwfmtsize) != (long)dwfmtsize)
    {
        mmioClose(hmmio, 0);
        return FALSE;
    }

  // search data chunk
    subchunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
    if (mmioDescend(hmmio, &subchunk, &mmckchunkinfo,
                MMIO_FINDCHUNK))
    {
        delete format;
        mmioClose(hmmio, 0);
        return FALSE;
    }

    size = subchunk.cksize;
    if (size == 0)
    {
        delete format;
        mmioClose(hmmio, 0);
        return FALSE;
    }

    data = new BYTE[size];

    if (mmioRead(hmmio, (char *)data, size) != (long)size)
    {
        delete data;
        delete format;
        mmioClose(hmmio, 0);
        return FALSE;
    }

    mmioClose(hmmio, 0);

    return TRUE;
}

TSound::TSound()
{
    SoundBuffer = NULL;
    looping = FALSE;
    next = NULL;
}

TSound::~TSound()
{
    if (SoundBuffer)
    {
        SoundBuffer->Release();
        SoundBuffer = NULL;
    }
    if (next)
        delete next;
}

BOOL TSound::IsPlaying()
{
    return (GetStatus() & DSBSTATUS_PLAYING);
}

BOOL TSound::IsLooping()
{
    return (GetStatus() & DSBSTATUS_LOOPING);
}

// Load from wave file
PTSound TSound::Load(char *name)
{
    char filename[MAX_PATH];

    makepath(ResourcePath, filename, MAX_PATH - 1);
    strcat(filename, Language);
    strcat(filename, "\\");
    strcat(filename, name);
    strcat(filename, ".wav");

    LPWAVEFORMATEX format;
    DWORD size;
    LPBYTE data;

    if (!LoadWave(filename, format, size, data))
        return NULL;

    PTSound sound = Load(format, size, data, FALSE);

    delete format;
    delete data;

    return sound;
}

// Load from resource file
PTSound TSound::Load(int resid)
{
    char filename[MAX_PATH];
    strcpy(filename, Language);
    strcat(filename, "\\wave");

    PTWaveData wave = (PTWaveData)LoadResource(filename, resid);
    if (!wave)
        return NULL;

    BOOL looping = (wave->loopend - wave->loopstart) > 0;

    PTSound sound = Load(&(wave->format), wave->size, wave->data, looping);

    delete wave;

    return sound;
}

// Load from a resource buffer
PTSound TSound::Load(WAVEFORMATEX *format, DWORD size, BYTE *data, BOOL looping)
{
    MMRESULT mmres;
    HACMSTREAM acmstream;
    BOOL acmdecomp;

    if (!format || !data)
        return NULL;

    if (!SoundPlayer.Functioning())
        return NULL;

    PTSound sound = new TSound;

  // Copy wave format
    memcpy(&sound->format, format, sizeof(WAVEFORMATEX));
    if (sound->format.wFormatTag != WAVE_FORMAT_PCM)
    {
        sound->format.wFormatTag = WAVE_FORMAT_PCM;
//      sound->format.nChannels = 1;
//      sound->format.wBitsPerSample = 16;
        sound->format.nBlockAlign = sound->format.wBitsPerSample * sound->format.nChannels / 8;
        sound->format.nAvgBytesPerSec = sound->format.nSamplesPerSec * sound->format.nBlockAlign;
        sound->format.cbSize = 0;

        mmres = acmStreamOpen(&acmstream, NULL, format, &(sound->format),
            NULL, NULL, NULL, ACM_STREAMOPENF_NONREALTIME);
        if (mmres != 0)
        {
            delete sound;
            return NULL;
        }
        
        acmStreamSize(acmstream, size, (DWORD *)&(sound->size), ACM_STREAMSIZEF_SOURCE);

        acmdecomp = TRUE;
    }
    else
    {
        sound->size = size;
        acmdecomp = FALSE;
    }

    DSBUFFERDESC desc;
    memset(&desc, 0, sizeof(DSBUFFERDESC));
    desc.dwSize = sizeof(DSBUFFERDESC);
//  desc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_CTRLVOLUME | DSBCAPS_STATIC;
    desc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_STATIC;
    desc.dwBufferBytes = sound->size;
    desc.lpwfxFormat = &sound->format;

    HRESULT res = SoundPlayer.DirectSound->CreateSoundBuffer(&desc, &(sound->SoundBuffer), NULL);
    if (res != DS_OK)
    {
        delete sound;
        return NULL;
    }

  // Get 3D buffer  
//  res = sound->SoundBuffer->QueryInterface(IID_IDirectSound3DBuffer, (LPVOID *)&(sound->SoundBuffer3D)); 
//  if (res != DS_OK)
//  {
//      delete sound;
//      return NULL;
//  }

    LPVOID buffer;
    res = sound->SoundBuffer->Lock(0, sound->size, &buffer, &sound->size, NULL, NULL, 0);
    if (res != DS_OK)
    {
        sound->SoundBuffer->Release();
        delete sound;
        return NULL;
    }

    if (acmdecomp)
    {
        ACMSTREAMHEADER acmheader;
    
        memset(&acmheader, 0, sizeof(ACMSTREAMHEADER));
        acmheader.cbStruct = sizeof(ACMSTREAMHEADER);
        acmheader.pbSrc = data;
        acmheader.cbSrcLength = size;
        acmheader.pbDst = (LPBYTE)buffer;
        acmheader.cbDstLength = sound->size;

        mmres = acmStreamPrepareHeader(acmstream, &acmheader, 0);
        if (mmres == 0)
            mmres = acmStreamConvert(acmstream, &acmheader, ACM_STREAMCONVERTF_BLOCKALIGN);

        if (mmres != 0)
        {
            sound->SoundBuffer->Unlock(&buffer, sound->size, NULL, 0);
            sound->SoundBuffer->Release();
            delete sound;
            return NULL;
        }
    
        acmStreamClose(acmstream, 0);
    }
    else
        memcpy(buffer, data, size);

  // Kill clicks 16 bit mono signed
    if (sound->format.nChannels == 1 && 
        sound->format.wBitsPerSample == 16 && 
        sound->size > 64)
    {
        short *s = (short *)buffer;
        short *e = (short *)((BYTE *)buffer + sound->size - 2);
        for (int c = 0; c < 32; c++, s++, e--)
        {
            if (*s < 200 || *s > 200)
                *s = 0;
            if (*e < 200 || *e > 200)
                *e = 0;
        }
    }

  // Kill clicks 8 bit mono unsigned
    if (sound->format.nChannels == 1 && 
        sound->format.wBitsPerSample == 8 && 
        sound->size > 32)
    {
        BYTE *s = (BYTE *)buffer;
        BYTE *e = (BYTE *)((BYTE *)buffer + sound->size - 1);
        for (int c = 0; c < 32; c++, s++, e--)
        {
            if (*s < 124 || *s > 132)
                *s = 128;
            if (*e < 124 || *e > 132)
                *e = 128;
        }
    }

    sound->SoundBuffer->Unlock(&buffer, sound->size, NULL, 0);

  // Set 3D info
//  sound->SoundBuffer3D->SetMode(DS3DMODE_DISABLE, DS3D_IMMEDIATE);
//  sound->SoundBuffer3D->SetConeAngles(DS3D_MAXCONEANGLE, DS3D_MAXCONEANGLE, DS3D_IMMEDIATE);
//  sound->SoundBuffer3D->SetConeOrientation(0.0f, 0.0f, 0.0f, DS3D_IMMEDIATE);
//  sound->SoundBuffer3D->SetConeOutsideVolume(DSBVOLUME_MAX, DS3D_IMMEDIATE);
//  sound->SoundBuffer3D->SetMinDistance(256.0f, DS3D_IMMEDIATE);
//  sound->SoundBuffer3D->SetMaxDistance(256.0f + 768.0f, DS3D_IMMEDIATE);

    sound->looping = looping;
    sound->next = NULL;

    return sound;
}

PTSound TSound::Duplicate()
{
    if (!SoundPlayer.Functioning())
        return NULL;

    PTSound sound = new TSound;

  // Copy format and size
    memcpy(&sound->format, &format, sizeof(WAVEFORMATEX));
    sound->size = size;

  // Copy buffer
    HRESULT res = SoundPlayer.DirectSound->DuplicateSoundBuffer(SoundBuffer, &sound->SoundBuffer);
    if (res != DS_OK)
    {
        delete sound;
        return NULL;
    }
 
  // Great!
    return sound;
}

#define MINDIST 256
#define MAXDIST 1024

void TSound::Play(int volume, int freq, PS3DPoint dpos)
{
    if (!SoundPlayer.Functioning() || SoundBuffer == NULL)
        return;
 
  // Get volume and panning based on delta point
    int pan = 0;
    if (dpos)
    {
      // Get volume based on distance
        int distance = (int)sqrt((float)dpos->x * (float)dpos->x + (float)dpos->y + (float)dpos->y);
        if (distance > MAXDIST)
            return;
        else if (distance > MINDIST)
            volume = ((volume - DSBVOLUME_MIN) * (MAXDIST - distance) / (MAXDIST - MINDIST)) + DSBVOLUME_MIN;

      // Get panning based on angle from position
        double a = atan2(dpos->y, dpos->x);
        a += M_PI / 4.0;
        pan = -(int)(cos(a) * (double)DSBPAN_RIGHT);
    }
    
    SoundBuffer->SetVolume(volume);
    SoundBuffer->SetFrequency(freq);
    SoundBuffer->SetPan(pan);

    SoundBuffer->SetCurrentPosition(0);
    HRESULT error = SoundBuffer->Play(0, 0, looping ? DSBPLAY_LOOPING : 0);
    if (error != DS_OK)
    {
        if (error == DSERR_BUFFERLOST)
            error = 0;
        else if (error == DSERR_INVALIDCALL)
            error = 0;
        else if (error == DSERR_INVALIDPARAM)
            error = 0;
        else if (error == DSERR_PRIOLEVELNEEDED)
            error = 0;
    }

//  if (volume != 0)
//      SoundBuffer->SetVolume(DSBVOLUME_MAX);
}

void TSound::Stop()
{
    if (!SoundPlayer.Functioning() || SoundBuffer == NULL)
        return;

    SoundBuffer->Stop();
}

DWORD TSound::GetStatus()
{
    if (!SoundPlayer.Functioning() || SoundBuffer == NULL)
        return 0;

    DWORD status;
    SoundBuffer->GetStatus(&status);

    return status;
}

//void TSound::SetPosition(int x, int y, int z)
//{
//  SoundBuffer3D->SetMode(DS3DMODE_HEADRELATIVE, DS3D_IMMEDIATE);
//  SoundBuffer3D->SetPosition((float)x * DISTFACTOR, (float)y * DISTFACTOR, (float)z * DISTFACTOR, DS3D_IMMEDIATE);
//}

//void TSound::SetVelocity(int x, int y, int z)
//{
//  SoundBuffer3D->SetMode(DS3DMODE_HEADRELATIVE, DS3D_IMMEDIATE);
//  SoundBuffer3D->SetPosition((float)x * DISTFACTOR, (float)y * DISTFACTOR, (float)z * DISTFACTOR, DS3D_IMMEDIATE);
//}

//void TSound::SetOrientation(float x, float y, float z)
//{
//  SoundBuffer3D->SetMode(DS3DMODE_HEADRELATIVE, DS3D_IMMEDIATE);
//  SoundBuffer3D->SetConeOrientation(x, y, z, DS3D_IMMEDIATE);
//}

// ***********************
// * Sound Effect Player *
// ***********************

BOOL TSoundPlayer::Initialize()
{
    // Note that the soundplayer returns TRUE even when directsound fails
    // to initialize: this is because we don't want to fail out of the
    // game if they don't have a soundcard...

    HRESULT res = DirectSoundCreate(NULL, &DirectSound, NULL);
    if (res != DS_OK)
        return TRUE;

    res = DirectSound->SetCooperativeLevel(MainWindow.Hwnd(), DSSCL_PRIORITY);
    if (res != DS_OK)
    {
        DirectSound = NULL;
        return TRUE;
    }

    DSBUFFERDESC desc;
    memset(&desc, 0, sizeof(DSBUFFERDESC));
    desc.dwSize = sizeof(DSBUFFERDESC);
    desc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;
    desc.dwBufferBytes = 0;
    desc.lpwfxFormat = NULL;

    res = DirectSound->CreateSoundBuffer(&desc, &PrimaryBuffer, NULL);
    if (res != DS_OK)
    {
        DirectSound->Release();
        DirectSound = NULL;
        return TRUE;
    }

  // Set primary buffer format     WAVEFORMATEX wfx;
    WAVEFORMATEX wfx;
    memset(&wfx, 0, sizeof(WAVEFORMATEX)); 
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 2; 
    wfx.nSamplesPerSec = 44100;
    wfx.wBitsPerSample = 16; 
    wfx.nBlockAlign = wfx.wBitsPerSample / 8 * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign; 
    res = PrimaryBuffer->SetFormat(&wfx);
    if (res != DS_OK)
    {
        DirectSound->Release();
        DirectSound = NULL;
        return TRUE;
    }

    // Get listener interface
//  res = PrimaryBuffer->QueryInterface(IID_IDirectSound3DListener, (LPVOID *)&Listener);
//  if (res != DS_OK)
//  {
//      DirectSound->Release();
//      DirectSound = NULL;
//      return TRUE;
//  }

  // Set the listener format
//  Listener->SetRolloffFactor(1.0f, DS3D_IMMEDIATE);
//  Listener->SetDopplerFactor(0.0f, DS3D_IMMEDIATE);
//  Listener->SetVelocity(0.0f, 0.0f, 0.0f, DS3D_IMMEDIATE);
//  Listener->SetOrientation(-0.7f, -0.7f, 0.0f, 0.0f, 0.0f, 0.99f, DS3D_IMMEDIATE);

  // Start primary buffer
    PrimaryBuffer->Play(0, 0, DSBPLAY_LOOPING);

  // Reads in sound list
    ReadSoundList();

    return TRUE;
}

void TSoundPlayer::Close()
{
    DestroySoundList();

    if (PrimaryBuffer)
    {
        PrimaryBuffer->Release();
        PrimaryBuffer = NULL;
    }

    if (DirectSound)
    {
        DirectSound->Release();
        DirectSound = NULL;
    }
}

void TSoundPlayer::Pause()
{
    if (Functioning())
        PrimaryBuffer->Stop();
}

void TSoundPlayer::Unpause()
{
    if (Functioning())
        PrimaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
}

void TSoundPlayer::SetVolume(int volume)
{
    if (Functioning())
        PrimaryBuffer->SetVolume(volume);
}

// The sound system high-level (easy-interface) functions

int TSoundPlayer::FindSound(char *soundname, int nr)
{
    char buf[80];
    if (nr >= 0)
    {
        sprintf(buf, "%s%d", soundname, nr);
        return FindSound(buf, -1);
    }
    else
        strcpy(buf, soundname);

    for (int c = 0; c < soundlist.NumItems(); c++)
    {
        PSSoundRef ref = soundlist[c];
        if (!ref)
            continue;

        if (!stricmp(ref->name, buf))
            return c;
    }

    return -1;
}

int TSoundPlayer::NewSound(char *soundname, int nr)
{
    char buf[80];
    if (nr >= 0)
    {
        sprintf(buf, "%s%d", soundname, nr);
        return FindSound(buf, -1);
    }
    else
        strcpy(buf, soundname);

    PSSoundRef ref = new SSoundRef;
    ref->name = strdup(buf);
    ref->resid = -1;
    ref->usecount = 0;
    ref->sound = NULL;

    int id = soundlist.Add(ref);
    if (id < 0)
    {
        free(ref->name);
        delete ref;
        return -1;
    }

    return id;
}

void TSoundPlayer::UpdateDying()
{
    for (int c = 0; c < soundlist.NumItems(); c++)
    {
        PSSoundRef ref = soundlist[c];
        if (!ref)
            continue;

      // Clear any duplicated sounds for this ref if they've finished playing
        if (ref->sound && ref->sound->Next())
        {
            PTSound prev, snd, next;

            prev = ref->sound;
            next = prev->Next();

            while ((snd = next))
            {
                next = snd->Next();

                if (snd->IsPlaying())
                    prev = snd;
                else
                {
                    prev->SetNext(snd->Next());
                    snd->SetNext(NULL);
                    delete snd;
                }
            }
        }

      // Clear the main sound if its SOUND_DYING flag is set
        if (ref->flags & SOUND_DYING)
        {
            if (ref->sound && (!ref->sound->IsPlaying() || ref->sound->IsLooping()))
            {
                delete ref->sound;
                ref->sound = NULL;
                ref->flags &= ~SOUND_DYING;
            }
        }
    }
}

BOOL TSoundPlayer::Mount(int id)
{
    if (!Functioning() || id < 0 || id >= soundlist.NumItems())
        return FALSE;

    PSSoundRef ref = soundlist[id];
    if (!ref)
        return FALSE;

    if (ref->usecount < 1)
    {
        if (ref->flags & SOUND_DYING)
            ref->flags &= ~SOUND_DYING;
        else
        {
            if (ref->resid != -1)
                ref->sound = TSound::Load(ref->resid);
            else
                ref->sound = TSound::Load(ref->name);
        }

        if (ref->sound)
            ref->usecount = 1;
        else
            ref->usecount = 0;
    }
    else
        ref->usecount++;

    UpdateDying();

    return TRUE;
}

BOOL TSoundPlayer::Unmount(int id)
{
    if (!Functioning() || id < 0 || id >= soundlist.NumItems())
        return FALSE;

    PSSoundRef ref = soundlist[id];
    if (!ref)
        return FALSE;

    ref->usecount--;

    if (ref->usecount < 1)
    {
        if (ref->sound)
        {
            if (ref->sound->IsPlaying() && !ref->sound->IsLooping())
                ref->flags |= SOUND_DYING;
            else
            {
                delete ref->sound;
                ref->sound = NULL;
            }
        }

        ref->usecount = 0;
    }

    UpdateDying();

    return TRUE;
}

BOOL TSoundPlayer::Play(int id, int volume, int freq, PS3DPoint dpos)
{
    if (!Functioning() || id < 0 || id >= soundlist.NumItems())
        return FALSE;

    PSSoundRef ref = soundlist[id];
    if (!ref)
        return FALSE;

    if (ref && ref->sound)
    {
        if (!ref->sound->IsPlaying())
            ref->sound->Play(volume, freq, dpos);
        else
        {
            // create a duplicate of the buffer to play seperately
            PTSound newsound = ref->sound->Duplicate();
            if (!newsound)
                return FALSE;

            newsound->SetNext(ref->sound->Next());
            ref->sound->SetNext(newsound);
            newsound->Play(volume);
        }
    }

    UpdateDying();

    return TRUE;
}

BOOL TSoundPlayer::Stop(int id)
{
    if (!Functioning() || id < 0 || id >= soundlist.NumItems())
        return FALSE;

    PSSoundRef ref = soundlist[id];
    if (!ref)
        return FALSE;

    if (ref && ref->sound)
        ref->sound->Stop();

    return TRUE;
}

// Returns actual sound object for sound (sound must be MOUNTED or this will return NULL)
PTSound TSoundPlayer::GetSound(int id)
{
    if (!Functioning() || id < 0 || id >= soundlist.NumItems())
        return NULL;

    PSSoundRef ref = soundlist[id];
    if (!ref)
        return NULL;

    return ref->sound;
}

//void TSoundPlayer::SetPosition(int x, int y, int z)
//{
//  Listener->SetPosition((float)x * DISTFACTOR, (float)y * DISTFACTOR, (float)z * DISTFACTOR, DS3D_IMMEDIATE);
//}

//void TSoundPlayer::SetVelocity(int x, int y, int z)
//{
//  Listener->SetPosition((float)x * DISTFACTOR, (float)y * DISTFACTOR, (float)z * DISTFACTOR, DS3D_IMMEDIATE);
//}

//void TSoundPlayer::SetOrientation(float facex, float facey, float facez,
//  float topx, float topy, float topz)
//{
//  Listener->SetOrientation(facex, facey, facez, topx, topy, topz, DS3D_IMMEDIATE);
//}

//void TSoundPlayer::CommitSettings()
//{
//  Listener->CommitDeferredSettings(); 
//}


BOOL TSoundPlayer::ReadSoundList()
{
    soundlist.Clear();

    char fname[MAX_PATH];
    sprintf(fname, "%ssound.def", ClassDefPath);

    FILE *fp = TryOpen(fname, "rb");
    if (fp == NULL)
        return FALSE;

    TFileParseStream s(fp, fname);
    TToken t(s);

    t.Get();

    PSSoundRef ref;
    char name[128];
    int resid;

    do
    {
        if (t.Type() == TKN_RETURN || t.Type() == TKN_WHITESPACE)
            t.LineGet();

        if (t.Type() == TKN_EOF)
            break;

        if (!Parse(t, "%s %d", name, &resid))
            return FALSE;

        ref = new SSoundRef;
        ref->name = strdup(name);
        ref->resid = resid;
        ref->usecount = 0;
        ref->sound = NULL;

        soundlist.Add(ref);

    } while (t.Type() != TKN_EOF);

    fclose(fp);

  // ****** Search for sounds in current sound directory

    makepath(ResourcePath, fname, MAX_PATH - 1);
    strcat(fname, Language);
    strcat(fname, "\\*.wav");

    struct _finddata_t data;

    long found, handle;
    found = handle = _findfirst(fname, &data);
    while (found != -1)
    {
        strcpy(name, data.name);
        char *p = strchr(name, '.');
        if (p)
            *p = NULL;

        ref = new SSoundRef;
        ref->name = strdup(name);
        ref->resid = -1;
        ref->usecount = 0;
        ref->sound = NULL;

        soundlist.Add(ref);

        found = _findnext(handle, &data);
    }

    return TRUE;
}

void TSoundPlayer::DestroySoundList()
{
    for (int c = 0; c < soundlist.NumItems(); c++)
    {
        PSSoundRef ref = soundlist[c];
        if (!ref)
            continue;

        if (ref->sound)
            delete ref->sound;

        free(ref->name);
    }

    soundlist.DeleteAll();
}



