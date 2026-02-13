CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS_PTHREAD = -pthread   # oppure: -lpthread

all: broker publisher subscriber

broker: broker.c pubsub.h
	$(CC) $(CFLAGS) -o broker broker.c $(LDFLAGS_PTHREAD)

publisher: publisher.c pubsub.h
	$(CC) $(CFLAGS) -o publisher publisher.c

subscriber: subscriber.c pubsub.h
	$(CC) $(CFLAGS) -o subscriber subscriber.c

clean: objclean ipcclean procclean

objclean:
	rm -f broker publisher subscriber
	rm -f *.o

ipcclean:
	ipcs -q | awk '($$2~/^[0-9]+$$/) { system("ipcrm -q " $$2) }'

procclean:
	ps aux | grep -v grep | grep -E "./broker|./subscriber|./publisher" | awk '{ system("kill -9 " $$2) }'

start: all
	./broker &
	./subscriber &
	sleep 1 && ./publisher
