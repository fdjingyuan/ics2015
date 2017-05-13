#include "cpu/exec/template-start.h"

#define instr sub

static void do_execute(){
	DATA_TYPE result = op_dest->val - op_src->val;

	int len=(DATA_BYTE<<3)-1;
	int sign1=!!(op_dest->val >>len);
	int sign2=!!(op_src->val >>len);
	int count=0;
	int tmp=1;
	while(len>=0)
	{
		count+=!!(tmp&result);
		tmp=tmp<<1;
		len=len-1;
	}

	//CF=cout^cin or Co^Sub
	cpu.CF=op_dest->val < op_src->val;
	//PF=1 when the count of 1 is even, PF=0 when the count of 1 is odd	
	cpu.PF=!(count&0x1);
	//ZF=1 when result is 0
	cpu.ZF=!result;
	//SF=1 when the sign bit of result is 1
	cpu.SF=!!(result>>len);
	//OF=1 when the sign bit of dest and src are different and result's sign bit==src's sign bit
	cpu.OF=(sign1^sign2)&&(sign2==cpu.SF);
	
	OPERAND_W(op_dest,result);
	print_asm_template2();
}
#if DATA_BYTE ==2 || DATA_BYTE ==4
make_instr_helper(si2rm)
#endif

make_instr_helper(i2a);
make_instr_helper(i2rm);
make_instr_helper(r2rm);
make_instr_helper(rm2r);

#include "cpu/exec/template-end.h"



