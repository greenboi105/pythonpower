CC=cc
CFLAGS=-g -Wall -Werror -std=c99
TARGETS=power-in-python

all: $(TARGETS)

#Static Pattern Rules
$(TARGETS): % : %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf *.o *~ $(TARGETS)
