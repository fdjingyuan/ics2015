#include "cpu/exec/template-start.h"

#define instr jmp


static void do_execute() {
	DATA_TYPE_S displacement = op_src->val;
	
	if(op_src->type == OP_TYPE_IMM)
	{
		if(DATA_BYTE == 2)
			cpu.eip=(cpu.eip+displacement) & 0x0000ffff;
		else
			cpu.eip = cpu.eip + displacement;
		print_asm("jmp %x",cpu.eip + 1 + DATA_BYTE);
	}
	else
	{
		cpu.eip = displacement - concat (decode_rm_,SUFFIX)(cpu.eip+1) - 1;
		print_asm_no_template1();
	}
}
make_instr_helper(i)
make_instr_helper(rm)


#include "cpu/exec/template-end.h"
