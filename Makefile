all: gen out.fuc up down

ET=../envytools

CFLAGS= -I$(ET)/include -Wall -g -O
LDFLAGS= -L$(ET)/nva -lnva -lpciaccess
ED2AS=$(ET)/envydis/envyas -w -m fuc -V nva3
ED2DIS=$(ET)/envydis/envydis -w -m fuc -V nva3

in.fuc.h: in.fuc
	$(ED2AS) -a in.fuc -o in.fuc.h

out.fuc: in.fuc
	$(ED2AS) in.fuc | $(ED2DIS) -d 24 > out.fuc

gen: run.c in.fuc.h init.h
	$(CC) run.c -DSCRIPT='"init.h"' -o gen $(CFLAGS) $(LDFLAGS)

up: run.c in.fuc.h up.h
	$(CC) run.c -DSCRIPT='"up.h"' -o up $(CFLAGS) $(LDFLAGS)

down: run.c in.fuc.h down.h
	$(CC) run.c -DSCRIPT='"down.h"' -o down $(CFLAGS) $(LDFLAGS)

dead: gen
	sync
	./reclock
	sync
	./peek > core
	sync

