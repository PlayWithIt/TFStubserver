/*
 * SoundPlayback.h
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

#ifndef SOUNDPLAYBACK_H_
#define SOUNDPLAYBACK_H_

#include <memory>
#include <list>

#include "Mutexes.h"

// ALSA handle
struct _snd_pcm;

namespace utils {

/**
 * Class that enables an application to stream WAV files on a sound device using
 * the ASLA API.
 * <P>
 * You'll have to install 'libasound2-dev' on Debian to compile this or
 * 'alsa-devel' on OpenSuse.
 */
class SoundPlayback : public SyncedObject
{
public:
    SoundPlayback();
    virtual ~SoundPlayback() noexcept;

    /**
     * Used to store and allocated buffer for a wav-file.
     */
    struct WavBuffer
    {
        WavBuffer();
        /**
         * Allocate char* array with given size.
         */
        WavBuffer(int);
        ~WavBuffer();

        std::shared_ptr<char> data;
        int size;
    };



    /**
     * Return true if a sound device is open.
     */
    bool connected() const;

    /**
     * Play the given file on the alsa steam and wait until the playback
     * has finished. If 'async' the set to true, the function will
     * return after loading the file and play the sound in another thread.
     *
     * @param filename path + name of a file to load a WAV files from
     * @param async start the sound playback in a separate thread?
     * @return false if the file cannot be read, true on success
     */
    bool playback(const char *filename, bool async = false);

    /**
     * Play a sound from the given wav-buffer and wait until the playback
     * has finished. If 'async' the set to true, the function will
     * return after passing the sound to a playback thread.
     *
     * @param buffer wav source to playback
     * @param async start the sound playback in a separate thread?
     * @return false if the file cannot be read, true on success
     */
    bool playback(const WavBuffer &buffer, bool async = false);

    /**
     * Create a buffer which contains a WAV file with a constant frequency
     * and given duration. The result with have 2 channels and a sample rate
     * of 22000Hz.
     *
     * @param durationMs the duration of the tone in milliseconds
     * @param frequency the frequency to use for the sound
     * @return an object with a dynamically allocated buffer and the size of the buffer
     */
    WavBuffer makeWav(unsigned durationMs, unsigned frequency);

    /**
     * A single sound / tone with duration and frequency.
     */
    typedef std::pair<unsigned,unsigned> Sound;

    /**
     * A list of sounds.
     */
    typedef std::list<Sound> SoundList;

    /**
     * Create a buffer which contains a WAV file with a given sound list. Each
     * single sound from the list is constant.
     *
     * @param sounds - list of single tones / sounds to create a WAV part for
     * @return an object with a dynamically allocated buffer and the size of the buffer
     */
    WavBuffer makeWav(SoundList &sounds);

    /**
     * Stop playback as soon as possible.
     */
    void stopPlayback();

    /**
     * Release all sound resources: should be called at the end of a program
     * in order to release static ALSA data and not confuse 'valgrind'.
     */
    static void releaseAll();

private:
    bool finish;
    bool done;
    bool swapBuffer;
    int  channels;
    int  sampleRate;
    int  sampleSize;
    _snd_pcm *playbackHandle;

    WavBuffer wavBuffer;
    WavBuffer newBuffer;

    bool loadSound(const char *name);
    int  openSoundHandle() noexcept;
    void closeSoundHandle() noexcept;
    void clearBuffer() noexcept;

    static void triggerSound();

    /**
     * Plays that sound that was loading into memory the last time.
     */
    void playSound();

};

} /* namespace utils */
#endif /* SOUNDPLAYBACK_H_ */
