#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include "nva.h"
#include "in.fuc.h"

enum fuc_ops {
   fuc_ops_none = -1,
   fuc_ops_done,
   fuc_ops_mmwrs,
   fuc_ops_mmwr,
   fuc_ops_mmrd,
   fuc_ops_wait_mask_ext,
   fuc_ops_wait_mask_iord,
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
#include SCRIPT
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

			case 0x14: {
				unsigned tab[] = { 1, 4, 8, 0x20, 0x10, 0x40 };
				args[0] = 0x7c4 * 0x40;
				if (d[1] / 2 < sizeof(tab)/sizeof(*tab))
					args[1] = tab[d[1] / 2];
				else {
					args[1] = tab[0];
					assert(0);
				}
				args[2] = (d[1] & 1) ? args[1] : 0;
				args[3] = d[2];
				emit(fuc_ops_wait_mask_iord, 4, args, 1);
				break;
			}

			case 0x15:
				args[0] = unk3ec[1];
				args[1] = d[1];
				args[2] = unk3ec[0];
				args[3] = d[2];
				emit(fuc_ops_wait_mask_ext, 4, args, 1);
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
					assert(method == fuc_ops_mmwr || method == fuc_ops_mmwrs);
					emit(fuc_op, 2, args, 0);
				}
				unk3ec[0] = d[i-1];
				unk3ec[1] = d[i-2];
				break;

			case 0x34:
				//printf("OUT[%u] call_timer_read\n", d[1]);
				break;

			default: assert(op < 0x36 && !(op >= 0x10 && op <= 0x12));
				fprintf(stderr, "Op %02x\n", op);
				assert(!"valid but unhandled op");
				return -1;
		}
		d += len;
	}
	assert(out[outpos - 2] == nvc0_pdaemon_data[fuc_ops_done]);

	upload(cnum);
	return 0;
}

