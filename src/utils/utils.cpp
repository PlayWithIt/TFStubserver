/*
 * utils.cpp
 *
 * Copyright (C) 2013-2022 Holger Grosenick
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
#include <fcntl.h>
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
 * Convert button states for 13 buttons into an unsigned with 3 bits set.
 */
unsigned bool2bits(bool states[], unsigned num)
{
    unsigned newStates = 0;

    for (unsigned i = 0; i < num; ++i)
    {
        if (states[i]) {
            unsigned mask = (1 << i);
            newStates |= mask;
        }
    }

    return newStates;
}

/**
 * Convert an unsigned with 13 bits set into button states for 13 buttons
 */
void bits2bool(unsigned statesIn, bool statesOut[], unsigned num)
{
    for (unsigned i = 0; i < num; ++i)
    {
        unsigned mask = (1 << i);
        statesOut[i] = (statesIn & mask);
    }
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


// returns the current process id like "getpid()" on Linux
uint64_t getProcessId()
{
    return getpid();
}

/**
 * Return the hexValue of a char:
 * 0 .. 9 a b c d e f => 0 .. 15
 *
 * If a non hex char was input: an exception is thrown !
 */
int hexValue(char c1)
{
    if (c1 >= '0' && c1 <= '9')
        return c1 - '0';
    if (c1 >= 'A' && c1 <= 'F')
        return (c1 - 'A') + 10;
    if (c1 >= 'a' && c1 <= 'f')
        return (c1 - 'a') + 10;

    char msg[128];
    sprintf(msg, "hexValue: '%c' (%d) is not a valid hex char", c1, c1);
    throw std::invalid_argument(msg);
}

/**
 * Return the hexValue of two hex chars: hexValue(c1)*16 + hexValue(c2)
 */
int hexValue(char c1, char c2) {
    return hexValue(c1)*16 + hexValue(c2);
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

/**
 * Open a log-file: redirect stdout + stderr into a file.
 * Do nothing if the log-name is NULL or empty.
 *
 * return true if OK, false in case of error
 */
bool redirectStdout(const char *logName)
{
    if (!logName)
        return false;

    // empty filename or beginning with ' ' is not allowed
    if (*logName == 0 || *logName == ' ')
        return false;

    int fd = open(logName, O_CREAT | O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP);
    if (fd < 0) {
        fprintf(stderr, "Error creating %s: %s\n", logName, strerror(errno));
        return false;
    }

    bool result = true;
    if (dup2(fd, STDOUT_FILENO) != STDOUT_FILENO) {
        fprintf(stderr, "Could not redirect stdout: %s\n", strerror(errno));
        result = false;
    }
    if (dup2(fd, STDERR_FILENO) != STDERR_FILENO) {
        fprintf(stderr, "Could not redirect stderr: %s\n", strerror(errno));
        result = false;
    }
    return result;
}

}
