/*
 * utils.h
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

#ifndef UTILS_H_
#define UTILS_H_

#include <string>

namespace utils {

// sleep 'ms' milliseconds
void msleep(int ms);

// sleep 'us' microseconds
void usleep(int us);

// returns the current process id, like "getpid()" on Linux
int getProcessId();

// convert long to char with 10 a base, use a 32 byte buffer which will be null terminated at the end
char*  ltoa(char dest[32], int64_t value);
char* ultoa(char dest[32], uint64_t value);
char*  itoa(char dest[16], int32_t value);
char* uitoa(char dest[16], uint32_t value);

/**
 * Similar to the standard snprintf, but with one difference:
 * if the buffer is too small, an exception is thrown.
 */
int snprintf(char *str, size_t size, const char *format, ...);

/**
 * Convert an int into a base58 encoded string.
 */
const std::string base58Encode(unsigned int value);
unsigned int      base58Decode(const char *value);

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
bool dumpAsShell(int argPos, int argc, char const* const* argv, char const* const* env);

/**
 * Open a log-file: redirect stdout + stderr into a file.
 * Do nothing if the log-name is NULL or empty.
 *
 * return true if OK, false in case of error
 */
bool redirectStdout(const char *logName);


/**
 * Convert boolean states (e.g. for buttons) into an unsigned with 1 bit set per boolean.
 */
unsigned bool2bits(bool states[], unsigned num);

/**
 * Converts bits of on an unsigned into an array of boolean, one value per bit
 */
void bits2bool(unsigned statesIn, bool statesOut[], unsigned num);

/**
 * Return the hexValue of a char:
 * 0 .. 9 a b c d e f => 0 .. 15
 *
 * If a non hex char was input: an exception is thrown !
 */
int hex2int(char c1);

/**
 * Return the hexValue of two hex chars: hexValue(c1)*16 + hexValue(c2)
 */
int hex2int(char c1, char c2);

/**
 * Return the hexValue of a string with hex sequence chars.
 *
 * Be aware:
 * the result is int64 => ffffffff is a positive value.
 */
int64_t hex2int(const char *in);
inline int64_t hex2int(const std::string &in) {
    return hex2int(in.c_str());
}

}


#endif /* UTILS_H_ */
