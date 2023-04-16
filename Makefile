all: check

%:
	make -C test $@
