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

LIB := $(LIB_DIR)/libutils.a

ifneq "$(DONT_USE_ALSA)" ""
CC_USER_FLAGS += -DDONT_USE_ALSA=1
endif
ifneq "$(HAS_NEW_BFD)" ""
CC_USER_FLAGS += -DHAS_NEW_BFD=1
endif

all:	$(LIB)

include ../../rules.mak
