CC=gcc -lm

status: status.c
	${CC} status.c -o status

clean:
	rm status
