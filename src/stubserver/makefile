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

include ../../settings.mak

EXE := $(BIN_DIR)/stubserver
LIB := $(LIB_DIR)/libstubserver.a

ifeq "$(DONT_USE_ALSA)" ""
SOUND:=-lasound
endif

HDR_DEPS := ../utils/*.h

CC_USER_FLAGS  := -I../bindings -I../brickapi
NO_COMPILE_CPP := main.cpp

all:	$(LIB) $(EXE) $(EXE2)

$(EXE): main.cpp $(LIB) $(LIB_DIR)/*.a
	$(CC) $(DEBUG) $< $(CCFLAGS) -o $@ $(OBJECTS) -L$(LIB_DIR) -lstubserver -lutils $(SOUND) -lpthread

include ../../rules.mak
