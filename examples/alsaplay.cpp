/*
	Audio File Library

	Copyright (c) 2010, Michael Pruett.  All rights reserved.

	Redistribution and use in source and binary forms, with or
	without modification, are permitted provided that the following
	conditions are met:

	1. Redistributions of source code must retain the above copyright
	notice, this list of conditions and the following disclaimer.

	2. Redistributions in binary form must reproduce the above
	copyright notice, this list of conditions and the following
	disclaimer in the documentation and/or other materials provided
	with the distribution.

	3. The name of the author may not be used to endorse or promote
	products derived from this software without specific prior
	written permission.

	THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
	OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
	WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
	NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
	THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
		Audio File Library - Playback using ALSA on Linux or PortAudio elsewhere
*/

#include <audiofile.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef HAVE_ALSA
#include <alsa/asoundlib.h>
#elif defined(HAVE_PORTAUDIO)
#include <portaudio.h>
#else
#error "No audio backend available!"
#endif

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		fprintf(stderr, "usage: %s filename\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	// Open audio file using Audio File Library
	AFfilehandle file = afOpenFile(argv[1], "r", AF_NULL_FILESETUP);
	if (!file)
	{
		fprintf(stderr, "Could not open '%s'.\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	int channels = afGetChannels(file, AF_DEFAULT_TRACK);
	double rate = afGetRate(file, AF_DEFAULT_TRACK);

	// Force 16-bit 2's complement output for simplicity
	afSetVirtualSampleFormat(file, AF_DEFAULT_TRACK, AF_SAMPFMT_TWOSCOMP, 16);

	const int bufferFrames = 4096;
	int16_t *buffer = new int16_t[bufferFrames * channels];

#if USE_ALSA
	// --- ALSA playback for Linux ---
	int err;
	snd_pcm_t *handle;

	if ((err = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0)) < 0)
	{
		fprintf(stderr, "Could not open audio output: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}

	if ((err = snd_pcm_set_params(handle, SND_PCM_FORMAT_S16,
		SND_PCM_ACCESS_RW_INTERLEAVED, channels, rate, 1, 500000)) < 0)
	{
		fprintf(stderr, "Could not set audio output parameters: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}

	while (true)
	{
		// Read frames from the audio file
		AFframecount framesRead = afReadFrames(file, AF_DEFAULT_TRACK, buffer, bufferFrames);
		if (framesRead <= 0)
			break;

		// Write frames to ALSA device
		snd_pcm_sframes_t framesWritten = snd_pcm_writei(handle, buffer, framesRead);
		if (framesWritten < 0)
			framesWritten = snd_pcm_recover(handle, framesWritten, 0);
		if (framesWritten < 0)
		{
			fprintf(stderr, "Could not write audio data: %s\n", snd_strerror(framesWritten));
			break;
		}
	}

	snd_pcm_drain(handle);
	snd_pcm_close(handle);

#else
	// --- PortAudio playback for non-Linux platforms ---
	PaError err;
	err = Pa_Initialize();
	if (err != paNoError)
	{
		fprintf(stderr, "PortAudio init failed: %s\n", Pa_GetErrorText(err));
		exit(EXIT_FAILURE);
	}

	PaStream *stream;
	err = Pa_OpenDefaultStream(&stream,
														 0,         // no input channels
														 channels,  // output channels
														 paInt16,   // 16-bit PCM
														 rate,
														 bufferFrames,
														 NULL,      // no callback, blocking
														 NULL);
	if (err != paNoError)
	{
		fprintf(stderr, "PortAudio open stream failed: %s\n", Pa_GetErrorText(err));
		Pa_Terminate();
		exit(EXIT_FAILURE);
	}

	err = Pa_StartStream(stream);
	if (err != paNoError)
	{
		fprintf(stderr, "PortAudio start stream failed: %s\n", Pa_GetErrorText(err));
		Pa_CloseStream(stream);
		Pa_Terminate();
		exit(EXIT_FAILURE);
	}

	while (true)
	{
		AFframecount framesRead = afReadFrames(file, AF_DEFAULT_TRACK, buffer, bufferFrames);
		if (framesRead <= 0)
			break;

		err = Pa_WriteStream(stream, buffer, framesRead);
		if (err != paNoError)
		{
			fprintf(stderr, "PortAudio write failed: %s\n", Pa_GetErrorText(err));
			break;
		}
	}

	Pa_StopStream(stream);
	Pa_CloseStream(stream);
	Pa_Terminate();
#endif

	delete[] buffer;
	afCloseFile(file);

	return 0;
}
