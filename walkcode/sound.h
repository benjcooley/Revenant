// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   sound.h - Music and sound module                    *
// *************************************************************************

#ifndef _SOUND_H
#define _SOUND_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

// Make it so we don't have to include directsound
#ifndef __DSOUND_INCLUDED__
struct IDirectSoundBuffer;
typedef struct IDirectSoundBuffer *LPDIRECTSOUNDBUFFER;
struct IDirectSound;
typedef struct IDirectSound *LPDIRECTSOUND;
#endif

// CD functions
void CDOpen();
void CDClose();
void CDPlayTrack(int track);
DWORD CDTrackLength(int track);
void CDPlayRandomTrack();
void CDStop();
BOOL CDPlaying();
void CDSetVolume(WORD volume);

// Wave data structure
_CLASSDEF(TWaveData)

// Sound effects classes
_CLASSDEF(TSound)
class TSound
{
  public:
    TSound();
    ~TSound();

    static PTSound Load(PTWaveData wave);
    static PTSound Load(int resource);

    PTSound Duplicate();
        // Make a duplicate of this sound

    void Play(int volume = -1, int freq = -1);
    void Stop();

    DWORD GetStatus();
    BOOL IsPlaying();
    BOOL IsLooping();

    PTSound Next() { return next; }
    void SetNext(PTSound n) { next = n; }
        // Linked list utils

  protected:
    LPDIRECTSOUNDBUFFER SoundBuffer;
    BOOL looping;

    PTSound next;           // next in list
};

// Sound ref flags
#define SOUND_DYING     (1 << 0)        // The sound should be deallocated once it stops playing

_STRUCTDEF(SSoundRef)
struct SSoundRef
{
    char *name;             // name of sound
    int resid;              // resource id
    int usecount;           // how many in use
    DWORD flags;            // sound flags
    PTSound sound;          // sound data, may be more than one

    PSSoundRef next;        // next in list
};

_CLASSDEF(TSoundPlayer)
class TSoundPlayer
{
  public:
    TSoundPlayer() { DirectSound = NULL; PrimaryBuffer = NULL; soundlist = NULL; }
    ~TSoundPlayer() { Close(); }

    BOOL Initialize();
    void Close();

    BOOL Functioning() { return (DirectSound && PrimaryBuffer); }

    void Pause();
    void Unpause();
        // Start and stop all sound effects (ie, game pausing/unpausing)

    void SetVolume(int volume = 0);
        // An argument of 0 is the normal playing level

    // A simple sound garbage-collector to make playing sounds a bit handier
    void Mount(char *soundname, int nr = -1);
        // Inform sound system to prepare this sound for later use
    void Unmount(char *soundname, int nr = -1);
        // Inform sound system that you are done with this sound
    void Play(char *soundname, int nr = -1, int volume = -1, int freq = -1);
        // Play a mounted sound
    void Stop(char *soundname);
        // Stop a playing, mounted sound

    LPDIRECTSOUND DirectSound;

  private:
    BOOL ReadSoundList();
        // Load in sound list for easy-access functions
    void DestroySoundList();
        // Delete the sound list
    PSSoundRef FindSound(char *soundname, int nr = -1);
        // Find a given sound
    void UpdateDying();
        // Loop through ref list and kill off any dying sounds

    LPDIRECTSOUNDBUFFER PrimaryBuffer;

    PSSoundRef soundlist;           // linked list of sound refs
};

// Easy access function for one-time sounds
#define PLAY(x)     { SoundPlayer.Mount(x); SoundPlayer.Play(x); SoundPlayer.Unmount(x); }
#define PLAYN(x, n) { SoundPlayer.Mount((x), (n)); SoundPlayer.Play((x), (n)); SoundPlayer.Unmount((x), (n)); }

#endif