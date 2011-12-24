#include <stdio.h>
#include <unistd.h>
#include <assert.h>

#define DISABLE_LOCK 0
#define DEBUG_C 0

static unsigned data[] = {
0xd0021,
0x132100,
0x1,
0x10fe20,
0x20000000,
0x137320,
0x3,
0x137330,
0x81200006,
0x10fe24,
0x1160f,
0x10fe20,
0x20000001,
0x20000,
0x20000,
0x20001,
0x137390,
0x30015,
0x20000,
0xfa00,
0x70021,
0x10fe20,
0x20000005,
0x132004,
0x11c09,
0x132000,
0x18000101,
0x20000,
0x2,
0x20001,
0x137390,
0x30015,
0x2,
0xfa00,
0xf0021,
0x10fb04,
0x55550000,
0x10fb08,
0x55550000,
0x10f988,
0x2004ff00,
0x10f98c,
0x3fc040,
0x10f990,
0x20012001,
0x10f998,
0x11a00,
0x13d974,
0,
0x20013,
0x174,
0x30021,
0x100b0c,
0x80012,
0x20034,
0xa,
0x30020,
0x1,
0,
0x50021,
0x10f330,
0x100014,
0xe108,
0x4007064d,
0x2002e,
0x4e20,
0x70021,
0x10f200,
0x8f8000,
0x10f808,
0x4aa20050,
0x10f210,
0,
0x2002e,
0x3e8,
0x30021,
0x10f310,
0x1,
0x2002e,
0x3e8,
0x50021,
0x10f090,
0x61,
0x10f090,
0xc000007f,
0x2002e,
0x3e8,
0x70021,
0x10f824,
0x7fd4,
0x10f800,
0x1800,
0x13d974,
0,
0x20013,
0x22e,
0xb0021,
0x1373ec,
0,
0x1373f0,
0x3,
0x10f830,
0x40700010,
0x10f830,
0x40500010,
0x13d974,
0,
0x20013,
0x174,
0x110021,
0x1373f8,
0,
0x132100,
0x101,
0x137310,
0x89201616,
0x10f050,
0xff000090,
0x1373ec,
0x30000,
0x1373f0,
0x2,
0x132100,
0x1,
0x1373f8,
0x2000,
0x2002e,
0x7d0,
0x90021,
0x10f808,
0x4aaa0050,
0x10f830,
0x500010,
0x10f200,
0x8f0000,
0x10f090,
0x4000007e,
0x2002e,
0x7d0,
0x90021,
0x10f314,
0x1,
0x10f210,
0x80000000,
0x10f338,
0x300100,
0x10f300,
0xa4d,
0x2002e,
0x3e8,
0x170021,
0x10f290,
0xc3a3428,
0x10f294,
0x3493828e,
0x10f298,
0x440e0800,
0x10f29c,
0x214c,
0x10f2a0,
0x42e20069,
0x10f604,
0xf1000000,
0x10f614,
0x60044e77,
0x10f610,
0x60044e77,
0x10f808,
0x7aaa0050,
0x10f870,
0x99999999,
0x10f344,
0x600008,
0x2002e,
0x3e8,
0x50021,
0x10f348,
0x700000,
0x13d974,
0,
0x20013,
0x5b,
0xd0021,
0x61c140,
0x9a40000,
0x10f644,
0x44444444,
0x10f640,
0x11111111,
0x10f910,
0x800e1008,
0x10f914,
0x800e1008,
0x13d974,
0,
0x20013,
0x1a,
0x20001,
0x110974,
0x30015,
0xf,
0x7a120,
0x20001,
0x111974,
0x30015,
0xf,
0x7a120,
0x20001,
0x112974,
0x30015,
0xf,
0x7a120,
0x20001,
0x113974,
0x30015,
0xf,
0x7a120,
0x20001,
0x114974,
0x30015,
0xf,
0x7a120,
0x20001,
0x115974,
0x30015,
0xf,
0x7a120,
0x2002e,
0x3e8,
0x30021,
0x10f800,
0x1804,
0x30020,
0,
0,
0x20034,
0xb,
0x30021,
0x13d974,
0,
0x20013,
0xd,
0x30021,
0x611200,
0x3330,
0x2002e,
0x186a0,
0xd0021,
0x10f9b0,
0x5313f41,
0x10f9b4,
0x2f50,
0x10f910,
0x10c1001,
0x10f914,
0x10c1001,
0x100b0c,
0x80028,
0x10f200,
0x8f0800,
0x20016,
0
};

static char *method = "mmwrs";

static unsigned unk3ec[3];

#define MMSYNC(x) \
"   mov $r9 0\n" \
"   mmloop_" x ":\n" \
"      push $r9\n" \
"      pop $r9\n" \
"      movw $r15 0xeb00\n" \
"      sethi $r15 0x10000\n" \
"      iord $r15 I[$r15]\n" \
"      add b32 $r9 $r9 1\n" \
"      extr $r15 $r15 12:14\n" \
"      bra nz #mmloop_" x "\n" \

