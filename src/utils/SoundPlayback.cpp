/*
 * SoundPlayback.cpp
 *
 * Copyright (C) 2013 Holger Grosenick
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define ALSA_PCM_NEW_HW_PARAMS_API

#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <thread>

#ifndef DONT_USE_ALSA
#include <alsa/asoundlib.h>
#else
#define snd_pcm_close(x)
#define snd_config_update_free_global()
#endif

#include <utils/utils.h>

#include "SoundPlayback.h"
#include "Log.h"

static std::mutex g_sound_mutex;

/**
 * See https://ccrma.stanford.edu/courses/422/projects/WaveFormat/
 */
struct WavHeader {
    char  chunkID[4];     // Contains the letters "RIFF" in ASCII form
    int   chunkSize;
    char  format[4];      // Contains the letters "WAVE"
    char  subchunk1ID[4]; // Contains the letters "fmt "
    int   subchunk1Size;  // 16 for PCM.  This is the size of the
    short audioFormat;    // PCM = 1 (i.e. Linear quantization)
                          // Values other than 1 indicate some form of compression.
    short numChannels;    // Mono = 1, Stereo = 2, etc.
    int   sampleRate;     // 8000, 44100, etc.
    int   byteRate;       // SampleRate * NumChannels * BitsPerSample/8
    short blockAlign;     // NumChannels * BitsPerSample/8
    short bitsPerSample;  // 8 bits = 8, 16 bits = 16, etc.
    short extraParamSize;
};

/**
 * shared_ptr calls 'delete', but we need 'delete[]' here for the char* which is an array.
 */
template <typename T> class ArrayDeleter
{
public:
    void operator () (T* d) const
    {
        delete [] d;
    }
};


// this is about 6 seconds of 44KHz WAV file
static const int MAX_SIZE = 1024000;
static int instanceCount = 0;


