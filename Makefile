CC = gcc
CFLAGS = -Wall -pthread
LDLIBS = -lssl -lcrypto

OBJS = main.o chat.o network.o

all: chat

chat: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -f *.o chat