#define MMWR \
"   movw $r15 0xe800\n" \
"   sethi $r15 0x10000\n" \
"   iowr I[$r15] $r10\n" \
"   add b32 $r15 0x100\n" \
"   iowr I[$r15] $r11\n" \
"   add b32 $r15 0x200\n" \
"   mov $r11 0xf2\n" \
"   sethi $r11 0x10000\n" \
"   iowr I[$r15] $r11\n" \
"   iord $r11 I[$r15]\n"

static char pre[] =
"section #data\n"
"section #code\n"
"init:\n"
"   bclr $flags ie0\n"
"   bclr $flags ie1\n"
"   mov $r3 0x5600\n"
"   mov $sp $r3\n"
"   movw $r0 -1\n"
"   st b32 D[$r3] $r0\n"
"   add b32 $r3 $r3 4\n"
"   bra #main\n"
"\n"
"mmsync:\n"
MMSYNC("")
"   ret\n\n"

#undef MMSYNC
#define MMSYNC(x) "   call #mmsync\n"

// Cannot use registers 10..13 in mmsync because of them being function arguments
// The pushes are mostly for debugging, putting the most recent mmrd/wr on the stack
"mmwrs:\n" // mmwrs(register, value)
"   push $r10\n"
MMSYNC("wrsi")
MMWR
MMSYNC("wrso")
"   pop $r10\n"
"   ret\n"
"\n"
"mmwr:\n" // mmwr(register, value)
"   push $r10\n"
MMSYNC("wri")
MMWR
"   pop $r10\n"
"   ret\n"
"\n"
"mmrd:\n" // mmrd(register)
"   push $r10\n"
MMSYNC("rdi")
"   movw $r15 0xe800\n"
"   sethi $r15 0x10000\n"
"   iowr I[$r15] $r10\n"
"   add b32 $r15 0x300\n"
"   mov $r11 0xf1\n"
"   sethi $r11 0x10000\n"
"   iowr I[$r15] $r11\n"
"   iord $r11 I[$r15]\n"
MMSYNC("rdo")
"   movw $r15 0xe900\n"
"   sethi $r15 0x10000\n"
"   pop $r10\n"
"   iord $r10 I[$r15]\n"
"   ret\n"
"\n"
"wait_mask:\n" // mmrd(register, mask, expected, timeout)
"   push $r8\n"
"   push $r7\n"
"   push $r6\n"
"   push $r5\n"
"   push $r4\n"
"   mov b32 $r6 $r10\n" // register
"   mov b32 $r7 $r11\n" // mask
"   mov b32 $r8 $r12\n" // expected
"   mov b32 $r5 $r13\n" // timeout
"   movw $r9 0xb00\n"
"   iord $r4 I[$r9]\n" // start time
"   repeat:\n"
"      mov b32 $r10 $r6\n"
"      call #mmrd\n"
"      and $r12 $r10 $r7\n"
"      mov $r10 0x1111\n" // return 0x1111 if succesful
"      cmp b32 $r12 $r8\n" // value & mask == expected
"      bra e #success\n"
"      movw $r9 0xb00\n"
"      iord $r10 I[$r9]\n" // start time
"      sub b32 $r10 $r10 $r4\n" // now - start < timeout? repeat
"      cmp b32 $r10 $r5\n"
"      bra l #repeat\n"
"   mov $r10 0x999\n" // nein nein nein (failed)
"   success:\n"
"   pop $r4\n"
"   pop $r5\n"
"   pop $r6\n"
"   pop $r7\n"
"   pop $r8\n"
"   ret\n"
"\n"
"sleep:\n"
"   movw $r11 0xb00\n"
"   iord $r15 I[$r11]\n"
"   add b32 $r15 $r10\n"
"   sleeploop:\n"
"      push $r12\n"
"      pop $r12\n"
"      iord $r10 I[$r11]\n"
"      sub b32 $r12 $r10 $r15\n"
"      bra l #sleeploop\n"
"   ret\n"
"\n"
"enter_lock:\n"
"   push $r0\n"
"   mov $r10 0x1620\n"
"   call #mmrd\n"
"   mov b32 $r0 $r10\n"
"   movw $r11 -0xaa3\n"
"   mov $r10 0x1620\n"
"   and $r0 $r0 $r11\n"
"   mov b32 $r11 $r0\n"
"   call #mmwrs\n"
"   bclr $r0 0\n"
"   mov b32 $r11 $r0\n"
"   mov $r10 0x1620\n"
"   call #mmwrs\n"
"\n"
"   mov $r10 0x26f0\n"
"   call #mmrd\n"
"   mov b32 $r11 $r10\n"
"   bclr $r11 0\n"
"   mov $r10 0x26f0\n"
"   call #mmwrs\n"
"   movw $r10 0xf800\n"
"   sethi $r10 0x10000\n"
"\n"
"   mov $r11 4\n"
"   iowr I[$r10] $r11\n"
"   iord $r11 I[$r10]\n"
"   sub b32 $r10 $r10 0x700\n"
"   enterloop:\n"
"      iord $r11 I[$r10]\n"
"      and $r11 4\n"
"      bra z #enterloop\n"
"   pop $r0\n"
"   ret\n"
"\n"
"leave_lock:\n"
"   push $r0\n"
"   movw $r10 0xf900\n"
"   sethi $r10 0x10000\n"
"   mov $r11 4\n"
"   iowr I[$r10] $r11\n"
"   iord $r11 I[$r10]\n"
"   sub b32 $r10 $r10 0x800\n"
"   leaveloop:\n"
"      iord $r11 I[$r10]\n"
"      and $r11 4\n"
"      bra nz #leaveloop\n"
"\n"
"   mov $r10 0x26f0\n"
"   call #mmrd\n"
"   mov b32 $r10 $r11\n"
"   bset $r11 0\n"
"   mov $r10 0x26f0\n"
"   call #mmwrs\n"
"\n"
"   mov $r10 0x1620\n"
"   call #mmrd\n"
"   mov b32 $r0 $r10\n"
"   bset $r0 0\n"
"   mov b32 $r11 $r0\n"
"   mov $r10 0x1620\n"
"   call #mmwrs\n"
"   mov $r12 0xaa2\n"
"   mov $r10 0x1620\n"
"   or $r11 $r0 $r12\n"
"   call #mmwrs\n"
"   pop $r0\n"
"   ret\n"
"\n"
"main:\n";

