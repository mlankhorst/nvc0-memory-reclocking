#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include "nva.h"
#include "gen.h"

enum fuc_ops {
   fuc_ops_none = -1,
   fuc_ops_done,
   fuc_ops_mmwrs,
   fuc_ops_mmwr,
   fuc_ops_mmrd,
   fuc_ops_wait_mask,
   fuc_ops_sleep,
   fuc_ops_enter_lock,
   fuc_ops_leave_lock
};

#define DISABLE_LOCK 0

// Calling convention
// r9..r15 always trash
// r4..r8 untouched by mm* functions
// r0...r3 used by main

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

static enum fuc_ops method = fuc_ops_mmwrs;

static unsigned out[0x800];
static unsigned outpos;

static void emit(enum fuc_ops func, unsigned len_args, unsigned args[], unsigned saveret)
{
	unsigned j, oldpos = outpos;
	out[outpos++] = (8 + 4 * len_args) | (saveret << 31);
	out[outpos++] = nvc0_pdaemon_data[func];
	for (j = 0; j < len_args; ++j)
		out[outpos++] = args[j];
	assert(outpos == (oldpos + 2 + len_args));
}

unsigned unk3ec[2];

static void upload(unsigned cnum) {
	unsigned val = nva_rd32(cnum, 0x200);
	int i;
	val &= ~(1<<13);
	nva_wr32(cnum, 0x200, val);
	nva_wr32(cnum, 0x200, val | (1<<13));
	nva_wr32(cnum, 0x10a47c, 0x7013452f);
	nva_wr32(cnum, 0x10a180, 0x01000000);
	for (i = 0; i < sizeof(nvc0_pdaemon_code)/sizeof(*nvc0_pdaemon_code); ++i) {
		if (i % 64 == 0)
			nva_wr32(cnum, 0x10a188, i >> 6);
		nva_wr32(cnum, 0x10a184, nvc0_pdaemon_code[i]);
	}

	nva_wr32(cnum,0x10a1c0, 0x01000800);
	for (i = 0; i < outpos; ++i)
		nva_wr32(cnum, 0x10a1c4, out[i]);

	nva_wr32(cnum,0x10a1c0, 0x010055c0);
	for (i = 0; i < 0x440; i += 4)
		nva_wr32(cnum, 0x10a1c4, 0);
	nva_wr32(cnum, 0x10a104, 0);
	nva_wr32(cnum, 0x10a10c, 0);
	nva_wr32(cnum, 0x10a100, 2);
}

int main(int argc, char **argv) {
	unsigned cnum = 0;
	if (nva_init()) {
		fprintf (stderr, "PCI init failure!\n");
		return 1;
	}
	if (cnum >= nva_cardsnum) {
		fprintf (stderr, "No cards found.\n");
		return 1;
	}
	unsigned *d = data, *end = data + (sizeof(data)/sizeof(*data));
	unsigned i;
	while (d < end) {
		unsigned op = *d & 0xffff, len = *d >> 0x10;
		enum fuc_ops fuc_op;
		unsigned args[4];

		switch (op) {
			case 0:
			case 1:
				unk3ec[op] = d[1];
				break;

			case 0x2e:
				args[0] = 0;
				emit(fuc_ops_mmrd, 1, args, 1);
			case 0x13: // Fallthrough
				args[0] = d[1];
				emit(fuc_ops_sleep, 1, args, 0);
				break;

			case 0x15:
				args[0] = unk3ec[1];
				args[1] = d[1];
				args[2] = unk3ec[0];
				args[3] = d[2];
				emit(fuc_ops_wait_mask, 4, args, 1);
				break;

			case 0x16:
				args[0] = d[1];
				emit(fuc_ops_done, 1, args, 0);
				break;

			case 0x20:
				assert(!d[2]);
#if !DISABLE_LOCK
				if (d[1])
					fuc_op = fuc_ops_enter_lock;
				else
					fuc_op = fuc_ops_leave_lock;
				emit(fuc_op, 0, args, 0);
#endif
				method = d[1] ? fuc_ops_mmwr : fuc_ops_mmwrs;
				break;

			case 0x21:
				fuc_op = len > 3 ? fuc_ops_mmwr : method;
				for (i = 1; i < len; i += 2) {
					args[0] = d[i];
					args[1] = d[i+1];
					emit(fuc_op, 2, args, 0);
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
	assert(out[outpos - 2] == nvc0_pdaemon_data[fuc_ops_done]);

	upload(cnum);
	return 0;
}

