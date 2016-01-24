#Makefile

all:
	cd src && make --quiet chatnut-server

default:
	cd src && make --quiet chatnut-server

clean:
	rm -f *~
	rm -f .*~
	cd src && make --quiet clean