static char post[] =
"   st b32 D[$r3] $r0\n"
"   add b32 $r3 $r3 4\n"
"   st b32 D[$r3] $r3\n"
"   dead:\n"
"      bra #dead\n";

static void record_op(unsigned op, unsigned len) {
#if !DEBUG_C
	printf("   mov $r12 0x%x\n", op);
	printf("   st b32 D[$r3] $r12\n");
	printf("   add b32 $r3 $r3 4\n");
#else
	printf("op %x with len %u\n", op, len);
#endif
}

static void prefn(const char *fn) {
#if !DEBUG_C
#else
	printf("\n%s(", fn);
#endif
}

static void postfn(const char *fn) {
#if !DEBUG_C
	printf("   call #%s\n", fn);
#else
	printf(");\n", fn);
#endif
}


static void out(char *reg, unsigned val) {
#if !DEBUG_C
	printf("   movw $%s 0x%x\n", reg, val & 0xffff);
	if (val >= 0x8000)
		printf("   sethi $%s %#x\n", reg, val & ~0xffff);
	printf("   st b32 D[$r3] $%s\n", reg);
	printf("   add b32 $r3 $r3 4\n");
#else
	printf("%s%#x", strcmp(reg, "r10") ? ", " : "", val);
#endif
}

static void saveret(void) {
#if !DEBUG_C
	printf("   st b32 D[$r3] $r10\n");
	printf("   add b32 $r3 $r3 4\n");
#endif
}

static void fn0(const char *fn) {
	prefn(fn);
	postfn(fn);
}

static void fn1(const char *fn, unsigned x) {
	prefn(fn);
	out("r10", x);
	postfn(fn);
}

static void fn2(const char *fn, unsigned x, unsigned y) {
	prefn(fn);
	out("r10", x);
	out("r11", y);
	postfn(fn);
}

static void fn4(const char *fn, unsigned x, unsigned y, unsigned z, unsigned w) {
	prefn(fn);
	out("r10", x);
	out("r11", y);
	out("r12", z);
	out("r13", w);
	postfn(fn);
}

int main(int argc, char **argv) {
	unsigned *d = data, *end = data + (sizeof(data)/sizeof(*data));
	unsigned i;
	printf("/* envyas -a -w -m fuc -V nva3 -o braaksel */\n");
	printf("%s\n", pre);
	while (d < end) {
		unsigned op = *d & 0xffff, len = *d >> 0x10;
		record_op(op, len);

		switch (op) {
			case 0:
			case 1:
				unk3ec[op] = d[1];
				break;

			case 0x2e:
				fn1("mmrd", 0);
				saveret();
			case 0x13: // Fallthrough
				fn1("sleep", d[1]);
				break;

			case 0x15:
				unk3ec[2] *= 2;
				fn4("wait_mask", unk3ec[1], d[1], unk3ec[0], d[2]);
				saveret();
				// unk3ec[2] |= success;
				break;

			case 0x16:
				printf("%s\n", post);
				return 0;

			case 0x20:
				assert(!d[2]);
#if !DISABLE_LOCK
				if (d[1])
					fn0("enter_lock");
				else
					fn0("leave_lock");
#endif
				method = d[1] ? "mmwr" : "mmwrs";
				break;

			case 0x21:
				for (i = 1; i < len; i += 2) {
					//printf("%s(0x%x, 0x%x)\n", len > 3 ? "mmwrs_group" : method, d[i], d[i+1]);
					fn2(len > 3 ? "mmwr" : method, d[i], d[i+1]);
				}
				unk3ec[0] = d[i-1];
				unk3ec[1] = d[i-2];
				break;

			case 0x34:
				//printf("OUT[%u] call_timer_read\n", d[1]);
				break;

			default: assert(op < 0x36 && !(op >= 0x10 && op <= 0x12));
				assert(!"valid but unhandled op");
				return -1;
		}
		d += len;
	}
	assert(0);
	return 1;
}

