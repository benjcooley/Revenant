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

#ifndef _WAVEDATA_H
#include "wavedata.h"
#endif

// Make it so we don't have to include directsound
#ifndef __DSOUND_INCLUDED__
struct IDirectSoundBuffer;
typedef struct IDirectSoundBuffer *LPDIRECTSOUNDBUFFER;
struct IDirectSound;
typedef struct IDirectSound *LPDIRECTSOUND;
struct IDirectSound3DBuffer;
typedef struct IDirectSound3DBuffer *LPDIRECTSOUND3DBUFFER;
struct IDirectSound3DListener;
typedef struct IDirectSound3DListener *LPDIRECTSOUND3DLISTENER;
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

// Load a wave as a game sound object
PTWaveData LoadWave(char *filename, 
    long volume = 0, long loopstart = 0x7FFFFFFF, long loopend = 0x7FFFFFFF);

// Wave data structure
_CLASSDEF(TWaveData)

// Sound effects classes
_CLASSDEF(TSound)
class TSound
{
  public:
    TSound();
    ~TSound();

    static PTSound Load(WAVEFORMATEX *format, DWORD size, BYTE *data, BOOL looping);
    static PTSound Load(char *name, int dirresid);
    static PTSound Load(int resid);

    PTSound Duplicate();
        // Make a duplicate of this sound

    void Play(int volume = 0, int freq = 0, PS3DPoint lpos = NULL, PS3DPoint spos = NULL);
    void Stop();

    void SetListenerPos(PS3DPoint lpos = NULL);
        // set the listener's position for direction-based audio
    void SetSoundPos(PS3DPoint spos = NULL);
        // set the sound's position for direction-based audio

    void GetListenerPos(PS3DPoint lpos) { memcpy(lpos, &listener_pos, sizeof(S3DPoint)); }
    void GetSoundPos(PS3DPoint spos) { memcpy(spos, &sound_pos, sizeof(S3DPoint)); }
    int GetSoundVolume() { return sound_volume; }

    DWORD GetStatus();
    BOOL IsPlaying();
    BOOL IsLooping();
    void SetLooping(BOOL state) { looping = state; }

    PTSound Next() { return next; }
    void SetNext(PTSound n) { next = n; }
        // Linked list utils

    int GetSize() { return size; }
      // Gets size of wave buffer data
    int GetChannels() { return format.nChannels; }
      // Gets number of channels for buffer
    int GetSamples() { return size / format.nBlockAlign; }
      // Gets number of samples for buffer data
    int GetSamplesPerSec() { return format.nSamplesPerSec; }
      // Gets number of samples for buffer data
    int GetLength() { return GetSamples() * 100 / GetSamplesPerSec(); }
      // Returns length of sound in 100ths of a second

  // 3D Sound parameters!
//  void SetPosition(int x, int y, int z);
//    // Set sound maker position
//  void SetVelocity(int x, int y, int z);
//    // Set sound maker velocity
//  void SetOrientation(float x, float y, float z);
//    // Set sound maker orientation (unit vector in float format)
//  void CommitSettings();
//    // Commits the previously set settings

  protected:
    S3DPoint listener_pos;
    S3DPoint sound_pos;
    int sound_volume;
        // save the volume of this sound so that we can get the correct value later on...
    
    DWORD size; 
    WAVEFORMATEX format;
    LPDIRECTSOUNDBUFFER SoundBuffer;
//  LPDIRECTSOUND3DBUFFER SoundBuffer3D;
    BOOL looping;

    PTSound next;           // next in list
};

// Sound ref flags
#define SOUND_DYING     (1 << 0)        // The sound should be deallocated once it stops playing

#define DIRRESID_EFFECTDIR -1
#define DIRRESID_DIALOGDIR -2

_STRUCTDEF(SSoundRef)
struct SSoundRef
{
    char *name;             // name of sound
    char *dir;              // directory where sound is stored
    int resid;              // Resource id (negative value is directory id for WAV files)
    int usecount;           // how many in use
    DWORD flags;            // sound flags
    PTSound sound;          // sound data, may be more than one
};
typedef TPointerArray<SSoundRef, 32, 32> TSoundArray;

