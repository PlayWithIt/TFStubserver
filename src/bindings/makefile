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

LIB  := $(LIB_DIR)/libtfbindings.a
EXTRA:= device_identifiers.h device_table.h

# source that will be removed after an update (not used / supported)
TO_REMOVE:=bricklet_piezo_buzzer.*


all:	$(EXTRA) $(LIB)

extra:  $(EXTRA)

device_identifiers.h:   brick*.h ip*.h
	echo "// generated file - do not touch !!"  > $@
	echo "#ifndef DEVICE_IDENTIFIERS_H" >> $@
	echo "#define DEVICE_IDENTIFIERS_H" >> $@
	echo                                >> $@
	grep -hw define brick*.h | grep DEVICE_IDENTIFIER | sort -n --key=3 >> $@
	echo          >> $@
	echo "#endif" >> $@

device_table.h:   device_identifiers.h
	echo "// generated file - do not touch !!"  > $@
	echo "#ifndef DEVICE_TABLE_H"      >> $@
	echo "#define DEVICE_TABLE_H"      >> $@
	echo                               >> $@
	echo "#include \"device_identifiers.h\"" >> $@
	echo                               >> $@
	echo "static const struct {"       >> $@
	echo "  const char *name;"         >> $@
	echo "  int deviceIdentifier;"     >> $@
	echo "} gAllDeviceIdentifiers[] =" >> $@
	echo "  {"                         >> $@
	for i in `grep -hw define brick*.h | grep "DEVICE_IDENTIFIER " | sed "s/#define //" | sed "s/_DEVICE_IDENTIFIER [0-9]*//" | sort -u`; do printf '  {"%s", %s_DEVICE_IDENTIFIER },\n' $$i $$i >> $@; done
	echo "  { 0, -1 } };" >> $@
	echo          >> $@
	echo "#endif" >> $@

include ../../rules.mak
