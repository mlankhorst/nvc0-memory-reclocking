#

all: out.asm

gen: gen.c
	$(CC) gen.c -o gen

in.asm: gen
	./gen > in.asm

values: in.asm
	envyas -w -m fuc -V nva3 in.asm > values

out.asm: values
	fucdis -n -w -V nva3 values > out.asm