namespace utils {


static std::thread *playbackThread;
static SoundPlayback *threadObject;

/**
 * Init: empty values
 */
SoundPlayback::WavBuffer::WavBuffer()
  : data()
  , size(-1)
{ }

/**
 * Allocate char* array with given size.
 */
SoundPlayback::WavBuffer::WavBuffer(int s)
: data()
, size(s)
{
    if (s > 0)
    {
        char *v = new char[s];
        bzero(v, s);
        data.reset(v, ArrayDeleter<char>());
    }
}

SoundPlayback::WavBuffer::~WavBuffer() {

}


/**
 * Try to stop an active thread.
 */
static void stopThread()
{
    if (threadObject)
        threadObject->stopPlayback();
    if (playbackThread) {
        playbackThread->join();
        delete playbackThread;
    }
    playbackThread = NULL;
}

/**
 * Start playback.
 */
void SoundPlayback::triggerSound()
{
    //printf("replaySound(), object = %p\n", threadObject);
    if (threadObject) {
        threadObject->playSound();
        threadObject = NULL;
    }
}

/**
 * Init.
 */
SoundPlayback::SoundPlayback()
  : finish(false)
  , done(true)
  , swapBuffer(false)
  , channels(-1)
  , sampleRate(-1)
  , sampleSize(-1)
  , playbackHandle(NULL)
{
    ++instanceCount;
    //printf("Created %d\n", instanceCount);
}


SoundPlayback::~SoundPlayback() noexcept
{
    closeSoundHandle();
    --instanceCount;
}

/**
 * Loads a sound file into memory.
 * @return a buffer if OK, NULL otherwise
 */
bool SoundPlayback::loadSound(const char *name)
{
    FILE *f = fopen(name, "r");
    if (f == NULL)
    {
        Log::perror(name);
        return false;
    }

    struct stat statBuf;
    int rc = fstat(fileno(f), &statBuf);
    if (rc != 0)
    {
        Log::perror("fstat()");
        fclose(f);
        return false;
    }

    if (statBuf.st_size > MAX_SIZE)
    {
        Log::log(std::string("Wav-file ") + name + " too large, reduced to ", MAX_SIZE);
        wavBuffer.size = MAX_SIZE;
    }
    else {
        wavBuffer.size = statBuf.st_size;
    }

    wavBuffer.data.reset(new char[wavBuffer.size]);
    rc = fread(wavBuffer.data.get(), 1, wavBuffer.size, f);
    if (rc != wavBuffer.size)
    {
        clearBuffer();
        Log::log("read-size mismatch, read-rc ==", rc);
    }

    fclose(f);
    return true;
}

/**
 * Create a wav file with a constant frequency.
 * See also
 * http://gasstationwithoutpumps.wordpress.com/2011/10/08/making-wav-files-from-c-programs/
 */
SoundPlayback::WavBuffer SoundPlayback::makeWav(SoundList &sounds)
{
    // first sum up all the durations
    unsigned durationSum = 0;
    for (auto it : sounds)
    {
        durationSum += it.first;
    }

    const unsigned num_channels     = 1;
    const unsigned sample_rate      = 22000;
    const unsigned bytes_per_sample = 2;
    const unsigned byte_rate        = sample_rate*num_channels*bytes_per_sample;
    unsigned num_samples            = 22 * durationSum;

    WavHeader header = {
            {'R', 'I', 'F', 'F'}, // chunkID[4]
            (int) (38 + bytes_per_sample * num_samples * num_channels), // chunkSize
            {'W', 'A', 'V', 'E'}, // format[4]
            "fmt",  // subchunk1ID[4]
            16,     // subchunk1Size
            1,      // audioFormat, PCM = 1
            num_channels,
            sample_rate,
            byte_rate,       // SampleRate * NumChannels * BitsPerSample/8
            num_channels*bytes_per_sample, // blockAlign
            8*bytes_per_sample, //bitsPerSample
            0
    };

    int size = sizeof(WavHeader) + 8 + bytes_per_sample * num_samples * num_channels;
    WavBuffer result(size);

    char *cur = result.data.get();
    memcpy(cur, &header, sizeof(header));

    cur += sizeof(WavHeader);
    memcpy(cur, "data", 4);
    cur += 4;

    /* write data subchunk */
    unsigned xx = bytes_per_sample* num_samples*num_channels;
    memcpy(cur, &xx, 4);
    cur += 4;

    for (auto it : sounds)
    {
        float amplitude = 16000;  // can be up to 32000, but piezo speaker isn't that loud
        float phase = 0;
        float freq_radians_per_sample = it.second * 2 * M_PI/sample_rate;

        /* fill buffer with a sine wave */
        unsigned soundSamples = 22 * it.first;
        for (unsigned i=0; i < soundSamples; ++i)
        {
            phase += freq_radians_per_sample;
            short value = (short)(amplitude * sin(phase));
            memcpy(cur, &value, bytes_per_sample);
            cur += bytes_per_sample;
        }
        num_samples -= soundSamples;
    }

    if (num_samples != 0)
        throw std::logic_error("num_sample is not 0!");
    return result;
}

/**
 * Create a wav file with a given duration and a constant frequency.
 */
SoundPlayback::WavBuffer SoundPlayback::makeWav(unsigned durationMs, unsigned frequencyHz)
{
    SoundList sounds;
    sounds.push_back(std::make_pair(durationMs, frequencyHz));
    return makeWav(sounds);
}

/**
 * Open the sound device. Returns the sample size in bytes if it was OK,
 * a value<= 0 otherwise. Only WAV files are supported !
 */
int SoundPlayback::openSoundHandle() noexcept
{
    if (!wavBuffer.data.get())
        return -1;

    WavHeader *hdr = (WavHeader*) wavBuffer.data.get();

#ifdef MAIN
    // for testing
    printf("audioFormat: %d\n"
	   "numChannels: %d\n"
           "blockAlign:  %d\n"
	   "sampleRate:  %d\n"
           "bits/sample: %d\n"
	   "byteRate:    %d\n"
	   , hdr->audioFormat
           , hdr->numChannels
           , hdr->blockAlign
	   , hdr->sampleRate
	   , hdr->bitsPerSample
	   , hdr->byteRate);
#endif

    if (memcmp("WAVE", hdr->format, 4) != 0) {
        fprintf(stderr, "Format error, expected 'WAVE', but found '%c%c%c%c'\n",
                hdr->format[0], hdr->format[1], hdr->format[2], hdr->format[3]);
        return -1;
    }

    // The handle stays open as long as possible, because the alsa lib has
    // some memory leaks within each snd_pcm_open(). If the type of file is
    // the same as from the previous open: we don't close, just reuse.
    int bytesPerSample = hdr->numChannels * hdr->bitsPerSample / 8;
    if (playbackHandle) {
        if (channels == hdr->numChannels && sampleRate == hdr->sampleRate) {
            return bytesPerSample;
        }
        snd_pcm_close(playbackHandle);
        playbackHandle = NULL;
    }

#ifndef DONT_USE_ALSA
    // Open the sound device
    int rc = snd_pcm_open (&playbackHandle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (rc < 0) {
        fprintf(stderr, "Unable to open pcm device: %s\n", snd_strerror(rc));
        playbackHandle = NULL;
        return -1;
    }

    rc = snd_pcm_set_params(playbackHandle,
                            hdr->bitsPerSample == 8 ? SND_PCM_FORMAT_U8 : SND_PCM_FORMAT_S16_LE,
                            SND_PCM_ACCESS_RW_INTERLEAVED,
                            hdr->numChannels, hdr->sampleRate, 1, 50000); // 0.05 s latency
    if (rc < 0) {
        fprintf(stderr, "snd_pcm_set_params() failed: %s\n", snd_strerror(rc));
        snd_pcm_close (playbackHandle);
        playbackHandle = NULL;
        return -1;
    }
#endif

    channels = hdr->numChannels;
    sampleRate = hdr->sampleRate;
    sampleSize = bytesPerSample;
    return bytesPerSample;
}

/**
 * Play the given file on the alsa steam, return false
 * if the file cannot be read. Another running playback
 * will be stopped immediately!
 */
void SoundPlayback::playSound()
{
    MutexLock guard(g_sound_mutex);

start:
    if (wavBuffer.data.get() == NULL || wavBuffer.size < 1024)
        return;

    //printf("SoundPlayback::playAgain(), this = %p, size = %d, handle = %p\n",
    //       this, wavBuffer.size, playbackHandle);

    // Skip the WAV header and some extra bytes;
    // one sample with 16Bit/channel == 4 bytes
    int remain = (wavBuffer.size-32) / sampleSize;
    const char *ptr = wavBuffer.data.get() + 32;
    done = false;

    // 41000 Hz / 256 => about 180 packets/loops per second here
    const int PAKET_SIZE = 256;
    while (remain > 0 && !finish && !utils::shouldFinish())
    {
        int todo = remain > PAKET_SIZE ? PAKET_SIZE : remain;

#ifndef DONT_USE_ALSA
        int err;
        if ((err = snd_pcm_writei (playbackHandle, ptr, todo)) != todo) {
            if (err < 0) {
                fprintf(stderr, "snd_pcm_writei() failed: %s\n", snd_strerror(err));
                break;
            }
            else
                fprintf(stderr, "Expected to write %d packets, wrote %d\n", todo, err);
        }
#endif

        {
            if (swapBuffer)
            {
                MutexLock lock(myMutex);
                wavBuffer  = newBuffer;
                ptr        = wavBuffer.data.get() + 32;
                remain     = (wavBuffer.size-32) / sampleSize;
                swapBuffer = false;
            }
            else {
                remain -= todo;
                ptr    += (todo*sampleSize);
            }
        }
    }

    if (swapBuffer)
    {
        MutexLock lock(myMutex);
        wavBuffer  = newBuffer;
        swapBuffer = false;
        goto start;
    }

    // we have do close the handle here: two playbacks which have a delay of
    // some seconds will fail otherwise: the ALSA handle will result in the
    // broken pipe after some seconds unused.
    // TODO: close async if there is not more input for 0.5 seconds.
    // TODO: rework the mechanism with 'swapBuffer'
    closeSoundHandle();
    done = true;
}

/**
 * Play the given file on the alsa stream, return false
 * if the file cannot be read. Another running playback
 * will be stopped immediately!
 */
bool SoundPlayback::playback(const char *filename, bool async)
{
    // check that the last playback has finished
    stopPlayback();

    if (!loadSound(filename))
        return false;

    if (openSoundHandle() <= 0) {
        clearBuffer();
        return false;
    }

    if (async) {
        stopThread();
        threadObject = this;
        playbackThread = new std::thread(triggerSound);
    }
    else {
        playSound();
    }
    return done;
}

/**
 * Playback a given buffer.
 */
bool SoundPlayback::playback(const WavBuffer &newBuffer, bool async)
{
    // if a playback is still active: try to swap the playback data
    // while the playback is running. This only works if the playback
    // has the same sample size and rate.
    if (async && !done)
    {
        WavHeader *hdr = (WavHeader*) newBuffer.data.get();
        if (sampleRate == hdr->sampleRate && sampleSize == hdr->numChannels * hdr->bitsPerSample / 8)
        {
            MutexLock lock(myMutex);
            this->newBuffer = newBuffer;
            this->swapBuffer = true;
            return true;
        }
    }

    // check that the last playback has finished
    stopPlayback();

    wavBuffer = newBuffer;
    if (openSoundHandle() <= 0) {
        clearBuffer();
        return false;
    }

    if (async) {
        stopThread();
        threadObject = this;
        playbackThread = new std::thread(triggerSound);
    }
    else {
        playSound();
    }
    return done;
}

/**
 * Release buffer and set to 0
 */
void SoundPlayback::clearBuffer() noexcept
{
    wavBuffer.data.reset();
    wavBuffer.size = -1;
}

/**
 * Close the sound handle and release resources.
 */
void SoundPlayback::closeSoundHandle() noexcept
{
    stopPlayback();

    if (playbackHandle) {
        // snd_pcm_drain() makes the system wait too long. We close the handle here
        // and might loose so much frames as defined as latency in snd_open.
        // snd_pcm_drain(playbackHandle);
        snd_pcm_close(playbackHandle);
        playbackHandle = NULL;
    }
    clearBuffer();
}

/**
 * Return true if a sound device is open.
 */
bool SoundPlayback::connected() const
{
    return playbackHandle != NULL;
}

/**
 * Stop playback as soon as possible.
 */
void SoundPlayback::stopPlayback()
{
    if (done)
        return;

    finish = true;
    for (int i = 0; !done && i < 20; ++i)
        utils::msleep(10);
    finish = false;
}

/**
 * Release as much memory as possible.
 */
void SoundPlayback::releaseAll()
{
    stopThread();

    if (instanceCount == 0) {
        snd_config_update_free_global();
    }
    else {
        Log::log("SoundPlayback::releaseAll: cannot release, still instances allocated:", instanceCount);
    }
}

} /* namespace brickapi */

#ifdef MAIN

using utils::SoundPlayback;

/**
 * To compile via commandline:
 * g++ -DMAIN -g -std=c++0x -Wall SoundPlayback.cpp -I.. -L../../lib/Debug -o sound -lutils -lasound
 */
int main(int argc, char **argv)
{
    if (argc <= 1) {
        puts("No args ...");
        return 1;
    }

    if (strcmp(argv[1], "X") == 0)
    {
        SoundPlayback player;

        SoundPlayback::WavBuffer wav = player.makeWav(500, 3000);
        player.playback(wav);
        utils::msleep(200);
        player.playback(wav);
        utils::msleep(200);

        wav = player.makeWav(60, 3000);
        player.playback(wav);
        utils::msleep(100);
        player.playback(wav);
        utils::msleep(100);
    }
    else
    {
        SoundPlayback player;
        for (int i = 1; i < argc; ++i)
            player.playback(argv[i]);
        return 1;
        sleep(2);

        puts("Now async");
        player.playback(argv[1], true);
        puts("Started...");
        sleep(4);
        puts("Destroy...");
    }
    SoundPlayback::releaseAll();
    return 0;
}

#endif
