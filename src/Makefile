#Makefile

objects-debug = Action-debug.o Buddy-debug.o chatnut-server-debug.o Client-debug.o Command-debug.o\
				Getch-debug.o helpers-debug.o Horst-debug.o LogWriter-debug.o Message-debug.o Storage-debug.o Userlist-debug.o
objects = Action.o Buddy.o chatnut-server.o Client.o Command.o Getch.o helpers.o Horst.o LogWriter.o Message.o Storage.o Userlist.o
compiler = g++
cflags = -Wall -Wextra -std=c++0x

chatnut-server: $(objects)
	echo "Linking "$(objects)
	$(compiler) $(objects) -o $@ $(libs)

chatnut-server-debug: $(objects-debug)
	echo "Linking "$(objects-debug)
	$(compiler) $(objects-debug) -o $@ $(libs)

%.o: %.cpp
	echo "Compiling "$<
	$(compiler) -c $< -o $@ $(cflags)

%-debug.o: %.cpp
	echo "Compiling "$<" with debugging information"
	$(compiler) -c $< -o $@ $(cflags) -g

install:
	echo "Installing chatnut-server"
	cp chatnut-server /usr/local/bin

uninstall:
	echo "Uninstalling chatnut-server"
	rm /usr/local/bin/chatnut-server

clean:
	echo "Removing executable"
	rm -f chatnut-server
	rm -f chatnut-server-debug
	echo "Removing objects"
	rm -f *.o
	echo "Removing temporary files created by gedit"
	rm -f *~
