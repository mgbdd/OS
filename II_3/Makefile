CC = gcc
CFLAGS = -std=c17 -Wall -pthread -lrt

SRCS = draft.c
OBJS = $(SRCS:.c=.o)

draft: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) draft