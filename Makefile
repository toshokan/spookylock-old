spookylock: 
	gcc -o spookylock spookylock.c auth.c -lpam -lncurses
	sudo chown -R root:root spookylock
	chmod +s spookylock

clean:
	rm spookylock 
