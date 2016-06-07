#Makefile

all:
	cd src && make --quiet chatnut-server

default:
	cd src && make --quiet chatnut-server

debug:
	cd src && make --quiet chatnut-server-debug

install:
	cd src && make --quiet install

uninstall:
	cd src && make --quiet uninstall

clean:
	rm -f *~
	rm -f .*~
	cd src && make --quiet clean
