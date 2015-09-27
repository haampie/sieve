LFLAGS = /home/teun/MulticoreBSP-for-C/lib/compat-libmcbsp1.2.0.a  -pthread -lm -std=c11
CFLAGS = -Wall -o psieve

all: psieve.c
	gcc psieve.c $(CFLAGS) $(LFLAGS)