_CLASSDEF(TSoundPlayer)
class TSoundPlayer
{
  public:
    friend class TSound;

    TSoundPlayer() { DirectSound = NULL; PrimaryBuffer = NULL; soundlist.Clear(); }
    ~TSoundPlayer() { Close(); }

    BOOL Initialize();
    void Close();

    BOOL Functioning() { return (DirectSound && PrimaryBuffer); }

    void Pause();
    void Unpause();
        // Start and stop all sound effects (ie, game pausing/unpausing)

    void SetVolume(int volume = 0);
        // An argument of 0 is the normal playing level

  // Finds sound id's by name
    int FindSound(char *soundname, int nr = -1);
        // Find a given sound
    int NewSound(char *soundname, int nr = -1);
        // Creates a new sound

  // Simple sound garbage-collector functions to make playing sounds a bit handier
  // These functions work with the sound id returned from FindSound()
    BOOL Mount(int id);
        // Inform sound system to prepare this sound for later use
    BOOL Unmount(int id);
        // Inform sound system that you are done with this sound
    BOOL Play(int id, int volume = 0, int freq = 0, PS3DPoint spos = NULL);
        // Play a mounted sound
    BOOL Stop(int id);
        // Stop a playing, mounted sound

  // Sound name functions (has to search sound list every time, but easier to use)
    BOOL Mount(char *n, int nr = -1) { return Mount(FindSound(n, nr)); }
        // Mount by sound name and number
    BOOL Unmount(char *n, int nr = -1) { return Unmount(FindSound(n, nr)); }
        // Unmount by sound name and number
    BOOL Play(char *n, int nr = -1, int volume = -1, int freq = -1)
      { return Play(FindSound(n, nr), volume, freq); }
        // Play a mounted sound  by name and number
    BOOL Stop(char *n, int nr = -1) { return Stop(FindSound(n, nr)); }
        // Stop a sound by name and number 

  // 3D Sound functions
    void SetListenerPos(int x, int y, int z);
//    // Sets the current listeners position

//  void SetOrientation(float facex, float facey, float facez, 
//      float topx, float topy, float topz);
//    // Set the current listener's orientation (f-front vector, t-top of head vector)
//  void SetVelocity(int x, int y, int z);
//    // Set the current listener's orientation
//  void CommitSettings();
//    // Commits all settings that were just set

  // Returns actual sound object for sound
  // NOTE: Sound must be MOUNTED or this will return NULL!
    PTSound GetSound(int id);

    LPDIRECTSOUND DirectSound;

  private:
    BOOL SearchSoundDir(char *soundpath, char *subdir, int dirresid);
        // Searches the given sound dir in the given sound path for all WAV files
        // and adds them to the sound list
    BOOL ReadSoundList();
        // Reads in sound list from SOUND.DEF file (for backwards compatibility)
    void DestroySoundList();
        // Destroys sound list
    void UpdateDying();
        // Loop through ref list and kill off any dying sounds

    LPDIRECTSOUNDBUFFER PrimaryBuffer;
//  LPDIRECTSOUND3DLISTENER Listener;

    TSoundArray soundlist;          // Array of active sounds

    S3DPoint listener_pos;
};

// Easy access function for one-time sounds
inline BOOL PLAY(char *x)
{
    int id = SoundPlayer.FindSound(x);
    if (id < 0)
        return FALSE;
    if (!SoundPlayer.Mount(id))
        return FALSE;
    if (!SoundPlayer.Play(id))
        return FALSE;
    if (!SoundPlayer.Unmount(id))
        return FALSE;
    return TRUE;
}

inline BOOL PLAYN(char *x, int n)
{
    int id = SoundPlayer.FindSound(x, n);
    if (id < 0)
        return FALSE;
    if (!SoundPlayer.Mount(id))
        return FALSE;
    if (!SoundPlayer.Play(id))
        return FALSE;
    if (!SoundPlayer.Unmount(id))
        return FALSE;
    return TRUE;
}

#endif