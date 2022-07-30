#
# Copyright (C) 2013 Holger Grosenick
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

GCC:=$(shell test -x /usr/bin/g++-11 && echo g++-11)
ifeq "$(GCC)" ""
    GCC:=$(shell test -x /usr/bin/g++-10 && echo g++-10)
endif
ifeq "$(GCC)" ""
    GCC:=g++
endif

ifdef USE_LLVM
CC:= clang++
else
CC:= $(GCC)
endif

ifeq "$(CONFIG)" "Release"
  DEBUG := -g -O2
  CONFIG:= Release
else
  DEBUG := -g
  CONFIG:= Debug
endif

OBJ_DIR :=../../obj/$(CONFIG)
BIN_DIR :=../../bin/$(CONFIG)
LIB_DIR :=../../lib/$(CONFIG)

HDR := $(wildcard *.h)
SRC := $(wildcard *.cpp)
C_SRC := $(wildcard *.c)

# does not always exist ...
LIB_CPPUNIT=$(LIB_DIR)/libcppunit.so
