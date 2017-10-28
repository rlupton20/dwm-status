CC=gcc -lm

status: status.c config.h logging.h compile_checks.h
	${CC} status.c -o status

clean:
	rm status
