// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  sound.cpp - Music and sound module                   *
// *************************************************************************

#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <stdio.h>
#include <dsound.h>

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
/*	char buf[40];
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

TSound::TSound()
{
	SoundBuffer = NULL;
	looping = FALSE;
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

PTSound TSound::Load(int resource)
{
	PTWaveData rawdata = (PTWaveData)LoadResource("sound\\wave", resource);
	return Load(rawdata);
}

PTSound TSound::Load(PTWaveData wave)
{
	if (!SoundPlayer.Functioning())
		return NULL;

	if (!wave)
		return NULL;

	PTSound sound = new TSound;

	DSBUFFERDESC desc;
	memset(&desc, 0, sizeof(DSBUFFERDESC));
	desc.dwSize = sizeof(DSBUFFERDESC);
	desc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_STATIC;
	desc.dwBufferBytes = wave->size;
	desc.lpwfxFormat = &wave->format;

	HRESULT res = SoundPlayer.DirectSound->CreateSoundBuffer(&desc, &(sound->SoundBuffer), NULL);
	if (res != DS_OK)
	{
		delete wave;
		delete sound;
		return NULL;
	}

	LPVOID buffer;
	res = sound->SoundBuffer->Lock(0, wave->size, &buffer, &wave->size, NULL, NULL, 0);
	if (res == DS_OK)
	{
		memcpy(buffer, wave->data, wave->size);
		sound->SoundBuffer->Unlock(&buffer, wave->size, NULL, 0);
	}

	if ((wave->loopend - wave->loopstart) > 0)
		sound->looping = TRUE;

	delete wave;
	return sound;
}

PTSound TSound::Duplicate()
{
	if (!SoundPlayer.Functioning())
		return NULL;

	PTSound sound = new TSound;

	HRESULT res = SoundPlayer.DirectSound->DuplicateSoundBuffer(SoundBuffer, &sound->SoundBuffer);
	if (res != DS_OK)
	{
		delete sound;
		return NULL;
	}

	return sound;
}

void TSound::Play(int volume, int freq)
{
	if (!SoundPlayer.Functioning() || SoundBuffer == NULL)
		return;

	if (volume != 0)
		SoundBuffer->SetVolume(volume);

	SoundBuffer->SetFrequency(freq);

	SoundBuffer->SetCurrentPosition(0);
	SoundBuffer->Play(0, 0, looping ? DSBPLAY_LOOPING : 0);

	if (volume != 0)
		SoundBuffer->SetVolume(0);
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

// ***********************
// * Sound Effect Player *
// ***********************

BOOL TSoundPlayer::Initialize()
{
	if (!ReadSoundList())
		return FALSE;

	// Note that the soundplayer returns TRUE even when directsound fails
	// to initialize: this is because we don't want to fail out of the
	// game if they don't have a soundcard...

	HRESULT res = DirectSoundCreate(NULL, &DirectSound, NULL);
	if (res != DS_OK)
		return TRUE;

	res = DirectSound->SetCooperativeLevel(MainWindow.Hwnd(), DSSCL_EXCLUSIVE);
	if (res != DS_OK)
	{
		DirectSound = NULL;
		return TRUE;
	}

	DSBUFFERDESC desc;
	memset(&desc, 0, sizeof(DSBUFFERDESC));
	desc.dwSize = sizeof(DSBUFFERDESC);
	desc.dwFlags = DSBCAPS_PRIMARYBUFFER;
	desc.dwBufferBytes = 0;
	desc.lpwfxFormat = NULL;

	res = DirectSound->CreateSoundBuffer(&desc, &PrimaryBuffer, NULL);
	if (res != DS_OK)
	{
		DirectSound->Release();
		DirectSound = NULL;
		return TRUE;
	}

	PrimaryBuffer->Play(0, 0, DSBPLAY_LOOPING);

	return TRUE;
}

void TSoundPlayer::Close()
{
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

	DestroySoundList();
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

PSSoundRef TSoundPlayer::FindSound(char *soundname, int nr)
{
	if (nr >= 0)
	{
		char buf[80];
		sprintf(buf, "%s%d", soundname, nr);
		return FindSound(buf, -1);
	}

	PSSoundRef next, prev = NULL;

	for (PSSoundRef ref = soundlist; ref; prev = ref, ref = next)
	{
		next = ref->next;

		if (stricmp(ref->name, soundname) == 0)
		{
			if (prev)
			{
				// move it to the head of the list
				prev->next = ref->next;
				ref->next = soundlist;
				soundlist = ref;
			}

			return ref;
		}
	}

	return NULL;
}

void TSoundPlayer::UpdateDying()
{
	PSSoundRef ref;
	for (ref = soundlist; ref; ref = ref->next)
	{
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

void TSoundPlayer::Mount(char *soundname, int nr)
{
	if (!Functioning())
		return;

	PSSoundRef ref = FindSound(soundname, nr);
	if (!ref)
		return;

	if (ref->usecount < 1)
	{
		if (ref->flags & SOUND_DYING)
			ref->flags &= ~SOUND_DYING;
		else
			ref->sound = TSound::Load(ref->resid);

		ref->usecount = 1;
	}
	else
		ref->usecount++;

	UpdateDying();
}

void TSoundPlayer::Unmount(char *soundname, int nr)
{
	if (!Functioning())
		return;

	PSSoundRef ref = FindSound(soundname, nr);
	if (!ref)
		return;

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
}

void TSoundPlayer::Play(char *soundname, int nr, int volume, int freq)
{
	if (!Functioning())
		return;

	PSSoundRef ref = FindSound(soundname, nr);
	if (!ref)
		return;

	if (ref && ref->sound)
	{
		if (!ref->sound->IsPlaying())
			ref->sound->Play(volume, freq);
		else
		{
			// create a duplicate of the buffer to play seperately
			PTSound newsound = ref->sound->Duplicate();
			if (!newsound)
				return;

			newsound->SetNext(ref->sound->Next());
			ref->sound->SetNext(newsound);
			newsound->Play(volume);
		}
	}

	UpdateDying();
}

void TSoundPlayer::Stop(char *soundname)
{
	if (!Functioning())
		return;

	PSSoundRef ref = FindSound(soundname);

	if (ref && ref->sound)
		ref->sound->Stop();
}

BOOL TSoundPlayer::ReadSoundList()
{
	soundlist = NULL;

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
		ref->next = soundlist;
		soundlist = ref;

	} while (t.Type() != TKN_EOF);

	fclose(fp);
	return TRUE;
}

void TSoundPlayer::DestroySoundList()
{
	PSSoundRef next;

	for (PSSoundRef ref = soundlist; ref; ref = next)
	{
		next = ref->next;
		delete ref;
	}

	soundlist = NULL;
}



