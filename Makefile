all: gen out.fuc

ET=../envytools

gen.h: in.fuc
	envyas -a -w -m fuc -V nva3 in.fuc -o gen.h

out.fuc: in.fuc
	envyas -w -m fuc -V nva3 in.fuc | envydis -w -m fuc -V nva3 -d 20 > out.fuc

gen: gen.c gen.h
	$(CC) gen.c -o gen -I $(ET)/include -lpciaccess -L $(ET)/nva -lnva -Wall -g -O

dead: gen
	sync
	./reclock
	sync
	./peek > core
	sync

