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

CCFLAGS += $(DEBUG) -std=c++0x -Wall -Werror -I. -I../../src $(CC_USER_FLAGS)

ifneq "$(NO_COMPILE_CPP)" ""
SRC := $(filter-out $(NO_COMPILE_CPP),$(SRC))
endif

OBJ   := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))
C_OBJ := $(patsubst %.c,$(OBJ_DIR)/%.o,$(C_SRC))
EXTRA_OBJ=$(patsubst %.cpp,$(OBJ_DIR)/%.o,$(NO_COMPILE_CPP))

print:
	@printf "===========================================\n    Using config: $(CONFIG) $(CC) $(CC_VERS)\n===========================================\n"

status:
	@echo "HDR:   $(HDR)"
	@echo "SRC:   $(SRC)"
	@echo "OBJ:   $(OBJ)"
	@echo "C_SRC: $(C_SRC)"
	@echo "C_OBJ: $(C_OBJ)"
	@echo "OBJ_DIR: $(OBJ_DIR)"

$(OBJ_DIR) $(BIN_DIR) $(LIB_DIR):
	mkdir $@

objects:   print $(OBJ_DIR) $(BIN_DIR) $(OBJ) $(C_OBJ)

$(OBJ_DIR)/%.o:	%.cpp $(HDR) $(HDR_DEPS)
	$(CC) -c $(CCFLAGS) $(*F).cpp -o $@

# Rule for bindings which have the extension '.c'
$(OBJ_DIR)/%.o:	%.c *.h
	$(CC) -c $(CCFLAGS) $(*F).c -o $@

# optionally build a lib
ifneq "$(LIB)" ""
$(LIB): $(LIB_DIR) objects
	ar rc $@ $(OBJ) $(C_OBJ)
	ranlib $@
endif

# copy pre-compiled cppunit lib
$(LIB_CPPUNIT):
	cp -a $(LIB_DIR)/../libcppunit* $(LIB_DIR)

clean:
	rm -f $(OBJ) $(EXTRA_OBJ) $(C_OBJ) $(LIB) $(EXE) $(EXTRA) $(TO_REMOVE)

new:
	make clean
	make -j 4 objects
	make all
