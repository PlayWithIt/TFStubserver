
all:  directories
	$(MAKE) -C src

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

allclean:
	rm -rf html bin/* obj/* lib/Debug lib/Release

new:  directories
	$(MAKE) -C src new

tests:

directories:	bin lib obj

bin lib obj:
	mkdir $@

# generator html doc via doxygen
.PHONY: html doc
html doc:
	doxygen
