.PRECIOUS : %.o %.yy.c %.out


LEX=flex
YACC=bison
YFLAGS=-d -v -t
CC=gcc
LDLIBS=-lm -lfl -ly
CFLAGS=-std=c99 -g -pedantic -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes 

%.tab.c : %.y
	$(YACC) $(YFLAGS)  -o $@ $<

%.tab.o : %.tab.c
	$(CC) -c -o $@ $<

expr.o : expr.c 
	$(CC) -c -o $@ $<

machine.o : machine.c 
	$(CC) -c -o $@ $<

%.out : %.yy.o %.tab.o machine.o expr.o 
	$(CC)  $^ -o $@ $(LDLIBS)

%.yy.c : %.l %.tab.c
	$(LEX) -o $@ $<

%.yy.o : %.yy.c
	$(CC) -c -o $@ $< 

%.result: %.out %.input
	./$< < $(word 2, $^) > $@; cat $@


%:
    @:

.PHONY: clean
clean:
	rm -f *.o *.result *.out core
