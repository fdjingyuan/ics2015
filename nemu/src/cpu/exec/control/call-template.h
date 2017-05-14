#include "cpu/exec/template-start.h"

#define instr call

static void do_execute(){

	DATA_TYPE_S rel=op_src->val;

	if (DATA_BYTE == 2){
		//PUSH(IP)
		REG(R_ESP) -= 2;
		MEM_W(REG(R_ESP),cpu.eip);
		//update eip
		cpu.eip = (cpu.eip +rel) & 0x0000ffff;
	}

	else{
		//PUSH(EIP)
		REG(R_ESP) -= 4;
		MEM_W(REG(R_ESP),cpu.eip);
		cpu.eip = cpu.eip +rel;
	}
		
	print_asm_template1();
}

make_instr_helper(i);

#include "cpu/exec/template-end.h"
