
HAS_CPPUNIT:=$(wildcard /usr/include/cppunit/extensions/HelperM*.h)

all:  directories
	$(MAKE) -C src
ifndef NO_TESTS
ifneq "$(HAS_CPPUNIT)" ""
	$(MAKE) -C test/stubserver
else
	@echo "==========================================="
	@echo "     CPPUNIT not installed => no tests"
	@echo "==========================================="
endif
endif

help:
	@echo
	@echo "Make options:"
	@echo
	@echo "CONFIG   = Release | Debug"
	@echo "NO_TESTS = 1"
	@echo "USE_LLVM = 1"
	@echo

clean:
	$(MAKE) -C src clean
ifneq "$(HAS_CPPUNIT)" ""
	$(MAKE) -C test/stubserver clean
endif

allclean:
	rm -rf html bin/* obj/* lib/Debug lib/Release

new:  directories
	$(MAKE) -C src new
ifndef NO_TESTS
ifneq "$(HAS_CPPUNIT)" ""
	$(MAKE) -C test/stubserver new
endif
endif

tests:

directories:	bin lib obj

bin lib obj:
	mkdir $@

# generator html doc via doxygen
.PHONY: html doc
html doc:
	doxygen
