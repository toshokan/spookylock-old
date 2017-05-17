spookylock: 
	gcc -o spookylock spookylock.c auth.c -lpam -lncurses

clean:
	rm spookylock 
