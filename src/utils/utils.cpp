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
#include <stdarg.h>
#include <stdexcept>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#endif

#include <signal.h>
#include <string.h>
#include <atomic>
#include <filesystem>
#include <fstream>
#include <list>
#include <stdexcept>
#include <type_traits>

#include "Log.h"
#include "StringUtil.h"
#include "utils.h"


#define BASE58_MAX_STR_SIZE 13

static const char BASE58_ALPHABET[] = \
        "123456789abcdefghijkmnopqrstuvwxyzABCDEFGHJKLMNPQRSTUVWXYZ";

namespace fs = std::filesystem;

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


/**
 * Create a shell file which contains the current environment and the commandline
 * that was called.
 *
 * argPos - the position of the command  line option that was used to trigger
 *          dumping the command line, this argument and the next one are skipped
 *          in the dump (use -1 to skip nothing)
 * argc, argv, env - as passed to main()
 *
 * returns true if the dump was OK, false in case of file errors.
 */
bool dumpAsShell(int argPos, int argc, char const* const* argv, char const* const* env)
{
    const char *file = argv[argPos + 1];
    if (!file || !(*file))
        return false;

    // wrong arguments
    if (!argv)
        return false;

    // start writing the file
    std::ofstream os(file, std::ofstream::out);
    if (!os.is_open()) {
        Log::perror(file);
        return false;
    }

    // make the file readable + exec for the owner only, no other user
    fs::permissions(file, fs::perms::owner_all, fs::perm_options::replace);

    if (env) {
        // these variables should not be dumped, they are re-created in normal ssh shell
        std::list<std::string> excludes;
        excludes.push_back("_");
        excludes.push_back("HOME");         // user specific
        excludes.push_back("HOST");         // current host
        excludes.push_back("LESS");         // only for 'less'
        excludes.push_back("LS_");          // only for 'less'
        excludes.push_back("LOG_NAME");     // user specific
        excludes.push_back("SHLVL");        // shell specific
        excludes.push_back("SSH_");         // ssh, never overwrite !
        excludes.push_back("USER");         // user specific
        excludes.push_back("XTERM");        // user specific

        // sort the env list and remove vars which are mentioned in the exclude list
        const char *shell = nullptr;
        std::list<std::string> envList;

        for (int i = 0; env[i]; ++i) {
            bool skip = false;
            std::string e = env[i];

            // find the shell name
            if (strings::startsWith(e, "SHELL=")) {
                shell = strchr(env[i], '=');
                if (shell)
                    ++shell;
            }

            // check for excludes
            for (auto &s : excludes) {
                if (strings::startsWith(e, s)) {
                    skip = true;
                    break;
                }
            }
            if (!skip)
                envList.push_back(env[i]);
        }

        envList.sort();
        if (shell) {
            os << "#!" << shell << std::endl;

        }
        os << std::endl;
        for (auto e : envList) {
            os << "export " << e << std::endl;
        }
    }

    os << std::endl;
    for (int i = 0; i < argc && argv[i]; ++i) {
        if (i == argPos) {
            // skip the arg to dump the environment and call, so that if the shell
            // is started again, this should not dump again.
            ++i;
        }
        else {
            os << argv[i] << ' ';
        }
    }
    os << std::endl << std::endl;
    return true;
}

// returns the current process id like "getpid()" on Linux
int getProcessId()
{
    return getpid();
}

/**
 * Return the hexValue of a char:
 * 0 .. 9 a b c d e f => 0 .. 15
 *
 * If a non hex char was input: an exception is thrown !
 */
int hex2int(char c1)
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
int hex2int(char c1, char c2) {
    return hex2int(c1)*16 + hex2int(c2);
}

/**
 * Return the hexValue of a string with hex sequence chars
 */
int64_t hex2int(const char *in)
{
    if (!in)
        return 0;

    int64_t result = 0;
    while (*in) {
        result *= 16;
        result += hex2int(*in);
        ++in;
    }
    return result;
}

/**
 * Convert a numeric value with base 10 into a string.
 */
template<class T> char* convertToAscii(char *dest, int size, T value)
{
    if (value == 0) {
        dest[0] = '0';
        dest[1] = 0;
        return dest;
    }

    int i = 0;
    bool sign = false;
    char buffer[128];

    // only check if the type is signed
    if (std::is_signed<T>::value) {
        sign = value < 0;
    }

    // now convert
    while (value) {
        int mod = (value % 10);
        buffer[i] = '0' + (mod < 0 ? -mod : mod);
        ++i;
        value /= 10;

        if (i >= size) {
            char msg[256];
            sprintf(msg, "convertToAscii: value exceeds '%u' chars", size);
            throw std::overflow_error(msg);
        }
    }

    // copy back in reverse (correct order).
    int d = 0;
    if (sign) {
        // value was negative: start with minus
        dest[d++] = '-';
    }

    // copy back in correct order
    while (i > 0) {
        dest[d++] = buffer[--i];
    }
    dest[d] = 0;

    return dest;
}

/**
 * Integer to ascii
 */
char* itoa(char dest[16], int32_t value) {
    return convertToAscii(dest, 16, value);
}

char* uitoa(char dest[16], uint32_t value) {
    return convertToAscii(dest, 16, value);
}

char* ltoa(char dest[32], int64_t value) {
    return convertToAscii(dest, 32, value);
}

char* ultoa(char dest[32], uint64_t value) {
    return convertToAscii(dest, 32, value);
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

/**
 * Similar to the standard snprintf, but with one difference:
 * if the buffer is too small, an exception is thrown.
 */
int snprintf(char *str, size_t size, const char  *format, ...)
{
    str[size-1] = 0;

    va_list args;
    va_start(args, format);
    size_t res = ::vsnprintf(str, size, format, args);
    va_end(args);

    if (str[size-1] != 0 || res >= size) {
        char value1[32];
        char value2[32];
        ltoa(value1, size);
        ltoa(value2, res);
        throw std::length_error(std::string("utils::snprintf overflow, size=") + value1 + " result=" + value2);
    }

    return res;
}

}
