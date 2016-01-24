#Makefile

all:
	cd src && make --quiet chatnut-server

default:
	cd src && make --quiet chatnut-server

debug:
	cd src && make --quiet chatnut-server-debug

clean:
	rm -f *~
	rm -f .*~
	cd src && make --quiet clean
