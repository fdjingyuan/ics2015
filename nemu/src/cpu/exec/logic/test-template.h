#include "cpu/exec/template-start.h"

#define instr test

static void do_execute(){
	DATA_TYPE result = op_dest->val & op_src->val;
	int len=(DATA_BYTE<<3)-1;
	int count=0;
	int tmp=1;
	while(len>=0)
	{
		count+=!!(tmp&result);
		tmp=tmp<<1;
		len=len-1;
	}

	cpu.CF=0;
	//PF=1 when the count of 1 is even, PF=0 when the count of 1 is odd	
	cpu.PF=!(count&0x1);
	//ZF=1 when result is 0
	cpu.ZF=!result;
	//SF=1 when the sign bit of result is 1
	cpu.SF=!!(result>>len);
	cpu.OF=0;
	
	//OPERAND_W(op_dest,result);
	print_asm_template2();
}

make_instr_helper(i2a)
make_instr_helper(i2rm)
make_instr_helper(r2rm)

#include "cpu/exec/template-end.h"
