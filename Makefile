all: gen out.fuc up down

ET=../envytools

in.fuc.h: in.fuc
	envyas -a -w -m fuc -V nva3 in.fuc -o in.fuc.h

out.fuc: in.fuc
	envyas -w -m fuc -V nva3 in.fuc | envydis -w -m fuc -V nva3 -d 20 > out.fuc

gen: run.c in.fuc.h init.h
	$(CC) run.c -DSCRIPT='"init.h"' -o gen -I $(ET)/include -lpciaccess -L $(ET)/nva -lnva -Wall -g -O

up: run.c in.fuc.h up.h
	$(CC) run.c -DSCRIPT='"up.h"' -o up -I $(ET)/include -lpciaccess -L $(ET)/nva -lnva -Wall -g -O

down: run.c in.fuc.h down.h
	$(CC) run.c -DSCRIPT='"down.h"' -o down -I $(ET)/include -lpciaccess -L $(ET)/nva -lnva -Wall -g -O

dead: gen
	sync
	./reclock
	sync
	./peek > core
	sync

