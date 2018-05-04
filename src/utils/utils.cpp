/*
 * utils.cpp
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

#ifdef _WIN32
#include <stdint.h>
#include <Windows.h>
#else
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#endif

#include <signal.h>
#include <string.h>
#include <atomic>
#include <stdexcept>

#include "Log.h"
#include "utils.h"


#define BASE58_MAX_STR_SIZE 13

static const char BASE58_ALPHABET[] = \
        "123456789abcdefghijkmnopqrstuvwxyzABCDEFGHJKLMNPQRSTUVWXYZ";


namespace utils {

static std::atomic_bool finish(false);



/**
 * Convert an int into a base58 encoded string.
 */
const std::string base58Encode(unsigned int value)
{
    uint32_t mod;
    char reverse_str[BASE58_MAX_STR_SIZE] = {'\0'};
    char str[BASE58_MAX_STR_SIZE] = {'\0'};
    int i = 0;
    int k = 0;

    while (value >= 58) {
        mod = value % 58;
        reverse_str[i] = BASE58_ALPHABET[mod];
        value = value / 58;
        ++i;
    }

    reverse_str[i] = BASE58_ALPHABET[value];
    for (k = 0; k <= i; k++) {
        str[k] = reverse_str[i - k];
    }

    for (; k < BASE58_MAX_STR_SIZE; k++) {
        str[k] = '\0';
    }
    return str;
}


/**
 * Read an int out of as base58 encoded string.
 */
unsigned int base58Decode(const char *str)
{
    int i;
    int k;
    uint64_t value = 0;
    uint64_t base = 1;

    for (i = 0; i < BASE58_MAX_STR_SIZE; i++) {
        if (str[i] == '\0') {
            break;
        }
    }

    --i;

    for (; i >= 0; i--) {
        if (str[i] == '\0') {
            continue;
        }

        for (k = 0; k < 58; k++) {
            if (BASE58_ALPHABET[k] == str[i]) {
                break;
            }
        }

        uint64_t oldValue = value;
        value += k * base;
        base *= 58;

        // invalid ID?
        if (value < oldValue || value > 0xffffffff)
        {
            char msg[256];
            sprintf(msg, "base58Decode: overflow in string '%s'", str);
            throw std::overflow_error(msg);
        }
    }

    return value;
}


// sleep some milliseconds
void msleep(int ms)
{
    // This doesn't work yet on all systems: _GLIBCXX_USE_NANOSLEEP is missing
    // std::this_thread::sleep_for(std::chrono::milliseconds(ms));
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms*1000);
#endif
}

// sleep some microseconds
void usleep(int us)
{
#ifdef _WIN32
    // TODO
    Sleep(us < 1000 ? 1 : us / 1000);
#else
    ::usleep(us);
#endif
}

#ifndef _WIN32
// signal handler to terminate the loop
static void sigTermHandler(int sig, siginfo_t *, void *)
{
    static unsigned count = 0;

    if (++count == 5) {
        Log() << "sigTermHandler(" << sig << ") called 5 times => hard exit ...";
        exit(16);
    }

    Log() << "sigTermHandler(" << sig << ") called (" << count << ") => finish ...";
    finish = true;
}

static void sigKillHandler(int sig, siginfo_t *, void *)
{
    char buffer[128];
    const char *sigName;
    switch(sig) {
    case SIGSEGV:
        sigName = "SIGSEGV";
        break;
    case SIGPIPE:
        sigName = "SIGPIPE";
        break;
    case SIGILL:
        sigName = "SIGILL";
        break;
    case SIGBUS:
        sigName = "SIGBUS";
        break;
    default:
        sigName = "???";
    }
    sprintf(buffer, "CAUGHT CRITICAL SIGNAL: signal=%d (%s) => abort!", sig, sigName);
    Log::error(buffer);
    exit(4);
}
#endif

/**
 * Should the application finish?
 */
bool shouldFinish()
{
    return finish;
}

// set signal handler for example for SIGUSR1 or similar
void setSignalHandler(int signalNo, void (*function)(int, void *sigInfo, void *u_context), void* extraData)
{
#ifndef _WIN32
    struct sigaction act;
    memset(&act, 0, sizeof(act));

    act.sa_sigaction = (void (*)(int, siginfo_t *sigInfo, void *u_context))function;
    act.sa_flags     = SA_SIGINFO;
    sigaction(signalNo, &act, NULL);
#endif
}

/**
 * Install the signal handler to terminate the process.
 */
void initSignalHandlers()
{
#ifndef _WIN32
    struct sigaction act;
    memset(&act, 0, sizeof(act));

    act.sa_sigaction = sigTermHandler;
    act.sa_flags     = SA_SIGINFO;
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGINT,  &act, NULL);

    act.sa_sigaction = sigKillHandler;
    sigaction(SIGSEGV, &act, NULL);
    sigaction(SIGPIPE, &act, NULL);
    sigaction(SIGBUS,  &act, NULL);
    sigaction(SIGILL,  &act, NULL);

    struct rlimit rlim;
    getrlimit(RLIMIT_CORE, &rlim);
    if (rlim.rlim_cur == 0) {
        rlim.rlim_cur = 300000 * 1024;
        if (setrlimit(RLIMIT_CORE, &rlim) != 0)
            Log::perror("Failed to set RLIMIT_CORE");
    }
#endif
}

}
