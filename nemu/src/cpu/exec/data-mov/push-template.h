#include "cpu/exec/template-start.h"

#define instr push

static void do_execute() {
	//StackAddrSize=32

	//ESP=ESP-2 or 4
	REG(R_ESP)-=DATA_BYTE;
	//(ESP)=(SOURCE)
	MEM_W(REG(R_ESP),op_src->val);
	print_asm_template1();
}

make_instr_helper(r)


#include "cpu/exec/template-end.h"
