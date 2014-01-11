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

#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>

#include "utils.h"


#define BASE58_MAX_STR_SIZE 13

static const char BASE58_ALPHABET[] = \
        "123456789abcdefghijkmnopqrstuvwxyzABCDEFGHJKLMNPQRSTUVWXYZ";


namespace utils {

static bool finish = false;



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
    uint32_t value = 0;
    uint32_t base = 1;

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

        value += k * base;
        base *= 58;
    }

    return value;
}


// sleep some milliseconds
void msleep(int ms) noexcept
{
    // This doesn't work yet on all systems: _GLIBCXX_USE_NANOSLEEP is missing
    // std::this_thread::sleep_for(std::chrono::milliseconds(ms));

    usleep(ms*1000);
}

// sleep some microseconds
void usleep(int us) noexcept
{
    ::usleep(us);
}


// signal handler to terminate the loop
static void sigTermHandler(int sig, siginfo_t *, void *)
{
    finish = true;
}

/**
 * Should the application finish?
 */
bool shouldFinish() noexcept
{
    return finish;
}

/**
 * Install the signal handler to terminate the process.
 */
void initSignalHandlers() noexcept
{
    struct sigaction act;
    memset(&act, 0, sizeof(act));

    act.sa_sigaction = sigTermHandler;
    act.sa_flags     = SA_SIGINFO;
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGINT,  &act, NULL);
}

/**
 * Save a PID file:
 * returns 0 if OK, 1 on file write error, 2 if 'filename' is invalid
 */
int writePidFile(const char *filename) noexcept
{
    if (filename && *filename)
    {
        FILE *f = fopen(filename, "w");
        if (f == NULL) {
            perror(filename);
            return 1;
        }
        fprintf(f, "%d", getpid());
        fclose(f);
        return 0;
    }
    return 2;
}

}
