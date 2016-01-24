#Makefile

objects = chatnut-server.o getch.o users.o socketlayer.o
cflags = -Wall -Wextra -std=c99
libs = -lm

chatnut-server: $(objects)
	echo "Linking "$(objects)
	gcc $(objects) -o $@ $(libs)

%.o: %.c
	echo "Compiling "$<
	gcc -c $< -o $@ $(cflags)

clean:
	echo "Removing executable"
	rm -f chatnut-server
	echo "Removing objects"
	rm -f *.o
	echo "Removing temporary files created by gedit"
	rm -f *~
