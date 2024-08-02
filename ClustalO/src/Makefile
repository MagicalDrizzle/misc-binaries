all: bali_score bali_score_block bali_score_reliable

clean:
	rm bali_score bali_score_block bali_score_reliable

HEADERS = general.h clustalw.h

CC	= gcc
CFLAGS  = -O2 -s -flto=auto
LFLAGS	= -lm -lexpat

bali_score :
	$(CC) $(CFLAGS) -o bali_score readseq.c readxml.c init.c util.c bali_score.c $(LFLAGS)

bali_score_block :
	$(CC) $(CFLAGS) -o bali_score_block readseq.c readxml.c init.c util.c bali_score_block.c $(LFLAGS)

bali_score_reliable :
	$(CC) $(CFLAGS) -o bali_score_reliable readseq.c readxml.c init.c util.c bali_score_reliable.c $(LFLAGS)
