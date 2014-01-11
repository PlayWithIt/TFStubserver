/*
 * utils.h
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

#ifndef UTILS_H_
#define UTILS_H_

#include <string>

namespace utils {

// set SIGTERM + SIGINT signal handler which sets in internal flag so that 'doFinish()'
// will return true afterwards.
void initSignalHandlers() noexcept;

// return a flag that is true after a signal handler is called.
bool shouldFinish() noexcept;

// sleep 'ms' milliseconds
void msleep(int ms) noexcept;

// sleep 'us' microseconds
void usleep(int us) noexcept;

/**
 * Save a PID file:
 * returns 0 if OK, 1 on file write error, 2 if 'filename' is invalid
 */
int writePidFile(const char *name) noexcept;

/**
 * Convert an int into a base58 encoded string.
 */
const std::string base58Encode(unsigned int value);
unsigned int      base58Decode(const char *value);

}


#endif /* UTILS_H_ */
