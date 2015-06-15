/*
 * Compatability.h
 *
 * Copyright (C) 2015 Holger Grosenick
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

#ifndef UTILS_COMPATABILITY_H_
#define UTILS_COMPATABILITY_H_

/**
 * Some basic defines to handle differences between Windows (Visual Studio 2013) and Linux.
 */

#ifdef _WIN32
#define NOEXCEPT
#include <stdint.h>
#else
#define NOEXCEPT noexcept
#endif

#endif /* UTILS_COMPATABILITY_H_ */
