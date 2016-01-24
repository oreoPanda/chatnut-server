#Makefile

objects-debug = chatnut-server-debug.o getch-debug.o users-debug.o socketlayer-debug.o
objects = chatnut-server.o getch.o users.o socketlayer.o
cflags = -Wall -Wextra -std=c99
libs = -lm

chatnut-server: $(objects)
	echo "Linking "$(objects)
	gcc $(objects) -o $@ $(libs)

chatnut-server-debug: $(objects-debug)
	echo "Linking "$(objects-debug)
	gcc $(objects) -o $@ $(libs)

%.o: %.c
	echo "Compiling "$<
	gcc -c $< -o $@ $(cflags)

%-debug.o: %.c
	echo "Compiling "$<" with debugging information"
	gcc -c $< -o $@ $(cflags) -g

clean:
	echo "Removing executable"
	rm -f chatnut-server
	rm -f chatnut-server-debug
	echo "Removing objects"
	rm -f *.o
	echo "Removing temporary files created by gedit"
	rm -f *~
