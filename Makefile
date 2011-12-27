#

all: gen

ET=../envytools

gen.h:
	envyas -a -w -m fuc -V nva3 in.fuc -o gen.h

gen: gen.c gen.h
	$(CC) gen.c -o gen -I $(ET)/include -lpciaccess -L $(ET)/nva -lnva -Wall -g -O

dead: gen
	sync
	./reclock < values
	sync
	./peek > core
	sync

